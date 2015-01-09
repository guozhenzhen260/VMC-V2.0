#ifndef _CHANNELACCEPTER_H
#define _CHANNELACCEPTER_H


//出货操作
#define CHANNEL_OUTGOODS		(1)
//检测货道当前状态			
#define CHANNEL_CHECKSTATE		(2)
//检测货道上次的出货结果			
#define CHANNEL_CHECKOUTRESULT	(3)	
//清楚sn
#define CHANNEL_CLEARSN			(4)


//重置
#define MACHINE_RESET			(5)
//开启照明灯
#define MACHINE_LIGHTOPEN		(6)
//关闭照明灯
#define MACHINE_LIGHTCLOSE		(7)
//开门
#define MACHINE_OPENDOOR		(8)
//关门
#define MACHINE_CLOSEDOOR		(9)
//获得机器数据
#define MACHINE_INFORMATION		(10)	
	
//出货
#define OUTGOODSCMD				(0x70)
//查询当前状态			
#define CHECKCHANNEL			(0x71)
//清楚SN
#define CLEARSN					(0x72)
//查询出货结果				
#define CHECKOUTRESULT			(0x73)	


struct ChannelErrForm
{
	//出货确认设备故障状态位
	unsigned char GOCErrFlag[2];
	//货道板故障状态位
	unsigned char HuoDaoBanFlag[2];
};
//货道故障列表
extern struct ChannelErrForm stChannelErrForm;		


unsigned char ChannelHandle(unsigned char Handletype,unsigned char Add,unsigned char CheckFlag,unsigned char Binnum);





#endif
