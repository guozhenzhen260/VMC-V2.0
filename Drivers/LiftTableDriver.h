#ifndef _LIFTTABLEDRIVER_H
#define _LIFTTABLEDRIVER_H

//VMC通知GCC复位初始化
#define VMC_RESET_REQ				(0x80)
//VMC通知GCC报告GCC状态
#define VMC_STATUS_REQ				(0x81)
//VMC通知GCC出货
#define VMC_VENDING_REQ				(0x82)
//VMC通知GCC报告出货情况
#define VMC_VENDINGRESULT_REQ		(0x83)
//VMC通知GCC开门
#define VMC_OPENDOOR_REQ			(0x84)
//VMC通知GCC关门
#define VMC_CLOSEDOOR_REQ			(0x85)
//VMC通知GCC打开照明灯
#define VMC_OPENLIGHT_REQ			(0x86)
//VMC通知GCC关闭照明灯
#define VMC_CLOSELIGHT_REQ			(0x87)
//VMC通知GCC报告配置参数
#define VMC_GETINFO_REQ				(0x88)

#define VMC_CHUCHOU_REQ				(0x8A)

#define GCC_STATUS_ACK				(0x01)
//GCC通知VMC出货
#define GCC_VENDING_ACK				(0x02)
//GCC通知VMC报告出货情况
#define GCC_VENDINGRESULT_ACK		(0x03)
//GCC通知VMC开门
#define GCC_OPENDOOR_ACK			(0x04)
//GCC通知VMC关门
#define GCC_CLOSEDOOR_ACK			(0x05)
//GCC通知VMC打开照明灯
#define GCC_OPENLIGHT_ACK			(0x06)
//GCC通知VMC关闭照明灯
#define GCC_CLOSELIGHT_ACK			(0x07)
//GCC通知VMC报告配置参数
#define GCC_GETINFO_ACK				(0x08)

#define GCC_CHUCHOU_ACK				(0x0A)

#define GCC_RESET_ACK               (0x00)


typedef struct 
{
	//机器类型
	unsigned char Magic;
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
	//整机状态 0：正常   1：故障
	unsigned char Error_OVERALLUINT;
	//升降机状态 
	unsigned char Error_LIFTER;
	//取货电机状态
	unsigned char Error_TAKEMOTTOR;
	//用户电动门状态
	unsigned char Error_USERDOOR;
	//弹币机构状态
	unsigned char Error_DISPENCE;
	//防盗模块状态
	unsigned char Error_FANGDAO;
	//货道控制器状态
	unsigned char Error_COLUMN;
	//售货机配置设备参数
	//是否支持推簧	0:不支持 1：支持
	unsigned char SupportSpring;
	//是否支持照明灯
	unsigned char Light;
	//是否支持制冷
	unsigned char Cold;
	//是否支持加热
	unsigned char Hot;
	//是否支持升降
	unsigned char Lifter;
	//是否支持平推
	unsigned char Flat;
	//是否支持履带
	unsigned char ApronWheel;
}STLIFTTABLE;

extern STLIFTTABLE stLiftTable;


unsigned short CrcCheck(unsigned char *msg, unsigned short len) ;
unsigned char LiftTableDriver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char Level,unsigned char *Result);

unsigned char LIFT_vmcStatusReq(unsigned char bin);
#endif
