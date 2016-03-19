/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           WDTDog.c
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        看门狗驱动程序                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-01-04
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

/*********************************************************************************************************
** Function name:     	WDT_Init
** Descriptions:	    看门狗初始化
** input parameters:   second 多少秒
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void WDT_Init(INT16U second)
{
	WDCLKSEL=0;
	WDTC = WDTTICKETS*second;	// 设置超时时间
	WDMOD = 0x03;	// 使能
	
	WDT_Feed();
}

/*********************************************************************************************************
** Function name:     	WDT_Feed()
** Descriptions:	    喂狗
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void WDT_Feed( void )
{
	WDFEED = 0xAA;		// 喂狗
	WDFEED = 0x55;
}
/**************************************End Of File*******************************************************/
