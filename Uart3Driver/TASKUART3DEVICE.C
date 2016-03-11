/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           TASKDEVICE
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        串口3任务                     
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
#include "TASKUART3DEVICE.H"
#include "ZHIHUIPC.H"
#include "UBProtocol.H"
#include "UBProtocol_SIMPLE.h"
#include "VMC_GPRS_PC.h"



#define PC_ZHIHUI			1				//当前系统识别到的PC通讯类型为一鸣智慧
#define PC_UBOX  		    2			    //当前系统识别到的PC通讯类型为友宝
#define PC_GPRS				3     			//当前系统识别到的PC通讯类型为GPRS
#define PC_SIMPUBOX			4               //新友宝pc通讯
#define PC_UBOXCR			5               //友宝成人pc通讯
//extern uint8_t GetWeihuStatus();





/*********************************************************************************************************
** Function name:       TaskDevice
** Descriptions:        设备任务:主要处理挂在UART2上的设备，如MDB协议,EVB协议等
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart3TaskDevice(void *pvData)
{
	MESSAGE_ZHIHUI *accepter_msg;
	MessageUboxPCPack *AccepterUboxMsg;
	MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	unsigned char ComStatus;
	//当前PC设备的类型
	uint8_t NowPCDev = 0;
	uint8_t WeihuMode = 0;//1在维护模式下,0退出维护模式
	uint8_t retRpt=0;
	uint8_t isInit=0;//1系统已经开始响应,0系统还没响应
	
	
	//检查PC控制
	if((SystemPara.PcEnable == ZHIHUI_PC)||(SystemPara.PcEnable == GPRS_PC) )
	{
		accepter_msg = OSQPend(g_PCMail,OS_TICKS_PER_SEC * 10,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{		
			if(accepter_msg->vmc_type ==  MBOX_VMC_ZH_START)
			{			
				Trace("ZhiHuiStart......\r\n");
				//PC结构体初始化			
				vmc_data_init();
				//LCDNumberFontPrintf(40,LINE15,2,"ZhiHuiAccepter-1");				
				vmc_setup_pc(1);//签到 
				zh_vmc_post(MBOX_ZH_VMC_ONLINE);
				accepter_msg = OSQPend(g_PCMail,100,&ComStatus);
				if(ComStatus == OS_NO_ERR)
				{
					if(accepter_msg->vmc_type == MBOX_VMC_ZH_OFFLINE_REPORT)
					{
						MACData.write_index = accepter_msg->trade_index;
						vmc_report_pc(0);
						
					}
				}	
				NowPCDev = PC_ZHIHUI;			
			}
			else if(accepter_msg->vmc_type == MBOX_VMC_GPRS_START)
			{
				//add by yoc gprs
				vmc_data_init();
				//LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-1");
				Trace("\r\n GPRS init........"); 				
				mvc_gprs_init();
				trade_cache_init();
				vmc_gprs_setup();//gprs 注册服务器
				//通知主控板进入在线模式
				Trace("\r\n gprs online....\r\n");
				zh_vmc_post(MBOX_ZH_VMC_ONLINE);
				accepter_msg = OSQPend(g_PCMail,100,&ComStatus);
				if(ComStatus == OS_NO_ERR)
				{
					if(accepter_msg->vmc_type == MBOX_VMC_ZH_OFFLINE_REPORT)
					{	
						Trace("gprs recv mbox...\r\n"); 
						MACData.write_index = accepter_msg->trade_index;
					}
				}
				NowPCDev = PC_GPRS;	
			}
			else 
			{
				
				NowPCDev = 0;
			}
			
		}	
		else
		{
			NowPCDev = 0;
		}
		OSTimeDly(OS_TICKS_PER_SEC / 100);

		
	}
	else if( SystemPara.PcEnable == UBOX_PC )
	{
		//检查友宝PC控制
		AccepterUboxMsg = OSQPend(g_Ubox_VMCTOPCQ,OS_TICKS_PER_SEC,&ComStatus);
		if(ComStatus == OS_NO_ERR)
		{
			if((AccepterUboxMsg->PCCmd)==MBOX_VMC_UBOXINITDEV)
			{	
				//OSTimeDly(1000);
				if(SystemPara.EasiveEnable == 1)
				{
					TracePC("\r\n Taskpend Easivendinit"); 
					LCDNumberFontPrintf(40,LINE15,2,"Easivend-1");
				}
				else
				{
					TracePC("\r\n Taskpend Uboxinit"); 
					LCDNumberFontPrintf(40,LINE15,2,"BAccepter-1");
				}
				VPSerialInit();
				if(SystemPara.EasiveEnable == 1)
				{
					VPMission_Act_RPT(VP_ACT_PCON,0,0,0,0,0,0);
				}
				else
				{
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}	
				NowPCDev = PC_UBOX;	
			}
			else
			{
				NowPCDev = 0;
			}
		}	
		else
		{
			NowPCDev = 0;
		}
		OSTimeDly(4);
	}	
	else if( SystemPara.PcEnable == SIMPUBOX_PC )
	{
		TracePC("\r\n Taskpend SIMPLEUboxinit"); 
		LCDNumberFontPrintf(40,LINE15,2,"SIMPLEAccepter-1");
		SIMPLESIMPLEVPSerialInit();
		NowPCDev = PC_SIMPUBOX;
	}
	else if( SystemPara.PcEnable == CRUBOX_PC )
	{
		TracePC("\r\n Taskpend CRUboxinit"); 
		LCDNumberFontPrintf(40,LINE15,2,"CRAccepter-1");
		VPSerialInit_CR();
		VPMission_Setup_RPT_CR();
		NowPCDev = PC_UBOXCR;
	}
	else
	{
		NowPCDev = 0;
	}

	while(1)
	{
		TracePC("\r\n Task PcEnable=%d",SystemPara.PcEnable); 
		if(SystemPara.PcEnable == ZHIHUI_PC)
		{
			task3_zh_poll();
			//OSTimeDly(100/5);
			accepter_msg = OSQPend(g_PCMail,250/5,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{			
				switch(accepter_msg->vmc_type)
				{
					case  MBOX_VMC_ZH_COIN:
						Trace("MBOX_VMC_ZH_COIN....\r\n");
						vmc_money_rpt(ZH_COIN,
							accepter_msg->recv_amount,accepter_msg->recv_all_amount);
					break;
					case  MBOX_VMC_ZH_BILL:
						Trace("MBOX_VMC_ZH_BILL....\r\n");
						vmc_money_rpt(ZH_BILL,
							accepter_msg->recv_amount,accepter_msg->recv_all_amount);					
					break;
					case MBOX_VMC_ZH_ESCROWIN:						
						Trace("MBOX_VMC_ZH_ESCROWIN....\r\n");
						vmc_money_rpt(ZH_ESCROWIN,
							accepter_msg->recv_amount,accepter_msg->recv_all_amount);
					break;
					case  MBOX_VMC_ZH_BUTTON:					
						Trace("MBOX_VMC_ZH_BUTTON....\r\n");
						vmc_button_rpt(accepter_msg->pay_mode,
							accepter_msg->cur_logic_no);					
					break;
					case  MBOX_VMC_ZH_ESCROWOUT:
						Trace("MBOX_VMC_ZH_ESCROWOUT....\r\n");
						vmc_money_rpt(ZH_ESCROWOUT,
							accepter_msg->recv_amount,accepter_msg->recv_all_amount);
						break;
					
					case MBOX_VMC_ZH_TRADE://上报一次出货
						Trace("MBOX_VMC_ZH_TRADE....\r\n");
						vmc_trade_rpt(accepter_msg->cur_logic_no,accepter_msg->pay_mode,
							accepter_msg->cur_goods_price,accepter_msg->cur_trade_state);					
						break;
					case  MBOX_VMC_ZH_PAYOUT://找零			
						Trace("MBOX_VMC_ZH_PAYOUT....\r\n");
						break;
					case  MBOX_VMC_ZH_TRADE_OVER://交易结束	
						Trace("MBOX_VMC_ZH_TRADE_OVER....\r\n");
						vmc_trade_over_rpt();
						break;
					
					case MBOX_VMC_ZH_WEIHU://进入维护模式
						Trace("MBOX_VMC_ZH_WEIHU....\r\n");
						vmc_weihu_rpt();		
						break;
					case MBOX_VMC_ZH_NORMAL://离开维护模式
						Trace("MBOX_VMC_ZH_NORMAL....\r\n");
						vmc_normal_rpt();						
						break;
					case MBOX_VMC_ZH_CLEAR:
						Trace("MBOX_VMC_ZH_CLEAR....\r\n");
						vmc_trade_clear();
						break;
					case MBOX_VMC_ZH_ACK:
						Trace("MBOX_VMC_ZH_ACK....\r\n");
						vmc_ack_rpt(ACK_T);
						break;
						
					case MBOX_VMC_ZH_NAK:
						Trace("MBOX_VMC_ZH_NAK....\r\n");
						vmc_ack_rpt(NAK_T);
						break;
					case MBOX_VMC_ZH_HUMAN://人体接近
						Trace("MBOX_VMC_ZH_HUMAN....\r\n");
							vmc_human_rpt(accepter_msg->humanState);
						break;

					case MBOX_VMC_ZH_DOOR_OPEN://大门开
						Trace("MBOX_VMC_ZH_DOOR_OPEN....\r\n");
							vmc_door_rpt(1);
						break;

					case MBOX_VMC_ZH_DOOR_CLOSE://大门guan
						Trace("MBOX_VMC_ZH_DOOR_CLOSE....\r\n");
							vmc_door_rpt(0);
						break;

					case MBOX_VMC_ZH_OFFLINE_REPORT:
						Trace("MBOX_VMC_ZH_OFFLINE_REPORT....\r\n");
							vmc_online_rpt(LogPara.LogDetailPage);						
						break;
					default:
						break;
				
				}
				OSTimeDly(250/5);
				printTrade();
			}

		}		
		else if(SystemPara.PcEnable == UBOX_PC)
		{
			if(GetWeihuStatus()==0)
			{
				//OSTimeSet(0);
				VPMission_Poll(&isInit);
				OSTimeDly(OS_TICKS_PER_SEC/4);
				//Trace("Poll time %d\r\r\n",OSTimeGet());	
			}
			
			//检查友宝PC控制
			AccepterUboxMsg = OSQPend(g_Ubox_VMCTOPCQ,20,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				/*
				if((AccepterUboxMsg->PCCmd)==MBOX_VMC_UBOXINITDEV)
				{	
					//OSTimeDly(1000);
					TracePC("\r\n Taskpend Uboxinit"); 
					LCDNumberFontPrintf(40,LINE15,2,"UBOXAccepter-1");
					VPSerialInit();
					NowPCDev = PC_UBOX;	
				}*/
				//TracePC("\r\n Taskpend Ubox=%d",AccepterUboxMsg->PCCmd);	
				switch(AccepterUboxMsg->PCCmd)
				{
					case MBOX_VMCTOPC_PAYINCOIN:
						VPMission_Payin_RPT(1,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxcoin=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYINBILL:
						VPMission_Payin_RPT(2,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxbill=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYINESCROWIN:
						VPMission_Payin_RPT(3,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxescrowin=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;	
					case MBOX_VMCTOPC_PAYINESCROWOUT:
						VPMission_Payin_RPT(4,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxescrowout=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYINREADER:
						VPMission_Payin_RPT(5,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxbill=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;	
					case MBOX_VMCTOPC_PAYOUT:
						VPMission_Payout_RPT(AccepterUboxMsg->payoutDev,AccepterUboxMsg->Type,AccepterUboxMsg->payoutMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxpayout=%d,all=%ld,type=%d",AccepterUboxMsg->payoutMoney,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->Type); 
						break;	
					case MBOX_VMCTOPC_COST:
						VPMission_Cost_RPT(AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxcost=%d,all=%ld,type=%d",AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->Type); 
						break;
					case MBOX_VMCTOPC_BUTTON:
						VPMission_Button_RPT(AccepterUboxMsg->Type,AccepterUboxMsg->Column,AccepterUboxMsg->device);
						TracePC("\r\n Taskpend UboxBtn=%d,type=%d",AccepterUboxMsg->Column,AccepterUboxMsg->Type); 
						break;	
					case MBOX_VMCTOPC_VENDOUT:
						VPMission_Vendout_RPT(AccepterUboxMsg->status,AccepterUboxMsg->device,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->ColumnSum);
						TracePC("\r\n Taskpend UboxVendout=%d,%d,%d,%ld,%ld,%d",AccepterUboxMsg->status,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->ColumnSum); 
						break;
					case MBOX_VMCTOPC_STATUS:
						VPMission_Status_RPT(AccepterUboxMsg->check_st,AccepterUboxMsg->bv_st,AccepterUboxMsg->cc_st,AccepterUboxMsg->vmc_st,AccepterUboxMsg->change,AccepterUboxMsg->tem_st,AccepterUboxMsg->recyclerSum,AccepterUboxMsg->coinSum);
						TracePC("\r\n Taskpend UboxStatus=%d,%d,%d,%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",AccepterUboxMsg->check_st,AccepterUboxMsg->bv_st,AccepterUboxMsg->cc_st,AccepterUboxMsg->vmc_st,AccepterUboxMsg->change,AccepterUboxMsg->recyclerSum[0],AccepterUboxMsg->recyclerSum[1],
						      AccepterUboxMsg->recyclerSum[2],AccepterUboxMsg->recyclerSum[3],AccepterUboxMsg->recyclerSum[4],AccepterUboxMsg->recyclerSum[5],AccepterUboxMsg->coinSum[0],AccepterUboxMsg->coinSum[1],AccepterUboxMsg->coinSum[2],AccepterUboxMsg->coinSum[3],AccepterUboxMsg->coinSum[4],AccepterUboxMsg->coinSum[5]); 
						break;	
					case MBOX_VMCTOPC_ACTION:
						TracePC("\r\n Taskpend UboxAction=%d,%d,%d,%d,%d,%ld,%ld",AccepterUboxMsg->action,AccepterUboxMsg->value,AccepterUboxMsg->second,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney); 
						if(AccepterUboxMsg->action==VP_ACT_ADMIN)
						{
							WeihuMode = AccepterUboxMsg->value;
							if(WeihuMode==0)
							{
								VPMission_Act_RPT(VP_ACT_ADMIN,AccepterUboxMsg->value,0,0,0,0,0);
							}
						}
						else
						{
							VPMission_Act_RPT(AccepterUboxMsg->action,AccepterUboxMsg->value,AccepterUboxMsg->second,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney);
						}
						break;	
					case MBOX_VMCTOPC_ADMIN:
						TracePC("\r\n Taskpend UboxAdmin=%d,%d,%d",AccepterUboxMsg->Type,AccepterUboxMsg->Column,AccepterUboxMsg->ColumnSum); 
						retRpt = VPMission_Admin_RPT(AccepterUboxMsg->Type,AccepterUboxMsg->Column,AccepterUboxMsg->ColumnSum);
						if(retRpt == VP_ERR_NULL)
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_ACK;	
							OSMboxPost(g_Ubox_VMCTOPCBackCMail,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
							TracePC("\r\n Taskpend UboxAdminSuc");
                	    }
						else
						{
							MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_NAK;
							OSMboxPost(g_Ubox_VMCTOPCBackCMail,&MsgUboxPack[g_Ubox_Index]);
							UpdateIndex();
							TracePC("\r\n Taskpend UboxAdminFail");
                	    }	
						break;	
					case MBOX_VMCTOPC_INFORPT:						
						VPMission_Info_RPT(AccepterUboxMsg->Type,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->check_st,AccepterUboxMsg->bv_st,AccepterUboxMsg->cc_st,AccepterUboxMsg->Control_Huodao,AccepterUboxMsg->value,AccepterUboxMsg->Control_device,AccepterUboxMsg->cabinetNums,AccepterUboxMsg->cabinetdata);						
						TracePC("\r\n Taskpend UboxInfo=%d,%ld",AccepterUboxMsg->Type,AccepterUboxMsg->payAllMoney); 
						break;		
				}
				OSTimeDly(OS_TICKS_PER_SEC/4);
			}	

			//每隔时间，发送action指令
			//交易模式下
			if(GetWeihuStatus()==0)
			{
				//如果未启动完成，在易丰协议下，需要连续发送主机已经重启指令
				if(isInit==0)
				{
					if(SystemPara.EasiveEnable == 1)
					{
						if(Timer.ActionPCTimer==0)
						{
							Timer.ActionPCTimer = 5;
							VPMission_Act_RPT(VP_ACT_PCON,0,0,0,0,0,0);
							OSTimeDly(OS_TICKS_PER_SEC/4);
						}
					}
				}
				//如果启动完成，在友宝协议下，每10s发送一次heart指令
				else
				{
					if(SystemPara.EasiveEnable == 0)
					{
						if(Timer.ActionPCTimer==0)
						{
							Timer.ActionPCTimer = 10;
							VPMission_Act_RPT(VP_ACT_HEART,0,0,0,0,0,0);
							OSTimeDly(OS_TICKS_PER_SEC/4);
						}
					}
				}
			}
			//维护模式下
			else if(GetWeihuStatus()==1)
			{
				if(Timer.ActionPCTimer==0)
				{
					Timer.ActionPCTimer = 1;
					VPMission_Act_RPT(VP_ACT_ADMIN,1,0,0,0,0,0);
				}
			}
		}
		else if(SystemPara.PcEnable == GPRS_PC)//add by yoc 2013.11.13
		{
			//add by yoc gprs
			Trace("fun: Uart3TaskDevice 'NowPCDev == PC_GPRS' is finish! \r\n");	
			task3_gprs_poll();		
			accepter_msg = OSQPend(g_PCMail,250/5,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				switch(accepter_msg->vmc_type)
				{
					case  MBOX_VMC_ZH_COIN:
						print_log("GPRS:MBOX_VMC_ZH_COIN\r\n");
						break;
					case  MBOX_VMC_ZH_BILL:
						print_log("GPRS:MBOX_VMC_ZH_BILL\r\n");
						break;
					case  MBOX_VMC_ZH_ESCROWIN:
						print_log("GPRS:MBOX_VMC_ZH_ESCROWIN\r\n");
						break;		
					case MBOX_VMC_ZH_ESCROWOUT:
						print_log("GPRS:MBOX_VMC_ZH_ESCROWOUT\r\n");
						break;	
					case  MBOX_VMC_ZH_PAYOUT:
						print_log("GPRS:MBOX_VMC_ZH_PAYOUT\r\n");
						break;
					case  MBOX_VMC_ZH_TRADE:
						print_log("GPRS:MBOX_VMC_ZH_TRADE\r\n");
						
						break;
					case  MBOX_VMC_ZH_TRADE_OVER:
						print_log("GPRS:MBOX_VMC_ZH_TRADE_OVER\r\n");
						vmc_gprs_trade_rpt(accepter_msg->trade_index);
						vmc_trade_over_rpt();
						break;
					case  MBOX_VMC_ZH_WEIHU:
						print_log("GPRS:MBOX_VMC_ZH_WEIHU\r\n");
						vmc_weihu_rpt();
						break;
					case  MBOX_VMC_ZH_NORMAL:
						print_log("GPRS:MBOX_VMC_ZH_NORMAL\r\n");
						vmc_normal_rpt();		
						break;
					case  MBOX_VMC_ZH_CLEAR:
						print_log("GPRS:MBOX_VMC_ZH_CLEAR\r\n");
						vmc_trade_clear();
						break;
					default:
						
						break;
				
				}
				OSTimeDly(250/2);
			 }
		}
		else if(SystemPara.PcEnable == SIMPUBOX_PC)
		{
			VPMissionSIMPLESIMPLE_Poll();
			//OSTimeDly(OS_TICKS_PER_SEC/4);
			//OSTimeDly(OS_TICKS_PER_SEC/50);
			OSTimeDly(7);
			//取得返回值
			AccepterSIMPLEUboxMsg = OSQPend(g_SIMPLEUbox_VMCTOPCQ,1,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				switch(AccepterSIMPLEUboxMsg->VMCTOPCCmd)
				{
					case MBOX_SIMPLEVMCTOPC_BUTTONRPT:
						TracePC("\r\n Taskpend btnchannel_id=%d",AccepterSIMPLEUboxMsg->btnchannel_id); 						
						VPMissionSIMPLE_Button_RPT(AccepterSIMPLEUboxMsg->btnchannel_id);
						break;
					case MBOX_SIMPLEVMCTOPC_ADMINRPT:
						TracePC("\r\n Taskpend Admin=%d,%d,%d",AccepterSIMPLEUboxMsg->admintype,AccepterSIMPLEUboxMsg->admincolumn,AccepterSIMPLEUboxMsg->admincolumnsum);					
						VPMissionSIMPLE_Admin_RPT(AccepterSIMPLEUboxMsg->admintype,AccepterSIMPLEUboxMsg->admincolumn,AccepterSIMPLEUboxMsg->admincolumnsum);
						break;
					case MBOX_SIMPLEVMCTOPC_GETADMIN:						
						TracePC("\r\n Taskpend GetAdmin=%d,%d",AccepterSIMPLEUboxMsg->admintype,AccepterSIMPLEUboxMsg->admincolumn);					
						if(AccepterSIMPLEUboxMsg->admintype==3)
						{
							VPSIMPLE_Sethdquery(AccepterSIMPLEUboxMsg->admincolumn);
						}
						VPMissionSIMPLE_Get_Admin2(AccepterSIMPLEUboxMsg->admintype);
						break;	
				}
			}
		}
		else if(SystemPara.PcEnable == CRUBOX_PC)
		{
			if(GetWeihuStatus()==0)
			{
				VPMission_Poll_CR();
				OSTimeDly(OS_TICKS_PER_SEC/4);
				//OSTimeDly(OS_TICKS_PER_SEC/50);
				//OSTimeDly(7);
			}
			
			//检查友宝PC控制
			AccepterUboxMsg = OSQPend(g_Ubox_VMCTOPCQ,20,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				/*
				if((AccepterUboxMsg->PCCmd)==MBOX_VMC_UBOXINITDEV)
				{	
					//OSTimeDly(1000);
					TracePC("\r\n Taskpend Uboxinit"); 
					LCDNumberFontPrintf(40,LINE15,2,"UBOXAccepter-1");
					VPSerialInit();
					NowPCDev = PC_UBOX;	
				}*/
				//TracePC("\r\n Taskpend Ubox=%d",AccepterUboxMsg->PCCmd);	
				switch(AccepterUboxMsg->PCCmd)
				{		
					case MBOX_VMCTOPC_PAYINCOIN:
						VPMission_Payin_RPT_CR(1,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxcoin=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYINBILL:
						VPMission_Payin_RPT_CR(2,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxbill=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYINESCROWIN:
						VPMission_Payin_RPT_CR(3,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxescrowin=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;	
					case MBOX_VMCTOPC_PAYINESCROWOUT:
						VPMission_Payin_RPT_CR(4,AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxescrowout=%d,all=%ld",AccepterUboxMsg->payInMoney,AccepterUboxMsg->payAllMoney); 
						break;
					case MBOX_VMCTOPC_PAYOUT:
						VPMission_Payout_RPT_CR(AccepterUboxMsg->payoutDev,AccepterUboxMsg->Type,AccepterUboxMsg->payoutMoney,AccepterUboxMsg->payoutRemain,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxpayout=%d,remain=%d,all=%ld,type=%d",AccepterUboxMsg->payoutMoney,AccepterUboxMsg->payoutRemain,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->Type); 
						break;	
					case MBOX_VMCTOPC_COST:
						VPMission_Cost_RPT_CR(AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney);
						TracePC("\r\n Taskpend Uboxcost=%d,all=%ld,type=%d",AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney,AccepterUboxMsg->Type); 
						break;
					case MBOX_VMCTOPC_INFORPT:
						VPMission_Info_RPT_CR(0);
						TracePC("\r\n Taskpend UboxStatus=%d,%d,%d,%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",AccepterUboxMsg->check_st,AccepterUboxMsg->bv_st,AccepterUboxMsg->cc_st,AccepterUboxMsg->vmc_st,AccepterUboxMsg->change,AccepterUboxMsg->recyclerSum[0],AccepterUboxMsg->recyclerSum[1],
						      AccepterUboxMsg->recyclerSum[2],AccepterUboxMsg->recyclerSum[3],AccepterUboxMsg->recyclerSum[4],AccepterUboxMsg->recyclerSum[5],AccepterUboxMsg->coinSum[0],AccepterUboxMsg->coinSum[1],AccepterUboxMsg->coinSum[2],AccepterUboxMsg->coinSum[3],AccepterUboxMsg->coinSum[4],AccepterUboxMsg->coinSum[5]); 
						break;	
					case MBOX_VMCTOPC_ACTION:
						TracePC("\r\n Taskpend UboxAction=%d",AccepterUboxMsg->action); 
						VPMission_Act_RPT_CR(AccepterUboxMsg->action);
						break;	
					case MBOX_VMCTOPC_BUTTON:
						VPMission_Button_RPT_CR();
						TracePC("\r\n Taskpend UboxBtn"); 
						break;	
					case MBOX_PCTOVMC_RESETRPT:
						VP_Reset_Rpt_CR();
						TracePC("\r\n Taskpend UboxReset"); 
						break;
				}
				OSTimeDly(OS_TICKS_PER_SEC/4);
			}	
			
			//每隔时间，发送action指令
			//交易模式下
			if(GetWeihuStatus()==0)
			{		
				if(Timer.ActionPCTimer==0)
				{
					Timer.ActionPCTimer = 10;
					VPMission_Act_RPT_CR(0);
					OSTimeDly(OS_TICKS_PER_SEC/4);
				}					
			}
			//维护模式下
			else if(GetWeihuStatus()==1)
			{
				if(Timer.ActionPCTimer==0)
				{
					Timer.ActionPCTimer = 10;
					VPMission_Act_RPT_CR(1);
				}
			}

		}
		else if(NowPCDev == 0)
		{
			OSTimeDly(OS_TICKS_PER_SEC/2);
		}
		//OSTimeDly(OS_TICKS_PER_SEC/2);
	}
}
