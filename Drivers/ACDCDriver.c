/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CompressorAndLightAccepter.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        展示灯压缩机控制驱动                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"




static unsigned char lSn = 0;

unsigned char ACDCHandle(unsigned char Add,unsigned short nData)
{
	uint8_t ackdata[36];
	uint8_t res = 0;
	
	if(Add==1)
		Add = 0x1e;
	else
		Add = 0x1f;
	res = EvbConversation(3,0xED,lSn,0x51,Add,nData,ackdata);
	TraceCompress("EvbConversation_3 res = %d,ackdata[5] = %d\r\n",res,ackdata[5]);
	if(res == 1)
	{
		if(lSn>=255)
			lSn = 0;
		else
			lSn++;
		
		//return ackdata[5];
		return 1;
	}
	return 0xff;
}


/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/

