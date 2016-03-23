/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           BILLACCEPTER
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        纸币器中间程序相关函数声明                     
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
** Function name:       BillDevInit
** Descriptions:        纸币器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevInitAPI()
{
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:
			//纸币循环找零器
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				TraceBill("\r\n Mddpost recyclerinit");
				//1.启动纸币循环找零器
				MsgAccepterPack.BillBack = MBOX_BILLRECYCLERDEV;
			}
			//纸币器
			else
			{
				TraceBill("\r\n Mddpost billinit");
				//1.启动纸币器
				MsgAccepterPack.BillBack = MBOX_BILLINITDEV;
			}
			OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
			
			break;	
		default:break;	
	}
	return;
}

/*********************************************************************************************************
** Function name:       GetBillDevMoneyInAPI
** Descriptions:        获得纸币器设备的收币状态(收到后自动压抄)
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetBillDevMoneyInAPI(void)
{
	unsigned char ComStatus;
	MessagePack *BillMsg;
	uint32_t money,ReturnBack;
	uint8_t err,i;

	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:
			BillMsg = OSQPend(g_BillMoneyQ,10,&ComStatus);
			//Trace("mdb=%d\r\n",ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				money = BillMsg->BillMoney;				
				//ReturnBack = money;
				
				//压抄纸币器
				MsgAccepterPack.BillBack = MBOX_BILLESCROW;			
				OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
				for(i=0;i<10;i++)
				{
					//Trace("\r\n 1.");
					//3.等待是否压抄成功
					BillMsg = OSMboxPend(g_BillMoneyBackMail,OS_TICKS_PER_SEC*4,&err);//返回是否压抄成功			
					if(err == OS_NO_ERR) 
					{					
						//4.压抄成功，发送压抄成功给纸币器
						if(BillMsg->BillBackCmd == MBOX_BILLESCROWSUCC)
						{
							ReturnBack = money;
						}
						//5.压抄失败，发送压抄失败给纸币器
						else if(BillMsg->BillBackCmd == MBOX_BILLESCROWFAIL)
						{
							ReturnBack = 0;
						}
						//Trace("\r\n 2.=%d",ReturnBack);
						break;
					}
					OSTimeDly(7);
				}
				if(i >= 10)
				{
					ReturnBack = 0;
				}
			}
			else
			{
				ReturnBack = 0;
			}
			break;	
		default:break;	
	}
	return ReturnBack;
	
}

/*********************************************************************************************************
** Function name:       GetBillDevMoneyUnStackInAPI
** Descriptions:        获得纸币器设备的收币状态(收到后不自动压抄)
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetBillDevMoneyUnStackInAPI(void)
{
	unsigned char ComStatus;
	MessagePack *BillMsg;
	uint32_t money,ReturnBack;
	

	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:
			BillMsg = OSQPend(g_BillMoneyQ,10,&ComStatus);
			//Trace("mdb=%d\r\n",ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				money = BillMsg->BillMoney;				
				ReturnBack = money;								
			}
			else
			{
				ReturnBack = 0;
			}
			break;
		default:break;	
	}
	return ReturnBack;
	
}

/*********************************************************************************************************
** Function name:       StackedBillDevMoneyInAPI
** Descriptions:        压抄存入的纸币
** input parameters:    无
** output parameters:   无
** Returned value:      成功-1,失败-0
*********************************************************************************************************/
uint8_t StackedBillDevMoneyInAPI(void)
{
	MessagePack *BillMsg;
	uint32_t ReturnBack;
	uint8_t err,i;

	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:
			if(stDevValue.BillEscrowFun == 0)//该纸币器无暂存功能
			{
				ReturnBack = 1;
			}
			//该纸币器有暂存功能
			else
			{
				//压抄纸币器
				MsgAccepterPack.BillBack = MBOX_BILLESCROW;			
				OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
				for(i=0;i<10;i++)
				{
					//Trace("\r\n 1.");
					//3.等待是否压抄成功
					BillMsg = OSMboxPend(g_BillMoneyBackMail,OS_TICKS_PER_SEC*4,&err);//返回是否压抄成功			
					if(err == OS_NO_ERR) 
					{					
						//4.压抄成功，发送压抄成功给纸币器
						if(BillMsg->BillBackCmd == MBOX_BILLESCROWSUCC)
						{
							ReturnBack = 1;
						}
						//5.压抄失败，发送压抄失败给纸币器
						else if(BillMsg->BillBackCmd == MBOX_BILLESCROWFAIL)
						{
							ReturnBack = 0;
						}
						//Trace("\r\n 2.=%d",ReturnBack);
						break;
					}
					OSTimeDly(7);
				}
				if(i >= 10)
				{
					ReturnBack = 0;
				}	
			}
			break;	
		default:break;	
	}
	return ReturnBack;
	
}

/*********************************************************************************************************
** Function name:       ReturnBillDevMoneyInAPI
** Descriptions:        退出暂存的纸币
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t ReturnBillDevMoneyInAPI(void)
{
	MessagePack *BillMsg;
	uint32_t ReturnBack;
	uint8_t err,i;

	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:
			if(stDevValue.BillEscrowFun == 0)//该纸币器无暂存功能
			{
				ReturnBack = 1;
			}
			//该纸币器有暂存功能
			else
			{
				//退币纸币器
				MsgAccepterPack.BillBack = MBOX_BILLRETURN;			
				OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
				for(i=0;i<10;i++)
				{
					//Trace("\r\n 1.");
					//3.等待是否退币成功
					BillMsg = OSMboxPend(g_BillMoneyBackMail,OS_TICKS_PER_SEC*5,&err);//返回是否退币成功			
					if(err == OS_NO_ERR) 
					{					
						//4.退币成功，发送退币成功给纸币器
						if(BillMsg->BillBackCmd == MBOX_BILLRETURNSUCC)
						{
							ReturnBack = 1;
						}
						//5.退币失败，发送退币失败给纸币器
						else if(BillMsg->BillBackCmd == MBOX_BILLRETURNFAIL)
						{
							ReturnBack = 0;
						}
						//Trace("\r\n 2.=%d",ReturnBack);
						break;
					}
				}
				if(i >= 10)
				{
					ReturnBack = 0;
				}	
			}
			break;	
		default:break;	
	}
	return ReturnBack;
	
}



/*********************************************************************************************************
** Function name:       BillDevDisable
** Descriptions:        禁能纸币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevDisableAPI(void)
{
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:		
			TraceBill("\r\n MddBILLDISABLEDEV post");
			MsgAccepterPack.BillBack = MBOX_BILLDISABLEDEV;			
			OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
			break;
		default:break;	
	}
	return;	
}






/*********************************************************************************************************
** Function name:       BillDevEnable
** Descriptions:        使能纸币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevEnableAPI(void)
{
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:			
			MsgAccepterPack.BillBack = MBOX_BILLENABLEDEV;			
			OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
			break;
		default:break;	
	}
	return;
}

/*********************************************************************************************************
** Function name:       BillDevResetAPI
** Descriptions:        重启纸币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevResetAPI(void)
{
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:			
			MsgAccepterPack.BillBack = MBOX_BILLRESETDEV;			
			OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
			break;	
		default:break;	
	}
	return;
}


/*********************************************************************************************************
** Function name:       GetBillDevState
** Descriptions:        获得硬币器设备的当前状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void GetBillDevStateAPI()
{
	return;	
}

/*********************************************************************************************************
** Function name:       BillRecyclerPayoutNumExpanseAPI
** Descriptions:        按张数找币,0-15的面值用0x00-0x0f来表示,0x00,0x01表示出币数量
** input parameters:    RecyPayoutMoney需要找的面值,RecyPayoutNum需要找的张数
** output parameters:   无
** Returned value:      1找零成功,0找零失败
*********************************************************************************************************/
unsigned char BillRecyclerPayoutNumExpanseAPI(unsigned int RecyPayoutMoney,unsigned char RecyPayoutNum)
{
	MessagePack *BillMsg;
	//MessageFSBillRecyclerPack *FSBillRecyclerMsg;
	uint8_t err,ReturnBack = 1;	
	uint32_t PayoutMoney=0,PayouBacktMoney=0;//找零金额，实际找零金额
	
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:	
			//纸币循环找零器
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				MsgAccepterPack.BillBack = MBOX_BILLRECYPAYOUTNUM;	
				MsgAccepterPack.RecyPayoutMoney = RecyPayoutMoney;	
				MsgAccepterPack.RecyPayoutNum = RecyPayoutNum;	
				OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
				TraceBill("\r\n MddRECPay=%ld,Num=%d",RecyPayoutMoney,RecyPayoutNum);
				OSMboxAccept(g_BillMoneyBackMail);
				//3.等待是否找零成功
				BillMsg = OSMboxPend(g_BillMoneyBackMail,OS_TICKS_PER_SEC*15*RecyPayoutNum,&err);//返回是否找零成功			
				if(err == OS_NO_ERR) 
				{					
					//4.成功，发送压抄成功给纸币器
					if(BillMsg->BillBackCmd == MBOX_BILLRECYPAYOUTSUCC)
					{
						ReturnBack = 1;
					}
					//5.失败，发送压抄失败给纸币器
					else if(BillMsg->BillBackCmd == MBOX_BILLRECYPAYOUTFAIL)
					{
						ReturnBack = 0;
					}
					//Trace("\r\n 2.=%d",ReturnBack);					
				}				
					
			}
			else if(SystemPara.BillRecyclerType == FS_BILLRECYCLER)
			{
				/*MsgFSBillRecyclerPack.BillBack = MBOX_FSBILLRECYPAYOUTNUM;	
				MsgFSBillRecyclerPack.RecyPayoutMoney = RecyPayoutMoney;	
				MsgFSBillRecyclerPack.RecyPayoutNum = RecyPayoutNum;	
				OSMboxPost(g_FSBillRecyclerMail,&MsgFSBillRecyclerPack);
				print_fs("\r\n MddFSPay=%ld,Num=%d",RecyPayoutMoney,RecyPayoutNum);
				OSMboxAccept(g_FSBillRecyclerBackMail);
				//3.等待是否找零成功
				FSBillRecyclerMsg = OSMboxPend(g_FSBillRecyclerBackMail,OS_TICKS_PER_SEC*15*RecyPayoutNum,&err);//返回是否找零成功			
				if(err == OS_NO_ERR) 
				{					
					//4.成功，发送压抄成功给纸币器
					if(FSBillRecyclerMsg->BillBackCmd == MBOX_FSBILLRECYPAYOUTSUCC)
					{
						print_fs("\r\n MddFSPayREC=SUCC");
						ReturnBack = 1;
					}
					//5.失败，发送压抄失败给纸币器
					else if(FSBillRecyclerMsg->BillBackCmd == MBOX_FSBILLRECYPAYOUTFAIL)
					{
						print_fs("\r\n MddFSPayREC=FAIL");
						ReturnBack = 0;
					}
					//Trace("\r\n 2.=%d",ReturnBack);					
				}
				*/
				PayoutMoney=RecyPayoutMoney*RecyPayoutNum;
				PayouBacktMoney=FS_dispense(PayoutMoney);		
				print_fs("\r\n MddFSPay=%ld,MddFSBackPay=%ld",PayoutMoney,PayouBacktMoney);
			}
			break;	
		default:break;	
	}	
	TraceBill("\r\n MddRECPaySuc=%d",ReturnBack);
	return ReturnBack;
}


/*********************************************************************************************************
** Function name:       BillRecyclerPayoutValueExpanse
** Descriptions:        按金额找币                        
** input parameters:    RecyPayoutMoney需要找的金额,以分为单位
** output parameters:   无
** Returned value:      1找零成功,0找零失败
*********************************************************************************************************/
unsigned char BillRecyclerPayoutValueExpanseAPI(unsigned int RecyPayoutMoney,uint32_t  *RecyPayoutMoneyBack)
{
	MessagePack *BillMsg;
	uint8_t err,ReturnBack = 1;
	
	switch(SystemPara.BillValidatorType)
	{		
		case MDB_BILLACCEPTER:	
			//纸币循环找零器
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				if(RecyPayoutMoney)
				{
					MsgAccepterPack.BillBack = MBOX_BILLRECYPAYOUTVALUE;	
					MsgAccepterPack.RecyPayoutMoney = RecyPayoutMoney;	
					OSMboxPost(g_BillMoneyMail,&MsgAccepterPack);
					TraceBill("\r\n MddRECPay=%ld",RecyPayoutMoney);
					OSMboxAccept(g_BillMoneyBackMail);
					//3.等待是否找零成功
					BillMsg = OSMboxPend(g_BillMoneyBackMail,OS_TICKS_PER_SEC*60*5,&err);//返回是否找零成功 		
					if(err == OS_NO_ERR) 
					{					
						//4.找零成功，发送找零成功给纸币器
						if(BillMsg->BillBackCmd == MBOX_BILLRECYPAYOUTSUCC)
						{
							*RecyPayoutMoneyBack=BillMsg->RecyPayoutMoneyBack;
							ReturnBack = 1;
						}
						//5.找零失败，发送找零失败给纸币器
						else if(BillMsg->BillBackCmd == MBOX_BILLRECYPAYOUTFAIL)
						{
							*RecyPayoutMoneyBack=0;
							ReturnBack = 0;
						}
						//Trace("\r\n 2.=%d",ReturnBack);					
					}	
					else
					{
						ReturnBack = 1;
						*RecyPayoutMoneyBack=RecyPayoutMoney;
					}	
				}
					
			}
			else if(SystemPara.BillRecyclerType == FS_BILLRECYCLER)
			{
				if(RecyPayoutMoney)
				{
					/*MsgFSBillRecyclerPack.BillBack = MBOX_FSBILLRECYPAYOUTNUM;	
					MsgFSBillRecyclerPack.RecyPayoutMoney = RecyPayoutMoney;	
					MsgFSBillRecyclerPack.RecyPayoutNum = RecyPayoutNum;	
					OSMboxPost(g_FSBillRecyclerMail,&MsgFSBillRecyclerPack);
					print_fs("\r\n MddFSPay=%ld,Num=%d",RecyPayoutMoney,RecyPayoutNum);
					OSMboxAccept(g_FSBillRecyclerBackMail);
					//3.等待是否找零成功
					FSBillRecyclerMsg = OSMboxPend(g_FSBillRecyclerBackMail,OS_TICKS_PER_SEC*15*RecyPayoutNum,&err);//返回是否找零成功			
					if(err == OS_NO_ERR) 
					{					
						//4.成功，发送压抄成功给纸币器
						if(FSBillRecyclerMsg->BillBackCmd == MBOX_FSBILLRECYPAYOUTSUCC)
						{
							print_fs("\r\n MddFSPayREC=SUCC");
							ReturnBack = 1;
						}
						//5.失败，发送压抄失败给纸币器
						else if(FSBillRecyclerMsg->BillBackCmd == MBOX_FSBILLRECYPAYOUTFAIL)
						{
							print_fs("\r\n MddFSPayREC=FAIL");
							ReturnBack = 0;
						}
						//Trace("\r\n 2.=%d",ReturnBack);					
					}
					*/
					*RecyPayoutMoneyBack=FS_dispense(RecyPayoutMoney);		
					print_fs("\r\n MddFSPay=%ld,MddFSBackPay=%ld",RecyPayoutMoney,*RecyPayoutMoneyBack);
					ReturnBack = 1;
				}
			}
			break;
		default:break;	
	}	
	TraceBill("\r\n MddRECPaySuc=%d",ReturnBack);
	return ReturnBack;
}



