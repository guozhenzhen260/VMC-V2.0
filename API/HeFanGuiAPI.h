#ifndef _HEFANGUIAPI_H
#define _HEFANGUIAPI_H

//盒饭柜开门操作
#define HEFANGUI_KAIMEN			(1)
//盒饭柜查询操作
#define HEFANGUI_CHAXUN			(2)
//盒饭柜加热开
#define HEFANGUI_JIAREKAI		(3)
//盒饭柜加热关				
#define HEFANGUI_JIAREGUAN		(4)
//盒饭柜制冷开
#define HEFANGUI_ZHILENGKAI		(5)
//盒饭柜制冷关
#define HEFANGUI_ZHILENGGUAN	(6)
//盒饭柜整机照明开
#define HEFANGUI_ZHAOMINGKAI	(7)
//盒饭柜整机照明关
#define HEFANGUI_ZHAOMINGGUAN	(8)

//======================定义盒饭柜货道结构体=================================================
#define HE_FAN_MAX_CABINET    10
#define HE_FAN_MAX_HUODAO	   88


#define HEFAN_LIGHT    0
#define HEFAN_LIGHT_OPEN    1

#define HEFAN_HOT      2
#define HEFAN_HOT_OPEN      3
#define HEFAN_COOL     4
#define HEFAN_COOL_OPEN     5
#define HEFAN_EXIST 6






#define HEFAN_HUODAO_NORMAL   1
#define HEFAN_HUODAO_FAULT   2
#define HEFAN_HUODAO_EMPTY   3
#define HEFAN_HUODAO_CLOSE   4

/*
type：1——货道单价
	  2——货道数量
	  3——货道状态
	  4——货道最大存货量
	  5——选货按键值
	  6——成功交易次数
	  7——商品编号

*/
#define HEFAN_HUODAO_PRICE  1
#define HEFAN_HUODAO_STATE  3
#define HEFAN_HUODAO_TRADE_NUM  6
#define HEFAN_HUODAO_ID     7

#define HEFAN_CABINET_NUMS   11


typedef struct _huodao_{

	unsigned short price;
	unsigned short tradeNum;//销售数量
	unsigned char  state;	//  1正常  2故障3 缺货 4 关闭
	unsigned char  id;
	

}HUODAO;


typedef struct _hefan_cabinet_{
	
	HUODAO huodao[HE_FAN_MAX_HUODAO];
	unsigned char state;// xx cc hh ll       xx 表示柜子存在与否1存在 0不存在 cc柜内制冷状态 hh柜内加热状态 ll柜内照明装填
	unsigned char huodaoNums;//货道总数

}HEFAN_CABINET;

extern HEFAN_CABINET stHefanCabinet[HE_FAN_MAX_CABINET];






extern unsigned char ChaXunRst[2];

extern unsigned char HeFanGuiSendMBox(unsigned char HandleType,unsigned char ChannelNum,unsigned char Binnum);

extern unsigned char HeFanGuiHandle(unsigned char LogicNum,unsigned char HandleType,unsigned char Binnum);
unsigned int hefanGetParaVlaue(unsigned char cabinetNo,unsigned char boxNo,unsigned char type);
unsigned char hefanSetParaVlaue(unsigned char cabinetNo,unsigned char boxNo,unsigned int value,unsigned char type);
unsigned char read_hefan_info_by_flash(void);
unsigned char hefanHuodaoCheck(unsigned char cabinetNo,unsigned char boxNo);
unsigned char setHefanCabinetState(unsigned char cabinetNo,unsigned char type,unsigned char state);
unsigned char getHefanCabinetState(unsigned char cabinetNo,unsigned char type);
unsigned char hefanOutGoodsHandle(unsigned char LogicNum,unsigned char HandleType,unsigned char Binnum);
void save_hefan_huodao_by_flash(void);
unsigned char save_hefan_huodao_info(unsigned char cabinetNo);

unsigned char hefanSetParaVlaue(unsigned char cabinetNo,unsigned char boxNo,unsigned int value,unsigned char type);
unsigned int hefanGetParaVlaue(unsigned char cabinetNo,unsigned char boxNo,unsigned char type);
unsigned char get_hefan_Index(unsigned char cabinetNo);
unsigned char getHefanHuodaotState(unsigned char cabinetNo,unsigned char boxNO);
void hefanMannage(void );

unsigned char hefanHuodaoInit(unsigned char cabinetNo);
unsigned char hefan_clear_trade(unsigned char cabinetNo);
unsigned char hefanAddgoodsHandle(unsigned char cabinetNo,unsigned  char type);


#endif
