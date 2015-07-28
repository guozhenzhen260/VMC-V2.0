/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           MAINTAIN.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        维护流程                     
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h" 
#include "USERMAINTAIN_MENU.C"
#include "MAINTAIN_MENU.C"
#include "SELFCHECK_MENU.C"
#include "CHANNEL.h"
#include "ACDC.h"



#define SOFTWAREVERSION "VER:V2.05-01" //注意暂时未与系统参数中的版本做关联

//extern SYSTEMPARAMETER SystemPara;
extern RTC_DATE RTCData;

void EnterSelfCheckFuction(void);
void EnterVendingSetFuction(void);
void FactorySetting(void);
void RstFactorySetting(void);
void DefaultSystemParaMenu(void);
char * ChooseSet(uint8_t flag);


/*********************************************************************************************************
** Function name:     	PasswordCheck
** Descriptions:	    密码验证页面
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t PasswordCheck(void)
{
	unsigned char i;
	unsigned char Index = 0,Key;
	unsigned char Pin[6],PinOk = 0;
	unsigned char ADMINPASSWORD[8] 	 = {'8','3','7','1','8','5','5','7'};
	
	LCDClrScreen();
	Trace("EnterSelfCheckFuction\r\n");
	while(1)
	{
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDPrintf(8,1,0,SystemPara.Language,PasswordMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf(48,6,0,SystemPara.Language,PasswordMenuList.EnterPin[SystemPara.Language]);
		LCDClrArea(1,13,238,14);
		LCDPrintf(28,13,0,SystemPara.Language,PasswordMenuList.ReturnCapion[SystemPara.Language]);
		
		while(1)
		{
			PinOk = 0;
			for(i=0;i<8;i++)
			{
				LCDPutAscii(52+(i*16),9,"_",0);
			}
			while(1)//进入密码验证
			{
				Key = ReadKeyValue();
				if(Key != 0x00)
				{
					if(Key == 'C')
						return 0;	//按取消直接返回
					else
					{
						Pin[Index] = Key;
						LCDPutAscii(52+(16*Index),9,"*",0);
						Index++;
						if(Index == 8)
						{
							Index = 0;
							PinOk = 1;
							for(i=0;i<8;i++)
							{
								//Trace("\r\n pin=%c",Pin[i]);
								if(Pin[i] != ADMINPASSWORD[i])//对比密码									
									PinOk = 0;	
							}
							if(PinOk == 1)
							{
								return 1;
							}
							else//密码错误
							{
								LCDClrArea(1,13,238,14);
								LCDPrintf(48,13,0,SystemPara.Language,PasswordMenuList.PinError[SystemPara.Language]);
								OSTimeDly(400);
								LCDClrArea(1,13,238,14);
								LCDPrintf(48,13,0,SystemPara.Language,PasswordMenuList.ReturnCapion[SystemPara.Language]);
								for(i=0;i<8;i++)
								{
									LCDPutAscii(52+(i*16),9,"_",0);
								}	
							}
						}/**/
					}
				}//等待按键
				OSTimeDly(5);
			}//密码验证循环	
		}
	} 	
}

/*********************************************************************************************************
** Function name:     	SelectOKCheck
** Descriptions:	    确认取消对话框页面
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
uint8_t SelectOKCheck(void)
{	
	unsigned char Key;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		LCDPrintf(8,1,0,SystemPara.Language,OKMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf(60,7,0,SystemPara.Language,OKMenuList.SureCapion[SystemPara.Language]);
		LCDPrintf(40,11,0,SystemPara.Language,OKMenuList.OKPin[SystemPara.Language]);
		LCDPrintf(140,11,0,SystemPara.Language,OKMenuList.CancelPin[SystemPara.Language]);		
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return 0;
			}
			else if(Key == 'E')
			{				
				return 1;
			}
			OSTimeDly(10);	
		}			
	}
}


/*********************************************************************************************************
** Function name:     	TubeMoneyEnough
** Descriptions:	    更新可找零金额,是否不足
** input parameters:    无
** output parameters:   无
** Returned value:      1不足,0足够
*********************************************************************************************************/
uint8_t TubeMoneyEnough()
{
	uint32_t coinMoney;	
	uint8_t state = 0;
	uint8_t i;
	//ChangeGetTubesAPI();
	//OSTimeDly(OS_TICKS_PER_SEC / 10);
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		coinMoney = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
					+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5]
					+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
					+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
					+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
					+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15];  
		//添加纸币循环器金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			for(i=0;i<7;i++)
			{
				if(stDevValue.RecyclerValue[i])
				{
					coinMoney = coinMoney + stDevValue.RecyclerValue[i]*stDevValue.RecyclerNum[i];
				}
			}
		}
		if(coinMoney < SystemPara.BillEnableValue)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}
	}	
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		state =HopperIsEmpty();		
	}
	else if(SystemPara.CoinChangerType == OFF_CHANGER)
	{
		state =0;
	}
	return state;
}

/*********************************************************************************************************
** Function name:     	PCError
** Descriptions:	    PC故障
** input parameters:    PCType=1是小数点位数scale故障,2离线故障
** output parameters:   无
** Returned value:      1故障,0没故障
*********************************************************************************************************/
uint8_t PCError(uint8_t PCType)
{
	//uint32_t coinMoney;	
	uint8_t state = 0;

	if(SystemPara.PcEnable == UBOX_PC)
	{
		if(PCType == 1)//小数点位数scale故障
		{			
			state = GetScaleError();
			TracePC("\r\n App PCErrorScale=%d",state,GetScaleError());
		}
		else if(PCType == 2)//离线故障
		{
			state = LogPara.offLineFlag;
			TracePC("\r\n App PCErrorOffLine=%d",LogPara.offLineFlag);
		}
	}
	return state;
}


/*****************************************************************************
** Function name:	ChooseSet	
**
** Descriptions:	输入菜单值选择	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
char * ErrorState(uint8_t flag)
{
	switch(flag)
	{		
		case 1:
			return ErrorStateTo.EMPTY[SystemPara.Language];	
		case 2:
			return ErrorStateTo.ERR[SystemPara.Language];		
		default:
			return ErrorStateTo.UNSET[SystemPara.Language];								
	}
}

/*********************************************************************************************************
** Function name:     	ErrorCheck
** Descriptions:	    故障查询
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
void ErrorCheck(void)
{	
	uint8_t pageNum = 0,lineNum = 3;
	unsigned char Key;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		lineNum = 3;		
		switch(pageNum)
		{
			case 0:	
				//纸币器
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.BillCapion[SystemPara.Language],1);
				if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
				{
					if(DeviceStateBusiness.BillCommunicate)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.BillCommunicate[SystemPara.Language]);
					if(DeviceStateBusiness.Billmoto)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Billmoto[SystemPara.Language]);
					if(DeviceStateBusiness.Billsensor)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Billsensor[SystemPara.Language]);
					if(DeviceStateBusiness.Billromchk)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Billromchk[SystemPara.Language]);
					if(DeviceStateBusiness.Billjam)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Billjam[SystemPara.Language]);
					
					if((DeviceStateBusiness.BillCommunicate==0)&&(DeviceStateBusiness.Billmoto==0)&&(DeviceStateBusiness.Billsensor==0)&&(DeviceStateBusiness.Billromchk==0)&&(DeviceStateBusiness.Billjam==0))
					{
						LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
					}
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageDown[SystemPara.Language]);//翻页提示
				break;
			case 1:		
				//纸币器
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.BillCapion[SystemPara.Language],2);
				if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
				{	
					if(DeviceStateBusiness.BillremoveCash)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.BillremoveCash[SystemPara.Language]);
					if(DeviceStateBusiness.BillcashErr)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.BillcashErr[SystemPara.Language]);
					if(DeviceStateBusiness.Billdisable)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Billdisable[SystemPara.Language]);
					if(TubeMoneyEnough())
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.BillCoinEmp[SystemPara.Language]);
							
					
					if((DeviceStateBusiness.BillremoveCash==0)&&(DeviceStateBusiness.BillcashErr==0)&&(DeviceStateBusiness.Billdisable==0)&&(TubeMoneyEnough()==0))
					{
						LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
					}
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 2:	
				//硬币器				
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.CoinCapion[SystemPara.Language],1);
				if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
				{
					if(DeviceStateBusiness.CoinCommunicate)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.CoinCommunicate[SystemPara.Language]);
					if(DeviceStateBusiness.Coinsensor)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Coinsensor[SystemPara.Language]);
					if(DeviceStateBusiness.Cointubejam)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Cointubejam[SystemPara.Language]);
					if(DeviceStateBusiness.Coinromchk)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Coinromchk[SystemPara.Language]);
					if(DeviceStateBusiness.Coinrouting)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Coinrouting[SystemPara.Language]);

					if((DeviceStateBusiness.CoinCommunicate==0)&&(DeviceStateBusiness.Coinsensor==0)&&(DeviceStateBusiness.Cointubejam==0)&&(DeviceStateBusiness.Coinromchk==0)&&(DeviceStateBusiness.Coinrouting==0))
					{
						LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
					}
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 3:
				//硬币器
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.CoinCapion[SystemPara.Language],2);
				if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
				{
					if(DeviceStateBusiness.Coinjam)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Coinjam[SystemPara.Language]);
					if(DeviceStateBusiness.CoinremoveTube)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.CoinremoveTube[SystemPara.Language]);
					if(DeviceStateBusiness.Coindisable)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Coindisable[SystemPara.Language]);
					if(DeviceStateBusiness.CoinunknowError)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s[%#02x][%#02x]",UserMaintainErrorMenuList.CoinunknowError[SystemPara.Language],DeviceStateBusiness.CoinunknowError,DeviceStateBusiness.CoinunknowErrorLow);

					if((DeviceStateBusiness.Coinjam==0)&&(DeviceStateBusiness.CoinremoveTube==0)&&(DeviceStateBusiness.Coindisable==0)&&(DeviceStateBusiness.CoinunknowError==0))
					{
						LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
					}
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;	
			case 4:
				//Hopper找零器
				LCDPrintf(8,1,0,SystemPara.Language,UserMaintainErrorMenuList.HopperCapion[SystemPara.Language]);
				if(SystemPara.CoinChangerType == HOPPER_CHANGER)
				{
					if(DeviceStateBusiness.Hopper1State)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%s[%#02x]",UserMaintainErrorMenuList.Hopper1State[SystemPara.Language],ErrorState(DeviceStateBusiness.Hopper1State),DeviceStatePack.Hopper1StateChk);
					if(DeviceStateBusiness.Hopper2State)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%s[%#02x]",UserMaintainErrorMenuList.Hopper2State[SystemPara.Language],ErrorState(DeviceStateBusiness.Hopper2State),DeviceStatePack.Hopper2StateChk);
					if(DeviceStateBusiness.Hopper3State)
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%s[%#02x]",UserMaintainErrorMenuList.Hopper3State[SystemPara.Language],ErrorState(DeviceStateBusiness.Hopper3State),DeviceStatePack.Hopper3StateChk);

					if((DeviceStateBusiness.Hopper1State==0)&&(DeviceStateBusiness.Hopper2State==0)&&(DeviceStateBusiness.Hopper3State==0))
					{
						LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
					}
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;	
			case 5:
				//出货机构故障
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.GOCCapion[SystemPara.Language],1);
				if(SystemPara.GOCIsOpen == 1)
				{
					if(DeviceStateBusiness.GOCError)
					{
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.GOCError[SystemPara.Language]);
					}
					if(DeviceStateBusiness.GOCErrorSub)
					{
						LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.GOCErrorSub[SystemPara.Language]);
					}
				}
				if(DeviceStateBusiness.ColBoardError)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.ColBoardError[SystemPara.Language]);
				}
				if(DeviceStateBusiness.ColBoardErrorSub)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.ColBoardErrorSub[SystemPara.Language]);
				}

				if((DeviceStateBusiness.GOCError==0)&&(DeviceStateBusiness.GOCErrorSub==0)&&(DeviceStateBusiness.ColBoardError==0)&&(DeviceStateBusiness.ColBoardErrorSub==0))
				{
					LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 6:
				//出货机构故障
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.GOCCapion[SystemPara.Language],2);
				if(DeviceStateBusiness.Error_FMD)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_FMD[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_FUM)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_FUM[SystemPara.Language]);
				}				
				if(DeviceStateBusiness.Error_GOC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_GOC[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_FLD)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_FLD[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_FOD)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_FOD[SystemPara.Language]);
				}
				if((DeviceStateBusiness.Error_FMD==0)&&(DeviceStateBusiness.Error_FUM==0)&&(DeviceStateBusiness.Error_GOC==0)&&(DeviceStateBusiness.Error_FLD==0)&&(DeviceStateBusiness.Error_FOD==0))
				{
					LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 7:
				//出货机构故障
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.GOCCapion[SystemPara.Language],3);
				if(DeviceStateBusiness.Error_UDS)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_UDS[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_GCD)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_GCD[SystemPara.Language]);
				}				
				if(DeviceStateBusiness.Error_SOVB)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SOVB[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_SOMD1)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SOMD1[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Error_SOEC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SOEC[SystemPara.Language]);
				}
				if((DeviceStateBusiness.Error_UDS==0)&&(DeviceStateBusiness.Error_GCD==0)&&(DeviceStateBusiness.Error_SOVB==0)&&(DeviceStateBusiness.Error_SOMD1==0)&&(DeviceStateBusiness.Error_SOEC==0))
				{
					LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 8:
				//出货机构故障
				LCDPrintf(8,1,0,SystemPara.Language,"%s-%d",UserMaintainErrorMenuList.GOCCapion[SystemPara.Language],4);
				if(DeviceStateBusiness.Error_SFHG)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SFHG[SystemPara.Language]);
				}								
				if(DeviceStateBusiness.Error_SOFP)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SOFP[SystemPara.Language]);
				}				
				if(DeviceStateBusiness.Error_SOMD2)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Error_SOMD2[SystemPara.Language]);
				}
				if(DeviceStateBusiness.Emp_Gol)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.Emp_Gol[SystemPara.Language]);
				}
				if((DeviceStateBusiness.Error_SFHG==0)&&(DeviceStateBusiness.Error_SOFP==0)&&(DeviceStateBusiness.Error_SOMD2==0)&&(DeviceStateBusiness.Emp_Gol==0))
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
				}				
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;	
			case 9:
				//PC故障
				LCDPrintf(8,1,0,SystemPara.Language,"%s",UserMaintainErrorMenuList.PCCapion[SystemPara.Language]);
				if(PCError(1))
				{
					TracePC("\r\n App PCErrorScale");
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.scaleError[SystemPara.Language]);
				}								
				if(PCError(2))
				{
					TracePC("\r\n App PCErrorOffLine");
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,UserMaintainErrorMenuList.offLineError[SystemPara.Language]);
				}
				if((PCError(1)==0)&&(PCError(2)==0))
				{
					TracePC("\r\n App PCErrorOK");
					LCDPrintf(5,7,0,SystemPara.Language,UserMaintainErrorMenuList.DeviceOK[SystemPara.Language]);
				}
				LCDPrintf(231-8*strlen(UserMaintainErrorMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainErrorMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;	
			
		}
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return;
			}
			else if(Key == '>')
			{				
				if(pageNum < 9)
					pageNum++;
				break;
			}
			else if(Key == '<')
			{				
				if(pageNum > 0)
					pageNum--;
				break;
			}
			OSTimeDly(10);	
		}			
	}
}

/*********************************************************************************************************
** Function name:     	LogTransaction
** Descriptions:	    日常交易记录查询
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
void LogTransaction(void)
{	
	uint8_t pageNum = 0,lineNum = 3;
	unsigned char Key;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		lineNum = 3;
		switch(pageNum)
		{
			case 0:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);				
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Income[SystemPara.Language],LogPara.Income/100,LogPara.Income%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.NoteIncome[SystemPara.Language],LogPara.NoteIncome/100,LogPara.NoteIncome%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.CoinsIncome[SystemPara.Language],LogPara.CoinsIncome/100,LogPara.CoinsIncome%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.TotalChange[SystemPara.Language],LogPara.TotalChange/100,LogPara.TotalChange%100);								
				LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageDown[SystemPara.Language]);//翻页提示
				break;
			case 1:		
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);
				//LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.DoubtNum[SystemPara.Language],LogPara.DoubtNum);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Iou[SystemPara.Language],LogPara.Iou/100,LogPara.Iou%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_num[SystemPara.Language],LogPara.vpSuccessNum);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_money[SystemPara.Language],LogPara.vpSuccessMoney/100,LogPara.vpSuccessMoney%100);
				if(SystemPara.CashlessDeviceType != OFF_READERACCEPTER)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_card_money[SystemPara.Language],LogPara.vpCardMoney/100,LogPara.vpCardMoney%100);	
				}				
				LCDPrintf(231-8*strlen(LogTransMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 2:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);				
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_cash_num[SystemPara.Language],LogPara.vpCashNum);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_cash_money[SystemPara.Language],LogPara.vpCashMoney/100,LogPara.vpCashMoney%100);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_card_num[SystemPara.Language],LogPara.vpCardNum);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_card_money[SystemPara.Language],(LogPara.vpCardMoney)/100,(LogPara.vpCardMoney)%100);								
					LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				}
				break;
			case 3:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);	
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_pc_num[SystemPara.Language],LogPara.vpOnlineNum);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_pc_money[SystemPara.Language],LogPara.vpOnlineMoney/100,LogPara.vpOnlineMoney%100);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_game_num[SystemPara.Language],LogPara.vpGameNum);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_game_money[SystemPara.Language],LogPara.vpGameMoney/100,LogPara.vpGameMoney%100);		
				}
				LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			
		}
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return;
			}
			else if(Key == '>')
			{	
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					if(pageNum < 3)
						pageNum++;
				}
				else
				{
					if(pageNum < 1)
						pageNum++;
				}	
				break;
			}
			else if(Key == '<')
			{				
				if(pageNum > 0)
					pageNum--;
				break;
			}
			OSTimeDly(10);	
		}			
	}
	
}


/*********************************************************************************************************
** Function name:     	LogTransactionTotal
** Descriptions:	    总交易记录查询
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
void LogTransactionTotal(void)
{	
	uint8_t pageNum = 0,lineNum = 3;
	unsigned char Key;

	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDClrArea(1,4,238,14);
		lineNum = 3;
		switch(pageNum)
		{
			case 0:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);				
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Income[SystemPara.Language],LogPara.IncomeTotal/100,LogPara.IncomeTotal%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.NoteIncome[SystemPara.Language],LogPara.NoteIncomeTotal/100,LogPara.NoteIncomeTotal%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.CoinsIncome[SystemPara.Language],LogPara.CoinsIncomeTotal/100,LogPara.CoinsIncomeTotal%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.TotalChange[SystemPara.Language],LogPara.TotalChangeTotal/100,LogPara.TotalChangeTotal%100);								
				LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageDown[SystemPara.Language]);//翻页提示
				break;
			case 1:		
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);	
				//LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.DoubtNum[SystemPara.Language],LogPara.DoubtNum);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.Iou[SystemPara.Language],LogPara.Iou/100,LogPara.Iou%100);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_num[SystemPara.Language],LogPara.vpSuccessNumTotal);
				LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_money[SystemPara.Language],LogPara.vpSuccessMoneyTotal/100,LogPara.vpSuccessMoneyTotal%100);
				
				LCDPrintf(231-8*strlen(LogTransMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 2:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);	
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_cash_num[SystemPara.Language],LogPara.vpCashNumTotal);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_cash_money[SystemPara.Language],LogPara.vpCashMoneyTotal/100,LogPara.vpCashMoneyTotal%100);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_card_num[SystemPara.Language],LogPara.vpCardNumTotal);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_card_money[SystemPara.Language],(LogPara.vpCardMoneyTotal)/100,(LogPara.vpCardMoneyTotal)%100);
				}			
				LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;
			case 3:
				LCDPrintf(8,1,0,SystemPara.Language,LogTransMenuList.ModeCapion[SystemPara.Language]);	
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_pc_num[SystemPara.Language],LogPara.vpOnlineNumTotal);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_pc_money[SystemPara.Language],LogPara.vpOnlineMoneyTotal/100,LogPara.vpOnlineMoneyTotal%100);		
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%ld",LogTransMenuList.total_game_num[SystemPara.Language],LogPara.vpGameNumTotal);
					LCDPrintf(5,lineNum+=2,0,SystemPara.Language,"%s%02ld.%02ld",LogTransMenuList.total_game_money[SystemPara.Language],LogPara.vpGameMoneyTotal/100,LogPara.vpGameMoneyTotal%100);		
				}
				LCDPrintf(231-8*strlen(LogTransMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogTransMenuList.PageUp[SystemPara.Language]);//翻页提示
				break;

		}
		while(1)
		{
			Key = ReadKeyValue();
			if(Key == 'C')
			{				
				return;
			}
			else if(Key == '>')
			{	
				if(SystemPara.PcEnable==ZHIHUI_PC)
				{
					if(pageNum < 3)
						pageNum++;
				}
				else
				{
					if(pageNum < 1)
						pageNum++;
				}
				break;
			}
			else if(Key == '<')
			{				
				if(pageNum > 0)
					pageNum--;
				break;
			}
			OSTimeDly(10);	
		}			
	}	
}



/*********************************************************************************************************
** Function name:     	LogClearRecord
** Descriptions:	    清除交易记录
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
void LogClearRecord(void)
{	
	
	if(PasswordCheck())
	{
		LCDClrArea(1,4,238,14);
		if(SelectOKCheck())
		{
			pc_trade_info_clear(3);//add by yoc 2013.12.16
			LogClearAPI();
			ChannelClearSuccesCount();
		}
	}	
}


/*********************************************************************************************************
** Function name:     	LogRecordDetail
** Descriptions:	    交易记录明细
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
void LogRecordDetail(void)
{	
	
	//uint8_t	        buf[10];
	//uint16_t		index;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		LCDPrintf(8,3,0,SystemPara.Language,LogMaintainMenuList.printTrade[SystemPara.Language]);
		//LCDPrintf(16,9,0,SystemPara.Language,LogMaintainMenuList.RecordIng[SystemPara.Language]);
		/*
		Trace("PrintReading?");
		index = 0;
		for(i = 0;i < 10;i++)
		{	
			if(Uart0BuffIsNotEmpty())
			{
				while(Uart0BuffIsNotEmpty())
					buf[index++] = Uart0GetCh();
				return ;
			}
			OSTimeDly(10);
		}
		if(strncmp(buf,"Readying",sizeof("Readying")))//没有串口准备
		{
			Trace("%s",buf);
			return ;
		}
		
		*/		
		if(SelectOKCheck())//确认
		{
		
			return;
			
		}
		return;
		
	}
}



/*********************************************************************************************************
** Function name:     	LogMaintainProcess
** Descriptions:	    进入交易日志主菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogMaintainProcess(void)
{
	unsigned char Key;
	
	void (*TestFunctonPtr)(void);
	while(1)
	{
		LCDClrScreen();
		Trace("\r\nEnterLogFuction");	
		
		LogMaintainMainMenu.PageNumb	  = 0x00;
		LogMaintainMainMenu.KeyCurrent  = 0x00;
		LogMaintainMainMenu.ExtSelfCheck= 0x00;
		LogMaintainMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(LogMaintainMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,LogMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,LogMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,LogMaintainMenuList.ColumnRecord[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,LogMaintainMenuList.ClearRecord[SystemPara.Language]);
			//LCDPrintf(5,11,0,SystemPara.Language,LogMaintainMenuList.RecordTotal[SystemPara.Language]);
			//LCDPrintf(5,13,0,SystemPara.Language,LogMaintainMenuList.RecordPrint[SystemPara.Language]);
			//LCDPrintf(231-8*strlen(LogMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示
		}/*
		else
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,LogMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(16,5,0,SystemPara.Language,LogMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(16,7,0,SystemPara.Language,LogMaintainMenuList.ColumnRecord[SystemPara.Language]);
			LCDPrintf(16,9,0,SystemPara.Language,LogMaintainMenuList.ClearRecord[SystemPara.Language]);
			LCDPrintf(16,11,0,SystemPara.Language,LogMaintainMenuList.RecordTotal[SystemPara.Language]);
			LCDPrintf(16,13,0,SystemPara.Language,LogMaintainMenuList.RecordPrint[SystemPara.Language]);
			LCDPrintf(231-8*strlen(LogMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageUp[SystemPara.Language]);
		}*/
		
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	LogMaintainMainMenu.ExtSelfCheck = 0x01;
							Trace("\r\n c");
							break;
				case '<' :	/*
							if(LogMaintainMainMenu.PageNumb == 0x01)
							{
								LogMaintainMainMenu.PageNumb   = 0x00;
								LogMaintainMainMenu.KeyCurrent = 0x00;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,LogMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,LogMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,LogMaintainMenuList.ColumnRecord[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,LogMaintainMenuList.ClearRecord[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,LogMaintainMenuList.RecordTotal[SystemPara.Language]);
								LCDPrintf(16,13,0,SystemPara.Language,LogMaintainMenuList.RecordPrint[SystemPara.Language]);
								LCDPrintf(231-8*strlen(LogMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示	
							}
							Trace("\r\n <");*/
							break;
				case '>' :	/*
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.PageNumb   = 0x01;
								LogMaintainMainMenu.KeyCurrent = 0x00;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,LogMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,LogMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,LogMaintainMenuList.ColumnRecord[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,LogMaintainMenuList.ClearRecord[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,LogMaintainMenuList.RecordTotal[SystemPara.Language]);
								LCDPrintf(16,13,0,SystemPara.Language,LogMaintainMenuList.RecordPrint[SystemPara.Language]);
								LCDPrintf(231-8*strlen(LogMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageUp[SystemPara.Language]);	
							}
							Trace("\r\n >");*/
							break;
				case '1' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = LogTransaction;
							}
							Trace("\r\n 1");
							break;
				case '2' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = ChannelLookUpSuccessCount;
							}
							Trace("\r\n 2");
							break;
				case '3' :	
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = LogClearRecord;
							}
							Trace("\r\n 3");
							break;
				case '4' :	
							/*
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x04;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = LogTransactionTotal;
							}*/
							Trace("\r\n 4");
							break;
				case '5' :
							/*
							if(LogMaintainMainMenu.PageNumb == 0x00)
							{
								LogMaintainMainMenu.KeyCurrent = 0x02;
								LogMaintainMainMenu.ExtSelfCheck = 0x00;
								LogMaintainMainMenu.TestOperate = LogRecordDetail;
							}
							*/
							Trace("\r\n 5");
							break;				
				default  :	
							break;	
			}
			if(LogMaintainMainMenu.ExtSelfCheck == 0x01)
			{
				//Trace("\r\nSwitch exit ExtSelfCheck");
				break;
			}
			if(LogMaintainMainMenu.KeyCurrent != 0x00)
			{
				TestFunctonPtr = LogMaintainMainMenu.TestOperate;
				(*TestFunctonPtr)();//由此进入各个设备的自检函数
				break;	
			}
		}	
		if(LogMaintainMainMenu.ExtSelfCheck == 0x01)
		{
			Trace("\r\nSwitch exit ExtSelfCheck");
			break;
		}
	}
}


/*********************************************************************************************************
** Function name:     	AdminRPT
** Descriptions:	    进入工控机配置
** input parameters:    adminType:1加满全部货道,2货道存货数量修改,3补硬币完成,4全系统同步,8按层加满,9取纸币完成,10补货完成
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void AdminRPT(uint8_t adminType,uint8_t Column,uint8_t ColumnSum)
{
	uint8_t res = 0;
	if(SelectOKCheck())
	{
		while( 1 )
    	{	
			res = AdminRPTAPI(adminType,Column,ColumnSum);
			LCDClrScreen();
			if(res)
			{
				LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPSuccess[SystemPara.Language]);
			}
			else
			{
				if(adminType==4)
					LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPSyning[SystemPara.Language]);
				else
					LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPFail[SystemPara.Language]);
			}	
			OSTimeDly(OS_TICKS_PER_SEC * 2);
			if( (adminType==4)&&(res==0) )
				continue;
			break;
		}
	}
}


/*********************************************************************************************************
** Function name:     	AdminRPTWeihu
** Descriptions:	    补货配置
** input parameters:    adminType:1加满全部货道,2货道存货数量修改,3补硬币完成,4全系统同步,8按层加满,9取纸币完成,10补货完成
** output parameters:   无
** Returned value:      1ACK,0NAK
*********************************************************************************************************/
uint8_t AdminRPTWeihu(uint8_t adminType,uint8_t Column,uint8_t ColumnSum)
{
	uint8_t res = 0;	
	
	while( 1 )
	{	
		res = AdminRPTAPI(adminType,Column,ColumnSum);		
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		if(res)
		{
			LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPSuccess[SystemPara.Language]);
		}
		else
		{
			if(adminType==4)
				LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPSyning[SystemPara.Language]);
			else
				LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPFail[SystemPara.Language]);
		}	
		OSTimeDly(OS_TICKS_PER_SEC * 2);
		if( (adminType==4)&&(res==0) )
			continue;
		break;
	}
	return res;
}

/*********************************************************************************************************
** Function name:     	AdminRPTSIMPLE
** Descriptions:	    补货配置
** input parameters:    adminType:1加满全部货道,2按层加满,3按货道加满
** output parameters:   无
** Returned value:      1ACK,0NAK
*********************************************************************************************************/
uint8_t AdminRPTSIMPLE(uint8_t adminType,uint8_t Column,uint8_t ColumnSum)
{
	uint8_t res = 0;	
	
	while( 1 )
	{	
		LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPBuhuoing[SystemPara.Language]);
		res = AdminRPTSIMPLEAPI(adminType,Column,ColumnSum);		
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		if(res)
		{
			LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPSuccess[SystemPara.Language]);
		}
		else
		{
			
			LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPFail[SystemPara.Language]);
		}	
		OSTimeDly(OS_TICKS_PER_SEC * 2);
		if( (adminType==4)&&(res==0) )
			continue;
		break;
	}
	return res;
}





/*********************************************************************************************************
** Function name:     	PCProcess
** Descriptions:	    进入工控机配置菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void PCProcess(void *pvData)
{
	unsigned char Key;
		
	void (*TestFunctonPtr)(void);
	pvData = pvData;
	while(1)
	{
		LCDClrScreen();
		Trace("\r\nEnterUserSelfCheckFuction");	
		UserMaintainMainMenu.PageNumb	  = 0x00;
		UserMaintainMainMenu.KeyCurrent  = 0x00;
		UserMaintainMainMenu.ExtSelfCheck= 0x00;
		UserMaintainMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(UserMaintainMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserUboxPCMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,UserUboxPCMenuList.VPAddCoin[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,UserUboxPCMenuList.VPSynGoodsCol[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,UserUboxPCMenuList.VPAddBill[SystemPara.Language]);	
			LCDPrintf(5,11,0,SystemPara.Language,UserUboxPCMenuList.VPBuhuo[SystemPara.Language]);	
			//LCDPrintf(231-8*strlen(UserMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示
		}
		/*
		else
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
			LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
			LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
			LCDPrintf(231-8*strlen(UserMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageUp[SystemPara.Language]);
		}*/
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	//SelfCheckMenu.ExtSelfCheck = 0x01;
							return;
				case '<' :	/*
							if(UserMaintainMainMenu.PageNumb == 0x01)
							{
								UserMaintainMainMenu.PageNumb   = 0x00;
								UserMaintainMainMenu.KeyCurrent = 0x00;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
								LCDPrintf(231-8*strlen(UserMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示		
							}*/
							break;
				case '>' :	
							/*
							if(UserMaintainMainMenu.PageNumb == 0x00)
							{
								UserMaintainMainMenu.PageNumb   = 0x01;
								UserMaintainMainMenu.KeyCurrent = 0x00;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
								LCDPrintf(231-8*strlen(UserMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageUp[SystemPara.Language]);		
							}*/
							break;
				case '1' :	
							AdminRPT(3,0,0);
							UserMaintainMainMenu.ExtSelfCheck = 1;
							break;
				case '2' :	
							AdminRPT(4,0,0);
							UserMaintainMainMenu.ExtSelfCheck = 1;
							break;
				case '3' :	
							AdminRPT(9,0,0);
							UserMaintainMainMenu.ExtSelfCheck = 1;
							break;	
				case '4' :	
							AdminRPT(10,0,0);
							UserMaintainMainMenu.ExtSelfCheck = 1;
							break;				
				default  :	
							break;	
			}
			if(UserMaintainMainMenu.ExtSelfCheck == 0x01)
			{
				Trace("\r\nSwitch exit User");
				break;
			}
			if(UserMaintainMainMenu.KeyCurrent != 0x00)
			{
				Trace("\r\nEnter  TestFunctonPtr");
				TestFunctonPtr = UserMaintainMainMenu.TestOperate;
				(*TestFunctonPtr)();//由此进入各个设备的自检函数
				break;	
			}
		}			
	}
}


/*****************************************************************************
** Function name:	RTCParaMenu	
**
** Descriptions:	配置时间参数	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
void RTCParaMenu(void)
{	
	uint8_t flag = 0, resetPar = 0;
	unsigned char Key;
	uint32_t keyValue = 0;
	uint8_t PageNumb = 0x00;
	
	Trace("\r\n RTC3-Time");
	RTCReadTime(&RTCData);
	RTCData.second = 0;
	//RTCData.week = 1;
	//Trace("\r\n read=%d,%d,%d,%d,%d",RTCData.year,RTCData.month,RTCData.day,RTCData.hour,RTCData.minute);
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{
		if(flag == 0)
		{
			
			if(PageNumb == 0x00)
			{			
				//1.显示页面			
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,RtcMenuList.ModeCapion[SystemPara.Language]);
				LCDPrintf(5,5,0,SystemPara.Language,"%s%04d",RtcMenuList.year[SystemPara.Language],RTCData.year);
				LCDPrintf(5,7,0,SystemPara.Language,"%s%02d",RtcMenuList.month[SystemPara.Language],RTCData.month);
				LCDPrintf(5,9,0,SystemPara.Language,"%s%02d",RtcMenuList.day[SystemPara.Language],RTCData.day);
				LCDPrintf(5,11,0,SystemPara.Language,"%s%02d",RtcMenuList.hour[SystemPara.Language],RTCData.hour);
				LCDPrintf(5,13,0,SystemPara.Language,"%s%02d",RtcMenuList.minute[SystemPara.Language],RTCData.minute);	
				LCDPrintf(231-8*strlen(RtcMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,RtcMenuList.PageDown[SystemPara.Language]);//翻页提示
			}
			else if(PageNumb == 0x01)
			{			
				//1.显示页面			
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,SystemPara.Language,RtcMenuList.ModeCapion[SystemPara.Language]);
				LCDPrintf(5,5,0,SystemPara.Language,"%s%02d",RtcMenuList.week[SystemPara.Language],RTCData.week);
				LCDPrintf(231-8*strlen(RtcMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,RtcMenuList.PageUp[SystemPara.Language]);
			}
		}
		//2.捕捉按键
		while(1)
		{			
			Key = ReadKeyValue();			
			if(Key != 0x00)
				break;
		}
		switch(Key)
		{
			case 'C':
				if(flag >0)
					break;
				else
				{
					if(resetPar == 1)
					{
						//Trace("\r\n write=%d,%d,%d,%d,%d",RTCData.year,RTCData.month,RTCData.day,RTCData.hour,RTCData.minute);	
						RTCSetTime(&RTCData);
					}
					return;
				}
			case '>':	
				if(PageNumb < 1)
					PageNumb++;				
				break;				
			case '<':					
				if(PageNumb > 0)
					PageNumb--;
				
				break;
			case '1':
				if(flag == 0)
					flag = 1;
				break;
			case '2':
				if(flag == 0)
					flag = 2;
				break;
			case '3':
				if(flag == 0)
					flag = 3;
				break;
			case '4':
				if(flag == 0)
					flag = 4;
				break;
			case '5':
				if(flag == 0)
					flag = 5;
				break;
			default:
				break;					
		}			
		while(flag)
		{			
			//3.反白显示菜单,以及显示稍候的输入数字
			switch(flag)
			{
				case 1:
					//Trace("\r\n show=%ld",keyValue);	
					
					switch(PageNumb)
					{
						case 0: 
							LCDPrintf(5,5,1,SystemPara.Language,"%s%04d",RtcMenuList.year[SystemPara.Language],keyValue);
							break;
						case 1: 	
							LCDPrintf(5,5,1,SystemPara.Language,"%s%02d",RtcMenuList.week[SystemPara.Language],keyValue);
							break;						
					}
					
					break;
				case 2:					
					switch(PageNumb)
					{
						case 0: 
							LCDPrintf(5,7,1,SystemPara.Language,"%s%02d",RtcMenuList.month[SystemPara.Language],keyValue);
							break;
						case 1: 								
							break;						
					}
					break;	
				case 3:					
					switch(PageNumb)
					{
						case 0: 
							LCDPrintf(5,9,1,SystemPara.Language,"%s%02d",RtcMenuList.day[SystemPara.Language],keyValue);
							break;
						case 1: 								
							break;						
					}
					break;	
				case 4:					
					switch(PageNumb)
					{
						case 0: 
							LCDPrintf(5,11,1,SystemPara.Language,"%s%02d",RtcMenuList.hour[SystemPara.Language],keyValue);
							break;
						case 1: 								
							break;						
					}
					break;	
				case 5:
					switch(PageNumb)
					{
						case 0: 
							LCDPrintf(5,13,1,SystemPara.Language,"%s%02d",RtcMenuList.minute[SystemPara.Language],keyValue);
							break;
						case 1: 								
							break;						
					}							
					break;	
				default:
					break;						
			}
			//输入数字
			while(1)
			{			
				Key = GetKeyInValue();			
				if(Key != 0xff)
				{
					//4.显示输入的数字
					if(Key <= 0x09)
						keyValue = keyValue*10+Key;	
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//4.将输入数字保存到参数中
					else if(Key == 0x0f)//确定按键
					{
						switch(flag)
						{
							case 1:
								switch(PageNumb)
								{
									case 0: 
										RTCData.year=keyValue;
										break;
									case 1: 	
										RTCData.week=keyValue;
										break;						
								}								
								break;
							case 2:
								RTCData.month=keyValue;
								break;	
							case 3:
								RTCData.day=keyValue;
								break;	
							case 4:
								RTCData.hour=keyValue;
								break;	
							case 5:
								RTCData.minute=keyValue;		
								break;	
							default:
								break;						
						}
						//RTCData.week = 1;
						RTCData.second = 0;
						resetPar = 1;
						keyValue = 0;
						flag = 0;
					}
					//Trace("\r\n input=%ld,%d",keyValue,Key);
					break;
				}
			}
			
		}
		OSTimeDly(10);			
	}  
}

/*****************************************************************************
** Function name:	ChooseSet	
**
** Descriptions:	输入菜单值选择	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
char * ChooseSet(uint8_t flag)
{
	switch(flag)
	{
		case 0:	
			return UserChoose.OFF[SystemPara.Language];
		case 1:
			return UserChoose.ON[SystemPara.Language];	
		case 50:
			return UserChoose.Choose[SystemPara.Language];		
		default:
			return UserChoose.UNSET[SystemPara.Language];								
	}
}


/*****************************************************************************
** Function name:	RstUserPara	
**
** Descriptions:	配置用户参数	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
void RstUserPara(uint8_t pageNum,uint8_t flag,uint32_t keyValue)
{
	switch(flag)
	{
		case 1:	
			switch(pageNum)
			{
				case 0: 
					break;
				case 1: 	
					break;
				case 2: 
					SystemPara.BillValidatorType = keyValue;		
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 2:	
			switch(pageNum)
			{
				case 0: 													
					break;
				case 1: 	
					break;
				case 2: 
					SystemPara.CoinAcceptorType = keyValue;	
					SystemPara.CoinChangerType = keyValue;	
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 3:	
			switch(pageNum)
			{
				case 0:								
					break;
				case 1: 	
					break;
				case 2: 
					SystemPara.CashlessDeviceType = keyValue;	
					break;
				case 3:	
					SystemPara.BillEnableValue = keyValue;	
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 4:
			switch(pageNum)
			{
				case 0: 
					UserPara.TransMultiMode = keyValue;										
					break;
				case 1: 
					SystemPara.MaxValue = keyValue;	
					break;
				case 2: 
					SystemPara.SaleTime = keyValue;	
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 5:
			switch(pageNum)
			{
				case 0: 
					UserPara.TransEscape = keyValue;									
					break;
				case 1: 	
					break;
				case 2: 
					SystemPara.ColumnTime = keyValue;	
					break;
				case 3:
					break;	
				case 4:
					break;	
				
			}	
			break;	
		default:
			break;						
	}
}

/*********************************************************************************************************
** Function name:     	UserTradeMaintainProcess
** Descriptions:	    进入交易设置主菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UserTradeMaintainProcess(void)
{
	unsigned char Key,flag = 0, resetPar = 0,resetAcptorPar = 0;
	uint32_t keyValue = 0;
	unsigned char adminflag=0;
	
	void (*TestFunctonPtr)(void);
	UserTradeMainMenu.PageNumb	  = 0x00;
	UserTradeMainMenu.KeyCurrent  = 0x00;
	UserTradeMainMenu.ExtSelfCheck= 0x00;
	while(1)
	{
		LCDClrScreen();
		Trace("\r\nTradeFuction");	
		
		//UserTradeMainMenu.PageNumb	  = 0x00;
		UserTradeMainMenu.KeyCurrent  = 0x00;
		UserTradeMainMenu.ExtSelfCheck= 0x00;
		UserTradeMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(UserTradeMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.RTC[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.Compressor[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.LED[SystemPara.Language]);
			LCDPrintf(5,11,0,SystemPara.Language,"%s%s",UserTradeMenuList.Multivend[SystemPara.Language],ChooseSet(UserPara.TransMultiMode));
			LCDPrintf(5,13,0,SystemPara.Language,"%s%s",UserTradeMenuList.Refund[SystemPara.Language],ChooseSet(UserPara.TransEscape));
			//LCDPrintf(231-8*strlen(LogMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示
		}
		else if(UserTradeMainMenu.PageNumb == 0x01)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.BillValidatorTest[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.CoinAcceptorTest[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.CoinChangerTest[SystemPara.Language]);
			LCDPrintf(5,11,0,SystemPara.Language,"%s%d.%02d",UserTradeMenuList.MaxValue[SystemPara.Language],SystemPara.MaxValue/100,SystemPara.MaxValue%100);
			LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
		}
		else if(UserTradeMainMenu.PageNumb == 0x02)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,"%s%d",UserTradeMenuList.BillValidatorType[SystemPara.Language],SystemPara.BillValidatorType);
			LCDPrintf(5,7,0,SystemPara.Language,"%s%d",UserTradeMenuList.CoinAcceptorType[SystemPara.Language],SystemPara.CoinAcceptorType);
			LCDPrintf(5,9,0,SystemPara.Language,"%s%d",UserTradeMenuList.CashlessDeviceType[SystemPara.Language],SystemPara.CashlessDeviceType);
			if(SystemPara.PcEnable==SIMPUBOX_PC)
			{
				LCDPrintf(5,11,0,SystemPara.Language,"%s%d",UserTradeMenuList.SaleTime[SystemPara.Language],SystemPara.SaleTime);
				LCDPrintf(5,13,0,SystemPara.Language,"%s%d",UserTradeMenuList.ColumnTime[SystemPara.Language],SystemPara.ColumnTime);
				//LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
			}
		}
		else if(UserTradeMainMenu.PageNumb == 0x03)
		{
			
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,"%s",UserTradeMenuList.box_hot[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,"%s",UserTradeMenuList.box_chuchou[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,"%s%d.%02d",UserTradeMenuList.BillEnableValue[SystemPara.Language],SystemPara.BillEnableValue/100,SystemPara.BillEnableValue%100);
			LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
		}
		
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	UserTradeMainMenu.ExtSelfCheck = 0x01;
							if(resetPar == 1)
							{
								WriteUserSystemPara(UserPara,0);								
							}
							if(resetAcptorPar == 1)
							{
								//2.保存系统参数
								WriteDefaultSystemPara(SystemPara,0);							
							}
							//Trace("\r\n c");
							break;
				case '<' :	
							//Trace("\r\n <,page=%d",);
							if(UserTradeMainMenu.PageNumb == 0x03)
							{
								UserTradeMainMenu.PageNumb   = 0x02;
								UserTradeMainMenu.KeyCurrent = 0x00;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,"%s%d",UserTradeMenuList.BillValidatorType[SystemPara.Language],SystemPara.BillValidatorType);
								LCDPrintf(5,7,0,SystemPara.Language,"%s%d",UserTradeMenuList.CoinAcceptorType[SystemPara.Language],SystemPara.CoinAcceptorType);
								LCDPrintf(5,9,0,SystemPara.Language,"%s%d",UserTradeMenuList.CashlessDeviceType[SystemPara.Language],SystemPara.CashlessDeviceType);
								if(SystemPara.PcEnable==SIMPUBOX_PC)
								{
									LCDPrintf(5,11,0,SystemPara.Language,"%s%d",UserTradeMenuList.SaleTime[SystemPara.Language],SystemPara.SaleTime);
									LCDPrintf(5,13,0,SystemPara.Language,"%s%d",UserTradeMenuList.ColumnTime[SystemPara.Language],SystemPara.ColumnTime);
									//LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
								}
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								UserTradeMainMenu.PageNumb   = 0x01;
								UserTradeMainMenu.KeyCurrent = 0x00;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.BillValidatorTest[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.CoinAcceptorTest[SystemPara.Language]);
								LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.CoinChangerTest[SystemPara.Language]);
								LCDPrintf(5,11,0,SystemPara.Language,"%s%d.%02d",UserTradeMenuList.MaxValue[SystemPara.Language],SystemPara.MaxValue/100,SystemPara.MaxValue%100);
								LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
							}	
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								UserTradeMainMenu.PageNumb	  = 0x00;
								UserTradeMainMenu.KeyCurrent  = 0x00;
								UserTradeMainMenu.ExtSelfCheck= 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.RTC[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.Compressor[SystemPara.Language]);
								LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.LED[SystemPara.Language]);
								LCDPrintf(5,11,0,SystemPara.Language,"%s%s",UserTradeMenuList.Multivend[SystemPara.Language],ChooseSet(UserPara.TransMultiMode));
								LCDPrintf(5,13,0,SystemPara.Language,"%s%s",UserTradeMenuList.Refund[SystemPara.Language],ChooseSet(UserPara.TransEscape));
								//LCDPrintf(231-8*strlen(LogMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,LogMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示	
							}								 
							
							break;
				case '>' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								UserTradeMainMenu.PageNumb   = 0x01;
								UserTradeMainMenu.KeyCurrent = 0x00;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,UserTradeMenuList.BillValidatorTest[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,UserTradeMenuList.CoinAcceptorTest[SystemPara.Language]);
								LCDPrintf(5,9,0,SystemPara.Language,UserTradeMenuList.CoinChangerTest[SystemPara.Language]);
								LCDPrintf(5,11,0,SystemPara.Language,"%s%d.%02d",UserTradeMenuList.MaxValue[SystemPara.Language],SystemPara.MaxValue/100,SystemPara.MaxValue%100);
								LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
							}	
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								UserTradeMainMenu.PageNumb   = 0x02;
								UserTradeMainMenu.KeyCurrent = 0x00;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,"%s%d",UserTradeMenuList.BillValidatorType[SystemPara.Language],SystemPara.BillValidatorType);
								LCDPrintf(5,7,0,SystemPara.Language,"%s%d",UserTradeMenuList.CoinAcceptorType[SystemPara.Language],SystemPara.CoinAcceptorType);
								LCDPrintf(5,9,0,SystemPara.Language,"%s%d",UserTradeMenuList.CashlessDeviceType[SystemPara.Language],SystemPara.CashlessDeviceType);
								if(SystemPara.PcEnable==SIMPUBOX_PC)
								{
									LCDPrintf(5,11,0,SystemPara.Language,"%s%d",UserTradeMenuList.SaleTime[SystemPara.Language],SystemPara.SaleTime);
									LCDPrintf(5,13,0,SystemPara.Language,"%s%d",UserTradeMenuList.ColumnTime[SystemPara.Language],SystemPara.ColumnTime);
									//LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
								}
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								UserTradeMainMenu.PageNumb	  = 0x03;
								UserTradeMainMenu.KeyCurrent  = 0x00;
								UserTradeMainMenu.ExtSelfCheck= 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserTradeMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,"%s",UserTradeMenuList.box_hot[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,"%s",UserTradeMenuList.box_chuchou[SystemPara.Language]);	
								LCDPrintf(5,9,0,SystemPara.Language,"%s%d.%02d",UserTradeMenuList.BillEnableValue[SystemPara.Language],SystemPara.BillEnableValue/100,SystemPara.BillEnableValue%100);
								LCDPrintf(231-8*strlen(UserTradeMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTradeMenuList.PageUp[SystemPara.Language]);
							}
							break;
				case '1' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = RTCParaMenu;
							}
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = TestBillValidator;
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								if(flag == 0)
									flag = 1;
							}
							else if(UserTradeMainMenu.PageNumb == 0x03)
							{
								UserTradeMainMenu.KeyCurrent = 0x03;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = acdc_controlHot;
								resetPar = 1;
							}
							//Trace("\r\n 1");
							break;
				case '2' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = ACDCManageCompressor;
								resetPar = 1;
							}
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = TestCoinAcceptor;
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								if(flag == 0)
									flag = 2;
							}
							else if(UserTradeMainMenu.PageNumb == 0x03)
							{
								UserTradeMainMenu.KeyCurrent = 0x03;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = acdc_controlChuchou;
								resetPar = 1;
							}
							//Trace("\r\n 2");
							break;
				case '3' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = ACDCManageLight;
								resetPar = 1;
							}
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								UserTradeMainMenu.KeyCurrent = 0x02;
								UserTradeMainMenu.ExtSelfCheck = 0x00;
								UserTradeMainMenu.TestOperate = TestChanger;
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								if(flag == 0)
									flag = 3;
							}
							else if(UserTradeMainMenu.PageNumb == 0x03)
							{
								if(flag == 0)
									flag = 3;
							}
							//Trace("\r\n 3");
							break;
				case '4' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								if(flag == 0)
									flag = 4;
							}
							else if(UserTradeMainMenu.PageNumb == 0x01)
							{
								if(flag == 0)
									flag = 4;
							}
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								if(flag == 0)
									flag = 4;
							}
							//Trace("\r\n 4");
							break;
				case '5' :	
							if(UserTradeMainMenu.PageNumb == 0x00)
							{
								if(flag == 0)
									flag = 5;
							}		
							else if(UserTradeMainMenu.PageNumb == 0x02)
							{
								if(flag == 0)
									flag = 5;
							}
							//Trace("\r\n 5");
							break;				
				default  :	
							break;	
			}
			if(UserTradeMainMenu.ExtSelfCheck == 0x01)
			{
				//Trace("\r\nSwitch exit ExtSelfCheck");
				break;
			}
			if(UserTradeMainMenu.KeyCurrent != 0x00)
			{
				TestFunctonPtr = UserTradeMainMenu.TestOperate;
				(*TestFunctonPtr)();//由此进入各个设备的自检函数
				break;	
			}
			if(flag > 0)
			{
				break;
			}
		}	
		//退出
		if(UserTradeMainMenu.ExtSelfCheck == 0x01)
		{
			Trace("\r\nSwitch exit ExtSelfCheck");
			break;
		}
		while(flag)
		{			
			//3.反白显示菜单,以及显示稍候的输入数字
			switch(flag)
			{
				case 1:
					switch(UserTradeMainMenu.PageNumb)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2:
							LCDPrintf(5,5,1,SystemPara.Language,"%s%d",UserTradeMenuList.BillValidatorType[SystemPara.Language],keyValue);
							break;
						case 3:																	
							break;	
						case 4:													
							break;	
					}
					break;
				case 2:
					switch(UserTradeMainMenu.PageNumb)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2:
							LCDPrintf(5,7,1,SystemPara.Language,"%s%d",UserTradeMenuList.CoinAcceptorType[SystemPara.Language],keyValue);
							break;
						case 3:																	
							break;	
						case 4:													
							break;	
					}
					break;
				case 3:
					switch(UserTradeMainMenu.PageNumb)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2:
							LCDPrintf(5,9,1,SystemPara.Language,"%s%d",UserTradeMenuList.CashlessDeviceType[SystemPara.Language],keyValue);
							break;
						case 3:		
							LCDPrintf(5,9,1,SystemPara.Language,"%s%ld",UserTradeMenuList.BillEnableValue[SystemPara.Language],keyValue);
							break;	
						case 4:													
							break;	
					}
					break;
				case 4:
					switch(UserTradeMainMenu.PageNumb)
					{
						case 0: 
							LCDPrintf(5,11,1,SystemPara.Language,"%s%s",UserTradeMenuList.Multivend[SystemPara.Language],ChooseSet(50));
							break;
						case 1: 	
							LCDPrintf(5,11,1,SystemPara.Language,"%s%ld",UserTradeMenuList.MaxValue[SystemPara.Language],keyValue);
							break;
						case 2:
							LCDPrintf(5,11,1,SystemPara.Language,"%s%d",UserTradeMenuList.SaleTime[SystemPara.Language],keyValue);
							break;
						case 3:																	
							break;	
						case 4:													
							break;	
					}
					break;
				case 5:
					switch(UserTradeMainMenu.PageNumb)
					{
						case 0: 
							LCDPrintf(5,13,1,SystemPara.Language,"%s%s",UserTradeMenuList.Refund[SystemPara.Language],ChooseSet(50));							
							break;
						case 1: 
							break;
						case 2:
							LCDPrintf(5,13,1,SystemPara.Language,"%s%d",UserTradeMenuList.ColumnTime[SystemPara.Language],keyValue);
							break;
						case 3:																	
							break;	
						case 4:													
							break;	
					}
					break;
			}
			//输入数字
			while(1)
			{			
				Key = GetKeyInValue();			
				if(Key != 0xff)
				{
					/*
					//4.显示输入的数字
					if(Key <= 0x09)
						keyValue = keyValue*10+Key;	
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//4.将输入数字保存到参数中
					else if(Key == 0x0f)//确定按键
					*/
					if(Key <= 0x09)
					{
						keyValue = keyValue*10+Key;	
						if( ((flag==4)&&(UserTradeMainMenu.PageNumb==1))
							||((flag==3)&&(UserTradeMainMenu.PageNumb==3))
							||((flag==4)&&(UserTradeMainMenu.PageNumb==2))
							||((flag==5)&&(UserTradeMainMenu.PageNumb==2))
						  )
						{
						}
						else
						{
							RstUserPara(UserTradeMainMenu.PageNumb,flag,keyValue);
							if(
								((flag==1)&&(UserTradeMainMenu.PageNumb==2))
								||((flag==2)&&(UserTradeMainMenu.PageNumb==2))
								||((flag==3)&&(UserTradeMainMenu.PageNumb==2))								
							)
							{
								resetAcptorPar = 1;
							}
							else
							{
								resetPar = 1;
							}
							keyValue = 0;
							flag = 0;
						}
					}
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//4.将输入数字保存到参数中
					else if(Key == 0x0f)//确定按键
					{
						if( 
							((flag==4)&&(UserTradeMainMenu.PageNumb==2)) 
							||((flag==5)&&(UserTradeMainMenu.PageNumb==2)) 
						  )
						{
							if(SystemPara.PcEnable==SIMPUBOX_PC)
							{
								if(flag==4)
								{
									//同步到pc机上这个货道的存货数量
									adminflag=AdminRPTSIMPLEAPI(5,0,keyValue);
								}
								else if(flag==5)
								{
									//同步到pc机上这个货道的存货数量
									adminflag=AdminRPTSIMPLEAPI(4,0,keyValue);
								}
								if(adminflag)
								{
									RstUserPara(UserTradeMainMenu.PageNumb,flag,keyValue);
									resetAcptorPar = 1;
								}
								keyValue = 0;
								flag = 0;
							}
						}
						else if( ((flag==4)&&(UserTradeMainMenu.PageNumb==1))
							||((flag==3)&&(UserTradeMainMenu.PageNumb==3)) 							
						  )
						{
							RstUserPara(UserTradeMainMenu.PageNumb,flag,keyValue);
							resetAcptorPar = 1;
							keyValue = 0;
							flag = 0;
						}						
					}
					//Trace("\r\n input=%ld,%d",keyValue,Key);
					break;
				}
			}
		}
	}
}


/*****************************************************************************
** Function name:	RstUserPara	
**
** Descriptions:	配置调试参数	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
void RstTracePara(uint8_t pageNum,uint8_t flag,uint32_t keyValue)
{
	switch(flag)
	{
		case 1:	
			switch(pageNum)
			{
				case 0: 
					UserPara.billTrace = keyValue;										
					break;
				case 1: 
					UserPara.compressorTrace = keyValue;	
					break;
				case 2: 
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;
		case 2:	
			switch(pageNum)
			{
				case 0: 
					UserPara.coinTrace = keyValue;										
					break;
				case 1: 	
					UserPara.ledTrace = keyValue;	
					break;
				case 2: 
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;
		case 3:	
			switch(pageNum)
			{
				case 0: 
					UserPara.changelTrace = keyValue;										
					break;
				case 1: 
					UserPara.selectTrace = keyValue;
					break;
				case 2: 
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 4:
			switch(pageNum)
			{
				case 0: 
					UserPara.readerTrace = keyValue;										
					break;
				case 1: 	
					UserPara.PC = keyValue;
					break;
				case 2: 
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 5:
			switch(pageNum)
			{
				case 0: 
					UserPara.channelTrace = keyValue;									
					break;
				case 1: 	
					UserPara.TraceFlag = keyValue;
					break;
				case 2: 
					break;
				case 3:
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 6:
			switch(pageNum)
			{
				case 0: 														
					break;
				case 1: 	
					break;
				case 2: 
					break;
				case 3:						
					break;	
				case 4:
					break;	
				
			}	
			break;	
		case 7:
			switch(pageNum)
			{
				case 0: 													
					break;
				case 1: 					
					break;
				case 2: 
					break;
				case 3:
					break;	
				case 4:
					break;	
				
			}	
			break;	
		default:
			break;						
	}
}

/*********************************************************************************************************
** Function name:     	TraceMaintainProcess
** Descriptions:	    进入Trace参数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TraceMaintainProcess(void)
{	
	uint8_t pageNum = 0,flag = 0, resetPar = 0;
	unsigned char Key;
	uint32_t keyValue = 0;

	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	while(1)
	{		
		if(flag == 0)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,"%s",UserTraceMenuList.ModeCapion[SystemPara.Language]);
			//1.显示页面
			switch(pageNum)
			{
				case 0: 
					LCDPrintf(5,5,0,SystemPara.Language,"%s%s",UserTraceMenuList.billTrace[SystemPara.Language],ChooseSet(UserPara.billTrace));
					LCDPrintf(5,7,0,SystemPara.Language,"%s%s",UserTraceMenuList.coinTrace[SystemPara.Language],ChooseSet(UserPara.coinTrace));
					LCDPrintf(5,9,0,SystemPara.Language,"%s%s",UserTraceMenuList.changelTrace[SystemPara.Language],ChooseSet(UserPara.changelTrace));
					LCDPrintf(5,11,0,SystemPara.Language,"%s%s",UserTraceMenuList.readerTrace[SystemPara.Language],ChooseSet(UserPara.readerTrace));
					LCDPrintf(5,13,0,SystemPara.Language,"%s%s",UserTraceMenuList.channelTrace[SystemPara.Language],ChooseSet(UserPara.channelTrace));
										
					LCDPrintf(231-8*strlen(UserTraceMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserTraceMenuList.PageDown[SystemPara.Language]);//翻页提示			
					break;
				case 1: 
					LCDPrintf(5,5,0,SystemPara.Language,"%s%s",UserTraceMenuList.compressorTrace[SystemPara.Language],ChooseSet(UserPara.compressorTrace));
					LCDPrintf(5,7,0,SystemPara.Language,"%s%s",UserTraceMenuList.ledTrace[SystemPara.Language],ChooseSet(UserPara.ledTrace));
					LCDPrintf(5,9,0,SystemPara.Language,"%s%s",UserTraceMenuList.selectTrace[SystemPara.Language],ChooseSet(UserPara.selectTrace));
					LCDPrintf(5,11,0,SystemPara.Language,"%s%s",UserTraceMenuList.PCTrace[SystemPara.Language],ChooseSet(UserPara.PC));
					LCDPrintf(5,13,0,SystemPara.Language,"%s%s",UserTraceMenuList.TraceFlag[SystemPara.Language],ChooseSet(UserPara.TraceFlag));
					LCDPrintf(231-8*strlen(UserTraceMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserTraceMenuList.PageUp[SystemPara.Language]);//翻页提示
					break;
				case 2: 					
					break;
				case 3:								
					break;	
				case 4:					
					break;	
				
			}
		}
		//2.捕捉按键
		while(1)
		{			
			Key = ReadKeyValue();			
			if(Key != 0x00)
				break;
		}
		switch(Key)
		{
			case 'C':
				if(flag >0)
					break;
				else
				{
					if(resetPar == 1)
					{
						WriteUserSystemPara(UserPara,0);							
					}
					return;
				}
			case '>':
				if(pageNum < 1)
					pageNum++;
				break;				
			case '<':
				if(pageNum > 0)
					pageNum--;
				break;
			case '1':
				if(flag == 0)
					flag = 1;
				break;
			case '2':
				if(flag == 0)
					flag = 2;
				break;
			case '3':
				if(flag == 0)
					flag = 3;
				break;
			case '4':
				if(flag == 0)
					flag = 4;
				break;
			case '5':
				if(flag == 0)
					flag = 5;
				break;
			case '6':				
				break;
			case '7':				
				break;
			case '8':
				break;
			default:
				break;					
		}			
		while(flag)
		{			
			//3.反白显示菜单,以及显示稍候的输入数字
			switch(flag)
			{
				case 1:
					//Trace("\r\n show=%ld",keyValue);	
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(5,5,1,SystemPara.Language,"%s%s",UserTraceMenuList.billTrace[SystemPara.Language],ChooseSet(50));
							break;
						case 1: 	
							LCDPrintf(5,5,1,SystemPara.Language,"%s%s",UserTraceMenuList.compressorTrace[SystemPara.Language],ChooseSet(50));
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
					}
					break;
				case 2:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(5,7,1,SystemPara.Language,"%s%s",UserTraceMenuList.coinTrace[SystemPara.Language],ChooseSet(50));										
							break;
						case 1: 	
							LCDPrintf(5,7,1,SystemPara.Language,"%s%s",UserTraceMenuList.ledTrace[SystemPara.Language],ChooseSet(50));
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 3:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(5,9,1,SystemPara.Language,"%s%s",UserTraceMenuList.changelTrace[SystemPara.Language],ChooseSet(50));									
							break;
						case 1: 
							LCDPrintf(5,9,1,SystemPara.Language,"%s%s",UserTraceMenuList.selectTrace[SystemPara.Language],ChooseSet(50));
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 4:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(5,11,1,SystemPara.Language,"%s%s",UserTraceMenuList.readerTrace[SystemPara.Language],ChooseSet(50));										
							break;
						case 1: 
							LCDPrintf(5,11,1,SystemPara.Language,"%s%s",UserTraceMenuList.PCTrace[SystemPara.Language],ChooseSet(50));
							break;
						case 2: 
							break;
						case 3: 
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 5:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(5,13,1,SystemPara.Language,"%s%s",UserTraceMenuList.channelTrace[SystemPara.Language],ChooseSet(50));									
							break;
						case 1: 
							LCDPrintf(5,13,1,SystemPara.Language,"%s%s",UserTraceMenuList.TraceFlag[SystemPara.Language],ChooseSet(50));
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 6:
					switch(pageNum)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 7:
					switch(pageNum)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;	
				case 8:
					switch(pageNum)
					{
						case 0: 
							break;
						case 1: 
							break;
						case 2: 
							break;
						case 3:
							break;	
						case 4:
							break;	
						
					}	
					break;						
			}
			//输入数字
			while(1)
			{			
				Key = GetKeyInValue();			
				if(Key != 0xff)
				{
					/*
					//4.显示输入的数字
					if(Key <= 0x09)
						keyValue = keyValue*10+Key; 
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//4.将输入数字保存到参数中
					else if(Key == 0x0f)//确定按键
					*/
					if(Key <= 0x09)
					{
						keyValue = keyValue*10+Key; 
						RstTracePara(pageNum,flag,keyValue);
						resetPar = 1;
						keyValue = 0;
						flag = 0;
					}
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//Trace("\r\n input=%ld,%d",keyValue,Key);
					break;
				}
			}
			
		}
		OSTimeDly(10);			
	}  
}



/*********************************************************************************************************
** Function name:     	IsAdminSet
** Descriptions:	    管理员模式密码设置
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
uint8_t IsAdminSet(uint8_t NowKey)
{
	static uint8_t  adminSet=0;
	if((NowKey=='8')&&(adminSet==0))
	{
		adminSet++;
		Trace("\r\n set=%d,8",adminSet);
	}
	else if((NowKey=='3')&&(adminSet==1))
	{
		adminSet++;
		Trace("\r\n set=%d,3",adminSet);
	}
	else if((NowKey=='7')&&(adminSet==2))
	{
		adminSet++;
		Trace("\r\n set=%d,7",adminSet);
	}
	else if((NowKey=='1')&&(adminSet==3))
	{
		adminSet++;
		Trace("\r\n set=%d,1",adminSet);
	}
	else if((NowKey=='8')&&(adminSet==4))
	{
		adminSet++;
		Trace("\r\n set=%d,8",adminSet);
	}
	else if((NowKey=='5')&&(adminSet==5))
	{
		adminSet++;
		Trace("\r\n set=%d,5",adminSet);
	}
	else if((NowKey=='5')&&(adminSet==6))
	{
		adminSet++;
		Trace("\r\n set=%d,5",adminSet);
	}
	else if((NowKey=='7')&&(adminSet==7))
	{
		Trace("\r\n set=%d,7",adminSet);
		adminSet=0;		
		return 1;
	}	
	else
	{
		adminSet = 0;
	}
	return 0;
}

/*********************************************************************************************************
** Function name:     	IsFactorySet
** Descriptions:	    保存为出厂配置密码设置
** input parameters:    无
** output parameters:   无
** Returned value:      1进入
*********************************************************************************************************/
uint8_t IsFactorySet(uint8_t NowKey)
{
	static uint8_t  adminSet=0;
	if((NowKey=='8')&&(adminSet==0))
	{
		adminSet++;
		Trace("\r\n set=%d,8",adminSet);
	}
	else if((NowKey=='3')&&(adminSet==1))
	{
		adminSet++;
		Trace("\r\n set=%d,3",adminSet);
	}
	else if((NowKey=='7')&&(adminSet==2))
	{
		adminSet++;
		Trace("\r\n set=%d,7",adminSet);
	}
	else if((NowKey=='0')&&(adminSet==3))
	{
		adminSet++;
		Trace("\r\n set=%d,0",adminSet);
	}
	else if((NowKey=='2')&&(adminSet==4))
	{
		adminSet++;
		Trace("\r\n set=%d,2",adminSet);
	}
	else if((NowKey=='1')&&(adminSet==5))
	{
		adminSet++;
		Trace("\r\n set=%d,1",adminSet);
	}
	else if((NowKey=='5')&&(adminSet==6))
	{
		adminSet++;
		Trace("\r\n set=%d,5",adminSet);
	}
	else if((NowKey=='3')&&(adminSet==7))
	{
		Trace("\r\n set=%d,3",adminSet);
		adminSet=0;		
		return 1;
	}	
	else
	{
		adminSet = 0;
	}
	return 0;
}


/*********************************************************************************************************
** Function name:     	MaintainUserProcess
** Descriptions:	    进入用户模式菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MaintainUserProcess(void *pvData)
{
	unsigned char Key;
	unsigned char adminMode = 0;
	unsigned char adminflag=0;
	
	void (*TestFunctonPtr)(void);
	pvData = pvData;
	while(1)
	{
		LCDClrScreen();
		Trace("\r\nEnterUserSelfCheckFuction");	
		UserMaintainMainMenu.PageNumb	  = 0x00;
		UserMaintainMainMenu.KeyCurrent  = 0x00;
		UserMaintainMainMenu.ExtSelfCheck= 0x00;
		UserMaintainMainMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(UserMaintainMainMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
			LCDPrintf(5,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
			LCDPrintf(5,13,0,SystemPara.Language,UserMaintainMenuList.Trace[SystemPara.Language]);
			//LCDPrintf(231-8*strlen(UserMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示
		}
		/*
		else
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
			LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
			LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
			LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
			LCDPrintf(231-8*strlen(UserMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageUp[SystemPara.Language]);
		}*/
		while(1)
		{
			Key = ReadKeyValue();
			//Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	//SelfCheckMenu.ExtSelfCheck = 0x01;
							return;
				case '<' :	/*
							if(UserMaintainMainMenu.PageNumb == 0x01)
							{
								UserMaintainMainMenu.PageNumb   = 0x00;
								UserMaintainMainMenu.KeyCurrent = 0x00;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
								LCDPrintf(231-8*strlen(UserMaintainMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageDown[SystemPara.Language]);//翻页提示		
							}*/
							break;
				case '>' :	
							/*
							if(UserMaintainMainMenu.PageNumb == 0x00)
							{
								UserMaintainMainMenu.PageNumb   = 0x01;
								UserMaintainMainMenu.KeyCurrent = 0x00;
								UserMaintainMainMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,UserMaintainMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(16,5,0,SystemPara.Language,UserMaintainMenuList.Record[SystemPara.Language]);
								LCDPrintf(16,7,0,SystemPara.Language,UserMaintainMenuList.Error[SystemPara.Language]);
								LCDPrintf(16,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
								LCDPrintf(16,11,0,SystemPara.Language,UserMaintainMenuList.Column[SystemPara.Language]);
								LCDPrintf(231-8*strlen(UserMaintainMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,UserMaintainMenuList.PageUp[SystemPara.Language]);		
							}*/
							break;
				case '1' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x01;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = LogMaintainProcess;
								}
							}
							break;
				case '2' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x02;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = ErrorCheck;
								}
							}
							break;
				case '3' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									//判断能否得到pc参数
									if(SystemPara.PcEnable==SIMPUBOX_PC)
									{
										adminflag=GetAdminSIMPLEAPI(4,0);
										//adminflag=GetAdminSIMPLEAPI(5,0);
										if(adminflag==0)
										{
								
											LCDPrintf(5,9,0,SystemPara.Language,"参数取失败");
											OSTimeDly(OS_TICKS_PER_SEC * 2);
											LCDPrintf(5,9,0,SystemPara.Language,UserMaintainMenuList.System[SystemPara.Language]);
										}	
										else
										{
											UserMaintainMainMenu.KeyCurrent = 0x03;
											UserMaintainMainMenu.ExtSelfCheck = 0x00;
											UserMaintainMainMenu.TestOperate = UserTradeMaintainProcess;	
										}
									}
									else
									{
										UserMaintainMainMenu.KeyCurrent = 0x03;
										UserMaintainMainMenu.ExtSelfCheck = 0x00;									
										UserMaintainMainMenu.TestOperate = UserTradeMaintainProcess;	
									}
								}
							}
							break;
				case '4' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x04;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = ChannelManage;
								}
							}
							break;
				case '5' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x05;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = TraceMaintainProcess;
								}
							}
							break;	
				case '7' :	
							if(adminMode)
							{
								if(IsAdminSet(Key))
								{
									//Trace("\r\nInput Admin");
									adminMode = 0;
									LCDClrScreen();
									MaintainProcess((void*)0);
								}
							}
							else
							{
								/*
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x05;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = TestCashless;
								}
								*/
							}
							break;				
				case '8' :	
							if(adminMode)
							{
								IsAdminSet(Key);
							}
							else
							{
								/*
								if(UserMaintainMainMenu.PageNumb == 0x00)
								{
									UserMaintainMainMenu.KeyCurrent = 0x05;
									UserMaintainMainMenu.ExtSelfCheck = 0x00;
									UserMaintainMainMenu.TestOperate = TestCashless;
								}
								*/
							}
							break;			
				case '.' :  
							adminMode = (adminMode == 0)?1:0;
							//Trace("\r\n ad=%d",adminMode);
							break;
				default  :	
							break;	
			}
			if(UserMaintainMainMenu.ExtSelfCheck == 0x01)
			{
				Trace("\r\nSwitch exit User");
				break;
			}
			if(UserMaintainMainMenu.KeyCurrent != 0x00)
			{
				Trace("\r\nEnter  TestFunctonPtr");
				TestFunctonPtr = UserMaintainMainMenu.TestOperate;
				(*TestFunctonPtr)();//由此进入各个设备的自检函数
				break;	
			}
		}	
		if(UserMaintainMainMenu.ExtSelfCheck == 0x01)
		{
			Trace("\r\nSwitch exit User");
			break;
		}
	}
}

/*********************************************************************************************************
** Function name:     	MaitainProcess
** Descriptions:	    维护任务
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MaintainProcess(void *pvData)
{
	unsigned char Key;	
	unsigned char adminMode = 0;
		
	void (*KeyFunctonPtr)(void);
	pvData = pvData;
	while(1)
	{
		MaintainMainMenu.KeyCurrentState= 0x00;
		MaintainMainMenu.KeyEnterState  = 0x00;
		MaintainMainMenu.CurrentOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDPrintf(8,1,0,SystemPara.Language,MaintainMenuList.ModeCapion[SystemPara.Language]);
		LCDPrintf((231-8*strlen(SOFTWAREVERSION)),1,0,0x01,SOFTWAREVERSION);
		LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
		LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
		LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
		LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
		LCDPrintf(20,12,0,SystemPara.Language,MaintainMenuList.MenuCapion[SystemPara.Language]);
		while(1)
		{			
			Key = ReadKeyValue();	
			switch(Key)
			{
				case '1':
							if(adminMode)
							{
								IsFactorySet(Key);
							}
							else
							{
								MaintainMainMenu.KeyCurrentState = 0x01;
								MaintainMainMenu.KeyEnterState   = 0x00;
								MaintainMainMenu.CurrentOperate	 = EnterSelfCheckFuction;
							}
							break;
				case '2':	
							if(adminMode)
							{
								IsFactorySet(Key);
							}
							else
							{
								MaintainMainMenu.KeyCurrentState = 0x02;
								MaintainMainMenu.KeyEnterState   = 0x00;
								MaintainMainMenu.CurrentOperate	 = DefaultSystemParaMenu;
							}
							break;
				case '3':	
							if(adminMode)
							{
								if(IsFactorySet(Key))
								{
									//Trace("\r\nInput Admin");
									adminMode = 0;
									LCDClrScreen();
									MaintainMainMenu.CurrentOperate	 = RstFactorySetting;
									MaintainMainMenu.KeyEnterState = 0x00;
									KeyFunctonPtr = MaintainMainMenu.CurrentOperate;
									(*KeyFunctonPtr)();//由此进入各个子菜单
									MaintainMainMenu.KeyCurrentState= 0x00;
									MaintainMainMenu.KeyEnterState  = 0x00;
									MaintainMainMenu.CurrentOperate = (void *)0;
									LCDClrScreen();
									LCDDrawRectangle(0,0,239,15);
									LCDDrawRectangle(0,0,239,3);
									LCDPrintf(8,1,0,SystemPara.Language,MaintainMenuList.ModeCapion[SystemPara.Language]);
									LCDPrintf((231-8*strlen(SOFTWAREVERSION)),1,0,0x01,SOFTWAREVERSION);
									LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
									LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
									LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
									LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
									LCDPrintf(20,12,0,SystemPara.Language,MaintainMenuList.MenuCapion[SystemPara.Language]);
								}
							}
							else
							{
								MaintainMainMenu.KeyCurrentState = 0x03;
								MaintainMainMenu.KeyEnterState   = 0x00;
								MaintainMainMenu.CurrentOperate	 = ChannelAttributeManage;
							}
							break;
				case '4':	
							if(adminMode)
							{
								IsFactorySet(Key);
							}
							else
							{
								MaintainMainMenu.KeyCurrentState = 0x04;
								MaintainMainMenu.KeyEnterState   = 0x00;
								MaintainMainMenu.CurrentOperate	 = FactorySetting;
							}
							break;
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '0':
							if(adminMode)
							{
								IsFactorySet(Key);
							}
							else
							{								
							}
							break;
				case '<':	switch(MaintainMainMenu.KeyCurrentState)
							{
								case 0x00:	MaintainMainMenu.KeyCurrentState = 0x01;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = EnterSelfCheckFuction;
											break;
								case 0x01:	MaintainMainMenu.KeyCurrentState = 0x04;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = FactorySetting;
											break;
								case 0x02:	MaintainMainMenu.KeyCurrentState = 0x01;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = EnterSelfCheckFuction;
											break;
								case 0x03:	MaintainMainMenu.KeyCurrentState = 0x02;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = DefaultSystemParaMenu;
											break;
								case 0x04:	MaintainMainMenu.KeyCurrentState = 0x03;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = ChannelAttributeManage;
								default:	break;	

							}
							break;
				case '>':	switch(MaintainMainMenu.KeyCurrentState)
							{
								case 0x00:	MaintainMainMenu.KeyCurrentState = 0x04;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = FactorySetting;
											break;
								case 0x01:	MaintainMainMenu.KeyCurrentState = 0x02;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = DefaultSystemParaMenu;
											break;
								case 0x02:	MaintainMainMenu.KeyCurrentState = 0x03;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = ChannelAttributeManage;
											break;
								case 0x03:	MaintainMainMenu.KeyCurrentState = 0x04;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = FactorySetting;
											break;
								case 0x04:	MaintainMainMenu.KeyCurrentState = 0x01;
											MaintainMainMenu.KeyEnterState   = 0x00;
											MaintainMainMenu.CurrentOperate	 = EnterSelfCheckFuction;
								default:	break;	

							}
							break;
				case 'E' :	
							MaintainMainMenu.KeyEnterState  = 0x01;
							break;	
				case 'C' :	//SelfCheckMenu.ExtSelfCheck = 0x01;
							return;	
				case '.' :  
							adminMode = (adminMode == 0)?1:0;
							//Trace("\r\n ad=%d",adminMode);
							break;			
				default	:	
							break;
			}
			if((MaintainMainMenu.KeyCurrentState != 0x00)&&(MaintainMainMenu.KeyEnterState == 0x00))
			{
				switch(MaintainMainMenu.KeyCurrentState)
				{
					case 0x00:	LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
								LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
								LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
								LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
								LCDClrArea(1,12,238,14);
								LCDPrintf(20,12,0,SystemPara.Language,MaintainMenuList.MenuCapion[SystemPara.Language]);
								break;
					case 0x01:	LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,1);
								LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
								LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
								LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
								LCDClrArea(1,12,238,14);
								LCDPrintf(8,12,0,SystemPara.Language,MaintainMenuList.SelfCheckCapion[SystemPara.Language]);
								break;
					case 0x02:	LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
								LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,1);
								LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
								LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
								LCDClrArea(1,12,238,14);
								LCDPrintf(64,12,0,SystemPara.Language,MaintainMenuList.SystemSetCapion[SystemPara.Language]);
								break;
					case 0x03:	LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
								LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
								LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,1);
								LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,0);
								LCDClrArea(1,12,238,14);
								LCDPrintf((183-8*strlen(MaintainMenuList.AppSetCapion[SystemPara.Language])),12,0,SystemPara.Language,MaintainMenuList.AppSetCapion[SystemPara.Language]);
								break;
					case 0x04:	LCDPutBmp(16,6,DEVICESELFCHECKPIC.Width,DEVICESELFCHECKPIC.Hight,DEVICESELFCHECKPIC.Data,0);
								LCDPutBmp(72,6,DEVICEMANAGEPIC.Width,DEVICEMANAGEPIC.Hight,DEVICEMANAGEPIC.Data,0);
								LCDPutBmp(128,6,VENDINGMANAGEPIC.Width,VENDINGMANAGEPIC.Hight,VENDINGMANAGEPIC.Data,0);
								LCDPutBmp(184,6,VENDINGPIC.Width,VENDINGPIC.Hight,VENDINGPIC.Data,1);
								LCDClrArea(1,12,238,14);
								LCDPrintf((231-8*strlen(MaintainMenuList.ReturnVendCapion[SystemPara.Language])),12,0,SystemPara.Language,MaintainMenuList.ReturnVendCapion[SystemPara.Language]);
								break;
					default:	break;
				}
				OSTimeDly(5);	
			}
			if((MaintainMainMenu.KeyCurrentState != 0x00)&&(MaintainMainMenu.KeyEnterState == 0x01))
			{
				MaintainMainMenu.KeyEnterState = 0x00;
				KeyFunctonPtr = MaintainMainMenu.CurrentOperate;
				(*KeyFunctonPtr)();//由此进入各个子菜单
				break;	
			}			
		}
	}
}
/*********************************************************************************************************
** Function name:     	EnterSelfCheckFuction
** Descriptions:	    进入设备自检菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void EnterSelfCheckFuction(void)
{
	unsigned char Key;
	void (*TestFunctonPtr)(void);
	LCDClrScreen();
	Trace("EnterSelfCheckFuction\r\n");
	
	SelfCheckMenu.PageNumb	  = 0x00;
	while(1)//进入自检菜单
	{
		SelfCheckMenu.KeyCurrent  = 0x00;
		SelfCheckMenu.ExtSelfCheck= 0x00;
		SelfCheckMenu.TestOperate = (void *)0;
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		if(SelfCheckMenu.PageNumb == 0x00)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,SelfCheckMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,SelfCheckMenuList.BillValidatorTest[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,SelfCheckMenuList.CoinAcceptorTest[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,SelfCheckMenuList.CoinChangerTest[SystemPara.Language]);
			LCDPrintf(5,11,0,SystemPara.Language,SelfCheckMenuList.GoodsChannelTest[SystemPara.Language]);
			LCDPrintf(5,13,0,SystemPara.Language,SelfCheckMenuList.CashlessDeviceTest[SystemPara.Language]);
			LCDPrintf(231-8*strlen(SelfCheckMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,SelfCheckMenuList.PageDown[SystemPara.Language]);//翻页提示
		}
		else
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,SystemPara.Language,SelfCheckMenuList.ModeCapion[SystemPara.Language]);
			LCDPrintf(5,5,0,SystemPara.Language,SelfCheckMenuList.KeyBoardTest[SystemPara.Language]);
			LCDPrintf(5,7,0,SystemPara.Language,SelfCheckMenuList.SelectKeyTest[SystemPara.Language]);
			LCDPrintf(5,9,0,SystemPara.Language,SelfCheckMenuList.AtcTest[SystemPara.Language]);
			LCDPrintf(5,11,0,SystemPara.Language,SelfCheckMenuList.TuibiTest[SystemPara.Language]);
			LCDPrintf(231-8*strlen(SelfCheckMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,SelfCheckMenuList.PageUp[SystemPara.Language]);
		}
		while(1)
		{
			while(1)
			{
				Key = ReadKeyValue();
				if((Key != 0x00)&&(Key != '0')&&(Key != '.')&&(Key != 'D')&&(Key != 'E'))
					break;
				OSTimeDly(20);
			}
			Trace("Key Select = %C\r\n",Key);
			switch(Key)
			{
				case 'C' :	SelfCheckMenu.ExtSelfCheck = 0x01;
							break;
				case '<' :	if(SelfCheckMenu.PageNumb == 0x01)
							{
								SelfCheckMenu.PageNumb   = 0x00;
								SelfCheckMenu.KeyCurrent = 0x00;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,SelfCheckMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,SelfCheckMenuList.BillValidatorTest[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,SelfCheckMenuList.CoinAcceptorTest[SystemPara.Language]);
								LCDPrintf(5,9,0,SystemPara.Language,SelfCheckMenuList.CoinChangerTest[SystemPara.Language]);
								LCDPrintf(5,11,0,SystemPara.Language,SelfCheckMenuList.GoodsChannelTest[SystemPara.Language]);
								LCDPrintf(5,13,0,SystemPara.Language,SelfCheckMenuList.CashlessDeviceTest[SystemPara.Language]);
								LCDPrintf(231-8*strlen(SelfCheckMenuList.PageDown[SystemPara.Language]),13,0,SystemPara.Language,SelfCheckMenuList.PageDown[SystemPara.Language]);		
							}
							break;
				case '>' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.PageNumb   = 0x01;
								SelfCheckMenu.KeyCurrent = 0x00;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								LCDClrArea(1,4,238,14);
								LCDPrintf(8,1,0,SystemPara.Language,SelfCheckMenuList.ModeCapion[SystemPara.Language]);
								LCDPrintf(5,5,0,SystemPara.Language,SelfCheckMenuList.KeyBoardTest[SystemPara.Language]);
								LCDPrintf(5,7,0,SystemPara.Language,SelfCheckMenuList.SelectKeyTest[SystemPara.Language]);
								LCDPrintf(5,9,0,SystemPara.Language,SelfCheckMenuList.AtcTest[SystemPara.Language]);
								LCDPrintf(5,11,0,SystemPara.Language,SelfCheckMenuList.TuibiTest[SystemPara.Language]);
								LCDPrintf(231-8*strlen(SelfCheckMenuList.PageUp[SystemPara.Language]),13,0,SystemPara.Language,SelfCheckMenuList.PageUp[SystemPara.Language]);		
							}
							break;
				case '1' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.KeyCurrent = 0x01;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestBillValidator;
							}
							break;
				case '2' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.KeyCurrent = 0x02;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestCoinAcceptor;
							}
							break;
				case '3' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.KeyCurrent = 0x03;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestChanger;
							}
							break;
				case '4' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.KeyCurrent = 0x04;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = ChannelTest;
							}
							break;
				case '5' :	if(SelfCheckMenu.PageNumb == 0x00)
							{
								SelfCheckMenu.KeyCurrent = 0x05;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestCashless;
							}
							break;
				case '6' :	if(SelfCheckMenu.PageNumb == 0x01)
							{
								SelfCheckMenu.KeyCurrent = 0x06;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestKeyBoard;
							}
							break;
				case '7' :	if(SelfCheckMenu.PageNumb == 0x01)
							{
								SelfCheckMenu.KeyCurrent = 0x07;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestSelectKey;
							}
							break;
				case '8' :	if(SelfCheckMenu.PageNumb == 0x01)
							{
								SelfCheckMenu.KeyCurrent = 0x08;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestATC;
							}
							break;
				case '9' :	if(SelfCheckMenu.PageNumb == 0x01)
							{
								SelfCheckMenu.KeyCurrent = 0x09;
								SelfCheckMenu.ExtSelfCheck = 0x00;
								SelfCheckMenu.TestOperate = TestTuibi;
							}
							break;
				default  :	break;	
			}
			if(SelfCheckMenu.ExtSelfCheck == 0x01)
			{
				Trace("Switch exit ExtSelfCheck\r\n");
				break;
			}
			if(SelfCheckMenu.KeyCurrent != 0x00)
			{
				TestFunctonPtr = SelfCheckMenu.TestOperate;
				(*TestFunctonPtr)();//由此进入各个设备的自检函数
				break;	
			}
		}
		if(SelfCheckMenu.ExtSelfCheck == 0x01)
		{
			Trace("while Exit selfcheck Menu...\r\n");
			break;
		} 
	}//退出自检菜单并返回维护主菜单
	return;		
}

/*****************************************************************************
** Function name:	DefaultSystemParaMenu	
**
** Descriptions:	配置工程参数	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
void RstSystemPara(uint8_t pageNum,uint8_t flag,uint32_t keyValue)
{
	uint8_t i;
	
	switch(flag)
	{
		case 1:
			//Trace("\r\n show=%ld",keyValue);	
			switch(pageNum)
			{
				case 0: 
					SystemPara.Language = keyValue;											
					break;
				case 1: 	
					SystemPara.UserSelectKeyBoard = keyValue;	
					if(SystemPara.UserSelectKeyBoard == 1)
					{
						SystemPara.CunhuoKou = 1;
					}
					break;
				case 2: 
					SystemPara.CoinValue[0] = keyValue;								
					break;
				case 3:
					SystemPara.HopperValue[0] = keyValue;											
					break;	
				case 4:
					SystemPara.BillValue[0] = keyValue;								
					break;	
				case 5:
					SystemPara.CunhuoKou = keyValue;								
					break;
				case 7:
					SystemPara.XMTTemp = keyValue;								
					break;	
			}
			break;
		case 2:
			switch(pageNum)
			{
				case 0: 
					SystemPara.CoinAcceptorType = keyValue;											
					break;
				case 1: 	
					SystemPara.ChannelType = keyValue;											
					break;
				case 2: 
					SystemPara.CoinValue[1] = keyValue;								
					break;
				case 3:
					SystemPara.HopperValue[1] = keyValue;										
					break;	
				case 4:
					SystemPara.BillValue[1] = keyValue;								
					break;	
				case 5:	
					SystemPara.EasiveEnable = keyValue;	
					break;	
				case 7:
					SystemPara.DecimalNumExt = keyValue;								
					break;	
				
			}	
			break;	
		case 3:
			switch(pageNum)
			{
				case 0: 
					SystemPara.CoinChangerType = keyValue;										
					break;
				case 1: 	
					SystemPara.SubBinOpen = keyValue;										
					break;
				case 2: 
					SystemPara.CoinValue[2] = keyValue;									
					break;
				case 3:
					SystemPara.HopperValue[2] = keyValue;				
					break;	
				case 4:
					SystemPara.BillValue[2] = keyValue;								
					break;
				case 5:	
					SystemPara.MaxValue = keyValue;	
					break;	
				case 7:
					SystemPara.GeziDeviceType = keyValue;								
					break;		
				
			}	
			break;	
		case 4:
			switch(pageNum)
			{
				case 0: 
					SystemPara.BillValidatorType = keyValue;										
					break;
				case 1: 	
					SystemPara.SubBinChannelType = keyValue;				
					break;
				case 2: 
					SystemPara.CoinValue[3] = keyValue;								
					break;
				case 3:	
					SystemPara.HpEmpCoin = keyValue;	
					break;	
				case 4:
					SystemPara.BillValue[3] = keyValue;									
					break;	
				case 5:		
					SystemPara.Channel = keyValue;
					break;	
				case 7:	
					SystemPara.BillEnableValue = keyValue;	
					break;		
				
			}	
			break;	
		case 5:
			switch(pageNum)
			{
				case 0: 
					SystemPara.BillRecyclerType = keyValue;									
					break;
				case 1: 	
					SystemPara.GOCIsOpen = keyValue;	
					break;
				case 2: 
					SystemPara.CoinValue[4] = keyValue;									
					break;
				case 3:
					break;	
				case 4:
					SystemPara.BillValue[4] = keyValue;								
					break;
				case 5:			
					SystemPara.SubChannel = keyValue;
					break;
				case 7:			
					SystemPara.ColumnTime = keyValue;
					break;		
				
			}	
			break;	
		case 6:
			switch(pageNum)
			{
				case 0: 
					SystemPara.CashlessDeviceType = keyValue;									
					break;
				case 1: 
					SystemPara.BillITLValidator = keyValue;	
					break;
				case 2: 
					SystemPara.CoinValue[5] = keyValue;								
					break;
				case 3:
					break;	
				case 4:
					SystemPara.BillValue[5] = keyValue;								
					break;
				case 5:	
					SystemPara.threeSelectKey = keyValue;		
					break;		
				
			}	
			break;	
		case 7:
			switch(pageNum)
			{
				case 0: 
					SystemPara.DecimalNum = keyValue;									
					break;
				case 1: 
					SystemPara.SaleTime = keyValue;	
					break;
				case 2: 
					SystemPara.CoinValue[6] = keyValue;									
					break;
				case 3:
					keyValue = 0;
					flag = 0;
					break;	
				case 4:
					SystemPara.BillValue[6] = keyValue;							
					break;	
				case 5:		
					SystemPara.hefangGui = keyValue;	
					break;		
				
			}	
			break;	
		case 8:
			switch(pageNum)
			{
				case 0: 
					SystemPara.Logo = keyValue;				
					break;
				case 1: 
					SystemPara.PcEnable = keyValue;	
					break;
				case 2: 
					SystemPara.CoinValue[7] = keyValue;
					break;
				case 3:
					break;	
				case 4:
					SystemPara.BillValue[7] = keyValue;
					break;	
				case 5:		
					SystemPara.hefangGuiKou = keyValue;	
					break;
				case 6:		
					SystemPara.RecyclerMoney = 0;
					//只有原先存在的循环纸币面值才能设置
					for(i=0;i<7;i++)
					{
						if(keyValue==SystemPara.RecyclerValue[i])
							SystemPara.RecyclerMoney = keyValue;	
					}
					break;		
				
			}	
			break;						
	}
}

/*****************************************************************************
** Function name:	DefaultSystemParaMenu	
**
** Descriptions:	配置工程参数	
**					
**														 			
** parameters:		
					
** Returned value:	0——完成
** 
*******************************************************************************/
void DefaultSystemParaMenu(void)
{	
	uint8_t pageNum = 0,flag = 0, resetPar = 0;
	unsigned char Key;
	uint32_t keyValue = 0;

	LCDClrScreen();
	while(1)
	{
		if(flag == 0)
		{
			LCDClrScreen();
			//1.显示页面
			switch(pageNum)
			{
				case 0: 
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d",SysMenuList.Language[SystemPara.Language],SystemPara.Language);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d",SysMenuList.CoinAcceptorType[SystemPara.Language],SystemPara.CoinAcceptorType);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d",SysMenuList.CoinChangerType[SystemPara.Language],SystemPara.CoinChangerType);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d",SysMenuList.BillValidatorType[SystemPara.Language],SystemPara.BillValidatorType);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d",SysMenuList.BillRecyclerType[SystemPara.Language],SystemPara.BillRecyclerType);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d",SysMenuList.CashlessDeviceType[SystemPara.Language],SystemPara.CashlessDeviceType);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d",SysMenuList.DecimalNum[SystemPara.Language],SystemPara.DecimalNum);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d",SysMenuList.Logo[SystemPara.Language],SystemPara.Logo);					
					break;
				case 1: 	
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d",SysMenuList.UserSelectKeyBoard[SystemPara.Language],SystemPara.UserSelectKeyBoard);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d",SysMenuList.ChannelType[SystemPara.Language],SystemPara.ChannelType);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d",SysMenuList.SubBinOpen[SystemPara.Language],SystemPara.SubBinOpen);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d",SysMenuList.SubBinChannelType[SystemPara.Language],SystemPara.SubBinChannelType);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d",SysMenuList.GOCIsOpen[SystemPara.Language],SystemPara.GOCIsOpen);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d",SysMenuList.BillITLValidator[SystemPara.Language],SystemPara.BillITLValidator);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d",SysMenuList.SaleTime[SystemPara.Language],SystemPara.SaleTime);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d",SysMenuList.PcEnable[SystemPara.Language],SystemPara.PcEnable);
					break;
				case 2: 
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin1[SystemPara.Language],SystemPara.CoinValue[0]/100,SystemPara.CoinValue[0]%100);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin2[SystemPara.Language],SystemPara.CoinValue[1]/100,SystemPara.CoinValue[1]%100);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin3[SystemPara.Language],SystemPara.CoinValue[2]/100,SystemPara.CoinValue[2]%100);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin4[SystemPara.Language],SystemPara.CoinValue[3]/100,SystemPara.CoinValue[3]%100);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin5[SystemPara.Language],SystemPara.CoinValue[4]/100,SystemPara.CoinValue[4]%100);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin6[SystemPara.Language],SystemPara.CoinValue[5]/100,SystemPara.CoinValue[5]%100);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin7[SystemPara.Language],SystemPara.CoinValue[6]/100,SystemPara.CoinValue[6]%100);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d.%02d",SysMenuList.coin8[SystemPara.Language],SystemPara.CoinValue[7]/100,SystemPara.CoinValue[7]%100);
					break;
				case 3:
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d.%02d",SysMenuList.hopper1[SystemPara.Language],SystemPara.HopperValue[0]/100,SystemPara.HopperValue[0]%100);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d.%02d",SysMenuList.hopper2[SystemPara.Language],SystemPara.HopperValue[1]/100,SystemPara.HopperValue[1]%100);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d.%02d",SysMenuList.hopper3[SystemPara.Language],SystemPara.HopperValue[2]/100,SystemPara.HopperValue[2]%100);				
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d",SysMenuList.HpEmpCoin[SystemPara.Language],SystemPara.HpEmpCoin);
					break;	
				case 4:
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill1[SystemPara.Language],SystemPara.BillValue[0]/100,SystemPara.BillValue[0]%100);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill2[SystemPara.Language],SystemPara.BillValue[1]/100,SystemPara.BillValue[1]%100);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill3[SystemPara.Language],SystemPara.BillValue[2]/100,SystemPara.BillValue[2]%100);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill4[SystemPara.Language],SystemPara.BillValue[3]/100,SystemPara.BillValue[3]%100);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill5[SystemPara.Language],SystemPara.BillValue[4]/100,SystemPara.BillValue[4]%100);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill6[SystemPara.Language],SystemPara.BillValue[5]/100,SystemPara.BillValue[5]%100);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill7[SystemPara.Language],SystemPara.BillValue[6]/100,SystemPara.BillValue[6]%100);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d.%02d",SysMenuList.bill8[SystemPara.Language],SystemPara.BillValue[7]/100,SystemPara.BillValue[7]%100);
					break;	
				case 5: 
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d",SysMenuList.CunhuoKou[SystemPara.Language],SystemPara.CunhuoKou);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d",SysMenuList.EasiveEnable[SystemPara.Language],SystemPara.EasiveEnable);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d.%02d",SysMenuList.MaxValue[SystemPara.Language],SystemPara.MaxValue/100,SystemPara.MaxValue%100);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d",SysMenuList.Channel[SystemPara.Language],SystemPara.Channel);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d",SysMenuList.SubChannel[SystemPara.Language],SystemPara.SubChannel);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d",SysMenuList.threeSelectKey[SystemPara.Language],SystemPara.threeSelectKey);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d",SysMenuList.hefangGui[SystemPara.Language],SystemPara.hefangGui);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d",SysMenuList.hefangGuiKou[SystemPara.Language],SystemPara.hefangGuiKou);
					break;
				case 6:
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue1[SystemPara.Language],SystemPara.RecyclerValue[0]/100,SystemPara.RecyclerValue[0]%100);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue2[SystemPara.Language],SystemPara.RecyclerValue[1]/100,SystemPara.RecyclerValue[1]%100);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue3[SystemPara.Language],SystemPara.RecyclerValue[2]/100,SystemPara.RecyclerValue[2]%100);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue4[SystemPara.Language],SystemPara.RecyclerValue[3]/100,SystemPara.RecyclerValue[3]%100);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue5[SystemPara.Language],SystemPara.RecyclerValue[4]/100,SystemPara.RecyclerValue[4]%100);
					LCDPrintf(0,10,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue6[SystemPara.Language],SystemPara.RecyclerValue[5]/100,SystemPara.RecyclerValue[5]%100);
					LCDPrintf(0,12,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerValue7[SystemPara.Language],SystemPara.RecyclerValue[6]/100,SystemPara.RecyclerValue[6]%100);
					LCDPrintf(0,14,0,SystemPara.Language,"%s%d.%02d",SysMenuList.RecyclerMoney[SystemPara.Language],SystemPara.RecyclerMoney/100,SystemPara.RecyclerMoney%100);
					break;
				case 7:	
					LCDPrintf(0,0,0,SystemPara.Language,"%s%d",SysMenuList.XMTTemp[SystemPara.Language],SystemPara.XMTTemp);
					LCDPrintf(0,2,0,SystemPara.Language,"%s%d",SysMenuList.DecimalNumExt[SystemPara.Language],SystemPara.DecimalNumExt);
					LCDPrintf(0,4,0,SystemPara.Language,"%s%d",SysMenuList.GeziDeviceType[SystemPara.Language],SystemPara.GeziDeviceType);
					LCDPrintf(0,6,0,SystemPara.Language,"%s%d.%02d",SysMenuList.BillEnableValue[SystemPara.Language],SystemPara.BillEnableValue/100,SystemPara.BillEnableValue%100);
					LCDPrintf(0,8,0,SystemPara.Language,"%s%d",SysMenuList.ColumnTime[SystemPara.Language],SystemPara.ColumnTime);
					break;
			}
		}
		//2.捕捉按键
		while(1)
		{			
			Key = ReadKeyValue();			
			if(Key != 0x00)
				break;
		}
		switch(Key)
		{
			case 'C':
				if(flag >0)
					break;
				else
				{
					if(resetPar == 1)
					{
						//Trace("\r\n 1==%d,%d,%d,%d",SystemPara.Language,SystemPara.CoinAcceptorType,SystemPara.CoinChangerType,SystemPara.BillValidatorType);
						WriteDefaultSystemPara(SystemPara,0);
						//Trace("\r\n ==%d,%d,%d,%d",SystemPara.Language,SystemPara.CoinAcceptorType,SystemPara.CoinChangerType,SystemPara.BillValidatorType);
					}
					return;
				}
			case '>':							
				if(pageNum < 7)
					pageNum++;
				break;				
			case '<':							
				if(pageNum > 0)
					pageNum--;
				break;
			case '1':
				if(flag == 0)
					flag = 1;
				break;
			case '2':
				if(flag == 0)
					flag = 2;
				break;
			case '3':
				if(flag == 0)
					flag = 3;
				break;
			case '4':
				if(flag == 0)
					flag = 4;
				break;
			case '5':
				if(flag == 0)
					flag = 5;
				break;
			case '6':
				if(flag == 0)
					flag = 6;
				break;
			case '7':
				if(flag == 0)
					flag = 7;
				break;
			case '8':
				if(flag == 0)
					flag = 8;
				break;
			default:
				break;					
		}			
		while(flag)
		{			
			//3.反白显示菜单,以及显示稍候的输入数字
			switch(flag)
			{
				case 1:
					//Trace("\r\n show=%ld",keyValue);	
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,0,1,SystemPara.Language,"%s%d",SysMenuList.Language[SystemPara.Language],keyValue);											
							break;
						case 1: 	
							LCDPrintf(0,0,1,SystemPara.Language,"%s%d",SysMenuList.UserSelectKeyBoard[SystemPara.Language],keyValue);												
							break;
						case 2: 
							LCDPrintf(0,0,1,SystemPara.Language,"%s%ld",SysMenuList.coin1[SystemPara.Language],keyValue);								
							break;
						case 3:
							LCDPrintf(0,0,1,SystemPara.Language,"%s%ld",SysMenuList.hopper1[SystemPara.Language],keyValue);											
							break;	
						case 4:
							LCDPrintf(0,0,1,SystemPara.Language,"%s%ld",SysMenuList.bill1[SystemPara.Language],keyValue);								
							break;
						case 5: 
							LCDPrintf(0,0,1,SystemPara.Language,"%s%d",SysMenuList.CunhuoKou[SystemPara.Language],keyValue);	
							break;	
						case 7:	
							LCDPrintf(0,0,1,SystemPara.Language,"%s%d",SysMenuList.XMTTemp[SystemPara.Language],keyValue);	
							break;
					}
					break;
				case 2:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,2,1,SystemPara.Language,"%s%d",SysMenuList.CoinAcceptorType[SystemPara.Language],keyValue);											
							break;
						case 1: 	
							LCDPrintf(0,2,1,SystemPara.Language,"%s%d",SysMenuList.ChannelType[SystemPara.Language],keyValue);											
							break;
						case 2: 
							LCDPrintf(0,2,1,SystemPara.Language,"%s%ld",SysMenuList.coin2[SystemPara.Language],keyValue);								
							break;
						case 3:
							LCDPrintf(0,2,1,SystemPara.Language,"%s%ld",SysMenuList.hopper2[SystemPara.Language],keyValue);										
							break;	
						case 4:
							LCDPrintf(0,2,1,SystemPara.Language,"%s%ld",SysMenuList.bill2[SystemPara.Language],keyValue);								
							break;
						case 5:	
							LCDPrintf(0,2,1,SystemPara.Language,"%s%d",SysMenuList.EasiveEnable[SystemPara.Language],keyValue);
							break;	
						case 7:	
							LCDPrintf(0,2,1,SystemPara.Language,"%s%d",SysMenuList.DecimalNumExt[SystemPara.Language],keyValue);
							break;	
						
					}	
					break;	
				case 3:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,4,1,SystemPara.Language,"%s%d",SysMenuList.CoinChangerType[SystemPara.Language],keyValue);										
							break;
						case 1: 	
							LCDPrintf(0,4,1,SystemPara.Language,"%s%d",SysMenuList.SubBinOpen[SystemPara.Language],keyValue);										
							break;
						case 2: 
							LCDPrintf(0,4,1,SystemPara.Language,"%s%ld",SysMenuList.coin3[SystemPara.Language],keyValue);								
							break;
						case 3:
							LCDPrintf(0,4,1,SystemPara.Language,"%s%ld",SysMenuList.hopper3[SystemPara.Language],keyValue);				
							break;	
						case 4:
							LCDPrintf(0,4,1,SystemPara.Language,"%s%ld",SysMenuList.bill3[SystemPara.Language],keyValue);								
							break;	
						case 5:	
							LCDPrintf(0,4,1,SystemPara.Language,"%s%ld",SysMenuList.MaxValue[SystemPara.Language],keyValue);
							break;
						case 7:
							LCDPrintf(0,4,1,SystemPara.Language,"%s%d",SysMenuList.GeziDeviceType[SystemPara.Language],keyValue);
							break;
						
					}	
					break;	
				case 4:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,6,1,SystemPara.Language,"%s%d",SysMenuList.BillValidatorType[SystemPara.Language],keyValue);										
							break;
						case 1: 	
							LCDPrintf(0,6,1,SystemPara.Language,"%s%d",SysMenuList.SubBinChannelType[SystemPara.Language],keyValue);				
							break;
						case 2: 
							LCDPrintf(0,6,1,SystemPara.Language,"%s%ld",SysMenuList.coin4[SystemPara.Language],keyValue);								
							break;
						case 3:	
							LCDPrintf(0,6,1,SystemPara.Language,"%s%d",SysMenuList.HpEmpCoin[SystemPara.Language],keyValue);
							break;	
						case 4:
							LCDPrintf(0,6,1,SystemPara.Language,"%s%ld",SysMenuList.bill4[SystemPara.Language],keyValue);								
							break;	
						case 5:	
							LCDPrintf(0,6,1,SystemPara.Language,"%s%d",SysMenuList.Channel[SystemPara.Language],keyValue);
							break;	
						case 7:
							LCDPrintf(0,6,1,SystemPara.Language,"%s%ld",SysMenuList.BillEnableValue[SystemPara.Language],keyValue);
							break;
						
					}	
					break;	
				case 5:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,8,1,SystemPara.Language,"%s%d",SysMenuList.BillRecyclerType[SystemPara.Language],keyValue);									
							break;
						case 1: 
							LCDPrintf(0,8,1,SystemPara.Language,"%s%d",SysMenuList.GOCIsOpen[SystemPara.Language],keyValue);
							break;
						case 2: 
							LCDPrintf(0,8,1,SystemPara.Language,"%s%ld",SysMenuList.coin5[SystemPara.Language],keyValue);								
							break;
						case 3:
							keyValue = 0;
							flag = 0;
							break;	
						case 4:
							LCDPrintf(0,8,1,SystemPara.Language,"%s%ld",SysMenuList.bill5[SystemPara.Language],keyValue);								
							break;
						case 5:	
							LCDPrintf(0,8,1,SystemPara.Language,"%s%d",SysMenuList.SubChannel[SystemPara.Language],keyValue);
							break;	
						case 7:	
							LCDPrintf(0,8,1,SystemPara.Language,"%s%d",SysMenuList.ColumnTime[SystemPara.Language],keyValue);
							break;		
						
					}	
					break;	
				case 6:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,10,1,SystemPara.Language,"%s%d",SysMenuList.CashlessDeviceType[SystemPara.Language],keyValue);									
							break;
						case 1: 
							LCDPrintf(0,10,1,SystemPara.Language,"%s%d",SysMenuList.BillITLValidator[SystemPara.Language],keyValue);
							break;
						case 2: 
							LCDPrintf(0,10,1,SystemPara.Language,"%s%ld",SysMenuList.coin6[SystemPara.Language],keyValue);								
							break;
						case 3:
							keyValue = 0;
							flag = 0;
							break;	
						case 4:
							LCDPrintf(0,10,1,SystemPara.Language,"%s%ld",SysMenuList.bill6[SystemPara.Language],keyValue);								
							break;
						case 5:	
							LCDPrintf(0,10,1,SystemPara.Language,"%s%d",SysMenuList.threeSelectKey[SystemPara.Language],keyValue);
							break;	
						
					}	
					break;	
				case 7:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,12,1,SystemPara.Language,"%s%d",SysMenuList.DecimalNum[SystemPara.Language],keyValue);									
							break;
						case 1: 
							LCDPrintf(0,12,1,SystemPara.Language,"%s%d",SysMenuList.SaleTime[SystemPara.Language],keyValue);
							break;
						case 2: 
							LCDPrintf(0,12,1,SystemPara.Language,"%s%ld",SysMenuList.coin7[SystemPara.Language],keyValue);								
							break;
						case 3:
							keyValue = 0;
							flag = 0;
							break;	
						case 4:
							LCDPrintf(0,12,1,SystemPara.Language,"%s%ld",SysMenuList.bill7[SystemPara.Language],keyValue);								
							break;	
						case 5:	
							LCDPrintf(0,12,1,SystemPara.Language,"%s%d",SysMenuList.hefangGui[SystemPara.Language],keyValue);
							break;	
						
					}	
					break;	
				case 8:
					switch(pageNum)
					{
						case 0: 
							LCDPrintf(0,14,1,SystemPara.Language,"%s%d",SysMenuList.Logo[SystemPara.Language],keyValue);				
							break;
						case 1: 
							LCDPrintf(0,14,1,SystemPara.Language,"%s%d",SysMenuList.PcEnable[SystemPara.Language],keyValue);
							break;
						case 2: 
							LCDPrintf(0,14,1,SystemPara.Language,"%s%ld",SysMenuList.coin8[SystemPara.Language],keyValue);
							break;
						case 3:
							keyValue = 0;
							flag = 0;
							break;	
						case 4:
							LCDPrintf(0,14,1,SystemPara.Language,"%s%ld",SysMenuList.bill8[SystemPara.Language],keyValue);
							break;	
						case 5:	
							LCDPrintf(0,14,1,SystemPara.Language,"%s%d",SysMenuList.hefangGuiKou[SystemPara.Language],keyValue);
							break;
						case 6:	
							LCDPrintf(0,14,1,SystemPara.Language,"%s%ld",SysMenuList.RecyclerMoney[SystemPara.Language],keyValue);
							break;		
						
					}	
					break;						
			}
			//输入数字
			while(1)
			{			
				Key = GetKeyInValue();			
				if(Key != 0xff)
				{
					//4.显示输入的数字
					if(Key <= 0x09)
						keyValue = keyValue*10+Key;	
					else if(Key == 0x0e)//取消按键
					{
						keyValue = 0;
						flag = 0;						
					}
					//4.将输入数字保存到参数中
					else if(Key == 0x0f)//确定按键
					{
						RstSystemPara(pageNum,flag,keyValue);
						resetPar = 1;
						keyValue = 0;
						flag = 0;
					}
					//Trace("\r\n input=%ld,%d",keyValue,Key);
					break;
				}
			}
			
		}
		OSTimeDly(10);			
	}  
}



/*********************************************************************************************************
** Function name:     	EnterDeviceManageFuction
** Descriptions:	    进入设备管理菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
/*void EnterDeviceManageFuction(void)
{
	LCDClrScreen();
	Trace("EnterDeviceManageFuction\r\n");
	while(1)
	{
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDPrintf(0,0,0,SystemPara.Language,"EnterDeviceManageFuction");
		while(1)
		{
			if(ReadKeyValue() == 'C')
				break;
			OSTimeDly(50);
		}
		break;
	}
	return;
}*/
/*********************************************************************************************************
** Function name:     	EnterVendingSetFuction
** Descriptions:	    进入售货应用程序设置菜单
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void EnterVendingSetFuction(void)
{
	LCDClrScreen();
	Trace("EnterVendingSetFuction\r\n");
	while(1)
	{
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		while(1)
		{
			if(ReadKeyValue() == 'C')
				break;
			OSTimeDly(50);
		}
		break;
	}
	return;
}
/*********************************************************************************************************
** Function name:     	FactorySetting
** Descriptions:	    恢复出厂配置
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void FactorySetting(void)
{
	Trace("ReturnToVendingMode\r\n");	
	LCDClrArea(1,4,238,14);
	if(SelectOKCheck())
	{
		Trace("Factory1\r\n");
		FactoryDefaultSystemPara();
		//WriteDefaultSystemPara();
	}	
	return;
}

/*********************************************************************************************************
** Function name:     	FactorySetting
** Descriptions:	    设置出厂配置
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void RstFactorySetting(void)
{
	Trace("ReturnToVendingMode\r\n");	
	LCDClrArea(1,4,238,14);
	if(SelectOKCheck())
	{
		Trace("RstFactory1\r\n");
		RstFactoryDefaultSystemPara();
	}	
	return;
}


uint32_t GetTubeMoney()
{
	uint32_t coinMoney;		
	coinMoney = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
				+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5]
				+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
				+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
				+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
				+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; ; 
		
	return coinMoney;
}


/*********************************************************************************************************
** Function name:       TestBillValidator
** Descriptions:        按张数找币,0-9出币数量,面值为存入循环斗的面值
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TestBillValidator(void)
{	
	uint32_t InValue;
	uint32_t SumValue = 0;
	char    *pstr;
	char	strMoney[10];
	uint8_t KeyValue=0,i;
	
	LCDClrScreen();
	LCDPrintf(0,LINE1,0,0,"TestBillValidator=MDB");
	LCDPrintf(0,LINE3,0,0,"Level=%d,Scale=%ld",stDevValue.BillLevel,stDevValue.BillScale);
	LCDPrintf(0,LINE5,0,0,"Capacity=%ld,Escrow=%d",stDevValue.BillStkCapacity,stDevValue.BillEscrowFun);
	LCDPrintf(0,LINE7,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.BillValue[0],stDevValue.BillValue[1],stDevValue.BillValue[2],stDevValue.BillValue[3]);
	LCDPrintf(0,LINE9,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.BillValue[4],stDevValue.BillValue[5],stDevValue.BillValue[6],stDevValue.BillValue[7]);
	if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
	{
		LCDPrintf(0,LINE11,0,0,"recva =%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.RecyclerValue[0],stDevValue.RecyclerValue[1],stDevValue.RecyclerValue[2],stDevValue.RecyclerValue[3],stDevValue.RecyclerValue[4],stDevValue.RecyclerValue[5],stDevValue.RecyclerValue[6]);
		LCDPrintf(0,LINE13,0,0,"recNum=%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.RecyclerNum[0],stDevValue.RecyclerNum[1],stDevValue.RecyclerNum[2],stDevValue.RecyclerNum[3],stDevValue.RecyclerNum[4],stDevValue.RecyclerNum[5],stDevValue.RecyclerNum[6]);
	}
	Trace("TestBillValidator\r\n");
	BillDevEnableAPI();
	while(1)
	{
		InValue=GetBillDevMoneyInAPI();
		if(InValue>0)
		{
			//Trace("2.bill=%ld\r\n",InValue);			
			SumValue += InValue;
			pstr = PrintfMoney(SumValue);
			strcpy(strMoney, pstr);
			LCDPrintf(0,LINE15,0,SystemPara.Language," %s%s",SelfCheckText.Balance[SystemPara.Language],strMoney);
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				LCDPrintf(0,LINE13,0,0,"recNum=%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.RecyclerNum[0],stDevValue.RecyclerNum[1],stDevValue.RecyclerNum[2],stDevValue.RecyclerNum[3],stDevValue.RecyclerNum[4],stDevValue.RecyclerNum[5],stDevValue.RecyclerNum[6]);
			}
		}

		KeyValue = GetKeyInValue();	
		if(KeyValue != 0xff)
		{
			//4.显示输入的数字
			if( (KeyValue >= 0x01)&&(KeyValue <= 0x09) )
			{
				if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
				{
					for(i=0;i<7;i++)
					{
						if(stDevValue.RecyclerValue[i])
						{
							pstr = PrintfMoney(stDevValue.RecyclerValue[i]*KeyValue);
							strcpy(strMoney, pstr);
							LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
							BillRecyclerPayoutNumExpanseAPI(stDevValue.RecyclerValue[i],KeyValue);
							//BillRecyclerPayoutValueExpanseAPI(2000);
							LCDPrintf(0,LINE13,0,0,"recNum=%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.RecyclerNum[0],stDevValue.RecyclerNum[1],stDevValue.RecyclerNum[2],stDevValue.RecyclerNum[3],stDevValue.RecyclerNum[4],stDevValue.RecyclerNum[5],stDevValue.RecyclerNum[6]);
							SumValue=0;
							break;
						}
					}
				}
			}
			else if(KeyValue == 0x0e)//取消按键
			{
				break;						
			}
		}
		OSTimeDly(10);		
	}
	BillDevDisableAPI();
}
void TestCoinAcceptor(void)
{	
	uint32_t InValue;
	uint32_t SumValue = 0;
	char    *pstr;
	char	strMoney[10];
	
	LCDClrScreen();
	switch(SystemPara.CoinAcceptorType)
	{
		case PARALLEL_COINACCEPTER:			
			LCDPrintf(0,LINE1,0,0,"TestCoinAcceptor=PARALLEL");
			LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3]);
			LCDPrintf(0,LINE7,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.CoinValue[4],stDevValue.CoinValue[5],stDevValue.CoinValue[6],stDevValue.CoinValue[7]);
			break;
		case MDB_COINACCEPTER:
			LCDPrintf(0,LINE1,0,0,"TestCoinAcceptor=MDB");
			LCDPrintf(0,LINE3,0,0,"Level=%d,Scale=%ld",stDevValue.CoinLevel,stDevValue.CoinScale);
			LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3]);
			LCDPrintf(0,LINE7,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.CoinValue[4],stDevValue.CoinValue[5],stDevValue.CoinValue[6],stDevValue.CoinValue[7]);
			LCDPrintf(0,LINE9,0,0,"Num  =%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
			LCDPrintf(0,LINE11,0,0,"    =%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
			//总金额
			InValue = GetTubeMoney();
			pstr = PrintfMoney(InValue);
			strcpy(strMoney, pstr);
			LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
			break;
		case SERIAL_COINACCEPTER:					
			LCDPrintf(0,LINE1,0,0,"TestCoinAcceptor=SERIAL");
			LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3]);
			LCDPrintf(0,LINE7,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.CoinValue[4],stDevValue.CoinValue[5],stDevValue.CoinValue[6],stDevValue.CoinValue[7]);
			break;
			
	}
			
	Trace("TestCoinAcceptor\r\n");
	CoinDevEnableAPI();
	while(1)
	{
		InValue=GetCoinDevMoneyInAPI();
		if(InValue>0)
		{
			//Trace("2.coin=%ld\r\n",InValue);
			SumValue += InValue;
			pstr = PrintfMoney(SumValue);
			strcpy(strMoney, pstr);
			LCDPrintf(0,LINE15,0,SystemPara.Language," %s%s",SelfCheckText.Balance[SystemPara.Language],strMoney);
			if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
			{
				LCDPrintf(0,LINE9,0,0,"Num  =%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"    =%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
			}
		}
		
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
	CoinDevDisableAPI();
}
void TestChanger(void)
{	
	uint32_t backmoney,scaleMoney,InValue;
	char    *pstr;
	char	strMoney[10];
	
	LCDClrScreen();	
	switch(SystemPara.CoinChangerType)
	{		
		case HOPPER_CHANGER:
			LCDPrintf(0,LINE1,0,0,"TestChanger=HOPPER");
			LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld",SystemPara.HopperValue[0],SystemPara.HopperValue[1],SystemPara.HopperValue[2]);
			scaleMoney = SystemPara.HopperValue[0];
			break;
		case MDB_CHANGER:
			LCDPrintf(0,LINE1,0,0,"TestChanger=MDB");
			LCDPrintf(0,LINE5,0,0,"value=%ld,%ld,%ld,%ld",stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3]);
			LCDPrintf(0,LINE7,0,0,"     =%ld,%ld,%ld,%ld",stDevValue.CoinValue[4],stDevValue.CoinValue[5],stDevValue.CoinValue[6],stDevValue.CoinValue[7]);
			LCDPrintf(0,LINE9,0,0,"Num  =%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
			LCDPrintf(0,LINE11,0,0,"    =%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
			//总金额
			InValue = GetTubeMoney();
			pstr = PrintfMoney(InValue);
			strcpy(strMoney, pstr);
			LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
			//得到最小找零金额
			scaleMoney = stDevValue.CoinValue[0];
			break;
	}
	Trace("TestChanger\r\n");
	//ChangerDevPayoutAPI(250);
	while(1)
	{
		//if(ReadKeyValue() == 'C')
		//	break;
		switch(ReadKeyValue())
		{
			case '1':
				if(SystemPara.CoinChangerType == MDB_CHANGER)
				{
					pstr = PrintfMoney(scaleMoney*1);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
					ChangerDevPayoutAPI(scaleMoney*1,&backmoney);
					LCDPrintf(0,LINE9,0,0,"Num  =%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
					LCDPrintf(0,LINE11,0,0,"    =%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
					//总金额
					InValue = GetTubeMoney();
					pstr = PrintfMoney(InValue);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				}
				else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
				{
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper1",SelfCheckText.Change[SystemPara.Language]);
					TestHopperHandle(1,5);
				}
				break;
			case '2':
				if(SystemPara.CoinChangerType == MDB_CHANGER)
				{
					pstr = PrintfMoney(scaleMoney*2);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
					ChangerDevPayoutAPI(scaleMoney*2,&backmoney);					
					LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
					LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
					//总金额
					InValue = GetTubeMoney();
					pstr = PrintfMoney(InValue);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				}
				else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
				{
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper2",SelfCheckText.Change[SystemPara.Language]);
					TestHopperHandle(2,5);
				}
				break;
			case '3':
				if(SystemPara.CoinChangerType == MDB_CHANGER)
				{
					pstr = PrintfMoney(scaleMoney*3);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
					ChangerDevPayoutAPI(scaleMoney*3,&backmoney);
					LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
					LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
					//总金额
					InValue = GetTubeMoney();
					pstr = PrintfMoney(InValue);
					strcpy(strMoney, pstr);
					LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				}
				else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
				{
					LCDPrintf(0,LINE15,0,SystemPara.Language,"%shopper3",SelfCheckText.Change[SystemPara.Language]);
					TestHopperHandle(3,5);
				}
				break;
			case '4':
				pstr = PrintfMoney(scaleMoney*4);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*4,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;	
			case '5':
				pstr = PrintfMoney(scaleMoney*5);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*5,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;
			case '6':
				pstr = PrintfMoney(scaleMoney*6);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*6,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;
			case '7':
				pstr = PrintfMoney(scaleMoney*7);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*7,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;
			case '8':
				pstr = PrintfMoney(scaleMoney*8);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*8,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;	
			case '9':
				pstr = PrintfMoney(scaleMoney*9);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE15,0,SystemPara.Language,"%s%s",SelfCheckText.Change[SystemPara.Language],strMoney);
				ChangerDevPayoutAPI(scaleMoney*9,&backmoney);
				LCDPrintf(0,LINE9,0,0,"Num	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3]);
				LCDPrintf(0,LINE11,0,0,"	=%ld,%ld,%ld,%ld",stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
				//总金额
				InValue = GetTubeMoney();
				pstr = PrintfMoney(InValue);
				strcpy(strMoney, pstr);
				LCDPrintf(0,LINE13,0,0,"Total Money=%s",strMoney);
				break;	
			case 'C':
				return;
		}
		OSTimeDly(10);		
	}
}
void TestGoodsChannel(void)
{	
	LCDClrScreen();
	LCDPrintf(16,7,0,0,"TestGoodsChannel");
	Trace("TestGoodsChannel\r\n");
	while(1)
	{
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
}
void TestCashless(void)
{	
	uint32_t InValue;
	uint8_t  KeyValue;
	uint8_t readerType = 0,flag;
	
	LCDClrScreen();
	LCDPrintf(0,LINE2,0,SystemPara.Language,"TestCashless");
	Trace("TestCashless\r\n");
	ReaderDevEnableAPI();
	
	while(1)
	{
		InValue = GetReaderDevMoneyInAPI(&readerType);
		if( (readerType == 1)&&(InValue > 0) )
		{
			Trace("\r\nReader=%ld\r\n",InValue);
			//LCDPrintf(0,LINE4,0,SystemPara.Language,"Read Card...");
			//OSTimeDly(OS_TICKS_PER_SEC * 5);
			LCDPrintf(0,LINE4,0,SystemPara.Language,"Reader Balance:%ld",InValue);	
			ClearKey();
		}
		else if(readerType == 2)
		{		
			Trace("\r\n OUTPUT card");
			LCDClearLineDraw(0,LINE4,1);
			LCDClearLineDraw(0,LINE6,1);
			readerType = 0;
		}
		
		KeyValue = ReadKeyValue();
		if(KeyValue > 0)
		{
			Trace("\r\n %c",KeyValue);
			if(KeyValue == 'C')
				break;
			else 
			{	
				if(readerType == 1)
				{	
					switch(KeyValue)
					{
						case '1'://扣款成功,交易成功
							LCDPrintf(0,LINE6,0,SystemPara.Language,"To deduct money..0");					
							flag=ReaderDevVendoutTestAPI(0,0);
							if(flag)
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Completion of the transaction");
							else
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Failure of the transaction");
							break;
						case '2'://扣款失败
							LCDPrintf(0,LINE6,0,SystemPara.Language,"To deduct money..60000");
							flag=ReaderDevVendoutTestAPI(60000,0);
							if(flag)
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Completion of the transaction");
							else
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Not Enough");
							break;	
						case '3'://扣款失败,交易失败
							LCDPrintf(0,LINE6,0,SystemPara.Language,"To deduct money..0");
							flag=ReaderDevVendoutTestAPI(0,1);
							if(flag)
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Completion of the transaction");
							else
								LCDPrintf(0,LINE6,0,SystemPara.Language,"transaction Fail");
							break;	
						case '4'://扣款成功,交易成功
							LCDPrintf(0,LINE6,0,SystemPara.Language,"To deduct money..150");					
							flag=ReaderDevVendoutTestAPI(150,0);
							if(flag)
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Completion of the transaction");
							else
								LCDPrintf(0,LINE6,0,SystemPara.Language,"Failure of the transaction");
							break;	
					}
					
					ClearKey();		
				}
				//OSTimeDly(OS_TICKS_PER_SEC);	
							
			}
		}
		
	}	
	ReaderDevDisableAPI();
}
void TestKeyBoard(void)
{
	LCDClrScreen();
	LCDPrintf(16,7,0,0,"TestKeyBoard");
	Trace("TestKeyBoard\r\n");
	while(1)
	{
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
}
void TestSelectKey(void)
{	
	uint8_t key;
	LCDClrScreen();
	LCDPrintf(16,7,0,0,"TestSelectKey");
	Trace("TestLCD\r\n");
	FreeSelectionKeyAPI(1);
	while(1)
	{
		key = GetSelectKeyAPI();
		if(key)
		{
			TraceSelection("\r\n AppKey=%d",key);
			LCDPrintf(16,11,0,0,"TestKey=%d",key);
		}
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
	FreeSelectionKeyAPI(0);
}

void TestATC(void)
{	
	LCDClrScreen();
	LCDPrintf(16,7,0,0,"TestATC");
	Trace("TestATC\r\n");
	while(1)
	{
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
}

void TestTuibi(void)
{
	LCDClrScreen();
	LCDPrintf(16,7,0,0,"TestTuibi按下退币按键");
	Trace("TestCOM\r\n");
	while(1)
	{
		IsTuibiAPI();
		if(ReadKeyValue() == 'C')
			break;
		OSTimeDly(10);		
	}
}



/**************************************End Of File*******************************************************/
