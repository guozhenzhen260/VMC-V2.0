/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        主程序文件                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "BUSINESS.H"
#include "MAINTAIN.H"
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
SYSTEMPARAMETER SystemPara;
SYSUSERPARAMETER UserPara;
RTC_DATE RTCData;
RTC_DATE vmRTC;//更新屏幕的时间结构体
struct STDevValue stDevValue;
TIMER Timer;
LOGPARAMETER LogPara;
LOGPARAMETERDETAIL LogParaDetail;
SELECTKEYAPI selectKey;
GLOBALSYS globalSys;

//static OS_STK MainTaskStk[2048];
//static OS_STK Uart2DeviceTaskStk[512];
//static OS_STK Uart3DeviceTaskStk[1024];
//static OS_STK Uart1DeviceTaskStk[256];


/*********************************************************************************************************
** Function name:     	DispVersionText
** Descriptions:	    显示版本号，等待设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispVersionText()
{	
	LCDPutLOGOBmp(24,LINE2,SystemPara.Logo);
	LCDNumberFontPrintf(40,LINE15,2,"version%d.%02d",2,17);	
}


/*********************************************************************************************************
** Function name:       mainTask
** Descriptions:        主任务
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MainTask(void *pvData)
{
	//uint32_t *InValue;
	uint8_t 	err;
	
	//unsigned char Id[4];
	pvData = pvData;
	WDT_Init(2*60);//开启看门狗，设置120s超时
	InitKeyboard();//初始化拨码(不在这里做的话，硬币器拨码就需要拨了，不然机器蓝屏)
	InitUart0();
	//InitUart1();
	InitUart2();
	InitUart3();
      InitBuzzer();
      LCDInit();
	InitTimer(0,240000);
	InitRtc();
	//RTCStartOrStop(1);	
	InitAT45DB();
	InitKeyboard();	
	//Trace("sizeof(SystemPara) = %d\r\n",(sizeof(SystemPara)));
	//Trace("System Init Ok...\r\n");
	//Trace("AT45DBReadStatus = %02X \r\n",AT45DBReadStatus());
	//AT45DBReadID(&Id[0]);
	//Trace("AT45DBReadID = 0x%02X 0x%02X 0x%02X 0x%02X \r\n",Id[0],Id[1],Id[2],Id[3]);
	LoadDefaultSystemPara(0);
	LoadUserSystemPara(0);
	DispVersionText();//显示版本号，等待设备初始化
	InitUart1();	
	CreateMBox();
	OSTaskCreate(Uart2TaskDevice,(void *)0,&Uart2DeviceTaskStk[sizeof(Uart2DeviceTaskStk)/4 - 1],6);	
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	pcControlInit();//by yoc newAisle pc管理模式入口
	CoinDevInitAPI();
	OSTimeDly(OS_TICKS_PER_SEC / 100);	
	BillDevInitAPI();
	OSTimeDly(OS_TICKS_PER_SEC / 100);	
	ChangerDevInitAPI();
	ReaderDevInitAPI();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//ChannelResetinit();
	//从FLASH读出货道参数
	ChannelGetFlashData();	
	OSSemPend(g_DeviceSem,0,&err);	//一直等待，直到设备初始化完成
	ResetSystemPara();
	//Trace("\r\n 1.de=%ld,%ld,%ld",LogParaDetail.IncomeBill,LogParaDetail.IncomeCoin,LogParaDetail.IncomeCard);
	//Trace("\r\n 1.log=%ld,%ld",LogPara.Income,LogPara.NoteIncome);
	ReadLogAPI();//读取交易日志
	memset(&LogParaDetail,0,sizeof(LogParaDetail));
	//PrintLogDetailAPI();
	OSTimeDly(OS_TICKS_PER_SEC/100);
	//展示位通讯串口
	OSTaskCreate(Uart1TaskDevice,(void *)0,&Uart1DeviceTaskStk[sizeof(Uart1DeviceTaskStk)/4 - 1],9);
	OSTimeDly(OS_TICKS_PER_SEC/100);
	SelectInitAPI();
	OSTimeDly(OS_TICKS_PER_SEC/100);
	//pc通讯串口
	OSTaskCreate(Uart3TaskDevice,(void *)0,&Uart3DeviceTaskStk[sizeof(Uart3DeviceTaskStk)/4 - 1],8);
	OSTimeDly(OS_TICKS_PER_SEC/100);
	PCInitAPI();
	OSTimeDly(OS_TICKS_PER_SEC);

	memset(&taskDevSignal,0,sizeof(taskDevSignal));
	
	//err = ChannelGetGocState(1);
	//Trace("GOCState=%d\r\n",err);
	//Trace("\r\n 2.de=%ld,%ld,%ld",LogParaDetail.IncomeBill,LogParaDetail.IncomeCoin,LogParaDetail.IncomeCard);
	//Trace("\r\n 2.log=%ld,%ld",LogPara.Income,LogPara.NoteIncome);
	while(1)
	{
		//CurMaintainKey = ReadMaintainKeyValue();
		//if(ReadMaintainKeyValue())
		//	MaintainUserProcess((void*)0);
		//else
		BusinessProcess((void*)0);		
	}
}
/*********************************************************************************************************
** Function name:       main
** Descriptions:        用户程序入口函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int main(void)
{
    targetInit();																
    pinInit();      															                                                                    
    OSInit();                                                                                                       
	OSTaskCreateExt(MainTask,(void *)0,&MainTaskStk[sizeof(MainTaskStk)/4 - 1],7,7,&MainTaskStk[0],256,(void *)0,OS_TASK_OPT_STK_CHK);
    OSStart();
	return 0;
}
/**************************************End Of File*******************************************************/
