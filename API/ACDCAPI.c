/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CompressorAndLightAPI.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        温控设备和展示灯控制API接口                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"



/*****************************************************************************
** Function name:	acdc_handle_API	
**
** Descriptions:         ACDC控制函数												 			
** parameters:		cprFlag:是否开启压缩机标志  1开启  0 关闭
					ledFlag:是否开启展示灯标志  1开启  0 关闭
					Binnum:箱柜编号
** Returned value:	1:成功;0:失败;
*****************************************************************************/
unsigned char acdc_handle_API(unsigned char binnum,unsigned char ledFlag,unsigned char cprFlag)
{
	unsigned short cmd ;// 高两字节控制 展示灯 低两位控制压缩机

	TraceCompress("acdc_handle:bin= %d,led=%d,cpr=%d\r\n",binnum,ledFlag,cprFlag);
	
	if(binnum == 1 && SystemPara.Channel >= 3)
	{
		return liftTableLight(binnum,ledFlag);
	}
	else if(binnum == 2 && SystemPara.SubBinOpen == 1 && SystemPara.SubChannel >= 3)
	{
		return liftTableLight(binnum,ledFlag);
	}
	else
	{
		cmd = INTEG16(cprFlag,ledFlag);
		ACDCHandle(binnum,cmd);
	}
	
	return 1;
}

/*****************************************************************************
** Function name:	acdc_cpr_API	
**
** Descriptions:         ACDC压缩机控制函数												 			
** parameters:		cprFlag:是否开启压缩机标志  1开启  0 关闭
			
					Binnum:箱柜编号
** Returned value:	1:成功;0:失败;
*****************************************************************************/
unsigned char acdc_cpr_API(unsigned char binnum,unsigned char cprFlag)
{
	unsigned short cmd ;// 高两字节控制 展示灯 低两位控制压缩机
	unsigned char temp = 0; 
	TraceCompress("acdc_handle:bin= %d,cpr=%d\r\n",binnum,cprFlag);
	if(binnum == 1)
		temp = UserPara.LEDCtr.flag;
	else
		temp = UserPara.LEDCtr2Bin.flag;
	cmd = INTEG16(temp,cprFlag);

	ACDCHandle(binnum,cmd);
	return 1;
}

/*****************************************************************************
** Function name:	acdc_led_API	
**
** Descriptions:         ACDC照明控制函数												 			
** parameters:		ledFlag:是否开启照明标志  1开启  0 关闭
			
					Binnum:箱柜编号
** Returned value:	1:成功;0:失败;
*****************************************************************************/
unsigned char acdc_led_API(unsigned char binnum,unsigned char ledFlag)
{
	unsigned short cmd ;// 高两字节控制 展示灯 低两位控制压缩机
	unsigned char temp = 0; 
	TraceCompress("acdc_handle:bin= %d,led=%d\r\n",binnum,ledFlag);
	if(binnum == 1)
		temp = UserPara.CompressorCtr.flag;
	else
		temp = UserPara.CompressorCtr2Bin.flag;
	cmd = INTEG16(ledFlag,temp);
	if(binnum == 1 && SystemPara.Channel >= 3)
	{
		return liftTableLight(binnum,ledFlag);
	}
	else if(binnum == 2 && SystemPara.SubBinOpen == 1 && SystemPara.SubChannel >= 3)
	{
		return liftTableLight(binnum,ledFlag);
	}
	else
	{
		ACDCHandle(binnum,cmd);
	}
		
	return 1;
}



/*****************************************************************************
** Function name:	API_ACDCHandle	
**
** Descriptions:	给设备管理任务发送ACDC控制邮箱	
**					
**														 			
** parameters:		HandleType:操作类型
					ChkFlag:是否开启货到检测
					Binnum:箱柜编号
** Returned value:	0:成功;1:失败;
** 
*****************************************************************************/
unsigned char API_ACDCHandle(unsigned char flag,unsigned char binnum,unsigned char HandleType)
{
	MessagePack *RecvBoxPack;
	uint8_t err;
	uint16_t ACDCData = 0x0000;

	if(HandleType == OPENCOMPRESSORANDLIGHT)
	{
		ACDCData = 0x0101;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;	
	}
	else
	if(HandleType == CLOSECOMPRESSORANDLIGHT)
	{
		ACDCData = 0x0000;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;
	}
	else
	if(HandleType == OPENCOMPRESSOR)
	{
		if(flag==1)
			ACDCData = 0x0101;
		else
		if(flag==0)
			ACDCData = 0x0100;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;
	}
	else
	if(HandleType == OPENLIGHT)
	{
		if(flag==1)
			ACDCData = 0x0101;
		else
		if(flag==0)
			ACDCData = 0x0001;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;
	}
	else
	if(HandleType == CLOSECOMPRESSOR)
	{
		if(flag==1)
			ACDCData = 0x0001;
		else
		if(flag==0)
			ACDCData = 0x0000;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;
	}
	else
	if(HandleType == CLOSELIGHT)
	{
		if(flag==1)
			ACDCData = 0x0100;
		else
		if(flag==0)
			ACDCData = 0x0000;
		MsgAccepterPack.Binnum = binnum;
		MsgAccepterPack.ACDCHandleType = ACDCHANDLE;
		MsgAccepterPack.ACDCData = ACDCData;
	}
	OSMboxPost(g_ACDCMail,&MsgAccepterPack);
	RecvBoxPack = OSMboxPend(g_ACDCBackMail,100,&err);
	if(err == OS_NO_ERR)
	{
		return RecvBoxPack->HandleResult;
	}
	return 0;
}



/*****************************************************************************
** Function name:	acdc_hot_API	
**
** Descriptions:         ACDC控制函数												 			
** parameters:		
					Binnum:箱柜编号
** Returned value:	1:成功;0:失败;
*****************************************************************************/
unsigned char acdc_hot_API(unsigned char binnum,const unsigned short temp)
{	
	//unsigned char res = 0;
	unsigned short hotTemp;
	binnum = binnum;
	Trace("\r\n acdc_hot_API send=\r\n");
	hotTemp = (temp > ACDC_MAX_HOT_TEMP) ?  ACDC_MAX_HOT_TEMP : temp;
	return SetTemperatureAPI(hotTemp);
}






/*****************************************************************************
** Function name:	acdc_getTemp_API	
**
** Descriptions:         ACDC读取温度										 			
** parameters:		
					Binnum:箱柜编号
** Returned value:	返回温度值
*****************************************************************************/
unsigned short acdc_getTemp_API(unsigned char binnum)
{	
	binnum = binnum;
	return sysXMTMission.recPVTemp;
}





/*****************************************************************************
** Function name:	acdc_chuchou_API	
**
** Descriptions:         ACDC控制函数												 			
** parameters:		flag:是否开标志  1开启  0 关闭
					Binnum:箱柜编号
** Returned value:	1:成功;0:失败;
*****************************************************************************/
unsigned char acdc_chuchou_API(unsigned char binnum,unsigned char flag)
{
	unsigned short cmd ,res = 0;
	TraceCompress("acdc_chuchou_API:bin= %d,flag =%d\r\n",binnum,flag);
	cmd = INTEG16(flag,1);
	if(binnum == 1 && SystemPara.Channel >= 3)
	{
		return liftTableChuchou(binnum,flag);
	}
	else if(binnum == 2 && SystemPara.SubBinOpen == 1 && SystemPara.SubChannel >= 3)
	{
		return liftTableChuchou(binnum,flag);
	}
	else
	{
		res = ACDCHandle(binnum,cmd);
	}
	
	TraceCompress("res = %d\r\n",res);
	return ((res == 1) ?  1 : 0);

}



/*****************************************************************************
** Function name:	acdc_chuchou_API	
**
** Descriptions:         ACDC控制函数												 			
** parameters:		type:1 照明 2 制冷 3加热 4除臭
					cabinet:箱柜编号1主柜,2副柜
** Returned value:	0关闭  1开启  2故障  3定时 0xff不支持该设备
*****************************************************************************/
unsigned char acdc_status_API(unsigned char cabinet,unsigned char type)
{

	unsigned  char temp = 0; 
	switch(type)
	{
		case 1:
			if(cabinet == 1)
				temp = UserPara.LEDCtr.flag & 0x01;
			else
				temp = UserPara.LEDCtr2Bin.flag & 0x01;
			break;
		case 2:
			if(cabinet == 1)
				temp = UserPara.CompressorCtr.flag & 0x01;
			else
				temp = UserPara.CompressorCtr2Bin.flag & 0x01;
			break;
		case 3:
			if(cabinet == 1)
				temp = UserPara.box1Hot.flag & 0x01;
			else
				temp = UserPara.box2Hot.flag & 0x01;
		break;
		case 4:
			if(cabinet == 1)
				temp = UserPara.box1Chuchou.flag & 0x01;
			else
				temp = UserPara.box2Chuchou.flag & 0x01;
		break;
		default:break;
	}
	temp=0xff;
	return temp;

}











/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/




