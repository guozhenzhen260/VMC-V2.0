/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           READERACCEPTERAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        刷卡器中间程序相关函数声明                     
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
** Function name:       ReaderDevInit
** Descriptions:        读卡器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ReaderDevInitAPI()
{
	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			//1.启动纸币器
			MsgAccepterPack.ReaderBack = MBOX_READERINITDEV;			
			OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
			break;		
	}
	return;
}

/*********************************************************************************************************
** Function name:       GetReaderDevMoneyInAPI
** Descriptions:        获得读卡器设备的收币状态
** input parameters:    readerType=1有插卡,2没插卡
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetReaderDevMoneyInAPI(uint8_t *readerType)
{
	unsigned char ComStatus;
	MessagePack *ReaderMsg;
	uint32_t money,ReturnBack;

	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			ReaderMsg = OSMboxPend(g_ReaderMoneyMail,10,&ComStatus);
			//TraceReader("\r\nMiddReadermdb=%d\r\n",ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				if((ReaderMsg->ReaderCmd)==MBOX_READERMONEY)
				{
					money = ReaderMsg->ReaderMoney;				
					TraceReader("\r\nMiddReadermiddlevalue=%ld",money);
					ReturnBack = money;
					*readerType = 1;					
				}
				else if((ReaderMsg->ReaderCmd)==MBOX_READERMONEYOUT)
				{
					money = ReaderMsg->ReaderMoney;				
					TraceReader("\r\nMiddReadervalue=%ld\r\n",money);
					ReturnBack = money;
					*readerType = 2;
				}
			}
			else
				ReturnBack = 0;	
			break;					
	}
	return ReturnBack;
	
}




/*********************************************************************************************************
** Function name:       ReaderDevDisable
** Descriptions:        禁能读卡器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ReaderDevDisableAPI(void)
{
	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:			
			MsgAccepterPack.ReaderBack = MBOX_READERDISABLEDEV;			
			OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
			break;					
	}
	return;	
}






/*********************************************************************************************************
** Function name:       ReaderDevEnable
** Descriptions:        使能读卡器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ReaderDevEnableAPI(void)
{
	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:			
			MsgAccepterPack.ReaderBack = MBOX_READERENABLEDEV;			
			OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
			break;					
	}
	return;
}


/*********************************************************************************************************
** Function name:       ReaderDevVendoutAPI
** Descriptions:        刷卡器交易
** input parameters:    money交易金额(分为单位),vendrst=0扣款后交易成功,1扣款后交易失败
** output parameters:   无
** Returned value:      交易成功1，交易失败0
*********************************************************************************************************/
uint8_t ReaderDevVendoutTestAPI(uint16_t money,uint8_t vendrst)
{		
	MessagePack *AccepterMsg;
	uint8_t err,i,returnvalue;
	//uint32_t backmoney;
	//unsigned char ComStatus;

	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			//2.给读卡器发送扣钱命令
			OSMboxAccept(g_ReaderBackMoneyMail);
			MsgAccepterPack.ReaderBack = MBOX_READERVENDREQUEST;	
			MsgAccepterPack.ReaderPrice = money;
			OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);	
			//OSTimeDly(OS_TICKS_PER_SEC*2);
			
			for(i=0;i<10;i++)
			{
				//3.等待是否允许扣钱
				AccepterMsg = OSMboxPend(g_ReaderBackCmdMail,OS_TICKS_PER_SEC*5,&err);//返回是否允许交易				
				if(err == OS_NO_ERR) 
				{
					//4.允许扣钱
					if(AccepterMsg->ReaderBackCmd == MBOX_READERVENDOUT)
					{
						TraceReader("\r\nMiddReader 10.VendSuccess");
						OSTimeDly(OS_TICKS_PER_SEC*2);
						OSMboxAccept(g_ReaderBackCmdMail);
						OSMboxAccept(g_ReaderBackMoneyMail);
						//发送交易成功给读卡器
						if(vendrst==0)
						{
							TraceReader("\r\nMiddReader 10.VendSUCC");
							MsgAccepterPack.ReaderBack = MBOX_READERVENDSUCCESS;
							//MsgAccepterPack.ReaderPrice = money;
							OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
							returnvalue = 1;
						}
						//发送交易失败给读卡器
						else if(vendrst==1)
						{
							TraceReader("\r\nMiddReader 10.VendFail");
							MsgAccepterPack.ReaderBack = MBOX_READERVENDFAIL;
							OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
							
							TraceReader("\r\nMiddReader 10.VendFialComp");
							OSTimeDly(OS_TICKS_PER_SEC*2);
							OSMboxAccept(g_ReaderBackCmdMail);
							OSMboxAccept(g_ReaderBackMoneyMail);
							MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
							OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
							returnvalue = 0;
						}						
						break;
					}
					//5.不允许扣钱，发送交易失败给读卡器
					else if(AccepterMsg->ReaderBackCmd == MBOX_READERVENDDENY)
					{
						TraceReader("\r\nMiddReader 10.VendFialComp");
						OSTimeDly(OS_TICKS_PER_SEC*2);
						OSMboxAccept(g_ReaderBackCmdMail);
						OSMboxAccept(g_ReaderBackMoneyMail);
						MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
						//MsgAccepterPack.ReaderPrice = money;
						OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
						returnvalue = 0;
						break;
					}
				}
			}
			//5.没有收到读卡器信息，发送交易失败给读卡器
			if(i >= 10)
			{
				TraceReader("\r\nMiddReader 10.VendFialComp");
				OSMboxAccept(g_ReaderBackCmdMail);
				OSMboxAccept(g_ReaderBackMoneyMail);
				MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
				//MsgAccepterPack.ReaderPrice = money;
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);	
				returnvalue = 0;
			}	
									
			break;					
	}
	return returnvalue;	
}


/*********************************************************************************************************
** Function name:       ReaderDevVendoutAPI
** Descriptions:        vmc只发送交易请求给读卡器
** input parameters:    money交易金额
** output parameters:   无
** Returned value:      发送成功1，发送失败0
*********************************************************************************************************/
uint8_t ReaderDevVendoutRPTAPI(uint16_t money)
{		
	MessagePack *AccepterMsg;
	uint8_t err,i,returnvalue;
	//uint32_t backmoney;
	//unsigned char ComStatus;

	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			OSMboxAccept(g_ReaderBackMoneyMail);
			MsgAccepterPack.ReaderBack = MBOX_READERVENDREQUEST;	
			MsgAccepterPack.ReaderPrice = money;
			OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);	
			//OSTimeDly(OS_TICKS_PER_SEC*2);
			
			for(i=0;i<3;i++)
			{
				//3.等待是否允许扣钱
				AccepterMsg = OSMboxPend(g_ReaderBackCmdMail,OS_TICKS_PER_SEC*4,&err);//返回是否允许交易				
				if(err == OS_NO_ERR) 
				{
					//4.允许扣钱，交易请求成功
					if(AccepterMsg->ReaderBackCmd == MBOX_READERVENDOUT)
					{
						TraceReader("\r\nMiddReader 10.VendRptSuccess");						
						returnvalue = 1;
						break;
					}
					//5.不允许扣钱，交易请求失败
					else if(AccepterMsg->ReaderBackCmd == MBOX_READERVENDDENY)
					{
						TraceReader("\r\nMiddReader 10.VendFialComp");
						OSTimeDly(OS_TICKS_PER_SEC*2);
						OSMboxAccept(g_ReaderBackCmdMail);
						OSMboxAccept(g_ReaderBackMoneyMail);
						MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
						//MsgAccepterPack.ReaderPrice = money;
						OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
						returnvalue = 0;
						break;
					}
				}
			}
			//5.没有收到读卡器信息，发送交易失败给读卡器
			if(i >= 3)
			{
				TraceReader("\r\nMiddReader 10.VendFialComp");
				OSMboxAccept(g_ReaderBackCmdMail);
				OSMboxAccept(g_ReaderBackMoneyMail);
				MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
				//MsgAccepterPack.ReaderPrice = money;
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);					
				returnvalue = 0;
			}	
									
			break;					
	}
	return returnvalue;	
}

/*********************************************************************************************************
** Function name:       ReaderDevVendoutResult
** Descriptions:        vmc发送交易结果给读卡器
** input parameters:    result交易结果,1成功,2失败
** output parameters:   无
** Returned value:      交易成功1，交易失败0
*********************************************************************************************************/
void ReaderDevVendoutResultAPI(uint8_t result)
{		
		
	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			OSMboxAccept(g_ReaderBackCmdMail);
			OSMboxAccept(g_ReaderBackMoneyMail);
			//发送交易成功给读卡器
			if(result == 1)
			{
				
				TraceReader("\r\nMiddReader 10.VendSUCC");				
				MsgAccepterPack.ReaderBack = MBOX_READERVENDSUCCESS;	
				//MsgAccepterPack.ReaderPrice = money;
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);				
			}
			//发送交易失败给读卡器
			else if(result == 2)
			{
				TraceReader("\r\nMiddReader 10.VendFail");
				MsgAccepterPack.ReaderBack = MBOX_READERVENDFAIL;	
				//MsgAccepterPack.ReaderPrice = money;
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);

				TraceReader("\r\nMiddReader 10.VendFialComp");
				OSTimeDly(OS_TICKS_PER_SEC*2);
				OSMboxAccept(g_ReaderBackCmdMail);
				OSMboxAccept(g_ReaderBackMoneyMail);
				MsgAccepterPack.ReaderBack = MBOX_READERVENDCOMP;	
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack);
			}
			break;
	}
}


/*********************************************************************************************************
** Function name:       GetReaderDevState
** Descriptions:        获得读卡器设备的当前状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void GetReaderDevStateAPI()
{
	return;	
}


/*********************************************************************************************************
** Function name:       ReaderDevCashSaleAPI
** Descriptions:        vmc发送详细现金交易记录给读卡器，用于保存详细信息
** input parameters:    money交易金额
** output parameters:   无
** Returned value:      发送成功1，发送失败0
*********************************************************************************************************/
void ReaderDevCashSaleAPI(uint16_t money)
{		
	//MessagePack *AccepterMsg;
	//uint8_t err,i,returnvalue;
	//uint32_t backmoney;
	//unsigned char ComStatus;

	switch(SystemPara.CashlessDeviceType)
	{		
		case MDB_READERACCEPTER:
			if(stDevValue.ReaderCashSale==1)
			{
				TraceReader("\r\nMiddReader CashSale=%ld",money);
				OSMboxAccept(g_ReaderBackMoneyMail);
				MsgAccepterPack.ReaderBack = MBOX_READERCASHSALE;	
				MsgAccepterPack.ReaderPrice = money;
				OSMboxPost(g_ReaderBackMoneyMail,&MsgAccepterPack); 
			}
			break;					
	}
}



