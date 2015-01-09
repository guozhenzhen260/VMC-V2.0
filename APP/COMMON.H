/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        系统参数定义及通用函数部分                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __COMMON_H 
#define __COMMON_H

#define MAXLANGUAGETYPE 5 //系统支持最大的语言种类，目前暂定5种，中文，英文，土耳其，俄罗斯，葡萄牙

#define OFF_COINACCEPTER         0//关硬币器
#define PARALLEL_COINACCEPTER    1//并行脉冲硬币器
#define MDB_COINACCEPTER         2//MDB硬币器
#define SERIAL_COINACCEPTER      3//串行脉冲硬币器

#define OFF_BILLACCEPTER         0//关纸币器
#define MDB_BILLACCEPTER         2//MDB纸币器

#define NO_ITLBILLACCEPTER       0//不是ITL纸币器
#define ITL_BILLACCEPTER         1//是ITL纸币器

#define OFF_BILLRECYCLER         0//关循环纸币找零器
#define MDB_BILLRECYCLER         2//MDB循环纸币找零器

#define OFF_CHANGER       		 0//关找零器
#define HOPPER_CHANGER           1//Hopper找零器
#define MDB_CHANGER              2//MDB找零器

#define OFF_READERACCEPTER		 0//关读卡器
#define MDB_READERACCEPTER       2//MDB读卡器

#define OFF_PC			         0//关PC通讯
#define ZHIHUI_PC			     1//一鸣智慧pc通讯
#define UBOX_PC			         2//友宝pc通讯
#define GPRS_PC					 3 //GPRS

#define OFF_KEY			         0//普通键盘按键
#define SELECT_KEY			     1//选货按键

#define TEMPERATURE			     1//开启加热模块



extern RTC_DATE RTCData;


/******************************************屏幕显示行数*****************************************************/
#define LINE1                    0
#define LINE2                    1
#define LINE3                    2
#define LINE4                    3
#define LINE5                    4
#define LINE6                    5
#define LINE7                    6
#define LINE8                    7
#define LINE9                    8
#define LINE10                   9
#define LINE11                   10
#define LINE12                   11
#define LINE13                   12
#define LINE14                   13
#define LINE15                   14
#define LINE16                   15

/******************************************工程系统参数*****************************************************/
typedef struct	
{
	uint8_t		SoftwareVersion;				//软件版本
	uint8_t		HardwareVersion;				//硬件版本
	uint8_t     Language;						//语言选择	
	uint8_t		MaintainPassword[6];			//维护密码
	uint8_t		AdminPassword[8];				//管理员密码
	uint8_t		MechineSerialNo[10];			//设备序列号
	uint8_t		MechineDeviceType[6];			//设备型号,如EV7636			31
	//硬币器,如MDB硬币器、串行脉冲硬币器
	uint8_t		CoinAcceptorType;				//硬币接收器类型
	//找零器,如MDB找零器，Hopper
	uint8_t		CoinChangerType;				//硬币找零器类型
	//纸币器,如MDB纸币器器，SSP接口纸币器
	uint8_t		BillValidatorType;				//纸币接收器类型
	uint8_t		BillITLValidator;				//针对ITL纸币接收器,因为暂存时需要不断发poll指令,因此出货时根据情况要退出暂存纸币类型
	//纸币找零,如MDB 找币器
	uint8_t		BillRecyclerType;				//纸币找币器类型
	//非现金设备，如MDB读卡器，公交RFID卡等
	uint8_t		CashlessDeviceType;				//非现金设备类型
	uint8_t     DecimalNum;						//显示金额界面的小数点位数,2分,1角,0元
	//通讯设备，如MDB Communications Gateway
	uint8_t		DecimalNumExt;					//小数点位数扩展,前提DecimalNum必须为0; 0是元,1是10元,2是100元,3是1000元,4是10000元
	//验证模块，如MDB Age Verification Device
	uint8_t		GeziDeviceType;				    //1代表只有格子柜，没有主副货道柜。0是有主副货道柜
	//显示设备，如两行屏、四行屏、点阵屏
	uint8_t 	DisplayMoudleType;				//显示器类型
	uint8_t     Logo;							//Logo选择	
	//选货按键板
	uint8_t		UserSelectKeyBoard;				//用户选货按键				42
	uint8_t     GOCIsOpen;						//出货确认开启标志
	uint8_t		SaleTime;                       //交易倒计时
	uint8_t		PcEnable;                       //开启pc通讯
	//主柜的货道排列类型：0——8x8   1——8x10
	uint8_t     ChannelType;					//主柜货道类型
	//是否开启副柜标志   1——开启，0——关闭
	uint8_t     SubBinOpen;						//是否开启副柜
	//副柜的货道排列类型:0——8x8    1——8*10
	uint8_t     SubBinChannelType;				//副柜货道排列类型
	uint8_t     CunhuoKou;				        //出货扣存货数量1扣除,0不扣除
	uint8_t     EasiveEnable;				    //在PcEnable=2,代表使用友宝协议的时候，这个=1,代表转为骏鹏协议
	uint32_t    MaxValue;				    	//选择的最高交易上限
	uint8_t 	Channel;		                //主柜货道种类 0 :关闭1 弹簧 2老式升降机3新式升降机+传送4升降机+弹簧
	uint8_t		SubChannel;                     //副柜货道种类0 :关闭1 弹簧 2老式升降机3新式升降机+传送4升降机+弹簧
	//纸币器面值
	uint32_t BillValue[8];//各个通道面值	
	//硬币器面值
	uint32_t CoinValue[8];	     //各个通道面值
	//Hopper找零器面值
	uint32_t HopperValue[3];	     //各个通道面值
	//交易记录存储最高页
	//uint8_t		VendingLogMaxPage[2];			//交易记录储存最高页,固定为4000
	//指定交易总记录存储页面
	//uint8_t		VendingLogTotalPage[2];			//指定交易统计总记录的存储页面(4060~4080) 		46
	//Reserve
	//uint8_t		ReserveByte[466];				//保留字节(0x0填充)，后续扩展				 	510
	//================add by yoc 2013.12.16=============================
	uint32_t  macSN; //机器型号
	uint16_t  armSN[2];//主控板版本号
	uint8_t   vmcType; //机器类型 0X01饮料机，0X11食品机，0X21
	uint8_t   vmcRow;//货道排数
	uint8_t   vmcColumn; //货道列数	
	uint8_t   aisleSize; //货道总数
	uint8_t   threeSelectKey;//是否使用三条的选货按键板,1是使用三条的,0是使用6条的
	uint8_t   hefangGui;//是否开启盒饭柜
	uint8_t   hefangGuiKou;	//盒饭柜出货扣存货数量1扣除,0不扣除;
	//纸币器找币面值
	uint32_t RecyclerValue[7];//各个通道面值
	uint32_t  RecyclerMoney; //纸币器找币面值
	uint8_t   XMTTemp;//是否使用温控器,1是使用
	uint32_t  BillEnableValue;				   //选择的开纸币器交易上限,0代表永远不关纸币器
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRC校验

}	SYSTEMPARAMETER;							//系统参数保存在SPI_FLASH的4095页即最后一页
extern SYSTEMPARAMETER SystemPara;


/******************************************设备控制时间参数*****************************************************/
typedef struct	
{
uint8_t DeviceOn;
uint8_t HourOn;
uint8_t	MinOn;
uint8_t HourOff;
uint8_t	MinOff;
uint8_t flag;
uint8_t TimingFlag;
}DEVICECONTROLTIME;

/******************************************加热设备控制时间参数*****************************************************/
typedef struct	
{
uint8_t DeviceOn;
uint8_t HourOn[5];	 //开启小时
uint8_t	MinOn[5];	//开启分
uint8_t HourOff[5]; //关闭小时
uint8_t	MinOff[5];	//关闭分
uint16_t setTemp[5];//设置温度
uint16_t setTemp1;//设置温度


uint8_t flag;	 //  bit0: 0 关闭状态 1开启状态 bit1:0定时状态  1非定时状态
uint8_t TimingFlag;

uint16_t curTemp;//当前温度


}ST_HOT_DEV;




/******************************************用户系统参数*****************************************************/
typedef struct	
{
	uint8_t     		TransMultiMode;				//允许多次交易
	uint8_t     		TransEscape;				//强制购买
	DEVICECONTROLTIME	CompressorCtr;		  		//压缩机开关控制
	DEVICECONTROLTIME	LEDCtr;		  				//展示灯开关控制
	DEVICECONTROLTIME   CompressorCtr2Bin;			//2柜压缩机控制时间
	DEVICECONTROLTIME	LEDCtr2Bin;					//2柜展示灯控制时间
	ST_HOT_DEV			box1Hot;					//1柜加热
	ST_HOT_DEV			box2Hot;					//2柜加热
	DEVICECONTROLTIME	box1Chuchou;				//1柜除臭
	DEVICECONTROLTIME	box2Chuchou;				//2柜除臭

	//打开调试信息
	uint8_t     		billTrace;				
	uint8_t     		coinTrace;				
	uint8_t     		changelTrace;				
	uint8_t     		readerTrace;				
	uint8_t     		channelTrace;				
	uint8_t     		compressorTrace;				
	uint8_t     		ledTrace;	
	uint8_t     		selectTrace;
	uint8_t     		PC;
	uint8_t     		TraceFlag;		//add by yoc 2013.10.16
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRC校验
	

}	SYSUSERPARAMETER;							//系统参数保存在SPI_FLASH的4095页即最后一页
extern SYSUSERPARAMETER UserPara;


/******************************************总交易日志参数*****************************************************/
typedef struct	
{
	//区域销售统计
	uint32_t	Income;		  					//总收币值
	uint32_t	NoteIncome;	  					//纸币总收币值
	uint32_t	CoinsIncome;  					//硬币总收币值
	uint32_t	TotalTrans;	  					//交易总金额
	uint32_t	TotalChange;  					//找零总金额
	uint32_t	CoinsChange[3];	 				//找零器出币数
	uint32_t	SuccessNum;	  					//成功交易总次数
	uint32_t	DoubtNum;	  					//疑问交易总次数
	uint32_t		Iou;	  					//欠费总额


	//PC通讯用
	uint32_t   	vpSuccessNum;                    //全部出货数量
	uint32_t   	vpSuccessMoney;                  //全部出货金额
	uint32_t    vpCashNum;                       //现金出货数量
	uint32_t    vpCashMoney;                     //现金出货金额	
	uint32_t    vpGameNum;                 		 //游戏出货数量 
	uint32_t    vpGameMoney;              		 //游戏出货金额
	//uint32_t	CardNum;  						//一卡通交易次数
	//uint32_t	CardIncome;  					//一卡通消费金额
	uint32_t    vpCardNum;                       //所有卡出货数量
	uint32_t    vpCardMoney;                     //所有卡出货金额
	uint32_t    vpOnlineNum;                     //在线出货数量
	uint32_t    vpOnlineMoney;                  //在线出货金额
	
	//总销售统计
	uint32_t	IncomeTotal;		  			//总收币值
	uint32_t	NoteIncomeTotal;	  			//纸币总收币值
	uint32_t	CoinsIncomeTotal;  				//硬币总收币值
	uint32_t	TotalTransTotal;	  			//交易总金额
	uint32_t	TotalChangeTotal;  				//找零总金额
	uint32_t	CoinsChangeTotal[3];	 		//找零器出币数
	//PC通讯用
	uint32_t   	vpSuccessNumTotal;              //全部出货数量
	uint32_t   	vpSuccessMoneyTotal;            //全部出货金额
	uint32_t    vpCashNumTotal;                 //现金出货数量
	uint32_t    vpCashMoneyTotal;               //现金出货金额
	uint32_t    vpGameNumTotal;                 //游戏出货数量 
	uint32_t    vpGameMoneyTotal;               //游戏出货金额
	uint32_t    vpCardNumTotal;                 //刷卡出货数量
	uint32_t    vpCardMoneyTotal;               //刷卡出货金额
	uint32_t    vpOnlineNumTotal;               //在线出货数量
	uint32_t    vpOnlineMoneyTotal;             //在线出货金额

	//add by yoc zhihui 2013.12.16
	uint32_t    vpPC2NumTotal;               //互联网提货，包括支付宝提货
	uint32_t    vpPC2MoneyTotal;            
	uint32_t    vpPC3NumTotal;               //其他PC支付方式的汇总信息
	uint32_t    vpPC3MoneyTotal; 
	uint32_t	vpOneCardNumTotal;  		//一卡通交易次数 
	uint32_t	vpOneCardMoneyTotal;  		//一卡通消费金额
	uint32_t    vpUCardNumTotal;             //银联卡 交易次数 
	uint32_t    vpUCardMoneyTotal;             //银联卡消费金额
	
	//------------------------------------------------
	//离线销售数据
	uint8_t	 offLineFlag;                   //是否离线处理 1离线模式,0在线模式 
	uint32_t   offLineNum;                    //离线出货数量
	uint32_t   offLineMoney;                  //离线出货金额
	uint16_t	offDetailPage;				  //离线详细交易日志索引页
	//详细交易日志索引页
	uint16_t	LogDetailPage;					//详细交易日志索引页
	uint8_t    run_no;
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRC校验	

}	LOGPARAMETER;							
extern LOGPARAMETER LogPara;

/******************************************交易日志明细参数*****************************************************/
typedef struct	
{
	uint16_t		BeginYear;		//开始交易年
	uint8_t			BeginMonth;		//开始交易月
	uint8_t			BeginDay;		//开始交易日
	uint8_t			BeginHour;		//开始交易时
	uint8_t			BeginMinute;	//开始交易分
	uint8_t			EndMinute;		//结束交易分
	//add by yoc 2013.9.11
	uint8_t			BeginSec;	//开始交易秒
	uint8_t			EndSec;		//结束交易秒
	
	uint16_t		IncomeBill;		//投入纸币
	uint16_t		IncomeCoin;		//投入硬币
	uint32_t		IncomeCard;		//刷卡收入

	char			ColumnNo[24];	//出货货道编号
	uint8_t			GoodsNo[8];		//出货商品编号
	uint32_t		PriceNo[8];      //出货商品单价
	uint8_t			TransSucc[8];    //出货结果成功还是失败,1成功,0失败
	/*************add by yoc 2013.9.11***********************/
	uint8_t			PayMode[8];//单次交易类型
	uint8_t			GoodsMin[8];//单次出货分 为16进制非BCD码 下同
	uint8_t			GoodsSec[8];//单次出货秒	
	uint8_t 		run_no[8];//流水号
	uint8_t			TradeNum;//多次购买次数
	
	/*******************end add*************************/
	uint16_t		Change;			//找零
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRC校验	
}	LOGPARAMETERDETAIL;							
extern LOGPARAMETERDETAIL LogParaDetail;



/******************************************纸币器硬币器设备参数*****************************************************/
struct STDevValue
{
    //纸币器相关参数
    uint8_t  BillLevel;			  //纸币器等级
	uint16_t BillCode;		  //国家代码
	uint32_t BillScale;			  //比例因子
	uint16_t BillDecimal;		  //10^小数位数
	uint16_t BillStkCapacity;	  //储币容量
	uint16_t BillSecurity;		  //安全等级
	uint8_t  BillEscrowFun;		  //暂存功能
	uint32_t BillValue[16];//各个通道面值
	uint8_t billIDENTITYBuf[36];  //Identification
	
	//硬币器相关参数
	uint32_t CoinValue[16];	     //各个通道面值
	uint8_t  CoinLevel;			 //硬币器等级
	uint16_t CoinCode;			 //国家代码
	uint32_t  CoinScale;		 //比例因子
	uint16_t CoinDecimal;		 //10^小数位数
	uint16_t CoinRouting;		 //Bit is set to indicate a coin type can be routed to the tube
	uint16_t CoinNum[16];	     //各个通道存币数量
	uint8_t coinIDENTITYBuf[36];//Identification
	
	//Hopper找零器相关参数
	//uint32_t HopperValue[3];	     //各个通道面值

	//读卡器相关参数
	uint8_t  ReaderLevel;			  //读卡器等级
	//uint16_t ReaderCode;		  //国家代码
	uint8_t  ReaderScale;			  //比例因子
	uint16_t ReaderDecimal;		  //10^小数位数
	uint8_t  ReaderRecTime;		  //最大回应时间
	uint8_t  ReaderMiscelOpt;	  //是否支持其他功能，比如允许退回钱等等
	uint8_t  ReaderCashSale;	  //1允许现金详细交易记录都提交给他

	//纸币循环找零器
	uint32_t RecyclerValue[7];    //各个通道面值
	uint16_t RecyclerNum[7];	  //各个通道存币数量
};
extern struct STDevValue stDevValue;

/******************************************展示位按键设备参数*****************************************************/
typedef struct	
{
	//选货按键;by gzz 20121025
	uint8_t  sel1ReadyLed;               //按键板1的√灯
	uint8_t  sel1ErrLed;                 //按键板1的×灯
	uint8_t  sel2ReadyLed;               //按键板2的√灯
	uint8_t  sel2ErrLed;                 //按键板2的×灯
	uint8_t  sel3ReadyLed;               //按键板3的√灯
	uint8_t  sel3ErrLed;                 //按键板3的×灯
	uint8_t  sel4ReadyLed;               //按键板4的√灯
	uint8_t  sel4ErrLed;                 //按键板4的×灯
	uint8_t  sel5ReadyLed;               //按键板5的√灯
	uint8_t  sel5ErrLed;                 //按键板5的×灯
	uint8_t  sel6ReadyLed;               //按键板6的√灯
	uint8_t  sel6ErrLed;                 //按键板6的×灯
}SELECTKEYAPI;
extern SELECTKEYAPI selectKey;


/******************************************纸币器硬币器通讯结构体*****************************************************/
//纸币器收到纸币返回面值
#define MBOX_BILLMONEY					0x01
//禁能纸币器命令
#define MBOX_BILLDISABLEDEV				0x02
//使能纸币器命令
#define MBOX_BILLENABLEDEV				0x03
//启动纸币器命令
#define MBOX_BILLINITDEV				0x04
//纸币器压抄命令
#define MBOX_BILLESCROW					0x05
//返回纸币器压抄成功命令
#define MBOX_BILLESCROWSUCC				0x06
//返回纸币器压抄失败命令
#define MBOX_BILLESCROWFAIL				0x07
//纸币器退币命令
#define MBOX_BILLRETURN					0x08
//返回纸币器退币成功命令
#define MBOX_BILLRETURNSUCC				0x09
//返回纸币器退币失败命令
#define MBOX_BILLRETURNFAIL				0x0A

//返回纸币器参数命令
#define MBOX_BILLSETUPDEV				0x0B


//硬币器收到硬币返回面值
#define MBOX_COINMONEY					0x0C
//禁能硬币器命令
#define MBOX_COINDISABLEDEV				0x0D
//使能硬币器命令
#define MBOX_COINENABLEDEV				0x0E
//启动硬币器命令
#define MBOX_COININITDEV				0x0F
//返回硬币器按下退币键退币请求命令
#define MBOX_COINRETURN					0x10
//返回硬币器参数命令
#define MBOX_COINSETUPDEV				0x11


//对于level3的mdb找零器找零命令
#define MBOX_MDBCHANGELEVEL3			0x12
//对于level2的mdb找零器找零命令
#define MBOX_MDBCHANGELEVEL2			0x13
//对于得到找零器余币量
#define MBOX_MDBCHANGETUBE				0x14
//对于hopper的找零器找零命令
#define MBOX_MDBCHANGEHOPPER			0x15
//找零器找零返回命令
#define MBOX_CHANGERBACKMONEY			0x16

//对于Hopper的找零命令
#define MBOX_HOPPEROUT              	0x17
//对于Hopper的找零器找零返回命令
#define MBOX_HOPPERBACKMONEY			0x18
//启动时监测Hopper状态
#define MBOX_HOPPERINITSTATE            0x19


//读卡器上传给vmc
//读卡器收到金额返回面值
#define MBOX_READERMONEY				0x1A
//读卡器允许出货
#define MBOX_READERVENDOUT  			0x1B
//读卡器拒绝出货
#define MBOX_READERVENDDENY  			0x1C
//读卡器拔卡
#define MBOX_READERMONEYOUT				0x1D


//vmc下发给读卡器
//禁能读卡器命令
#define MBOX_READERDISABLEDEV			0x1E
//使能读卡器命令
#define MBOX_READERENABLEDEV			0x1F
//启动读卡器命令
#define MBOX_READERINITDEV				0x20
//读卡器扣钱命令
#define MBOX_READERVENDREQUEST			0x21
//交易成功命令
#define MBOX_READERVENDSUCCESS			0x22
//交易失败命令
#define MBOX_READERVENDFAIL 			0x23
//结束交易流程命令
#define MBOX_READERVENDCOMP 			0x24
//读卡器记录现金详细交易记录命令
#define MBOX_READERCASHSALE 			0x25


//启动纸币循环找零器命令
#define MBOX_BILLRECYCLERDEV			0x26
//纸币器按张数找币
#define MBOX_BILLRECYPAYOUTNUM			0x27
//纸币器按金额找币
#define MBOX_BILLRECYPAYOUTVALUE		0x28
//返回纸币器找零成功命令
#define MBOX_BILLRECYPAYOUTSUCC			0x29
//返回纸币器找零失败命令
#define MBOX_BILLRECYPAYOUTFAIL			0x2A




typedef struct
{
	//纸币器
    uint8_t   BillCmd;						//投币命令类型
	uint32_t  BillMoney;					//投币金额
	uint8_t   BillBack;						//控制命令类型
	uint8_t   BillBackCmd;					//纸币器上传给vmc命令控制的返回类型

	//硬币器
    uint8_t   CoinCmd;						//投币命令类型
	uint32_t  CoinMoney;					//投币金额
	uint8_t   CoinBack;						//控制命令类型
	uint8_t   CoinBackCmd;					//硬币器上传给vmc命令控制的返回类型

	//找零器
	uint8_t   PayoutCmd;					//找零命令类型,包括MDB,Hopper
	uint32_t  PayoutMoney;					//找零命令金额
	uint32_t  PayoutType;					//找零命令通道,包括MDB,Hopper
	uint32_t  PayoutNum;					//找零命令枚数,包括MDB,Hopper
	uint8_t   PayoutBackCmd;				//找零返回命令类型,包括MDB,Hopper
	uint32_t  PayoutBackNum[16];			//MDB找零返回各通道枚数
	uint8_t   SubBackType;					//Hopper找零返回命令通道
	uint32_t  BkPackValue;					//Hopper找零返回通道枚数

	//读卡器
	//上报插卡指令
    uint8_t   ReaderCmd;					//插卡,读卡器上传给vmc插卡命令类型
	uint32_t  ReaderMoney;					//插卡交易金额
	
	//vmc向reader发送命令
	uint8_t   ReaderBack;					//vmc下发给读卡器控制命令类型
	uint16_t  ReaderPrice;					//销售单价
	//reader返回命令
	uint8_t   ReaderBackCmd;				//读卡器上传给vmc命令类型
	//货道指令参数
	uint8_t   Binnum;						//机柜编号
	uint8_t   ChannelNum;					//货道编号
	uint8_t   GOCFlag;						//出货检测开启标志
	uint8_t   ChnlHandleType;				//货道操作类型
	uint8_t   HandleResult;					//操作结果
	uint8_t   ACDCHandleType;				//压缩机控制操作类型
	uint16_t   ACDCData;						//压缩机操作数据
	uint8_t   LiftTablePack[10];   //change by yoc  
	uint8_t   LevelNum;						//层号
	//盒饭柜邮箱通信参数 Add by liya 2014-01-20
	uint8_t   HeFanGuiHandle;				//盒饭柜操作类型
	uint8_t   HeFanGuiNum;					//盒饭柜柜号
	uint8_t   HeFanGuiBuf[36];				//接收到的自定义数据缓存
	uint8_t   HeFanGuiRst;					//盒饭柜操作结果

	//纸币找零循环器
	uint32_t  RecyPayoutMoney;			    //找零金额
	uint8_t   RecyPayoutNum;				//找零数量
	
} MessagePack;	
extern MessagePack MsgAccepterPack;



extern OS_EVENT	*g_DeviceSem;
extern OS_EVENT *g_BillMoneyQ;
extern OS_EVENT *g_BillMoneyMail;
extern OS_EVENT *g_BillMoneyBackMail;
extern OS_EVENT *g_CoinMoneyBackMail;
extern OS_EVENT *g_CoinMoneyQ;
extern OS_EVENT *g_CoinMoneyMail;
extern OS_EVENT *g_ChangeMoneyMail;
extern OS_EVENT *g_ChangeBackMoneyMail;
extern OS_EVENT *g_ReaderMoneyMail;
extern OS_EVENT *g_ReaderBackMoneyMail;
extern OS_EVENT *g_ReaderBackCmdMail;
extern OS_EVENT *g_ChannelMail;
extern OS_EVENT *g_ChannelBackMail;
//ACDC控制邮箱
extern OS_EVENT *g_ACDCMail;
extern OS_EVENT *g_ACDCBackMail;

extern OS_EVENT *g_LiftTableMail;
extern OS_EVENT *g_LiftTableBackMail;

//盒饭柜专用邮箱  add by liya 2014-01-20
extern OS_EVENT *g_HeFanGuiMail;
extern OS_EVENT *g_HeFanGuiBackMail;

/******************************************设备状态通讯结构体*****************************************************/
//纸币器收到纸币返回面值
#define MBOX_GETDEVSTATE					0x01
#define MBOX_RETURNDEVSTATE					0x02

typedef struct
{
	uint8_t   DevStateCmd;						//投币命令类型
	//货道板故障
	unsigned char ErrorInit;                    //是否正在初始化,0正在初始化,1初始化完成
	//MDB纸币器
	uint8_t BillCommunicate;//1代表纸币器通讯不上
	uint8_t Billmoto;//1代表马达故障
	uint8_t Billsensor;//1代表传感器故障
	uint8_t Billromchk;//1代表rom出错
	uint8_t Billjam;//1代表投币卡币
	uint8_t BillremoveCash;//1移除纸币钞箱
	uint8_t BillcashErr;//1纸币钞箱故障
	uint8_t Billdisable;//1纸币因为各种原因被禁能了

	//MDB硬币器
	uint8_t CoinCommunicate;//1代表硬币器通讯不上
	uint8_t Coinsensor;//1代表传感器故障
	uint8_t Cointubejam;//1代表出币口卡币
	uint8_t Coinromchk;//1代表rom出错
	uint8_t Coinrouting;//1代表进币通道出错
	uint8_t Coinjam;//1代表投币卡币
	uint8_t CoinremoveTube;//1移除硬币斗
	uint8_t Coindisable;//1硬币因为各种原因被vmc禁能了
	uint8_t CoinunknowError;//1硬币其他各种故障
	uint8_t CoinunknowErrorLow;//硬币其他各种故障的子状态码

	//Hopper找零器
	uint8_t Hopper1State;//Hopper1状态，0正常，1缺币，2故障
	uint8_t Hopper2State;//Hopper2状态，0正常，1缺币，2故障
	uint8_t Hopper3State;//Hopper3状态，0正常，1缺币，2故障
	uint8_t Hopper1StateChk;//Hopper1故障码
	uint8_t Hopper2StateChk;//Hopper2故障码
	uint8_t Hopper3StateChk;//Hopper3故障码

	//出货机构
	uint8_t GOCError;	    //主箱体GOC状态，0正常，1故障
	uint8_t GOCErrorSub;	//从箱体GOC状态，0正常，1故障
	uint8_t ColBoardError;	    //主箱体货道板状态，0正常，1故障
	uint8_t ColBoardErrorSub;	//从箱体货道板状态，0正常，1故障
	//取货电机
	unsigned char Error_FMD;
	//上下电机
	unsigned char Error_FUM;
	//出货确认对射板
	unsigned char Error_GOC;
	//层架检测传感器
	unsigned char Error_FLD;
	//起始位置传感器
	unsigned char Error_FOD;
	//用户取货门故障
	unsigned char Error_UDS;
	//货道板故障
	unsigned char Error_GCD;
	//出货机构忙
	unsigned char Error_SOVB;
	//机器大门故障
	unsigned char Error_SOMD1;
	//收到错误数据包
	unsigned char Error_SOEC;
	//升降台正确取货
	unsigned char Error_SFHG;
	//货柜门是否打开
	unsigned char Error_SOFP;
	//取货门
	unsigned char Error_SOMD2;
	//主箱体是否已经全部都不可用
	unsigned char Emp_Gol;//1不可用,0可用
}DEVICESTATEPACK;
extern DEVICESTATEPACK DeviceStatePack;
extern DEVICESTATEPACK DeviceStateBusiness;


extern OS_EVENT *g_DeviceStateMail;
extern OS_EVENT *g_DeviceStateBackMail;


/******************************************Hopper通讯结构体*****************************************************/
//三个地址Hopper的出币金额保存地址
#define ADD_HP1					(32)
#define ADD_HP2					(64)
#define ADD_HP3					(96)
//三个地址Hopper的单位金额存储地址
#define ADD_HPVALUE1			(1)
#define ADD_HPVALUE2			(2)
#define ADD_HPVALUE3			(3)
//地址为00的Hopper类型
#define EVBDEV_HP1						(1)					
//地址为01的Hopper类型			
#define EVBDEV_HP2						(2)					
//地址为10的Hopper类型
#define EVBDEV_HP3						(3)					

//Hopper的查询操作指令
#define EVBHANDLE_HPQUERY				(0x51)
//Hopper的出币操作指令				
#define EVBHANDLE_HPOUTPUT				(0x50)				

struct EvbHopper
{
	//硬件连接上得索引编号
	unsigned char Index;
	//Hp排序列表中的编号
	unsigned char Num;
	//上次通信后得到的状态:0,可用;2,缺币;3,故障;
	//返回的信息：0x00:正常;0x20:缺币;0x11:出币时卡币了；0x39:光侧无币时出不了币；0x41:光侧有币时出币出不了币
	unsigned char State;
	//面值
	unsigned short Price;
	//物理固定地址
	unsigned char	Add; 
	//上次操作出币数量
	unsigned char OutPutCount;
	//Hp通讯故障不可用标志
	unsigned char DevBadFlag;
	unsigned int MemoryAdd;
	//Hp故障代码
	//返回的信息：0x00:正常;0x80:通讯失败;0x01:单价为0;0x20,0x29:缺币;0x11:出币时卡币了；0x28也是一种故障,0x39:光侧无币时出不了币；0x41:光侧有币时出币出不了币
	unsigned char StateChk;
};
//定义三个地址Hopper的结构体
extern struct EvbHopper stEvbHp[3];
//Hopper找零单位面值大小设置标志：1：三个Hopper的面值不相等；2：000地址Hopper与001地址Hopper地址面值相等，与010不相等
//							  	  3：000地址Hopper与010地址Hopper地址面值相等，与001不相等
//							  	  4：001地址Hopper与010地址Hopper地址面值相等，与000不相等	
//							  	  5：三个Hopper的面值相等
extern unsigned char HpValueFlag;


//add by yoc 2013.12.16
/***********************************一鸣智慧*PC通讯结构体*****************************************************/

//上发 表示VMC _PC  下发表示 PC_VMC
//上发启动一鸣智慧
#define MBOX_VMC_ZH_START		    	0x01
//上发启动GPRS
#define MBOX_VMC_GPRS_START		   		0x02
//vmc上发给PC机投入硬币
#define MBOX_VMC_ZH_COIN		        0x03
//vmc上发给PC机投入纸币
#define MBOX_VMC_ZH_BILL 		        0x04
//vmc上发给PC机暂存纸币
#define MBOX_VMC_ZH_ESCROWIN 		    0x05
//vmc上发给PC机暂存纸币出币
#define MBOX_VMC_ZH_ESCROWOUT		    0x06
//vmc上发给PC机找零	  即 
#define MBOX_VMC_ZH_PAYOUT     		    0x07

//上发离线交易数据
#define MBOX_VMC_ZH_OFFLINE_REPORT		0x08
//上发出货信息
#define MBOX_VMC_ZH_TRADE			 0x09

//上发按键 选货
#define MBOX_VMC_ZH_BUTTON			0x0B
//上发进入维护模
#define MBOX_VMC_ZH_WEIHU			0x0C
//上发离开维护模式					
#define MBOX_VMC_ZH_NORMAL			0x0D
//清零
#define MBOX_VMC_ZH_CLEAR 			0x0E
//上发GPRS同步数据
#define MBOX_VMC_GPRS_CLEAR			0x0F

//上发ACK
#define MBOX_VMC_ZH_ACK			0x10
//上发NAK
#define MBOX_VMC_ZH_NAK			0x11
//一次交易结束
#define MBOX_VMC_ZH_TRADE_OVER 	0x12
//上发人体接近
#define MBOX_VMC_ZH_HUMAN	   0x13
//上发大门打开
#define MBOX_VMC_ZH_DOOR_OPEN   0x14
//上发大门关闭
#define MBOX_VMC_ZH_DOOR_CLOSE   0x15



//下发离线状态
#define MBOX_ZH_VMC_OFFLINE			   0x01
//下发在线状态
#define MBOX_ZH_VMC_ONLINE			   0x02
//下发出货指令
#define MBOX_ZH_VMC_TRADE			   0x03
//下发清零完成标志
#define MBOX_ZH_VMC_CLEAR_OK		 0x04

/****************************************************************************
*
*add by yoc 2013.8.29
****************************************************************************/
//一鸣智慧通信邮箱
typedef struct _message_zhihui{

	INT8U  vmc_type;//发给智慧的邮箱类型
	INT8U  zh_type;//发给主控板的邮箱类型
	//本次交易信息	
	INT8U  zh_status;//一鸣智慧状态1表示在线 0 离线	
	INT16U trade_index;//当前的交易记录保存索引
	INT8U  usr_coming;//是否是第一次
	INT8U  escrowInFlag;//暂存纸币
	INT16U  recv_amount; //单次投入金额
	INT16U  recv_all_amount;//总投入金额
	INT16U 	changed;//本次交易找零的金额
	INT8U  cur_logic_no;//本次交易的货道逻辑编号
	INT16U  cur_goods_price;//本次交易的商品单价
	INT8U  cur_goods_sn;//本次交易的商品编号
	INT8U  cur_trade_state;//出货操作结果
	
	INT8U  pay_mode;//本次交易支付方式
	
	//设置商品序号
	INT8U aisle_index;//物理货道 索引
	INT8U goodsID;//商品序号 十进制
	INT8U humanState;//人体感应状态
	
}MESSAGE_ZHIHUI;

//extern MESSAGE_ZHIHUI vmc_msg_pc;//发送PC 的邮箱
extern OS_EVENT *g_PCMail;
extern OS_EVENT *g_PCBackMail;
extern unsigned char ZhIndex;//add by yoc 2013.10.10
extern MESSAGE_ZHIHUI vmc_zh_mbox[MBOX_ST_LEN];//add by yoc 2013.12.02
extern MESSAGE_ZHIHUI zh_vmc_mbox;

/******************************************end PC通讯结构体*****************************************************/



/******************************************UboxPC通讯结构体*****************************************************/
//vmc下发给PC机启动签到命令
#define MBOX_VMC_UBOXINITDEV			    0x01
//vmc下发给PC机投入硬币
#define MBOX_VMCTOPC_PAYINCOIN		        0x02
//vmc下发给PC机投入纸币
#define MBOX_VMCTOPC_PAYINBILL 		        0x03
//vmc下发给PC机暂存纸币
#define MBOX_VMCTOPC_PAYINESCROWIN 		    0x04
//vmc下发给PC机暂存纸币出币
#define MBOX_VMCTOPC_PAYINESCROWOUT		    0x05
//vmc下发给PC机找零
#define MBOX_VMCTOPC_PAYOUT     		    0x06
//pc上发给vmc机找零指令
#define MBOX_PCTOVMC_PAYOUTIND     		    0x07
//vmc下发给PC同意ACK
#define MBOX_VMCTOPC_ACK     		        0x08
//vmc下发给PC拒绝NAK
#define MBOX_VMCTOPC_NAK    		        0x09
//pc上发给vmc机扣款指令
#define MBOX_PCTOVMC_COSTIND     		    0x0A
//vmc下发给PC机扣款
#define MBOX_VMCTOPC_COST       		    0x0B
//vmc下发给PC机按键
#define MBOX_VMCTOPC_BUTTON       		    0x0C
//vmc下发给PC机出货信息
#define MBOX_VMCTOPC_VENDOUT       		    0x0D
//pc上发给vmc机出货指令
#define MBOX_PCTOVMC_VENDOUTIND     		0x0E
//pc上发给vmc机重新复位指令
#define MBOX_PCTOVMC_RESETIND     		    0x0F
//pc上发给vmc机得到设备状态指令
#define MBOX_PCTOVMC_STATUSIND     		    0x10
//向PC机上传设备状态指令
#define MBOX_VMCTOPC_STATUS     		    0x11
//向PC机上传Action信息
#define MBOX_VMCTOPC_ACTION     		    0x12
//pc上发给vmc机CONTROL指令
#define MBOX_PCTOVMC_CONTROLIND     	    0x13
//向PC机上传Admin信息
#define MBOX_VMCTOPC_ADMIN      		    0x14
//pc上发给vmc机得到Info指令
#define MBOX_PCTOVMC_INFOIND     		    0x15
//向PC机上传相应的Info指令
#define MBOX_VMCTOPC_INFORPT     		    0x16




typedef struct
{
	//纸币器
    uint8_t	PCCmd;						//vmc下发给PC控制命令类型
	uint8_t	PCBackCmd;					//PC机上传给vmc命令控制的返回类型	
	
	uint16_t payInMoney;                //vmc下发给PC投币金额
	uint16_t payoutMoney;                //vmc下发给PC找零总金额
	uint32_t payAllMoney;               //vmc下发给PC总共投入的金额
	uint8_t  payoutDev;					//vmc下发给PC找零设备
	
	uint32_t changeMoney;               //pc上传给vmc需要找零的金额
	uint32_t costMoney;                 //pc上传给vmc需要扣款的金额
	uint8_t Type;						//type协议字节的值
	uint8_t Column;						//vmc下发给PC出货货道
	uint8_t status;                     //vmc下发给PC出货结果
	uint8_t ColumnSum;                  //vmc下发给PC本货道剩余商品个数
	uint8_t method;                     //pc上传给vmc出货方式1通过商品id出货,2通过货道id出货
	uint8_t device;                     //pc上传给vmc出货箱柜号

	//向PC机下发设备状态
	uint8_t check_st;                   //"出货检测"设备：0=正常，1=被软件禁用，2=故障，3=不支持出货检测功能
	uint8_t bv_st;                      //纸币器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t cc_st;                      //硬币器状态（包含找零功能）：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t vmc_st;                     //VMC状态（代表整个售货机）：0=正常，1= 正常货道商品全部售空，或非售卖时间，2=故障，3=维护模式
	uint16_t change;                    //找零量总金额
	uint8_t tem_st;                     //货仓状态设置值，共支持4个货仓
	uint16_t coinSum[6];                //每种通道硬币的存币数量
	uint16_t recyclerSum[6];            //每种通道纸币的存币数量

	uint8_t action;                     //Action动作
	uint8_t value;                      //Action动作的值
	uint8_t second;                     //出货或者出币预计时间
	//时间
	uint16_t RTCyear;                     //年
	uint8_t  RTCmonth;                    //月
	uint8_t  RTCday;                      //日
	uint8_t  RTChour;                     //时
	uint8_t  RTCmin;                      //分
	uint8_t  RTCsecond;                   //秒
	uint8_t  RTCweek;                     //周	
	//向vmc发盒饭柜控制指令
	uint8_t  Control_device;              //箱柜号（3 – N）
	uint8_t  Control_type;                //控制类型0x53 ：表示加热控制 0x55：表示制冷控制 0x56表示整机照明控制
	uint8_t  Control_Huodao;              //箱柜的货道号 
	uint8_t  cabinetNums;                 //目前非普通柜的数量 
	uint8_t  cabinetdata[30];             //所有非普通柜状态
	uint16_t wenduvalue;                  //温度设置的值
	
	
} MessageUboxPCPack;	
#define UBOX_SIZE 7
extern MessageUboxPCPack MsgUboxPack[UBOX_SIZE];

extern uint8_t  g_Ubox_Index;

extern OS_EVENT *g_Ubox_VMCTOPCQ;
extern OS_EVENT *g_Ubox_VMCTOPCBackCMail;
extern OS_EVENT *g_Ubox_PCTOVMCQ;
extern OS_EVENT *g_Ubox_PCTOVMCBackQ;


/******************************************展示位通讯结构体*****************************************************/
//启动选货按键板命令
#define MBOX_SELECTKEYINIT			0x01
//vmc下发给选货按键板按键灯亮命令
#define MBOX_SELECTLIGHT			0x02
//选货按键板上传按键值
#define MBOX_SELECTVALUE		    0x03

typedef struct	
{
	uint8_t	KeyCmd;						//vmc下发给选货按键板控制命令类型
	uint8_t	KeyBackCmd;					//选货按键板上传给vmc命令控制的返回类型
	//选货按键;by gzz 20121025
	uint8_t  sel1ReadyLed;               //按键板1的√灯
	uint8_t  sel1ErrLed;                 //按键板1的×灯
	uint8_t  sel2ReadyLed;               //按键板2的√灯
	uint8_t  sel2ErrLed;                 //按键板2的×灯
	uint8_t  sel3ReadyLed;               //按键板3的√灯
	uint8_t  sel3ErrLed;                 //按键板3的×灯
	uint8_t  sel4ReadyLed;               //按键板4的√灯
	uint8_t  sel4ErrLed;                 //按键板4的×灯
	uint8_t  sel5ReadyLed;               //按键板5的√灯
	uint8_t  sel5ErrLed;                 //按键板5的×灯
	uint8_t  sel6ReadyLed;               //按键板6的√灯
	uint8_t  sel6ErrLed;                 //按键板6的×灯
	uint8_t  selectMode;                 //模式,0空闲状态,1交易状态
	uint8_t	 selectInput;                //按键值
}MessageKEYPack;
extern MessageKEYPack MsgKEYPack;

extern OS_EVENT *g_KEYMail;

typedef struct	
{
	uint8_t	KeyBackCmd;					//选货按键板上传给vmc命令控制的返回类型	
	uint8_t	 selectInput;                //按键值
}MessageKEYPackBack;
extern MessageKEYPackBack MsgKEYBackPack;
extern OS_EVENT *g_KEYBackMail;

/******************************************温控器通讯结构体*****************************************************/
//设置温度命令
#define MBOX_XMTSETTEMP 			0x01
//设置温度成功
#define MBOX_XMTTEMPOK			    0x02
//设置温度失败
#define MBOX_XMTTEMPFAIL		    0x03

typedef struct	
{
	uint8_t	KeyCmd;						//vmc下发温度命令
	uint8_t	KeyBackCmd;					//温控器上传给vmc设置结果
	//选货按键;by gzz 20121025
	uint16_t  temparater;            	   //温度数量
}MessageXMTPack;
extern MessageXMTPack MsgXMTPack;

extern OS_EVENT *g_XMTMail;
extern OS_EVENT *g_XMTBackMail;



/******************************************货道参数*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRC校验
} GOODSCHANNELPARAMETER;
/******************************************交易参数*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRC校验
} VENDINGPARAMETER;

/******************************************交易记录*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRC校验
} VENDINGLOG;

/******************************************位图格式*****************************************************/
typedef struct
{
	unsigned char Width;			//位图宽度
	unsigned char Hight;			//位图高度
	const unsigned char *Data;		//位图数据
} BMP;
/******************************************定时器设置*****************************************************/
typedef struct	
{
	uint8_t		PayoutTimer;					//出币等待定时器
	uint8_t		EscrowTimer;					//压抄等待定时器
	uint8_t		DispFreeTimer;					//交易界面空闲界面刷新定时器
	uint8_t		CheckDevStateTimer;				//检测设备故障状态定时器
	uint8_t		GetTubeTimer;					//检测设备故障状态定时器
	uint8_t		ChaxunTimer;					//交易界面输入货道查询定时器
	uint8_t		SaleTimer;					    //交易倒计时定时器
	uint8_t		PCRecTimer;					    //PC串口接收定时器
	uint8_t		ActionPCTimer;					//UBOX的维护模式下Action定时器
	uint8_t		offLinePCTimer;				    //UBOX的离线模式下定时器
	uint8_t     printTimer;                     //打印交易记录计时器
	uint8_t		selTimer;						//选货按键接收定时器

	//add by yoc zhihui 2013.12.16
	uint8_t    	zhihui_recv_timeout;
	uint8_t		zhihui_button_timeout;
	uint8_t     zhihui_trade_time;
	uint8_t		pcm_handle_timeout;	//售货机处理PC请求超时 add by yoc newAisle
	
	uint8_t		getTempTimer;				  //读取温度定时器
} TIMER;
extern TIMER Timer;

/******************************************全局变量参数*****************************************************/
typedef struct	
{
	uint8_t pcInitFlag;              
}GLOBALSYS;
extern GLOBALSYS globalSys;

/******************************************函数声明*****************************************************/
void CreateMBox(void);
void LoadMechineSerialNo(unsigned char *Data);
void FactoryDefaultSystemPara(void);
void RstFactoryDefaultSystemPara(void);
uint8_t LoadDefaultSystemPara(uint8_t backup);
void WriteDefaultSystemPara(SYSTEMPARAMETER SysPara,uint8_t backup);
void ResetSystemPara(void);
uint8_t LoadUserSystemPara(uint8_t backup);
void WriteUserSystemPara(SYSUSERPARAMETER UPara,uint8_t backup);

unsigned char XorCheck(unsigned char *pstr,unsigned short len);
unsigned short CrcCheck(unsigned char *msg, unsigned short len);
char *PrintfMoney(uint32_t dispnum);

#endif
/**************************************End Of File*******************************************************/
