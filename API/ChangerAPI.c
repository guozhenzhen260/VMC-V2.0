/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CHANGERACCEPTER
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        找零器中间程序相关函数声明                     
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

//Hopper找零单位面值大小设置标志：1：三个Hopper的面值不相等；2：000地址Hopper与001地址Hopper地址面值相等，与010不相等
//							  	  3：000地址Hopper与010地址Hopper地址面值相等，与001不相等
//							  	  4：001地址Hopper与010地址Hopper地址面值相等，与000不相等	
//							  	  5：三个Hopper的面值相等
unsigned char HpValueFlag = 0;


/*********************************************************************************************************
** Function name:     	Hopper_ValueListInit
** Descriptions:	    Hopper根据单位面值对HP结构链表进行高低排序函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Hopper_ValueListInit()
{
	uint16_t i,j,z,temp=0;
	

	//1.从Flash中读取Hp设置的面值，赋给Hp结构体中的单位价格
	//2.根据单位价格不同，进行从低到高排序
	i = stEvbHp[0].Price;
	j = stEvbHp[1].Price;
	z = stEvbHp[2].Price;

	//找出最大的斗		
	if( ( i >= j ) && ( i >= z ) )
	{	
		stEvbHp[0].Num = 3;
		if( j >= z )
		{
			stEvbHp[1].Num = 2;
			stEvbHp[2].Num = 1;
		}
		else
		{		
			stEvbHp[1].Num = 1;
			stEvbHp[2].Num = 2;
		}
	}
	else 
	if( ( i >= j ) && ( i <= z ) )
	{
		stEvbHp[2].Num = 3;
		stEvbHp[1].Num = 1;
		stEvbHp[0].Num = 2; 
	}
	else 
	if( ( j >= i ) && ( i >= z ) )
	{	
		stEvbHp[2].Num = 1;
		stEvbHp[1].Num = 3;
		stEvbHp[0].Num = 2;
	}
	else 
	if( ( j >= i ) && ( i <= z ) )
	{		
		stEvbHp[0].Num = 1;
		if( j >= z )
		{
			stEvbHp[1].Num = 3;
			stEvbHp[2].Num = 2;
		}
		else
		{
			stEvbHp[1].Num = 2;
			stEvbHp[2].Num = 3;
		}
	}	
	if(HpValueFlag==2)
	{
		if(stEvbHp[0].Num>stEvbHp[1].Num)
		{
			temp = stEvbHp[0].Num;
			stEvbHp[0].Num = stEvbHp[1].Num;
			stEvbHp[1].Num = temp;
		}	
	}	
}


/*********************************************************************************************************
** Function name:       ChangeGetTubes
** Descriptions:        启动时监测Hopper状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangeInitHopperState(void)
{
	if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{		
		MsgAccepterPack.PayoutCmd = MBOX_HOPPERINITSTATE;			
		OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);		
	}
}


/*********************************************************************************************************
** Function name:     	Hopper_Init
** Descriptions:	    Hopper设备初始化函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Hopper_Init()
{
	stEvbHp[0].Price = SystemPara.HopperValue[0];
	stEvbHp[1].Price = SystemPara.HopperValue[1];
	stEvbHp[2].Price = 0;
	stEvbHp[0].MemoryAdd = ADD_HP1;
	stEvbHp[1].MemoryAdd = ADD_HP2;
	stEvbHp[2].MemoryAdd = ADD_HP3;
	//对结构链表中的三个Hp做基本配置
	stEvbHp[0].State = 0;
	stEvbHp[0].Add = 0x00;
	stEvbHp[0].Index = 1;
	stEvbHp[1].State = 0;
	stEvbHp[1].Add = 0x01;
	stEvbHp[1].Index = 2;
	stEvbHp[2].State = 0;
	stEvbHp[2].Add = 0x02;
	stEvbHp[2].Index = 3;
	if((stEvbHp[0].Price == stEvbHp[1].Price)&&(stEvbHp[0].Price == stEvbHp[2].Price))
		HpValueFlag = 5;
	else
	if((stEvbHp[1].Price == stEvbHp[2].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))
		HpValueFlag = 4;
	else
	if((stEvbHp[0].Price == stEvbHp[2].Price)&&(stEvbHp[0].Price != stEvbHp[1].Price))
		HpValueFlag = 3;		
	else
	if((stEvbHp[0].Price == stEvbHp[1].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))
		HpValueFlag = 2;	
	else
	if(((stEvbHp[0].Price != stEvbHp[1].Price)&&(stEvbHp[0].Price != stEvbHp[2].Price))&&(stEvbHp[2].Price != stEvbHp[1].Price))
		HpValueFlag = 1;	
	//1.根据价格的大小不同，对三个Hp进行逻辑高低排序
	Hopper_ValueListInit();
	ChangeInitHopperState();
}

/*********************************************************************************************************
** Function name:     	Hopper_Distribute
** Descriptions:	    Hopper设备配币函数
** input parameters:    nMoney:需要找币的金额
						tHp1:000地址Hopper的找币枚数
						tHp2:010地址Hopper的找币枚数
						tHp3:100地址Hopper的找币枚数
** output parameters:   无
** Returned value:      1：配币成功；0：配备失败
*********************************************************************************************************/
uint8_t Hopper_Distribute(uint32_t nMoney,uint32_t *tHp1,uint32_t *tHp2,uint32_t *tHp3)
{
	uint32_t Count = 0;
	uint32_t i,nMoney1=0,nMoney2=0;
	uint8_t ln1=0,ln2=0,ln3=0;

	for(i=0;i<3;i++)
	{
		if(stEvbHp[i].Num == 1)
		{
			ln1 = i;
		}
		else
		if(stEvbHp[i].Num == 2)
		{
			ln2 = i;
		}
		else
		if(stEvbHp[i].Num == 3)
		{
			ln3 = i;
		}
	}
	//TraceChange("\r\nMidddistribute.1..%d..%d..%d..%d\r\n",stEvbHp[0].Price,stEvbHp[0].State,stEvbHp[0].DevBadFlag,stEvbHp[0].Num);
	//TraceChange("\r\nMidddistribute.2..%d..%d..%d..%d\r\n",stEvbHp[1].Price,stEvbHp[1].State,stEvbHp[1].DevBadFlag,stEvbHp[1].Num);
	//TraceChange("\r\nMidddistribute.3..%d..%d..%d..%d\r\n",stEvbHp[2].Price,stEvbHp[2].State,stEvbHp[2].DevBadFlag,stEvbHp[2].Num);
	//TraceChange("\r\nMidddistribute.1.1.%d..%d..%d..%d\r\n",stEvbHp[ln1].Price,stEvbHp[ln1].State,stEvbHp[ln1].DevBadFlag,stEvbHp[ln1].Num);
	//TraceChange("\r\nMidddistribute.2.2.%d..%d..%d..%d\r\n",stEvbHp[ln2].Price,stEvbHp[ln2].State,stEvbHp[ln2].DevBadFlag,stEvbHp[ln2].Num);
	//TraceChange("\r\nMidddistribute.3.3.%d..%d..%d..%d\r\n",stEvbHp[ln3].Price,stEvbHp[ln3].State,stEvbHp[ln3].DevBadFlag,stEvbHp[ln3].Num);
	*tHp1 = 0;
	*tHp2 = 0;
	*tHp3 = 0;
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln3].State != 3)&&(stEvbHp[ln3].State != 2)/*&&(stEvbHp[ln3].DevBadFlag < 2)*/)
	{
		Count = nMoney/stEvbHp[ln3].Price;
		nMoney1 = nMoney - Count*stEvbHp[ln3].Price;
		if(nMoney1 > nMoney)
			return 0;
		*tHp1 = Count;
	}
	else
	{
		*tHp1 = 0;
		nMoney1 = nMoney;
	}
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln2].State != 3)&&(stEvbHp[ln2].State != 2)/*&&(stEvbHp[ln2].DevBadFlag < 2)*/)
	{
		Count = nMoney1/stEvbHp[ln2].Price;
		nMoney2 = nMoney1 - Count*stEvbHp[ln2].Price;
		if(nMoney2 > nMoney1)
			return 0;
		*tHp2 = Count;
	}
	else
	{
		*tHp2 = 0;
		nMoney2 = nMoney1;
	}
	//如果状态不为故障或者不可服务状态则进行配币
	if((stEvbHp[ln1].State != 3&&(stEvbHp[ln1].State != 2))/*&&(stEvbHp[ln1].DevBadFlag < 2)*/)
	{
		Count = nMoney2/stEvbHp[ln1].Price;
		nMoney1 = nMoney2 - Count*stEvbHp[ln1].Price;
		if(nMoney1 > nMoney2)
			return 0;
		*tHp3 = Count;
	}
	else
	{
		*tHp3 = 0;
	}
	if((*tHp1==0)&&(*tHp2==0)&&(*tHp3==0))
		return 0;
	//TraceChange("\r\nMidddistribute.11..%d..%d..%d\r\n",stEvbHp[ln1].Price,stEvbHp[ln1].State,stEvbHp[ln1].DevBadFlag);
	//TraceChange("\r\nMidddistribute.22..%d..%d..%d\r\n",stEvbHp[ln2].Price,stEvbHp[ln2].State,stEvbHp[ln2].DevBadFlag);
	//TraceChange("\r\nMidddistribute.33..%d..%d..%d\r\n",stEvbHp[ln3].Price,stEvbHp[ln3].State,stEvbHp[ln3].DevBadFlag);
	return 1;
}


/*********************************************************************************************************
** Function name:     	Hopper_Dispence
** Descriptions:	    Hopper设备兑零操作函数
** input parameters:    ChangeMoney——需要兑零的金额
** output parameters:   SurplusMoney——兑零失败后剩余的金额
						Hp1OutNum——00地址Hopper的出币个数
						Hp2OutNum——01地址Hopper的出币个数
						Hp3OutNum——10地址Hopper的出币个数
** Returned value:      1：兑币成功；0：失败
*********************************************************************************************************/
uint32_t Hopper_Dispence(uint32_t ChangeMoney,uint32_t *SurplusMoney,uint8_t *Hp1OutNum,uint8_t *Hp2OutNum,uint8_t *Hp3OutNum)
{
	uint32_t Hp1Out=0,Hp2Out=0,Hp3Out=0;
	uint8_t res,mErrFlag = 5,err;
	uint8_t ln1=0,ln2=0,ln3=0,i;
	MessagePack *HpBackMsg;
	uint8_t hpoutcount = 0;
	uint32_t LastMoney = 0;

	//1.确定三个Hopper的单位面值大小顺序
	for(i=0;i<3;i++)
	{
		if(stEvbHp[i].Num == 1)
		{
			ln1 = i;
		}
		else
		if(stEvbHp[i].Num == 2)
		{
			ln2 = i;
		}
		else
		if(stEvbHp[i].Num == 3)
		{
			ln3 = i;
		}
	}
	//2.如果找零金额小于最小面值，则直接退出
	if(ChangeMoney < stEvbHp[ln1].Price)
	{
		*SurplusMoney = ChangeMoney; 
		return 0;
	}
	TraceChange("\r\nMidddprice=%d,%d,%d\r\n",stEvbHp[ln1].Price,stEvbHp[ln2].Price,stEvbHp[ln3].Price);

	
	
	while(mErrFlag)
	{
		//对Hopper进行配币
		res = Hopper_Distribute(ChangeMoney,&Hp1Out,&Hp2Out,&Hp3Out);
		TraceChange("\r\nMidddistribute..%d..%d..%d\r\n",Hp1Out,Hp2Out,Hp3Out);
		if(res == 1)
		{
			TraceChange("\r\nMidd H1=%d,H2=%d,H3=%d",Hp1Out,Hp2Out,Hp3Out);
			//mErrFlag--;
			if((stEvbHp[ln3].Price!=0)&&(Hp1Out>(ChangeMoney/stEvbHp[ln3].Price)))
				continue;
			if((stEvbHp[ln2].Price!=0)&&(Hp2Out>(ChangeMoney/stEvbHp[ln2].Price)))
				continue;
			if((stEvbHp[ln1].Price!=0)&&(Hp3Out>(ChangeMoney/stEvbHp[ln1].Price)))
				continue;
			//出币操作
			if(Hp1Out != 0)
			{
				MsgAccepterPack.PayoutCmd = MBOX_HOPPEROUT;
				MsgAccepterPack.PayoutType = stEvbHp[ln3].Index;
				MsgAccepterPack.PayoutNum = Hp1Out;
				//发送邮箱给DevieTask让其操作Hopper做出币操作
				OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
				TraceChange("\r\nMidd 2.1=%d,%d",MsgAccepterPack.PayoutType,MsgAccepterPack.PayoutNum);
				//等待DeviceTask任务返回应答邮箱，得到指定Hopper的出币情况
				HpBackMsg = OSMboxPend(g_ChangeBackMoneyMail,200*60*10,&err);
				if(err == OS_NO_ERR)
				{
					TraceChange("\r\nMidd 2.1BackCmd=%d",HpBackMsg->PayoutBackCmd);
					if(HpBackMsg->PayoutBackCmd== MBOX_HOPPERBACKMONEY)
					{
						LastMoney = ChangeMoney;
						hpoutcount = (uint8_t)HpBackMsg->BkPackValue;
						if(hpoutcount == 0)
						{
							//stEvbHp[ln3].DevBadFlag += 1;
							mErrFlag--;
							continue;
						}
						if(hpoutcount>Hp1Out)
							ChangeMoney -= Hp1Out*stEvbHp[ln3].Price;
						else
						{
							if(hpoutcount!=0)
								ChangeMoney = ChangeMoney - hpoutcount*stEvbHp[ln3].Price;
						}
						TraceChange("\r\nMidd 2.1out=%d",hpoutcount);
						if(ChangeMoney>LastMoney)
							ChangeMoney = LastMoney;
						if(stEvbHp[ln3].Index==3)
							*Hp3OutNum = hpoutcount;
						else
						if(stEvbHp[ln3].Index==2)
							*Hp2OutNum = hpoutcount;
						else
						if(stEvbHp[ln3].Index==1)
							*Hp1OutNum = hpoutcount;
						if(hpoutcount<Hp1Out)
						{
							mErrFlag--;
							//stEvbHp[ln3].DevBadFlag += 1;
							continue;
						}
					}
				}
				else
				{
					mErrFlag--;
					continue;
				}
			}	
			if(Hp2Out != 0)
			{		 
				MsgAccepterPack.PayoutCmd = MBOX_HOPPEROUT;
				MsgAccepterPack.PayoutType = stEvbHp[ln2].Index;
				MsgAccepterPack.PayoutNum = Hp2Out;
				//发送邮箱给DevieTask让其操作Hopper做出币操作
				OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
				TraceChange("\r\nMidd 2.2=%d,%d",MsgAccepterPack.PayoutType,MsgAccepterPack.PayoutNum);
				//等待DeviceTask任务返回应答邮箱，得到指定Hopper的出币情况
				HpBackMsg = OSMboxPend(g_ChangeBackMoneyMail,200*60*10,&err);
				if(err == OS_NO_ERR)
				{
					TraceChange("\r\nMidd 2.2BackCmd=%d",HpBackMsg->PayoutBackCmd);
					if(HpBackMsg->PayoutBackCmd== MBOX_HOPPERBACKMONEY)
					{
						LastMoney = ChangeMoney;
						hpoutcount = (uint8_t)HpBackMsg->BkPackValue;
						if(hpoutcount == 0)
						{
							//stEvbHp[ln2].DevBadFlag += 1;
							mErrFlag--;
							continue;
						}
						if(hpoutcount>Hp2Out)
							ChangeMoney -= Hp2Out*stEvbHp[ln2].Price;
						else
						{
							if(hpoutcount!=0)
								ChangeMoney = ChangeMoney - hpoutcount*stEvbHp[ln2].Price;
						}
						TraceChange("\r\nMidd 2.2out=%d",hpoutcount);
						if(ChangeMoney>LastMoney)
							ChangeMoney = LastMoney;
						if(stEvbHp[ln2].Index==3)
							*Hp3OutNum = hpoutcount;
						else
						if(stEvbHp[ln2].Index==2)
							*Hp2OutNum = hpoutcount;
						else
						if(stEvbHp[ln2].Index==1)
							*Hp1OutNum = hpoutcount;
						if(hpoutcount<Hp2Out)
						{
							mErrFlag--;
							//stEvbHp[ln2].DevBadFlag += 1;
							continue;
						}
					}
				}
				else
				{
					mErrFlag--;
					continue;
				}
			}
			if(Hp3Out != 0)
			{
				MsgAccepterPack.PayoutCmd = MBOX_HOPPEROUT;
				MsgAccepterPack.PayoutType = stEvbHp[ln1].Index;
				MsgAccepterPack.PayoutNum = Hp3Out;
				//发送邮箱给DevieTask让其操作Hopper做出币操作
				OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
				TraceChange("\r\nMidd 2.3=%d,%d",MsgAccepterPack.PayoutType,MsgAccepterPack.PayoutNum);
				//等待DeviceTask任务返回应答邮箱，得到指定Hopper的出币情况
				HpBackMsg = OSMboxPend(g_ChangeBackMoneyMail,200*60*10,&err);
				if(err == OS_NO_ERR)
				{
					TraceChange("\r\nMidd 2.3BackCmd=%d",HpBackMsg->PayoutBackCmd);
					if(HpBackMsg->PayoutBackCmd== MBOX_HOPPERBACKMONEY)
					{
						LastMoney = ChangeMoney;
						hpoutcount = (uint8_t)HpBackMsg->BkPackValue;
						if(hpoutcount == 0)
						{
							//stEvbHp[ln1].DevBadFlag += 1;
							mErrFlag--;
							continue;
						}
						if(hpoutcount>Hp3Out)
							ChangeMoney -= Hp3Out*stEvbHp[ln1].Price;
						else
						{
							if(hpoutcount!=0)
								ChangeMoney = ChangeMoney - hpoutcount*stEvbHp[ln1].Price;
						}
						TraceChange("\r\nMidd 2.3out=%d",hpoutcount);
						if(ChangeMoney>LastMoney)
							ChangeMoney = LastMoney;
						if(stEvbHp[ln1].Index==3)
							*Hp3OutNum = hpoutcount;
						else
						if(stEvbHp[ln1].Index==2)
							*Hp2OutNum = hpoutcount;
						else
						if(stEvbHp[ln1].Index==1)
							*Hp1OutNum = hpoutcount;
						if(hpoutcount<Hp3Out)
						{
							mErrFlag--;
							//stEvbHp[ln1].DevBadFlag += 1;
							continue;
						}
					}
				}
				else
				{
					mErrFlag--;
					continue;
				}
			}
			if((ChangeMoney>=stEvbHp[ln1].Price)&&(ChangeMoney!=0))
			{
				mErrFlag--;
				continue;
			}
			*SurplusMoney = ChangeMoney;
			return 1;	
		}
		else
		{
			mErrFlag--;
			continue;
		}
	}
	if(mErrFlag == 0)
	{
		*SurplusMoney = ChangeMoney; 
		return 0;
	}
	else
	{
		*SurplusMoney = ChangeMoney; 
		return 0;
	}	
}


/*********************************************************************************************************
** Function name:     	Hopper_DispenceAPI
** Descriptions:	    Hopper设备兑零操作函数
** input parameters:    ChangeMoney——需要兑零的金额
** output parameters:   SurplusMoney——兑零失败后剩余的金额
						Hp1OutNum——00地址Hopper的出币个数
						Hp2OutNum——01地址Hopper的出币个数
						Hp3OutNum——10地址Hopper的出币个数
** Returned value:      1：兑币成功；0：失败
*********************************************************************************************************/
uint32_t Hopper_DispenceAPI(uint32_t ChangeMoneyall,uint32_t *SurplusMoneyall,uint8_t *Hp1OutNum,uint8_t *Hp2OutNum,uint8_t *Hp3OutNum)
{
	uint8_t Hp1Out=0,Hp2Out=0,Hp3Out=0;
	uint8_t res,mErrFlag = 5,err;	
	uint8_t ln1=0,ln2=0,ln3=0,i;
	MessagePack *HpBackMsg;
	uint8_t hpoutcount = 0;
	uint32_t LastMoney = 0;

	uint32_t ChangeMoney = 0,SurplusMoney=0,minprice=0,dispenseValue=0,tempdispenseValue=0;
	uint8_t temp=0,j=0,hpprice[3]={0};

	//1.确定三个Hopper的单位面值大小顺序
	for(i=0;i<3;i++)
	{
		if(stEvbHp[i].Num == 1)
		{
			ln1 = i;
		}
		else
		if(stEvbHp[i].Num == 2)
		{
			ln2 = i;
		}
		else
		if(stEvbHp[i].Num == 3)
		{
			ln3 = i;
		}
	}
	//2.如果找零金额小于最小面值，则直接退出
	if(ChangeMoneyall < stEvbHp[ln1].Price)
	{
		*SurplusMoneyall = ChangeMoney; 
		return 0;
	}
	
	//3.计算最小找零面值
	if(stEvbHp[ln1].Price>0)
		hpprice[temp++]=stEvbHp[ln1].Price;
	if(stEvbHp[ln2].Price>0)
		hpprice[temp++]=stEvbHp[ln2].Price;
	if(stEvbHp[ln3].Price>0)
		hpprice[temp++]=stEvbHp[ln3].Price;	
	minprice=hpprice[0];
	for(j=0;j<temp;j++)
	{
		//TraceChange("\r\n%dhpprice=%ld,min=%ld",j,hpprice[j],minprice);
		if(minprice>hpprice[j])
		{
			minprice=hpprice[j];
			//TraceChange("\r\nnowmin=%ld",minprice);
		}
	}	
	//4.计算本次可找零数量
	dispenseValue = ChangeMoneyall / minprice;//发送找零基准数量	
	TraceChange("\r\nMidddprice=%d,%d,%d,min=%ld,change=%ld,dispense=%ld\r\n",stEvbHp[ln1].Price,stEvbHp[ln2].Price,stEvbHp[ln3].Price,minprice,ChangeMoneyall,dispenseValue);
	*SurplusMoneyall=0;
	while(dispenseValue>0)
	{
		tempdispenseValue=(dispenseValue>250)?250:dispenseValue;
		dispenseValue-=tempdispenseValue;

		ChangeMoney=tempdispenseValue*minprice;	
		TraceChange("\r\nMiddChangeAPI=%ld",ChangeMoney);
		Hopper_Dispence(ChangeMoney,&SurplusMoney, &Hp1Out, &Hp2Out, &Hp3Out);
		if(SurplusMoney > 0)
		{
			*SurplusMoneyall += SurplusMoney;
			TraceChange("\r\nMiddChangeAPI outfail=%ld",SurplusMoney);			
		}
		else
		{
			TraceChange("\r\nMiddChangeAPI outsucc");			
		}
		
	}
}

/*********************************************************************************************************
** Function name:       ChangeGetTubes
** Descriptions:        MDB硬币器余币量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangeGetTubesAPI(void)
{
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{		
		MsgAccepterPack.PayoutCmd = MBOX_MDBCHANGETUBE;			
		OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);		
	}
}


/*********************************************************************************************************
** Function name:		MDBMakeChange
** Descriptions:		MDB硬币器计算找零方案
** input parameters:	
**                      length 可找零的币种种类 
**                      coins  各个可找零的币种面额
**                      lefts  每种币剩余量
**                      money  需要找零金额         
** output parameters:	
                        results 找零结果（返回结果1代表"成功"时，分别列出了各面额找零枚数）
** Returned value:		找零结果（返回结果"成功"时有效）返回1：成功；返回0：失败
*********************************************************************************************************/
uint8_t MDBMakeChange(int length, unsigned char* coins,
unsigned char* lefts, unsigned char* results, int money)
{
    unsigned char  i=0;
    unsigned char  count;
    //printf("allMoney=%d\n", money);

    for(i=0; i<length; ++i)
    {
        unsigned char coin = coins[i];
        unsigned char left = lefts[i];

        if(left <= 0 || coin > money)
        {
            continue;
        }
        count = money / coin;

        if(count > left)
        {
            count = left;
        }
        lefts[i] = left - count;

        //printf("%d=%d\n",coin,count);
        results[i] = count;

        money -= count*coin;
        if(money == 0)
        {
            break;
        }
    }

    if(money == 0)
    {
        return 1;
    }
    else
    {
        for(i=0; i<length; ++i)
        {
            results[i] = 0;
        }
        return 0;
    }
}

/*********************************************************************************************************
** Function name:		MDBchange
** Descriptions:		MDB硬币器计算找零方案(上层函数)
** input parameters:	
**                      money  需要找零金额         
**                      找零money元,即3.5元=350(money)
** output parameters:	
                        results 找零结果（返回结果1代表"成功"时，分别列出了各面额找零枚数,从大到小排列）
                        *recoin 基准面值，因为返回结果是从大到小排列的，需要通过它知道最大的出币面额是多少钱，然后接下来的依次递减
** Returned value:		找零结果（返回结果"成功"时有效）返回1：成功；返回0：失败
*********************************************************************************************************/
uint8_t MDBchange(unsigned char* results,unsigned char* recoin, int money)
{
    //*可找零硬币面额（人民币分）（面额从大到小）*
    unsigned char coins[8]={0,0,0,0,0,0,0,0};
    //*每种面额硬币剩余个数 *
    unsigned char lefts[8]={0,0,0,0,0,0,0,0};
	uint8_t i,j=0;

	//后面几个0跳过排序
	for(i = 7;i > 0;i--)
	{
		if(stDevValue.CoinValue[i] > 0)
		{
			*recoin = i;//得到最大的出币面额
			break;
		}
	}

	//将它的面值，以及各面值存币数量从大到小倒叙排列
	do
	{
		coins[j] = stDevValue.CoinValue[i];
		lefts[j] = stDevValue.CoinNum[i];
		j++;
	}
	while(i--);
	
    

	TraceChange("\r\nMiddChange%d,%d,%d,%d,%d,%d,%d,%d",stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3],stDevValue.CoinValue[4],stDevValue.CoinValue[5],stDevValue.CoinValue[6],stDevValue.CoinValue[7]);
	TraceChange("\r\nMiddChange%d,%d,%d,%d,%d,%d,%d,%d",stDevValue.CoinNum[0],stDevValue.CoinNum[1],stDevValue.CoinNum[2],stDevValue.CoinNum[3],stDevValue.CoinNum[4],stDevValue.CoinNum[5],stDevValue.CoinNum[6],stDevValue.CoinNum[7]);
	TraceChange("\r\nMiddChange%d,%d,%d,%d,%d,%d,%d,%d",coins[0],coins[1],coins[2],coins[3],coins[4],coins[5],coins[6],coins[7]);	
	TraceChange("\r\nMiddChange%d,%d,%d,%d,%d,%d,%d,%d",lefts[0],lefts[1],lefts[2],lefts[3],lefts[4],lefts[5],lefts[6],lefts[7]);
	TraceChange("\r\nMiddChange j=%d",j);
	
    if(MDBMakeChange(j, coins, lefts, results, money))
    {
        
        TraceChange("\r\nMiddChangeCNY Success=%ld,%d\n",money,*recoin);		
		TraceChange("\r\nMiddChange %d,%d,%d,%d,%d,%d,%d,%d",results[0],results[1],results[2],results[3],results[4],results[5],results[6],results[7]);
        return 1;
    }
    else
    {
        TraceChange("\r\nMiddChange CNY Fail\n");
        return 0;
    }

}

/*********************************************************************************************************
** Function name:       ChangePayoutProcessLevel2
** Descriptions:        level2配币找零操作
** input parameters:    PayType——出币通道
**                      PayNum ——出币数量
** output parameters:   PayoutNum--各通道实际出币数量							
** Returned value:      有硬币找出返回1，无返回0
*********************************************************************************************************/
unsigned char ChangePayoutProcessLevel2API(unsigned int PayMoney, unsigned char PayoutNum[8])
{
	uint8_t  i,j;
	uint8_t err;
	uint8_t resultdisp[8]={0, 0, 0, 0, 0, 0, 0, 0},tempdisp = 0;
	uint8_t recoin;
	unsigned char ComStatus;
	MessagePack *AccepterMsg;
	
	//NowChangerDev = SystemPara.CoinChangerType;
	//Trace("6\r\n");	
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		ChangeGetTubesAPI();
		OSTimeDly(OS_TICKS_PER_SEC / 10);
		BillDevDisableAPI();
		CoinDevDisableAPI();
				
		/***************************************
		** level2级别，需要手动配币
		***************************************/  		
		{
			//1配币函数,得到每种面额的硬币的出币数量,以及最大出币面值
			ComStatus = MDBchange(resultdisp, &recoin, PayMoney);
			if(ComStatus == 1)
			{
				for(i=0;i<8;i++)
				{
					//2遇到不用出币的面额，跳过
					if(resultdisp[i]==0) 
					{
						if(recoin > 0)//更新基准
							recoin--;
						continue;
					}
					
					TraceChange("\r\nMiddChangeCNY i=%d,=%d\n",i,resultdisp[i]);
					//3需要出币的面额
					while(resultdisp[i])
					{
						//4最大一次只能出15枚，进行判断是否要分几次出币
						if(resultdisp[i] > 15)
						{
							tempdisp = 15;
							resultdisp[i] -= tempdisp;
						}
						else
						{
							tempdisp = resultdisp[i];
							resultdisp[i] -= tempdisp;
						}
						TraceChange("\r\nMiddChange2CNY re=%d, =%d\n",recoin,tempdisp);
						/*						
						VMCPoll[0]  =  recoin&0x0f;
    					VMCPoll[0] |= ((tempdisp<<4)&0xf0);
						ComStatus = MdbConversation(0x0D,VMCPoll,1,&CoinRdBuff[0],&CoinRdLen);
						//6上传通道的出币枚数
						if(ComStatus == 1)
						{
							PayoutNum[recoin] += tempdisp;	
							//Trace("\r\n nu=%d,%d",recoin,PayoutNum[recoin]);
						}	
						*/
						//5出币
						MsgAccepterPack.PayoutCmd = MBOX_MDBCHANGELEVEL2;	
						MsgAccepterPack.PayoutType = recoin;
						MsgAccepterPack.PayoutNum = tempdisp;
						OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
						while(1)
						{
							AccepterMsg = OSMboxPend(g_ChangeBackMoneyMail,OS_TICKS_PER_SEC/2,&err);//返回找零结果
							//等待找零结束//MDB硬币器找零
							if(err == OS_NO_ERR) 
							{								
								for(j = 0;j < 8;j++)
								{
									PayoutNum[j] += AccepterMsg->PayoutBackNum[j];									
								}
								break;
							}
						}
						
						
					}
					//更新基准
					if(recoin > 0)
						recoin--;
					
				}
				return 1;
			}
		}
		
		OSTimeDly(OS_TICKS_PER_SEC / 100);
		ChangeGetTubesAPI();		
	}	
	return 0;
}


/*********************************************************************************************************
** Function name:       ChangerDevPayoutAPI
** Descriptions:        找零器找零
** input parameters:    money需要找零的金额
** output parameters:   debtMoney欠款金额
** Returned value:      1找零成功,0找零失败
*********************************************************************************************************/
uint8_t ChangerDevPayoutAPI(uint32_t money,uint32_t *debtMoney)
{		
	MessagePack *AccepterMsg;
	uint8_t err,i;
	unsigned char PayoutNum[8] ={0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t backmoney;
	unsigned char HopperoutNum[3] ={0, 0, 0};
	unsigned char ComStatus;

	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		//stDevValue.CoinLevel = 2;
		if(stDevValue.CoinLevel >= 3)
		{
			MsgAccepterPack.PayoutCmd = MBOX_MDBCHANGELEVEL3;	
			MsgAccepterPack.PayoutMoney = money;
			OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
			for(i=0;i<10;i++)
			{
				AccepterMsg = OSMboxPend(g_ChangeBackMoneyMail,OS_TICKS_PER_SEC*5,&err);//返回找零结果
				//等待找零结束//MDB硬币器找零
				if(err == OS_NO_ERR) 
				{		
					TraceChange("\r\nMiddChange BackCmd=%d",AccepterMsg->PayoutBackCmd);
					if(AccepterMsg->PayoutBackCmd== MBOX_CHANGERBACKMONEY)
					{
						backmoney = stDevValue.CoinValue[0]*(AccepterMsg->PayoutBackNum[0]) + stDevValue.CoinValue[1]*(AccepterMsg->PayoutBackNum[1]) + stDevValue.CoinValue[2]*(AccepterMsg->PayoutBackNum[2])
						+stDevValue.CoinValue[3]*(AccepterMsg->PayoutBackNum[3]) + stDevValue.CoinValue[4]*(AccepterMsg->PayoutBackNum[4]) + stDevValue.CoinValue[5]*(AccepterMsg->PayoutBackNum[5])
						+stDevValue.CoinValue[6]*(AccepterMsg->PayoutBackNum[6]) + stDevValue.CoinValue[7]*(AccepterMsg->PayoutBackNum[7])
						+stDevValue.CoinValue[8]*(AccepterMsg->PayoutBackNum[8]) + stDevValue.CoinValue[9]*(AccepterMsg->PayoutBackNum[9]) + stDevValue.CoinValue[10]*(AccepterMsg->PayoutBackNum[10])
						+stDevValue.CoinValue[11]*(AccepterMsg->PayoutBackNum[11]) + stDevValue.CoinValue[12]*(AccepterMsg->PayoutBackNum[12]) + stDevValue.CoinValue[13]*(AccepterMsg->PayoutBackNum[13])
						+stDevValue.CoinValue[14]*(AccepterMsg->PayoutBackNum[14]) + stDevValue.CoinValue[15]*(AccepterMsg->PayoutBackNum[15]);  
						TraceChange("\r\nMiddChange out=%d,%d,%ld",AccepterMsg->PayoutBackNum[0],AccepterMsg->PayoutBackNum[1],backmoney);
						backmoney = money - backmoney;
						if(backmoney > 0)
						{
							*debtMoney = backmoney;
							TraceChange("\r\nMiddChange outfail=%ld",*debtMoney);
							return 0;
						}
						else
						{
							TraceChange("\r\nMiddChange outsucc");
							return 1;
						}
					}
				}
			}
			if(i >= 10)
			{
				*debtMoney = money;
				TraceChange("\r\nMiddChange outtime-fail=%ld",*debtMoney);
				return 0;
			}
		}
		else
		{
			ComStatus =ChangePayoutProcessLevel2API(money,PayoutNum);
			if(ComStatus)
			{
				TraceChange("\r\nMiddChange out=%d,%d",PayoutNum[0],PayoutNum[1]);
				TraceChange("\r\nMiddChange outsucc");
				PayoutNum[0] = 0;
				PayoutNum[1] = 0;
				return 1;
			}
			else
			{				
				*debtMoney = money;
				TraceChange("\r\nMiddChange outfail=%ld",*debtMoney);
				return 0;
			}
		}
	}
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		TraceChange("\r\nMiddChange 1.out=%ld",money);
		Hopper_DispenceAPI(money, &backmoney, &HopperoutNum[0], &HopperoutNum[1], &HopperoutNum[2]);
		if(backmoney > 0)
		{
			*debtMoney = backmoney;
			TraceChange("\r\nMiddChange outfail=%ld",*debtMoney);
			return 0;
		}
		else
		{
			TraceChange("\r\nMiddChange outsucc");
			return 1;
		}
	}	

	return 1;
}


/*********************************************************************************************************
** Function name:       ChangerDevInit
** Descriptions:        找零器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangerDevInitAPI()
{
	switch(SystemPara.CoinChangerType)
	{		
		case HOPPER_CHANGER:
			Hopper_Init();
			TraceChange("\r\nMiddChangeinit %d,%d,%d,%d,%d,%d",stEvbHp[0].Index,stEvbHp[0].Num,stEvbHp[0].State,stEvbHp[0].Price,stEvbHp[0].Add,stEvbHp[0].DevBadFlag);
			TraceChange("\r\nMiddChangeinit %d,%d,%d,%d,%d,%d",stEvbHp[1].Index,stEvbHp[1].Num,stEvbHp[1].State,stEvbHp[1].Price,stEvbHp[1].Add,stEvbHp[1].DevBadFlag);
			TraceChange("\r\nMiddChangeinit %d,%d,%d,%d,%d,%d",stEvbHp[2].Index,stEvbHp[2].Num,stEvbHp[2].State,stEvbHp[2].Price,stEvbHp[2].Add,stEvbHp[2].DevBadFlag);
			break;		
	}
	return;
}

/*********************************************************************************************************
** Function name:       TestHopperHandle
** Descriptions:        测试找零器
** input parameters:    HopperNum——所测试的找零器 1为000地址 2为001地址 3为010地址
						OutCount——找零的枚数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TestHopperHandle(unsigned char HopperNum,unsigned char OutCount)
{
	MessagePack *HpBackMsg;
	uint8_t err;
	
	if(stEvbHp[HopperNum-1].Price!=0)
	{
		MsgAccepterPack.PayoutCmd = MBOX_HOPPEROUT;
		MsgAccepterPack.PayoutType = HopperNum;
		MsgAccepterPack.PayoutNum = OutCount;
		//发送邮箱给DevieTask让其操作Hopper做出币操作
		OSMboxPost(g_ChangeMoneyMail,&MsgAccepterPack);
		//HopperAccepter_HpOutHandle(OutCount,HopperNum,Bufer);
		//等待DeviceTask任务返回应答邮箱，得到指定Hopper的出币情况
		HpBackMsg = OSMboxPend(g_ChangeBackMoneyMail,12000,&err);
		if(err == OS_NO_ERR)
		{
			if(HpBackMsg->PayoutBackCmd== MBOX_HOPPERBACKMONEY)
			{}
		}
	}
}








