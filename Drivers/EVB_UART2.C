/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           EVB_UART2.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        建立在UART2上的EVB通讯协议                     
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

volatile unsigned int  EVBCONVERSATIONWAITACKTIMEOUT;	//EVB通讯接收ACK超时时间
volatile unsigned int  UART3RECVACKMSGTIMEOUT;			//接收ACK数据包超时时间
volatile unsigned char EvbAckFromDevice[8];				//EVB ACK数据缓冲
/*********************************************************************************************************
** Function name:     	Uart3RecvEvbAckFromDevice
** Descriptions:	    EVB通讯，接收ACK包
** input parameters:    无
** output parameters:   无
** Returned value:      1：接收成功；0：接收失败
*********************************************************************************************************/
unsigned char Uart2RecvEvbAckFromDevice(void)
{
	unsigned char RecvData,AckIndex;
	if(Uart2BuffIsNotEmpty() == 1)
	{
		RecvData = Uart2GetCh();
		if((RecvData == 0xFD)||(RecvData == 0xEE))
		{
			AckIndex = 0;
			EvbAckFromDevice[AckIndex++] = RecvData;	
			UART3RECVACKMSGTIMEOUT = 20; 				//接收剩余字节超时时间，100ms
			while(UART3RECVACKMSGTIMEOUT)
			{
				if(Uart2BuffIsNotEmpty()==1)
				{
					EvbAckFromDevice[AckIndex++] = Uart2GetCh();
					if((AckIndex == 2)&&(EvbAckFromDevice[0]==0xfd)/*AckIndex == 2*/)
					{
						if(EvbAckFromDevice[1] >0x08)
							break;
					}
					if(RecvData == 0xfd)
					{
						if(AckIndex == EvbAckFromDevice[1])	//处理6/8字节的ACK
						{
							//Trace("Recv123=%02x %02x %02x %02x %02x %02x %02x %02x\r\n",EvbAckFromDevice[0],EvbAckFromDevice[1],EvbAckFromDevice[2],EvbAckFromDevice[3],EvbAckFromDevice[4],EvbAckFromDevice[5],EvbAckFromDevice[6],EvbAckFromDevice[7]);
							if(EvbAckFromDevice[AckIndex-1] == XorCheck((unsigned char *)EvbAckFromDevice,(EvbAckFromDevice[1]-1)))
								return 1;
							else
								break;
						}
					}
					else
					{
						if(AckIndex == 8)	//处理6/8字节的ACK
						{
							if(EvbAckFromDevice[AckIndex-1] == XorCheck((unsigned char *)EvbAckFromDevice,7))
								return 1;
							else
								break;
						}	
					}
				}
			}
		}
		return 0;
	}
	return 0;
}
/*********************************************************************************************************
** Function name:     	EvbConversation
** Descriptions:	    EVB通讯
** input parameters:    Chl:选择通道,1-Hopper操作，2-货道操作，3-ACDC操作；Head:包头；Sn：序列号；Type:消息类别；Addr:设备地址;Data：数据；
** output parameters:   *ACK：应答包
** Returned value:      1：收到应答；0：未收到应答即通讯失败
*********************************************************************************************************/
uint8_t EvbConversation(uint8_t Chl,uint8_t Head,uint8_t Sn,uint8_t Type,uint8_t Addr,uint16_t Data,uint8_t *Ack)
{
	unsigned char i,EvbSendBuff[8];

	switch(Chl)
	{
		case 1:
			SetUart2Evb1Mode();
			break;
		case 2:
			SetUart2Evb2Mode();
			break;
		case 3:
			SetUart2Evb2Mode();
			break;
		case 4:
			SetUart2Evb1Mode();
			break;
		default:
			SetUart2Evb1Mode();
			break;
	}
	SetUart2ParityMode(PARITY_DIS);
	OSTimeDly(3);
	ClrUart2Buff();
	//i = Chl;//目前未用，防警告
	EvbSendBuff[0] = Head;
	EvbSendBuff[1] = 0x08;
	if((Chl==2)&&(Type==0x71))
		Sn = 0x00;
	EvbSendBuff[2] = Sn;
	EvbSendBuff[3] = Type;
	EvbSendBuff[4] = Addr;
	EvbSendBuff[5] = (unsigned char)Data;//0x08;
	EvbSendBuff[6] = (unsigned char)(Data>>8);//0x00;	
	EvbSendBuff[7] = XorCheck(EvbSendBuff,7);
	Uart2PutStr(EvbSendBuff,8);
	TraceChange("Send=%02x %02x %02x %02x %02x %02x %02x %02x\r\n",EvbSendBuff[0],EvbSendBuff[1],EvbSendBuff[2],EvbSendBuff[3],EvbSendBuff[4],EvbSendBuff[5],EvbSendBuff[6],EvbSendBuff[7]);
	TraceChannel("Send=%02x %02x %02x %02x %02x %02x %02x %02x\r\n",EvbSendBuff[0],EvbSendBuff[1],EvbSendBuff[2],EvbSendBuff[3],EvbSendBuff[4],EvbSendBuff[5],EvbSendBuff[6],EvbSendBuff[7]);
	OSTimeDly(3);
	EVBCONVERSATIONWAITACKTIMEOUT = 100;
	while(EVBCONVERSATIONWAITACKTIMEOUT)			//1000ms内收到ACK,否则超时
	{
		if(Uart2RecvEvbAckFromDevice() == 1)
		{
			TraceChange("Recv=%02x %02x %02x %02x %02x %02x %02x %02x\r\n",EvbAckFromDevice[0],EvbAckFromDevice[1],EvbAckFromDevice[2],EvbAckFromDevice[3],EvbAckFromDevice[4],EvbAckFromDevice[5],EvbAckFromDevice[6],EvbAckFromDevice[7]);
			TraceChannel("Recv=%02x %02x %02x %02x %02x %02x %02x %02x\r\n",EvbAckFromDevice[0],EvbAckFromDevice[1],EvbAckFromDevice[2],EvbAckFromDevice[3],EvbAckFromDevice[4],EvbAckFromDevice[5],EvbAckFromDevice[6],EvbAckFromDevice[7]);
			if((EvbSendBuff[3]==0x70)&&(Chl==2 || Chl == 4))
			{
				EVBCONVERSATIONWAITACKTIMEOUT = 1500;
				while(EVBCONVERSATIONWAITACKTIMEOUT)
				{
					if(Uart2RecvEvbAckFromDevice() == 1)
					{
						for(i=0;i<EvbAckFromDevice[1];i++)
						{
							*Ack++ = EvbAckFromDevice[i];
						}
						SetUart2MdbMode();
						return 1;
					}
					OSTimeDly(5);//add by yoc newAisle
				}	
			}
			else
			{
				for(i=0;i<EvbAckFromDevice[1];i++)
				{
					*Ack++ = EvbAckFromDevice[i];
				}
			}
			SetUart2MdbMode();
			return 1;
		}

		OSTimeDly(5);//add by yoc newAisle
	}
	//Trace("Fail..\r\n");
	SetUart2MdbMode();
	return 0;
}
/**************************************End Of File*******************************************************/
