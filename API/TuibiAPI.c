/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           TuibiAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        退币程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
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
** Function name:       IsTuibiAPI
** Descriptions:        是否有按下找零器
** input parameters:    无
** output parameters:   无
** Returned value:      1按下，0没按下
*********************************************************************************************************/
uint8_t IsTuibiAPI()
{
	uint8_t err,returnvalue = 0;
	MessagePack *TuibiMsg;
	
	//针对主板退币按键
	if(ReadPayBackKeyValue())
		returnvalue = 1;
	//针对MDB退币按键
	TuibiMsg = OSMboxPend(g_CoinMoneyBackMail,5,&err);//返回是否退币		
	if(err == OS_NO_ERR) 
	{
		if(TuibiMsg->CoinBackCmd == MBOX_COINRETURN)
		{
			returnvalue = 2;
			OSMboxAccept(g_CoinMoneyBackMail);
		}
	}
	if((returnvalue == 1)||(returnvalue == 2))
	{
		Trace("\r\n MiddleCoinReturn=%d",returnvalue);
		Buzzer();	
		ButtonRPTAPI(4,0,0);//上报pc按键信息
	}	
	return returnvalue;
}



/*********************************************************************************************************
** Function name:       ClrTuibiAPI
** Descriptions:        清除退币标识
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClrTuibiAPI()
{
	uint8_t err;
	MessagePack *TuibiMsg;
	
	//针对主板退币按键
	ReadPayBackKeyValue();
	//针对MDB退币按键
	TuibiMsg = OSMboxPend(g_CoinMoneyBackMail,10,&err);//返回是否退币		
	if(err == OS_NO_ERR) 
	{
		if(TuibiMsg->CoinBackCmd == MBOX_COINRETURN)
		{
			OSMboxAccept(g_CoinMoneyBackMail);
		}
	}	
}


