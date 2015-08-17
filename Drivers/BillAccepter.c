/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           BILLACCEPTER
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        纸币器驱动程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-04
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"
#include "BillAccepter.h"


//当前纸币器设备的类型
//uint8_t NowBillDev = 0;
//纸币器通道面值
uint32_t nBillvalue[16]={0};

//接收纸币器通信应答数据包定时参数
//extern unsigned int MDBCONVERSATIONWAITACKTIMEOUT;
MDBBILLERROR MdbBillErr;

extern unsigned char BillRecyclerPayoutNumExpanse(unsigned int RecyPayoutMoney,unsigned char RecyPayoutNum);
extern unsigned char BillRecyclerPayoutValueExpanse(unsigned int RecyPayoutMoney,unsigned int *RecyPayoutMoneyBack);
extern void BillRecyclerTubeExpanse(void);


/*********************************************************************************************************
** Function name:       BillDevSellect
** Descriptions:       	纸币器设备选择
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevSellect()
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus;
	unsigned char i;

	//NowBillDev = SystemPara.BillValidatorType;
	if(SystemPara.BillValidatorType == 0)
	{
		//NowBillDev = SystemPara.BillValidatorType;
	}
	else
	{
		/*//检测是否是GBA纸币器
		for(i=0;i<3;i++)
		{
		
			ComStatus = GbaControl_BillAccept();
			if(ComStatus != 2)
			{
				NowBillDev = BILLDEV_RS232;
				return;
			}
			
		}*/
		for(i=0;i<10;i++)
		{
			//Trace("1\r\n");
			//检测是否是MDB纸币器
			//Reset
			ComStatus = MdbConversation(0x30,NULL,0x00,&BillRdBuff[0],&BillRdLen);
			//Trace("1=%d\r\n",ComStatus);
			if(ComStatus == 1)
			{
				//Setup
				//ComStatus = MdbConversation(0x31,NULL,0x00,&BillRdBuff[0],&BillRdLen);
				//Trace("2=%d\r\n",ComStatus);		
				//if(ComStatus == 1)
				//{
					//Trace("%d,%d,%d,%d,%d,%d,%d,%d\r\n",BillRdBuff[11],BillRdBuff[12],BillRdBuff[13],BillRdBuff[14],BillRdBuff[15],BillRdBuff[16],BillRdBuff[17],BillRdBuff[18]);
				//NowBillDev = BILLDEV_MDB;
				OSTimeDly(OS_TICKS_PER_SEC/2);
				return;
				//}
			}
			OSTimeDly(OS_TICKS_PER_SEC);
		}	
	}
}


/*********************************************************************************************************
** Function name:       BillDevInit
** Descriptions:        纸币器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      1使能失败,0使能成功
*********************************************************************************************************/
uint8_t BillDevInit()
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus,i;	
	unsigned char j;
	memset(BillRdBuff,0,sizeof(BillRdBuff));
	for(j = 0; j < 16; j++) 
	{
		 stDevValue.BillValue[j]=0;
		 //TraceBill("\r\nqqDrvChangebuf[%d] = %d,%d", j,ChangerRdBuff[j+2],stDevValue.CoinNum[j]);
	}
	//Reset
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus =MdbConversation(0x30,NULL,0x00,&BillRdBuff[0],&BillRdLen);
		OSTimeDly(OS_TICKS_PER_SEC);
	}
	//OSTimeDly(OS_TICKS_PER_SEC*3);
	TraceBill("\r\n DrvBILLRESET=%d",ComStatus);
	LCDNumberFontPrintf(40,LINE15,2,"BillAccepter-1");	
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n DrvBILLPOLL=%d",ComStatus);
	LCDNumberFontPrintf(40,LINE15,2,"BillAccepter-2");	
	//setup
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x31,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n DrvBILLSETUP=%d",ComStatus);	
	LCDNumberFontPrintf(40,LINE15,2,"BillAccepter-3");	
	if(ComStatus == 1)
	{
		stDevValue.BillLevel = BillRdBuff[0];
		stDevValue.BillCode = ((((uint16_t)BillRdBuff[1]) << 8) | BillRdBuff[2]);
		stDevValue.BillDecimal = 100;
	    for(i = 0; i < BillRdBuff[5]; i++) 
	    {
		   stDevValue.BillDecimal /= 10;
	    }
		TraceBill("\r\nDrvBILLDec=%d,%d,%d",BillRdBuff[3],BillRdBuff[4],BillRdBuff[5]);
		stDevValue.BillScale = (uint32_t)((((uint16_t)BillRdBuff[3]) << 8) | BillRdBuff[4]) * stDevValue.BillDecimal;
		TraceBill("\r\nDrvBILLDec2=%ld,%ld,%ld", ((((uint16_t)BillRdBuff[3]) << 8) | BillRdBuff[4]),stDevValue.BillDecimal,stDevValue.BillScale);
		//stDevValue.Decimal = BillRdBuff[5];
		stDevValue.BillStkCapacity = ((((uint16_t)BillRdBuff[6]) << 8) | BillRdBuff[7]);
		stDevValue.BillSecurity = ((((uint16_t)BillRdBuff[8]) << 8) | BillRdBuff[9]);
		if(BillRdBuff[10] == 0) 
	    {
	  	  stDevValue.BillEscrowFun = 0;//无暂存
	    }
	    else 
	    {
	  	  stDevValue.BillEscrowFun = 1; //有暂存
	    }			
		for(i=0;i<BillRdLen-11;i++)
		{
			if(BillRdBuff[11+i] == 0xFF) 
			{
			     break;
			}
			
			nBillvalue[i] = (uint32_t)BillRdBuff[i+11] * stDevValue.BillScale;	
			stDevValue.BillValue[i] = nBillvalue[i];			
		}	
		for(i = 0; i < 16; i++) 
		{
			 TraceBill("\r\n%dDrvBill[%d] = %d", BillRdLen,i,stDevValue.BillValue[i]);
		}
	}
	
	//轮询纸币器
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	}
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//Identification
	ComStatus = MdbConversation(0x37,0x00,1,&BillRdBuff[0],&BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		stDevValue.billIDENTITYBuf[i] = BillRdBuff[i];
	}
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//Stacker
	//ComStatus = MdbConversation(0x36,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	//OSTimeDly(OS_TICKS_PER_SEC / 100);
	//OSTimeDly(40);
	TraceBill("\r\nDrv4.1=%d,%ld,%ld,%ld,%d",stDevValue.BillLevel,stDevValue.BillScale,stDevValue.BillDecimal,stDevValue.BillStkCapacity,stDevValue.BillEscrowFun);
	TraceBill("\r\nDrv4.2=%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",nBillvalue[0],nBillvalue[1],nBillvalue[2],nBillvalue[3],nBillvalue[4],nBillvalue[5],nBillvalue[6],nBillvalue[7]);
	TraceBill("\r\nIden=");
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" [%02x]",stDevValue.billIDENTITYBuf[i]);
	}
	//OSTimeDly(OS_TICKS_PER_SEC*3);
	BillDevEnable();
	LCDNumberFontPrintf(40,LINE15,2,"BillAccepter-4");	
	ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	return MdbBillErr.Communicate;
	/*
	else
	if(NowBillDev == BILLDEV_RS232)
	{
		for(i=0;i<8;i++)
			nBillvalue[i] = GetMemoryData(ADD_BILLVALUE1+i*4);
		BillDevEnable();	
	}*/
	//从内存中读取硬币器通道面值
	//for(i=0;i<8;i++)
	//	nCoinvalue[i] = GetMemoryData(ADD_COINVALUE1+i*4);
}


/*********************************************************************************************************
** Function name:       BillDevDisable
** Descriptions:        禁能纸币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevDisable(void)
{
	uint8_t BillValue[4]={0},BillRdBuff[36],BillRdLen;
	uint8_t ComStatus;

	/*if(NowBillDev == BILLDEV_RS232)
	{
		GbaControl_BillDisable();
	}
	else*/
	
	BillValue[0] = 0x00;
	BillValue[1] = 0x00;
	BillValue[2] = 0xff;
	BillValue[3] = 0xff;
   	ComStatus = MdbConversation(0x34,BillValue,4,&BillRdBuff[0],&BillRdLen);
	if(ComStatus == 1)
		MdbBillErr.Communicate = 0;
	else
		MdbBillErr.Communicate = 1;
	
}


void BillDevReject(void)
{
	uint8_t BillValue[4]={0},BillRdBuff[36],BillRdLen;

	/*if(NowBillDev == BILLDEV_RS232)
	{
		GbaControl_BillReject();
	}
	else*/
	
	BillValue[0] = 0x00;
   	MdbConversation(0x35,BillValue,1,&BillRdBuff[0],&BillRdLen);	
		
}



/*********************************************************************************************************
** Function name:       BillDevEnable
** Descriptions:        使能纸币器
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillDevEnable(void)
{
	uint8_t BillWrBuff[4]={0},BillRdBuff[36],BillRdLen;
	uint8_t ComStatus;
	unsigned char j;
	
	/*if(NowBillDev == BILLDEV_RS232)
	{
		if(nBillvalue[0]!=0)
			BillColumnSet |= 0x01;
		if(nBillvalue[1]!=0)
			BillColumnSet |= 0x02;
		if(nBillvalue[2]!=0)
			BillColumnSet |= 0x04;
		if(nBillvalue[3]!=0)
			BillColumnSet |= 0x08;
		if(nBillvalue[4]!=0)
			BillColumnSet |= 0x10;
		if(nBillvalue[5]!=0)
			BillColumnSet |= 0x20;
		if(nBillvalue[6]!=0)
			BillColumnSet |= 0x40;
		if(nBillvalue[7]!=0)
			BillColumnSet |= 0x80;
	}
	else*/
	//Stacker
	ComStatus = MdbConversation(0x36,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	
	if(nBillvalue[8]!=0)
		BillWrBuff[0] |= 0x01;
	if(nBillvalue[9]!=0)
		BillWrBuff[0] |= 0x02;
	if(nBillvalue[10]!=0)
		BillWrBuff[0] |= 0x04;
	if(nBillvalue[11]!=0)
		BillWrBuff[0] |= 0x08;
	if(nBillvalue[12]!=0)
		BillWrBuff[0] |= 0x10;
	if(nBillvalue[13]!=0)
		BillWrBuff[0] |= 0x20;
	if(nBillvalue[14]!=0)
		BillWrBuff[0] |= 0x40;
	if(nBillvalue[15]!=0)
		BillWrBuff[0] |= 0x80;

	
	if(nBillvalue[0]!=0)
		BillWrBuff[1] |= 0x01;
	if(nBillvalue[1]!=0)
		BillWrBuff[1] |= 0x02;
	if(nBillvalue[2]!=0)
		BillWrBuff[1] |= 0x04;
	if(nBillvalue[3]!=0)
		BillWrBuff[1] |= 0x08;
	if(nBillvalue[4]!=0)
		BillWrBuff[1] |= 0x10;
	if(nBillvalue[5]!=0)
		BillWrBuff[1] |= 0x20;
	if(nBillvalue[6]!=0)
		BillWrBuff[1] |= 0x40;
	if(nBillvalue[7]!=0)
		BillWrBuff[1] |= 0x80;
	BillWrBuff[2] = 0xFF;
	BillWrBuff[3] = 0xFF;
	TraceBill("\r\nDrvBillEnable>>%#02x,%#02x,%#02x,%#02x,%#02x\r\n",0x34,BillWrBuff[0],BillWrBuff[1],BillWrBuff[2],BillWrBuff[3]);
	//Enable
	for(ComStatus = 0, j = 0; j < 2 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x34,BillWrBuff,4,&BillRdBuff[0],&BillRdLen);
		OSTimeDly(3);
	}
	TraceBill("\r\n DrvBILLENABLE=%d",ComStatus);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	if(ComStatus == 1)
		MdbBillErr.Communicate = 0;
	else
		MdbBillErr.Communicate = 1;
	
}




/*********************************************************************************************************
** Function name:       BillDevProcess
** Descriptions:        纸币器收币压钞操作
** input parameters:    无
** output parameters:   RecvMoney——收入的纸币金额
						BillType——收入的纸币通道
						billOpt--纸币器控制参数
						billOptBack--纸币器控制返回结果
** Returned value:      有纸币收入返回1，无返回0
*********************************************************************************************************/
uint8_t BillDevProcess(uint32_t *RecvMoney,unsigned char *BillType,unsigned char billOpt,unsigned char *billOptBack,uint32_t RecyPayoutMoney,uint8_t RecyPayoutNum,unsigned int *RecyPayoutMoneyBack)
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus,BillWrBuff[1];
	uint8_t type=0,i=0;
	static uint8_t billrec=0;


	switch(billOpt)
	{
		case MBOX_BILLENABLEDEV:
			//Trace("enable bill\r\n");
			BillDevEnable();
			break;
		case MBOX_BILLDISABLEDEV:
			//Trace("disable bill\r\n");
			TraceBill("\r\n DrvBILLDISABLEDEV opt");
			BillDevDisable();
			break;			
		case MBOX_BILLESCROW:
			//Trace("\r\nescrow bill");
			if(BillDevEscrow())
				*billOptBack = 2;	
			else
				*billOptBack = 1;	
			break;	
		case MBOX_BILLRETURN:
			//Trace("\r\nescrow bill");
			if(BillDevReturn())
				*billOptBack = 4;	
			else
				*billOptBack = 3;	
			break;	
		case MBOX_BILLRECYPAYOUTNUM:
			//TraceBill("\r\n DrvRECPay=%ld,Num=%d",RecyPayoutMoney,RecyPayoutNum);
			if(BillRecyclerPayoutNumExpanse(RecyPayoutMoney,RecyPayoutNum))
				*billOptBack = 6;	
			else
				*billOptBack = 5;		
			break;
		case MBOX_BILLRECYPAYOUTVALUE:
			//TraceBill("\r\n DrvRECPay=%ld,Num=%d",RecyPayoutMoney,RecyPayoutNum);
			if(BillRecyclerPayoutValueExpanse(RecyPayoutMoney,RecyPayoutMoneyBack))
				*billOptBack = 6;	
			else
				*billOptBack = 5;		
			break;	
	}
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//Trace("6\r\n");
	BillWrBuff[0] = 0x01;
	
	//轮询纸币器是否有收到纸币，有返回1
	ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	if(ComStatus == 1)
	{
		MdbBillErr.Communicate = 0;
		billrec=0;
		TraceBill("\r\nDrvBill= %02d-",BillRdLen);
		for(i=0;i<16;i++)
		{
			TraceBill(" %02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");
		OSTimeDly(OS_TICKS_PER_SEC / 100);
		if(BillRdLen==0)
		{
			TraceBill("\r\n Drvbill default");	
			OSTimeDly(OS_TICKS_PER_SEC / 100);
			memset(&MdbBillErr,0,sizeof(MdbBillErr));
		}
		for(i = 0; i < BillRdLen; i++) 
		{
			if((BillRdBuff[i]&0xf0)==0x90)
			{
				type = BillRdBuff[i]&0x0f;
				if(stDevValue.BillEscrowFun == 0)//该纸币器无暂存功能
				{
					//发送压仓指令
					MdbConversation(0x35,BillWrBuff,0x01,&BillRdBuff[0],&BillRdLen);
					Timer.EscrowTimer = 40;
					while(Timer.EscrowTimer)
					{
						//轮询检测是否压仓成功
						ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
						if(ComStatus == 1)
						{
							for(i = 0; i < BillRdLen; i++) 
							{
								if((BillRdBuff[i]&0xf0)==0x80)
								{
									*RecvMoney = nBillvalue[type];
									*BillType = type;						
									return 1;	
								}
							}
						}	
					}
				}
				else
				{
					*RecvMoney = nBillvalue[type];
					*BillType = type;
					TraceBill("Drvbill=%ld,%d\r\n",*RecvMoney,*BillType);
					OSTimeDly(OS_TICKS_PER_SEC / 100);
					return 1;
				}
			}
			else if((BillRdBuff[i] & 0xF0) == 0) 
			{   
				//validator status
			    switch(BillRdBuff[i]) 
				{
		            case 0x01:			                 //defective motor    
		            	TraceBill("\r\n Drvbill motor");
				        MdbBillErr.moto = 1;
				        break;
			
			        case 0x02:			                 //sensor problem
			        	TraceBill("\r\n Drvbill sensor");
					    MdbBillErr.sensor = 1;
				        break;
			
			        case 0x03:			                 //validator busy
			        	TraceBill("\r\n Drvbil busy");
			        	break;
			
			        case 0x04:			                 //rom chksum err
			        	TraceBill("\r\n Drvbill chksum");
				    	MdbBillErr.romchk = 1;
			        break;
			
			        case 0x05:			                 //validator jammed
			        	TraceBill("\r\n Drvbill jammed");
					    MdbBillErr.jam = 1;			       
				        break;
			
			        case 0x06:			                 //validator was reset
			        	TraceBill("\r\n Drvbil reset");
						break;
				 
			        case 0x07:			                 //bill removed	
			        	TraceBill("\r\n Drvbil removed");
			        	break;
			 
			        case 0x08:			                 //cash box out of position
			        	TraceBill("\r\n Drvbill removeCash");
					    MdbBillErr.removeCash = 1;	
				        break;
			
			        case 0x09:			                 //validator disabled	
			        	TraceBill("\r\n Drvbill disabled");
			        	MdbBillErr.disable = 1;
						break;
			
			        case 0x0A:			                 //invalid escrow request
			        	TraceBill("\r\n Drvbil invalid");
			       		break;
			
			        case 0x0B:			                 //bill rejected
			        	TraceBill("\r\n Drvbil rejected");
			        	break;
			
			        case 0x0C:			                 //possible credited bill removal
			        	TraceBill("\r\n Drvbill cashErr");
					    MdbBillErr.cashErr = 1;	
				        break;
			
			        default:
						TraceBill("\r\n Drvbill default");	
						memset(&MdbBillErr,0,sizeof(MdbBillErr));
					    break;
		         }
		    }
		}
	}
	else
	{
		billrec++;
		TraceBill("\r\n Drvbill commuFail=%d,billrec=%d",ComStatus,billrec);
		if(billrec>=30)
		{
			TraceBill("\r\n Drvbill commReject");
			BillDevReject();
			billrec=0;
		}
		MdbBillErr.Communicate = 1;
	}
	
	/*
	else
	if(NowBillDev == BILLDEV_RS232)
	{
		ComStatus = GbaControl_BillAccept();	
		if(ComStatus == 1)
		{
			ComStatus = GbaControl_BillStack();
			MDBCONVERSATIONWAITACKTIMEOUT1 = 800;
			while(MDBCONVERSATIONWAITACKTIMEOUT1)
			{
				ComStatus = GbaControl_BillStacked();
				if(ComStatus == 1)
				{
					type = RecvBillType/8-1;
					*RecvMoney = nBillvalue[type];
					*BillType = type;
					MsgPack.BackType = TASKCMD_BILLIN;
					MsgPack.SubBackType = type;
					MsgPack.moneyback = nBillvalue[type];
					//发送邮箱给DevieTask让其操作Hopper做出币操作
					OSMboxPost(g_MoneyIN,&MsgPack);
					return 1;	
				}
				OSTimeDly(5);	
			}
		}
		//BillDevReject();
	}*/
	return 0;
}


/*********************************************************************************************************
** Function name:       BillDevEscrow
** Descriptions:        暂存纸币压钞操作
** input parameters:    无
** output parameters:   
** Returned value:      压抄成功返回1，失败返回0
*********************************************************************************************************/
uint8_t BillDevEscrow()
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus,BillWrBuff[1],j;
	uint8_t i=0;
	BillWrBuff[0] = 0x01;

	//发送压仓指令
	for(ComStatus = 0, j = 0; j < 20 && ComStatus != 1; j++)
	{
		ComStatus =MdbConversation(0x35,BillWrBuff,0x01,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n DrvEscrowsend=%d",ComStatus);
	TraceBill("\r\nDrvEscrowsend>>%#02x,%#02x\r\n",0x35,BillWrBuff[0]);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	Timer.EscrowTimer = 13;
	while(Timer.EscrowTimer)
	{
		//轮询检测是否压仓成功
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
		TraceBill("\r\nDrvEscrowsend>>%#02x\r\n",0x33);
		if(ComStatus == 1)
		{
			TraceBill("\r\nDrvBill= %02d-",BillRdLen);
			for(i=0;i<16;i++)
			{
				TraceBill(" %02x ",BillRdBuff[i]);
			}
			TraceBill("\r\n");
			for(i = 0; i < BillRdLen; i++) 
			{
				//0x80到钞箱,0xb0到循环斗
				if( ((BillRdBuff[i]&0xf0)==0x80)||((BillRdBuff[i]&0xf0)==0xb0) )
				{			
					TraceBill("\r\n Drvescrow1");
					OSTimeDly(OS_TICKS_PER_SEC / 100);
					if((BillRdBuff[i]&0xf0)==0xb0)
					{
						BillRecyclerTubeExpanse();
					}
					return 1;	
				}
				//没有压抄成功，中途退出
				else if((BillRdBuff[i]&0xf0)==0xa0)
				{
					TraceBill("\r\n DrvescrowReturn");
					return 0;
				}
			}			
			TraceBill("\r\n Drvescrow2");
			OSTimeDly(OS_TICKS_PER_SEC / 100);
		}
		TraceBill("\r\n Drvescrow3");
		OSTimeDly(7);
	}	
	return 0;
}

/*********************************************************************************************************
** Function name:       BillDevReturn
** Descriptions:        暂存纸币退币操作
** input parameters:    无
** output parameters:   
** Returned value:      退币成功返回1，失败返回0
*********************************************************************************************************/
uint8_t BillDevReturn()
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus,BillWrBuff[1],j;
	BillWrBuff[0] = 0x00;

	//发送退币指令
	for(ComStatus = 0, j = 0; j < 20 && ComStatus != 1; j++)
	{
		ComStatus =MdbConversation(0x35,BillWrBuff,0x01,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n DrvReturnsend=%d",ComStatus);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	Timer.EscrowTimer = 10;
	while(Timer.EscrowTimer)
	{
		//轮询检测是否退币成功
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
		if(ComStatus == 1)
		{
			//if((BillRdBuff[0]&0xf0)==0x80)
			{			
				TraceBill("\r\n Drvreturn1");
				OSTimeDly(OS_TICKS_PER_SEC / 100);
				return 1;	
			}
			//TraceBill("\r\n escrow2");
		}
		TraceBill("\r\n Drvsend3");
		OSTimeDly(7);
	}	
	return 0;
}


//==============================
//====纸币循环找零器
//==============================

/*********************************************************************************************************
** Function name:       BillRecyclerIdExpanse
** Descriptions:        得到纸币器是否支持循环找零的信息,看z30-z33字节
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void BillRecyclerIdExpanse(void)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[1]={0x02},i;
    //uint8_t coinscale,dispenseValue;

	ComStatus = MdbConversation(0x37,VMCdata,1,&BillRdBuff[0],&BillRdLen);
	TraceBill("\r\n 6DrvRecyclerIDLevel2>>%#02x,%#02x",0x37,VMCdata[0]);
	if(ComStatus == 1)	
      {  	
	    	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");	    
     } 	  
}


/*********************************************************************************************************
** Function name:       BillRecyclerFeatureEnableExpanse
** Descriptions:        使能扩展指令，这样子纸币器就能接收扩展指令了
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void BillRecyclerFeatureEnableExpanse(void)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[5]={0x01,0x00,0x00,0x00,0x02};
    //uint8_t coinscale,dispenseValue;

	ComStatus = MdbConversation(0x37,VMCdata,5,&BillRdBuff[0],&BillRdLen);
	TraceBill("\r\n 7DrvRecyclerFeatureEnable>>%#02x,%#02x,%#02x,%#02x,%#02x,%#02x",0x37,VMCdata[0],VMCdata[1],VMCdata[2],VMCdata[3],VMCdata[4]);
	if(ComStatus == 1)	
    {  	
    		TraceBill("\r\nDrvRecEnable-7");    
    } 	  
}

/*********************************************************************************************************
** Function name:       BillRecyclerSetupExpanse
** Descriptions:        循环纸币器setup指令,得到哪些纸币值可以放到循环斗中
** input parameters:    无
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void BillRecyclerSetupExpanse(void)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[1]={0x03},i;
    
	
    ComStatus = MdbConversation(0x37,VMCdata,1,&BillRdBuff[0],&BillRdLen);
	TraceBill("\r\n 11DrvRecyclerSetup>>%#02x,%#02x",0x37,VMCdata[0]);
	
	if(ComStatus == 1)	
    {  	
    	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");	
		for(i=0;i<7;i++)
		{
			if((BillRdBuff[1]>>i)&0x01)
				stDevValue.RecyclerValue[i]=stDevValue.BillValue[i];
		}
		TraceBill("\r\nDrvRecValue=%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",stDevValue.RecyclerValue[0],stDevValue.RecyclerValue[1],stDevValue.RecyclerValue[2],stDevValue.RecyclerValue[3],stDevValue.RecyclerValue[4],stDevValue.RecyclerValue[5],stDevValue.RecyclerValue[6]);
    } 	  
}



/*********************************************************************************************************
** Function name:       BillRecyclerEnableExpanse
** Descriptions:        使能是否进入循环器,第3字节-18字节，就是通道0-15是否进入循环器
**                      全部进入循环器VMCdata[19]=  {0x04,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03};
**                      全部不进入循环器VMCdata[19]={0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
** input parameters:    enable=1打开循环器功能,0关闭循环器功能
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void BillRecyclerEnableExpanse(uint8_t enable)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,i;
	//uint8_t VMCdata[19]={0x04,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03};
    uint8_t VMCdata[19]={0x04,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	//进入循环器
	if(enable==1)
	{
		for(i=0;i<7;i++)
		{
			if(stDevValue.RecyclerValue[i]>0)
			{
				VMCdata[3+i]=0x03;
			}
		}		
	}
	//关闭循环器
	else if(enable==0)
	{		
		for(i=0;i<7;i++)
		{
			VMCdata[3+i]=0x00;				
		}				
	}	
	
	TraceBill("\r\n12DrvRecEnable>> [%02d]- %#02x,",20,0x37);
	for(i=0;i<19;i++)
	{
		TraceBill(" %#02x ",VMCdata[i]);
	}
	TraceBill("\r\n");	
	
    ComStatus = MdbConversation(0x37,VMCdata,19,&BillRdBuff[0],&BillRdLen);
	if(ComStatus == 1)	
    {  	 
    	TraceBill("\r\nDrvRecEnable-9");
    }
	 	
}


/*********************************************************************************************************
** Function name:       BillRecyclerTubeExpanse
** Descriptions:        纸币找零器z1-z2这个面值是否已经满了,z3-z34每种面值目前存储币数量,2个字节表示一个面值
** input parameters:    无
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void BillRecyclerTubeExpanse(void)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[1]={0x05},i,j;
	static uint8_t RecAble=1;//1已经打开,0已经关闭
    
	
    ComStatus = MdbConversation(0x37,VMCdata,1,&BillRdBuff[0],&BillRdLen);
	TraceBill("\r\n 13DrvRecyclerTube>>%#02x,%#02x",0x37,VMCdata[0]);
	if(ComStatus == 1)	
    {  	
    	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");
		OSTimeDly(OS_TICKS_PER_SEC / 100);
		//如果有任何一个面值存储满，就关闭循环器功能。否则打开循环器功能
		if(INTEG16(BillRdBuff[0],BillRdBuff[1]))
		{
			if(RecAble==1)
			{
				BillRecyclerEnableExpanse(0);
				RecAble=0;
			}
		}
		else
		{
			if(RecAble==0)
			{			
				BillRecyclerEnableExpanse(1);
				RecAble=1;
			}
		}
		//更新每种面值目前存储币数量
	    for( i=0,j=2; i<7; i++, j+=2 )
		{
			stDevValue.RecyclerNum[i]  = INTEG16(BillRdBuff[j],BillRdBuff[j+1]);
		}
		TraceBill("\r\nDrvRecNum=%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",stDevValue.RecyclerNum[0],stDevValue.RecyclerNum[1],stDevValue.RecyclerNum[2],stDevValue.RecyclerNum[3],stDevValue.RecyclerNum[4],stDevValue.RecyclerNum[5],stDevValue.RecyclerNum[6]);
    } 	  
}




/*********************************************************************************************************
** Function name:       BillDevInit
** Descriptions:        纸币器设备初始化
** input parameters:    无
** output parameters:   无
** Returned value:      1使能失败,0使能成功
*********************************************************************************************************/
uint8_t RecyclerDevInit()
{
	unsigned char BillRdBuff[36],BillRdLen,ComStatus,i;	
	unsigned char j;
	uint8_t VMCdata[2]={0x00,0x00};
	memset(BillRdBuff,0,sizeof(BillRdBuff));	
	for(j = 0; j < 16; j++) 
	{
		 stDevValue.BillValue[j]=0;
		 //TraceBill("\r\nqqDrvChangebuf[%d] = %d,%d", j,ChangerRdBuff[j+2],stDevValue.CoinNum[j]);
	}
	//1Reset
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus =MdbConversation(0x30,NULL,0x00,&BillRdBuff[0],&BillRdLen);
		OSTimeDly(OS_TICKS_PER_SEC/100);
	}
	//OSTimeDly(OS_TICKS_PER_SEC*3);
	TraceBill("\r\n 1DrvRecyclerRESET>>%#02x",0x30);	
	LCDNumberFontPrintf(40,LINE15,2,"Recycler-1");	
	//2Poll
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n 2DrvRecyclerPOLL>>%#02x",0x33);
	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" %#02x ",BillRdBuff[i]);
	}
	TraceBill("\r\n");
	LCDNumberFontPrintf(40,LINE15,2,"Recycler-2");
	
	//3setup
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x31,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	}
	TraceBill("\r\n 3DrvRecyclerSETUP>>%#02x",0x31);	
	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" %#02x ",BillRdBuff[i]);
	}
	TraceBill("\r\n");
	if(ComStatus == 1)
	{
		stDevValue.BillLevel = BillRdBuff[0];
		stDevValue.BillCode = ((((uint16_t)BillRdBuff[1]) << 8) | BillRdBuff[2]);
		stDevValue.BillDecimal = 100;
	    for(i = 0; i < BillRdBuff[5]; i++) 
	    {
		   stDevValue.BillDecimal /= 10;
	    }
		stDevValue.BillScale = ((((uint16_t)BillRdBuff[3]) << 8) | BillRdBuff[4]) * stDevValue.BillDecimal;
		//stDevValue.Decimal = BillRdBuff[5];
		stDevValue.BillStkCapacity = ((((uint16_t)BillRdBuff[6]) << 8) | BillRdBuff[7]);
		stDevValue.BillSecurity = ((((uint16_t)BillRdBuff[8]) << 8) | BillRdBuff[9]);
		TraceBill("\r\n DrvRecyclerSecu=%#02x,%#02x,%ld",BillRdBuff[8],BillRdBuff[9],stDevValue.BillSecurity);
		VMCdata[0]=BillRdBuff[8];
		VMCdata[1]=BillRdBuff[9];
		if(BillRdBuff[10] == 0) 
	    {
	  	  stDevValue.BillEscrowFun = 0;//无暂存
	    }
	    else 
	    {
	  	  stDevValue.BillEscrowFun = 1; //有暂存
	    }			
		for(i=0;i<BillRdLen-11;i++)
		{
			if(BillRdBuff[11+i] == 0xFF) 
			{
			     break;
			}
			
			nBillvalue[i] = (uint32_t)BillRdBuff[i+11] * stDevValue.BillScale;	
			stDevValue.BillValue[i] = nBillvalue[i];			
		}
		for(i = 0; i < 16; i++) 
		{
			 TraceBill("\r\n%dDrvRec[%d] = %d", BillRdLen,i,stDevValue.BillValue[i]);
		}
	}
	
	//4security
	for(ComStatus = 0, j = 0; j < 10 && ComStatus != 1; j++)
	{
		ComStatus = MdbConversation(0x32,VMCdata,2,&BillRdBuff[0],&BillRdLen);
	}
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	TraceBill("\r\n 4DrvRecyclerSecurity>>%#02x,%#02x,%#02x",0x32,VMCdata[0],VMCdata[1]);
	LCDNumberFontPrintf(40,LINE15,2,"Recycler-3");	
	
	//5IDLevel1
	ComStatus = MdbConversation(0x37,0x00,1,&BillRdBuff[0],&BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		stDevValue.billIDENTITYBuf[i] = BillRdBuff[i];
	}	
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	TraceBill("\r\n 5DrvRecyclerIDLevel1>>%#02x,%#02x",0x37,0x00);
	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" %#02x ",BillRdBuff[i]);
	}
	TraceBill("\r\n");
	//6IDLevel2
	BillRecyclerIdExpanse();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//7FeatureEnable
	BillRecyclerFeatureEnableExpanse();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//8Stacker
	ComStatus = MdbConversation(0x36,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	TraceBill("\r\n 8DrvRecyclerStacker>>%#02x",0x36);
	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" %#02x ",BillRdBuff[i]);
	}
	TraceBill("\r\n");
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//OSTimeDly(40);
	TraceBill("\r\nDrv4.1=%d,%ld,%ld,%ld,%d",stDevValue.BillLevel,stDevValue.BillScale,stDevValue.BillDecimal,stDevValue.BillStkCapacity,stDevValue.BillEscrowFun);
	TraceBill("\r\nDrv4.2=%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",nBillvalue[0],nBillvalue[1],nBillvalue[2],nBillvalue[3],nBillvalue[4],nBillvalue[5],nBillvalue[6],nBillvalue[7]);
	//OSTimeDly(OS_TICKS_PER_SEC*3);
	//9Enable
	BillDevEnable();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	LCDNumberFontPrintf(40,LINE15,2,"RecyclerAccepter-4");
	//10Poll
	ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	TraceBill("\r\n 10DrvRecyclerPOLL>>%#02x",0x33);
	TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
	for(i=0;i<BillRdLen;i++)
	{
		TraceBill(" %#02x ",BillRdBuff[i]);
	}
	TraceBill("\r\n");
	for(j = 0; j < 130; j++)
	{
		//12Poll
		ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
		OSTimeDly(OS_TICKS_PER_SEC / 10);
		TraceBill("\r\n 10DrvRecyclerPOLL>>%#02x",0x33);
		TraceBill("\r\nDrvRecyclerRec<< [%02d]-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");
	}
	
	//11RecyclerSetup
	BillRecyclerSetupExpanse();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	//12RecyclerEnable
	BillRecyclerEnableExpanse(1);
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	LCDNumberFontPrintf(40,LINE15,2,"RecyclerAccepter-5");
	//13RecyclerTube
	BillRecyclerTubeExpanse();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
	return MdbBillErr.Communicate;	
}


/*********************************************************************************************************
** Function name:       BillRecyclerPayoutNumExpanse
** Descriptions:        按张数找币,0-15的面值用0x00-0x0f来表示,0x00,0x01表示出币数量
                        例如10元面值找一张VMCdata[4]={0x06,0x02,0x00,0x01}
** input parameters:    PayoutType需要找的面值,PayoutNum需要找的张数
** output parameters:   无
** Returned value:      1找零成功,0找零失败
*********************************************************************************************************/
unsigned char BillRecyclerPayoutNumExpanse(unsigned int RecyPayoutMoney,unsigned char RecyPayoutNum)
{
    uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[4]={0x06,0x00,0x00,0x00},VMCPoll[1]={0x09},i,payout=1;
	uint8_t payStatus=0;

	for(i=0;i<7;i++)
	{
		if(stDevValue.RecyclerValue[i]==RecyPayoutMoney)
		{
			VMCdata[1]=i;
		}
	}	
	VMCdata[3]=RecyPayoutNum;
	TraceBill("\r\nDrvRecPay= %02d,Num=%02d",VMCdata[1],VMCdata[3]);
	//1发送找币命令
    ComStatus = MdbConversation(0x37,VMCdata,4,&BillRdBuff[0],&BillRdLen);
	if(ComStatus == 1)	
    {  	
    	TraceBill("\r\nDrvBillRecycler8= %02d-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");
		OSTimeDly(OS_TICKS_PER_SEC / 100);
		while(payout)
		{
			//2发送poll指令
			ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
			TraceBill("\r\nDrvBillPoll= %02d-",BillRdLen);
			for(i=0;i<BillRdLen;i++)
			{
				TraceBill(" %#02x ",BillRdBuff[i]);
			}
			TraceBill("\r\n");
			OSTimeDly(7);
			
			//3发送扩展payout value poll指令，检测找币情况
			ComStatus = MdbConversation(0x37,VMCPoll,1,&BillRdBuff[0],&BillRdLen);
			TraceBill("\r\nDrvRecVPoll= %02d-",BillRdLen);
			for(i=0;i<BillRdLen;i++)
			{
				TraceBill(" %#02x ",BillRdBuff[i]);
			}
			TraceBill("\r\n");
			if(BillRdLen==0)
			{
				VMCPoll[0]=0x08;
				//4发送扩展payout status指令，检测找币数量
				ComStatus = MdbConversation(0x37,VMCPoll,1,&BillRdBuff[0],&BillRdLen);
				TraceBill("\r\nDrvRecStatus= %02d-",BillRdLen);
				for(i=0;i<BillRdLen;i++)
				{
					TraceBill(" %#02x ",BillRdBuff[i]);
				}
				TraceBill("\r\n");
				OSTimeDly(7);
				//5发送指令更新币斗剩余纸币数量
				BillRecyclerTubeExpanse();
				payout=0;
				payStatus=1;
			}
			OSTimeDly(7);
			
		}
    } 
	return payStatus;
}


/*********************************************************************************************************
** Function name:       BillRecyclerPayoutValueExpanse
** Descriptions:        按张金额找币                        
** input parameters:    RecyPayoutMoney需要找的金额,以分为单位
** output parameters:   无
** Returned value:      1找零成功,0找零失败
*********************************************************************************************************/
unsigned char BillRecyclerPayoutValueExpanse(unsigned int RecyPayoutMoney,unsigned int *RecyPayoutMoneyBack)
{
	uint8_t BillRdBuff[36],BillRdLen,ComStatus,VMCdata[3]={0x07,0x00,0x00},VMCPoll[1]={0x09},i,payout=1;
	uint8_t billscale,dispenseValue;	
	uint8_t payStatus=0;
	uint32_t RecyclerValue=0;

	*RecyPayoutMoneyBack=0;
	billscale = stDevValue.BillScale;
	dispenseValue = RecyPayoutMoney / billscale;//发送找零基准数量 
	VMCdata[1]=HUINT16(dispenseValue);
	VMCdata[2]=LUINT16(dispenseValue);
	TraceBill("\r\nDrvBillRecyclerMoney= %#02x-%#02x",VMCdata[1],VMCdata[2]);
	ComStatus = MdbConversation(0x37,VMCdata,3,&BillRdBuff[0],&BillRdLen);
	if(ComStatus == 1)	
	{	
		TraceBill("\r\nDrvBillRecycler8= %02d-",BillRdLen);
		for(i=0;i<BillRdLen;i++)
		{
			TraceBill(" %#02x ",BillRdBuff[i]);
		}
		TraceBill("\r\n");
		OSTimeDly(OS_TICKS_PER_SEC / 100);
		while(payout)
		{
			//2发送poll指令
			ComStatus = MdbConversation(0x33,NULL,0x00,&BillRdBuff[0],&BillRdLen);
			TraceBill("\r\nDrvBillPoll= %02d-",BillRdLen);
			for(i=0;i<BillRdLen;i++)
			{
				TraceBill(" %#02x ",BillRdBuff[i]);
			}
			TraceBill("\r\n");
			OSTimeDly(7);
			
			//3发送扩展payout value poll指令，检测找币情况
			ComStatus = MdbConversation(0x37,VMCPoll,1,&BillRdBuff[0],&BillRdLen);
			TraceBill("\r\nDrvRecVPoll= %02d-",BillRdLen);
			for(i=0;i<BillRdLen;i++)
			{
				TraceBill(" %#02x ",BillRdBuff[i]);
			}
			TraceBill("\r\n");
			if(BillRdLen==0)
			{
				VMCPoll[0]=0x08;
				//4发送扩展payout status指令，检测找币数量
				ComStatus = MdbConversation(0x37,VMCPoll,1,&BillRdBuff[0],&BillRdLen);
				TraceBill("\r\nDrvRecStatus= %02d-",BillRdLen);
				for(i=0;i<BillRdLen;i++)
				{
					TraceBill(" %#02x ",BillRdBuff[i]);
				}
				TraceBill("\r\n");
				//5得到找零多少钱
				for(i=0;i<7;i++)
				{
					if(stDevValue.RecyclerValue[i])
					{
						RecyclerValue=stDevValue.RecyclerValue[i];//得到可以暂存到循环斗的面值
						break;
					}
				}
				for(i=0;i<BillRdLen;i++)
				{
					if(BillRdBuff[i]>0)
					{
						*RecyPayoutMoneyBack=RecyclerValue*BillRdBuff[i];
						break;
					}
				}
				OSTimeDly(7);
				//5发送指令更新币斗剩余纸币数量
				BillRecyclerTubeExpanse();
				payout=0;
				payStatus=1;
			}
			OSTimeDly(7);
			
		}
	}	
	return payStatus;
}



