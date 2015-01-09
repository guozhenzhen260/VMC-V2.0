/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           ChannelAccepter.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        用于货道底层驱动控制                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/

#include "..\config.h"

//通信包序列号
uint8_t chSn = 1;

struct ChannelErrForm stChannelErrForm;


/*********************************************************************************************
**函   数   名:	ChannelHandle
**函数功能说明：货道操作函数
**函数传入参数：Handletype：操作类型;Add：受控货道地址;CheckFlag:出货检测是否开启：1开启，0关闭
**函数传出参数：无
**函 数 返回值：1：成功；0：操作失败；
**********************************************************************************************/
uint8_t ChannelHandle(unsigned char Handletype,unsigned char Add,unsigned char CheckFlag,unsigned char Binnum)
{
	uint8_t ackdata[36];
	uint8_t res = 0,channel,index = 0;
	uint16_t data;
	
	channel = (Binnum == 1) ? 2 : 4;
	index = (Binnum == 1) ? 0 : 1;
	data = (CheckFlag == 1) ? 0x0100 : 0x0000;
	if(Handletype == CHANNEL_OUTGOODS)
	{
		res = EvbConversation(channel,0xED,chSn,OUTGOODSCMD,Add,data,ackdata);
		chSn = (chSn < 255) ? chSn + 1 : 1;
		if(res == 1)
		{
			stChannelErrForm.HuoDaoBanFlag[index] = 0;
			if(ackdata[5]==0x80)
				stChannelErrForm.GOCErrFlag[index]= 1;
			else
				stChannelErrForm.GOCErrFlag[index]=0;
			return ackdata[5];//出货成功
		}
		else
		{
			stChannelErrForm.HuoDaoBanFlag[index] = 1;
			return 0xff;
		}		
	}
	else if(Handletype == CHANNEL_CHECKOUTRESULT)
	{
		res = EvbConversation(channel,0xED,chSn,CHECKOUTRESULT,Add,data,ackdata);
		if(res == 1)
		{
			stChannelErrForm.HuoDaoBanFlag[index]=0;
			chSn = (chSn < 255) ? chSn + 1 : 1;
			if(ackdata[5]==0x80)
				stChannelErrForm.GOCErrFlag[index]=1;	
			else
				stChannelErrForm.GOCErrFlag[index]=0;
			return ackdata[5];//出货成功
		}
		else
		{
			stChannelErrForm.HuoDaoBanFlag[index]=1;
			return 0xff;
		}
	}
	else if(Handletype == CHANNEL_CHECKSTATE)
	{
		res = EvbConversation(channel,0xED,chSn,CHECKCHANNEL,Add,data,ackdata);	
		if(res == 1)
		{
			stChannelErrForm.HuoDaoBanFlag[index]=0;
			chSn = (chSn < 255) ? chSn + 1 : 1;
			if(ackdata[5]==0x80)
				stChannelErrForm.GOCErrFlag[index]=1;	
			else
				stChannelErrForm.GOCErrFlag[index]=0;
			return ackdata[5];//出货成功
		}
		else
		{
			stChannelErrForm.HuoDaoBanFlag[index]=1;
			return 0xff;
		}
	}

	return 0xff;
	
}

/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/

