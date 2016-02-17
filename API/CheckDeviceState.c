/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CheckDeviceState
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        检测设备故障状态相关函数声明                     
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
uint8_t	 	billEnable = 1;//当前纸币器状态,1打开,0关闭
uint8_t	 	coinEnable = 1;//当前硬币器状态,1打开,0关闭
uint8_t	 	scaleError = 0;//当前金额数据比例系数关系引起的错误,1错误,0正确
uint8_t	 	weihuMode = 0;//当前在维护状态1,交易状态0


extern void BillCoinCtr(uint8_t billCtr,uint8_t coinCtr,uint8_t readerCtr);

/*********************************************************************************************************
** Function name:       CheckDeviceState
** Descriptions:        更新纸币器，硬币器，找零器设备故障状态
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void CheckDeviceState()
{ 
	uint8_t err;
	DEVICESTATEPACK *DeviceMsg;
	
	//查询各设备状态
	DeviceStatePack.DevStateCmd = MBOX_GETDEVSTATE;						
	OSMboxPost(g_DeviceStateMail,&DeviceStatePack);
	//OSTimeDly(OS_TICKS_PER_SEC/2);
	//返回各设备状态
	DeviceMsg = OSMboxPend(g_DeviceStateBackMail,OS_TICKS_PER_SEC*30,&err);		
	if(err == OS_NO_ERR) 
	{
		TraceChange("\r\n Middle1Change");
		if(DeviceMsg->DevStateCmd == MBOX_RETURNDEVSTATE)
		{
			TraceChange("\r\n Middle2Change");
			memcpy(&DeviceStateBusiness,DeviceMsg,sizeof(DeviceStateBusiness));
			DeviceStateBusiness.ErrorInit = 1;
			TraceBill("\r\n Middlebill=%d,%d,%d,%d,%d,%d,%d,%d",DeviceStateBusiness.BillCommunicate,DeviceStateBusiness.Billmoto,DeviceStateBusiness.Billsensor,DeviceStateBusiness.Billromchk,
				DeviceStateBusiness.Billjam,DeviceStateBusiness.BillremoveCash,DeviceStateBusiness.BillcashErr,DeviceStateBusiness.Billdisable);
			TraceCoin("\r\n Middlecoin=%d,%d,%d,%d,%d,%d,%d,%d,%d",DeviceStateBusiness.CoinCommunicate,DeviceStateBusiness.Coinsensor,DeviceStateBusiness.Cointubejam,DeviceStateBusiness.Coinromchk,
				DeviceStateBusiness.Coinrouting,DeviceStateBusiness.Coinjam,DeviceStateBusiness.CoinremoveTube,DeviceStateBusiness.Coindisable,DeviceStateBusiness.CoinunknowError);
			TraceChange("\r\n MiddleHopper2=%d,%d,%d",DeviceStateBusiness.Hopper1State,DeviceStateBusiness.Hopper2State,DeviceStateBusiness.Hopper3State);
			TraceChannel("\r\n MiddleChannel=%d,%d,%d,%d",DeviceStateBusiness.GOCError,DeviceStateBusiness.GOCErrorSub,DeviceStateBusiness.ColBoardError,DeviceStateBusiness.ColBoardErrorSub);
			TraceChannel("\r\n MiddleChannelSJT1=%d,%d,%d,%d,%d",DeviceStateBusiness.Error_FMD,DeviceStateBusiness.Error_FUM,DeviceStateBusiness.Error_GOC,DeviceStateBusiness.Error_FLD,DeviceStateBusiness.Error_FOD);
			TraceChannel("\r\n MiddleChannelSJT2=%d,%d,%d,%d,%d",DeviceStateBusiness.Error_UDS,DeviceStateBusiness.Error_GCD,DeviceStateBusiness.Error_SOVB,DeviceStateBusiness.Error_SOMD1,DeviceStateBusiness.Error_SOEC);
			TraceChannel("\r\n MiddleChannelSJT3=%d,%d,%d,%d",DeviceStateBusiness.Error_SFHG,DeviceStateBusiness.Error_SOFP,DeviceStateBusiness.Error_SOMD2,DeviceStateBusiness.Emp_Gol);
		}
	}
}

/*********************************************************************************************************
** Function name:       SetScaleError
** Descriptions:        设置金额数据比例系数关系是否故障
** input parameters:    errorValue = 1故障,0正确
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void SetScaleError(uint8_t errorValue)
{
	scaleError = errorValue;
}

/*********************************************************************************************************
** Function name:       GetScaleError
** Descriptions:        得到金额数据比例系数关系是否故障
** input parameters:     1故障,0正确
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
uint8_t GetScaleError()
{
	return scaleError;
}



/*********************************************************************************************************
** Function name:       IsErrorState
** Descriptions:        是否进入故障状态
** input parameters:    无
** output parameters:   无
** Returned value:      1故障，0正常 
*********************************************************************************************************/
uint8_t IsErrorState()
{ 
	uint8_t coinError = 0,hopperError = 0,GOCError = 0,ColBoardError = 0,PcErr=0;
	static uint8_t billError = 0;
	//纸币器	
	if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
	{
		if(
			(DeviceStateBusiness.BillCommunicate)||(DeviceStateBusiness.Billmoto)||(DeviceStateBusiness.Billsensor)||(DeviceStateBusiness.Billromchk)
			||(DeviceStateBusiness.Billjam)||(DeviceStateBusiness.BillremoveCash)||(DeviceStateBusiness.BillcashErr)
		  )
		{
			billError = 1;
		}
		else if(billError == 1)
		{
			billError = 2;
		}
		TraceBill("\r\n MiddBillState=%d",billError);
	}
	
	//硬币器
	if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
	{
		if(
			(DeviceStateBusiness.CoinCommunicate)||(DeviceStateBusiness.Coinsensor)||(DeviceStateBusiness.Cointubejam)||(DeviceStateBusiness.Coinromchk)
			||(DeviceStateBusiness.Coinjam)||(DeviceStateBusiness.CoinremoveTube)||(DeviceStateBusiness.CoinunknowError)
		  )
		{
			coinError = 1;
		}
	}
	//Hopper找零器
	if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		if(DeviceStateBusiness.Hopper1State == 2)
		{
			hopperError = 1;
		}
	}
	if(SystemPara.GeziDeviceType==0)
	{
		//GOC
		if(SystemPara.GOCIsOpen == 1)
		{
			if(SystemPara.SubBinOpen==1)
			{
				if( (DeviceStateBusiness.GOCError != 0)&&(DeviceStateBusiness.GOCErrorSub != 0) )
				{
					GOCError = 1;
				}
			}
			else
			{
				if(DeviceStateBusiness.GOCError != 0)
				{
					GOCError = 1;
				}
			}
		}
		//货道板
		if(SystemPara.SubBinOpen==1)
		{
			if( (DeviceStateBusiness.ColBoardError != 0)&&(DeviceStateBusiness.ColBoardErrorSub != 0) )
			{
				ColBoardError = 1;
			}
		}
		else
		{
			if(DeviceStateBusiness.ColBoardError != 0)
			{
				ColBoardError = 1;
			}
		}
		//升降台
		if(SystemPara.ChannelType == 2)
		{
			if(
				(DeviceStateBusiness.Error_FMD)||(DeviceStateBusiness.Error_FUM)||(DeviceStateBusiness.Error_GOC)||(DeviceStateBusiness.Error_FLD)||(DeviceStateBusiness.Error_FOD)
				||(DeviceStateBusiness.Error_UDS)||(DeviceStateBusiness.Error_GCD)||(DeviceStateBusiness.Error_SOVB)||(DeviceStateBusiness.Error_SOMD1)||(DeviceStateBusiness.Error_SOEC)
				||(DeviceStateBusiness.Error_SFHG)||(DeviceStateBusiness.Error_SOFP)||(DeviceStateBusiness.Error_SOMD2)
			  )
			{
				ColBoardError = 1;
			}
		}
		//if(DeviceStateBusiness.Emp_Gol != 0)
		//{
		//	ColEmpErr = 1;
		//}
	}

	if(SystemPara.PcEnable == UBOX_PC)
	{
		if(LogPara.offLineFlag == 1)
			PcErr = 1;
	}

	if(coinError||hopperError||GOCError||ColBoardError||scaleError||PcErr)
	{
		return 1;
	}
	else
	{
		if(billError == 2)
		{
			billError = 0;
			ResetBill();
			TraceBill("\r\n MiddBillState2=%d",billError);
		}
		return 0;
	}
}

/*********************************************************************************************************
** Function name:       HopperIsEmpty
** Descriptions:        Hopper找利器是否为空
** input parameters:    无
** output parameters:   无
** Returned value:      1缺币,0足够
*********************************************************************************************************/
uint8_t HopperIsEmpty()
{ 
	if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		//if((DeviceStateBusiness.Hopper1State == 1)&&(DeviceStateBusiness.Hopper2State != 0)&&(DeviceStateBusiness.Hopper3State != 0))
        if(DeviceStateBusiness.Hopper1State == 1)
		{
			return 1;
		}
	}
	return 0;
}



/*********************************************************************************************************
** Function name:       ChangerIsErr
** Descriptions:        找零器是否不可用状态
** input parameters:    1故障,0正常
** output parameters:   无
** Returned value:      1是,0不是
*********************************************************************************************************/
uint8_t ChangerIsErr()
{ 
	if(SystemPara.CoinChangerType==OFF_CHANGER)
	{
		return 1;
	}
	else if(SystemPara.CoinChangerType==MDB_CHANGER)
	{
		if(
			(DeviceStateBusiness.CoinCommunicate)||(DeviceStateBusiness.Coinsensor)||(DeviceStateBusiness.Cointubejam)||(DeviceStateBusiness.Coinromchk)
			||(DeviceStateBusiness.Coinjam)||(DeviceStateBusiness.CoinremoveTube)||(DeviceStateBusiness.CoinunknowError)
		  )
		{
			return 1;
		}
	}
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		if((DeviceStateBusiness.Hopper1State != 0)&&(DeviceStateBusiness.Hopper2State != 0)&&(DeviceStateBusiness.Hopper3State != 0))
		{
			return 1;
		}
	}
	return 0;
}

/*********************************************************************************************************
** Function name:       BillIsErr
** Descriptions:        纸币器是否不可用状态
** input parameters:    1故障,0正常
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
uint8_t BillIsErr()
{ 
	if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
	{
		if(
			(DeviceStateBusiness.BillCommunicate)||(DeviceStateBusiness.Billmoto)||(DeviceStateBusiness.Billsensor)||(DeviceStateBusiness.Billromchk)
			||(DeviceStateBusiness.Billjam)||(DeviceStateBusiness.BillremoveCash)||(DeviceStateBusiness.BillcashErr)
		  )
		{
			return 1;
		}		
		//TraceBill("\r\n MiddBillState=%d",billError);
	}
	return 0;
}


/*********************************************************************************************************
** Function name:       SetBillCoinStatus
** Descriptions:        设置纸币器,硬币器当前状态
** input parameters:    type=1纸币器,2硬币器;enable = 1启动,0关闭
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void SetBillCoinStatus(uint8_t type,uint8_t enable)
{
	if(type == 1)
		billEnable = enable;
	else if(type == 2)
		coinEnable = enable;
}

/*********************************************************************************************************
** Function name:       SetBillCoinStatus
** Descriptions:        得到纸币器,硬币器当前状态
** input parameters:    type=1纸币器,2硬币器;
** output parameters:   无
** Returned value:      1打开,0关闭
*********************************************************************************************************/
uint8_t GetBillCoinStatus(uint8_t type)
{
	if(type == 1)
		return billEnable;
	else if(type == 2)
		return coinEnable;
	return 0;
}


/*********************************************************************************************************
** Function name:       SetWeihuStatus
** Descriptions:        设置维护状态
** input parameters:    enable=1维护状态;0交易状态
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void SetWeihuStatus(uint8_t enable)
{
	weihuMode = enable;	
}

/*********************************************************************************************************
** Function name:       GetWeihuStatus
** Descriptions:        设置维护状态
** input parameters:    enable=1维护状态;0交易状态
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
uint8_t GetWeihuStatus()
{
	return weihuMode;	
}


/*********************************************************************************************************
** Function name:       ErrorStatus
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
uint8_t ErrorStatus(uint8_t type)
{ 
	//找零器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	if(type == 1)
	{
		//MDB找零器
		if(SystemPara.CoinChangerType==MDB_CHANGER)
		{
			if(
				(DeviceStateBusiness.CoinCommunicate)||(DeviceStateBusiness.Coinsensor)||(DeviceStateBusiness.Cointubejam)||(DeviceStateBusiness.Coinromchk)
				||(DeviceStateBusiness.Coinjam)||(DeviceStateBusiness.CoinremoveTube)||(DeviceStateBusiness.CoinunknowError)
			  )
			{
				return 2;
			}			
			
		}
		//Hopper找零器
		else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
		{
			if(DeviceStateBusiness.Hopper1State == 2)
			{
				return 2;
			}
		}
		else
			return 3;
		
		return 0;
	}
	//纸币器	0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	if(type == 2)
	{
		if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
		{
			if(
				(DeviceStateBusiness.BillCommunicate)||(DeviceStateBusiness.Billmoto)||(DeviceStateBusiness.Billsensor)||(DeviceStateBusiness.Billromchk)
				||(DeviceStateBusiness.Billjam)||(DeviceStateBusiness.BillremoveCash)||(DeviceStateBusiness.BillcashErr)
			  )
			{
				return 2;
			}	
			if(GetBillCoinStatus(1)==0)
				return 1;
			
			return 0;
		}
		else
			return 3;
	}
	
	//硬币器   硬币器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	if(type == 3)
	{
		if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
		{
			if(
				(DeviceStateBusiness.CoinCommunicate)||(DeviceStateBusiness.Coinsensor)||(DeviceStateBusiness.Cointubejam)||(DeviceStateBusiness.Coinromchk)
				||(DeviceStateBusiness.Coinjam)||(DeviceStateBusiness.CoinremoveTube)||(DeviceStateBusiness.CoinunknowError)
			  )
			{
				return 2;
			}
			if(GetBillCoinStatus(2)==0)
				return 1;
			
		}
		//脉冲硬币器
		else if((SystemPara.CoinAcceptorType == PARALLEL_COINACCEPTER)||(SystemPara.CoinAcceptorType == SERIAL_COINACCEPTER))
		{
			if(GetBillCoinStatus(2)==0)
				return 1;			
		}
		else
			return 3;
		
		return 0;
	}
	//vmc 0=正常，1= 正常货道商品全部售空，或非售卖时间，2=故障，3=维护模式
	if(type == 4)
	{
		//if((DeviceStateBusiness.Emp_Gol != 0)&&(SystemPara.GeziDeviceType==0))
		//	return 1;
		//else 
		if(IsErrorState())
			return 2;
		else if(weihuMode==1)
			return 3;
		else
			return 0;
	}
	return 0;
}

/*********************************************************************************************************
** Function name:       SIMPLEErrorStatus
** Descriptions:        为SIMPLEUBox作的整机状态检测接口
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
uint8_t SIMPLEErrorStatus(uint8_t type)
{ 
	//正常货道商品全部售空，或非售卖时间
	if(type == 1)
	{
		if((DeviceStateBusiness.Emp_Gol != 0)&&(SystemPara.GeziDeviceType==0))
			return 1;
		else
			return 0;
	}
	//是否故障
	if(type == 2)
	{
		if(IsErrorState())
			return 1;
		else	
			return 0;		
	}
	
	//维护模式
	if(type == 3)
	{
		if(weihuMode==1)
			return 1;
		else
			return 0;
	}
	return 0;	
}



