#ifndef _LIFTTABLE_H
#define _LIFTTABLE_H

//出货操作
#define LIFTTABLE_OUTHANDLE				(1)
//查询当前状态
#define LIFTTABLE_CHECKHANDLE			(2)
//清楚SN
#define LIFTTABLE_OUTGOODSRST			(3)
//查询货物是否取走
#define LIFTTABLE_TAKEGOODS				(4)



void LiftTableTest(unsigned char binnum);

unsigned char LiftTable1Process(unsigned char Binnum,unsigned char ChannelNum,unsigned char HandleType);
unsigned char LiftTable2Process(unsigned char Binnum,const unsigned char ChannelNum,unsigned char HandleType);

unsigned char LiftTableProcess(unsigned char Binnum,unsigned char ChannelNum,unsigned char HandleType);
unsigned char LiftTableTestByPcPost(unsigned char Bin,unsigned char LogicNum,unsigned char HandleType);
unsigned char LiftTableTestByPcPend(unsigned char HandleType);
#endif
