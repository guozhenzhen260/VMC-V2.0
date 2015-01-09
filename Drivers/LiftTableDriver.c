/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LiftTableDriver.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        升降台的控制驱动                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-05
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"

STLIFTTABLE stLiftTable;

unsigned char GetAckFromDevice[128]={0};
extern volatile unsigned int  EVBCONVERSATIONWAITACKTIMEOUT;	//EVB通讯接收ACK超时时间
extern volatile unsigned int  UART3RECVACKMSGTIMEOUT;			//接收ACK数据包超时时间

/*********************************************************************************************************
** Function name:     	Uart3RecvEvbAckFromDevice
** Descriptions:	    EVB通讯，接收ACK包
** input parameters:    无
** output parameters:   无
** Returned value:      1：接收成功；0：接收失败
*********************************************************************************************************/
unsigned char RecvEvbAckFromDevice(void)
{
	unsigned char RecvData,AckIndex;
	unsigned short Crcdata = 0x0000;

	if(Uart2BuffIsNotEmpty() == 1)
	{
		RecvData = Uart2GetCh();
		if((RecvData == 0xC8))
		{
			AckIndex = 0;
			GetAckFromDevice[AckIndex++] = RecvData;	
			UART3RECVACKMSGTIMEOUT = 500; 				//接收剩余字节超时时间，100ms
			while(UART3RECVACKMSGTIMEOUT)
			{
				if(Uart2BuffIsNotEmpty()==1)
				{
					GetAckFromDevice[AckIndex++] = Uart2GetCh();
					if(AckIndex == GetAckFromDevice[1]+2)	//处理6/8字节的ACK
					{
						Crcdata = CrcCheck(GetAckFromDevice,GetAckFromDevice[1]);
						if((GetAckFromDevice[AckIndex-2] == (unsigned char)(Crcdata>>8))&&((GetAckFromDevice[AckIndex-1] == (unsigned char)(Crcdata&0x00ff))))
						{
							return 1;
						}
						else
							break;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}


//返回0:失败，1：成功，2：数据错误 3：无货 4：卡货 5：取货门未开启 6：货物未取走 7：未定义错误	0xff：通信失败
unsigned char LiftTableDriver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char Level,unsigned char *Result)
{
	unsigned char res = 0,i;
	uint8_t ackdata[36],len=0;
	uint8_t Mdata[36] = {0x00};
	uint8_t DevAdd = 0x40;

	if(Level!=0x00)
	{
		Mdata[0] = Level;//0x06-1;
		len++;
	}
	if(Add!=0x00)
	{
		Mdata[1] = Add;//53;;
		len++;
	}
	switch(Cmd)
	{
		case CHANNEL_OUTGOODS:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_VENDING_REQ,Mdata,len,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_VENDING_REQ,Mdata,len,ackdata);
			}
			break;
		case CHANNEL_CHECKSTATE:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_STATUS_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_STATUS_REQ,0,0,ackdata);
			}
			break;
		case CHANNEL_CHECKOUTRESULT:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_VENDINGRESULT_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_VENDINGRESULT_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_RESET:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_RESET_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_RESET_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_LIGHTOPEN:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_OPENLIGHT_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_OPENLIGHT_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_LIGHTCLOSE:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_CLOSELIGHT_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_CLOSELIGHT_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_OPENDOOR:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_OPENDOOR_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_OPENDOOR_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_CLOSEDOOR:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_CLOSEDOOR_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_CLOSEDOOR_REQ,0,0,ackdata);
			}
			break;
		case MACHINE_INFORMATION:
			if(Binnum == 1)
			{
				res = LiftTableUart(1,DevAdd,VMC_GETINFO_REQ,0,0,ackdata);
			}
			else
			if(Binnum == 2)
			{
				res = LiftTableUart(2,DevAdd,VMC_GETINFO_REQ,0,0,ackdata);
			}
			break;
		case VMC_CHUCHOU_REQ:
			Mdata[0] = Level;
			len = 1;
			LiftTableUart(Binnum,DevAdd,VMC_CHUCHOU_REQ,Mdata,len,ackdata);
			break;
	}
	if(res==1)
	{
		TraceChannel("\r\nackdata=");
		for(i=0;i<ackdata[1];i++)
		{
			Result[i] = ackdata[i];
			TraceChannel("%02x ",ackdata[i]);
		}
		TraceChannel("\r\n");
		if(ackdata[4] == GCC_RESET_ACK)
			return 1;
		else
		if(ackdata[4] == GCC_STATUS_ACK)
		{
			if(ackdata[5]==0x01)
				stLiftTable.Error_OVERALLUINT = 1;
			else
				stLiftTable.Error_OVERALLUINT = 0;
			if(ackdata[6]==0x01)
				stLiftTable.Error_LIFTER = 1;
			else
				stLiftTable.Error_LIFTER = 0;
			if(ackdata[7]==0x01)
				stLiftTable.Error_TAKEMOTTOR = 1;
			else
				stLiftTable.Error_TAKEMOTTOR = 0;
			if(ackdata[8]==0x01)
				stLiftTable.Error_USERDOOR = 1;
			else
				stLiftTable.Error_USERDOOR = 0;
			if(ackdata[9]==0x01)
				stLiftTable.Error_DISPENCE = 1;
			else
				stLiftTable.Error_DISPENCE = 0;
			if(ackdata[10]==0x01)
				stLiftTable.Error_FANGDAO = 1;
			else
				stLiftTable.Error_FANGDAO = 0;
			if(ackdata[11]==0x01)
				stLiftTable.Error_COLUMN = 1;
			else
				stLiftTable.Error_COLUMN = 0;
			return 1;
		}
		else
		if(ackdata[4] == GCC_VENDING_ACK)
			return 1;
		else
		if(ackdata[4] == GCC_VENDINGRESULT_ACK)
		{
			if((ackdata[5]==0x00)||(ackdata[5]==0x01))
			{
				if(ackdata[6]==0x01)
				{
					if(ackdata[7]==0x00)
						return 1;
					else
					if(ackdata[7]==0x01)
						return 2;
					else
					if(ackdata[7]==0x02)
						return 3;
					else
					if(ackdata[7]==0x03)
						return 4;
					else
					if(ackdata[7]==0x04)
						return 5;
					else
					if(ackdata[7]==0x05)
						return 6;
					else
					if(ackdata[7]==0x06)
						return 7;	
					else
					if(ackdata[7]==0x07)
						return 8;
					else
						return 9;	
				}
				else
					return 1;
			}
			return 1;
		}
		else
		if(ackdata[4] == GCC_OPENDOOR_ACK)
		{
			if(ackdata[5]==0x00)
				return 1;
			else
				return 0;
		}
		else
		if(ackdata[4] == GCC_CLOSEDOOR_ACK)
		{
			if(ackdata[5]==0x00)
				return 1;
			else
				return 0;
		}
		else
		if(ackdata[4] == GCC_OPENLIGHT_ACK)
		{
			if(ackdata[5]==0x00)
				return 1;
			else
				return 0;
		}
		else
		if(ackdata[4] == GCC_CLOSELIGHT_ACK)
		{
			if(ackdata[5]==0x00)
				return 1;
			else
				return 0;
		}
		else if(ackdata[4] == GCC_CHUCHOU_ACK)
		{
			return (ackdata[5] == 0 ?  1 : 0);
		}
		else
		if(ackdata[4] == GCC_GETINFO_ACK)
		{
			stLiftTable.Magic = ackdata[5];
			if((ackdata[6]&0x01)!=0x00)
				stLiftTable.SupportSpring = 1;
			else
				stLiftTable.SupportSpring = 0;	
			if((ackdata[6]&0x02)!=0x00)
				stLiftTable.ApronWheel = 1;	
			else
				stLiftTable.ApronWheel = 0;
			if((ackdata[6]&0x04)!=0x00)
				stLiftTable.Flat = 1;	
			else
				stLiftTable.Flat = 0;
			if((ackdata[6]&0x08)!=0x00)
				stLiftTable.Lifter = 1;	
			else
				stLiftTable.Lifter = 0;
			if((ackdata[6]&0x10)!=0x00)
				stLiftTable.Hot = 1;	
			else
				stLiftTable.Hot = 0;
			if((ackdata[6]&0x20)!=0x00)
				stLiftTable.Cold = 1;
			else
				stLiftTable.Cold = 0;
			if((ackdata[6]&0x40)!=0x00)
				stLiftTable.Light = 1;	
			else
				stLiftTable.Light = 0;
		}
		return 0;
	}
	else
		return 0xff;
}


unsigned char LiftTableUart(unsigned char Binnum,unsigned char BinAdd,unsigned char Cmd,
							unsigned char *Data,unsigned char len,unsigned char *Ack)
{
	unsigned char i,EvbSendBuff[512]={0};
	unsigned short CrcData = 0x0000;

	ClrUart2Buff();
	if(Binnum == 1)
	{
		SetUart2Evb2Mode();
	}
	else
	if(Binnum == 2)
	{
		SetUart2Evb1Mode();	
	}
	SetUart2ParityMode(PARITY_DIS);
	EvbSendBuff[0] = 0xC7;	
	EvbSendBuff[1] = len+5;
	EvbSendBuff[2] = BinAdd;
	EvbSendBuff[3] = 0x00;
	EvbSendBuff[4] = Cmd;//0x82;
	for(i=0;i<len;i++)
	{
		EvbSendBuff[5+i] = Data[i];
	}
	CrcData	= CrcCheck(EvbSendBuff,len+5);
	EvbSendBuff[len+5] = (unsigned char)(CrcData>>8);
	EvbSendBuff[len+5+1] = (unsigned char)(CrcData&0x00ff);
	Uart2PutStr(EvbSendBuff,len+7);
	TraceChannel("LiftTable[Send] ");
	for(i=0;i<len+7;i++)
		TraceChannel("%02x ",EvbSendBuff[i]);
	TraceChannel("\r\n");
	EVBCONVERSATIONWAITACKTIMEOUT = 1500;
	while(EVBCONVERSATIONWAITACKTIMEOUT)			//15s内收到ACK,否则超时
	{
		if(RecvEvbAckFromDevice() == 1)
		{
			TraceChannel("LiftTable[Recv] ");
			for(i=0;i<GetAckFromDevice[1];i++)
				TraceChannel("%02x ",GetAckFromDevice[i]);
			TraceChannel("\r\n");
			for(i=0;i<GetAckFromDevice[1];i++)
			{
				*Ack++ = GetAckFromDevice[i];
			}
			SetUart2MdbMode();
			return 1;
		}
		OSTimeDly(5);//add by yoc newAisle
	}
	SetUart2MdbMode();
	return 0;
}




