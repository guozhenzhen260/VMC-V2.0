#ifndef _CHANNEL_H
#define _CHANNEL_H


#define CHECKCOUNT			(1)
#define CHECKPRICE			(2)


//新UI货道超级管理结构体  create by yoc
typedef struct _ui_aisle_admin_{

	char *cabinetAdminMenu[MAXLANGUAGETYPE];//"货道属性设置"
	char *enternCabinetNo[MAXLANGUAGETYPE];//" 输入箱柜编号: "
	char *A_Bin[MAXLANGUAGETYPE];//" A 柜"
	char *B_Bin[MAXLANGUAGETYPE];//" B 柜"
	char *hefan[MAXLANGUAGETYPE];//"盒饭柜"
	char *columnInit[MAXLANGUAGETYPE];//" 货道初始化"
	char *columnInitOk[MAXLANGUAGETYPE];//" 初始化成功"
	char *levelManage[MAXLANGUAGETYPE];//" 层架管理"
	char *enterLevelNo[MAXLANGUAGETYPE];//" 请输入层编号:"
	char *columnmanage[MAXLANGUAGETYPE];//" 货道管理"
	char *enterColumNo[MAXLANGUAGETYPE];//" 请输入货道号: "
	char *noColumn[MAXLANGUAGETYPE];//" 货道不存在! "
	char *columnStatus[MAXLANGUAGETYPE];//" 货道状态 :"
	char *maxCount[MAXLANGUAGETYPE];//" 最大存货量:"
	char *levelState[MAXLANGUAGETYPE];//"层架状态: "
	char *open[MAXLANGUAGETYPE];//" 开启"
	char *close[MAXLANGUAGETYPE];//" 关闭"
	char *yes[MAXLANGUAGETYPE];//" 确定"
	char *cancel[MAXLANGUAGETYPE];//" 取消"
	char *no_1[MAXLANGUAGETYPE];//"①"
	char *no_2[MAXLANGUAGETYPE];
	char *no_3[MAXLANGUAGETYPE];
	char *no_4[MAXLANGUAGETYPE];
	char *no_5[MAXLANGUAGETYPE];


	//===货道用户管理
	char *columnUserManage[MAXLANGUAGETYPE];//"货道参数设置"
	char *columnAtrib[MAXLANGUAGETYPE];//"货道属性"
	char *columnAdd[MAXLANGUAGETYPE];//"货道添货"
	char *columnLook[MAXLANGUAGETYPE];//"货道查看"
	char *columnTest[MAXLANGUAGETYPE];//"货道测试"
	char *columnKaoji[MAXLANGUAGETYPE];//"拷机查询"
	char *columnPrice[MAXLANGUAGETYPE];//" 货道商品单价 :"
	char *columnCount[MAXLANGUAGETYPE];//" 货道储货数量 : "
	char *columnSuc[MAXLANGUAGETYPE];//" 成功交易次数: " 
	char *columnSelectNo[MAXLANGUAGETYPE];//"选货按键号"
	char *columnID[MAXLANGUAGETYPE];//"商品编号"
	char *columnOneAdd[MAXLANGUAGETYPE];//"单货道添货",
	char *columnLevelAdd[MAXLANGUAGETYPE];//"层货道添货",
	char *columnAllAdd[MAXLANGUAGETYPE];//"全货道添货",
	char *columnSelect[MAXLANGUAGETYPE];//"请选择操作:",
	char *sureAddLevel[MAXLANGUAGETYPE];//"确定要添加该层?",
	char *columnCheck[MAXLANGUAGETYPE];
	char *columnBad[MAXLANGUAGETYPE];
	char *columnEmpty[MAXLANGUAGETYPE];
	char *columnPriceZero[MAXLANGUAGETYPE];
	char *columnlOK[MAXLANGUAGETYPE];


	char *column99Test[MAXLANGUAGETYPE];//"全货道测试页面"
	char *column98Test[MAXLANGUAGETYPE];//"多层货道测试页面"
	char *column97Test[MAXLANGUAGETYPE];//"多个货道测试页面"
	char *columnOneTest[MAXLANGUAGETYPE];//"单货道测试页面"
	
	char *columnCheckWait[MAXLANGUAGETYPE];
	char *columnTestEmpty[MAXLANGUAGETYPE];
	char *columnTestOK[MAXLANGUAGETYPE];
	char *columnTestBad[MAXLANGUAGETYPE];
	char *columnTestMOTO[MAXLANGUAGETYPE];
	char *columnTestNA[MAXLANGUAGETYPE];
	char *columnCommunFail[MAXLANGUAGETYPE];
	
	
}UI_AISLE_ADMIN;

extern const UI_AISLE_ADMIN ui_aisle_admin;


void ChannelTest(unsigned char cabinetNo);

void ChannelManage(void);

void ChannelParamSet(unsigned char cabinetNo);

void ChannelAddGoods(unsigned char cabinetNo);

void ChannelLeaveFactory(void);

void ChannelAttributeManage(void);

void ChannelClearSuccesCount(void);

void ChannelLookUpSuccessCount(void);

unsigned int ChannelGetMaxGoodsPrice(unsigned char Binnum);

unsigned char ChannelReadChannelParam(unsigned char Bin,unsigned char logicnum);

unsigned char ChannelCheckCountOrPrice(unsigned char Logicnum,unsigned char Binnum,unsigned char Type,unsigned int price);

unsigned char ChannelAPIProcess(unsigned char LogicNum,unsigned char HandleType,unsigned char Binnum);
unsigned char TestColumnByPC(unsigned char Bin,unsigned LogicNum);
unsigned char ColumnTestByPcPost(unsigned char Bin,unsigned LogicNum);
unsigned char ColumnTestByPcPend(void);
void ChannelSomeTest(unsigned char cabinetNo,unsigned char *columnBuf,unsigned char columnNum,unsigned char titleType);
void ChannelKaojiClear(unsigned char cabinet);
#endif
