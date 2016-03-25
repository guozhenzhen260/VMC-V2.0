/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           timer.c
** Last modified Date:  2013-01-07
** Last Version:         
** Descriptions:        定时器驱动程序                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-01-07
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"
extern volatile unsigned int EVBCONVERSATIONWAITACKTIMEOUT;
extern volatile unsigned int MDBCONVERSATIONWAITACKTIMEOUT;
extern volatile unsigned int UART3RECVACKMSGTIMEOUT;
extern volatile unsigned int GOCCHECKTIMEOUT;
extern volatile unsigned int LIFTTABLETIMER;

volatile uint8_t Tick;
/*********************************************************************************************************
** Function name:     	InitTimer
** Descriptions:	    LPC1768内部4个定时器的初始化
** input parameters:    TimerNumb:定时器号(0~3);TimerInterval定时时间参数
** output parameters:   无
** Returned value:      无
** 定时时间计算方法：	定时时间(Second) = (TimerInterval*(PR+1))/FPCLK,其中FPCLK = 24000000,PR = 0;
						如定时10ms(0.01s),则TimerInterval = 240000
*********************************************************************************************************/
void InitTimer(unsigned char TimerNumb,unsigned int TimerInterval)
{
	switch(TimerNumb)
	{
		case 0:	T0TCR	= 0x02;
				T0IR	= 1;
				T0CTCR	= 0;
				T0TC	= 0;
				T0PR	= 0;
				T0MR0	= TimerInterval;
				T0MCR	= 0x03;
				zyIsrSet(NVIC_TIMER0,(unsigned long)TIMER0_IRQHandler,PRIO_TWO);
				T0TCR	= 0x01;
				Tick = 0;
				break;
		case 1:	T1TCR	= 0x02;
				T1IR	= 1;
				T1CTCR	= 0;
				T1TC	= 0;
				T1PR	= 0;
				T1MR0	= TimerInterval;
				T1MCR	= 0x03;
				zyIsrSet(NVIC_TIMER1,(unsigned long)TIMER1_IRQHandler,PRIO_ELEVEN);
				T1TCR	= 0x01;
				break;
		case 2:	T2TCR	= 0x02;
				T2IR	= 1;
				T2CTCR	= 0;
				T2TC	= 0;
				T2PR	= 0;
				T2MR0	= TimerInterval;
				T2MCR	= 0x03;
				zyIsrSet(NVIC_TIMER2,(unsigned long)TIMER2_IRQHandler,PRIO_TWELVE);
				T2TCR	= 0x01;
				break;
		case 3:	T3TCR	= 0x02;
				T3IR	= 1;
				T3CTCR	= 0;
				T3TC	= 0;
				T3PR	= 0;
				T3MR0	= TimerInterval;
				T3MCR	= 0x03;
				zyIsrSet(NVIC_TIMER3,(unsigned long)TIMER3_IRQHandler,PRIO_THIRTEEN);
				T3TCR	= 0x01;
				break;
		default:break;
	}
}
/*********************************************************************************************************
* Function Name:        TIMER0_IRQHandler
* Description:          定时器0中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void TIMER0_IRQHandler (void)
{  	
	OSIntEnter();
	T0IR = 1;
	WDT_Feed();
	//10ms级定时器
	if(EVBCONVERSATIONWAITACKTIMEOUT)
		EVBCONVERSATIONWAITACKTIMEOUT--;
	if(MDBCONVERSATIONWAITACKTIMEOUT)
		MDBCONVERSATIONWAITACKTIMEOUT--;
	if(UART3RECVACKMSGTIMEOUT)
		UART3RECVACKMSGTIMEOUT--;
	if(Timer.PCRecTimer)
			Timer.PCRecTimer--;
	if(Timer.PCRecTimerCR)
			Timer.PCRecTimerCR--;
	if(Timer.PCRecTimerSIMPLE)
			Timer.PCRecTimerSIMPLE--;
	if(Timer.printTimer)
			Timer.printTimer--;
	if(Timer.selTimer)
			Timer.selTimer--;

	if(Timer.zhihui_recv_timeout) //add by yoc zhihui 2013.12.16
		Timer.zhihui_recv_timeout--;
	if(LIFTTABLETIMER)
		LIFTTABLETIMER--;
	if(sysXMTMission.msTimer)
		sysXMTMission.msTimer--;
	 FS_TIMER();
	GOCCHECKTIMEOUT++;
	Tick++;
	if(Tick >= 100)
	{
		Tick = 0;
		if(Timer.WatchDogTimer)
			Timer.WatchDogTimer--;
		if(Timer.PayoutTimer)
			Timer.PayoutTimer--;
		if(Timer.EscrowTimer)
			Timer.EscrowTimer--;		
		if(Timer.DispFreeTimer)
			Timer.DispFreeTimer--;	
		if(Timer.CheckDevStateTimer)
			Timer.CheckDevStateTimer--;	
		if(Timer.GetTubeTimer)
			Timer.GetTubeTimer--;	
		if(Timer.ChaxunTimer)
			Timer.ChaxunTimer--;
		if(Timer.SaleTimer)
			Timer.SaleTimer--;
		if(Timer.ActionPCTimer)
			Timer.ActionPCTimer--;
		if(Timer.offLinePCTimer)
			Timer.offLinePCTimer--;

		if(Timer.zhihui_button_timeout)//add by yoc zhihui 2013.12.16
			Timer.zhihui_button_timeout--;
		if(Timer.zhihui_trade_time)//
			Timer.zhihui_trade_time--;
		if(Timer.pcm_handle_timeout)
			Timer.pcm_handle_timeout--;
		if(Timer.getTempTimer)
			Timer.getTempTimer--;
		if(Timer.mdb_col_timer)
			Timer.mdb_col_timer--;	
		if(Timer.weiTimer)
			Timer.weiTimer--;
	}
	OSIntExit();
}
/*********************************************************************************************************
* Function Name:        TIMER1_IRQHandler
* Description:          定时器1中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void TIMER1_IRQHandler (void)
{
	OSIntEnter();
	T1IR = 1;
	if(GetParallelCoinAcceptorStatus() == 0x01)
		ScanPPCoinChannel();
	if(GetSerialCoinAcceptorStatus() == 0x01)
		ScanSPCoinChannel();	
	OSIntExit();
}
/*********************************************************************************************************
* Function Name:        TIMER2_IRQHandler
* Description:          定时器2中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void TIMER2_IRQHandler (void)
{
	OSIntEnter();
	T2IR = 1;	
	OSIntExit();
}
/*********************************************************************************************************
* Function Name:        TIMER3_IRQHandler
* Description:          定时器3中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void TIMER3_IRQHandler (void)
{
	OSIntEnter();
	T3IR = 1;	
	OSIntExit();
}
/**************************************End Of File*******************************************************/
