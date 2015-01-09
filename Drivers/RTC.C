/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           RTC
** Last modified Date:  2013-09-12
** Last Version:         
** Descriptions:        RTC实时时钟驱动程序                     
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
#include "..\config.h"

/*********************************************************************************************************
** Function name:       RTCStartOrStop
** Descriptions:        Start或Stop RTC
** input parameters:    cmd     1       开启RTC
**                              0       关闭RTC
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
void InitRtc(void)
{
    PCONP |= (1 << 9);
	if(RTC_AUX & (0x1<<4) )
	{
		RTC_AUX |= (0x1<<4);	
	}
	RTCCCR   = 0;                                          
    RTCILR   = 0x03;                                         
    RTCCIIR  = 0;                                          
    RTCAMR   = 0;
	RTCStartOrStop(1);
}
/*********************************************************************************************************
** Function name:       RTCStartOrStop
** Descriptions:        Start或Stop RTC
** input parameters:    cmd     1       开启RTC
**                              0       关闭RTC
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
unsigned char RTCStartOrStop(unsigned char cmd)
{
	if(cmd == 1)
	{
		RTCCCR |= 0x01;;
		RTCILR = 0x01;
	}
	else
	{
		RTCCCR &= ~0x01;
	}
	return 1;
}
/*********************************************************************************************************
** Function name:       RTCReadTime
** Descriptions:        读取PCF8563,将YMWDHMS全部读出
** input parameters:    tp:指向时间结构体的指针
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
unsigned char RTCReadTime(RTC_DATE *tp)
{
	tp->second = RTCSEC;
	tp->minute = RTCMIN;
	tp->hour   = RTCHOUR;
	tp->day    = RTCDOM;
	tp->week   = RTCDOW;
	tp->month  = RTCMONTH;
	tp->year   = RTCYEAR;
	//Trace("readRTC:%04d-%02d-%02d,%02d:%02d:%02d",tp->year,tp->month,tp->day,tp->hour,tp->minute,tp->second);
	return TRUE;
}
/*********************************************************************************************************
** Function name:       RTCSetTime
** Descriptions:        设置RTC,一次设定YMWDHMS全部值
** input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
unsigned char RTCSetTime(RTC_DATE *tp)
{
    RTCYEAR  = tp->year;
    RTCMONTH = tp->month;
    RTCDOW   = tp->week;
    RTCDOM   = tp->day;
    RTCHOUR  = tp->hour;
    RTCMIN   = tp->minute;
    RTCSEC   = tp->second;
	//Trace("setRTC:%04d-%02d-%02d,%02d:%02d:%02d",tp->year,tp->month,tp->day,tp->hour,tp->minute,tp->second);
	return TRUE;	
}
/*********************************************************************************************************
** Function name:       RTCSetYMWDHMS
** Descriptions:        单独设置RTC的YMWDHMS
** input parameters:    cmd       Second         设置秒
**                                Minute         设置分
**                                Hour           设置小时
**                                Day            设置日
**                                Week           设置星期
**                                Month          设置月
**                                Year           设置年
**                        TD        设定值
** Output parameters:   FALSE or TRUE
** Returned value:      NONE
*********************************************************************************************************/
unsigned char RTCSetYMWDHMS(unsigned char cmd,unsigned short TD)
{
	switch(cmd)
	{
		case RTCSecond:	RTCSEC = TD;
						break;
		case RTCMinute:	RTCMIN = TD;		
						break;
		case RTCHour:	RTCHOUR= TD;	
						break;
		case RTCDay:	RTCDOM = TD;	
						break;
		case RTCWeek:	RTCDOW = TD;	
						break;
		case RTCMonth:	RTCMONTH = TD;	
						break;
		case RTCYear:	RTCYEAR	= TD;
						break;
		default:break;		
	}
	return TRUE;	
}
/**************************************End Of File*******************************************************/
