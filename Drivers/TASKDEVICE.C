/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           TASKDEVICE
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        串口2任务                   
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
#include "BillAccepter.h"
#include "CoinAccepter.h"
#include "MDBChanger.h"
#include "HopperDriver.h"
#include "MDBReader.h"



#define COINDEV_MDB			2				//当前系统识别到的硬币器类型为MDB
#define BILLDEV_MDB			2				//当前系统识别到的纸币器类型为MDB
#define CHANGEDEV_HOPPER	1				//当前系统识别到的找零器类型为Hopper
#define READERDEV_MDB		2				//当前系统识别到的读卡器类型为MDB



//extern SYSTEMPARAMETER SystemPara;

/*********************************************************************************************************
** Function name:       TaskDevice
** Descriptions:        设备任务:主要处理挂在UART2上的设备，如MDB协议,EVB协议等
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2TaskDevice(void *pvData)
{
	uint32_t Coinvalue = 0;
	uint8_t Cointype = 0;
	uint32_t Billvalue = 0;
	uint8_t Billtype = 0;
	uint32_t Readvalue = 0;
	uint32_t Changevalue = 0;
	uint8_t flag = 0,rst=0;
	uint8_t getTube = 0,getHopper = 0;
	uint8_t ReaderType = 0, cmdOpt = 0, billOpt = 0,billOptBack = 0,coinOptBack = 0;
	uint16_t  ReaderPrice;
	unsigned char PayoutNum[8];
	unsigned char ComStatus;
	uint8_t i,j;
	MessagePack *AccepterMsg;
	//当前硬币器设备的类型
	uint8_t NowCoinDev = 0;
	//当前纸币器设备的类型
	uint8_t NowBillDev = 0;
	uint8_t PayType,PayNum,PayoutHpNum;
	//当前找零器设备的类型
	uint8_t NowChangerDev = 0;
	//当前读卡器设备的类型
	uint8_t NowReaderDev = 0;
	uint8_t ReaderMoney = 0;
	//纸币找零循环器
	uint32_t RecyPayoutMoney=0;
	uint8_t RecyPayoutNum=0;

	
	OSTimeDly(OS_TICKS_PER_SEC * 2);
	pvData = pvData;		
	//切换串口为mdb模式
	MdbBusHardwareReset();
	OSTimeDly(2);	

	
	//检查纸币器控制
	AccepterMsg = OSMboxPend(g_BillMoneyMail,OS_TICKS_PER_SEC,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		if((AccepterMsg->BillBack)==MBOX_BILLINITDEV)
		{	
			TraceBill("\r\n Taskpend billinit");
			//打开纸币器
			//纸币器设备识别
			//BillDevSellect();
			//纸币器设备初始化
			if(BillDevInit()==0)
			{
				NowBillDev = BILLDEV_MDB;
			}
			else
			{
				NowBillDev = 0;
			}
		}
		else if((AccepterMsg->BillBack)==MBOX_BILLRECYCLERDEV)
		{	
			TraceBill("\r\n Taskpend recyclerinit");
			//打开纸币器
			//纸币器设备识别
			//BillDevSellect();
			//纸币器设备初始化
			if(RecyclerDevInit()==0)
			{
				NowBillDev = BILLDEV_MDB;
			}
			else
			{
				NowBillDev = 0;
			}
		}
		else
		{
			NowBillDev = 0;
		}
	}
	else
	{
		NowBillDev = 0;
	}
	OSTimeDly(7);
	

	//检查硬币器控制
	AccepterMsg = OSMboxPend(g_CoinMoneyMail,OS_TICKS_PER_SEC,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		if((AccepterMsg->CoinBack)==MBOX_COININITDEV)
		{
			//硬币器设备初始化
			if(CoinDevInit()==0)	
			{
				NowCoinDev = COINDEV_MDB;
			}
			else
			{
				NowCoinDev = 0;
			}
		}
		else
		{
			NowCoinDev = 0;
		}
	}
	else
	{
		NowCoinDev = 0;
	}
	OSTimeDly(7);
	

	//检查读卡器控制
	AccepterMsg = OSMboxPend(g_ReaderBackMoneyMail,OS_TICKS_PER_SEC,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		if((AccepterMsg->ReaderBack)==MBOX_READERINITDEV)
		{
			//读卡器设备初始化
			if(ReaderDevInit()==0)
			{
				NowReaderDev = READERDEV_MDB;
			}
			else
			{
				NowReaderDev = 0;
			}
		}
		else
		{
			NowReaderDev = 0;
		}
	}
	else
	{
		NowReaderDev = 0;
	}
	OSTimeDly(7);

	OSSemPost(g_DeviceSem);
	/**/
	while(1)
	{	
		//Trace("\r\n 1");
		TraceCoin("\r\n Taskcoin1Enable=%d\r\n",NowCoinDev);//第一次使能是否成功
		if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
		{
			//硬币器通讯函数
			flag = CoinDevProcess(&Coinvalue,&Cointype,&coinOptBack);
			if(flag == 1)
			{
				MsgAccepterPack.CoinCmd = MBOX_COINMONEY;			
				MsgAccepterPack.CoinMoney = Coinvalue;
				OSQPost(g_CoinMoneyQ,&MsgAccepterPack);
				Coinvalue = 0;
				Cointype = 0;
				//Trace("\r\n1.coin=%ld,%d\r\n",Bvalue,Btype);
				OSTimeDly(7);
			}

			if(coinOptBack == 1)
			{
				Trace("\r\n CoinReturn");
				OSMboxAccept(g_CoinMoneyBackMail);
				MsgAccepterPack.CoinBackCmd = MBOX_COINRETURN;						
				OSMboxPost(g_CoinMoneyBackMail,&MsgAccepterPack);
				coinOptBack = 0;
				OSTimeDly(7);
			}

			//轮询得到硬币器可找零硬币
			if(getTube++ >= 3)
			{
				getTube = 0;
				ChangeGetTubes();
				OSTimeDly(7);
			}	
			
			//轮询得到硬币器币斗是否移开
			CoinDevProcessExpanse();
		}
		OSTimeDly(7);	
		/**/
		//Trace("\r\n 2");
		TraceBill("\r\n Taskbil1Enable=%d\r\n",NowBillDev);//第一次使能是否成功
		if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
		{			
			//纸币器通讯函数
			flag = BillDevProcess(&Billvalue,&Billtype,billOpt,&billOptBack,RecyPayoutMoney,RecyPayoutNum);
			billOpt = 0;
			if(flag == 1)
			{
				MsgAccepterPack.BillCmd = MBOX_BILLMONEY;			
				MsgAccepterPack.BillMoney = Billvalue;
				OSQPost(g_BillMoneyQ,&MsgAccepterPack);
				TraceBill("Task1.bill=%ld,%d\r\n",Billvalue,Billtype);
				//BillDevEscrow();
				Billvalue = 0;
				Billtype = 0;
			}		

			switch(billOptBack)
			{
				case 2:
					TraceBill("\r\n TaskEscrowSuccess");
					MsgAccepterPack.BillBackCmd = MBOX_BILLESCROWSUCC;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;					
				case 1:
					TraceBill("\r\n TaskEscrowFail");
					MsgAccepterPack.BillBackCmd = MBOX_BILLESCROWFAIL;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;	
				case 4:
					TraceBill("\r\n TaskReturnSuccess");
					MsgAccepterPack.BillBackCmd = MBOX_BILLRETURNSUCC;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;					
				case 3:
					TraceBill("\r\n TaskReturnFail");
					MsgAccepterPack.BillBackCmd = MBOX_BILLRETURNFAIL;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;
				case 6:
					TraceBill("\r\n TaskRecPayoutSuccess");
					MsgAccepterPack.BillBackCmd = MBOX_BILLRECYPAYOUTSUCC;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;					
				case 5:
					TraceBill("\r\n TaskRecPayoutFail");
					MsgAccepterPack.BillBackCmd = MBOX_BILLRECYPAYOUTFAIL;						
					OSMboxPost(g_BillMoneyBackMail,&MsgAccepterPack);
					billOptBack = 0;
					break;		
				default:
					billOptBack = 0;
					break;	
			}
		}
		OSTimeDly(7);

		//Hopper找零器通讯函数
		if(NowChangerDev == CHANGEDEV_HOPPER)
		{
			//轮询得到硬币器可找零硬币
			if(getHopper++ >= 10)
			{
				getHopper = 0;
				HopperAccepter_GetState(EVBDEV_HP1);
				HopperAccepter_GetState(EVBDEV_HP2);
				HopperAccepter_GetState(EVBDEV_HP3);
			}
		}
		//GOC货道故障自检
		if(stChannelErrForm.GOCErrFlag[0]==1)
			ChannelHandle(CHANNEL_CHECKSTATE,11,1,1);
		if(stChannelErrForm.GOCErrFlag[1]==1)
			ChannelHandle(CHANNEL_CHECKSTATE,11,1,2);
		//TraceChannel("\r\nTaskGOC1=%d   GOC2=%d",stChannelErrForm.GOCErrFlag[0],stChannelErrForm.GOCErrFlag[1]);
		//TraceChannel("\r\nTaskhuodao1=%d   huodao2=%d",stChannelErrForm.HuoDaoBanFlag[0],stChannelErrForm.HuoDaoBanFlag[1]);
		OSTimeDly(7);
		TraceReader("\r\n Taskread1Enable=%d\r\n",NowReaderDev);//第一次使能是否成功
		if(SystemPara.CashlessDeviceType==MDB_READERACCEPTER)
		{
			//读卡器通讯函数
			ReaderDevProcess(&Readvalue,cmdOpt,ReaderPrice,&ReaderType);
			cmdOpt = 0;
			if((Readvalue>0)&&(ReaderMoney == 0))
			{
				TraceReader("\r\n Task +ReaderMoney=%ld\r\n",Readvalue);
				MsgAccepterPack.ReaderCmd = MBOX_READERMONEY;			
				MsgAccepterPack.ReaderMoney = Readvalue;
				OSMboxPost(g_ReaderMoneyMail,&MsgAccepterPack);
				ReaderMoney = 1;				
			}
			else if(Readvalue == 0)
			{
				if(ReaderMoney == 1)
				{
					MsgAccepterPack.ReaderCmd = MBOX_READERMONEYOUT;			
					MsgAccepterPack.ReaderMoney = 0;
					OSMboxPost(g_ReaderMoneyMail,&MsgAccepterPack);
					ReaderMoney = 0;
				}
			}
			
			switch(ReaderType)
			{
				case RTV_VEND_APPROVED:
					OSMboxAccept(g_ReaderBackCmdMail);
					TraceReader("\r\n Task 2.Approve Vendout");
					MsgAccepterPack.ReaderBackCmd = MBOX_READERVENDOUT;						
					OSMboxPost(g_ReaderBackCmdMail,&MsgAccepterPack);
					ReaderType = 0;
					break;
				case RTV_VEND_DENIED:
					OSMboxAccept(g_ReaderBackCmdMail);
					TraceReader("\r\n Task 2.Denied Vendout");
					MsgAccepterPack.ReaderBackCmd = MBOX_READERVENDDENY;						
					OSMboxPost(g_ReaderBackCmdMail,&MsgAccepterPack);
					ReaderType = 0;
					break;	
				case RTV_CMD_OUT_OF_SEQUENCE:
					ReaderDevInit();
					ReaderType = 0;
					break;
			}
			
		}		
		OSTimeDly(7);
		/**/	
		//Trace("\r\n 4");
		//检查纸币器控制
		AccepterMsg = OSMboxPend(g_BillMoneyMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			billOpt = (AccepterMsg->BillBack);
			RecyPayoutMoney=(AccepterMsg->RecyPayoutMoney);
			RecyPayoutNum=(AccepterMsg->RecyPayoutNum);
			/*
			switch(AccepterMsg->BillBack)
			{
				case MBOX_BILLENABLEDEV:
					//Trace("enable bill\r\n");
					BillDevEnable();
					break;
				case MBOX_BILLDISABLEDEV:
					//Trace("disable bill\r\n");
					BillDevDisable();
					break;			
				case MBOX_BILLESCROW:
					//Trace("\r\nescrow bill");
					//BillDevEscrow();
					billOpt = MBOX_BILLESCROW;
					break;	
			}
			*/
			TraceBill("\r\n TaskBill pend=%d",billOpt);
		}
		//Trace("\r\n 5");
		//检查硬币器控制
		AccepterMsg = OSMboxPend(g_CoinMoneyMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			switch(AccepterMsg->CoinBack)
			{
				case MBOX_COINENABLEDEV:
					//Trace("enable coin\r\n");
					CoinDevEnable();
					break;
				case MBOX_COINDISABLEDEV:
					//Trace("disable coin\r\n");
					CoinDevDisable();
					break;				
			}
		}
		//Trace("\r\n 6");
		//检查找零器控制
		AccepterMsg = OSMboxPend(g_ChangeMoneyMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			switch(AccepterMsg->PayoutCmd)
			{
				case MBOX_MDBCHANGELEVEL3:
					TraceChange("\r\nTaskpayout coin=%ld\r\nTask=",AccepterMsg->PayoutMoney);
					Changevalue = AccepterMsg->PayoutMoney;
					ChangePayoutProcessLevel3(Changevalue, PayoutNum);
					//Trace("\r\n%d,%d,%d,%d,%d,%d,%d,%d",PayoutNum[0],PayoutNum[1],PayoutNum[2],PayoutNum[3],PayoutNum[4],PayoutNum[5],PayoutNum[6],PayoutNum[7]);
					OSTimeDly(7);	
					//通过邮箱返回找零结果
					MsgAccepterPack.PayoutBackCmd = MBOX_CHANGERBACKMONEY;	
					for(i = 0;i < 16;i++)
					{
						MsgAccepterPack.PayoutBackNum[i] = PayoutNum[i];
						PayoutNum[i] = 0;
						TraceChange("\r\n %d",MsgAccepterPack.PayoutBackNum[i]);
					}
					OSMboxPost(g_ChangeBackMoneyMail,&MsgAccepterPack);
					break;
				case MBOX_MDBCHANGELEVEL2:
					//Trace("payout coin=%ld\r\n",AccepterMsg->PayoutMoney);
					PayType = AccepterMsg->PayoutType;
					PayNum =  AccepterMsg->PayoutNum;
					ChangePayoutProcessLevel2(PayType,PayNum, PayoutNum);
					//Trace("\r\n%d,%d,%d,%d,%d,%d,%d,%d",PayoutNum[0],PayoutNum[1],PayoutNum[2],PayoutNum[3],PayoutNum[4],PayoutNum[5],PayoutNum[6],PayoutNum[7]);
					OSTimeDly(7);
					//通过邮箱返回找零结果
					MsgAccepterPack.PayoutBackCmd = MBOX_CHANGERBACKMONEY;	
					for(i = 0;i < 8;i++)
					{
						MsgAccepterPack.PayoutBackNum[i] = PayoutNum[i];
						PayoutNum[i] = 0;
					}
					OSMboxPost(g_ChangeBackMoneyMail,&MsgAccepterPack);
					break;
				case MBOX_MDBCHANGETUBE:
					//ChangeGetTubes();
					break;
				//下面针对的是Hopper	
				case MBOX_HOPPEROUT:
					PayType = AccepterMsg->PayoutType;
					PayNum =  AccepterMsg->PayoutNum;
					HopperAccepter_HpOutHandle(PayNum,PayType,&PayoutHpNum);
					OSTimeDly(7);
					//通过邮箱返回找零结果
					MsgAccepterPack.PayoutBackCmd = MBOX_HOPPERBACKMONEY;	
					MsgAccepterPack.SubBackType = PayType;	
					MsgAccepterPack.BkPackValue = PayoutHpNum;
					Trace("\r\n 3.hopper=%d,%d",MsgAccepterPack.SubBackType,MsgAccepterPack.BkPackValue);					
					OSMboxPost(g_ChangeBackMoneyMail,&MsgAccepterPack);
					break;
				case MBOX_HOPPERINITSTATE:
					HopperAccepter_CheckHPOk();
					NowChangerDev = CHANGEDEV_HOPPER;
					break;
					
			}
			
		}
		
		//Trace("\r\n 7");
		//检查读卡器控制
		AccepterMsg = OSMboxPend(g_ReaderBackMoneyMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			cmdOpt = (AccepterMsg->ReaderBack);
			if((cmdOpt == MBOX_READERVENDREQUEST)||(cmdOpt == MBOX_READERCASHSALE))
				ReaderPrice = (AccepterMsg->ReaderPrice);
			/*
			switch(AccepterMsg->ReaderBack)
			{
				case MBOX_READERENABLEDEV:
					//Trace("enable Reader\r\n");
					ReaderDevEnable();
					break;
				case MBOX_READERDISABLEDEV:
					//Trace("disable Reader\r\n");
					ReaderDevDisable();
					break;	
				case MBOX_READERVENDREQUEST:
					Trace("\r\n costVend=%ld",((AccepterMsg->ReaderPrice)/stDevValue.ReaderScale));
					//ReaderSessionVendRequest((AccepterMsg->ReaderPrice)/stDevValue.ReaderScale);
					ReaderSessionVendRequest(0);
					OSTimeDly(200);
					break;
				case MBOX_READERVENDSUCCESS:
					//Trace("\r\n costVend=%ld",((AccepterMsg->ReaderPrice)/stDevValue.ReaderScale));
					ReaderSessionVendSuccess();
					break;
				case MBOX_READERVENDFAIL:
					//Trace("\r\n costVend=%ld",((AccepterMsg->ReaderPrice)/stDevValue.ReaderScale));
					ReaderSessionVendFail();
					break;	
			}	
			*/		
		}
		//检查货道控制
		AccepterMsg = OSMboxPend(g_ChannelMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			switch(AccepterMsg->ChnlHandleType)	
			{
				case CHANNEL_OUTGOODS:
				case CHANNEL_CHECKSTATE:
				case CHANNEL_CHECKOUTRESULT:
					//Trace("channel..%x  %d  %d\r\n",OUTGOODSCMD,RecvBoxPack->SubSendType,(uint8_t)RecvBoxPack->SdPackValue);
					rst = ChannelHandle(AccepterMsg->ChnlHandleType,AccepterMsg->ChannelNum,AccepterMsg->GOCFlag,AccepterMsg->Binnum);
					MsgAccepterPack.ChnlHandleType = OUTGOODSCMD;
					MsgAccepterPack.HandleResult = rst;
					OSMboxPost(g_ChannelBackMail,&MsgAccepterPack);
					break;
				default:
					break;
			}
		}
		//接收升降台控制邮箱
		AccepterMsg = OSMboxPend(g_LiftTableMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			switch(AccepterMsg->ChnlHandleType)	
			{
				case CHANNEL_CLEARSN:
				case CHANNEL_OUTGOODS:
				case CHANNEL_CHECKSTATE:
				case CHANNEL_CHECKOUTRESULT:
				case MACHINE_RESET:
				case MACHINE_LIGHTOPEN:
				case MACHINE_LIGHTCLOSE:
				case MACHINE_OPENDOOR:
				case MACHINE_CLOSEDOOR:
				case MACHINE_INFORMATION:
				case VMC_CHUCHOU_REQ:
					TraceChannel("Task_recv:g_LiftTableMail    level=%d column=%d\r\n",
						AccepterMsg->LevelNum,AccepterMsg->ChannelNum);
					rst = LiftTableDriver(AccepterMsg->Binnum,AccepterMsg->ChnlHandleType,AccepterMsg->ChannelNum,AccepterMsg->LevelNum,MsgAccepterPack.LiftTablePack);

					TraceChannel("Task_res==%d\r\n",rst);
					MsgAccepterPack.ChnlHandleType = AccepterMsg->ChnlHandleType;
					MsgAccepterPack.HandleResult = rst;
					OSMboxPost(g_LiftTableBackMail,&MsgAccepterPack);
					break;
			}
		}
		//检查ACDC控制
		AccepterMsg = OSMboxPend(g_ACDCMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			if(AccepterMsg->ACDCHandleType==ACDCHANDLE)
			{
				rst = ACDCHandle(AccepterMsg->Binnum,AccepterMsg->ACDCData);
				MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
				MsgAccepterPack.HandleResult = rst;
				OSMboxPost(g_ACDCBackMail,&MsgAccepterPack);
			}
		}
		/*
		//接收盒饭柜控制邮箱  Add by liya 2014-01-20
		AccepterMsg = OSMboxPend(g_HeFanGuiMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			switch(AccepterMsg->HeFanGuiHandle)	
			{
				case HEFANGUI_KAIMEN:
				case HEFANGUI_CHAXUN:
				case HEFANGUI_JIAREKAI:
				case HEFANGUI_JIAREGUAN:
				case HEFANGUI_ZHILENGKAI:
				case HEFANGUI_ZHILENGGUAN:
				case HEFANGUI_ZHAOMINGKAI:
				case HEFANGUI_ZHAOMINGGUAN:
					TraceChannel("recv..CMD    lvel=%d\r\n",AccepterMsg->HeFanGuiHandle);
					rst = HeFanGuiDriver(AccepterMsg->Binnum,AccepterMsg->HeFanGuiHandle,AccepterMsg->HeFanGuiNum,MsgAccepterPack.HeFanGuiBuf);
					TraceChannel("Task_res==%d\r\n",rst);
					MsgAccepterPack.HeFanGuiHandle = AccepterMsg->HeFanGuiHandle;
					MsgAccepterPack.HeFanGuiRst = rst;
					OSMboxPost(g_HeFanGuiBackMail,&MsgAccepterPack);
					break;
			}
		}
		*/
		//检查各个设备状态
		OSMboxPend(g_DeviceStateMail,2,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{	
			//Trace("\r\n GetDeviceState");
			TraceBill("\r\n Taskbill=%d,%d,%d,%d,%d,%d,%d,%d",MdbBillErr.Communicate,MdbBillErr.moto,MdbBillErr.sensor,MdbBillErr.romchk,
				MdbBillErr.jam,MdbBillErr.removeCash,MdbBillErr.cashErr,MdbBillErr.disable);
			TraceCoin("\r\n Taskcoin=%d,%d,%d,%d,%d,%d,%d",MdbCoinErr.Communicate,MdbCoinErr.sensor,MdbCoinErr.tubejam,MdbCoinErr.romchk,
				MdbCoinErr.routing,MdbCoinErr.jam,MdbCoinErr.removeTube);			
			DeviceStatePack.DevStateCmd = MBOX_RETURNDEVSTATE;
			//MDB纸币器故障状态
			DeviceStatePack.BillCommunicate = MdbBillErr.Communicate;
			DeviceStatePack.Billmoto = MdbBillErr.moto;
			DeviceStatePack.Billsensor = MdbBillErr.sensor;
			DeviceStatePack.Billromchk = MdbBillErr.romchk;
			DeviceStatePack.Billjam = MdbBillErr.jam;
			DeviceStatePack.BillremoveCash = MdbBillErr.removeCash;
			DeviceStatePack.BillcashErr = MdbBillErr.cashErr;
			DeviceStatePack.Billdisable = MdbBillErr.disable;
			//MDB硬币器故障状态
			DeviceStatePack.CoinCommunicate = MdbCoinErr.Communicate;
			DeviceStatePack.Coinsensor = MdbCoinErr.sensor;
			DeviceStatePack.Cointubejam = MdbCoinErr.tubejam;
			DeviceStatePack.Coinromchk = MdbCoinErr.romchk;
			DeviceStatePack.Coinrouting = MdbCoinErr.routing;
			DeviceStatePack.Coinjam = MdbCoinErr.jam;
			DeviceStatePack.CoinremoveTube = MdbCoinErr.removeTube;
			DeviceStatePack.Coindisable = MdbCoinErr.disable;
			DeviceStatePack.CoinunknowError = MdbCoinErr.unknowError;
			DeviceStatePack.CoinunknowErrorLow = MdbCoinErr.unknowErrorLow;
			//Hopper找零器
			//DeviceStatePack.Hopper1State = stEvbHp[0].DevBadFlag;	
			for(j = 0;j < 3;j++)
			{
				switch(j)
				{
					case 0:
						//Trace("\r\n =%d,%d,%d,%d",stEvbHp[j].Num,stEvbHp[j].DevBadFlag,stEvbHp[j].State,stEvbHp[j].Price);
						if((stEvbHp[j].DevBadFlag >= 3)||(stEvbHp[j].Price == 0)||(stEvbHp[j].State == 3))//故障或者单价为0
						{
							DeviceStatePack.Hopper1State = 2;
							if(stEvbHp[j].DevBadFlag >= 3)//通讯失败的故障代码
							{
								stEvbHp[j].StateChk=0x80;
							}
							else if(stEvbHp[j].Price == 0)//单价为0
							{
								stEvbHp[j].StateChk=0x01;
							}
						}
						else if(stEvbHp[j].State == 2)//缺币
						{
							DeviceStatePack.Hopper1State = 1;
							stEvbHp[j].StateChk=0x20;
						}
						else if(stEvbHp[j].State == 0)//正常
						{
							DeviceStatePack.Hopper1State = 0;
							stEvbHp[j].StateChk=0;
						}
						DeviceStatePack.Hopper1StateChk = stEvbHp[j].StateChk;
						TraceChange("\r\n TaskHopper1=%d",DeviceStatePack.Hopper1State);
						break;
					case 1:
						//Trace("\r\n =%d,%d,%d,%d",stEvbHp[j].Num,stEvbHp[j].DevBadFlag,stEvbHp[j].State,stEvbHp[j].Price);
						if((stEvbHp[j].DevBadFlag >= 3)||(stEvbHp[j].Price == 0)||(stEvbHp[j].State == 3))
						{
							DeviceStatePack.Hopper2State = 2;
							if(stEvbHp[j].DevBadFlag >= 3)//通讯失败的故障代码
							{
								stEvbHp[j].StateChk=0x80;
							}
							else if(stEvbHp[j].Price == 0)//单价为0
							{
								stEvbHp[j].StateChk=0x01;
							}							
						}
						else if(stEvbHp[j].State == 2)
						{
							DeviceStatePack.Hopper2State = 1;
							stEvbHp[j].StateChk=0x20;
						}
						else if(stEvbHp[j].State == 0)
						{
							DeviceStatePack.Hopper2State = 0;
							stEvbHp[j].StateChk=0;
						}
						DeviceStatePack.Hopper2StateChk = stEvbHp[j].StateChk;
						TraceChange("\r\n TaskHopper2=%d",DeviceStatePack.Hopper2State);
						break;
					case 2:
						//Trace("\r\n =%d,%d,%d,%d",stEvbHp[j].Num,stEvbHp[j].DevBadFlag,stEvbHp[j].State,stEvbHp[j].Price);
						if((stEvbHp[j].DevBadFlag >= 3)||(stEvbHp[j].Price == 0)||(stEvbHp[j].State == 3))
						{
							DeviceStatePack.Hopper3State = 2;
							if(stEvbHp[j].DevBadFlag >= 3)//通讯失败的故障代码
							{
								stEvbHp[j].StateChk=0x80;
							}
							else if(stEvbHp[j].Price == 0)//单价为0
							{
								stEvbHp[j].StateChk=0x01;
							}
						}
						else if(stEvbHp[j].State == 2)
						{
							DeviceStatePack.Hopper3State = 1;
							stEvbHp[j].StateChk=0x20;
						}
						else if(stEvbHp[j].State == 0)
						{
							DeviceStatePack.Hopper3State = 0;
							stEvbHp[j].StateChk=0;
						}
						DeviceStatePack.Hopper3StateChk = stEvbHp[j].StateChk;
						TraceChange("\r\n TaskHopper3=%d",DeviceStatePack.Hopper3State);
						break;
				}
			}
			//Trace("\r\n Hopper1=%d,%d,%d,%d,%d,%d",stEvbHp[0].DevBadFlag,stEvbHp[0].State,stEvbHp[1].DevBadFlag,stEvbHp[1].State,stEvbHp[2].DevBadFlag,stEvbHp[2].State);
			//Trace("\r\n Hopper=%d,%d,%d",DeviceStatePack.Hopper1State,DeviceStatePack.Hopper2State,DeviceStatePack.Hopper3State);

			//出货机构
			//GOC状态
			DeviceStatePack.GOCError = stChannelErrForm.GOCErrFlag[0];
			DeviceStatePack.GOCErrorSub = stChannelErrForm.GOCErrFlag[1];
			//货道板
			DeviceStatePack.ColBoardError = stChannelErrForm.HuoDaoBanFlag[0];
			DeviceStatePack.ColBoardErrorSub = stChannelErrForm.HuoDaoBanFlag[1];
			
			//升降台
			DeviceStatePack.Error_FMD = stLiftTable.Error_FMD;
			DeviceStatePack.Error_FUM = stLiftTable.Error_FUM;
			DeviceStatePack.Error_GOC = stLiftTable.Error_GOC;
			DeviceStatePack.Error_FLD = stLiftTable.Error_FLD;
			DeviceStatePack.Error_FOD = stLiftTable.Error_FOD;
			DeviceStatePack.Error_UDS = stLiftTable.Error_UDS;
			DeviceStatePack.Error_GCD = stLiftTable.Error_GCD;
			DeviceStatePack.Error_SOVB = stLiftTable.Error_SOVB;
			DeviceStatePack.Error_SOMD1 = stLiftTable.Error_SOMD1;
			DeviceStatePack.Error_SOEC = stLiftTable.Error_SOEC;
			DeviceStatePack.Error_SFHG = stLiftTable.Error_SFHG;
			DeviceStatePack.Error_SOFP = stLiftTable.Error_SOFP;
			DeviceStatePack.Error_SOMD2 = stLiftTable.Error_SOMD2;

			//判断货道是否全部不可用
			DeviceStatePack.Emp_Gol = hd_all_check();
			
			
			//Trace("\r\n Em2=%d",DeviceStatePack.Emp_Gol);
			OSMboxPost(g_DeviceStateBackMail,&DeviceStatePack);			
		}
	
		
	}
	
	//while(1)
	//{
		//HopperAccepter_HpOutHandle(2,1);
		//OSTimeDly(2000);
	//}
}
