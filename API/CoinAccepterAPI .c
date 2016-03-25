/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           COINACCEPTER
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        硬币器中间程序相关函数声明                     
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
** Function name:       CoinDevInit
** Descriptions:        硬币器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CoinDevInitAPI()
{
	switch(SystemPara.CoinAcceptorType)
	{
		case PARALLEL_COINACCEPTER:
			TraceCoin("\r\nMiddbingx\r\n");
			InitParallelPluseCoinAcceptor();	
			break;
		case MDB_COINACCEPTER:
			//1.启动硬币器
			MsgAccepterPack.CoinBack = MBOX_COININITDEV;			
			OSMboxPost(g_CoinMoneyMail,&MsgAccepterPack);
			break;
		case SERIAL_COINACCEPTER:
			TraceCoin("\r\nMiddchuangx\r\n");
			InitSerialPluseCoinAcdeptor();
			break;
		default:break;	
	}
	return;
}

/*********************************************************************************************************
** Function name:       GetCoinDevMoneyInAPI
** Descriptions:        获得硬币器设备的收币状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetCoinDevMoneyInAPI(void)
{
	unsigned char ComStatus;
	MessagePack *CoinMsg;
	uint32_t money,ReturnBack;

	switch(SystemPara.CoinAcceptorType)
	{
		case PARALLEL_COINACCEPTER:			
			ComStatus = ReadParallelCoinAcceptor();	
			//TraceCoin("\r\nMiddbingx=%d\r\n",ComStatus);
			if(ComStatus != 0)
			{
				money = stDevValue.CoinValue[ComStatus - 1];
				TraceCoin("\r\nMiddvalue=%ld\r\n",money);
				ReturnBack = money;
			}
			else
				ReturnBack = 0;
			break;
		case MDB_COINACCEPTER:
			CoinMsg = OSQPend(g_CoinMoneyQ,5,&ComStatus);
			//TraceCoin("\r\nMiddmdb=%d\r\n",ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				money = CoinMsg->CoinMoney;				
				TraceCoin("\r\nMiddvalue=%ld",money);
				ReturnBack = money;
			}
			else
				ReturnBack = 0;	
			break;
		case SERIAL_COINACCEPTER:					
			ComStatus = ReadSerialPluseCoinAcceptor();	
			//TraceCoin("\r\nMiddchuangx=%d\r\n",ComStatus);
			if(ComStatus != 0)
			{
				money = stDevValue.CoinValue[ComStatus - 1];
				TraceCoin("\r\nMiddvalue=%ld\r\n",money);
				ReturnBack = money;
			}
			else
				ReturnBack = 0;
			break;
		default:break;	
			
	}
	return ReturnBack;
	
}




/*********************************************************************************************************
** Function name:       CoinDevDisable
** Descriptions:        禁能硬币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CoinDevDisableAPI(void)
{
	switch(SystemPara.CoinAcceptorType)
	{		
		case PARALLEL_COINACCEPTER:
			DisableParallelPluseCoinAcceptor();
			break;
		case MDB_COINACCEPTER:			
			MsgAccepterPack.CoinBack = MBOX_COINDISABLEDEV;			
			OSMboxPost(g_CoinMoneyMail,&MsgAccepterPack);
			break;	
		case SERIAL_COINACCEPTER:	
			DisableSerialPluseCoinAcceptor();
			break;
		default:break;	
	}		
	return;	
}






/*********************************************************************************************************
** Function name:       CoinDevEnable
** Descriptions:        使能硬币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CoinDevEnableAPI(void)
{
	switch(SystemPara.CoinAcceptorType)
	{		
		case PARALLEL_COINACCEPTER:
			EnableParallelPluseCoinAcceptor();
			break;
		case MDB_COINACCEPTER:			
			MsgAccepterPack.CoinBack = MBOX_COINENABLEDEV;			
			OSMboxPost(g_CoinMoneyMail,&MsgAccepterPack);
			break;	
		case SERIAL_COINACCEPTER:
			EnableSerialPluseCoinAcceptor();
			break;
		default:break;	
	}
	return;
}


/*********************************************************************************************************
** Function name:       GetCoinDevState
** Descriptions:        获得硬币器设备的当前状态
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void GetCoinDevStateAPI()
{
	return;	
}




