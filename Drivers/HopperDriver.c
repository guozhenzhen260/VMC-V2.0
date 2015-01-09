/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           HopperAccepter.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        Hopper操作功能接口                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "HopperDriver.h"
#include "..\config.h"

//定义三个地址Hopper的结构体
struct EvbHopper stEvbHp[3];



/*********************************************************************************************************
** Function name:     	HopperAccepter_Handle
** Descriptions:	    EVB设备操作控制函数
** input parameters:    DeviceCmdType:需要操作控制的设备类型
						Address:设备的硬件物理地址
						nbuf:操作内容，如Hopper出币操作的出币数量
** output parameters:   无
** Returned value:      1：收到应答；0：未收到应答即通讯失败
*********************************************************************************************************/
unsigned char HopperAccepter_Handle(unsigned char DeviceCmdType,unsigned char Address,unsigned char *nbuf)
{
	uint8_t EvbAckMsg[36],i,j;
	uint8_t MsgHead,MsgType,MsgAdd;
	uint16_t MsgDate;
	uint8_t res,DevNum=0;
	static uint8_t sn=0;
	
	//Hopper查询操作指令	
	if(DeviceCmdType == EVBHANDLE_HPQUERY)
	{
		TraceChange("HpAdd=%d\r\n",Address);
		MsgHead = 0xED;
		MsgType = EVBHANDLE_HPQUERY;
		MsgAdd = Address;
		MsgDate = 0;	
	}
	else
	//Hopper出币操作指令
	if(DeviceCmdType == EVBHANDLE_HPOUTPUT)
	{
		sn++;
		MsgHead = 0xED;
		MsgType = EVBHANDLE_HPOUTPUT;
		MsgAdd = Address;
		MsgDate = (nbuf[0]<<8)|(nbuf[1]&0x00ff);	
	}
	for(j=0;j<5;j++)
	{
		//发送操作包，并判断应答包
		res = EvbConversation(1,MsgHead,sn,MsgType,MsgAdd,MsgDate,EvbAckMsg);
		if(res == 1)
		{
			if(DeviceCmdType == EVBHANDLE_HPOUTPUT && EvbAckMsg[1] == 0x06)//出币命令且返回ACK  6字节
			{
				//if(EvbAckMsg[3] & 0x01)//重包
				//{
				//	sn++;
				//	continue;
				//}
				return 1;
			}
			else if(DeviceCmdType == EVBHANDLE_HPQUERY && EvbAckMsg[1] == 0x08)//查询命令并返回 状态结果 8字节
			{
				for(i=0;i<3;i++) 
				{
					if(stEvbHp[i].Add == EvbAckMsg[4])
					{
						DevNum = i;
						break;
					}
				}
				if(i>=3)
					return 0;
				TraceChange("1_EvbAckMsg=%x state=%d\r\n",EvbAckMsg[3],stEvbHp[DevNum].State);
				if(EvbAckMsg[3]==0x00)
					stEvbHp[DevNum].State = 0x00;
				else
				if((EvbAckMsg[3]==0x20)||(EvbAckMsg[3]==0x29))
					stEvbHp[DevNum].State = 0x02;
				else
				if((EvbAckMsg[3]==0x11)||(EvbAckMsg[3]==0x39)||(EvbAckMsg[3]==0x28)||(EvbAckMsg[3]==0x41))
					stEvbHp[DevNum].State = 0x03;	
				stEvbHp[DevNum].OutPutCount = INTEG16(EvbAckMsg[6],EvbAckMsg[5]);
				TraceChange("2_EvbAckMsg=%x state=%d\r\n",EvbAckMsg[3],stEvbHp[DevNum].State);	
				stEvbHp[DevNum].StateChk=EvbAckMsg[3];
				return 1;
	
			}
	
		}
		else
		{
			for(i=0;i<3;i++)
			{
				if(stEvbHp[i].Add == MsgAdd)
				{
					stEvbHp[i].State = 0x03;
					break;
				}
			}		
		}
	}
	return 0;
}



/*********************************************************************************************************
** Function name:     	HopperAccepter_CheckHPOk
** Descriptions:	    检测连接到机器上得Hopper设备是否可用,只启动时候用
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void HopperAccepter_CheckHPOk(void)
{
	uint8_t j,i,res;

	//2.检测当前几个Hopper可用,不可用则将该Hp的故障位标志位1
	for(j=0;j<3;j++)
	{
		for(i=0;i<3;i++)
		{
			res = HopperAccepter_Handle(EVBHANDLE_HPQUERY,stEvbHp[j].Add,0x00);	
			if(res == 1)
			{
				stEvbHp[j].DevBadFlag = 0;
				break;	
			}
			else
			{
				stEvbHp[j].DevBadFlag += 1;	
			}
		}
	}
}


/*********************************************************************************************************
** Function name:     	HopperAccepter_HpOutHandle
** Descriptions:	    Hopper出币操作函数
** input parameters:    Count：出币数量
						HpNum:Hp编号：1,00地址Hp;2,01地址Hp;3,10地址Hp
** output parameters:   无
** Returned value:      1：成功出币；0：出币失败
*********************************************************************************************************/
uint8_t HopperAccepter_HpOutHandle(uint8_t Count,uint8_t HpNum,uint8_t *HpOutNum)
{
	uint8_t res;
	uint8_t buf[2]={0};

	if(Count<=0)
		return 0;
	buf[0] = Count/256;
	buf[1] = Count%256;
	//发送出币指令
	res = HopperAccepter_Handle(EVBHANDLE_HPOUTPUT,stEvbHp[HpNum-1].Add,buf);
	if(res==1)
	{
		//等待出币完成返回应答包
		Timer.PayoutTimer = 40+Count/2;
		while(Timer.PayoutTimer)
		{
			//查询是否出币完成
			res = HopperAccepter_Handle(EVBHANDLE_HPQUERY,stEvbHp[HpNum-1].Add,0);
			Trace("\r\n HpOutNum=%d,res=%d",stEvbHp[HpNum-1].OutPutCount,res);
			if(res==1)
			{
				*HpOutNum = stEvbHp[HpNum-1].OutPutCount;
				return 1;
			}
		}
		return 0;	
	}
	return 0;
}

/*********************************************************************************************************
** Function name:     	HopperAccepter_GetState
** Descriptions:	    Hopper设备更改设备状态函数,空闲时，或找零后实时监测
** input parameters:    DevType:需要操作控制的设备类型
						--	EVBDEV_HP1:物理地址为00的Hopper
							EVBDEV_HP2:物理地址为01的Hopper
							EVBDEV_HP3:物理地址为10的Hopper
** output parameters:   无
** Returned value:      1：收到应答；0：未收到应答即通讯失败
*********************************************************************************************************/
void HopperAccepter_GetState(uint8_t DevType)
{
	if(DevType == EVBDEV_HP1)
	{
		TraceChange("Hp1State=%d\r\n",stEvbHp[0].State);
		if((stEvbHp[0].Price !=0)&&(stEvbHp[0].State != 0x03))
			HopperAccepter_Handle(EVBHANDLE_HPQUERY,0x00,0x00);	
	}
	if(DevType == EVBDEV_HP2)
	{
		TraceChange("Hp2State=%d\r\n",stEvbHp[1].State);
		if((stEvbHp[1].Price !=0)&&(stEvbHp[1].State != 0x03))
			HopperAccepter_Handle(EVBHANDLE_HPQUERY,0x01,0x00);	
	}
	if(DevType == EVBDEV_HP3)
	{
		TraceChange("Hp3State=%d\r\n",stEvbHp[2].State);
		if((stEvbHp[2].Price !=0)&&(stEvbHp[2].State != 0x03))
			HopperAccepter_Handle(EVBHANDLE_HPQUERY,0x02,0x00);	
	}	
}






