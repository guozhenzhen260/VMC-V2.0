/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           Uart2.C
** Last modified Date:  2013-01-08
** Last Version:         
** Descriptions:        Uart2驱动程序                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-08
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

volatile unsigned char  Uart2_RdBuf[UART2_BUFLEN];
volatile unsigned char  Uart2_RIndex;
volatile unsigned char  Uart2_FIndex;
volatile unsigned char  Uart2_Mode;
volatile unsigned char  MdbStatus = MDB_RCV_START;

/*********************************************************************************************************
** Function name:     	Uart2_Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为9600
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void InitUart2(void)
{	
	//evbpoweron
	FIO2DIR |= 1UL<<9;
	FIO2CLR |= 1UL<<9;
	//uart  set
	PCONP = PCONP | (1<<24);
    U2LCR  = 0x80;                         
    U2DLM  = 0;
    U2DLL  = 125;
	U2FDR  = 0x41; 
    U2LCR  = 0x03;                      
    U2FCR  = 0x07;                      
    U2IER  = 0x05;
	ClrUart2Buff();
	memset((void *)Uart2_RdBuf,0x00,UART2_BUFLEN);                       
    zyIsrSet(NVIC_UART2,(unsigned long)Uart2IsrHandler,PRIO_FIVE);                                 
}
/*********************************************************************************************************
** Function name:     	Uart2_ClrBuf
** Descriptions:	    清除串口2接收缓冲区
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClrUart2Buff(void) 
{
	Uart2_FIndex = 0;
	Uart2_RIndex = 0;
	SetMdbStartStatus();//设置开始状态
}

/*********************************************************************************************************
** Function name:	    Uart2BuffIsNotEmpty
** Descriptions:	    判断缓冲区是否非空
** input parameters:    无
** output parameters:   无
** Returned value:      1：非空；0：空
*********************************************************************************************************/
unsigned char Uart2BuffIsNotEmpty(void)
{
	if(Uart2_FIndex==Uart2_RIndex) 
		return 0;
	else
		return 1;
}


/*********************************************************************************************************
** Function name:     	SetUart2ParityMode
** Descriptions:	    设置串口奇偶校验位
** input parameters:    mode:奇偶校验设置,即设置MDB协议的标志位
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart2ParityMode(unsigned char mode) 
{
	switch(mode) 
	{		
		case PARITY_ODD	: 	U2LCR = UART_LCR_PARITY_ODD|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_EVEN:	U2LCR = UART_LCR_PARITY_EVEN|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_1	:	U2LCR = UART_LCR_PARITY_F_1|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_0	:	U2LCR = UART_LCR_PARITY_F_0|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		default:		 	U2LCR = UART_LCR_WLEN8;
						 	break;
	}
}
/*********************************************************************************************************
** Function name:     	MdbPutChr
** Descriptions:	    向MDB总线上送出一个字节的数据
** input parameters:    dat--要送的数据；mode--发送方式：0为地址字节/1为数据字节;按MDB协议处理
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MdbPutChr(unsigned char dat,unsigned char mode)
{
	switch(mode) 
	{
		case MDB_DAT :	SetUart2ParityMode(PARITY_F_0);
						Uart2PutCh(dat);
						break;
		case MDB_ADD:	SetUart2ParityMode(PARITY_F_1);
						Uart2PutCh(dat);
						break;
		default	:		break;
	}
	while(!(U2LSR & UART_LSR_TEMT));
}
/*********************************************************************************************************
** Function name:     	Uart2PutCh
** Descriptions:	    向串口2发送一个字节
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2PutCh(unsigned char dat) 
{
	U2THR = dat;
	while(!(U2LSR & UART_LSR_THRE)); 
}

/*********************************************************************************************************
** Function name:     	Uart2_PutStr
** Descriptions:	    向串口2发送一个字符串
** input parameters:    *buf字符串；Lenth字符串长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2PutStr(unsigned char *buf,unsigned char lenth) 
{
	unsigned int i;

	OSSchedLock();
	for(i=0;i<lenth;i++)
	{
		Uart2PutCh(*buf++);
	}
	OSSchedUnlock();
}
/*********************************************************************************************************
** Function name:     	Uart2GetStr
** Descriptions:	    从串口2接收缓冲区读取一个字符串
** input parameters:    *buf字符串；Lenth字符串长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char Uart2GetStr(unsigned char *buf, unsigned char len) 
{
	unsigned char count,i;
	count = Uart2_RIndex - Uart2_FIndex;	
	if((len == 0) || (count == 0)) 
	    return 0;
	else if(len > count) 
	    len = count;
	for(i = 0; i < len; i++)	
	{
		//Trace("u=%d",Uart2_RdBuf[Uart2_FIndex]);
		//Trace(" %02x ",Uart2_RdBuf[Uart2_FIndex]);
	    *buf++ = Uart2_RdBuf[Uart2_FIndex++];
	}
	if(Uart2_FIndex == Uart2_RIndex) 
	{
		Uart2_FIndex = 0;
		Uart2_RIndex = 0;
	}
	return (len-1);
}

/*********************************************************************************************************
** Function name:	    Uart2GetCh
** Descriptions:	    从串口缓冲区读取一个字节，读之前需要判断缓冲区非空
** input parameters:    无
** output parameters:   无
** Returned value:      读取到的数据
*********************************************************************************************************/
unsigned char Uart2GetCh(void)
{
    unsigned char ch;
    ch = Uart2_RdBuf[Uart2_FIndex];
    Uart2_RdBuf[Uart2_FIndex] = 0;
    Uart2_FIndex++;
    if(Uart2_FIndex >= UART2_BUFLEN)
		Uart2_FIndex = 0;
	return ch;
}

/*********************************************************************************************************
** Function name:     	GetMdbStatus
** Descriptions:	    读取当前MDB接收状态
** input parameters:    无
** output parameters:   无
** Returned value:      当前MDB接收状态
*********************************************************************************************************/
unsigned char GetMdbStatus(void) 
{
	unsigned char status;
	status = MdbStatus;
	return status;
}
/*********************************************************************************************************
** Function name:     	SetMdbStartStatus
** Descriptions:	    设置MDB状态
** input parameters:    无
** output parameters:   MdbStatus
** Returned value:      无
*********************************************************************************************************/
void SetMdbStartStatus(void)
{
	MdbStatus = MDB_RCV_START;
}
/*********************************************************************************************************
** Function name:     	Uart2_SetMdbMode
** Descriptions:	    设置串口为MDB接收状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart2MdbMode(void) 
{
	//硬件设置
	FIO1DIR |= UART2MODESELECT0;
	FIO1CLR |= UART2MODESELECT0;
	FIO1DIR |= UART2MODESELECT1;
	FIO1CLR |= UART2MODESELECT1;
	//软件定义	
	Uart2_Mode = UART2_MDB_MODE;
}
/*********************************************************************************************************
** Function name:     	Uart2_SetEvbMode
** Descriptions:	    设置串口为EVB1模式
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart2Evb1Mode(void) 
{
	//硬件设置
	FIO1DIR |= UART2MODESELECT0;
	FIO1CLR |= UART2MODESELECT0;
	FIO1DIR |= UART2MODESELECT1;
	FIO1SET |= UART2MODESELECT1;
	//软件定义
	Uart2_Mode = UART2_EVB1_MODE;
}
/*********************************************************************************************************
** Function name:     	Uart2_SetEvbMode
** Descriptions:	    设置串口为EVB2模式
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart2Evb2Mode(void) 
{
	//硬件设置
	FIO1DIR |= UART2MODESELECT0;
	FIO1SET |= UART2MODESELECT0;
	FIO1DIR |= UART2MODESELECT1;
	FIO1SET |= UART2MODESELECT1;
	//软件定义
	Uart2_Mode = UART2_EVB1_MODE;
}
/*********************************************************************************************************
** Function name:     	Uart2_SetEvbMode
** Descriptions:	    设置串口为EVB3模式
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart2Evb3Mode(void) 
{
	//硬件设置
	FIO1DIR |= UART2MODESELECT0;
	FIO1SET |= UART2MODESELECT0;
	FIO1DIR |= UART2MODESELECT1;
	FIO1SET |= UART2MODESELECT1;
	//软件定义
	Uart2_Mode = UART2_EVB3_MODE;
}
/*********************************************************************************************************
** Function name:     	Uart2IsrHandler
** Descriptions:	    串口2中断处理函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2IsrHandler(void) 
{
	volatile unsigned char dummy;
	unsigned int intsrc, tmp, tmp1;
	unsigned char udata;
	static unsigned char checksum;	
	OSIntEnter();	
	intsrc = U2IIR;									//Determine the interrupt source 
	tmp = intsrc & UART_IIR_INTID_MASK;				//UART_IIR_INTID_MASK = 0x0000000E,U2IIR[3:1]为中断标识
	//U2IIR[3:1] = 011;RLS接收线状态，产生该中断为四个条件之一(OE,PE,FE,BI);需要通过查看LSR寄存区得到错误原因
	if(tmp == UART_IIR_INTID_RLS) 		
	{
		tmp1 = U2LSR;
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		if(tmp1 & UART_LSR_PE)//每次VMC发送完MDB数据后，Uart2接收第九位总是被强制为校验0模式，至此说明收到MDB数据结束标志 
		{
			udata = U2RBR & UART_RBR_MASKBIT;			//数据可用			
			if(Uart2_Mode == UART2_MDB_MODE) 
			{				
				if(MdbStatus == MDB_RCV_START) 			//若前状态为开始接收状态
				{
					Uart2_RdBuf[Uart2_RIndex++] = udata;
					MdbStatus = MDB_RCV_OK;				//状态转入接收完毕
					MdbPutChr(MDB_ACK,MDB_DAT);			//送出ACK
				} 
				else if(MdbStatus == MDB_RCV_DATA) 		//若前状态为接收数据状态
				{
					if(Uart2_RIndex < UART2_BUFLEN)		//缓冲区未满
					{
						Uart2_RdBuf[Uart2_RIndex++] = udata;
						if(udata == checksum) 			//判断校验值
						{
							MdbStatus = MDB_RCV_OK;
							MdbPutChr(MDB_ACK,MDB_DAT);
						} 
						else 
						{
							MdbStatus = MDB_CHKSUM_ERR;
							MdbPutChr(MDB_NAK,MDB_DAT);
						}
					} 
					else 
					{
						MdbStatus = MDB_OUT_RANGE;
						MdbPutChr(MDB_NAK,MDB_DAT);
					}
				}	
			}
		}
		else if(tmp1)							//其他错误引起的中断则忽略掉 
		{		
		    dummy = U2RBR & UART_RBR_MASKBIT;
		}
	}
	//U2IIR[3:1]=010接收数据可用 || U2IIR[3:1]=110字符超时指示	  
	if((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))  
	{	    
		if(Uart2_RIndex < UART2_BUFLEN)	
		{		    
			udata = U2RBR & UART_RBR_MASKBIT;
			Uart2_RdBuf[Uart2_RIndex++] = udata;
			if(Uart2_Mode == UART2_MDB_MODE)	//若是MDB模式 
			{
				if(MdbStatus == MDB_RCV_START) 
				{
					checksum = 0;
				}
				MdbStatus = MDB_RCV_DATA;
				checksum += udata;
			}
		} 
		else									//缓冲区满
		{
		    dummy = U2RBR & UART_RBR_MASKBIT;
			if(Uart2_Mode == UART2_MDB_MODE)	//若是MDB模式，送出NAK
			{
				MdbStatus = MDB_OUT_RANGE;
				MdbPutChr(MDB_NAK,MDB_DAT);
			}
		}
	}
    OSIntExit();
}
/**************************************End Of File*******************************************************/
