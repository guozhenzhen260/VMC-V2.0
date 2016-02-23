/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LiftTableAPI.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        升降机控制API接口                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"

#define 	LIFT_HANDLE_TIMEOUT			(5000) //50秒
#define     LIFT_HANDLE_TIMEOUT1         (30)

static   unsigned char   isReady = 1;

/*****************************************************************************
** Function name:	LiftTableHandle	
** Descriptions:	给设备管理任务发送升降机控制邮箱														 			
** parameters:		HandleType:操作类型1:出货操作 2:检查状态 3:查询出货结果
					ChannelNum:逻辑货道编号
					LevelNum:层编号
					Binnum:箱柜编号
					BackMsg:返回的数据
** Returned value:	0:成功;1:失败;
*****************************************************************************/
unsigned char LiftTableHandle(unsigned char HandleType,unsigned char ChannelNum,unsigned char LevelNum,unsigned char Binnum,unsigned char *BackMsg)
{
	MessagePack *RecvBoxPack;
	uint8_t err,i;
	unsigned int timeout1 = 0;

	MsgAccepterPack.ChnlHandleType = HandleType;
	MsgAccepterPack.ChannelNum = ChannelNum;
	MsgAccepterPack.Binnum = Binnum;
	MsgAccepterPack.LevelNum = LevelNum;
	TraceChannel("LiftTableHandle[start]:type = %d\r\n",HandleType);
	liftSetReady(0);
	OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
	if(HandleType == CHANNEL_OUTGOODS){
		for(i = 0;i < 5;i++){
			RecvBoxPack = OSMboxPend(g_LiftTableBackMail,60000,&err);
			if(err == OS_NO_ERR){
				break;
			}
		}
	}
	else{
		RecvBoxPack = OSMboxPend(g_LiftTableBackMail,LIFT_HANDLE_TIMEOUT,&err);
		
	}
	liftSetReady(1);
	if(err == OS_NO_ERR){
		TraceChannel("LiftTableHandle[pend]:type=%d\r\n",HandleType);
		for(i=0;i<10;i++){
			BackMsg[i] = RecvBoxPack->LiftTablePack[i];
		}
		return RecvBoxPack->HandleResult;
	}
	TraceChannel("LiftTableHandle[pend]:timeout!\r\n");
	return 0xFF;


}



/*****************************************************************************
** Function name:	LiftTableHandle1	
** Descriptions:	给设备管理任务发送升降机控制邮箱														 			
** parameters:		HandleType:操作类型1:出货操作 2:检查状态 3:查询出货结果
					ChannelNum:逻辑货道编号
					LevelNum:层编号
					Binnum:箱柜编号
					BackMsg:返回的数据
** Returned value:	0:成功;1:失败;
*****************************************************************************/
unsigned char LiftTableHandle1(unsigned char HandleType,unsigned char ChannelNum,unsigned char LevelNum,unsigned char Binnum,unsigned char *BackMsg)
{
	MessagePack *RecvBoxPack;
	uint8_t err,i;

	MsgAccepterPack.ChnlHandleType = HandleType;
	MsgAccepterPack.ChannelNum = ChannelNum;
	MsgAccepterPack.Binnum = Binnum;
	MsgAccepterPack.LevelNum = LevelNum;
	TraceChannel("LiftTableHandle[start]:type = %d\r\n",HandleType);
	OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
	RecvBoxPack = OSMboxPend(g_LiftTableBackMail,LIFT_HANDLE_TIMEOUT1,&err);
	if(err == OS_NO_ERR){
		TraceChannel("LiftTableHandle[pend]:type=%d\r\n",HandleType);
		for(i=0;i<10;i++){
			BackMsg[i] = RecvBoxPack->LiftTablePack[i];
		}
		return RecvBoxPack->HandleResult;
	}
	TraceChannel("LiftTableHandle[pend]:timeout!\r\n");
	return 0xFF;


}



unsigned char TestLiftTablePost(unsigned char HandleType,unsigned char ChannelNum,unsigned char LevelNum,unsigned char Binnum)
{
	switch(HandleType)
	{
		case 1:
			MsgAccepterPack.ChnlHandleType = HandleType;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.Binnum = Binnum;
			MsgAccepterPack.LevelNum = LevelNum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
			break;
		case 2:
			MsgAccepterPack.ChnlHandleType = HandleType;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.Binnum = Binnum;
			MsgAccepterPack.LevelNum = LevelNum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
			break;
		case 3:
			MsgAccepterPack.ChnlHandleType = HandleType;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.Binnum = Binnum;
			MsgAccepterPack.LevelNum = LevelNum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
			break;
		case 4:
			MsgAccepterPack.ChnlHandleType = HandleType;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.Binnum = Binnum;
			MsgAccepterPack.LevelNum = LevelNum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_LiftTableMail,&MsgAccepterPack);
			break;
	}
	return 1;
}


unsigned char TestLiftTablePend(unsigned char *BackMsg)
{
	MessagePack *RecvBoxPack;
	uint8_t err,i;

	//等待邮箱返回
	RecvBoxPack = OSMboxPend(g_LiftTableBackMail,50/5,&err);
	if(err == OS_NO_ERR)
	{
		TraceChannel("BackMsg=\r\n");
		for(i=0;i<10;i++)
		{
			BackMsg[i] = RecvBoxPack->LiftTablePack[i];
			TraceChannel("%x ",BackMsg[i]);
		}
		return RecvBoxPack->HandleResult;
	}
	return 0xff;	
}



void liftSetReady(unsigned char flag)
{
	isReady = (flag == 0 ? 0 : 1);
}



unsigned char liftIsReady(void)
{
	return (isReady == 1);
}


/*****************************************************************************
** Function name:	liftTableLight	
** Descriptions:		控制升降机柜内照明											 			
** parameters:		type 1-开灯  0-关灯
** Returned value:	1:成功;0:失败;
*****************************************************************************/

unsigned char liftTableLight(unsigned char cabinet,unsigned char type)
{
	unsigned char ack[36] = {0};
	unsigned char opt  = ((type == 1) ?  MACHINE_LIGHTOPEN: MACHINE_LIGHTCLOSE);
	if(!liftIsReady())
		return 0;
	return LiftTableHandle(opt,0,0,cabinet,ack);
}




/*****************************************************************************
** Function name:	liftTableChuchou	
** Descriptions:		控制升降机柜内除臭											 			
** parameters:		type 1-开灯  0-关灯
** Returned value:	1:成功;0:失败;
*****************************************************************************/

unsigned char liftTableChuchou(unsigned char cabinet,unsigned char type)
{
	unsigned char ack[36] = {0};
	unsigned char opt  = ((type == 0) ?  0: 1);
	if(!liftIsReady())
		return 0;
	return LiftTableHandle(VMC_CHUCHOU_REQ,opt,opt,cabinet,ack);
}












