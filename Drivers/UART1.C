/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           Uart1.c
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        Uart1驱动程序                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-01-04
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

#define	UART1_BPS96			9600
#define UART1_BPS576		57600

#define	UART1BUFFERLEN		256


#pragma arm section zidata = "RAM2" rwdata = "RAM2"
unsigned char Uart1RevBuff[UART1BUFFERLEN];
#pragma arm section




volatile unsigned char Uart1RxdHead;
volatile unsigned char Uart1RxdTail;

/*********************************************************************************************************
** Function name:     	InitUart1
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为9600
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void InitUart1 (void)
{
    unsigned short int ulFdiv;
		
	
	PCONP = PCONP | (1<<4);   
    U1LCR  = 0x83;                                                  //允许设置波特率
    //为了SystemPara.threeSelectKey的判断
   // if( (SystemPara.threeSelectKey==1)||(SystemPara.XMTTemp==1)||(SystemPara.hefangGui==1)||(SystemPara.BillRecyclerType == FS_BILLRECYCLER) )
//		ulFdiv = (FPCLK / 16) / UART1_BPS96;
	//else
//		ulFdiv = (FPCLK / 16) / UART1_BPS576;                              //设置波特率
      //为了SystemPara.threeSelectKey的判断
    if( (SystemPara.UserSelectKeyBoard==SELECT_KEY)&&(SystemPara.threeSelectKey==0))	
		ulFdiv = (FPCLK / 16) / UART1_BPS576;                              //设置波特率
    else	
		ulFdiv = (FPCLK / 16) / UART1_BPS96;
    U1DLM  = ulFdiv / 256;
    U1DLL  = ulFdiv % 256; 
    U1LCR  = 0x03;                                                  //锁定波特率
    U1FCR  = 0x87;                                                  //使能FIFO，设置8个字节触发点
    U1IER  = 0x01;                                                  //使能接收中断
	Uart1RxdHead  = 0;
	Uart1RxdTail  = 0;
	memset((void *)Uart1RevBuff,0x00,UART1BUFFERLEN);				//初始化缓冲区
	zyIsrSet(NVIC_UART1,(unsigned long)Uart1IsrHandler,PRIO_ONE);	//使能接收中断
}
/*********************************************************************************************************
* Function Name:        Uart1IsrHandler
* Description:          Uart1 中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void Uart1IsrHandler (void)
{
	unsigned char Num;
	unsigned char rxd_head;
	unsigned char rxd_data;
	//Trace("\r\n 2.");
	OSIntEnter();      
    while((U1IIR & 0x01) == 0)                          			//判断是否有中断挂起
	{
        switch(U1IIR & 0x0E) 										//判断中断标志
		{                                         
            case 0x04 : for (Num = 0; Num < 8; Num++)				//接收数据中断
						{
		                	rxd_data = U1RBR;
							rxd_head = (Uart1RxdHead + 1);
					        if( rxd_head >= UART1BUFFERLEN ) 
					           	rxd_head = 0;
					        if( rxd_head != Uart1RxdTail) 
					        {
					           	Uart1RevBuff[Uart1RxdHead] = rxd_data;
					           	Uart1RxdHead = rxd_head;
								//Trace("%x ",rxd_data);
					        }
		                }
		                break;
            case 0x0C : while((U1LSR & 0x01) == 0x01)				//字符超时中断，判断数据是否接收完毕
						{                         
		                	rxd_data = U1RBR;
							rxd_head = (Uart1RxdHead + 1);
					        if( rxd_head >= UART1BUFFERLEN ) 
					           	rxd_head = 0;
					        if( rxd_head != Uart1RxdTail) 
					        {
					           	Uart1RevBuff[Uart1RxdHead] = rxd_data;
					           	Uart1RxdHead = rxd_head;
								//Trace("%x ",rxd_data);
					        }
		                }
		                break;
            default	: break;
        }
    }
    OSIntExit();
}
/*********************************************************************************************************
** Function name:	    Uart1PutChar
** Descriptions:	    向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    ch:要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart1PutChar(unsigned char ch)
{
    U1THR = ch;                                                      //写入数据
    while((U1LSR & 0x20) == 0);                                      //等待数据发送完毕
}

/*********************************************************************************************************
** Function name:	    Uart1PutStr
** Descriptions:	    向串口发送字符串
** input parameters:    Str:  要发送的字符串指针
**                      Len:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart1PutStr(unsigned char const *Str, unsigned int Len)
{
    unsigned int i;
	//Trace("\r\nsendBegin");
    for (i=0;i<Len;i++) 
	{
		//Trace("\r\n++*send=%x",*Str);
        Uart1PutChar(*Str++);
    }
}
/*********************************************************************************************************
** Function name:	    Uart1BuffIsNotEmpty
** Descriptions:	    判断缓冲区是否非空
** input parameters:    无
** output parameters:   无
** Returned value:      1：非空；0：空
*********************************************************************************************************/
unsigned char Uart1BuffIsNotEmpty(void)
{
	if(Uart1RxdHead==Uart1RxdTail) 
		return 0;
	else
		return 1;
}

/*****************************************************************************
** Function name:	Uart1_ClrBuf	
**
** Descriptions:	清除串口1缓存数据	
**						
**
** parameters:		无	
** Returned value:	无
** 
*****************************************************************************/
void Uart1_ClrBuf(void) 
{
	Uart1RxdHead = 0;
	Uart1RxdTail = 0;
	memset(Uart1RevBuff,0,sizeof(Uart1RevBuff));
}

/*********************************************************************************************************
** Function name:	    Uart1GetCh
** Descriptions:	    从串口缓冲区读取一个字节，读之前需要判断缓冲区非空
** input parameters:    无
** output parameters:   无
** Returned value:      读取到的数据
*********************************************************************************************************/
unsigned char Uart1GetCh(void)
{
    unsigned char ch;
    ch = Uart1RevBuff[Uart1RxdTail];
    Uart1RevBuff[Uart1RxdTail] = 0;
    Uart1RxdTail++;
    if(Uart1RxdTail >= UART1BUFFERLEN)
		Uart1RxdTail = 0;
	return ch;
}

/*****************************************************************************
** Function name:	Uart1_Read	
**
** Descriptions:	读取串口1缓存数据	
**						
**
** parameters:		buf:数据指针,lenth:数据长度	
** Returned value:	无
** 
*****************************************************************************/
uint8_t Uart1_Read(uint8_t *buf, uint8_t len) 
{
	uint8_t count,i,j;
	count=0;

	//Trace("\r\nREADBegin=%d\r\n",(Uart1RxdHead - Uart1RxdTail)); 
	while( Uart1BuffIsNotEmpty() )
	{		
		//Trace("\r\n 1.");
		j=Uart1RxdHead - Uart1RxdTail;
		//Trace("\r\n++RIndex=%d,FIndex=%d",Uart1RxdHead,Uart1RxdTail);
		count += j;
		for( i=0; i<j; i++ )
		//for( i=0; i<8; i++ )
		{
			*buf++ = Uart1GetCh();
			//Trace("\r\n++*buf=%x",*buf);
			//Trace("\r\n++*buf=%x",*(buf-1));
			//Trace("%x ",*(buf-1));			
		}		
	}
	Uart1_ClrBuf();	
	//Trace("\r\n++READ=End");
	return count;
	
}





	/*********************************************************************************************************
** Function name:     	SetUart3ParityMode
** Descriptions:	    设置串口奇偶校验位
** input parameters:    mode:奇偶校验设置,即设置MDB协议的标志位
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SetUart1ParityMode(unsigned char mode) 
{
	switch(mode) 
	{		
		case PARITY_ODD	: 	U1LCR = UART_LCR_PARITY_ODD|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_EVEN:	U1LCR = UART_LCR_PARITY_EVEN|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_1	:	U1LCR = UART_LCR_PARITY_F_1|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_0	:	U1LCR = UART_LCR_PARITY_F_0|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		default:		 	U1LCR = UART_LCR_WLEN8;
						 	break;
	}
}
/**************************************End Of File*******************************************************/
