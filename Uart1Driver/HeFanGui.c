/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           HeFanGui.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        用于盒饭柜底层驱动控制                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2014-01-20
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/

#include "..\config.h"


unsigned char HeFanGuiRecvAckBuf[128]={0};
extern volatile unsigned int  EVBCONVERSATIONWAITACKTIMEOUT;	//EVB通讯接收ACK超时时间
extern volatile unsigned int  UART3RECVACKMSGTIMEOUT;			//接收ACK数据包超时时间

/*********************************************************************************************************
** Function name:     	Uart3RecvEvbAckFromDevice
** Descriptions:	    EVB通讯，接收ACK包
** input parameters:    无
** output parameters:   无
** Returned value:      1：接收成功；0：接收失败
*********************************************************************************************************/
unsigned char HeFanGuiRecvAckDevice(void)
{
	unsigned char RecvData,AckIndex;
	unsigned short Crcdata = 0x0000;

	if(Uart1BuffIsNotEmpty() == 1)
	{
		RecvData = Uart1GetCh();
		Trace("RecvData=%x\r\n",RecvData);
		if((RecvData == 0xC8))
		{
			AckIndex = 0;
			HeFanGuiRecvAckBuf[AckIndex++] = RecvData;	
			UART3RECVACKMSGTIMEOUT = 500; 				//接收剩余字节超时时间，100ms
			while(UART3RECVACKMSGTIMEOUT)
			{
				if(Uart1BuffIsNotEmpty()==1)
				{
					HeFanGuiRecvAckBuf[AckIndex++] = Uart1GetCh();
					if(AckIndex == HeFanGuiRecvAckBuf[1]+2)	//处理6/8字节的ACK
					{
						Crcdata = CrcCheck(HeFanGuiRecvAckBuf,HeFanGuiRecvAckBuf[1]);
						Trace("CRC_DATA=%x %x %x %x\r\n",HeFanGuiRecvAckBuf[AckIndex-2],HeFanGuiRecvAckBuf[AckIndex-1],(unsigned char)(Crcdata>>8),(unsigned char)(Crcdata&0x00ff));
						if((HeFanGuiRecvAckBuf[AckIndex-2] == (unsigned char)(Crcdata>>8))&&((HeFanGuiRecvAckBuf[AckIndex-1] == (unsigned char)(Crcdata&0x00ff))))
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
unsigned char HeFanGuiDriver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char *Result)
{
	unsigned char res = 0,i;
	uint8_t ackdata[36],len=0;
	uint8_t Mdata[36] = {0x00};
	uint8_t DevAdd = Add;

	memset(Result,0x00,36);
	len = 1;
	switch(Cmd)
	{
		case HEFANGUI_KAIMEN:
			Mdata[0] = Add;
			res = HeFanGuiUart(Binnum,DevAdd,0x52,Mdata,len,ackdata);
			break;
		case HEFANGUI_CHAXUN:
			Mdata[0] = Binnum;
			res = HeFanGuiUart(Binnum,DevAdd,0x51,Mdata,len,ackdata);
			break;
		case HEFANGUI_JIAREKAI:
		case HEFANGUI_JIAREGUAN:
			if(Cmd == HEFANGUI_JIAREKAI)
				Mdata[0] = 0x01;
			else
				Mdata[1] = 0x00;
			res = HeFanGuiUart(Binnum,DevAdd,0x53,Mdata,len,ackdata);
			break;
		case HEFANGUI_ZHILENGKAI:
		case HEFANGUI_ZHILENGGUAN:
			if(Cmd == HEFANGUI_ZHILENGKAI)
				Mdata[0] = 0x01;
			else
				Mdata[1] = 0x00;
			res = HeFanGuiUart(Binnum,DevAdd,0x55,Mdata,len,ackdata);
			break;
		case HEFANGUI_ZHAOMINGKAI:
		case HEFANGUI_ZHAOMINGGUAN:
			if(Cmd == HEFANGUI_ZHAOMINGKAI)
				Mdata[0] = 0x01;
			else
				Mdata[1] = 0x00;
			res = HeFanGuiUart(Binnum,DevAdd,0x56,Mdata,len,ackdata);
			break;
		default:
			break;
	}
	if(res==1)
	{
		for(i=0;i<ackdata[1];i++)
			Result[i] = ackdata[i];	
		return 1;		
	}
	return 0;
}


unsigned char HeFanGuiUart(unsigned char Binnum,unsigned char BinAdd,unsigned char Cmd,
							unsigned char *Data,unsigned char len,unsigned char *Ack)
{
	unsigned char i,EvbSendBuff[128]={0},index=0;
	unsigned short CrcData = 0x0000;

	
	Uart1_ClrBuf();
	EvbSendBuff[index++] = 0xC7;	
	EvbSendBuff[index++] = len+5;
	EvbSendBuff[index++] = Binnum;
	EvbSendBuff[index++] = Cmd;
	EvbSendBuff[index++] = BinAdd;
	EvbSendBuff[index++] = BinAdd;
	for(i=0;i<len;i++)
	{
		EvbSendBuff[index++] = Data[i];
	}
	EvbSendBuff[1] = index;
	CrcData	= CrcCheck(EvbSendBuff,EvbSendBuff[1]);
	EvbSendBuff[index++] = (unsigned char)(CrcData>>8);
	EvbSendBuff[index++] = (unsigned char)(CrcData&0x00ff);
	Uart1PutStr(EvbSendBuff,index);
	TraceChannel("\r\nHefanGui_Send=");
	for(i=0;i<len+7;i++)
		TraceChannel("%02x ",EvbSendBuff[i]);
	TraceChannel("\r\n");
	EVBCONVERSATIONWAITACKTIMEOUT = 500;
	while(EVBCONVERSATIONWAITACKTIMEOUT)			//1000ms内收到ACK,否则超时
	{
		if(HeFanGuiRecvAckDevice() == 1)
		{
			TraceChannel("\r\nHeFanGui_Recv=");
			for(i=0;i<HeFanGuiRecvAckBuf[1];i++)
				TraceChannel("%02x ",HeFanGuiRecvAckBuf[i]);
			TraceChannel("\r\n");
			for(i=0;i<HeFanGuiRecvAckBuf[1];i++)
			{
				*Ack++ = HeFanGuiRecvAckBuf[i];
			}
			
			return 1;
		}
		OSTimeDly(5);//add by yoc newAisle
	}
	
	return 0;
}






