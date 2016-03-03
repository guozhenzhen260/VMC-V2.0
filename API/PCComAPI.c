/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           PCComAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        pc通讯程序相关函数声明                     
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
#include "PCComAPI.h"
#include "..\App\Business.h"
#include "..\App\Channel.h"
uint32_t changeMoneyInd;
uint8_t TypeInd;
uint32_t payAllMoneyInd;


//更新邮箱缓冲区数组索引
void UpdateIndex()
{
	if(g_Ubox_Index<(UBOX_SIZE-1))
		g_Ubox_Index++;
	else
		g_Ubox_Index=0;
}
//更新SIMPLEUbox邮箱缓冲区数组索引
void UpdateSIMPLEIndex()
{
	if(g_SIMPLEUbox_Index<(SIMPLEUBOX_SIZE-1))
		g_SIMPLEUbox_Index++;
	else
		g_SIMPLEUbox_Index=0;
}

uint32_t getchangeMoneyInd()
{
	return changeMoneyInd;
}

void setchangeMoneyInd(uint32_t changeMoney)
{
	(changeMoneyInd>=changeMoney)?(changeMoneyInd-=changeMoney):0;
}
uint32_t getpayAllMoneyInd()
{
	return payAllMoneyInd;
}
uint8_t getTypeInd()
{
	return TypeInd;
}
	

//等待pc初始化成功
void WaitForPCInit()
{
	static uint8_t simple=0;
	if(SystemPara.PcEnable==UBOX_PC)
	{
		if(globalSys.pcInitFlag == 0)
		{
			LCDNumberFontPrintf(10,LINE15,2,"Registration PC,Please Wait");	
			BillCoinCtr(2,2,2);
			while(globalSys.pcInitFlag == 0)
			{
				OSTimeDly(OS_TICKS_PER_SEC);
				PollAPI(0);
				
				mainTaskPollPC(0);
			}
			BillCoinCtr(1,1,1);
			//LCDClrScreen();
			LCDClearLineDraw(0,LINE15,2);
			rstTime();
		}
	}
	else if(SystemPara.PcEnable==SIMPUBOX_PC)
	{
		if(simple==0)
		{
			simple=1;
			OSTimeDly(OS_TICKS_PER_SEC);
			LCDClearLineDraw(0,LINE15,2);
			rstTime();
		}
	}
}

/*********************************************************************************************************
** Function name:       PayinRPTAPI
** Descriptions:        投币后,发送消息给PC机
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币,5读卡器
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PayinRPTAPI(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney)
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//add by yoc zhihui 2013.12.16				
			vmc_zh_mbox[ZhIndex].recv_all_amount = (uint16_t)payAllMoney;
			vmc_zh_mbox[ZhIndex].recv_amount = payInMoney;
			vmc_zh_mbox[ZhIndex].pay_mode = 0x01;	
			if(dev==1)
			{
				zh_task3_post(MBOX_VMC_ZH_COIN);	
			}	
			else if(dev==2)
			{		
				zh_task3_post(MBOX_VMC_ZH_BILL);  				
			}
			else if(dev==3)	
			{				
				zh_task3_post(MBOX_VMC_ZH_ESCROWIN);		
			}
			else if(dev==4)		
			{						
				zh_task3_post(MBOX_VMC_ZH_ESCROWOUT);
			}		
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxPayin=%d",dev);	
			if(dev==1)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINCOIN;
			else if(dev==2)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINBILL;	
			else if(dev==3)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINESCROWIN;	
			else if(dev==4)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINESCROWOUT;	
			else if(dev==5)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINREADER;	
			MsgUboxPack[g_Ubox_Index].payInMoney = payInMoney;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			TraceCoin("\r\nMidcoin=%ld,%ld",MsgUboxPack[g_Ubox_Index].payInMoney,MsgUboxPack[g_Ubox_Index].payAllMoney);
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
		case CRUBOX_PC:
			TracePC("\r\n MiddUboxPayin");	
			if(dev==1)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINCOIN;
			else if(dev==2)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINBILL;	
			else if(dev==3)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINESCROWIN;	
			else if(dev==4)
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYINESCROWOUT;				
			MsgUboxPack[g_Ubox_Index].payInMoney = payInMoney;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			TraceCoin("\r\nMidcoin=%ld,%ld",MsgUboxPack[g_Ubox_Index].payInMoney,MsgUboxPack[g_Ubox_Index].payAllMoney);
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
	}
}


/*********************************************************************************************************
** Function name:       PayoutRPTAPI
** Descriptions:        找零后,发送消息给PC机
** input parameters:    payoutDev:找零设备,0硬币,1纸币,2读卡器
						payoutType: 1找币类型
                        payoutMoney找出的金额,以分为单位
                        payAllMoney还剩的金额,以分为单位 
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PayoutRPTAPI(uint8_t payoutDev,uint8_t payoutType,uint16_t payoutMoney,uint16_t payoutRemain,uint32_t payAllMoney)
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//add by yoc zhihui 2013.12.16		
			vmc_zh_mbox[ZhIndex].changed = payoutMoney;	
			zh_task3_post(MBOX_VMC_ZH_PAYOUT) ;			
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxPayout");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYOUT;	
			MsgUboxPack[g_Ubox_Index].payoutDev = payoutDev;
			MsgUboxPack[g_Ubox_Index].payoutMoney = payoutMoney;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			MsgUboxPack[g_Ubox_Index].Type        = payoutType;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
		case CRUBOX_PC:
			TracePC("\r\n MiddUboxPayout");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_PAYOUT;	
			MsgUboxPack[g_Ubox_Index].payoutDev = payoutDev;
			MsgUboxPack[g_Ubox_Index].payoutMoney = payoutMoney;
			MsgUboxPack[g_Ubox_Index].payoutRemain = payoutRemain;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			MsgUboxPack[g_Ubox_Index].Type        = payoutType;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;
	}
}


/*********************************************************************************************************
** Function name:       CostRPTAPI
** Descriptions:        扣钱后,发送消息给PC机
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CostRPTAPI(uint8_t costType,uint32_t costMoney,uint32_t payAllMoney)
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//Trace("\r\n MiddPCCommInit");			
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxCost");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_COST;				
			MsgUboxPack[g_Ubox_Index].costMoney = costMoney;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			MsgUboxPack[g_Ubox_Index].Type        = costType;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
		case CRUBOX_PC:
			TracePC("\r\n MiddUboxCost");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_COST;				
			MsgUboxPack[g_Ubox_Index].costMoney = costMoney;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			MsgUboxPack[g_Ubox_Index].Type        = costType;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;		
	}
}

/*********************************************************************************************************
** Function name:       ResetRPTAPI
** Descriptions:        复位后,发送消息给PC机
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ResetRPTAPI()
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//Trace("\r\n MiddPCCommInit");			
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxCost");	
			break;	
		case CRUBOX_PC:
			TracePC("\r\n MiddUboxReset");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_RESETRPT;				
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;		
	}
}


/*********************************************************************************************************
** Function name:       BillStatusRPTAPI
** Descriptions:        向PC机下发纸币器设备故障和恢复状态
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillStatusRPTAPI()
{
	static uint8_t billError = 0;
	switch(SystemPara.PcEnable)
	{	
		case UBOX_PC:
			if(BillIsErr()==1)
			{
				if(billError==0)
				{
					StatusRPTAPI();
					billError=1;
				}
			}
			else
			{
				if(billError==1)
				{
					StatusRPTAPI();
					billError=0;
				}
			}
			break;
	}
}

/*********************************************************************************************************
** Function name:       StatusRPTAPI
** Descriptions:        向PC机下发设备状态
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void StatusRPTAPI()
{
	uint32_t coinMoney=0;
	
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//Trace("\r\n MiddPCCommInit");			
			break;	
		case UBOX_PC:			
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_STATUS;
			MsgUboxPack[g_Ubox_Index].check_st = ErrorStatus(1);
			MsgUboxPack[g_Ubox_Index].bv_st = ErrorStatus(2);	
			MsgUboxPack[g_Ubox_Index].cc_st = ErrorStatus(3);	
			MsgUboxPack[g_Ubox_Index].vmc_st = ErrorStatus(4);	
			if(SystemPara.CoinChangerType == MDB_CHANGER)
			{
				MsgUboxPack[g_Ubox_Index].change = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
						+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5] 
						+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
						+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
						+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
						+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; 
			}
			else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
			{
				coinMoney =( !HopperIsEmpty() )? SystemPara.MaxValue:0;
				if(coinMoney < SystemPara.BillEnableValue)
				{
					MsgUboxPack[g_Ubox_Index].change  = 0;
				}
				else
				{
					MsgUboxPack[g_Ubox_Index].change  = coinMoney;
				}
				//MsgUboxPack[g_Ubox_Index].change  =( !HopperIsEmpty() )? SystemPara.MaxValue:0;				
			}
			//无硬币器
			else if(SystemPara.CoinChangerType == OFF_CHANGER)
			{	
				MsgUboxPack[g_Ubox_Index].change =0;
			}
			
			//添加纸币循环器时，可找硬币金额
			//if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			//{
			//	MsgUboxPack[g_Ubox_Index].change += stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
			//			+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
			//			+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
			//}
			//添加纸币循环器时，纸币的存币数量
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				MsgUboxPack[g_Ubox_Index].recyclerSum[0] = stDevValue.RecyclerNum[0];
				MsgUboxPack[g_Ubox_Index].recyclerSum[1] = stDevValue.RecyclerNum[1];
				MsgUboxPack[g_Ubox_Index].recyclerSum[2] = stDevValue.RecyclerNum[2];
				MsgUboxPack[g_Ubox_Index].recyclerSum[3] = stDevValue.RecyclerNum[3];
				MsgUboxPack[g_Ubox_Index].recyclerSum[4] = stDevValue.RecyclerNum[4];
				MsgUboxPack[g_Ubox_Index].recyclerSum[5] = stDevValue.RecyclerNum[5];
			}
			else
			{
				MsgUboxPack[g_Ubox_Index].recyclerSum[0] = 0;
				MsgUboxPack[g_Ubox_Index].recyclerSum[1] = 0;
				MsgUboxPack[g_Ubox_Index].recyclerSum[2] = 0;
				MsgUboxPack[g_Ubox_Index].recyclerSum[3] = 0;
				MsgUboxPack[g_Ubox_Index].recyclerSum[4] = 0;
				MsgUboxPack[g_Ubox_Index].recyclerSum[5] = 0;
			}
			//硬币的存币数量
			if(SystemPara.CoinChangerType == MDB_CHANGER)
			{
				MsgUboxPack[g_Ubox_Index].coinSum[0] = stDevValue.CoinNum[0];
				MsgUboxPack[g_Ubox_Index].coinSum[1] = stDevValue.CoinNum[1];
				MsgUboxPack[g_Ubox_Index].coinSum[2] = stDevValue.CoinNum[2];
				MsgUboxPack[g_Ubox_Index].coinSum[3] = stDevValue.CoinNum[3];
				MsgUboxPack[g_Ubox_Index].coinSum[4] = stDevValue.CoinNum[4];
				MsgUboxPack[g_Ubox_Index].coinSum[5] = stDevValue.CoinNum[5];
			}
			//找零斗的存币数量
			else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
			{
				//正常
				if(DeviceStateBusiness.Hopper1State==0)
				{
					MsgUboxPack[g_Ubox_Index].coinSum[0] = 255;
				}
				else
				{
					MsgUboxPack[g_Ubox_Index].coinSum[0] = 0;
				}	
				//正常
				if(DeviceStateBusiness.Hopper2State==0)
				{
					MsgUboxPack[g_Ubox_Index].coinSum[1] = 255;
				}
				else
				{
					MsgUboxPack[g_Ubox_Index].coinSum[1] = 0;
				}
				//正常
				if(DeviceStateBusiness.Hopper3State==0)
				{
					MsgUboxPack[g_Ubox_Index].coinSum[2] = 255;
				}
				else
				{
					MsgUboxPack[g_Ubox_Index].coinSum[2] = 0;
				}
				MsgUboxPack[g_Ubox_Index].coinSum[3] = 0;
				MsgUboxPack[g_Ubox_Index].coinSum[4] = 0;
				MsgUboxPack[g_Ubox_Index].coinSum[5] = 0;
			}
			
			MsgUboxPack[g_Ubox_Index].tem_st = UserPara.CompressorCtr.flag;
			TracePC("\r\n MiddUboxSataus check_st=%d,bv_st=%d,cc_st=%d,vmc_st=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,change=%ld",MsgUboxPack[g_Ubox_Index].check_st,MsgUboxPack[g_Ubox_Index].bv_st,MsgUboxPack[g_Ubox_Index].cc_st,MsgUboxPack[g_Ubox_Index].vmc_st,stDevValue.CoinValue[0],MsgUboxPack[g_Ubox_Index].coinSum[0],stDevValue.CoinValue[1],MsgUboxPack[g_Ubox_Index].coinSum[1],
				stDevValue.CoinValue[2],MsgUboxPack[g_Ubox_Index].coinSum[2],stDevValue.CoinValue[3],MsgUboxPack[g_Ubox_Index].coinSum[3],stDevValue.CoinValue[4],MsgUboxPack[g_Ubox_Index].coinSum[4],stDevValue.CoinValue[5],MsgUboxPack[g_Ubox_Index].coinSum[5],MsgUboxPack[g_Ubox_Index].change);	
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;		
		case CRUBOX_PC:			
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_INFORPT;
			TracePC("\r\n MiddUboxSataus check_st=%d,bv_st=%d,cc_st=%d,vmc_st=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,change=%ld",MsgUboxPack[g_Ubox_Index].check_st,MsgUboxPack[g_Ubox_Index].bv_st,MsgUboxPack[g_Ubox_Index].cc_st,MsgUboxPack[g_Ubox_Index].vmc_st,stDevValue.CoinValue[0],MsgUboxPack[g_Ubox_Index].coinSum[0],stDevValue.CoinValue[1],MsgUboxPack[g_Ubox_Index].coinSum[1],
				stDevValue.CoinValue[2],MsgUboxPack[g_Ubox_Index].coinSum[2],stDevValue.CoinValue[3],MsgUboxPack[g_Ubox_Index].coinSum[3],stDevValue.CoinValue[4],MsgUboxPack[g_Ubox_Index].coinSum[4],stDevValue.CoinValue[5],MsgUboxPack[g_Ubox_Index].coinSum[5],MsgUboxPack[g_Ubox_Index].change);	
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;
	}
}

/*********************************************************************************************************
** Function name:       PayoutRecyAPI
** Descriptions:        返回可找零硬币金额和纸币金额
** input parameters:    type=0硬币找零金额,1纸币找零金额
** output parameters:  无
** Returned value:      找零金额
*********************************************************************************************************/
uint32_t PayoutRecyAPI(uint8_t type)
{
	uint32_t coinMoney=0,change=0;
	if(type==0)//硬币找零
	{
		//硬币器是否故障
		if(ChangerIsErr()==1)
		{
			change = 0;
		}
		//找零金额
	       else if(SystemPara.CoinChangerType == MDB_CHANGER)
		{
			change = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
					+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5] 
					+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
					+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
					+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
					+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; 
		}
		else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
		{
			coinMoney =( !HopperIsEmpty() )? SystemPara.MaxValue:0;
			if(coinMoney < SystemPara.BillEnableValue)
			{
				change  = 0;
			}
			else
			{
				change  = coinMoney;
			}
			//MsgUboxPack[g_Ubox_Index].change  =( !HopperIsEmpty() )? SystemPara.MaxValue:0;				
		}
		//无硬币器
		else if(SystemPara.CoinChangerType == OFF_CHANGER)
		{	
			change =0;
		}
	}	
	else if(type==1)//纸币找零
	{
		//添加纸币循环器时，可找硬币金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			if(ErrorStatus(2)>0)//故障
			{	
				change =0;
			}
			else if(GetHoldMoney()>0)//有暂存纸币
			{	
				change =0;
			}
			else 
			{
				change = stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
						+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
						+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
			}
		}
		//富士纸币器
		else if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)
		{
			change = stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
					+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
					+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
		}
	}		
	return change;		
}

/*********************************************************************************************************
** Function name:       ActionRPTAPI
** Descriptions:        向PC机下发Action信息
** input parameters: action
#define VP_ACT_HEART          0
#define VP_ACT_CHUHUO         1
#define VP_ACT_PAYOUT         2
#define VP_ACT_ADMIN          5
#define VP_ACT_ONLINE         6

** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ActionRPTAPI(uint8_t action,uint8_t value,uint8_t second,uint8_t column,uint8_t type,uint32_t cost,uint32_t payAllMoney)
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC://add by yoc 2013.12.16
			if(action == 1)//出货
			{
				
			}
			else if(action == 2)//找零
			{
			}
			else if(action == 5)//维护
			{
				if(value == 1)//进入维护模式
				{
					zh_task3_post(MBOX_VMC_ZH_WEIHU);
				}
				else
				{
					zh_task3_post(MBOX_VMC_ZH_NORMAL);
				}
			}						
			break;	
		case UBOX_PC:			
			TracePC("\r\n MiddUboxAction Post");
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACTION;
			MsgUboxPack[g_Ubox_Index].action = action;
			MsgUboxPack[g_Ubox_Index].value = value;
			MsgUboxPack[g_Ubox_Index].second = second;
			MsgUboxPack[g_Ubox_Index].Column	= (column/10-1)*8+column%10;
			MsgUboxPack[g_Ubox_Index].Type	= type;
			MsgUboxPack[g_Ubox_Index].costMoney = cost;
			MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;
		case CRUBOX_PC:			
			TracePC("\r\n MiddUboxAction Post");
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACTION;
			MsgUboxPack[g_Ubox_Index].action = 2;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
	}
}


/*********************************************************************************************************
** Function name:       ChangeMoneyInd
** Descriptions:        PC指示找零硬币
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangeMoneyInd(uint32_t changeMoney,uint8_t Type,uint32_t payAllMoney)
{	
	uint32_t backmoney;
	unsigned char ComStatus;
	
	if(changeMoney)
	{
		if(SystemPara.PcEnable!=CRUBOX_PC)
		{
			ActionRPTAPI(2,0,30,0,Type,changeMoney,payAllMoney);
			OSTimeDly(OS_TICKS_PER_SEC*2);
		}
		ComStatus = ChangerDevPayoutAPI(changeMoney,&backmoney);				
	}
	else
	{
		ComStatus = 1;
	}
	//找零失败
	if(!ComStatus)
	{	
		PayoutRPTAPI(0,Type,changeMoney-backmoney,backmoney,payAllMoney);
	}
	//找零成功
	else
	{
		PayoutRPTAPI(0,Type,changeMoney,0,payAllMoney);
	}	
	 	
}

/*********************************************************************************************************
** Function name:       ChangerRecyclerInd
** Descriptions:        纸币找零器找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangerRecyclerInd(uint32_t changeMoney,uint8_t Type,uint32_t payAllMoney)
{
	uint8_t ComStatus;
	uint32_t  backmoney=0;

	//设置到全局变量中
	changeMoneyInd=changeMoney;
	TypeInd=Type;
	payAllMoneyInd=payAllMoney;
	
	if(changeMoney)
	{
		ComStatus=BillRecyclerPayoutValueExpanseAPI(changeMoney,&backmoney);		
	}
	else
	{
		ComStatus = 1;
	}
	if(SystemPara.BillValidatorType==FS_BILLRECYCLER)
	{
		//找零失败
		if(!ComStatus)
		{	
			PayoutRPTAPI(0,Type,backmoney,changeMoney-backmoney,payAllMoney);
		}
		//找零成功
		else
		{
			PayoutRPTAPI(0,Type,backmoney,changeMoney-backmoney,payAllMoney);
		}	
	}
	
}

/*********************************************************************************************************
** Function name:       ChangeMoneyIndFail
** Descriptions:        PC指示找零硬币失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangeMoneyIndFail(uint32_t changeMoney,uint32_t remainMoney,uint8_t Type,uint32_t payAllMoney)
{		
	PayoutRPTAPI(0,Type,changeMoney,remainMoney,payAllMoney);	 	
}


/*********************************************************************************************************
** Function name:       ButtonRPTAPI
** Descriptions:        向PC上报按键信息
** input parameters:    type=1小键盘选择货道,4退币,0游戏按键
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ButtonRPTAPI(uint8_t type,unsigned char Logicnum,unsigned char binnum)
{	
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC://add by yoc 2013.12.16
			if(type == 1)
			{
				vmc_zh_mbox[ZhIndex].cur_logic_no =  Logicnum;
				zh_task3_post(MBOX_VMC_ZH_BUTTON);
			}			
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxButton");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_BUTTON;	
			MsgUboxPack[g_Ubox_Index].Type	= type;
			if(type==1)
			{
				MsgUboxPack[g_Ubox_Index].Column =  Logicnum;
				MsgUboxPack[g_Ubox_Index].device =  binnum;
			}
			else
			{
				MsgUboxPack[g_Ubox_Index].Column =  0;
				MsgUboxPack[g_Ubox_Index].device =  0;
			}
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
		case CRUBOX_PC:
			TracePC("\r\n MiddUboxButton");	
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_BUTTON;							
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;	
	}

}



/*********************************************************************************************************
** Function name:       VendoutRPTAPI
** Descriptions:        向PC上报出货信息
** input parameters:    status=0出货成功,2，出货失败
                        column实际出货货道  
                        type出货类型
                        cost商品的花费
                        payAllMoney出货完成后，用户投币余额
                        columnLeft本货道剩余商品个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void VendoutRPTAPI( unsigned char status, unsigned char Binnum,unsigned char column, unsigned char type, uint32_t cost,uint32_t payAllMoney, unsigned char columnLeft )
{	
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC://add by yoc 2013.11.13
			vmc_zh_mbox[ZhIndex].cur_trade_state = status;
			vmc_zh_mbox[ZhIndex].cur_logic_no = column;
			vmc_zh_mbox[ZhIndex].pay_mode = (type == 0) ? 0x01 : type;	
			vmc_zh_mbox[ZhIndex].cur_goods_price = cost;
			vmc_zh_mbox[ZhIndex].trade_index =LogPara.LogDetailPage;
			zh_task3_post(MBOX_VMC_ZH_TRADE);		
			break;	
		case UBOX_PC:
			TracePC("\r\n %dMiddUboxVendout",OSTimeGet());	
			if(LogPara.offLineFlag == 1)
			{		
				if(status == 0)
				{
					LogPara.offLineMoney = LogPara.offLineMoney + cost;
					LogPara.offLineNum++;
				}
			}	
			else
			{
				MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_VENDOUT;	
				MsgUboxPack[g_Ubox_Index].status	= status;
				MsgUboxPack[g_Ubox_Index].device	= Binnum;
				MsgUboxPack[g_Ubox_Index].Column	= column;
				MsgUboxPack[g_Ubox_Index].Type	= type;
				MsgUboxPack[g_Ubox_Index].costMoney = cost;
				MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
				MsgUboxPack[g_Ubox_Index].ColumnSum =  columnLeft;			
				OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
				UpdateIndex();
			}
			OSTimeDly(OS_TICKS_PER_SEC/100);
			break;		
	}

}





/*********************************************************************************************************
** Function name:       ColumnCheckError
** Descriptions:        货道是否可以出货
** input parameters:    1故障,0正常
** output parameters:   无
** Returned value:      0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用
*********************************************************************************************************/
uint8_t ColumnCheckError(uint8_t method,uint8_t Binnum,uint8_t Column,uint16_t *columnNo)
{ 
	uint8_t retcol=0;
	//这只是格子柜机，没有弹簧货道
	if(SystemPara.GeziDeviceType==1)
	{
		*columnNo = 0;
		retcol = 5;
	}
	else if(method==1)
	{
		*columnNo = 0;
		*columnNo = ChannelGetSelectColumn(Binnum,2,Column);
		if(*columnNo == 0)
		{
			retcol = 5;
		}
		else
		{
			*columnNo = Binnum * 100 + *columnNo;
			retcol = 0;
		}
	}
	else if(method==2)
	{
		
		*columnNo = Binnum * 100 + Column;
		if(Binnum<3)
		{
			//0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用			
			retcol = ChannelCheckIsOk(*columnNo%100,*columnNo/100);
		}
		else
		{			
			retcol = 0;
		}
	}
	else
	{
		retcol = 5;
	}
	return retcol;
}


/*********************************************************************************************************
** Function name:     	AdminRPTAPI
** Descriptions:	    进入AdminRPT配置
** input parameters:    adminType:1加满全部货道,2货道存货数量修改,3补硬币完成,4全系统同步,8按层加满,9取纸币完成,10补货完成
** output parameters:   无
** Returned value:      1ACK,0NAK
*********************************************************************************************************/
uint8_t AdminRPTAPI(uint8_t adminType,uint8_t Column,uint8_t ColumnSum)
{
	MessageUboxPCPack *AccepterUboxMsg;
	unsigned char ComStatus;
	
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			//Trace("\r\n MiddPCCommInit"); 		
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUbox Admin=%d,%d,%d",adminType,Column,ColumnSum);
			
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ADMIN;	
			MsgUboxPack[g_Ubox_Index].Type	= adminType;
			//按货道补货
			if(adminType==2)
				MsgUboxPack[g_Ubox_Index].Column	= (Column/10-1)*8+Column%10;
			//按层补货
			else
				MsgUboxPack[g_Ubox_Index].Column	= Column;
			MsgUboxPack[g_Ubox_Index].ColumnSum	= ColumnSum;
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);
			//取得返回值
			AccepterUboxMsg = OSMboxPend(g_Ubox_VMCTOPCBackCMail,OS_TICKS_PER_SEC*10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				switch(AccepterUboxMsg->PCCmd)
				{
					case MBOX_VMCTOPC_ACK:
						TracePC("\r\n MiddUbox Admin ACK"); 
						if(adminType == 10)
						{
							//1发送status指令
							StatusRPTAPI();
							OSTimeDly(OS_TICKS_PER_SEC*3);
							//1发送InfoRpt.Type=6指令
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_INFORPT;		
							MsgUboxPack[g_Ubox_Index].Type = 6;		
							OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
							OSTimeDly(OS_TICKS_PER_SEC/100);
						}
						return 1; 						
					case MBOX_VMCTOPC_NAK:
						TracePC("\r\n MiddUbox Admin NAK"); 
						return 0;  							
				}
			}
			break;		
	}
	return 0;
}

/*********************************************************************************************************
** Function name:     	AdminRPTSIMPLEAPI
** Descriptions:	    进入AdminRPT配置
** input parameters:    adminType:1加满全部货道,2按层加满,3按货道加满，4最长出货时间，5自动退币时间
** output parameters:   无
** Returned value:      1ACK,0NAK
*********************************************************************************************************/
uint8_t AdminRPTSIMPLEAPI(uint8_t adminType,uint8_t Column,uint8_t ColumnSum)
{
	MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	unsigned char ComStatus,result=0;
	
	switch(SystemPara.PcEnable)
	{		
		case SIMPUBOX_PC:
			TracePC("\r\n MiddUbox Admin=%d,%d,%d",adminType,Column,ColumnSum);

			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].VMCTOPCCmd = MBOX_SIMPLEVMCTOPC_ADMINRPT;
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admintype=adminType;
			//1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
			switch(adminType)
			{
				case 1:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumn=0;
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumnsum=0;
					break;
				case 2:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumn=Column;
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumnsum=0;
					break;	
				case 3:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumn=Column;
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumnsum=ColumnSum;
					break;
				case 4:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumnsum=ColumnSum;
					break;
				case 5:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumnsum=ColumnSum;
					break;	
			}
			OSQPost(g_SIMPLEUbox_VMCTOPCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
			UpdateSIMPLEIndex();
			//取得返回值
			AccepterSIMPLEUboxMsg = OSMboxPend(g_SIMPLEUbox_VMCTOPCBackCMail,OS_TICKS_PER_SEC*200,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{				
				switch(AccepterSIMPLEUboxMsg->PCTOVMCCmd)
				{
					case MBOX_SIMPLEVMCTOPC_RESULTIND:
						TracePC("\r\n MiddUbox AdminResult=%d",AccepterSIMPLEUboxMsg->adminresult);
						if(AccepterSIMPLEUboxMsg->adminresult==0)
							result=1;
						break;
				}		
			}
			
			break;		
	}
	return result;
}

/*********************************************************************************************************
** Function name:     	GetAdminSIMPLEAPI
** Descriptions:	    得到AdminRPT参数
** input parameters:    adminType:3按货道加满，4最长出货时间，5自动退币时间
** output parameters:   无
** Returned value:      1ACK,0NAK
*********************************************************************************************************/
uint8_t GetAdminSIMPLEAPI(uint8_t adminType,uint8_t Column)
{
	MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	unsigned char ComStatus,result=0;
	
	switch(SystemPara.PcEnable)
	{		
		case SIMPUBOX_PC:
			TracePC("\r\n MiddUbox GetAdmin=%d,%d",adminType,Column);

			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].VMCTOPCCmd = MBOX_SIMPLEVMCTOPC_GETADMIN;
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admintype=adminType;
			//3单货道补货，4最长出货时间，5自动退币时间
			switch(adminType)
			{
				case 3:
					MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].admincolumn=Column;					
					break;					
			}
			OSQPost(g_SIMPLEUbox_VMCTOPCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
			UpdateSIMPLEIndex();
			//取得返回值
			AccepterSIMPLEUboxMsg = OSMboxPend(g_SIMPLEUbox_VMCTOPCBackCMail,OS_TICKS_PER_SEC*200,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{				
				switch(AccepterSIMPLEUboxMsg->PCTOVMCCmd)
				{
					case MBOX_SIMPLEVMCTOPC_RESULTIND:
						TracePC("\r\n MiddUbox GetAdminResult=%d",AccepterSIMPLEUboxMsg->adminresult);
						if(AccepterSIMPLEUboxMsg->adminresult==0)
							result=1;
						break;
				}		
			}
			
			break;		
	}
	return result;
}



/*********************************************************************************************************
** Function name:       ButtonSIMPLERPTAPI
** Descriptions:        向PC上报按键信息，针对新的友宝通讯协议
** input parameters:    type=1小键盘选择货道,4退币,0游戏按键
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ButtonSIMPLERPTAPI(uint8_t channel_id)
{	
	switch(SystemPara.PcEnable)
	{		
		case SIMPUBOX_PC:
			TracePC("\r\n MiddUboxButton=%d",channel_id);	
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].VMCTOPCCmd = MBOX_SIMPLEVMCTOPC_BUTTONRPT;
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].btnchannel_id=channel_id;
			OSQPost(g_SIMPLEUbox_VMCTOPCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
			UpdateSIMPLEIndex();
			//OSTimeDly(OS_TICKS_PER_SEC/100);
			break;		
	}

}


/*********************************************************************************************************
** Function name:       PollAPI
** Descriptions:        轮询PC机上发的操作命令
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PollAPI(uint32_t payAllMoney)
{
	MessageUboxPCPack *AccepterUboxMsg;
	unsigned char ComStatus;
	char disp[5]={0, 0, 0, 0, 0};
	//uint8_t recoin;
	unsigned char ComReturn = 0;
	uint16_t columnNo=0;
	uint8_t vendrpt=0;
	MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	
	
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC://add by yoc 2013.12.16
			zh_task3_pend(payAllMoney); 		
			break;	
		case UBOX_PC:
			//TracePC("\r\n MiddUboxPayout");	
			AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCQ,10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{				
				switch(AccepterUboxMsg->PCCmd)
				{
					//payout_ind找零
					case MBOX_PCTOVMC_PAYOUTIND:
						TracePC("\r\n MiddUbox payoutInd=%d,type=%d",AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type); 
						//硬币器是否故障
						if(ChangerIsErr()==1)
						{
							TracePC("\r\n MiddUbox payoutErr");
							ComReturn = 0;
						}	
						else
						{
							//零钱是否够找
							if(SystemPara.CoinChangerType == MDB_CHANGER)
							{
								//ComReturn = MDBchange(resultdisp, &recoin, AccepterUboxMsg->changeMoney);//检查这些剩余的金额是否可以找零
								ComReturn = 1;
							}	
							else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
							{
								ComReturn = 1;
							}
						}
						//如果可以找零
                	    if(ComReturn == 1)
                	    {
							//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
							//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
							//UpdateIndex();
							ChangeMoneyInd(AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type,payAllMoney);
                	    }
						else
						{
							//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
							//UpdateIndex();
                	    }	
						
						break;		
					//cost_ind扣款
					case MBOX_PCTOVMC_COSTIND:
						TracePC("\r\n MiddUbox costInd=%d,type=%d",AccepterUboxMsg->costMoney,AccepterUboxMsg->Type);
						//ACK
						//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
						//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
						//UpdateIndex();
							
						//1.系统进入故障状态时，返回NAK_RPT  
						if(IsErrorState())
						{
							TracePC("\r\n MiddUbox vendE3");
							ComReturn = 24;		
						}	
						//2.用户投币金额小于扣款金额时，返回NAK_RPT    
					    else if( AccepterUboxMsg->costMoney > payAllMoney ) 
						{
							TracePC("\r\n MiddUbox vendE5");
							ComReturn = 0;
						}
						else
						{
							/*
							//3零钱是否够扣
							if(SystemPara.CoinChangerType == MDB_CHANGER)
							{
								//ComReturn = MDBchange(resultdisp, &recoin, payAllMoney - (AccepterUboxMsg->costMoney));//检查这些剩余的金额是否可以找零
								ComReturn = 1;
							}	
							else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
							{
								ComReturn = 1;
							}
							*/
							//读卡器有开启并且有插卡金额
							if((SystemPara.CashlessDeviceType != OFF_READERACCEPTER)&&(GetReaderAmount()))
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}	
							//3无找零器
							else if(SystemPara.CoinChangerType == OFF_CHANGER)
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}
							else
							{
								//4压抄是否成功,是否可以支付
								if(StackMoneyInd(AccepterUboxMsg->costMoney)==1)
								{
									ComReturn = 1;
									TracePC("\r\n Stack_CHANGER=1");
								}
								else
								{
									ComReturn = 26;
									TracePC("\r\n MiddUbox vendE7");
								}	
							}
						}
						//3如果可以扣款
                	    if(ComReturn == 1)
                	    {							
							//4扣款
							CostReaderRPT(AccepterUboxMsg->costMoney);
							CostMoneyInd(AccepterUboxMsg->costMoney);	
							LogCostAPI(AccepterUboxMsg->costMoney);	
							//5上报cost_report
							CostRPTAPI(AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,payAllMoney-(AccepterUboxMsg->costMoney));
                	    }
						else
						{
							//5上报cost_report
							CostRPTAPI(AccepterUboxMsg->Type,0,GetAmountMoney());
                	    }							
						break;		
					//vendout_ind出货
					case MBOX_PCTOVMC_VENDOUTIND:
						TracePC("\r\n %dMiddUbox vendoutInd=%d,%d,%d,%ld",OSTimeGet(),AccepterUboxMsg->method,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney);
						//发送ACK
						//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
						//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
						//UpdateIndex();
						
						vendrpt=ColumnCheckError(AccepterUboxMsg->method,AccepterUboxMsg->device,AccepterUboxMsg->Column,&columnNo);
						//1.下发的货道或者商品id对应的货道无法出货时，返回NAK_RPT  
						if(vendrpt)
						{
							TracePC("\r\n MiddUbox vendE4=%ld",columnNo);
							//0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用
							switch(vendrpt)
							{
								case 1:
									ComReturn = 16;	
									break;
								case 3:
									ComReturn = 17;	
									break;
								case 4:
									ComReturn = 18;	
									break;
								case 5:
									ComReturn = 19;	
									break;
								case 6:
									ComReturn = 20;	
									break;	
								case 7:
									ComReturn = 21;	
									break;	
									
							}								
						}
						//2.type和cost的关系不符合返回NAK_RPT    
						else if(( AccepterUboxMsg->Type > 0 )&&(AccepterUboxMsg->costMoney > 0))
						{
							TracePC("\r\n MiddUbox vendE1");
							ComReturn = 22;		
						}
						else if(( AccepterUboxMsg->Type == 0 )&&(AccepterUboxMsg->costMoney == 0))
						{
							TracePC("\r\n MiddUbox vendE2");
							ComReturn = 23;		
						}
						//3.系统进入故障状态时，返回NAK_RPT  
						else if(IsErrorState())
						{
							TracePC("\r\n MiddUbox vendE3");
							ComReturn = 24;		
						}														
						//4.用户投币金额小于扣款金额时，返回NAK_RPT    
					    else if( AccepterUboxMsg->costMoney > payAllMoney ) 
						{
							TracePC("\r\n MiddUbox vendE5");
							ComReturn = 25;
						}
						else
						{
							/*
							//5零钱是否够扣
							if(SystemPara.CoinChangerType == MDB_CHANGER)
							{
								//ComReturn = MDBchange(resultdisp, &recoin, payAllMoney - (AccepterUboxMsg->costMoney));//检查这些剩余的金额是否可以找零
								ComReturn = 1;
								//if(ComReturn == 0)
								//{
								//	TracePC("\r\n MiddUbox vendE6");
								//}
								TracePC("\r\n MDB_CHANGER=1");
							}	
							else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
							{
								ComReturn = 1;
								TracePC("\r\n HOPPER_CHANGER=1");
							}
							//6压抄是否成功,是否可以支付
							if(StackMoneyInd(AccepterUboxMsg->costMoney)==1)
							{
								ComReturn = 1;
								TracePC("\r\n Stack_CHANGER=1");
							}
							else
							{
								ComReturn = 0;
								TracePC("\r\n MiddUbox vendE7");
							}	
							*/
							//读卡器有开启并且有插卡金额
							if((SystemPara.CashlessDeviceType != OFF_READERACCEPTER)&&(GetReaderAmount()))
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}
							//5无找零器
							else if(SystemPara.CoinChangerType == OFF_CHANGER)
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}
							else
							{
								//6压抄是否成功,是否可以支付
								if(StackMoneyInd(AccepterUboxMsg->costMoney)==1)
								{
									ComReturn = 1;
									TracePC("\r\n Stack_CHANGER=1");
								}
								else
								{
									ComReturn = 26;
									TracePC("\r\n MiddUbox vendE7");
								}	
							}
						}
						//3如果可以出货
                	    if(ComReturn == 1)
                	    {
                	    	TracePC("\r\n %dMiddUbox vend=%ld",OSTimeGet(),columnNo);
							//4出货
							VendoutInd(columnNo,AccepterUboxMsg->costMoney,AccepterUboxMsg->Type);
                	    }
						else
						{
							//4出货失败
							VendoutIndFail(columnNo,AccepterUboxMsg->costMoney,AccepterUboxMsg->Type,ComReturn);
                	    }	
                	    
						break;
					//reset_ind重新复位
					case MBOX_PCTOVMC_RESETIND:
						TracePC("\r\n MiddUbox resetInd");
						ResetInd();
						ComReturn = 1;
						/*
						//3如果可以出货
                	    if(ComReturn == 1)
                	    {
                	    	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
							OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);	
							UpdateIndex();
                	    }
						else
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
                	    }
						*/	
						break;
					//status_ind得到设备状态
					case MBOX_PCTOVMC_STATUSIND:
						//TracePC("\r\n MiddUbox status_ind");
						
						//ComReturn = 1;
						//OSTimeDly(OS_TICKS_PER_SEC/10);
						//StatusRPTAPI();
						/*
						//3如果可以得到设备状态
                	    if(ComReturn == 1)
                	    {
                	    	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
							OSMboxPost(g_Ubox_PCTOVMCBackCMail,&MsgUboxPack[g_Ubox_Index]);	
							UpdateIndex();
							OSTimeDly(OS_TICKS_PER_SEC*2);
							StatusRPTAPI();
                	    }
						else
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							OSMboxPost(g_Ubox_PCTOVMCBackCMail,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
                	    }		
                	    */
						break;	
					//control_ind控制指令
					case MBOX_PCTOVMC_CONTROLIND:
						TracePC("\r\n MiddUbox controlInd");
						ComReturn = 1;
						/*
						//OSTimeDly(OS_TICKS_PER_SEC*4);
						//3如果可以出货
                	    if(ComReturn == 1)
                	    {
                	    	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
							OSMboxPost(g_Ubox_PCTOVMCBackCMail,&MsgUboxPack[g_Ubox_Index]);	
							UpdateIndex();
                	    }
						else
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							OSMboxPost(g_Ubox_PCTOVMCBackCMail,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
                	    }
                	    */
						switch(AccepterUboxMsg->Type)
						{
							case 2://开启收币模块
								TracePC("\r\n MiddUbox billcoin=%d",AccepterUboxMsg->value);
								if(AccepterUboxMsg->value==1)
								{
									BillCoinCtr(1,1,0);
								}
								else if(AccepterUboxMsg->value==0)
								{
									BillCoinCtr(2,2,0);
								}
								break;
							case 3://柜内设备控制
								TracePC("\r\n MiddUbox cabinet=%d,device=%d,value=%ld",AccepterUboxMsg->Control_device,AccepterUboxMsg->Control_type,AccepterUboxMsg->wenduvalue);
								if(AccepterUboxMsg->Control_type==1)//制冷
								{
									acdc_cpr_API(AccepterUboxMsg->Control_device,AccepterUboxMsg->wenduvalue);
								}
								else if(AccepterUboxMsg->Control_type==2)//加热
								{
									acdc_hot_API(AccepterUboxMsg->Control_device,AccepterUboxMsg->wenduvalue);
								}
								if(AccepterUboxMsg->Control_type==3)//照明
								{
									acdc_led_API(AccepterUboxMsg->Control_device,AccepterUboxMsg->wenduvalue);
								}
								if(AccepterUboxMsg->Control_type==4)//除臭
								{
									acdc_chuchou_API(AccepterUboxMsg->Control_device,AccepterUboxMsg->wenduvalue);
								}
								break;
							case 6://找零
								TracePC("\r\n %dMiddUbox ctrpayout",OSTimeGet());
								TuiMoneyInd();
								break;
							case 16://游戏灯开关
								TracePC("\r\n MiddUbox Ctrgame=%d",AccepterUboxMsg->value);
								if(AccepterUboxMsg->value== 1)
									TracePC("\r\n MiddUbox ctrgameon");
								else if(AccepterUboxMsg->value== 0)
									TracePC("\r\n MiddUbox ctrgameoff");
								break;	
							case 17://时钟同步
								TracePC("\r\n MiddUbox ctrclock=%ld,%d,%d,%d,%d,%d,%d",AccepterUboxMsg->RTCyear,AccepterUboxMsg->RTCmonth,AccepterUboxMsg->RTCday,
									AccepterUboxMsg->RTChour,AccepterUboxMsg->RTCmin,AccepterUboxMsg->RTCsecond,AccepterUboxMsg->RTCweek);
								RTCData.year=AccepterUboxMsg->RTCyear;
								RTCData.month=AccepterUboxMsg->RTCmonth;
								RTCData.day=AccepterUboxMsg->RTCday;
								RTCData.hour=AccepterUboxMsg->RTChour;
								RTCData.minute=AccepterUboxMsg->RTCmin;
								RTCData.second=AccepterUboxMsg->RTCsecond;
								RTCData.week=AccepterUboxMsg->RTCweek;
								RTCSetTime(&RTCData);
								break;
							case 18://金额数据比例系数关系
								TracePC("\r\n MiddUbox ctrscale=%d",AccepterUboxMsg->value);
								if(SystemPara.DecimalNum != AccepterUboxMsg->value)
								{
									TracePC("\r\n MiddUbox ctrscaleError");
									SetScaleError(1);
								}
								else
								{
									SetScaleError(0);
								}	
								break;
							case 19:
								TracePC("\r\n MiddUbox Init");
								globalSys.pcInitFlag = 1;
								break;
							case 21:
								TracePC("\r\n MiddUbox bin=%d,ctr=%d,value=%d",AccepterUboxMsg->Control_device,AccepterUboxMsg->Control_type,AccepterUboxMsg->value);
								switch(AccepterUboxMsg->Control_type)
								{
									case 0x53:
										if(AccepterUboxMsg->value)
											ChannelAPIProcess(0,HEFANGUI_JIAREKAI,AccepterUboxMsg->Control_device);	
										else
											ChannelAPIProcess(0,HEFANGUI_JIAREGUAN,AccepterUboxMsg->Control_device);		
										break;
									case 0x55:
										if(AccepterUboxMsg->value)
											ChannelAPIProcess(0,HEFANGUI_ZHILENGKAI,AccepterUboxMsg->Control_device);	
										else
											ChannelAPIProcess(0,HEFANGUI_ZHILENGGUAN,AccepterUboxMsg->Control_device);		
										break;
									case 0x56:
										if(AccepterUboxMsg->value)
											ChannelAPIProcess(0,HEFANGUI_ZHAOMINGKAI,AccepterUboxMsg->Control_device);	
										else
											ChannelAPIProcess(0,HEFANGUI_ZHAOMINGGUAN,AccepterUboxMsg->Control_device);		
										break;	
								};
								break;	
						}
						break;	
					//getInfo控制指令
					case MBOX_PCTOVMC_INFOIND:
						TracePC("\r\n MiddUbox InfoInd=%d",AccepterUboxMsg->Type);
						/*
						 VP_INFO_TOTALVALUE    3
						 VP_INFO_CLOCK         5
						 VP_INFO_CHUHUO        6
						 VP_INFO_HUODAO        10
						 VP_INFO_POSITION      11
						 VP_INFO_SALEPRICE     12
						 VP_INFO_VMC           13
						 VP_INFO_VER           14
						 VP_INFO_HARD          15
						 VP_INFO_BILL          16
						 VP_INFO_COIN          17
						 VP_INFO_COMP          18
						 VP_INFO_ERR           23
						 24:盒饭柜查询状态上报
						 25:非普通柜查询状态上报
						*/
						ComReturn = 1;
						//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
						//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);	
						//UpdateIndex();
						//OSTimeDly(OS_TICKS_PER_SEC/100);
						
						MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_INFORPT;		
						MsgUboxPack[g_Ubox_Index].Type = AccepterUboxMsg->Type;
						MsgUboxPack[g_Ubox_Index].payAllMoney = payAllMoney;
						if(AccepterUboxMsg->Type==24)
						{
							ChannelAPIProcess(0,HEFANGUI_CHAXUN,AccepterUboxMsg->Control_device);	
							MsgUboxPack[g_Ubox_Index].Control_device = AccepterUboxMsg->Control_device;
							MsgUboxPack[g_Ubox_Index].Control_Huodao = ChaXunRst[0];
							MsgUboxPack[g_Ubox_Index].value =  ChaXunRst[1];
							
						}
						else if(AccepterUboxMsg->Type==25)
						{
							MsgUboxPack[g_Ubox_Index].cabinetNums = special_cabinet_info(MsgUboxPack[g_Ubox_Index].cabinetdata);
						}
						TracePC("\r\n MiddUbox Totalvalue=%d,%ld",MsgUboxPack[g_Ubox_Index].Type,MsgUboxPack[g_Ubox_Index].payAllMoney);
						OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
						UpdateIndex();
						OSTimeDly(OS_TICKS_PER_SEC/100);		
						
						//ComReturn = 1;
						//3如果不可以操作
                	    /*if(ComReturn != 1)                	    
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
                	    }*/
							
						break;		
				}
			}	
			break;	
		case SIMPUBOX_PC:
			AccepterSIMPLEUboxMsg = OSQPend(g_SIMPLEUbox_PCTOVMCQ,10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{				
				switch(AccepterSIMPLEUboxMsg->PCTOVMCCmd)
				{
					//出货
					case MBOX_SIMPLEPCTOVMC_VENDIND:
						TracePC("\r\n MiddSIMPLEUbox VENDIND=%d",AccepterSIMPLEUboxMsg->channel_id);
						vendrpt=ColumnCheckError(1,1,AccepterSIMPLEUboxMsg->channel_id,&columnNo);
						//1.下发的货道或者商品id对应的货道无法出货时，返回NAK_RPT  
						if(vendrpt)
						{
							TracePC("\r\n MiddSIMPLEUbox vendE4=%ld,vendrpt=%d",columnNo,vendrpt);
							//0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用
							switch(vendrpt)
							{
								case 1:
									ComReturn = 16;	
									break;
								case 3:
									ComReturn = 17;	
									break;
								case 4:
									ComReturn = 18;	
									break;
								case 5:
									ComReturn = 19;	
									break;
								case 6:
									ComReturn = 20;	
									break;	
								case 7:
									ComReturn = 21;	
									break;	
									
							}								
						}						
						//3.系统进入故障状态时，返回NAK_RPT  
						else if(IsErrorState())
						{
							TracePC("\r\n MiddUbox vendE3");
							ComReturn = 24;		
						}
						else
						{							
							ComReturn = 1;									
						}
						//3如果可以出货
                	    if(ComReturn == 1)
                	    {
                	    	TracePC("\r\n %dMiddUbox vend=%ld",OSTimeGet(),columnNo);
							//4出货
							vendrpt=VendoutSIMPLEInd(columnNo);
							//出货成功
							if(vendrpt==1)
								MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_result=0;
							//出货失败
							else
								MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_result=1;
                	    }
						else
						{
							//4出货失败
							MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_result=1;
                	    }	
						TracePC("\r\n %dMiddUbox vendout=%d",OSTimeGet(),MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_result);
						//返回出货结果
						MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].VMCTOPCCmd = MBOX_SIMPLEVMCTOPC_VENDOUT;						       
					    OSMboxPost(g_SIMPLEUbox_PCTOVMCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);
						UpdateSIMPLEIndex();
						break;
					case MBOX_SIMPLEPCTOVMC_DISPLAYIND:
						//0用户余额，1时间，2文本，3商品单价 
						switch(AccepterSIMPLEUboxMsg->distype)
						{
							case 0:
								TracePC("\r\n MiddUbox money=%ld",AccepterSIMPLEUboxMsg->payInMoney);
								GetmoneySIMPLEInd(AccepterSIMPLEUboxMsg->payInMoney);
								break;
							case 3:
								TracePC("\r\n MiddUbox price=%ld",AccepterSIMPLEUboxMsg->payInMoney);
								PriceSIMPLEInd(AccepterSIMPLEUboxMsg->payInMoney);
								break;								
							case 2:
								disp[0]=AccepterSIMPLEUboxMsg->disp[0];
								disp[1]=AccepterSIMPLEUboxMsg->disp[1];
								disp[2]=AccepterSIMPLEUboxMsg->disp[2];
								disp[3]=AccepterSIMPLEUboxMsg->disp[3];
								disp[4]=AccepterSIMPLEUboxMsg->disp[4];
								TracePC("\r\n MiddUbox disptxt=%c%c%c%c%c",disp[0],disp[1],disp[2],disp[3],disp[4]);
								TxtSIMPLEInd(disp);
								break;
						}
						break;
				}
			}	
			break;
		case CRUBOX_PC:
			//TracePC("\r\n MiddUboxPayout");	
			AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCQ,10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{				
				switch(AccepterUboxMsg->PCCmd)
				{
					case MBOX_PCTOVMC_CONTROLIND:
						TracePC("\r\n MiddUbox controlInd");												
						switch(AccepterUboxMsg->Type)
						{
							case 0://开启收币模块
								TracePC("\r\n MiddUbox billcoin=%d",AccepterUboxMsg->value);
								if(AccepterUboxMsg->value==1)
								{
									//BillCoinCtr(1,1,0);
									BillCoinEnable(1);
								}
								else if(AccepterUboxMsg->value==0)
								{
									//BillCoinCtr(2,2,0);
									BillCoinEnable(2);
								}
								break;
						}
						break;
					case MBOX_PCTOVMC_PAYININD:
                        			StackReturnBillDev(AccepterUboxMsg->Type);                        
						break;
					//payout_ind找零
					case MBOX_PCTOVMC_PAYOUTIND:
						TracePC("\r\n MiddUbox payoutInd=%d,type=%d",AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type); 
						if(AccepterUboxMsg->Type==0)//硬币找零
						{
							//可以找零	
							if(PayoutRecyAPI(AccepterUboxMsg->Type)>(AccepterUboxMsg->changeMoney))
							{
								ChangeMoneyInd(AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type,payAllMoney);
							}
							//不可以找零
							else
							{
								ChangeMoneyIndFail(0,AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type,payAllMoney);
							}
						}
						else if(AccepterUboxMsg->Type==1)//纸币找零
						{
							//可以找零	
							if(//金额足够
								(PayoutRecyAPI(AccepterUboxMsg->Type)>=(AccepterUboxMsg->changeMoney))
								//可以用这个面值进行找币
								&&((AccepterUboxMsg->changeMoney)%SystemPara.RecyclerMoney==0)

							)
							{
								ChangerRecyclerInd(AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type,payAllMoney);
							}
							//不可以找零
							else
							{
								ChangeMoneyIndFail(0,AccepterUboxMsg->changeMoney,AccepterUboxMsg->Type,payAllMoney);
							}
						}						
						break;
					//cost_ind扣款
					case MBOX_PCTOVMC_COSTIND:
						TracePC("\r\n MiddUbox costInd=%d,type=%d",AccepterUboxMsg->costMoney,AccepterUboxMsg->Type);
						//ACK
						//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
						//OSQPost(g_Ubox_PCTOVMCBackQ,&MsgUboxPack[g_Ubox_Index]);
						//UpdateIndex();							
						//1	
						//2.用户投币金额小于扣款金额时，返回NAK_RPT    
					    if( AccepterUboxMsg->costMoney > payAllMoney ) 
						{
							TracePC("\r\n MiddUbox vendE5");
							ComReturn = 0;
						}
						else
						{
							/*
							//3零钱是否够扣
							if(SystemPara.CoinChangerType == MDB_CHANGER)
							{
								//ComReturn = MDBchange(resultdisp, &recoin, payAllMoney - (AccepterUboxMsg->costMoney));//检查这些剩余的金额是否可以找零
								ComReturn = 1;
							}	
							else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
							{
								ComReturn = 1;
							}
							*/
							//读卡器有开启并且有插卡金额
							if((SystemPara.CashlessDeviceType != OFF_READERACCEPTER)&&(GetReaderAmount()))
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}	
							//3无找零器
							else if(SystemPara.CoinChangerType == OFF_CHANGER)
							{
								ComReturn = 1;
								TracePC("\r\n OFF_CHANGER=1");
							}
							else
							{
								//4压抄是否成功,是否可以支付
								if(StackMoneyInd(AccepterUboxMsg->costMoney)==1)
								{
									ComReturn = 1;
									TracePC("\r\n Stack_CHANGER=1");
								}
								else
								{
									ComReturn = 26;
									TracePC("\r\n MiddUbox vendE7");
								}	
							}
						}
						//3如果可以扣款
		                	    	if(ComReturn == 1)
		                	    	{							
							//4扣款
							CostReaderRPT(AccepterUboxMsg->costMoney);
							CostMoneyInd(AccepterUboxMsg->costMoney);	
							LogCostAPI(AccepterUboxMsg->costMoney);	
							//5上报cost_report
							CostRPTAPI(0,AccepterUboxMsg->costMoney,payAllMoney-(AccepterUboxMsg->costMoney));
                	 		      }
						else
						{
							//5上报cost_report
							CostRPTAPI(0,0,GetAmountMoney());
                	   			 }							
						break;	
					//reset_ind重新复位
					case MBOX_PCTOVMC_RESETIND:
						TracePC("\r\n MiddUbox resetInd");
						ResetInd();						
						break;	
				}
			}	
	}
}

/*********************************************************************************************************
** Function name:       PCInitAPI
** Descriptions:        启动并签到PC机
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PCInitAPI()
{
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:
			TracePC("\r\n MiddPCCommInit");			
			//1.启动一鸣智慧的PC通讯
			zh_task3_post(MBOX_VMC_ZH_START);
			break;	
		case UBOX_PC:
			TracePC("\r\n MiddUboxCommInit");
			//1.启动友宝的PC通讯
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMC_UBOXINITDEV;			
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC);
			globalSys.pcInitFlag = 0;
			break;
		case GPRS_PC:
			zh_task3_post(MBOX_VMC_GPRS_START);
			break;
	}
	return;
}


/*********************************************************************************************************
** Function name:       PCHuamanCloseAPI
** Descriptions:        接近感应时操作
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PCHuamanCloseAPI()
{
	Buzzer();
}


