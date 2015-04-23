/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           MDB_UART2.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        建立在UART2上的MDB通讯协议                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-05
** Version:             V2.0
** Descriptions:        The original version     
********************************************************************************************************/
#include "..\config.h"

volatile unsigned int MDBCONVERSATIONWAITACKTIMEOUT;//MDB通讯超时时间

/*********************************************************************************************************
** Function name:     	MdbBusHardwareReset
** Descriptions:	    MDB总线复位：将MDB总线TXD线拉低200ms;注意该函数为硬件复位，总线上所有设备都复位
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MdbBusHardwareReset(void)
{
	SetUart2MdbMode();
	PINSEL0 &= (~(0x03<<20));	//P0.10改为GPIO模式
	FIO0DIR |= (1ul<<10);		//设置为输出
	FIO0CLR |= (1ul<<10);		//输出0
	OSTimeDly(40);				//延时200ms
	PINSEL0 |= (0x01<<20);		//设置为TXD功能
	InitUart2();
	return;
}
/*********************************************************************************************************
** Function name:     	MdbConversation
** Descriptions:	    MDB设备通讯
** input parameters:    Dev:设备号(高5位地址及低三位命令)；
**						*SeDat:要发送的数据；
**						SenDatLen：要发送数据的长度
** output parameters:   *RevDat:接收到的数据；RevLen:接收到数据的长度；
** Returned value:      1：通讯成功：0；通讯失败；
*********************************************************************************************************/
uint8_t MdbConversation(uint8_t Dev,uint8_t *SeDat,uint8_t SenDatLen,uint8_t *RevDat,uint8_t *RevLen)
{
	unsigned char i,check = 0,err = 0;
	unsigned char Temp;
	ClrUart2Buff();
	OSIntEnter();
	MdbPutChr(Dev,MDB_ADD);					//送出MDB地址及命令
	check = Dev;
	//Trace("\r\n%02x=",check);	
	for(i=0;i<SenDatLen;i++)				//送出数据
	{
		MdbPutChr(SeDat[i],MDB_DAT);
		check += SeDat[i];
		//Trace(" %02x ",SeDat[i]);
	}
	MdbPutChr(check,MDB_DAT);				//送出校验和
//	Trace("\r\nMDBSEND=[%02x,",Dev);
//	for(i=0;i<SenDatLen;i++)				//送出数据
//	{
//		Trace(" %02x ",SeDat[i]);
//	}
//	Trace(" %02x]\r\n",check);
	OSIntExit();
	OSTimeDly(3);
	MDBCONVERSATIONWAITACKTIMEOUT = 200;	//进入ACK等待，超时200ms
	while(MDBCONVERSATIONWAITACKTIMEOUT)
	{
		Temp = GetMdbStatus();
		if(Temp > MDB_RCV_DATA)
		{
			switch(Temp)
			{
				case MDB_RCV_OK		: 
									  //Trace("\r\n R- ");	
									  *RevLen = Uart2GetStr(RevDat,MDB_MAX_BLOCK_SIZE);
									  //Trace(" [%02x]",*RevLen);
//									  Trace("MDBREC=");
//									  for(i=0;i<*RevLen;i++)				//送出数据
//									  {
//											Trace(" %02x ",RevDat[i]);
//									  }
									  err = 1;
									  break;	
				case MDB_OUT_RANGE	: 
					                  //Trace("\r\n R-[out] ");
									  err = 2;
									  break;
				case MDB_CHKSUM_ERR	: 
					                  //Trace("\r\n R-[Err] ");
					                  err = 3;
									  break;
				default:
									  //Trace("\r\n R-[Unknow] ");
									  break;
			}
			break;	
		}	
		OSTimeDly(3);
	}
	ClrUart2Buff();
	return err;	
}
/**************************************End Of File*******************************************************/
