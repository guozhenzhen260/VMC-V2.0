/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           config.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        头文件配置，位于工程主目录                     
**------------------------------------------------------------------------------------------------------
** Created by:          chensunwei
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __CONFIG_H 
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
  通用库相关头文件
*********************************************************************************************************/
#include <stdint.h> 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <rt_misc.h>
#include <stdarg.h>
/*********************************************************************************************************
  固件库相关头文件
*********************************************************************************************************/
#include ".\firmware\firmware.h"
#include ".\firmware\zy_if.h"
#include ".\firmware\LPC1700.h"
#include ".\firmware\target_cfg.h"
#include ".\firmware\LPC1700PinCfg.h"
/*********************************************************************************************************
  外设驱动底层头文件
*********************************************************************************************************/
#include ".\Drivers\uart0.h"
#include ".\Drivers\uart1.h"
#include ".\Drivers\UART2.h"
#include ".\Drivers\uart3.h" 
#include ".\Drivers\buzzer.h"
#include ".\Drivers\lcd.h"
#include ".\Drivers\RTC.H"
#include ".\Drivers\i2c.h"
#include ".\Drivers\timer.h"
#include ".\Drivers\AT45DB161D.H"
#include ".\Drivers\WDTDog.h"
/*********************************************************************************************************
  外设驱动高层件头文件-串口2任务
*********************************************************************************************************/
#include ".\Drivers\MDB_UART2.h"
#include ".\Drivers\EVB_UART2.h"
//#include ".\Drivers\BillAccepter.h"
//#include ".\Drivers\CoinAccepter.h"
//#include ".\Drivers\ChangerAccepter.h"
//#include ".\Drivers\MDBReader.h"
#include ".\Drivers\HopperDriver.h"
#include ".\Drivers\pcoin.h" //脉冲硬币器
#include ".\Drivers\key.h"//按键
#include ".\Drivers\ChannelDriver.h"//货道驱动层
#include ".\Drivers\TASKDEVICE.h"//串口1高层任务
#include ".\Drivers\ACDCDriver.h"
#include ".\Drivers\LiftTableDriver.h"
#include ".\Drivers\GPRS_UART.h"
/*********************************************************************************************************
  外设驱动高层件头文件-串口3任务
*********************************************************************************************************/
//#include ".\Uart3Driver\ZHIHUIPC.H" //changed by yoc 2013.12.16
#include ".\Uart3Driver\TASKUART3DEVICE.H"
/*********************************************************************************************************
  外设驱动高层件头文件-串口1任务
*********************************************************************************************************/
//#include ".\Uart1Driver\SelectKey.H"
#include ".\Uart1Driver\TASKUART1DEVICE.H"
#include ".\Uart1Driver\Xmt.h"
#include ".\Uart1Driver\HeFanGui.h"
#include ".\Uart1Driver\FS_billChanger.h"

/*********************************************************************************************************
  外设驱动高层件头文件-串口0任务  by yoc newAisle
*********************************************************************************************************/
#include ".\Uart0Driver\taskUart0Device.h"


/*********************************************************************************************************
****************************** /\ ********************************************************
****************************** || ********************************************************
****************************[MAILBOX]*******************************************************
****************************** || ********************************************************
****************************** \/ ********************************************************
**********************************************************************************************************/
/*********************************************************************************************************
  中间件头文件
*********************************************************************************************************/
#include ".\API\BillAccepterAPI.h"
#include ".\API\CoinAccepterAPI.h"
#include ".\API\ChangerAPI.h"
#include ".\API\ReaderAPI.h"
#include ".\API\TuibiAPI.h"
#include ".\API\CheckDeviceState.h"
#include ".\API\ChannelAPI.h"
#include ".\API\ACDCAPI.h"
#include ".\API\LogAPI.h"
#include ".\API\TraceAPI.h"
#include ".\API\PCComAPI.h"
#include ".\API\LiftTableAPI.h"
#include ".\API\SelectKeyAPI.h"
#include ".\API\HeFanGuiAPI.h"
#include ".\API\colApi.h"
#include ".\API\ZhiHuiAPI.h" //add by yoc 2013.12.16
#include ".\API\GprsAPI.h"
/*********************************************************************************************************
  应用层头文件
*********************************************************************************************************/
#include ".\APP\common.h"
#include ".\APP\LANGUAGE.h"
#include ".\APP\MAINTAIN.H"
#include ".\APP\BUSINESS.H"

#ifdef __cplusplus
	}
#endif 

#endif

/**************************************End Of File*******************************************************/
