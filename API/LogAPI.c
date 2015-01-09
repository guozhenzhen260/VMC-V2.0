/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LogAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        日志程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
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
#include "..\Uart3Driver\ZHIHUIPC.h"


//static unsigned char run_no = 0,err_no = 0;



/*********************************************************************************************************
** Function name:       LogGetMoneyAPI
** Descriptions:        开始交易时更新日志记录
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogBeginTransAPI(void)
{
	//更新明细交易记录
	LogParaDetail.BeginYear = RTCData.year;
	LogParaDetail.BeginMonth = RTCData.month;
	LogParaDetail.BeginDay = RTCData.day;
	LogParaDetail.BeginHour = RTCData.hour;
	LogParaDetail.BeginMinute = RTCData.minute;	

	LogParaDetail.BeginSec = RTCData.second;//add by yoc 2013.9.11
	
	//Trace(("\r\n 4.=%ld,%ld,%ld",LogParaDetail.IncomeBill,LogParaDetail.IncomeCoin,LogParaDetail.IncomeCard);
}



/*********************************************************************************************************
** Function name:       LogGetMoneyAPI
** Descriptions:        收币时更新日志
** input parameters:    InValue投币的金额,MoneyType=1硬币器,2纸币器,3读卡器
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogGetMoneyAPI(uint32_t InValue,uint8_t MoneyType)
{
	if((MoneyType == 1)||(MoneyType == 2))
	{
		//更新区域交易记录
		LogPara.Income += InValue;
		if(MoneyType == 1)
			LogPara.CoinsIncome += InValue;
		else if(MoneyType == 2)
			LogPara.NoteIncome += InValue;

		//更新总交易记录
		LogPara.IncomeTotal += InValue;
		if(MoneyType == 1)
			LogPara.CoinsIncomeTotal += InValue;
		else if(MoneyType == 2)
			LogPara.NoteIncomeTotal += InValue;
	}

	//更新明细交易记录
	if(MoneyType == 1)
		LogParaDetail.IncomeCoin += InValue;
	else if(MoneyType == 2)
		LogParaDetail.IncomeBill += InValue;
	else if(MoneyType == 3)
		LogParaDetail.IncomeCard = InValue;
	
}

/*********************************************************************************************************
** Function name:       LogTransactionAPI
** Descriptions:        交易出货时更新日志
** input parameters:    InValue出货的金额,transMul第几次交易,ChannelNum出货货道,payMode交易类型
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogTransactionAPI(uint32_t InValue,uint8_t transMul,char *ChannelNum,uint8_t payMode)
{
	/******************add by yoc 2013.9.11**********************************/

	//重新定义出货方式
	// 0 现金  1 一卡通  2银联卡 3 PC出货 4支付宝 5游戏出货 6手机出货 。。。 
	unsigned char logicNo,pay_mode;
	//根据交易类型更新交易次数和金额
	if(SystemPara.PcEnable == UBOX_PC)//友宝协议
	{
		if(payMode == 0)//现金购物
		{
			LogPara.vpCashNum ++;
			LogPara.vpCashMoney += InValue;
			LogPara.vpCashNumTotal ++;
			LogPara.vpCashMoneyTotal += InValue;
			pay_mode = PAYMODE_CASH;
		}
		else if(payMode == 1)//游戏出货
		{
			LogPara.vpGameNum ++;
			LogPara.vpGameMoney += InValue;
			LogPara.vpGameNumTotal ++;
			LogPara.vpGameMoneyTotal += InValue;
			pay_mode = PAYMODE_GAME;
		}
		else if((payMode == 5) || ((payMode >= 101) && (payMode <= 255)))//刷卡出货
		{
			LogPara.vpCardNum ++;
			LogPara.vpCardMoney += InValue;
			LogPara.vpCardNumTotal ++;
			LogPara.vpCardMoneyTotal += InValue;
			pay_mode = PAYMODE_UNIONPAY;
		}
		else if(((payMode >= 2) && (payMode <= 4)) || ((payMode >= 6) && (payMode <= 100)))//在线出货
		{
			LogPara.vpOnlineNum ++;
			LogPara.vpOnlineMoney += InValue;
			LogPara.vpOnlineNumTotal ++;
			LogPara.vpOnlineMoneyTotal += InValue;
			pay_mode = PAYMODE_PC;
		}
	}
	else if((SystemPara.PcEnable == ZHIHUI_PC) || (SystemPara.PcEnable == GPRS_PC))//一鸣智慧 GPRS add by yoc zhihui
	{
		if(payMode == 0) payMode = 1;
		if(payMode == 5) payMode = 2;
		if(payMode == 0x01)//现金
		{
			LogPara.vpCashNum ++;
			LogPara.vpCashMoney += InValue;
			LogPara.vpCashNumTotal ++;
			LogPara.vpCashMoneyTotal += InValue;
			pay_mode = PAYMODE_CASH;
		}
		else if(payMode == 0x02)//一卡通
		{
			LogPara.vpCardNum++;
			LogPara.vpCardMoney += InValue;
			
			LogPara.vpCardNumTotal ++;
			LogPara.vpCardMoneyTotal += InValue;
			LogPara.vpOneCardNumTotal++;
			LogPara.vpOneCardMoneyTotal += InValue;
			pay_mode = PAYMODE_ONECARD;
		}
		else if(payMode == 0x11)//银联卡
		{
			LogPara.vpCardNum++;
			LogPara.vpCardMoney += InValue;

			LogPara.vpCardNumTotal ++;
			LogPara.vpCardMoneyTotal += InValue;
			LogPara.vpUCardNumTotal ++;
			LogPara.vpUCardMoneyTotal += InValue;
			pay_mode = PAYMODE_UNIONPAY;
		}
		else if(payMode == 0x21)//PC 正常出货
		{
			LogPara.vpOnlineNum++;
			LogPara.vpOnlineMoney += InValue;
			LogPara.vpOnlineNumTotal ++;
			LogPara.vpOnlineMoneyTotal += InValue;
			pay_mode = PAYMODE_PC;
		}
		else if(payMode == 0x41)//支付宝
		{
			LogPara.vpOnlineNum++;
			LogPara.vpOnlineMoney += InValue;
			LogPara.vpPC2NumTotal ++;
			LogPara.vpPC2MoneyTotal += InValue;
			pay_mode = PAYMODE_ALIPAY;
		}
		else
		{
			LogPara.vpOnlineNum++;
			LogPara.vpOnlineMoney += InValue;
			LogPara.vpPC3NumTotal++;
			LogPara.vpPC3MoneyTotal += InValue;
			pay_mode = PAYMODE_OTHER;
		}
	}
	else
	{
		if(payMode == 0)//现金购物
		{
			LogPara.vpCashNum ++;
			LogPara.vpCashMoney += InValue;
			LogPara.vpCashNumTotal ++;
			LogPara.vpCashMoneyTotal += InValue;
			pay_mode = PAYMODE_CASH;
		}
		else if(payMode == 0x02)//一卡通
		{
			LogPara.vpCardNum++;
			LogPara.vpCardMoney += InValue;
			LogPara.vpCardNumTotal ++;
			LogPara.vpCardMoneyTotal += InValue;
			pay_mode = PAYMODE_ONECARD;
		}
	}
	//更新总交易次数金额
	LogPara.vpSuccessNum++;
	LogPara.vpSuccessMoney += InValue;
	LogPara.vpSuccessNumTotal++;
	LogPara.vpSuccessMoneyTotal += InValue;

	/******************end add**********************************/
	LogPara.TotalTrans += InValue;
//	LogPara.SuccessNum ++;
	//更新总交易记录
	LogPara.TotalTransTotal += InValue; 
	
	//更新明细交易记录
	if(transMul < 8 )
	{
		
		
		if((ChannelNum[0]=='A')||(ChannelNum[0]=='B'))//开启副柜
		{
			LogParaDetail.ColumnNo[transMul*3] = ChannelNum[0];
			LogParaDetail.ColumnNo[transMul*3+1] = ChannelNum[1];
			LogParaDetail.ColumnNo[transMul*3+2] = ChannelNum[2];
			logicNo = (ChannelNum[1] - '0') * 10 + (ChannelNum[2] - '0');
		}
		else
		{
			LogParaDetail.ColumnNo[transMul*3] = 'A';
			LogParaDetail.ColumnNo[transMul*3+1] = ChannelNum[0];
			LogParaDetail.ColumnNo[transMul*3+2] = ChannelNum[1];	
			logicNo = (ChannelNum[0] - '0') * 10 + (ChannelNum[1] - '0');
		}
		
		//changed by yoc 2013.12.03
		LogParaDetail.GoodsNo[transMul] = (unsigned char)(getAisleInfo(logicNo,AISLE_ID) & 0xFF);
		LogParaDetail.PriceNo[transMul] = InValue;
		LogParaDetail.TransSucc[transMul] = 1;

		//add by yoc 2013.9.11
		LogParaDetail.PayMode[transMul] = pay_mode;
		LogParaDetail.TradeNum = transMul + 1;
		LogParaDetail.GoodsMin[transMul] = RTCData.minute;
		LogParaDetail.GoodsSec[transMul] = RTCData.second;
		LogParaDetail.run_no[transMul] = LogPara.run_no;
		
	}

	print_log("logTransaction tradeNum = %d\r\n",transMul);
	
	LogPara.run_no++;
	
}


/*********************************************************************************************************
** Function name:       LogCostAPI
** Descriptions:        PC扣款时更新日志
** input parameters:    InValue扣款的金额
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogCostAPI(uint32_t InValue)
{
	//更新PC扣款区域金额		
	LogPara.vpCashMoney += InValue;			
	LogPara.vpCashMoneyTotal += InValue;
		
	//更新PC扣款总交易次数金额
	LogPara.vpSuccessMoney += InValue;
	LogPara.vpSuccessMoneyTotal += InValue;

	/******************end add**********************************/
	//更新区域交易记录
	LogPara.TotalTrans += InValue;
	//更新总交易记录
	LogPara.TotalTransTotal += InValue;	
}


/*********************************************************************************************************
** Function name:       LogTransactionAPI
** Descriptions:        卡消费金额更新日志
** input parameters:    InValue出货的金额
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogCardIncomeAPI(uint32_t InValue)
{
	
	//changed by yoc 2013.9.11
	LogPara.vpCardNum ++ ;
	LogPara.vpCardMoney +=InValue;//old code
	LogPara.vpCardNumTotal ++;
	LogPara.vpCardMoneyTotal += InValue;
}



/*********************************************************************************************************
** Function name:       LogChangeAPI
** Descriptions:        找零时更新日志
** input parameters:    InValue找零的金额
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogChangeAPI(uint32_t InValue,uint32_t debtMoney)
{
	
	//更新区域交易记录
	LogPara.TotalChange += InValue;
	LogPara.Iou += debtMoney;

	//更新总交易记录	
	LogPara.TotalChangeTotal += InValue;

	//更新明细交易记录
	LogParaDetail.Change = InValue;	
}


/*********************************************************************************************************
** Function name:       LogEndTransAPI
** Descriptions:        结束交易时更新日志记录
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogEndTransAPI(void)
{	
	//更新明细交易记录	
	LogParaDetail.EndMinute = RTCData.minute;
	//详细交易记录写入FLASH是1-TRADE_PAGE_MAX,页号0保留。LogPara.LogDetailPage表示目前已经写入的flash页号
	if(LogPara.LogDetailPage < TRADE_PAGE_MAX)
		LogPara.LogDetailPage++;//更新明细交易记录页索引	
	else
		LogPara.LogDetailPage = 1;
	WriteLogDetailAPI(LogPara.LogDetailPage);//保存明细交易记录到flash中		
	//更新区域交易记录,总交易记录到flash中
	WriteLogAPI();
	Trace("\r\n page=%d",LogPara.LogDetailPage);
	OSTimeDly(OS_TICKS_PER_SEC/100);	
}



/*********************************************************************************************************
** Function name:       LogClearAPI
** Descriptions:        删除区域日志记录
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LogClearAPI(void)
{

	vmc_batch_info_write();	//add by yoc 2013.11.13
	LogPara.Income = 0;
	LogPara.NoteIncome = 0;
	LogPara.CoinsIncome = 0;
	LogPara.TotalTrans = 0;
	LogPara.TotalChange = 0;
	LogPara.CoinsChange[0] = 0;
	LogPara.CoinsChange[1] = 0;
	LogPara.CoinsChange[2] = 0;
	LogPara.SuccessNum = 0;
	LogPara.DoubtNum = 0;
	LogPara.Iou = 0;
//	LogPara.CardIncome = 0;
	
	//LogPara.LogDetailPage = 0;
	//更新区域交易记录,总交易记录到flash中
	LogPara.vpSuccessNum = 0;
	LogPara.vpSuccessMoney = 0;
	LogPara.vpCashNum = 0;
	LogPara.vpCashMoney = 0;
	LogPara.vpGameNum = 0;
	LogPara.vpGameMoney = 0;
	LogPara.vpCardNum = 0;
	LogPara.vpCardMoney = 0;
	LogPara.vpOnlineNum = 0;
	LogPara.vpOnlineMoney = 0;
	
	WriteLogAPI();
	
}


/*********************************************************************************************************
** Function name:       WriteLogAPI
** Descriptions:        将区域交易记录,总交易记录写到flash中
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void WriteLogAPI(void)
{
	uint8_t *tp2,buf[512];
	uint16_t crc,len;

	tp2 = (uint8_t *) &LogPara;//交易数据记录	
	/*
	crc = CrcCheck(tp2,sizeof(LogPara)-2);
	LogPara.CrcCheck[0] = crc/256;
	LogPara.CrcCheck[1] = crc%256; */
	//Trace("\r\n logcrc=%d,%x,%x",sizeof(LogPara)-2,LogPara.CrcCheck[0],LogPara.CrcCheck[1]);
	//changed by yoc 2013.9.11
	if(sizeof(LogPara) > 500)
	{
		Trace("sizeof(LogPara) is %d  > 500\r\n",sizeof(LogPara));
		return ;
	}
	len = sizeof(LogPara);
	memcpy(buf,tp2,len);
	crc = CrcCheck(buf,len);
	buf[len] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	buf[len + 2] = 0xE0;//区域交易记录写入flash标志
	AT45DBWritePage(4040,buf);
	//Trace("LogPara write ...\r\n");
}

/*********************************************************************************************************
** Function name:       ReadLogAPI
** Descriptions:        从flash读区域交易记录,总交易记录
** input parameters:    
** output parameters:   无
** Returned value:      1读取成功,0读取失败
*********************************************************************************************************/
uint8_t ReadLogAPI(void)
{
	uint8_t *tp2,buf[512];
	uint16_t crc,len;
	tp2 = (uint8_t *) &LogPara;//交易数据记录	

	AT45DBReadPage(4040,buf);

	len = sizeof(LogPara);
	if(len > 500) return 0;
	crc = CrcCheck(buf,len);
	//Trace("LogPara:\r\n crc =%d,buf[crc]=%d type = %d\r\n",crc,INTEG16(buf[len],buf[len + 1]),buf[len + 2]);
	if((crc == INTEG16(buf[len],buf[len + 1])) && (buf[len + 2] == 0xE0))
	{
		Trace("\r\nLogPara read from flash suc....\r\n");
		memcpy(tp2,buf,len);
		return 1;
	}
	//Trace("LogPara read from flash fail....\r\n");
	return 0;
/*
	crc = CrcCheck(tp2,sizeof(LogPara)-2);
	//Trace("\r\n logcrc=%d,%x,%x",sizeof(LogPara)-2,crc/256,crc%256);
	if( (crc/256 == LogPara.CrcCheck[0])&&(crc%256 == LogPara.CrcCheck[1]) )
	{
		return 1;
	}
	else
	{
		return 0;
	} */
}



/*********************************************************************************************************
** Function name:       WriteLogDetailAPI
** Descriptions:        将明细日志写到flash中
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void WriteLogDetailAPI(uint16_t	LogDetailPage)
{
	uint8_t *tp1,i,buf[512];
	uint16_t crc,len;

	tp1 = (uint8_t *) &LogParaDetail;//交易数据记录	
	
	if(sizeof(LogParaDetail) > 500)
	{
		Trace("sizeof(LogPara) is %d  > 500\r\n",sizeof(LogPara));
		return ;
	}
	len = sizeof(LogParaDetail);
	memcpy(buf,tp1,len);
	crc = CrcCheck(buf,len);
	buf[len] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	buf[len + 2] = 0xE1;//总交易记录写入flash 标志
	AT45DBWritePage(LogDetailPage,buf);
	Trace("TradeNum=%d\r\n",LogParaDetail.TradeNum);
	for(i = 0;i < 24 ; )
	{
		Trace("ColumnNo=%c%c%c\r\n ",LogParaDetail.ColumnNo[i],LogParaDetail.ColumnNo[i+1],LogParaDetail.ColumnNo[i+2]);
		i+=3;
		OSTimeDly(20);
	}
	//PrintLogDetailAPI();
	OSTimeDly(OS_TICKS_PER_SEC/100);
	
	memset(&LogParaDetail,0,sizeof(LogParaDetail));
}

/*********************************************************************************************************
** Function name:       WriteLogDetailAPI
** Descriptions:        从flash读每条明细日志
** input parameters:    
** output parameters:   无
** Returned value:      1读取成功,0读取失败
*********************************************************************************************************/
uint8_t ReadLogDetailAPI(uint16_t	LogDetailPage)
{
	uint8_t *tp1,buf[512];
	uint16_t crc,len;
	
	tp1 = (uint8_t *) &LogParaDetail;//交易数据记录		
	AT45DBReadPage(LogDetailPage,buf);

	len = sizeof(LogParaDetail);
	if(len > 500) return 0;
	crc = CrcCheck(buf,len);
	//Trace("LogDetailPage:\r\n crc =%d,buf[crc]=%d,typr = %d\r\n",crc,INTEG16(buf[len],buf[len + 1]),buf[len + 2]);
	if((buf[len + 2] == 0xE1) && (crc == INTEG16(buf[len],buf[len + 1])))
	{
		//Trace("LogDetailPage read from flash suc....\r\n");
		memcpy(tp1,buf,len);
		return 1;
	}
	//Trace("LogDetailPage read from flash fail...\r\n");
	return 0;
/*
	crc = CrcCheck(tp1,sizeof(LogParaDetail)-2);
	//Trace("\r\n detaillogcrc=%d,%x,%x",sizeof(LogParaDetail)-2,crc/256,crc%256);
	if( (LogParaDetail.CrcCheck[0]==0)&&(LogParaDetail.CrcCheck[1]==0) )
	{
		return 0;
	}
	else
	{
		if( (crc/256 == LogParaDetail.CrcCheck[0])&&(crc%256 == LogParaDetail.CrcCheck[1]) )
		{
			return 1;
		}
		else
		{
			return 0;
		} 
	}
	*/
}


/*********************************************************************************************************
** Function name:       ReadDetailTraceAPI
** Descriptions:        从flash读每条明细日志 不用全局变量
** input parameters:    
** output parameters:   无
** Returned value:      1读取成功,0读取失败
*********************************************************************************************************/
uint8_t ReadDetailTraceAPI(char * detailBuff,uint16_t	LogDetailPage)
{
	uint8_t buf[512];
	uint16_t crc,len;
		
	AT45DBReadPage(LogDetailPage,buf);

	len = sizeof(LogParaDetail);
	if(len > 500) return 0;
	crc = CrcCheck(buf,len);
	if((buf[len + 2] == 0xE1) && (crc == INTEG16(buf[len],buf[len + 1])))
	{
		//Trace("LogDetailPage read from flash suc....\r\n");
		if(detailBuff != NULL)
			memcpy(detailBuff,buf,len);
		return 1;
	}
	return 0;
}


