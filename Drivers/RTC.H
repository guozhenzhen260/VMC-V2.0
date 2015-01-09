/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           RTC
** Last modified Date:  2013-09-12
** Last Version:         
** Descriptions:        RTC实时时钟驱动程序头文件                    
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-04
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#ifndef _RTC_H
#define _RTC_H

#define RTCSecond		0x02
#define RTCMinute		0x03
#define RTCHour			0x04
#define	RTCDay			0x05
#define RTCWeek			0x06
#define RTCMonth		0x07
#define RTCYear			0x08

#define	BCD_to_HEC(b)	((((b)>>4)*10)+((b)&0x0f))
#define	HEC_to_BCD(h)	((((h)/10)<<4)|((h)%10))


typedef struct _RTC_DATE
{
   INT8U	second;
   INT8U	minute;
   INT8U	hour;
   INT8U	day;
   INT8U	week;
   INT8U	month;
   INT16U	year;
} RTC_DATE;

void InitRtc(void);
unsigned char RTCStartOrStop(unsigned char cmd);
unsigned char RTCReadTime(RTC_DATE *tp);
unsigned char RTCSetTime(RTC_DATE *tp);
unsigned char RTCSetYMWDHMS(unsigned char cmd, unsigned short TD);

#endif
/**************************************End Of File*******************************************************/
