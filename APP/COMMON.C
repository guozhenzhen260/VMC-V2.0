/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "CHANNEL.h"


/*********************************************************************************************************
** 通讯机制
*********************************************************************************************************/
TASKDEVICESIGNAL taskDevSignal,temptaskDevSignal;

/*********************************************************************************************************
** Function name:     	updateTaskDevSignal
** Descriptions:	    更新看门狗信号量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void updateTaskDevSignal(uint8 dev)
{
	switch(dev)
	{
		case UART1TASKSIG:
			taskDevSignal.Uart1TaskDevice++;
			break;
		case UART2TASKSIG:
			taskDevSignal.Uart2TaskDevice++;
			break;
		case UART3TASKSIG:
			taskDevSignal.Uart3TaskDevice++;
			break;	
	}
}

/*********************************************************************************************************
** Function name:     	checkTaskDevSignal
** Descriptions:	    检测看门狗信号量，如果出异常，复位系统
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void checkTaskDevSignal()
{
	static uint8_t dogUart1=0,dogUart2=0,dogUart3=0;
	if(Timer.WatchDogTimer==0)
	{
		Timer.WatchDogTimer = 10;
		//Trace("\r\ntaskDevSignal=%ld,%ld,%ld",temptaskDevSignal.Uart1TaskDevice,temptaskDevSignal.Uart2TaskDevice,temptaskDevSignal.Uart3TaskDevice);
		if(temptaskDevSignal.Uart1TaskDevice!=taskDevSignal.Uart1TaskDevice)
		{
			temptaskDevSignal.Uart1TaskDevice=taskDevSignal.Uart1TaskDevice;
			dogUart1=0;
			//Trace("\r\nUart1TaskDiff=%ld,dog=%d",taskDevSignal.Uart1TaskDevice,dogUart1);			
		}
		else
		{
			dogUart1++;
			//Trace("\r\nUart1TaskSame=%ld,dog=%d",taskDevSignal.Uart1TaskDevice,dogUart1);
		}
		if(temptaskDevSignal.Uart2TaskDevice!=taskDevSignal.Uart2TaskDevice)
		{
			temptaskDevSignal.Uart2TaskDevice=taskDevSignal.Uart2TaskDevice;
			//
			dogUart2=0;
			//Trace("\r\nUart2TaskDiff=%ld,dog=%d",taskDevSignal.Uart2TaskDevice,dogUart2);
		}
		else
		{
			//
			dogUart2++;
			//Trace("\r\nUart2TaskSame=%ld,dog=%d",taskDevSignal.Uart2TaskDevice,dogUart2);
		}
		if(temptaskDevSignal.Uart3TaskDevice!=taskDevSignal.Uart3TaskDevice)
		{
			temptaskDevSignal.Uart3TaskDevice=taskDevSignal.Uart3TaskDevice;
			//
			dogUart3=0;
			//Trace("\r\nUart3TaskDiff=%ld,dog=%d",taskDevSignal.Uart3TaskDevice,dogUart3);
		}
		else
		{
			//
			dogUart3++;
			//Trace("\r\nUart3TaskSame=%ld,dog=%d",taskDevSignal.Uart3TaskDevice,dogUart3);
		}

		//复位
		if((dogUart1>5)||(dogUart2>5)||(dogUart3>5))
		{
			if(dogUart1>5)
			{
				Trace("\r\nUart1TaskReset");
			}
			if(dogUart2>5)
			{
				Trace("\r\nUart2TaskReset");
			}
			if(dogUart3>5)
			{
				Trace("\r\nUart3TaskReset");
			}
			MdbBusHardwareReset();
		      OSTimeDly(OS_TICKS_PER_SEC*2);
			zyReset(ZY_HARD_RESET);
		}
	}	
}



//等待设备初始化完成信号量
OS_EVENT	*g_DeviceSem;
//纸币器通信队列
#define BILL_MONEY_SIZE 3
void *BillMoneyQ[BILL_MONEY_SIZE];
OS_EVENT *g_BillMoneyQ;//纸币接收队列
OS_EVENT *g_BillMoneyMail;//纸币控制邮箱
OS_EVENT *g_BillMoneyBackMail;//纸币返回给vmc控制结果的邮箱
//硬币器通信队列
#define COIN_MONEY_SIZE 40
void *CoinMoneyQ[COIN_MONEY_SIZE];
OS_EVENT *g_CoinMoneyQ;//硬币接收队列
OS_EVENT *g_CoinMoneyMail;//硬币控制邮箱
OS_EVENT *g_CoinMoneyBackMail;//硬币返回给vmc控制结果的邮箱
//找零器通信邮箱
OS_EVENT *g_ChangeMoneyMail;//找零命令邮箱
OS_EVENT *g_ChangeBackMoneyMail;//找零返回邮箱
//读卡器通信邮箱
OS_EVENT *g_ReaderMoneyMail;//读卡器上传给vmc插卡金额命令邮箱
OS_EVENT *g_ReaderBackMoneyMail;//vmc下发给读卡器控制邮箱
OS_EVENT *g_ReaderBackCmdMail;//读卡器上传给vmc命令邮箱
//检测各个设备状态的邮箱
OS_EVENT *g_DeviceStateMail;//查询各个设备状态
OS_EVENT *g_DeviceStateBackMail;//返回给vmc各个设备状态的邮箱
//货道操作专用邮箱
OS_EVENT *g_ChannelMail;//货道操作邮箱
OS_EVENT *g_ChannelBackMail;//货道应答邮箱

//升降台操作邮箱
OS_EVENT *g_LiftTableMail;
OS_EVENT *g_LiftTableBackMail;

//ACDC控制邮箱
OS_EVENT *g_ACDCMail;
OS_EVENT *g_ACDCBackMail;

//盒饭柜专用邮箱  add by liya 2014-01-20
OS_EVENT *g_HeFanGuiMail;
OS_EVENT *g_HeFanGuiBackMail;

MessagePack MsgAccepterPack;
DEVICESTATEPACK DeviceStatePack;
DEVICESTATEPACK DeviceStateBusiness;
CARDSTATEPACK CardStatePack;

//一鸣智慧通信邮箱 add by yoc 2013.12.16 
OS_EVENT *g_PCMail;//PC控制邮箱
OS_EVENT *g_PCBackMail;//PC返回给vmc控制结果的邮箱
unsigned char ZhIndex = 0;
void *ZhSizeQ[MBOX_ST_LEN];
#pragma arm section zidata = "RAM2" //add by yoc 2013.12.16
MESSAGE_ZHIHUI vmc_zh_mbox[MBOX_ST_LEN];
MESSAGE_ZHIHUI zh_vmc_mbox;
#pragma arm section zidata


//UBoxPC通讯队列
void *UboxSizeQ[UBOX_SIZE];
OS_EVENT *g_Ubox_VMCTOPCQ;//VMC下发给PC的消息队列

OS_EVENT *g_Ubox_VMCTOPCBackCMail;

//PCTOVMC通讯队列
void *PCTOVMCSizeQ[UBOX_SIZE];
OS_EVENT *g_Ubox_PCTOVMCQ;//PC给VMC的控制命令邮箱

//PCTOVMCBack通讯队列
void *PCTOVMCBackSizeQ[UBOX_SIZE];
OS_EVENT *g_Ubox_PCTOVMCBackQ;


uint8_t  g_Ubox_Index = 0;
MessageUboxPCPack MsgUboxPack[UBOX_SIZE];

//UboxSIMPLEPC通讯队列
void *SIMPLEUUboxSizeQ[SIMPLEUBOX_SIZE];
OS_EVENT *g_SIMPLEUbox_PCTOVMCQ;//pc给vmc的消息队列
OS_EVENT *g_SIMPLEUbox_PCTOVMCBackCMail;//pc给vmc的消息如出货命令等，需要返回给pc操作结果
void *SIMPLEUUboxVMCTOPCQ[SIMPLEUBOX_SIZE];
OS_EVENT *g_SIMPLEUbox_VMCTOPCQ;//vmc下发给PC的消息队列
OS_EVENT *g_SIMPLEUbox_VMCTOPCBackCMail;//vmc给pc发送admin补货等操作后，需要返回给vmc操作结果
uint8_t  g_SIMPLEUbox_Index = 0;
MessageSIMPLEUboxPCPack MsgSIMPLEUboxPack[SIMPLEUBOX_SIZE];



//展示位按键
OS_EVENT *g_KEYMail;//展示位控制邮箱
MessageKEYPack MsgKEYPack;

OS_EVENT *g_KEYBackMail;//展示位返回给vmc控制结果的邮箱
MessageKEYPackBack MsgKEYBackPack;

//温控器通讯队列
OS_EVENT *g_XMTMail;
OS_EVENT *g_XMTBackMail;

MessageXMTPack MsgXMTPack;

//纸币找零器
OS_EVENT *g_FSBillRecyclerMail;//纸币控制邮箱
OS_EVENT *g_FSBillRecyclerBackMail;//纸币返回给vmc控制结果的邮箱
MessageFSBillRecyclerPack MsgFSBillRecyclerPack;

//**********************************




//extern SYSTEMPARAMETER SystemPara;

const unsigned char MAINTAINPASSWORD[6]  = {'1','2','3','4','5','6'};
const unsigned char ADMINPASSWORD[8] 	 = {'8','3','7','1','8','5','5','7'};
const unsigned char MECHINEDEVICETYPE[6] = {'E','V','7','6','3','6'};




/*********************************************************************************************************
** Function name:       CreateBox
** Descriptions:        为任务之间通信创建邮箱,信号量,消息队列
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CreateMBox(void)
{
	//等待设备初始化完成信号量
	g_DeviceSem = OSSemCreate(0);
	//纸币器通信队列	
	g_BillMoneyQ = OSQCreate(&BillMoneyQ[0],BILL_MONEY_SIZE);
	g_BillMoneyMail = OSMboxCreate(NULL);
	g_BillMoneyBackMail = OSMboxCreate(NULL);
	//硬币器通信队列	
	g_CoinMoneyQ = OSQCreate(&CoinMoneyQ[0],COIN_MONEY_SIZE);
	g_CoinMoneyMail = OSMboxCreate(NULL);
	g_CoinMoneyBackMail = OSMboxCreate(NULL);
	//找零器通信邮箱
	g_ChangeMoneyMail = OSMboxCreate(NULL);
	g_ChangeBackMoneyMail = OSMboxCreate(NULL);
	//读卡器通信邮箱
	g_ReaderMoneyMail = OSMboxCreate(NULL);
	g_ReaderBackMoneyMail = OSMboxCreate(NULL);
	g_ReaderBackCmdMail = OSMboxCreate(NULL);
	//检测各个设备状态的邮箱
	g_DeviceStateMail = OSMboxCreate(NULL);
	g_DeviceStateBackMail = OSMboxCreate(NULL);
	//货道操作专用邮箱
	g_ChannelMail = OSMboxCreate(NULL);//货道操作邮箱
	g_ChannelBackMail = OSMboxCreate(NULL);//货道应答邮箱

	//ACDC专用邮箱
	g_ACDCMail = OSMboxCreate(NULL);//压缩机展示灯邮箱
	g_ACDCBackMail = OSMboxCreate(NULL);//压缩机展示灯应答邮箱

	   	//盒饭柜专用邮箱  add by liya 2014-01-20
	g_HeFanGuiMail = OSMboxCreate(NULL);//压缩机展示灯邮箱
	g_HeFanGuiBackMail = OSMboxCreate(NULL);//压缩机展示灯应答邮箱

	//PC通讯
	g_PCMail = OSQCreate(&ZhSizeQ[0],MBOX_ST_LEN);
	g_PCBackMail = OSMboxCreate(NULL);
	//UBoxPC通讯
	g_Ubox_VMCTOPCQ = OSQCreate(&UboxSizeQ[0],UBOX_SIZE);
	g_Ubox_VMCTOPCBackCMail = OSMboxCreate(NULL);
	g_Ubox_PCTOVMCQ = OSQCreate(&PCTOVMCSizeQ[0],UBOX_SIZE);
	g_Ubox_PCTOVMCBackQ = OSQCreate(&PCTOVMCBackSizeQ[0],UBOX_SIZE);
	//UboxSIMPLEPC通讯队列
	g_SIMPLEUbox_PCTOVMCQ = OSQCreate(&SIMPLEUUboxVMCTOPCQ[0],SIMPLEUBOX_SIZE);
	g_SIMPLEUbox_PCTOVMCBackCMail = OSMboxCreate(NULL);
	g_SIMPLEUbox_VMCTOPCQ = OSQCreate(&SIMPLEUUboxSizeQ[0],SIMPLEUBOX_SIZE);
	g_SIMPLEUbox_VMCTOPCBackCMail = OSMboxCreate(NULL);
	//创建升降台邮箱
	g_LiftTableMail = OSMboxCreate(NULL);
	g_LiftTableBackMail = OSMboxCreate(NULL);

	//展示位按键邮箱
	g_KEYMail = OSMboxCreate(NULL);
	g_KEYBackMail = OSMboxCreate(NULL);

	//温控器邮箱
	g_XMTMail = OSMboxCreate(NULL);
	g_XMTBackMail = OSMboxCreate(NULL);
	//纸币找零器
	g_FSBillRecyclerMail = OSMboxCreate(NULL);
	g_FSBillRecyclerBackMail = OSMboxCreate(NULL);
	
}


/*********************************************************************************************************
** Function name:     	LoadMechineSerialNo
** Descriptions:	    写入机器序列号
** input parameters:    Data:10个字节的序列号
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void LoadMechineSerialNo(unsigned char *Data)
{
	unsigned char i;
	unsigned char *Temp;
	unsigned short Crc;
	for(i=0;i<10;i++)
	{
		SystemPara.MechineSerialNo[i] = *Data;
		Data++;
	}
	Temp = (unsigned char *)&SystemPara;
	Crc = CrcCheck(Temp,510);
	SystemPara.CrcCheck[0] = (unsigned char)(Crc>>8);				//CRC校验
	SystemPara.CrcCheck[1] = (unsigned char)Crc;					//CRC校验
	//WriteSystemParameter(&SystemPara);
}
/*********************************************************************************************************
** Function name:     	FactoryDefaultSystemPara
** Descriptions:	    出厂默认配置工程系统参数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void FactoryDefaultSystemPara(void)
{
	unsigned int i;

	//1.读取系统镜像工程参数数据
	LoadDefaultSystemPara(1);
	//2.读取系统镜像用户参数数据
	LoadUserSystemPara(1);
	
	//2.读取镜像货道数据
	hd_read_param_bak(1);
	hd_read_param_bak(2);
	
	SystemPara.SoftwareVersion = 0x14;								//软件版本V2.0
	SystemPara.HardwareVersion = 0x19;								//硬件版本V2.5
	for(i=0;i<6;i++)
	{	
		SystemPara.MaintainPassword[i] = MAINTAINPASSWORD[i];		//维护密码123456
	}
	for(i=0;i<8;i++)
	{
		SystemPara.AdminPassword[i] = ADMINPASSWORD[i];				//管理员密码987654
	}
	SystemPara.Language = 0x00;										//语言选择
	for(i=0;i<6;i++)
	{
		SystemPara.MechineDeviceType[i] = MECHINEDEVICETYPE[i];		//设备型号,如EV7636
	}
	

	//3.保存系统工程参数
	WriteDefaultSystemPara(SystemPara,0);
	//4.保存用户参数
	WriteUserSystemPara(UserPara,0);
	//5.保存货道数据
	hd_save_param(1);
	hd_save_param(2);
	
	//3.清交易记录
	memset((void *)&LogPara,0,sizeof(LogPara));
	//memset((void *)&LogParaDetail,0,sizeof(LogParaDetail));
	//WriteLogAPI();
//	vmc_batch_info_write();
	//ChannelClearSuccesCount();
}

/*********************************************************************************************************
** Function name:     	RstFactoryDefaultSystemPara
** Descriptions:	    出厂默认保存镜像参数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void RstFactoryDefaultSystemPara(void)
{
	unsigned int i;
	
	SystemPara.SoftwareVersion = 0x14;								//软件版本V2.0
	SystemPara.HardwareVersion = 0x19;								//硬件版本V2.5
	for(i=0;i<6;i++)
	{	
		SystemPara.MaintainPassword[i] = MAINTAINPASSWORD[i];		//维护密码123456
	}
	for(i=0;i<8;i++)
	{
		SystemPara.AdminPassword[i] = ADMINPASSWORD[i];				//管理员密码987654
	}
	
	//1.保存系统参数
	WriteDefaultSystemPara(SystemPara,1);
	//2.保存用户参数
	WriteUserSystemPara(UserPara,1);
	// 2 货道
	hd_save_param_bak(1);
	hd_save_param_bak(2);
	
	
}


/*********************************************************************************************************
** Function name:     	LoadDefaultSystemPara
** Descriptions:	    读取工程系统参数设置
** input parameters:    backup=0,在主页面操作,=1在镜像页面操作
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t LoadDefaultSystemPara(uint8_t backup)
{
	//uint8_t *tp2;
	uint16_t crc;
	uint8_t RdBuf[512]={0};
	uint16_t num=0;
	/*
	tp2 = (uint8_t *) &SystemPara;//交易数据记录		
	AT45DBReadPage(4090,tp2);
	

	crc = CrcCheck(tp2,124);
	Trace("\r\n defaultsys=%d,%x,%x",124,crc/256,crc%256);
	if( (crc/256 == SystemPara.CrcCheck[0])&&(crc%256 == SystemPara.CrcCheck[1]) )
	{
		return 1;
	}
	else
	{
		return 0;
	}*/
	if(backup==0)
		AT45DBReadPage(4090,RdBuf);
	else if(backup==1)
		AT45DBReadPage(4093,RdBuf);
	
	SystemPara.SoftwareVersion = RdBuf[num++];
	SystemPara.HardwareVersion = RdBuf[num++];
	SystemPara.Language = RdBuf[num++];
	SystemPara.MaintainPassword[0] = RdBuf[num++];
	SystemPara.MaintainPassword[1] = RdBuf[num++];
	SystemPara.MaintainPassword[2] = RdBuf[num++];
	SystemPara.MaintainPassword[3] = RdBuf[num++];
	SystemPara.MaintainPassword[4] = RdBuf[num++];
	SystemPara.MaintainPassword[5] = RdBuf[num++];
	SystemPara.AdminPassword[0] = RdBuf[num++];
	SystemPara.AdminPassword[1] = RdBuf[num++];
	SystemPara.AdminPassword[2] = RdBuf[num++];
	SystemPara.AdminPassword[3] = RdBuf[num++];
	SystemPara.AdminPassword[4] = RdBuf[num++];
	SystemPara.AdminPassword[5] = RdBuf[num++];
	SystemPara.AdminPassword[6] = RdBuf[num++];
	SystemPara.AdminPassword[7] = RdBuf[num++];
	SystemPara.MechineSerialNo[0] = RdBuf[num++];
	SystemPara.MechineSerialNo[1] = RdBuf[num++];
	SystemPara.MechineSerialNo[2] = RdBuf[num++];
	SystemPara.MechineSerialNo[3] = RdBuf[num++];
	SystemPara.MechineSerialNo[4] = RdBuf[num++];
	SystemPara.MechineSerialNo[5] = RdBuf[num++];
	SystemPara.MechineSerialNo[6] = RdBuf[num++];
	SystemPara.MechineSerialNo[7] = RdBuf[num++];
	SystemPara.MechineSerialNo[8] = RdBuf[num++];
	SystemPara.MechineSerialNo[9] = RdBuf[num++];
	SystemPara.MechineDeviceType[0] = RdBuf[num++];
	SystemPara.MechineDeviceType[1] = RdBuf[num++];
	SystemPara.MechineDeviceType[2] = RdBuf[num++];
	SystemPara.MechineDeviceType[3] = RdBuf[num++];
	SystemPara.MechineDeviceType[4] = RdBuf[num++];
	SystemPara.MechineDeviceType[5] = RdBuf[num++];
	SystemPara.CoinAcceptorType = RdBuf[num++];
	SystemPara.CoinChangerType = RdBuf[num++];
	SystemPara.BillValidatorType = RdBuf[num++];
	SystemPara.BillITLValidator = RdBuf[num++];
	SystemPara.BillRecyclerType = RdBuf[num++];
	SystemPara.CashlessDeviceType = RdBuf[num++];
	SystemPara.DecimalNum = RdBuf[num++];
	SystemPara.DecimalNumExt = RdBuf[num++];
	SystemPara.GeziDeviceType = RdBuf[num++];
	SystemPara.DisplayMoudleType = RdBuf[num++];
	SystemPara.Logo = RdBuf[num++];
	SystemPara.UserSelectKeyBoard = RdBuf[num++];
	SystemPara.GOCIsOpen = RdBuf[num++];
	SystemPara.SaleTime  = RdBuf[num++];
	SystemPara.PcEnable  = RdBuf[num++];
	SystemPara.ChannelType = RdBuf[num++];
	SystemPara.SubBinOpen = RdBuf[num++];
	SystemPara.SubBinChannelType = RdBuf[num++];
	SystemPara.CunhuoKou = RdBuf[num++];
	SystemPara.EasiveEnable = RdBuf[num++];
	SystemPara.MaxValue     = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.Channel = RdBuf[num++];
	SystemPara.SubChannel = RdBuf[num++];
	SystemPara.BillValue[0] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[1] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[2] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[3] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[4] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[5] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[6] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.BillValue[7] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[0] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[1] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[2] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[3] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[4] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[5] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[6] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.CoinValue[7] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.HopperValue[0] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.HopperValue[1] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.HopperValue[2] = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);


	//================add by yoc 2013.12.16=========
	SystemPara.macSN = INTEG32(RdBuf[num++],RdBuf[num++],RdBuf[num++],RdBuf[num++]); 
	SystemPara.armSN[0] = RdBuf[num++];
	SystemPara.armSN[1] = RdBuf[num++];
	SystemPara.vmcType = RdBuf[num++]; 
	SystemPara.vmcRow = RdBuf[num++];
	SystemPara.vmcColumn = RdBuf[num++]; 
	SystemPara.aisleSize = RdBuf[num++]; 
	SystemPara.threeSelectKey = RdBuf[num++];
	SystemPara.hefangGui = RdBuf[num++];
    SystemPara.hefangGuiKou = RdBuf[num++];
	SystemPara.RecyclerMoney = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
    SystemPara.XMTTemp = RdBuf[num++];
	SystemPara.BillEnableValue   = (RdBuf[num++]<<24)|(RdBuf[num++]<<16)|(RdBuf[num++]<<8)|(RdBuf[num++]);
	SystemPara.ColumnTime  = RdBuf[num++];
	SystemPara.HpEmpCoin  = RdBuf[num++];
	
	crc = CrcCheck(RdBuf,num);
	//Trace("\r\n defaultsys=%d,%x,%x",num,crc/256,crc%256);
	SystemPara.CrcCheck[0] = RdBuf[num++];
	SystemPara.CrcCheck[1] = RdBuf[num++]; 
	Trace("\r\n defaultsys=%d,%x,%x,%x,%x",num,crc/256,crc%256,SystemPara.CrcCheck[0],SystemPara.CrcCheck[1]);

	//==========读取SN=====
	LOG_readId(&stMacSn); 
	if( (crc/256 == SystemPara.CrcCheck[0])&&(crc%256 == SystemPara.CrcCheck[1]) )
	{
		//Trace("\r\n1");
		return 1;
	}
	else
	{
		//Trace("\r\n0");
		memset((void *)&SystemPara,0,sizeof(SystemPara));
		LCDNumberFontPrintf(20,LINE15,2,"SystemPara-Failure");
		OSTimeDly(OS_TICKS_PER_SEC * 2);
		return 0;
	}
}


/*********************************************************************************************************
** Function name:     	LoadDefaultSystemPara
** Descriptions:	    保存工程系统参数设置
** input parameters:    backup=0,在主页面操作,=1在镜像页面操作
** output parameters:   无
** Returned value:      无
注意:sizeof得到结构体数据的大小，但是结构体存放在内存块中有存在他的一个对其方式,
下面用前面的例子来说明VC到底怎么样来存放结构的。

struct MyStruct

{

double dda1;

char dda;

int type

}；

为上面的结构分配空间的时候，VC根据成员变量出现的顺序和对齐方式，先为第一个成员dda1分配空间，其起始地址跟结构的起始地址相同
（刚好偏移量0刚好为sizeof(double)的倍数），该成员变量占用sizeof(double)=8个字节；接下来为第二个成员dda分配空间，这时下一个
可以分配的地址对于结构的起始地址的偏移量为8，是sizeof(char)的倍数，所以把dda存放在偏移量为8的地方满足对齐方式，该成员变量占
用sizeof(char)=1个字节；接下来为第三个成员type分配空间，这时下一个可以分配的地址对于结构的起始地址的偏移量为9，不是sizeof(int)=4的倍数，
为了满足对齐方式对偏移量的约束问题，VC自动填充3个字节（这三个字节没有放什么东西），这时下一个可以分配的地址对于结构的起始
地址的偏移量为12，刚好是sizeof(int)=4的倍数，所以把type存放在偏移量为12的地方，该成员变量占用sizeof(int)=4个字节；这时整个
结构的成员变量已经都分配了空间，总的占用的空间大小为：8+1+3+4=16，刚好为结构的字节边界数（即结构中占用最大空间的类型所占用的
字节数sizeof(double)=8）的倍数，所以没有空缺的字节需要填充。所以整个结构的大小为：sizeof(MyStruct)=8+1+3+4=16，其中有3个字节
是C自动填充的，没有放任何有意义的东西。

*********************************************************************************************************/
void WriteDefaultSystemPara(SYSTEMPARAMETER SysPara,uint8_t backup)
{
	//uint8_t *tp2;
	uint16_t crc;
	uint8_t Wrbuf[512]={0};
	uint16_t num=0;
	/*
	tp2 = (uint8_t *) &SystemPara;//交易数据记录	
	//crc = CrcCheck(tp2,sizeof(SystemPara)-2);	
	crc = CrcCheck(tp2,124);	
	SystemPara.CrcCheck[0] = crc/256;
	SystemPara.CrcCheck[1] = crc%256;
	Trace("\r\n defaultsys=%d,%x,%x",sizeof(SystemPara)-2,SystemPara.CrcCheck[0],SystemPara.CrcCheck[1]);	
	AT45DBWritePage(4090,tp2);
	*/
	Wrbuf[num++] = SysPara.SoftwareVersion;
	Wrbuf[num++] = SysPara.HardwareVersion;
	Wrbuf[num++] = SysPara.Language;
	Wrbuf[num++] = SysPara.MaintainPassword[0];
	Wrbuf[num++] = SysPara.MaintainPassword[1];
	Wrbuf[num++] = SysPara.MaintainPassword[2];
	Wrbuf[num++] = SysPara.MaintainPassword[3];
	Wrbuf[num++] = SysPara.MaintainPassword[4];
	Wrbuf[num++] = SysPara.MaintainPassword[5];
	Wrbuf[num++] = SysPara.AdminPassword[0];
	Wrbuf[num++] = SysPara.AdminPassword[1];
	Wrbuf[num++] = SysPara.AdminPassword[2];
	Wrbuf[num++] = SysPara.AdminPassword[3];
	Wrbuf[num++] = SysPara.AdminPassword[4];
	Wrbuf[num++] = SysPara.AdminPassword[5];
	Wrbuf[num++] = SysPara.AdminPassword[6];
	Wrbuf[num++] = SysPara.AdminPassword[7];
	Wrbuf[num++] = SysPara.MechineSerialNo[0];
	Wrbuf[num++] = SysPara.MechineSerialNo[1];
	Wrbuf[num++] = SysPara.MechineSerialNo[2];
	Wrbuf[num++] = SysPara.MechineSerialNo[3];
	Wrbuf[num++] = SysPara.MechineSerialNo[4];
	Wrbuf[num++] = SysPara.MechineSerialNo[5];
	Wrbuf[num++] = SysPara.MechineSerialNo[6];
	Wrbuf[num++] = SysPara.MechineSerialNo[7];
	Wrbuf[num++] = SysPara.MechineSerialNo[8];
	Wrbuf[num++] = SysPara.MechineSerialNo[9];
	Wrbuf[num++] = SysPara.MechineDeviceType[0];
	Wrbuf[num++] = SysPara.MechineDeviceType[1];
	Wrbuf[num++] = SysPara.MechineDeviceType[2];
	Wrbuf[num++] = SysPara.MechineDeviceType[3];
	Wrbuf[num++] = SysPara.MechineDeviceType[4];
	Wrbuf[num++] = SysPara.MechineDeviceType[5];
	Wrbuf[num++] = SysPara.CoinAcceptorType;
	Wrbuf[num++] = SysPara.CoinChangerType;
	Wrbuf[num++] = SysPara.BillValidatorType;
	Wrbuf[num++] = SysPara.BillITLValidator;
	Wrbuf[num++] = SysPara.BillRecyclerType;
	Wrbuf[num++] = SysPara.CashlessDeviceType;
	Wrbuf[num++] = SysPara.DecimalNum;
	Wrbuf[num++] = SysPara.DecimalNumExt;
	Wrbuf[num++] = SysPara.GeziDeviceType;
	Wrbuf[num++] = SysPara.DisplayMoudleType;
	Wrbuf[num++] = SysPara.Logo;
	Wrbuf[num++] = SysPara.UserSelectKeyBoard;
	Wrbuf[num++] = SysPara.GOCIsOpen;
	Wrbuf[num++] = SysPara.SaleTime;
	Wrbuf[num++] = SysPara.PcEnable;
	Wrbuf[num++] = SysPara.ChannelType;
	Wrbuf[num++] = SysPara.SubBinOpen;
	Wrbuf[num++] = SysPara.SubBinChannelType;
	Wrbuf[num++] = SysPara.CunhuoKou;
	Wrbuf[num++] = SysPara.EasiveEnable;
	Wrbuf[num++] = (SysPara.MaxValue>>24)&0xff;
	Wrbuf[num++] = (SysPara.MaxValue>>16)&0xff;
	Wrbuf[num++] = (SysPara.MaxValue>>8)&0xff;
	Wrbuf[num++] = (SysPara.MaxValue)&0xff;
	Wrbuf[num++] = SysPara.Channel;
	Wrbuf[num++] = SysPara.SubChannel;
	Wrbuf[num++] = (SysPara.BillValue[0]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[0]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[0]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[0])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[1]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[1]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[1]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[1])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[2]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[2]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[2]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[2])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[3]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[3]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[3]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[3])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[4]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[4]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[4]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[4])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[5]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[5]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[5]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[5])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[6]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[6]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[6]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[6])&0xff;
	Wrbuf[num++] = (SysPara.BillValue[7]>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[7]>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[7]>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillValue[7])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[0]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[0]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[0]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[0])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[1]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[1]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[1]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[1])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[2]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[2]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[2]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[2])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[3]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[3]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[3]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[3])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[4]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[4]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[4]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[4])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[5]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[5]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[5]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[5])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[6]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[6]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[6]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[6])&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[7]>>24)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[7]>>16)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[7]>>8)&0xff;
	Wrbuf[num++] = (SysPara.CoinValue[7])&0xff;	
	Wrbuf[num++] = (SysPara.HopperValue[0]>>24)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[0]>>16)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[0]>>8)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[0])&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[1]>>24)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[1]>>16)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[1]>>8)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[1])&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[2]>>24)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[2]>>16)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[2]>>8)&0xff;
	Wrbuf[num++] = (SysPara.HopperValue[2])&0xff;

	//================add by yoc 2013.12.16=========//
	Wrbuf[num++] = H0UINT32(SystemPara.macSN);
	Wrbuf[num++] = H1UINT32(SystemPara.macSN);
	Wrbuf[num++] = L0UINT32(SystemPara.macSN);
	Wrbuf[num++] = L1UINT32(SystemPara.macSN);
	Wrbuf[num++] = SystemPara.armSN[0];
	Wrbuf[num++] = SystemPara.armSN[1];
	Wrbuf[num++] = SystemPara.vmcType;
	Wrbuf[num++] = SystemPara.vmcRow;
	Wrbuf[num++] = SystemPara.vmcColumn;
	Wrbuf[num++] = SystemPara.aisleSize; 
	Wrbuf[num++] = SystemPara.threeSelectKey;
	Wrbuf[num++] = SystemPara.hefangGui;
	Wrbuf[num++] = SystemPara.hefangGuiKou;
	Wrbuf[num++] = (SysPara.RecyclerMoney>>24)&0xff;
	Wrbuf[num++] = (SysPara.RecyclerMoney>>16)&0xff;
	Wrbuf[num++] = (SysPara.RecyclerMoney>>8)&0xff;
	Wrbuf[num++] = (SysPara.RecyclerMoney)&0xff;
	Wrbuf[num++] = SystemPara.XMTTemp;
	Wrbuf[num++] = (SysPara.BillEnableValue>>24)&0xff;
	Wrbuf[num++] = (SysPara.BillEnableValue>>16)&0xff;
	Wrbuf[num++] = (SysPara.BillEnableValue>>8)&0xff;
	Wrbuf[num++] = (SysPara.BillEnableValue)&0xff;
	Wrbuf[num++] = SysPara.ColumnTime;
	Wrbuf[num++] = SysPara.HpEmpCoin;
	
	crc = CrcCheck(Wrbuf,num);	
	//Trace("\r\n defaultsys=%d,%x,%x",num,crc/256,crc%256);
	SysPara.CrcCheck[0] = crc/256;
	SysPara.CrcCheck[1] = crc%256;
	Wrbuf[num++] = SysPara.CrcCheck[0];
	Wrbuf[num++] = SysPara.CrcCheck[1];
	
	Trace("\r\n defaultsys=%d,%x,%x",num,SystemPara.CrcCheck[0],SystemPara.CrcCheck[1]);
	
	if(backup==0)
		AT45DBWritePage(4090,Wrbuf);	
	else if(backup==1)
		AT45DBWritePage(4093,Wrbuf);	
}



/*********************************************************************************************************
** Function name:     	LoadUserSystemPara
** Descriptions:	    保存用户系统参数设置
** input parameters:    backup=0,在主页面操作,=1在镜像页面操作
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t LoadUserSystemPara(uint8_t backup)
{
	uint16_t crc;
	uint8_t RdBuf[512]={0},i;
	uint16_t num=0;
	
	if(backup==0)
		AT45DBReadPage(4070,RdBuf);
	else if(backup==1)
		AT45DBReadPage(4080,RdBuf);
	
	UserPara.TransMultiMode = RdBuf[num++];
	UserPara.TransEscape = RdBuf[num++];	
	UserPara.CompressorCtr.DeviceOn = RdBuf[num++];
	UserPara.CompressorCtr.HourOn = RdBuf[num++];
	UserPara.CompressorCtr.MinOn = RdBuf[num++];
	UserPara.CompressorCtr.HourOff = RdBuf[num++];
	UserPara.CompressorCtr.MinOff = RdBuf[num++];
	UserPara.CompressorCtr.flag = RdBuf[num++];
	//UserPara.CompressorCtr.TimingFlag = RdBuf[num++];
	UserPara.LEDCtr.DeviceOn = RdBuf[num++];
	UserPara.LEDCtr.HourOn = RdBuf[num++];
	UserPara.LEDCtr.MinOn = RdBuf[num++];
	UserPara.LEDCtr.HourOff = RdBuf[num++];
	UserPara.LEDCtr.MinOff = RdBuf[num++];
	UserPara.LEDCtr.flag = RdBuf[num++];
	//UserPara.LEDCtr.TimingFlag = RdBuf[num++];
	UserPara.CompressorCtr2Bin.DeviceOn = RdBuf[num++];
	UserPara.CompressorCtr2Bin.HourOn = RdBuf[num++];
	UserPara.CompressorCtr2Bin.MinOn = RdBuf[num++];
	UserPara.CompressorCtr2Bin.HourOff = RdBuf[num++];
	UserPara.CompressorCtr2Bin.MinOff = RdBuf[num++];
	UserPara.CompressorCtr2Bin.flag = RdBuf[num++];
	//UserPara.CompressorCtr2Bin.TimingFlag = RdBuf[num++];
	UserPara.LEDCtr2Bin.DeviceOn = RdBuf[num++];
	UserPara.LEDCtr2Bin.HourOn = RdBuf[num++];
	UserPara.LEDCtr2Bin.MinOn = RdBuf[num++];
	UserPara.LEDCtr2Bin.HourOff = RdBuf[num++];
	UserPara.LEDCtr2Bin.MinOff = RdBuf[num++];
	UserPara.LEDCtr2Bin.flag = RdBuf[num++];
	//UserPara.LEDCtr2Bin.TimingFlag = RdBuf[num++];
	//写加热flash
	UserPara.box1Hot.DeviceOn = RdBuf[num++];
	for(i = 0; i < 5; i++)
	{
		UserPara.box1Hot.HourOn[i] = RdBuf[num++];
		UserPara.box1Hot.MinOn[i] = RdBuf[num++];
		UserPara.box1Hot.HourOff[i] = RdBuf[num++];
		UserPara.box1Hot.MinOff[i] = RdBuf[num++];
		UserPara.box1Hot.setTemp[i] = INTEG16(RdBuf[num++],RdBuf[num++]); 
		
	}
	UserPara.box1Hot.flag = RdBuf[num++];
	UserPara.box1Hot.curTemp = RdBuf[num++];
	UserPara.box1Hot.setTemp1 = RdBuf[num++];
	

		//写除臭flash
	UserPara.box1Chuchou.DeviceOn = RdBuf[num++];
	UserPara.box1Chuchou.HourOn = RdBuf[num++];
	UserPara.box1Chuchou.MinOn = RdBuf[num++];
	UserPara.box1Chuchou.HourOff = RdBuf[num++];
	UserPara.box1Chuchou.flag = RdBuf[num++];
	UserPara.box1Chuchou.MinOff = RdBuf[num++];
	

	UserPara.billTrace = RdBuf[num++];
	UserPara.coinTrace = RdBuf[num++];
	UserPara.changelTrace = RdBuf[num++];
	UserPara.readerTrace = RdBuf[num++];
	UserPara.channelTrace = RdBuf[num++];
	UserPara.compressorTrace = RdBuf[num++];
	UserPara.ledTrace = RdBuf[num++];
	UserPara.selectTrace = RdBuf[num++];
	UserPara.PC = RdBuf[num++];
	UserPara.TraceFlag = RdBuf[num++];
	UserPara.BillRecyclerTrace = RdBuf[num++];
	crc = CrcCheck(RdBuf,num);
	//Trace("\r\n defaultsys=%d,%x,%x",num,crc/256,crc%256);
	UserPara.CrcCheck[0] = RdBuf[num++];
	UserPara.CrcCheck[1] = RdBuf[num++]; 
	Trace("\r\n usersys=%d,%x,%x,%x,%x",num,crc/256,crc%256,UserPara.CrcCheck[0],UserPara.CrcCheck[1]);

	for(i = 0; i < 5;i++)
	{
		Trace("Read setTemp[%d]=%d\r\n",i,UserPara.box1Hot.setTemp[i]);
	}

	if( (crc/256 == UserPara.CrcCheck[0])&&(crc%256 == UserPara.CrcCheck[1]) )
	{
		//Trace("\r\n1");
		return 1;
	}
	else
	{
		//Trace("\r\n0");
		memset((void *)&UserPara,0,sizeof(UserPara));
		LCDNumberFontPrintf(20,LINE15,2,"UserPara-Failure");	
		OSTimeDly(OS_TICKS_PER_SEC * 2);
		return 0;
	}

	
}


/*********************************************************************************************************
** Function name:     	WriteUserSystemPara
** Descriptions:	    写入用户系统参数设置
** input parameters:    backup=0,在主页面操作,=1在镜像页面操作
** output parameters:   无
** Returned value:      无
注意:sizeof得到结构体数据的大小，但是结构体存放在内存块中有存在他的一个对其方式,
下面用前面的例子来说明VC到底怎么样来存放结构的。

struct MyStruct

{

double dda1;

char dda;

int type

}；

为上面的结构分配空间的时候，VC根据成员变量出现的顺序和对齐方式，先为第一个成员dda1分配空间，其起始地址跟结构的起始地址相同
（刚好偏移量0刚好为sizeof(double)的倍数），该成员变量占用sizeof(double)=8个字节；接下来为第二个成员dda分配空间，这时下一个
可以分配的地址对于结构的起始地址的偏移量为8，是sizeof(char)的倍数，所以把dda存放在偏移量为8的地方满足对齐方式，该成员变量占
用sizeof(char)=1个字节；接下来为第三个成员type分配空间，这时下一个可以分配的地址对于结构的起始地址的偏移量为9，不是sizeof(int)=4的倍数，
为了满足对齐方式对偏移量的约束问题，VC自动填充3个字节（这三个字节没有放什么东西），这时下一个可以分配的地址对于结构的起始
地址的偏移量为12，刚好是sizeof(int)=4的倍数，所以把type存放在偏移量为12的地方，该成员变量占用sizeof(int)=4个字节；这时整个
结构的成员变量已经都分配了空间，总的占用的空间大小为：8+1+3+4=16，刚好为结构的字节边界数（即结构中占用最大空间的类型所占用的
字节数sizeof(double)=8）的倍数，所以没有空缺的字节需要填充。所以整个结构的大小为：sizeof(MyStruct)=8+1+3+4=16，其中有3个字节
是C自动填充的，没有放任何有意义的东西。

*********************************************************************************************************/
void WriteUserSystemPara(SYSUSERPARAMETER UPara,uint8_t backup)
{
	uint16_t crc;
	uint8_t Wrbuf[512]={0},i;
	uint16_t num=0;
	
	Wrbuf[num++] = UPara.TransMultiMode;
	Wrbuf[num++] = UPara.TransEscape;
	Wrbuf[num++] = UPara.CompressorCtr.DeviceOn;
	Wrbuf[num++] = UPara.CompressorCtr.HourOn;
	Wrbuf[num++] = UPara.CompressorCtr.MinOn;
	Wrbuf[num++] = UPara.CompressorCtr.HourOff;
	Wrbuf[num++] = UPara.CompressorCtr.MinOff;
	Wrbuf[num++] = UPara.CompressorCtr.flag;
	//Wrbuf[num++] = UPara.CompressorCtr.TimingFlag;
	Wrbuf[num++] = UPara.LEDCtr.DeviceOn;
	Wrbuf[num++] = UPara.LEDCtr.HourOn;
	Wrbuf[num++] = UPara.LEDCtr.MinOn;
	Wrbuf[num++] = UPara.LEDCtr.HourOff;
	Wrbuf[num++] = UPara.LEDCtr.MinOff;
	Wrbuf[num++] = UPara.LEDCtr.flag;
	//Wrbuf[num++] = UPara.LEDCtr.TimingFlag;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.DeviceOn;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.HourOn;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.MinOn;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.HourOff;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.MinOff;
	Wrbuf[num++] = UPara.CompressorCtr2Bin.flag;
	//Wrbuf[num++] = UPara.CompressorCtr2Bin.TimingFlag;
	Wrbuf[num++] = UPara.LEDCtr2Bin.DeviceOn;
	Wrbuf[num++] = UPara.LEDCtr2Bin.HourOn;
	Wrbuf[num++] = UPara.LEDCtr2Bin.MinOn;
	Wrbuf[num++] = UPara.LEDCtr2Bin.HourOff;
	Wrbuf[num++] = UPara.LEDCtr2Bin.MinOff;
	Wrbuf[num++] = UPara.LEDCtr2Bin.flag;
	//Wrbuf[num++] = UPara.LEDCtr2Bin.TimingFlag;
	//写加热flash
	Wrbuf[num++] = UPara.box1Hot.DeviceOn;
	for(i = 0; i < 5; i++)
	{
		Wrbuf[num++] = UPara.box1Hot.HourOn[i] ;
		Wrbuf[num++] = UPara.box1Hot.MinOn[i] ;
		Wrbuf[num++] = UPara.box1Hot.HourOff[i] ;
		Wrbuf[num++] = UPara.box1Hot.MinOff[i] ;
		Wrbuf[num++] = HUINT16(UPara.box1Hot.setTemp[i]) ;
		Wrbuf[num++] = LUINT16(UPara.box1Hot.setTemp[i]) ;
	}
	
	Wrbuf[num++] = UPara.box1Hot.flag;
	Wrbuf[num++] = UPara.box1Hot.curTemp;
	Wrbuf[num++] = UPara.box1Hot.setTemp1;
	
	

		//写除臭flash
	Wrbuf[num++] = UPara.box1Chuchou.DeviceOn;
	Wrbuf[num++] = UPara.box1Chuchou.HourOn;
	Wrbuf[num++] = UPara.box1Chuchou.MinOn;
	Wrbuf[num++] = UPara.box1Chuchou.HourOff;
	Wrbuf[num++] = UPara.box1Chuchou.flag;
	Wrbuf[num++] = UPara.box1Chuchou.MinOff;

	
	Wrbuf[num++] = UPara.billTrace;
	Wrbuf[num++] = UPara.coinTrace;
	Wrbuf[num++] = UPara.changelTrace;
	Wrbuf[num++] = UPara.readerTrace;
	Wrbuf[num++] = UPara.channelTrace;
	Wrbuf[num++] = UPara.compressorTrace;
	Wrbuf[num++] = UPara.ledTrace;
	Wrbuf[num++] = UPara.selectTrace;
	Wrbuf[num++] = UPara.PC;
	Wrbuf[num++] = UPara.TraceFlag;
	Wrbuf[num++] = UPara.BillRecyclerTrace;
	
	crc = CrcCheck(Wrbuf,num);	
	//Trace("\r\n defaultsys=%d,%x,%x",num,crc/256,crc%256);
	UPara.CrcCheck[0] = crc/256;
	UPara.CrcCheck[1] = crc%256;
	Wrbuf[num++] = UPara.CrcCheck[0];
	Wrbuf[num++] = UPara.CrcCheck[1];
	Trace("\r\n defaultsys=%d,%x,%x",num,UPara.CrcCheck[0],UPara.CrcCheck[1]);
	for(i = 0; i < 5;i++)
	{
		Trace("setTemp[%d]=%d\r\n",i,UPara.box1Hot.setTemp[i]);
	}

	if(backup==0)
		AT45DBWritePage(4070,Wrbuf);	
	else if(backup==1)
		AT45DBWritePage(4080,Wrbuf);	
}




/*********************************************************************************************************
** Function name:     	ResetSystemPara
** Descriptions:	    根据当前系统设备重设默认工程系统参数并保存
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ResetSystemPara(void)
{
	uint8_t i;

	//纸币器通道面值同步
	if(SystemPara.BillValidatorType == MDB_BILLACCEPTER)
	{
		for(i=0;i<8;i++)
			SystemPara.BillValue[i] = stDevValue.BillValue[i];

		//循环纸币器通道面值同步
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			for(i=0;i<7;i++)
			{
				SystemPara.RecyclerValue[i] = stDevValue.RecyclerValue[i];
				if(SystemPara.RecyclerValue[i]>0)
				{
					SystemPara.RecyclerMoney = SystemPara.RecyclerValue[i];
				}
			}
		}
	}

	//硬币器通道面值同步
	if(SystemPara.CoinAcceptorType  == MDB_COINACCEPTER)
	{	
		for(i=0;i<8;i++)
			SystemPara.CoinValue[i] = stDevValue.CoinValue[i];
		switch(stDevValue.CoinDecimal) 
	   	{
	      case 1://以分为单位
		  	  SystemPara.DecimalNum  = 2;  
			  break;

		  case 10://以角为单位
		  	  SystemPara.DecimalNum  = 1;  
			  break;
		  
		  case 100://以元为单位
			  SystemPara.DecimalNum  = 0;  
			  break;
	  	 }
		                          //显示金额界面的小数点位数
	}
	else 
	{	
		for(i=0;i<8;i++)
			stDevValue.CoinValue[i] = SystemPara.CoinValue[i];
	}
}



/*********************************************************************************************************
** Function name:     	XorCheck
** Descriptions:	    累加校验和
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char XorCheck(unsigned char *pstr,unsigned short len)
{
	unsigned char xor_check = 0;
	unsigned short i;
	for(i=0;i<len;i++) 
	{
       	xor_check = xor_check^pstr[i];
   	}
	return xor_check;
}
/*********************************************************************************************************
** Function name:     	CrcCheck
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned short CrcCheck(unsigned char *msg, unsigned short len) 
{
    unsigned short i, j;
    unsigned short crc = 0;
    unsigned short current = 0;
    for(i=0;i<len;i++) 
    {
        current = msg[i] << 8;
        for(j=0;j<8;j++) 
        {
            if((short)(crc^current)<0)
                crc = (crc<<1)^0x1021;
            else 
                crc <<= 1; 
            current <<= 1; 
        }
    }
    return crc;
}

/********************************************************************************************************
** Function name:     	PrintfMoney
** Descriptions:	    依据小数位数，显示金额的函数
** input parameters:    dispnum:投币金额，以分为单位 
** output parameters:   无
** Returned value:      显示字符串
*********************************************************************************************************/
char *PrintfMoney(uint32_t dispnum)
{  
	char strnum[10];
	
	
   switch(SystemPara.DecimalNum) 
   {
      case 2://以分为单位
	  	  sprintf(strnum,"%d.%02d",dispnum/100,dispnum%100);	
		  
		  break;

	  case 1://以角为单位
	  	  dispnum /= 10;
		  sprintf(strnum,"%d.%d",dispnum/10,dispnum%10);
		  break;
	  
	  case 0://以元为单位
		  sprintf(strnum,"%d",dispnum/100);
		  break;
   }
   //Trace("\r\n pp=%s",strnum);
   return &strnum[0];
}

/**************************************End Of File*******************************************************/
