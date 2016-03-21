/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           WDTDog.h
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        看门狗驱动程序相关函数声明                      
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-04
** Version:             V0.1 
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#ifndef __WDTDOG_H 
#define __WDTDOG_H
//1秒钟
#define WDTTICKETS 1000000
extern void WDT_Init(INT16U second);
extern void WDT_Feed( void );

#endif
/**************************************End Of File*******************************************************/
