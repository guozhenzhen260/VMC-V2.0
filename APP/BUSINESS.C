/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           BUSINESS.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        交易流程                    
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"
#include "BUSINESS.H"
#include "MAINTAIN.H"
#include "CHANNEL.h"
#include "LIFTTABLE.h"
#include "ACDC.h"



//extern SYSTEMPARAMETER SystemPara;
extern RTC_DATE RTCData;
extern RTC_DATE vmRTC;


#define VMC_FREE		  0                         //空闲状态
#define VMC_CHAXUN	      1							//货道查询状态
#define VMC_CHAXUNHEFANG  2							//盒饭柜号查询状态
#define VMC_SALE 	      3							//交易状态
#define VMC_OVERVALUE	  4							//面值太大
#define VMC_XUANHUO    	  5							//选货
#define VMC_WUHUO    	  6							//无货
#define VMC_LESSMONEY 	  7							//钱不够商品单价
#define VMC_CHANGESHORT	  8							//零钱不够
#define VMC_READVENDFAIL  9							//读卡器扣款失败
#define VMC_CHUHUO  	  10						//出货
#define VMC_QUHUO  	  	  11						//取货
#define VMC_CHUHUOFAIL 	  12						//出货失败
#define VMC_PAYOUT 	      13						//找零
#define VMC_END 	      14						//结束交易
#define VMC_ERROR		  15                        //故障状态
#define VMC_ERRORSALE	  16                        //故障时投入钱币的状态
#define VMC_ERRORPAYOUT	  17                        //故障时找零的状态
#define VMC_ERROREND	  18                        //故障时结束交易的状态
#define VMC_WEIHU		  19                        //维护状态



char     	ChannelNum[3] = {0};//选择货道值
char     	BinNum[2] = {0};//选择盒饭箱柜值
uint8_t	    channelInput = 0;
uint8_t     channelMode = 0;//0输入柜子编号,1输入本柜商品编号
uint8_t	 	vmcStatus = VMC_FREE;//当前状态
uint8_t	 	vmcChangeLow = 0;//零钱是否不够找1不够找,0够找


uint32_t 	g_coinAmount = 0;   	 //当前投币硬币总金额
uint32_t 	g_billAmount = 0;    //当前压入纸币总金额
uint32_t 	g_holdValue = 0;    //当前暂存纸币金额
uint32_t 	g_readerAmount = 0; //当前读卡器总金额

uint16_t	vmcColumn = 0;//当前购买的商品编号
uint32_t 	vmcPrice  = 0;//当前购买的商品单价
unsigned char transMul = 0;//记录是第几次交易
uint8_t hefangMode = 0;//1代表盒饭柜出货,0代表售货机出货



uint32_t 	MoneyMaxin = 0;//单次交易最大投币上限
uint32_t 	PriceMaxin = 0;//当前最贵商品单价



extern void ClearDealPar(void);
extern void BillCoinCtr(uint8_t billCtr,uint8_t coinCtr,uint8_t readerCtr);
extern uint8_t SaleTimeSet(uint8_t haveSale);

volatile unsigned int GOCCHECKTIMEOUT;

/*********************************************************************************************************
** Function name:     	DispBusinessText
** Descriptions:	    交易时提示语言
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispBusinessText(char *strlanguage,char *streng,char *rightstrlanguage,char *rightstreng)
{
	LCDDrawLine(0,LINE11);
	LCDSpecialPicPrintf(0,LINE12,1,2);//箭头	
	LCDPrintf(40,LINE12,0,SystemPara.Language,strlanguage);
	if(SystemPara.Language != 1)
		LCDPrintf(40,LINE14,0,SystemPara.Language,streng);
	if(strlen(rightstrlanguage)!=NULL)
	{
		LCDSpecialPicPrintf(112,LINE12,1,2);//箭头	
		LCDPrintf(152,LINE12,0,SystemPara.Language,rightstrlanguage);
		if(SystemPara.Language != 1)
			LCDPrintf(152,LINE14,0,SystemPara.Language,rightstreng);
	}
}

/*********************************************************************************************************
** Function name:     	DispBusinessText
** Descriptions:	    清交易时提示语言
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CLrBusinessText()
{	
	LCDClearLineDraw(0,LINE11,1);
	LCDClearLineDraw(0,LINE12,3);	
}


/*********************************************************************************************************
** Function name:     	DispFreePage
** Descriptions:	    空闲页面
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispFreePage()
{
	char strlanguage[30] = {0};
	//char strone[30] = {0},strtwo[30] = {0};
	

	//LCDClrScreen();
	//Trace("\r\n RTC1-Time");
	RTCReadTime(&RTCData);
	OSTimeDly(2);
	//Trace("\r\n read=%d,%d,%d,%d,%d",RTCData.year,RTCData.month,RTCData.day,RTCData.hour,RTCData.minute);	
	//LCDPrintf(0,2,0,SystemPara.Language,"%d-%02d-%02d %02d:%02d:%02d",RTCData.year,RTCData.month,RTCData.day,RTCData.hour,RTCData.minute,RTCData.second);
	//LCDClrScreen();
	//LCDPutLOGOBmp(75,LINE1,SystemPara.Logo);	

	if(vmcChangeLow)
	{
		//Trace("\r\n RTC2-Time");
		RTCReadTime(&RTCData);
		OSTimeDly(2);
		//1警告提示
		LCDSpecialPicPrintf(0,LINE2,4,2);//箭头 
		strcpy(strlanguage,BUSINESSERROR[1][20]); 
		LCDNumberFontPrintf(40,LINE2,2,strlanguage);
		strcpy(strlanguage,BUSINESSERROR[1][22]); 
		LCDNumberFontPrintf(40,LINE4,1,strlanguage);
		strcpy(strlanguage,BUSINESSERROR[SystemPara.Language][21]);
		LCDPrintf(40,LINE6,0,SystemPara.Language,strlanguage);
		strcpy(strlanguage,BUSINESSERROR[SystemPara.Language][22]);
		LCDPrintf(40,LINE8,0,SystemPara.Language,strlanguage);
		//strcpy(strlanguage,BUSINESSERROR[SystemPara.Language][21]); 
		//LCDNumberFontPrintf(40,LINE6,1,strlanguage);
		//LCDClearLineDraw(0,LINE11,1);
		//开启温控器返回室内温度
		if(SystemPara.XMTTemp==1)
		{
			sprintf(strlanguage,"%d.%d \x92",sysXMTMission.recPVTemp/10, sysXMTMission.recPVTemp%10); 
			//LCDNumberFontPrintf(40,LINE4,1,strlanguage);
			LCDPrintf(40,LINE8,0,SystemPara.Language,strlanguage);
		}
		//2划线
		LCDDrawLine(0,LINE11);
		//3时间日期 
		if((vmRTC.year != RTCData.year)||(vmRTC.month !=RTCData.month)||(vmRTC.day !=RTCData.day))
		{
			sprintf( strlanguage, "%04d", RTCData.year);
			LCDNumberFontPrintf(40,LINE12,2,strlanguage);
			sprintf( strlanguage, "%02d-%02d", RTCData.month,RTCData.day);
			LCDNumberFontPrintf(40,LINE14,2,strlanguage);
			vmRTC.year = RTCData.year;
			vmRTC.month =RTCData.month;
			vmRTC.day =RTCData.day;			
		}
		if((vmRTC.hour != RTCData.hour)||(vmRTC.minute !=RTCData.minute))
		{
			sprintf( strlanguage, "%02d:%02d", RTCData.hour,RTCData.minute);
			LCDNumberFontPrintf(96,LINE12,3,strlanguage);
			vmRTC.hour = RTCData.hour;
			vmRTC.minute =RTCData.minute;
		}
		
	}
	else
	{
		strcpy(strlanguage,BUSINESS[SystemPara.Language][6]); 
		LCDPrintf(72,LINE3,0,SystemPara.Language,strlanguage);
		//开启温控器返回室内温度
		if(SystemPara.XMTTemp==1)
		{
			sprintf(strlanguage,"%d.%d \x92",sysXMTMission.recPVTemp/10, sysXMTMission.recPVTemp%10); 
			//LCDNumberFontPrintf(40,LINE4,1,strlanguage);
			LCDPrintf(170,LINE6,0,SystemPara.Language,strlanguage);
		}
		//2时间日期 
		if((vmRTC.year != RTCData.year)||(vmRTC.month !=RTCData.month)||(vmRTC.day !=RTCData.day))
		{
			LCDNumberFontPrintf(80,LINE7,2,"%04d-%02d-%02d",RTCData.year,RTCData.month,RTCData.day);
			vmRTC.year = RTCData.year;
			vmRTC.month =RTCData.month;
			vmRTC.day =RTCData.day;
		}
		if((vmRTC.hour != RTCData.hour)||(vmRTC.minute !=RTCData.minute))
		{
			LCDNumberFontPrintf(60,LINE9,4,"%02d:%02d",RTCData.hour,RTCData.minute);
			vmRTC.hour = RTCData.hour;
			vmRTC.minute =RTCData.minute;
		}
	}	
}
/*********************************************************************************************************
** Function name:     	DispFreePage
** Descriptions:	    故障页面
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispErrPage()
{
	char strlanguage[30] = {0};

	//Trace("\r\n RTC2-Time");
	RTCReadTime(&RTCData);
	OSTimeDly(2);
	//1故障提示
	LCDSpecialPicPrintf(0,LINE2,3,2);//箭头 
	strcpy(strlanguage,BUSINESSERROR[SystemPara.Language][19]); 
	//LCDNumberFontPrintf(40,LINE2,2,strlanguage);
	LCDPrintf(40,LINE2,0,SystemPara.Language,strlanguage);
	strcpy(strlanguage,BUSINESSERROR[SystemPara.Language][0]); 
	//LCDNumberFontPrintf(40,LINE4,1,strlanguage);
	LCDPrintf(40,LINE4,0,SystemPara.Language,strlanguage);
	//LCDClearLineDraw(0,LINE11,1);
	//开启温控器返回室内温度
	if(SystemPara.XMTTemp==1)
	{
		sprintf(strlanguage,"%d.%d \x92",sysXMTMission.recPVTemp/10, sysXMTMission.recPVTemp%10); 
		//LCDNumberFontPrintf(40,LINE4,1,strlanguage);
		LCDPrintf(40,LINE6,0,SystemPara.Language,strlanguage);
	}
	//2划线
	LCDDrawLine(0,LINE11);
	//3时间日期 
	if((vmRTC.year != RTCData.year)||(vmRTC.month !=RTCData.month)||(vmRTC.day !=RTCData.day))
	{
		sprintf( strlanguage, "%04d", RTCData.year);
		LCDNumberFontPrintf(40,LINE12,2,strlanguage);
		sprintf( strlanguage, "%02d-%02d", RTCData.month,RTCData.day);
		LCDNumberFontPrintf(40,LINE14,2,strlanguage);
		vmRTC.year = RTCData.year;
		vmRTC.month =RTCData.month;
		vmRTC.day =RTCData.day;
	}
	if((vmRTC.hour != RTCData.hour)||(vmRTC.minute !=RTCData.minute))
	{
		sprintf( strlanguage, "%02d:%02d", RTCData.hour,RTCData.minute);
		LCDNumberFontPrintf(96,LINE12,3,strlanguage);
		vmRTC.hour = RTCData.hour;
		vmRTC.minute =RTCData.minute;
	}
	
}



//算出需要使用的货道
uint16_t GetColumnNum(char *ChannelNum,uint8_t subType)
{
	uint16_t columnNo;
	//开启副柜或者盒饭柜
	if( (subType==1)||(SystemPara.hefangGui==1) )
	{
		if(ChannelNum[0]=='A')
		{
			//Trace("\r\n 1");
			columnNo = (1*100)+(ChannelNum[1]-'0')*10+(ChannelNum[2]-'0');	
		}
		else if(ChannelNum[0]=='B') 	
		{
			//Trace("\r\n 2");	
			columnNo = (2*100)+(ChannelNum[1]-'0')*10+(ChannelNum[2]-'0');	
		}
		else
		{
			columnNo = (ChannelNum[0]-'0')*100+(ChannelNum[1]-'0')*10+(ChannelNum[2]-'0');	
		}
	}
	//只有主柜
	else
		columnNo = (1*100)+(ChannelNum[0]-'0')*10+(ChannelNum[1]-'0');	
	//Trace("\r\n ChannelNum=%s,column=%ld",ChannelNum,columnNo);
	return columnNo;
}


//算出需要使用的盒饭柜货道
uint16_t GetHefangNum(char *ChannelNum,char *BinNum)
{
	uint16_t columnNo;
	
	columnNo = ((BinNum[0]-'0')*1000)+((BinNum[1]-'0')*100)+((ChannelNum[0]-'0')*10)+(ChannelNum[1]-'0');	
	return columnNo;
}



/*********************************************************************************************************
** Function name:     	DispChaxunFreePage
** Descriptions:	    空闲页面查询货道状态的页面
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChaxunFreePage()
{	
	LCDClrScreen();
	//RTCReadTime(&RTCData);
	//OSTimeDly(2);

	LCDNumberFontPrintf(80,LINE3,2,"%04d-%02d-%02d",RTCData.year,RTCData.month,RTCData.day);
	LCDNumberFontPrintf(60,LINE5,4,"%02d:%02d",RTCData.hour,RTCData.minute);	
}


/*********************************************************************************************************
** Function name:     	DispChaxunPage
** Descriptions:	    输入查询货道的页面
** input parameters:    keyMode=1,表明这个按键是选货按钮按下的,
                               =2,表明这个按键是小键盘按下的,
                               =0表明没有按键按下
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChaxunPage(uint8_t *keyValue,uint8_t *keyMode)
{
	char strlanguage[30] = {0},streng[30] = {0};
	//char tempstrlanguage[30] = {0},tempstreng[30] = {0};
	uint8_t columnState=0,isColumn=0;
	uint16_t columnNo=0;
	uint32_t debtMoney=0;
	uint8_t channel_id=0;

	if(*keyMode == 2)
	{
		if((*keyValue == 'C')||(*keyValue == 'E'))
		{
			channelInput = 0;
			memset(ChannelNum,0,sizeof(ChannelNum));
			if(GetAmountMoney() > 0)
			{				
				vmcStatus = VMC_SALE;
			}
			else
			{
				ClearDealPar(); 	
				CLrBusinessText();
				vmcStatus = VMC_FREE;
			}
			return;
		}		
	}
	//使用键盘按键
	if(*keyMode == 2)		
	{
		ChannelNum[channelInput++] = *keyValue; 
		strcpy(strlanguage,BUSINESS[SystemPara.Language][0]);		
		//盒饭柜都可以
		//开启副柜
		if(SystemPara.SubBinOpen==1)
		{
			if(ChannelNum[0]=='.')
			{
				ChannelNum[0]='A';	
			}
			else if(ChannelNum[0]=='D') 	
			{
				ChannelNum[0]='B';	
			}
			strcat(strlanguage,ChannelNum);
		}	
		//只有主柜
		else
		{
			strcat(strlanguage,ChannelNum);
		}
		strcpy(streng,BUSINESS[1][0]);	
		strcat(streng,ChannelNum);	
		DispBusinessText(strlanguage,streng,"","");

		
		//开启副柜,或者盒饭柜
		if( (SystemPara.SubBinOpen==1)||(SystemPara.hefangGui==1) )
		{
			//Trace("\r\n input=%d,ChannelNum=%s",channelInput,ChannelNum);
			if(channelInput >= 3)
			{
				//去掉乱按键的货道查询
				if
				(
					(SystemPara.SubBinOpen==0)				
					&&(
						(ChannelNum[0]=='.')||(ChannelNum[0]=='D')||(ChannelNum[1]=='.')||(ChannelNum[1]=='D')||
						(ChannelNum[2]=='.')||(ChannelNum[2]=='D')
					  )

				)  
				{
					columnState = 5;
					isColumn=1;
				}
				
				//去掉乱按键的货道查询
				else if((ChannelNum[1]=='.')||(ChannelNum[1]=='D')||(ChannelNum[2]=='.')||(ChannelNum[2]=='D')) 
				{
					columnState = 5;
					isColumn=1;
				}
				else
				{
					columnNo = GetColumnNum(ChannelNum,SystemPara.SubBinOpen);				
					columnState = ChannelCheckIsOk(columnNo%100,columnNo/100);
					if(SystemPara.PcEnable==UBOX_PC)//上报货道信息给工控机
					{
						ButtonRPTAPI(1,columnNo%100,columnNo/100);//上报pc按键信息
						channelInput = 0;
						memset(ChannelNum,0,sizeof(ChannelNum));
						if(GetAmountMoney() > 0)
						{				
							vmcStatus = VMC_SALE;
						}
						else
						{
							ClearDealPar(); 	
							CLrBusinessText();
							vmcStatus = VMC_FREE;
						}
						return;
					}
					//新友宝pc通讯
					else if(SystemPara.PcEnable==SIMPUBOX_PC)//上报货道所对应的商品id信息给工控机
					{
						channel_id=hd_id_by_logic(columnNo/100,columnNo%100);
						if(channel_id)
						{
							if(hd_SIMPLEstate_by_id(columnNo/100,channel_id)!=1)
								ButtonSIMPLERPTAPI(channel_id);//上报pc按键信息
						}
						channelInput = 0;
						memset(ChannelNum,0,sizeof(ChannelNum));
						if(GetAmountMoney() > 0)
						{				
							vmcStatus = VMC_SALE;
						}
						else
						{
							ClearDealPar(); 	
							CLrBusinessText();
							vmcStatus = VMC_FREE;
						}
						return;
					}
					else
					{
						isColumn=1;
					}
				}
				
			}
		}
		//只有主柜
		else
		{
			if(channelInput >= 2)
			{
				//去掉乱按键的货道查询
				if((ChannelNum[0]=='.')||(ChannelNum[0]=='D')||(ChannelNum[1]=='.')||(ChannelNum[1]=='D'))	
				{
					columnState = 5;
					isColumn=1;
				}
				else
				{
					columnNo = GetColumnNum(ChannelNum,SystemPara.SubBinOpen);
					columnState = ChannelCheckIsOk(columnNo%100,columnNo/100);
					TracePC("\r\n APPUboxBtncloumn=%d",columnNo);
					if(SystemPara.PcEnable==UBOX_PC)//上报货道信息给工控机
					{
						ButtonRPTAPI(1,columnNo%100,columnNo/100);//上报pc按键信息
						channelInput = 0;
						memset(ChannelNum,0,sizeof(ChannelNum));
						if(GetAmountMoney() > 0)
						{				
							vmcStatus = VMC_SALE;
						}
						else
						{
							ClearDealPar(); 	
							CLrBusinessText();
							vmcStatus = VMC_FREE;
						}
						return;
					}
					//新友宝pc通讯
					else if(SystemPara.PcEnable==SIMPUBOX_PC)//上报货道所对应的商品id信息给工控机
					{
						channel_id=hd_id_by_logic(columnNo/100,columnNo%100);
						//TracePC("\r\n 2APPUboxButton=%d",channel_id);
						if(channel_id)
						{
							if(hd_SIMPLEstate_by_id(columnNo/100,channel_id)!=1)//货道没有禁能
							{
								if(hd_get_by_id(columnNo/100,channel_id,3)!=3)
									ButtonSIMPLERPTAPI(channel_id);//上报pc按键信息
							}
						}
						channelInput = 0;
						memset(ChannelNum,0,sizeof(ChannelNum));
						if(GetAmountMoney() > 0)
						{				
							vmcStatus = VMC_SALE;
						}
						else
						{
							ClearDealPar(); 	
							CLrBusinessText();
							vmcStatus = VMC_FREE;
						}
						return;
					}
					else
					{
						isColumn=1;
					}
				}
				
			}
		}
	}
	//使用选货按键
	else if(*keyMode == 1)
	{
		//由选货按键得到可以使用的货道编号
		columnNo = ChannelGetSelectColumn(1,1,*keyValue);
		//Trace("\r\n colu=%d",columnNo);
		if(columnNo == 0)
		{
			columnState = 2;
			isColumn=1;
		}
		//按下一个小键盘，不能再按选货按键
		else if(channelInput>0)
		{
			columnState = 5;
			isColumn=1;
		}
		else
		{
			//显示货道编号	
			//开启副柜
			if(SystemPara.SubBinOpen==1)
			{
				ChannelNum[channelInput++] = 'A';
				ChannelNum[channelInput++] = columnNo/10 + '0'; 
				ChannelNum[channelInput++] = columnNo%10 + '0';
			}
			else
			{
				ChannelNum[channelInput++] = columnNo/10 + '0'; 
				ChannelNum[channelInput++] = columnNo%10 + '0';
			}
			strcpy(strlanguage,BUSINESS[SystemPara.Language][0]); 
			strcat(strlanguage,ChannelNum);
			strcpy(streng,BUSINESS[1][0]);	
			strcat(streng,ChannelNum);	
			DispBusinessText(strlanguage,streng,"","");

			columnNo = GetColumnNum(ChannelNum,SystemPara.SubBinOpen);
			columnState = ChannelCheckIsOk(columnNo%100,columnNo/100);
			if(SystemPara.PcEnable==UBOX_PC)
			{
				ButtonRPTAPI(1,columnNo%100,columnNo/100);//上报pc按键信息
				channelInput = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				if(GetAmountMoney() > 0)
				{				
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar(); 	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				return;
			}
			//新友宝pc通讯
			else if(SystemPara.PcEnable==SIMPUBOX_PC)//上报货道所对应的商品id信息给工控机
			{
				channel_id=hd_id_by_logic(columnNo/100,columnNo%100);
				if(channel_id)
				{
					ButtonSIMPLERPTAPI(channel_id);//上报pc按键信息
				}
				channelInput = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				if(GetAmountMoney() > 0)
				{				
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar(); 	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				return;
			}
			else
			{
				isColumn=1;
			}
		}		
	}
		
	if(isColumn)
	{	
		//Trace("\r\n chnum=%d,st=%d",GetColumnNum(ChannelNum),columnState);
		switch(columnState)
		{
			//缺货
			case 2:
			case 4:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][5]);
				strcpy(streng,BUSINESS[1][5]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{		
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar(); 
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;
			//故障
			case 3:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][4]);
				strcpy(streng,BUSINESS[1][4]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{	
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar(); 
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;
			//无此货道	
			case 5:
			case 1:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][1]);
				strcpy(streng,BUSINESS[1][1]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{	
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar(); 
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;	
			//正常	
			case 0: 								
				//strcpy(strlanguage,BUSINESS[SystemPara.Language][3]);
				//strcat(strlanguage,ChannelGetParamValue(GetColumnNum(ChannelNum),1,1));
				//strcpy(streng,BUSINESS[1][3]);	
				//Trace("\r\n money=%ld",GetAmountMoney());
				vmcPrice = ChannelGetParamValue(columnNo%100,1,columnNo/100);
				//Trace("vmcPrice=%d\r\n",vmcPrice);
				debtMoney = vmcPrice;
				switch(SystemPara.DecimalNum) 
				{
				  case 2://以分为单位
					  sprintf(strlanguage,"%s %s,%s%02d.%02d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/100,debtMoney%100);
					  sprintf(streng,"%s %s,%s%02d.%02d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/100,debtMoney%100); 
					  break;

				  case 1://以角为单位
					  debtMoney /= 10;
					  sprintf(strlanguage,"%s %s,%s%d.%d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/10,debtMoney%10);
					  sprintf(streng,"%s %s,%s%d.%d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/10,debtMoney%10);
					  break;
				  
				  case 0://以元为单位
					  sprintf(strlanguage,"%s %s,%s%d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/100);
					  sprintf(streng,"%s %s,%s%d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/100);
					  break;
				}	
				//strcpy(streng,BUSINESS[1][3]);
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC);
				if(GetAmountMoney() > 0)
				{
					vmcColumn = columnNo;
					vmcStatus = VMC_XUANHUO;
				}
				else
				{			
					OSTimeDly(OS_TICKS_PER_SEC);
					ClearDealPar(); 
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;	
		}
		//strcpy(strlanguage,BUSINESS[SystemPara.Language][3]);
		//strcpy(streng,BUSINESS[1][1]);	
		//DispBusinessText(strlanguage,streng,"","");
		//OSTimeDly(OS_TICKS_PER_SEC * 2);
		channelInput = 0;
		/*
		if(GetAmountMoney() > 0)
		{				
			vmcStatus = VMC_XUANHUO;
		}
		else
		{
			ClearDealPar(); 
			CLrBusinessText();
			vmcStatus = VMC_FREE;
		}*/
	}
	
	
}



/*********************************************************************************************************
** Function name:     	DispChaxunPage
** Descriptions:	    输入查询盒饭柜货道的页面
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChaxunHefangPage(uint8_t *keyValue)
{
	char strlanguage[30] = {0},streng[30] = {0};
	//char tempstrlanguage[30] = {0},tempstreng[30] = {0};	
	uint8_t columnState=0,isColumn=0,BinNo=0;
	uint16_t columnNo=0;
	uint32_t debtMoney=0;

	
	if(*keyValue == 'C')
	{
		channelInput = 0;
		channelMode = 0;
		memset(ChannelNum,0,sizeof(ChannelNum));
		memset(BinNum,0,sizeof(BinNum));
		if(GetAmountMoney() > 0)
		{				
			vmcStatus = VMC_SALE;
		}
		else
		{
			ClearDealPar();		
			CLrBusinessText();
			vmcStatus = VMC_FREE;
		}
		return;
	}
	
	//输入柜子编号
	if(channelMode == 0)
	{
		if(*keyValue != 'E')
		{
			BinNum[channelInput++] = *keyValue;	
			strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][18]); 
			strcat(strlanguage,BinNum);		
			strcpy(streng,BUSINESSDEAL[1][18]);	
			strcat(streng,BinNum);	
			DispBusinessText(strlanguage,streng,"","");
		}
		//Trace("\r\n00keyvalue=%c,%s,%s",*keyValue,BinNum,ChannelNum);
		if(channelInput>=2)
		{
			if((BinNum[0]>='0')&&(BinNum[0]<='9')&&(BinNum[1]>='0')&&(BinNum[1]<='9'))
			{
				channelMode = 1;
				channelInput = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				strcpy(strlanguage,BinNum); 
				strcat(strlanguage,BUSINESSDEAL[SystemPara.Language][19]);		
				strcpy(streng,BinNum);	
				strcat(streng,BUSINESSDEAL[1][19]);	
				DispBusinessText(strlanguage,streng,"","");
			}
			else
			{
				channelInput = 0;
				channelMode = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				memset(BinNum,0,sizeof(BinNum));
				if(GetAmountMoney() > 0)
				{				
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();		
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				return;
			}
		}
		//提前按下确认键的操作
		else if(*keyValue == 'E')
		{
			//Trace("\r\n01keyvalue=%c,%s,%s",*keyValue,BinNum,ChannelNum);
			if((BinNum[0]>='0')&&(BinNum[0]<='9'))
			{
				BinNum[1]=BinNum[0];
				BinNum[0]='0';
				channelMode = 1;
				channelInput = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				sprintf(strlanguage,"%s%s",BinNum,BUSINESSDEAL[SystemPara.Language][19]);
				sprintf(streng,"%s%s",BinNum,BUSINESSDEAL[1][19]);
				DispBusinessText(strlanguage,streng,"","");
				//Trace("\r\n1keyvalue=%s,%s",BinNum,ChannelNum);
			}
			else
			{
				channelInput = 0;
				channelMode = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				memset(BinNum,0,sizeof(BinNum));
				if(GetAmountMoney() > 0)
				{				
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();		
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				return;
				//Trace("\r\n02keyvalue=%c,%s,%s",*keyValue,BinNum,ChannelNum);
			}
		}
	}
	//输入本柜格子编号
	else if(channelMode == 1)
	{
		if(*keyValue != 'E')
		{
			ChannelNum[channelInput++] = *keyValue;	
			BinNo= (BinNum[0]-'0')*10+(BinNum[1]-'0');
			sprintf(strlanguage,"%d%s%s",BinNo,BUSINESSDEAL[SystemPara.Language][19],ChannelNum);
			sprintf(streng,"%d%s%s",BinNo,BUSINESSDEAL[1][19],ChannelNum);
			DispBusinessText(strlanguage,streng,"","");
		}
		//Trace("\r\n2keyvalue=%d,%s",BinNo,ChannelNum);//输入E时,BinNo没进去操作，所以为0
		if(channelInput>=2)
		{
			if((ChannelNum[0]>='0')&&(ChannelNum[0]<='9')&&(ChannelNum[1]>='0')&&(ChannelNum[1]<='9'))
			{
				columnNo = GetHefangNum(ChannelNum,BinNum);
				columnState = ChannelCheckIsOk(columnNo%100,columnNo/100);
				if(SystemPara.PcEnable==UBOX_PC)
				{
					ButtonRPTAPI(1,columnNo%100,columnNo/100);//上报pc按键信息
					channelInput = 0;
					channelMode = 0;
					memset(ChannelNum,0,sizeof(ChannelNum));
					memset(BinNum,0,sizeof(BinNum));
					if(GetAmountMoney() > 0)
					{				
						vmcStatus = VMC_SALE;
					}
					else
					{
						ClearDealPar();		
						CLrBusinessText();
						vmcStatus = VMC_FREE;
					}
					return;
				}
				else
				{
					isColumn=1;
				}
				//Trace("\r\ncolumn=%d",columnNo);
			}
			else
			{
				channelInput = 0;
				channelMode = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				memset(BinNum,0,sizeof(BinNum));
				if(GetAmountMoney() > 0)
				{				
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();		
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				return;
			}
		}
		else if(*keyValue == 'E')
		{
			//Trace("\r\n21keyvalue=%c,%s,%s",*keyValue,BinNum,ChannelNum);
			if((ChannelNum[0]>='0')&&(ChannelNum[0]<='9'))
			{
				ChannelNum[1]=ChannelNum[0];
				ChannelNum[0]='0';
				columnNo = GetHefangNum(ChannelNum,BinNum);
				columnState = ChannelCheckIsOk(columnNo%100,columnNo/100);
				if(SystemPara.PcEnable==UBOX_PC)
				{
					ButtonRPTAPI(1,columnNo%100,columnNo/100);//上报pc按键信息
					channelInput = 0;
					channelMode = 0;
					memset(ChannelNum,0,sizeof(ChannelNum));
					memset(BinNum,0,sizeof(BinNum));
					if(GetAmountMoney() > 0)
					{				
						vmcStatus = VMC_SALE;
					}
					else
					{
						ClearDealPar();		
						CLrBusinessText();
						vmcStatus = VMC_FREE;
					}
					return;
				}
				else
				{
					isColumn=1;
				}
				//Trace("\r\ncolumn=%d",columnNo);
			}
			else
			{
				channelInput = 0;
				memset(ChannelNum,0,sizeof(ChannelNum));
				sprintf(strlanguage,"%s%s",BinNum,BUSINESSDEAL[SystemPara.Language][19]);
				sprintf(streng,"%s%s",BinNum,BUSINESSDEAL[1][19]);
				DispBusinessText(strlanguage,streng,"","");
				return;
				//Trace("\r\n22keyvalue=%c,%s,%s",*keyValue,BinNum,ChannelNum);
			}
		}
	}

	if(isColumn)
	{	
		switch(columnState)
		{
			//缺货
			case 2:
			case 4:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][5]);
				strcpy(streng,BUSINESS[1][5]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{	
					channelInput = 0;
					channelMode = 0;
					memset(BinNum,0,sizeof(BinNum));
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;
			//故障
			case 3:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][4]);
				strcpy(streng,BUSINESS[1][4]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{	
					channelInput = 0;
					channelMode = 0;
					memset(BinNum,0,sizeof(BinNum));
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;
			//无此货道	
			case 5:
            case 1:
				strcpy(strlanguage,BUSINESS[SystemPara.Language][1]);
				strcpy(streng,BUSINESS[1][1]);	
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC * 2);
				if(GetAmountMoney() > 0)
				{	
					channelInput = 0;
					channelMode = 0;
					memset(BinNum,0,sizeof(BinNum));
					memset(ChannelNum,0,sizeof(ChannelNum));
					vmcStatus = VMC_SALE;
				}
				else
				{
					ClearDealPar();	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;	
			//正常	
			case 0:									
				vmcPrice = ChannelGetParamValue(columnNo%100,1,columnNo/100);
				//Trace("vmcPrice=%d\r\n",vmcPrice);
				debtMoney = vmcPrice;
				switch(SystemPara.DecimalNum) 
			    {
			      case 2://以分为单位
				  	  sprintf(strlanguage,"%s %s,%s%02d.%02d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/100,debtMoney%100);
					  sprintf(streng,"%s %s,%s%02d.%02d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/100,debtMoney%100);	
					  break;

				  case 1://以角为单位
				  	  debtMoney /= 10;
					  sprintf(strlanguage,"%s %s,%s%d.%d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/10,debtMoney%10);
					  sprintf(streng,"%s %s,%s%d.%d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/10,debtMoney%10);
					  break;
				  
				  case 0://以元为单位
					  sprintf(strlanguage,"%s %s,%s%d",BUSINESS[SystemPara.Language][7],ChannelNum,BUSINESS[SystemPara.Language][3],debtMoney/100);
					  sprintf(streng,"%s %s,%s%d",BUSINESS[1][7],ChannelNum,BUSINESS[1][3],debtMoney/100);
					  break;
			    }	
				//strcpy(streng,BUSINESS[1][3]);
				DispBusinessText(strlanguage,streng,"","");
				OSTimeDly(OS_TICKS_PER_SEC);
				if(GetAmountMoney() > 0)
				{
					vmcColumn = columnNo;
					vmcStatus = VMC_XUANHUO;
				}
				else
				{			
					OSTimeDly(OS_TICKS_PER_SEC);
					ClearDealPar();	
					CLrBusinessText();
					vmcStatus = VMC_FREE;
				}
				break;	
		}		
		channelInput = 0;
		channelMode = 0;
	}
	

}


/*********************************************************************************************************
** Function name:     	DispSalePage
** Descriptions:	    投币进行交易页面
** input parameters:    haveSale是否进行过一次交易
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispSalePage(uint8_t haveSale,uint8_t hefangMode)
{
	char strlanguage[30] = {0},streng[30] = {0};
	char rightstrlanguage[30] = {0},rightstreng[30] = {0};
	//char    *pstr;
	//char	strMoney[10];
	uint32_t dispnum;
	
	//LCDClrScreen();
	OSTimeDly(2);
	//g_Amount += moneyValue;
	//pstr = PrintfMoney(g_Amount);	
	//strcpy(strMoney, pstr);
	//Trace("\r\n2.coin=%s",strMoney);
	//Trace("\r\n money=%ld,%d,%ld,%ld",stDevValue.CoinDecimal,stDevValue.CoinScale,stDevValue.BillDecimal,stDevValue.BillScale);
	dispnum = GetAmountMoney();
	//Trace("\r\n 5money=%ld",dispnum);
	//第一次购买
	if(haveSale==0) 
	{
		//首行提示
		if(SystemPara.Language != 1)
			LCDPrintf(76,LINE3,0,SystemPara.Language,"%s/%s",BUSINESSDEAL[SystemPara.Language][0],BUSINESSDEAL[1][0]);			
		else
			LCDPrintf(96,LINE3,0,SystemPara.Language,"%s",BUSINESSDEAL[1][0]);

		LCDClearLineDraw(0,LINE5,4);
		//金额
		if(SystemPara.Language == 0)
			LCDSpecialPicPrintf(44,LINE6,0,2);//金额符号		
		switch(SystemPara.DecimalNum) 
	    {
	      case 2://以分为单位
		  	  //sprintf(strnum,"%02d.%02d",dispnum/100,dispnum%100);	
			  LCDNumberFontPrintf(76,LINE5,4,"%02d.%02d",dispnum/100,dispnum%100);	
			  break;

		  case 1://以角为单位
		  	  dispnum /= 10;
			  //sprintf(strnum,"%d.%d",dispnum/10,dispnum%10);
			  LCDNumberFontPrintf(76,LINE5,4,"%d.%d",dispnum/10,dispnum%10);	
			  break;
		  
		  case 0://以元为单位
			  //sprintf(strnum,"%d",dispnum/100);
			  LCDNumberFontPrintf(76,LINE5,4,"%d",dispnum/100);	
			  break;
	    }	/**/		
		//交易提示
		//盒饭柜输入提示
		if(hefangMode==1)
		{
			strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][18]); 
			strcpy(streng,BUSINESSDEAL[1][18]);	
		}
		//普通货道输入提示
		else
		{
			strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][4]); 
			strcpy(streng,BUSINESSDEAL[1][4]);	
		}
		DispBusinessText(strlanguage,streng,"","");
	}
	//继续购买
	else
	{
		//首行提示
		if(SystemPara.Language != 1)
			LCDPrintf(76,LINE3,0,SystemPara.Language,"%s/%s",BUSINESSDEAL[SystemPara.Language][1],BUSINESSDEAL[1][1]);			
		else
			LCDPrintf(96,LINE3,0,SystemPara.Language,"%s",BUSINESSDEAL[SystemPara.Language][1]);
		//Trace("\r\n 3money=%ld",dispnum);
		LCDClearLineDraw(0,LINE5,4);
		//金额
		if(SystemPara.Language == 0)
			LCDSpecialPicPrintf(44,LINE6,0,2);//金额符号
		switch(SystemPara.DecimalNum) 
	    {
	      case 2://以分为单位
		  	  //sprintf(strnum,"%02d.%02d",dispnum/100,dispnum%100);	
			  LCDNumberFontPrintf(76,LINE5,4,"%02d.%02d",dispnum/100,dispnum%100);	
			  break;

		  case 1://以角为单位
		  	  dispnum /= 10;
			  //sprintf(strnum,"%d.%d",dispnum/10,dispnum%10);
			  LCDNumberFontPrintf(76,LINE5,4,"%d.%d",dispnum/10,dispnum%10);	
			  break;
		  
		  case 0://以元为单位
			  //sprintf(strnum,"%d",dispnum/100);
			  LCDNumberFontPrintf(76,LINE5,4,"%d",dispnum/100);	
			  break;
	    }		
		//交易提示
		//盒饭柜输入提示
		if(hefangMode==1)
		{
			strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][18]); 
			strcpy(streng,BUSINESSDEAL[1][18]);	
		}
		//普通货道输入提示
		else
		{
			//强制购买	
			if(UserPara.TransEscape == 1)
			{
				strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][20]); 
				strcpy(streng,BUSINESSDEAL[1][20]);	
			}
			else
			{
				strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][5]); 
				strcpy(streng,BUSINESSDEAL[1][5]);	
				strcpy(rightstrlanguage,BUSINESSDEAL[SystemPara.Language][6]); 
				strcpy(rightstreng,BUSINESSDEAL[1][6]);
			}
		}
		DispBusinessText(strlanguage,streng,rightstrlanguage,rightstreng);
	}
}

/*********************************************************************************************************
** Function name:     	DispOverAmountPage
** Descriptions:	    投入纸币超过最大上限
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispOverAmountPage()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][2]); 
	strcpy(streng,BUSINESSDEAL[1][2]);		
	DispBusinessText(strlanguage,streng,"","");
	//OSTimeDly(OS_TICKS_PER_SEC*2);
}

/*********************************************************************************************************
** Function name:     	DispCannotBuyPage
** Descriptions:	    无法购买，因为金额不够
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispCannotBuyPage(uint32_t PriceSale)
{
	char strlanguage[30] = {0},streng[30] = {0},strlanguage2[30] = {0};
	
	
	strcpy(strlanguage2,BUSINESSDEAL[SystemPara.Language][3]); 
	strcpy(streng,BUSINESSDEAL[1][3]);	
	switch(SystemPara.DecimalNum) 
    {
      case 2://以分为单位
	  	  sprintf(strlanguage,"%02d.%02d",PriceSale/100,PriceSale%100);
		  //LCDNumberFontPrintf(51,LINE5,4,"%02d.%02d",debtMoney/100,debtMoney%100);	
		  break;

	  case 1://以角为单位
	  	  PriceSale /= 10;
		  //LCDNumberFontPrintf(51,LINE5,4,"%d.%d",debtMoney/10,debtMoney%10);	
		  sprintf(strlanguage,"%d.%d",PriceSale/10,PriceSale%10);
		  break;
	  
	  case 0://以元为单位
		  //LCDNumberFontPrintf(51,LINE5,4,"%d",debtMoney/100);	
		  sprintf(strlanguage,"%d",PriceSale/100);
		  break;
    }	
	strcat(strlanguage2,strlanguage);
	DispBusinessText(strlanguage2,streng,"","");
	OSTimeDly(OS_TICKS_PER_SEC * 2);
}

/*********************************************************************************************************
** Function name:     	DispCannotBuyPage
** Descriptions:	    无法购买，因为不能找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispCannotBuyAsPayoutPage()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][14]); 
	strcpy(streng,BUSINESSDEAL[1][14]);		
	DispBusinessText(strlanguage,streng,"","");
	OSTimeDly(OS_TICKS_PER_SEC * 2);
}

/*********************************************************************************************************
** Function name:     	DispReaderDevVendoutRPT
** Descriptions:	    读卡器正在扣款，请稍后
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispReaderDevVendoutRPT()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][15]); 
	strcpy(streng,BUSINESSDEAL[1][15]);		
	DispBusinessText(strlanguage,streng,"","");
	OSTimeDly(OS_TICKS_PER_SEC / 10);
}

/*********************************************************************************************************
** Function name:     	DispReaderVendoutFail
** Descriptions:	    读卡器扣款失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispReaderVendoutFail()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][16]); 
	strcpy(streng,BUSINESSDEAL[1][16]);		
	DispBusinessText(strlanguage,streng,"","");
	OSTimeDly(OS_TICKS_PER_SEC * 2);
}



/*********************************************************************************************************
** Function name:     	DispChuhuoPage
** Descriptions:	    出货
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChuhuoPage()
{
	char strlanguage[30] = {0},streng[30] = {0};

	sprintf(strlanguage,"%d %s",vmcColumn%100,BUSINESSDEAL[SystemPara.Language][11]);
	sprintf(streng,"%d %s",vmcColumn%100,BUSINESSDEAL[1][11]);
	//strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][11]); 
	//strcpy(streng,BUSINESSDEAL[1][11]);		
	DispBusinessText(strlanguage,streng,"","");	
	//OSTimeDly(OS_TICKS_PER_SEC / 5);
}


/*********************************************************************************************************
** Function name:     	DispChuhuoPagePC
** Descriptions:	    PC指示出货
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChuhuoPagePC()
{
	char strlanguage[30] = {0},streng[30] = {0};

	sprintf(strlanguage,"%s",BUSINESSDEAL[SystemPara.Language][11]);
	sprintf(streng,"%s",BUSINESSDEAL[1][11]);
	//strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][11]); 
	//strcpy(streng,BUSINESSDEAL[1][11]);		
	DispBusinessText(strlanguage,streng,"","");	
	//OSTimeDly(OS_TICKS_PER_SEC / 2);
}


/*********************************************************************************************************
** Function name:     	DispQuhuoPage
** Descriptions:	    取货
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispQuhuoPage()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][12]); 
	strcpy(streng,BUSINESSDEAL[1][12]);		
	DispBusinessText(strlanguage,streng,"","");	
	OSTimeDly(OS_TICKS_PER_SEC / 2);
}

/*********************************************************************************************************
** Function name:     	DispChhuoFailPage
** Descriptions:	    出货失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispChhuoFailPage()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSDEAL[SystemPara.Language][13]); 
	strcpy(streng,BUSINESSDEAL[1][13]);		
	DispBusinessText(strlanguage,streng,"","");	
	OSTimeDly(OS_TICKS_PER_SEC * 2);
}

/*********************************************************************************************************
** Function name:     	DispPayoutPage
** Descriptions:	    正在找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispPayoutPage()
{
	char strlanguage[30] = {0},streng[30] = {0};

	//LCDClrScreen();
	/*
	switch(SystemPara.DecimalNum) 
    {
      case 2://以分为单位
	  	  //sprintf(strnum,"%02d.%02d",dispnum/100,dispnum%100);	
		  LCDNumberFontPrintf(76,LINE5,4,"%02d.%02d",0/100,0%100);	
		  break;

	  case 1://以角为单位
	  	  //sprintf(strnum,"%d.%d",dispnum/10,dispnum%10);
		  LCDNumberFontPrintf(76,LINE5,4,"%d.%d",0/10,0%10);	
		  break;
	  
	  case 0://以元为单位
		  //sprintf(strnum,"%d",dispnum/100);
		  LCDNumberFontPrintf(76,LINE5,4,"%d",0/100);	
		  break;
    }*/	
	strcpy(strlanguage,BUSINESSCHANGE[SystemPara.Language][0]); 
	strcpy(streng,BUSINESSCHANGE[1][0]);		
	DispBusinessText(strlanguage,streng,"","");	
	//OSTimeDly(OS_TICKS_PER_SEC * 2);
}

/*********************************************************************************************************
** Function name:     	DispQuChangePage
** Descriptions:	    取零钱
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispQuChangePage()
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	strcpy(strlanguage,BUSINESSCHANGE[SystemPara.Language][2]); 
	strcpy(streng,BUSINESSCHANGE[1][2]);		
	DispBusinessText(strlanguage,streng,"","");	
	OSTimeDly(OS_TICKS_PER_SEC / 2);
}

/*********************************************************************************************************
** Function name:     	DispIOUPage
** Descriptions:	    找零失败
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispIOUPage(uint32_t debtMoney)
{
	char strlanguage[30] = {0},streng[30] = {0};
	
	switch(SystemPara.DecimalNum) 
    {
      case 2://以分为单位
	  	  sprintf(strlanguage,"%s%02d.%02d",BUSINESSCHANGE[SystemPara.Language][1],debtMoney/100,debtMoney%100);
		  //LCDNumberFontPrintf(51,LINE5,4,"%02d.%02d",debtMoney/100,debtMoney%100);	
		  break;

	  case 1://以角为单位
	  	  debtMoney /= 10;
		  //LCDNumberFontPrintf(51,LINE5,4,"%d.%d",debtMoney/10,debtMoney%10);	
		  sprintf(strlanguage,"%s%d.%d",BUSINESSCHANGE[SystemPara.Language][1],debtMoney/10,debtMoney%10);
		  break;
	  
	  case 0://以元为单位
		  //LCDNumberFontPrintf(51,LINE5,4,"%d",debtMoney/100);	
		  sprintf(strlanguage,"%s%d",BUSINESSCHANGE[SystemPara.Language][1],debtMoney/100);
		  break;
    }	
	strcpy(streng,BUSINESSCHANGE[1][1]);	
	//sprintf(streng,"%ld",debtMoney);
	DispBusinessText(strlanguage,streng,"","");	
	OSTimeDly(OS_TICKS_PER_SEC * 5);
}

/*********************************************************************************************************
** Function name:     	DispEndPage
** Descriptions:	    交易结束
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispEndPage()
{
	char strlanguage[30] = {0};
	LCDClrScreen();
	OSTimeDly(2);
	//LCDPutLOGOBmp(75,LINE1,SystemPara.Logo);
	strcpy(strlanguage,BUSINESSCHANGE[1][3]); 
	LCDNumberFontPrintf(48,LINE6,3,strlanguage);
	if(SystemPara.Language != 1)
	{
		//strcpy(strlanguage,BUSINESSCHANGE[SystemPara.Language][3]);
		//LCDPrintf(32,LINE7,0,SystemPara.Language,strlanguage);
		strcpy(strlanguage,BUSINESSCHANGE[SystemPara.Language][4]);
		LCDPrintf(60,LINE12,0,SystemPara.Language,strlanguage);
	}	
	//OSTimeDly(OS_TICKS_PER_SEC * 2);
}

/*********************************************************************************************************
** Function name:     	GetAmountMoney
** Descriptions:	    投币总金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetAmountMoney()
{	
	return g_coinAmount + g_billAmount + g_holdValue + g_readerAmount;
}

/*********************************************************************************************************
** Function name:     	GetHoldMoney
** Descriptions:	    暂存纸币金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetHoldMoney()
{	
	return g_holdValue;
}

/*********************************************************************************************************
** Function name:     	ClearHoldMoney
** Descriptions:	    清除纸币金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClearHoldMoney()
{	
	g_holdValue=0;
}

/*********************************************************************************************************
** Function name:     	GetReaderAmount
** Descriptions:	    刷卡总金额总金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t GetReaderAmount()
{	
	return g_readerAmount;
}



/*********************************************************************************************************
** Function name:       UpdateErrBill
** Descriptions:        遇到纸币器收币后，不显示金额时，自动予以退币和复位
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UpdateErrBill(void)
{		
	if( (GetAmountMoney()==0)&&(vmcChangeLow==0)&&(DeviceStateBusiness.Billdisable==1) )
	{
		TraceBill("\r\n ErrBill=%d,%d,%d",GetAmountMoney(),vmcChangeLow,DeviceStateBusiness.Billdisable);
		ReturnBillDevMoneyInAPI();
		OSTimeDly(OS_TICKS_PER_SEC / 10);
		BillCoinCtr(1,0,0);	
		OSTimeDly(OS_TICKS_PER_SEC / 10);
	}
}

/*********************************************************************************************************
** Function name:     	rstTime
** Descriptions:	    屏幕显示时间
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void rstTime()
{
	vmRTC.year = 0;
	vmRTC.month =0;
	vmRTC.day =0;
	vmRTC.hour = 0;
	vmRTC.minute =0;
	Timer.DispFreeTimer=1;
}

//设置是否开启现金设备1,开启,2关闭
void BillCoinEnable(uint8_t enable)
{	
	uint8_t billCtr=0,coinCtr=0,readerCtr=0;
	//启用
	if(enable==1)
	{
		//纸币器
		if((GetBillCoinStatus(1)==0)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
		{
			billCtr=1;
			vmcChangeLow = 0;
			LCDClrScreen();
			rstTime();			
		}
		//硬币器
		if((GetBillCoinStatus(2)==0)
			&&((SystemPara.CoinAcceptorType == PARALLEL_COINACCEPTER)||(SystemPara.CoinAcceptorType == SERIAL_COINACCEPTER))			
		)
		{
			coinCtr=1;
		}
	}
	//关闭
	else if(enable==2)
	{
		//纸币器
		if((GetBillCoinStatus(1)==1)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
		{
			billCtr=2;
			vmcChangeLow = 1;
			LCDClrScreen();
			rstTime();
		}
		//硬币器
		if((GetBillCoinStatus(2)==1)
			&&((SystemPara.CoinAcceptorType == PARALLEL_COINACCEPTER)||(SystemPara.CoinAcceptorType == SERIAL_COINACCEPTER))
			&&(SystemPara.HpEmpCoin!=1)
		)
		{
			coinCtr=2;
		}
	}

	BillCoinCtr(billCtr,coinCtr,readerCtr); 
	//有做纸币器操作之后，需要上报设备状态
	if(billCtr>0)
	{
		TracePC(">>status1");
		StatusRPTAPI();
	}
}

/*********************************************************************************************************
** Function name:     	UnpdateTubeMoney
** Descriptions:	    更新可找零金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t UnpdateTubeMoney()
{
	uint32_t coinMoney=0,billMoney=0;	
	uint8_t i,j=0;
	//ChangeGetTubesAPI();
	//OSTimeDly(OS_TICKS_PER_SEC / 10);
	//1.添加纸币循环器金额
	if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
	{
		for(i=0;i<7;i++)
		{
			if(stDevValue.RecyclerValue[i])
			{
				billMoney = billMoney + stDevValue.RecyclerValue[i]*stDevValue.RecyclerNum[i];
				j=i;
			}
		}
	}
	//2.添加硬币存币斗金额
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		coinMoney = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
					+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5]
					+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
					+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
					+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
					+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; 
	}
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{		
		coinMoney =( !HopperIsEmpty() )? SystemPara.MaxValue:0;
	}
	else if(SystemPara.CoinChangerType == OFF_CHANGER)
	{
		coinMoney = 0;
	}
	//3.设置最高投币上限
	MoneyMaxin =( (billMoney + coinMoney) > SystemPara.MaxValue )? SystemPara.MaxValue:(billMoney + coinMoney);
	TraceBill("\r\nAppChange 5=%ld,1=%ld,rec=%ld,bill=%ld,coin=%ld,max=%ld,money=%ld,enable=%ld",stDevValue.CoinValue[0]*stDevValue.CoinNum[0],stDevValue.CoinValue[1]*stDevValue.CoinNum[1],stDevValue.RecyclerValue[j]*stDevValue.RecyclerNum[j],
		billMoney,coinMoney,SystemPara.MaxValue,MoneyMaxin,SystemPara.BillEnableValue);

	//4.是否禁用现金设备
	//友宝成人由友宝控制，不自己禁能使能设备
	if(SystemPara.PcEnable==CRUBOX_PC)
	{
	}
	else if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		if(coinMoney < SystemPara.BillEnableValue)
		{
			BillCoinEnable(2);
		}
		else
		{
			BillCoinEnable(1);
		}
	}	
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{		
		if(coinMoney < SystemPara.BillEnableValue)
		{
			BillCoinEnable(2);
		}
		else
		{
			BillCoinEnable(1);
		}
	}
	else if(SystemPara.CoinChangerType == OFF_CHANGER)
	{
		//添加纸币循环器金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			if((billMoney + coinMoney) < SystemPara.BillEnableValue)
			{
				BillCoinEnable(2);
			}
			else
			{
				BillCoinEnable(1);
			}
		}		
		else
		{
			MoneyMaxin =SystemPara.MaxValue;
			TraceBill("\r\nAppChange max=%ld",PriceMaxin);
		}
	}
	//遇到纸币器收币后，不显示金额时，自动予以退币和复位
	UpdateErrBill();
	return 0;
}
/*
uint8_t UnpdateTubeMoney()
{
	uint32_t coinMoney=0;	
	uint8_t i,j=0;
	//ChangeGetTubesAPI();
	//OSTimeDly(OS_TICKS_PER_SEC / 10);
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		coinMoney = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
					+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5]
					+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
					+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
					+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
					+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; 
		//添加纸币循环器金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			for(i=0;i<7;i++)
			{
				if(stDevValue.RecyclerValue[i])
				{
					coinMoney = coinMoney + stDevValue.RecyclerValue[i]*stDevValue.RecyclerNum[i];
					j=i;
				}
			}
		}
		MoneyMaxin =( coinMoney > SystemPara.MaxValue )? SystemPara.MaxValue:coinMoney;
		TraceBill("\r\nAppChange 5=%ld,1=%ld,rec=%ld,money=%ld,max=%ld,bill=%ld",stDevValue.CoinValue[0]*stDevValue.CoinNum[0],stDevValue.CoinValue[1]*stDevValue.CoinNum[1],stDevValue.RecyclerValue[j]*stDevValue.RecyclerNum[j],coinMoney,MoneyMaxin,SystemPara.BillEnableValue);
		if(MoneyMaxin < SystemPara.BillEnableValue)
		{
			if((GetBillCoinStatus(1)==1)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
			{
				BillCoinCtr(2,0,0);	
				vmcChangeLow = 1;
				LCDClrScreen();
				rstTime();
			}
		}
		else
		{
			if((GetBillCoinStatus(1)==0)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
			{
				BillCoinCtr(1,0,0);	
				vmcChangeLow = 0;
				LCDClrScreen();
				rstTime();
			}
		}
	}	
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{		
		coinMoney =( !HopperIsEmpty() )? SystemPara.MaxValue:0;
		TraceBill("\r\nAppChange coinMoney=%ld",coinMoney);
		//添加纸币循环器金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			for(i=0;i<7;i++)
			{
				if(stDevValue.RecyclerValue[i])
				{
					coinMoney = coinMoney + stDevValue.RecyclerValue[i]*stDevValue.RecyclerNum[i];
					j=i;
				}
			}
		}
		MoneyMaxin =( coinMoney > SystemPara.MaxValue )? SystemPara.MaxValue:coinMoney;
		TraceBill("\r\nAppChange 5=%ld,1=%ld,rec=%ld,money=%ld,max=%ld,bill=%ld",stDevValue.CoinValue[0]*stDevValue.CoinNum[0],stDevValue.CoinValue[1]*stDevValue.CoinNum[1],stDevValue.RecyclerValue[j]*stDevValue.RecyclerNum[j],coinMoney,MoneyMaxin,SystemPara.BillEnableValue);
		if(MoneyMaxin < SystemPara.BillEnableValue)
		{
			if((GetBillCoinStatus(1)==1)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
			{
				BillCoinCtr(2,0,0);	
				vmcChangeLow = 1;
				LCDClrScreen();
				rstTime();
			}
		}
		else
		{
			if((GetBillCoinStatus(1)==0)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
			{
				BillCoinCtr(1,0,0);	
				vmcChangeLow = 0;
				LCDClrScreen();
				rstTime();
			}
		}
	}
	else if(SystemPara.CoinChangerType == OFF_CHANGER)
	{
		//添加纸币循环器金额
		if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
		{
			for(i=0;i<7;i++)
			{
				if(stDevValue.RecyclerValue[i])
				{
					coinMoney = coinMoney + stDevValue.RecyclerValue[i]*stDevValue.RecyclerNum[i];
					j=i;
				}
			}
			MoneyMaxin =( coinMoney > SystemPara.MaxValue )? SystemPara.MaxValue:coinMoney;
			TraceBill("\r\nAppChange rec=%ld,coinMoney=%ld,max=%ld,bill=%ld",stDevValue.RecyclerValue[j]*stDevValue.RecyclerNum[j],coinMoney,MoneyMaxin,SystemPara.BillEnableValue);
			if(MoneyMaxin < SystemPara.BillEnableValue)
			{
				if((GetBillCoinStatus(1)==1)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
				{
					BillCoinCtr(2,0,0);	
					vmcChangeLow = 1;
					LCDClrScreen();
					rstTime();
				}
			}
			else
			{
				if((GetBillCoinStatus(1)==0)&&(SystemPara.BillValidatorType != OFF_BILLACCEPTER))
				{
					BillCoinCtr(1,0,0);	
					vmcChangeLow = 0;
					LCDClrScreen();
					rstTime();
				}
			}
		}		
		else
		{
			MoneyMaxin =SystemPara.MaxValue;
			TraceBill("\r\nAppChange max=%ld",PriceMaxin);
		}
	}
	//遇到纸币器收币后，不显示金额时，自动予以退币和复位
	UpdateErrBill();
	return 0;
}
*/

/*********************************************************************************************************
** Function name:     	StackReturnBillDev
** Descriptions:	    暂存的纸币压超或退出
** input parameters:    2压超,3退出
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void StackReturnBillDev(uint8_t type)
{
	uint32_t InValue;	
	InValue=g_holdValue;
	switch(type)
	{
		case 2://暂存压超
			TracePC("\r\n App Escrowin");
			if(g_holdValue==0)
			{
				PayinRPTAPI(3,0,GetAmountMoney());//上报PC端
			}
			else
			{
				if(StackedBillDevMoneyInAPI())
				{
					g_billAmount += InValue;
					g_holdValue=0;	
					LogGetMoneyAPI(InValue,2);//记录日志
					PayinRPTAPI(2,InValue,GetAmountMoney());//上报PC端				
				}
				else
				{
					ReturnBillDevMoneyInAPI();
					g_holdValue=0;
					PayinRPTAPI(4,InValue,GetAmountMoney());//上报PC端
				}
			}
			break;
		case 3://暂存退出
			TracePC("\r\n App Escrowout");
			if(g_holdValue==0)
			{
				PayinRPTAPI(4,0,GetAmountMoney());//上报PC端
			}
			else
			{
				ReturnBillDevMoneyInAPI();
				g_holdValue=0;
				PayinRPTAPI(4,InValue,GetAmountMoney());//上报PC端
			}
			break;	
	}	

	//产生状态变化
	if(GetAmountMoney())
	{					
		vmcStatus = VMC_SALE;
		Timer.SaleTimer = SaleTimeSet(0);
		if(vmcStatus == VMC_SALE)
		{
			//Trace("\r\n 1money=%ld",GetAmountMoney());
			if(g_billAmount< MoneyMaxin)							
				BillCoinCtr(1,1,0);
			//Trace("\r\n 2money=%ld",GetAmountMoney());
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			DispSalePage(0,hefangMode);
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			SaleSelectionKeyAPI(GetAmountMoney());
		}
	}							
	else
	{
		BillCoinCtr(1,1,0);
		channelInput = 0;
		channelMode = 0;
		memset(BinNum,0,sizeof(BinNum));
		memset(ChannelNum,0,sizeof(ChannelNum));
		//Trace("\r\n 3money=%ld",GetAmountMoney());
		OSMboxAccept(g_CoinMoneyBackMail);
		LCDClrScreen();
		DispEndPage();
		vmcStatus = VMC_END;
	}
}

/*********************************************************************************************************
** Function name:     	GetMoney
** Descriptions:	    得到交易金额
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t GetMoney()
{
	uint32_t InValue;	
	static uint8_t readerType = 0;
	
	//1.硬币轮询
	if(SystemPara.CoinAcceptorType != OFF_COINACCEPTER)
	{
		InValue=GetCoinDevMoneyInAPI();
		if(InValue>0)
		{			
			g_coinAmount += InValue;
			/*
			if(GetAmountMoney() >= 5000)
			{
				if(g_holdValue > 0)
					BillCoinCtr(0,2,2);
				else
					BillCoinCtr(2,2,2);	
			}
			else
			*/
			TraceCoin("\r\nAppcoin1.coin=%ld,%ld",InValue,GetAmountMoney());
			{
				BillCoinCtr(0,0,2);	
			}			
			LogGetMoneyAPI(InValue,1);//记录日志
			PayinRPTAPI(1,InValue,GetAmountMoney());//上报PC端
			return 1;		
		}
	}
	//2.纸币轮询
	if(SystemPara.BillValidatorType != OFF_BILLACCEPTER)
	{
		InValue=GetBillDevMoneyUnStackInAPI();
		if(InValue>0)
		{
			TraceBill("\r\n2.Appbill=%ld",InValue);
			TraceBill("\r\nAppbill=%ld,Money=%ld,price=%ld\r\n",(g_billAmount + InValue),MoneyMaxin,PriceMaxin);
			if(SystemPara.PcEnable==CRUBOX_PC)//友宝成人模块，收币后暂存
			{
				g_holdValue = InValue;
				PayinRPTAPI(3,InValue,GetAmountMoney());//上报PC端
				InValue = 0;
			}
			else if( ((g_billAmount + InValue) < MoneyMaxin) )
			{
				if(StackedBillDevMoneyInAPI())
				{
					g_billAmount += InValue;
					LogGetMoneyAPI(InValue,2);//记录日志
					PayinRPTAPI(2,InValue,GetAmountMoney());//上报PC端
					InValue = 0;
				}
				else
				{
					ReturnBillDevMoneyInAPI();
					return 0;
				}
			}
			else
			{
				if(SystemPara.BillITLValidator == ITL_BILLACCEPTER)
				{
					DispOverAmountPage();					
					if(GetAmountMoney())
					{
						OSTimeDly(350);
						Timer.SaleTimer = SaleTimeSet(0);
						vmcStatus = VMC_SALE;
						channelInput = 0;
						channelMode = 0;
						memset(BinNum,0,sizeof(BinNum));
						memset(ChannelNum,0,sizeof(ChannelNum));
						OSMboxAccept(g_CoinMoneyBackMail);
						vmcPrice = 0;
						DispSalePage(0,hefangMode);
						ReturnBillDevMoneyInAPI();	
					}
					else
					{
						ClearDealPar();	
						vmcStatus = VMC_FREE;
						ReturnBillDevMoneyInAPI();	
						OSTimeDly(OS_TICKS_PER_SEC*2);
						CLrBusinessText();
						rstTime();
					}
					g_holdValue = 0;
					return 0;
				}
				else
				{
					g_holdValue = InValue;
					PayinRPTAPI(3,InValue,GetAmountMoney());//上报PC端
					InValue = 0;
				}
			}
			if((g_billAmount + InValue) >= MoneyMaxin)
			{}	//BillCoinCtr(2,0,2);	
			else
				BillCoinCtr(0,0,2);		
			return 1;
		}
	}

	//3.读卡器轮询
	if(SystemPara.CashlessDeviceType != OFF_READERACCEPTER)
	{
		InValue = GetReaderDevMoneyInAPI(&readerType);
		if( (readerType == 1)&&(InValue > 0) )
		{
			TraceReader("\r\n AppReader1.read=%ld",InValue);
			g_readerAmount = InValue;
			LogGetMoneyAPI(InValue,3);//记录日志
			BillCoinCtr(2,2,0);
			PayinRPTAPI(5,InValue,GetAmountMoney());//上报PC端
			return 1;		
		}
		else if(readerType == 2)
		{		
			TraceReader("\r\n AppReader OUTPUT card");
			PayoutRPTAPI(2,0,GetAmountMoney(),0,0);
			readerType = 0;
			g_readerAmount = 0;
			return 2;
		}
	}
	
	return 0;
}

/*********************************************************************************************************
** Function name:       ChangerRecycler
** Descriptions:        纸币循环找零器找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangerRecycler(void)
{
	uint8_t i,num,status;
	uint32_t paymoney=0;
	uint32_t  RecyPayoutMoneyBack=0;
	if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
	{
		paymoney=GetAmountMoney();
		if(paymoney)
		{
			TraceBill("\r\n AppNeedpay=%ld",paymoney);
			for(i=0;i<7;i++)
			{
				if(stDevValue.RecyclerValue[i])
				{
					num=paymoney/stDevValue.RecyclerValue[i];
					TraceBill("\r\n AppNeednum=%d",num);
					num=(stDevValue.RecyclerNum[i]>=num)?num:stDevValue.RecyclerNum[i];
					TraceBill("\r\n AppRealnum=%d",num);
					status=BillRecyclerPayoutValueExpanseAPI(stDevValue.RecyclerValue[i]*num,&RecyPayoutMoneyBack);
					TraceBill("\r\n AppRecback st=%d,pay=%ld",status,RecyPayoutMoneyBack);
					if(status==1)
					{
						paymoney = (g_coinAmount + g_billAmount)-RecyPayoutMoneyBack;
						g_billAmount=0;
						g_coinAmount = paymoney;
						TraceBill("\r\n Apppay=%ld",paymoney);
						if(SystemPara.EasiveEnable == 1)
						{
							PayoutRPTAPI(1,0,RecyPayoutMoneyBack,0,paymoney);
						}
					}
					break;
				}
			}
		}
	}
	else if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)
	{
		paymoney=GetAmountMoney();
		if(paymoney)
		{
			print_fs("\r\n AppNeedpay=%ld",paymoney);				
			num=paymoney/SystemPara.RecyclerMoney;
			print_fs("\r\n AppNeednum=%d",num);
			num=(stDevValue.RecyclerNum[0]>=num)?num:stDevValue.RecyclerNum[0];
			print_fs("\r\n AppRealnum=%d",num);
			status=BillRecyclerPayoutValueExpanseAPI(SystemPara.RecyclerMoney*num,&RecyPayoutMoneyBack);
			print_fs("\r\n AppRecback st=%d,pay=%ld",status,RecyPayoutMoneyBack);
			if(status==1)
			{
				paymoney = (g_coinAmount + g_billAmount)-RecyPayoutMoneyBack;
				g_billAmount=0;
				g_coinAmount = paymoney;
				print_fs("\r\n Apppay=%ld",paymoney);
				if(SystemPara.EasiveEnable == 1)
				{
					PayoutRPTAPI(1,0,RecyPayoutMoneyBack,0,paymoney);
				}
			}				
			
		}
	}
}

/*********************************************************************************************************
** Function name:       ChangerMoney
** Descriptions:        找零
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint32_t ChangerMoney(void)
{	
	uint32_t backmoney;
	unsigned char ComStatus;
	uint32_t tempmoney;//用来给友宝上报纸币和硬币找零金额之总和的

	//暂存退币
	if(g_holdValue)
	{
		if(ReturnBillDevMoneyInAPI())
		{
			PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
			g_holdValue = 0;			
		}
		TracePC("\r\n AppHoldRet%d",OSTimeGet());
		OSTimeDly(OS_TICKS_PER_SEC/2);
	}
	tempmoney=GetAmountMoney();//用来给友宝上报纸币和硬币找零金额之总和的
	//纸币循环找零器
	ChangerRecycler();
	//找零硬币
	if(GetAmountMoney())
	{
		TracePC("\r\n AppBegin change%d",OSTimeGet());
		//OSTimeDly(OS_TICKS_PER_SEC*2);
		//ActionRPTAPI(2,0,30,0,0,GetAmountMoney(),GetAmountMoney());
		//OSTimeDly(OS_TICKS_PER_SEC/2);
		ComStatus = ChangerDevPayoutAPI(GetAmountMoney(),&backmoney);		
	}
	else
	{
		ComStatus = 1;
	}

	if(SystemPara.EasiveEnable == 1)
	{
		//找零失败
		if(!ComStatus)
		{		
			TracePC("\r\n Appchange Fail=%ld,%ld",GetAmountMoney(),backmoney);
			LogChangeAPI(GetAmountMoney()-backmoney,backmoney);//记录日志
			PayoutRPTAPI(0,0,GetAmountMoney()-backmoney,0,0);
			//OSTimeDly(OS_TICKS_PER_SEC);
			//PayinRPTAPI(2,0,0);//上报PC端
			g_coinAmount = 0;
			g_billAmount = 0;
			return  backmoney;
		}
		//找零成功
		else
		{
			TracePC("\r\n Appchange succ=%ld",GetAmountMoney());
			LogChangeAPI(GetAmountMoney(),0);//记录日志
			PayoutRPTAPI(0,0,GetAmountMoney(),0,0);
			g_coinAmount = 0;
			g_billAmount = 0;
			return 0;
		}
	}
	else
	{
		//找零失败
		if(!ComStatus)
		{		
			TracePC("\r\n Appchange Fail");
			LogChangeAPI(tempmoney-backmoney,backmoney);//记录日志
			PayoutRPTAPI(0,0,tempmoney-backmoney,0,0);
			//OSTimeDly(OS_TICKS_PER_SEC);
			//PayinRPTAPI(2,0,0);//上报PC端
			g_coinAmount = 0;
			g_billAmount = 0;
			return  backmoney;
		}
		//找零成功
		else
		{
			TracePC("\r\n Appchange succ%d",OSTimeGet());
			LogChangeAPI(tempmoney,0);//记录日志
			PayoutRPTAPI(0,0,tempmoney,0,0);
			g_coinAmount = 0;
			g_billAmount = 0;
			return 0;
		}
	}	
}


/*********************************************************************************************************
** Function name:     	GetMoney
** Descriptions:	    回到空闲状态前，先清除交易的数据
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ClearDealPar()
{
	channelInput = 0;
	channelMode = 0;
	memset(BinNum,0,sizeof(BinNum));
	memset(ChannelNum,0,sizeof(ChannelNum));
	//g_Amount = 0;
	Timer.DispFreeTimer = 0;
	OSMboxAccept(g_CoinMoneyBackMail);
}

/*********************************************************************************************************
** Function name:     	BillCoinEnable
** Descriptions:	    使能禁能纸币器硬币器
** input parameters:    billCtr控制纸币器=1,使能,2禁能,0不操作
                        coinCtr控制硬币器=1,使能,2禁能,0不操作
                        readerCtr控制读卡器=1,使能,2禁能,0不操作
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BillCoinCtr(uint8_t billCtr,uint8_t coinCtr,uint8_t readerCtr)
{
	//1.纸币器控制
	if(SystemPara.BillValidatorType != OFF_BILLACCEPTER)
	{
		switch(billCtr)
		{
			case 1:
				TraceBill("\r\n AppenBill");
				BillDevEnableAPI();
				SetBillCoinStatus(1,1);
				break;
			case 2:
				TraceBill("\r\n AppdisBill");
				BillDevDisableAPI();
				SetBillCoinStatus(1,0);
				break;
		}
	}
	//2.硬币器控制
	if(SystemPara.CoinAcceptorType != OFF_COINACCEPTER)
	{
		switch(coinCtr)
		{
			case 1:
				TraceBill("\r\n AppenCoin");
				CoinDevEnableAPI();
				SetBillCoinStatus(2,1);
				break;
			case 2:
				TraceBill("\r\n AppdisCoin");
				CoinDevDisableAPI();
				SetBillCoinStatus(2,0);
				break;
		}
	}
	//3.读卡器控制
	if(SystemPara.CashlessDeviceType != OFF_READERACCEPTER)
	{
		switch(readerCtr)
		{
			case 1:
				ReaderDevEnableAPI();
				break;
			case 2:
				ReaderDevDisableAPI();
				break;
		}
	}
	OSTimeDly(OS_TICKS_PER_SEC / 10);
}

/*********************************************************************************************************
** Function name:     	ResetBill
** Descriptions:	    重启纸币器
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ResetBill()
{
	//1.纸币器控制
	if(SystemPara.BillValidatorType != OFF_BILLACCEPTER)
	{
		TraceBill("\r\n AppresetBill");
		BillDevResetAPI();
		SetBillCoinStatus(1,1);
	}	
	OSTimeDly(OS_TICKS_PER_SEC / 10);
}

/*********************************************************************************************************
** Function name:     	IsCanSale
** Descriptions:	    判断是否可以出货
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void IsCanSale()
{
	uint32_t tempPrice;
	//uint8_t resultdisp[8]={0, 0, 0, 0, 0, 0, 0, 0};
	//uint8_t recoin;
	unsigned char ComStatus = 0;
	uint32_t 	PriceSale  = 0;//当前购买的商品

	PriceSale  = vmcPrice;
	//商品单价大于现投入金额,而且是使用现金交易的
	if((PriceSale > GetAmountMoney())&&(g_readerAmount==0))
	{
		vmcStatus = VMC_LESSMONEY;
	}
	//使用读卡器交易
	else if(g_readerAmount > 0)
	{
		DispReaderDevVendoutRPT();
		ComStatus = ReaderDevVendoutRPTAPI(PriceSale);
		if(ComStatus == 1)
		{
			vmcStatus = VMC_CHUHUO;
		}
		else
		{
			vmcStatus = VMC_READVENDFAIL;
		}	
	}
	//商品单价大于压抄的纸币和投入硬币金额之和
	else if(PriceSale > (g_coinAmount + g_billAmount))
	{
		tempPrice = PriceSale - g_coinAmount - g_billAmount;//商品单价减去压抄的纸币和投入硬币金额之和
		tempPrice = g_holdValue - tempPrice;               //暂存的金额再减去还剩下的金额，就是要找零的金额
		if(SystemPara.CoinChangerType == MDB_CHANGER)
		{
			//ComStatus = MDBchange(resultdisp, &recoin, tempPrice);//检查这些剩余的金额是否可以找零
			ComStatus = 1;
		}	
		else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
		{
			ComStatus = 1;
		}
		else if(SystemPara.CoinChangerType == OFF_CHANGER)
		{
			//如果纸币循环器可用，可用他找纸笔，因此可以出货
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				ComStatus = 1;
			}
			else
			{
				if(tempPrice==0)
					ComStatus = 1;
				else
					ComStatus = 0;
			}
		}
		//如果可以找零
		if(ComStatus == 1)
		{
			//压抄成功
			if(StackedBillDevMoneyInAPI())
			{				
				g_billAmount += g_holdValue;
				LogGetMoneyAPI(g_holdValue,2);//记录日志
				PayinRPTAPI(2,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;
				vmcStatus = VMC_CHUHUO;
			}
			//压抄失败
			else
			{	
				ReturnBillDevMoneyInAPI();
				PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;
				vmcStatus = VMC_CHANGESHORT;
			}
		}
		else
		{
			vmcStatus = VMC_CHANGESHORT;
		}
	}
	//商品单价小于等于压抄的纸币和投入硬币金额之和
	else
	{
		tempPrice = (g_coinAmount + g_billAmount) - PriceSale;//纸币和投入硬币金额之和减去商品单价
		if(SystemPara.CoinChangerType == MDB_CHANGER)
		{
			//ComStatus = MDBchange(resultdisp, &recoin, tempPrice);//检查这些剩余的金额是否可以找零
			ComStatus = 1;
		}	
		else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
		{
			ComStatus = 1;
		}
		else if(SystemPara.CoinChangerType == OFF_CHANGER)
		{
			//如果纸币循环器可用，可用他找纸笔，因此可以出货
			if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
			{
				ComStatus = 1;
			}
			else
			{
				if(tempPrice==0)
					ComStatus = 1;
				else
					ComStatus = 0;
			}
		}
		//如果可以找零
		if(ComStatus == 1)
		{
			if((SystemPara.BillITLValidator == ITL_BILLACCEPTER)&&(g_holdValue > 0))
			{
				//Trace("\r\n ITLReturn");
				ReturnBillDevMoneyInAPI();
				PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;
			}
			vmcStatus = VMC_CHUHUO;
		}
		else
		{
			vmcStatus = VMC_CHANGESHORT;
		}		
	}
}

/*********************************************************************************************************
** Function name:     	SaleCostMoney
** Descriptions:	    出货后扣除金额
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SaleCostMoney(uint32_t PriceSale)
{	
	
	//扣除单价
	if(PriceSale)
	{
		//向读卡器发送交易成功
		if(g_readerAmount > 0)
		{
			TraceReader("\r\n AppCHuhuoSucc");
			ReaderDevVendoutResultAPI(1);
			g_readerAmount -= PriceSale;
			PriceSale = 0;
		}		
		else if(PriceSale >= g_billAmount)
		{
			PriceSale -= g_billAmount;
			g_billAmount = 0;
		}
		else
		{
			g_billAmount -= PriceSale;
			PriceSale = 0;
		}
	}
	if(PriceSale)
	{
		g_coinAmount -= PriceSale;					
	}	
}

/*********************************************************************************************************
** Function name:     	IsSaleTimeOver
** Descriptions:	    交易倒计时定时器结束交易
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t IsSaleTimeOver()
{
	if(SystemPara.SaleTime < 255)
	{
		if(Timer.SaleTimer == 0)
			return 1;
		else
			return 0;
	}
	else 
	{
		return 0;
	}	
}

/*********************************************************************************************************
** Function name:     	IsSaleTimeOver
** Descriptions:	    交易倒计时定时器控制
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t SaleTimeSet(uint8_t haveSale)
{
	uint8_t SaleTimer;
	if(haveSale == 0)
	{
		SaleTimer = (15 + SystemPara.SaleTime) > 255? 255: (15 + SystemPara.SaleTime);
	}
	else if(haveSale == 1)
	{
		SaleTimer = SystemPara.SaleTime > 255? 255: SystemPara.SaleTime;
	}
	
	return SaleTimer;
}

/*********************************************************************************************************
** Function name:     	DispSaleTime
** Descriptions:	    显示交易倒计时
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void DispSaleTime()
{
	LCDPrintf(150,LINE1,0,SystemPara.Language,"%s%02d",BUSINESSDEAL[SystemPara.Language][17],Timer.SaleTimer);	
}

/*********************************************************************************************************
** Function name:     	StackMoneyInd
** Descriptions:	    压抄暂存金额,并产生状态变化,判断是否可以支付
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      1可以支付,0不可以支付
*********************************************************************************************************/
uint8_t StackMoneyInd(uint32_t PriceSale)
{
	uint8_t state=1;
	//扣除单价
	if(PriceSale)
	{
		if(PriceSale > (g_coinAmount + g_billAmount))
		{
			//压抄成功
			if(StackedBillDevMoneyInAPI())
			{				
				g_billAmount += g_holdValue;
				//LogGetMoneyAPI(g_holdValue,2);//记录日志
				PayinRPTAPI(2,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;					
			}
			//压抄失败
			else
			{	
				ReturnBillDevMoneyInAPI();
				PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;					
			}
		}
		TracePC("\r\n App Price=%ld,money=%ld",PriceSale,GetAmountMoney());
		if(GetAmountMoney() >= PriceSale)
			state = 1;
		else
			state = 0;
	}
	//2产生状态变化
	if(GetAmountMoney())
	{					
		vmcStatus = VMC_SALE;
		Timer.SaleTimer = SaleTimeSet(0);
		if(vmcStatus == VMC_SALE)
		{
			//Trace("\r\n 1money=%ld",GetAmountMoney());
			if(g_billAmount< MoneyMaxin)							
				BillCoinCtr(1,1,0);
			//Trace("\r\n 2money=%ld",GetAmountMoney());
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			DispSalePage(0,hefangMode);
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			SaleSelectionKeyAPI(GetAmountMoney());
		}
	}							
	else
	{
		BillCoinCtr(1,1,0);
		channelInput = 0;
		channelMode = 0;
		memset(BinNum,0,sizeof(BinNum));
		memset(ChannelNum,0,sizeof(ChannelNum));
		//Trace("\r\n 3money=%ld",GetAmountMoney());
		OSMboxAccept(g_CoinMoneyBackMail);
		LCDClrScreen();
		DispEndPage();
		vmcStatus = VMC_END;
	}
	return state;
}
/*********************************************************************************************************
** Function name:     	CostMoneyInd
** Descriptions:	    通过PC机请求读卡器扣除金额
** input parameters:    PriceSale出货单价
** output parameters:   1允许扣款,0不允许扣款
** Returned value:      无
*********************************************************************************************************/
uint8_t CostReaderRPT(uint32_t PriceSale)
{
	uint8_t result=1,ComStatus=0;
	//扣除单价
	if(PriceSale)
	{
		//向读卡器发送交易成功
		if(g_readerAmount > 0)
		{			
			DispReaderDevVendoutRPT();
			ComStatus = ReaderDevVendoutRPTAPI(PriceSale);			
			if(ComStatus == 0)
			{
				result=0;
			}
		}	
	}
	return result;
}

/*********************************************************************************************************
** Function name:     	CostMoneyInd
** Descriptions:	    通过PC机扣除金额,并产生状态变化
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CostMoneyInd(uint32_t PriceSale)
{
	//unsigned char ComStatus = 0;
	
	//扣除单价
	if(PriceSale)
	{
		//向读卡器发送交易成功
		if(g_readerAmount > 0)
		{		
			TraceReader("\r\n AppPCCostSucc");
			ReaderDevVendoutResultAPI(1);
			g_readerAmount -= PriceSale;
			PriceSale = 0;			
		}	
		else
		{			
			if(PriceSale > (g_coinAmount + g_billAmount))
			{
				
				//压抄成功
				if(StackedBillDevMoneyInAPI())
				{				
					g_billAmount += g_holdValue;
					//LogGetMoneyAPI(g_holdValue,2);//记录日志
					PayinRPTAPI(2,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
					g_holdValue = 0;					
				}
				//压抄失败
				else
				{	
					ReturnBillDevMoneyInAPI();
					PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
					g_holdValue = 0;					
				}
				OSTimeDly(OS_TICKS_PER_SEC*4);		
			}

			if(PriceSale >= g_billAmount)
			{
				PriceSale -= g_billAmount;
				g_billAmount = 0;
			}
			else
			{
				g_billAmount -= PriceSale;
				PriceSale = 0;
			}
		}
	}
	if(PriceSale)
	{
		g_coinAmount -= PriceSale;					
	}	


	//2产生状态变化
	if(GetAmountMoney())
	{					
		vmcStatus = VMC_SALE;
		Timer.SaleTimer = SaleTimeSet(0);
		if(vmcStatus == VMC_SALE)
		{
			//Trace("\r\n 1money=%ld",GetAmountMoney());
			if(g_billAmount< MoneyMaxin)							
				BillCoinCtr(1,1,0);
			//Trace("\r\n 2money=%ld",GetAmountMoney());
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			DispSalePage(0,hefangMode);
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			SaleSelectionKeyAPI(GetAmountMoney());
		}
	}							
	else
	{
		BillCoinCtr(1,1,0);
		channelInput = 0;
		channelMode = 0;
		memset(BinNum,0,sizeof(BinNum));
		memset(ChannelNum,0,sizeof(ChannelNum));
		//Trace("\r\n 3money=%ld",GetAmountMoney());
		OSMboxAccept(g_CoinMoneyBackMail);
		LCDClrScreen();
		DispEndPage();
		vmcStatus = VMC_END;
	}
}

/*********************************************************************************************************
** Function name:     	VendoutInd
** Descriptions:	    通过PC机出货,并产生状态变化
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void VendoutInd(uint16_t columnNo, uint32_t PriceSale,uint8_t Type)
{
	uint8_t ChuhuoRst = 0;
	char     ChannelVend[3] = {0};//选择货道值

	DispChuhuoPagePC();
	BillCoinCtr(2,2,0);	

	TracePC("\r\n %dAppUboxVendout",OSTimeGet());	
	CostReaderRPT(PriceSale);
	//ActionRPTAPI(1,0,30,columnNo%100, Type,PriceSale,GetAmountMoney());
	ChuhuoRst = ChannelAPIProcess(columnNo%100,CHANNEL_OUTGOODS,columnNo/100);		
	//add by yoc 2013.12.16
	if((SystemPara.PcEnable == 1) || (SystemPara.PcEnable == 3))
	{
		if(ChuhuoRst == 1)
		{
			if(!GetAmountMoney())
				LogBeginTransAPI();
			if(Type == TRADE_CASH || Type == TRADE_ONE_CARD)
			{
				SaleCostMoney(PriceSale);//扣款
				
			}
			ChannelNum[0] = 'A';
			ChannelNum[1] = (columnNo%100/10) + '0';
			ChannelNum[2] = (columnNo%100%10) + '0';
			vmcPrice = PriceSale;
			Trace("LogTransactionAPI logic=%c%c%c price=%d type=%x,tranNums=%d\r\n ",
				ChannelNum[0],ChannelNum[1],ChannelNum[2],vmcPrice,Type,transMul);
			LogTransactionAPI(vmcPrice,transMul++,ChannelNum,Type);//记录日志
			VendoutRPTAPI(CHUHUO_OK, 1,columnNo%100, Type, PriceSale,GetAmountMoney(), ChannelGetParamValue(columnNo%100,2,columnNo/100) );		    					
			vmcStatus = VMC_QUHUO;
		}
		else
		{		
			DispChhuoFailPage();//add by yoc 2013.12.02
			VendoutRPTAPI(CHUHUO_ERR,1,columnNo%100, Type, PriceSale,GetAmountMoney(), ChannelGetParamValue(columnNo%100,2,columnNo/100) );		
		}
		
			
	}
	else if(SystemPara.PcEnable == 2)
	{
		if(ChuhuoRst==1)
		{
			DispQuhuoPage();		
			CostMoneyInd(PriceSale);
			ChannelVend[0] = columnNo%100/10+'0';
			ChannelVend[1] = columnNo%10+'0';
			LogBeginTransAPI();
			LogTransactionAPI(PriceSale,transMul++,ChannelVend,Type);//记录日志
			VendoutRPTAPI( 0, columnNo/100,columnNo%100, Type, PriceSale,GetAmountMoney(), ChannelGetParamValue(columnNo%100,2,columnNo/100) );
		}
		else
		{	
			DispChhuoFailPage();
			if(g_readerAmount > 0)
			{
				ReaderDevVendoutResultAPI(2);
			}
			VendoutRPTAPI( 2, columnNo/100,columnNo%100, Type, PriceSale,GetAmountMoney(), ChannelGetParamValue(columnNo%100,2,columnNo/100) );		
		}	
		//2产生状态变化
		if(GetAmountMoney())
		{					
			vmcStatus = VMC_SALE;
			Timer.SaleTimer = SaleTimeSet(0);
			if(vmcStatus == VMC_SALE)
			{
				//Trace("\r\n 1money=%ld",GetAmountMoney());
				if(g_billAmount< MoneyMaxin)							
					BillCoinCtr(1,1,0);
				//Trace("\r\n 2money=%ld",GetAmountMoney());
				channelInput = 0;
				channelMode = 0;
				memset(BinNum,0,sizeof(BinNum));
				memset(ChannelNum,0,sizeof(ChannelNum));
				//Trace("\r\n 3money=%ld",GetAmountMoney());
				OSMboxAccept(g_CoinMoneyBackMail);
				LCDClrScreen();
				//Trace("\r\n 4money=%ld",GetAmountMoney());
				DispSalePage(0,hefangMode);
				//Trace("\r\n 4money=%ld",GetAmountMoney());
				SaleSelectionKeyAPI(GetAmountMoney());
			}
		}							
		else
		{
			BillCoinCtr(1,1,0);
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			vmcStatus = VMC_END;
		}
	}

	
	
}


/*********************************************************************************************************
** Function name:     	VendoutIndFail
** Descriptions:	    通过PC机出货,因为各种原因出货失败
** input parameters:    PriceSale出货单价
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void VendoutIndFail(uint16_t columnNo, uint32_t PriceSale,uint8_t Type,uint8_t status)
{	
	if(SystemPara.PcEnable == 2)
	{			
		VendoutRPTAPI( status, columnNo/100,columnNo%100, Type, PriceSale,GetAmountMoney(), ChannelGetParamValue(columnNo%100,2,columnNo/100) );		
	}
}



/*********************************************************************************************************
** Function name:     	ResetInd
** Descriptions:	    PC机下发重启复位指令
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ResetInd()
{
	if(SystemPara.PcEnable==CRUBOX_PC)
	{
		//暂存退币
		if(g_holdValue)
		{
			if(ReturnBillDevMoneyInAPI())
			{
				PayinRPTAPI(4,g_holdValue,GetAmountMoney()-g_holdValue);//上报PC端
				g_holdValue = 0;			
			}
			TracePC("\r\n AppHoldRet%d",OSTimeGet());
			OSTimeDly(OS_TICKS_PER_SEC/2);
		}
	}
	else
	{
		if(GetAmountMoney())
		{
			ChangerMoney();
			OSTimeDly(OS_TICKS_PER_SEC*2);
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			DispEndPage();
			vmcStatus = VMC_END;
		}
	}
	ResetRPTAPI();
	MdbBusHardwareReset();
      OSTimeDly(OS_TICKS_PER_SEC*2);
	zyReset(ZY_HARD_RESET);
}


/*********************************************************************************************************
** Function name:     	TuiMoneyInd
** Descriptions:	    PC机下发退币指令
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TuiMoneyInd()
{
	uint32_t debtMoney;
	//读卡器不允许找零
	if((SystemPara.CashlessDeviceType != OFF_READERACCEPTER)&&(GetReaderAmount()))
	{
	}
	else if(GetAmountMoney())
	{
		DispPayoutPage();
		TracePC("\r\n AppTui%d",OSTimeGet());
		ActionRPTAPI(2,0,30,0,1,GetAmountMoney(),GetAmountMoney());
		OSTimeDly(OS_TICKS_PER_SEC*2);
		debtMoney = ChangerMoney();
		if(debtMoney)
		{
			DispIOUPage(debtMoney);
		}
		else
		{
			DispQuChangePage();
		}
		vmcStatus = VMC_END;
	}
	else
	{
		DispPayoutPage();
		TracePC("\r\n Appchange Fail");
		LogChangeAPI(0,0);//记录日志
		if(SystemPara.EasiveEnable == 1)
		{
			PayoutRPTAPI(0,0,0,0,0);
		}
		//OSTimeDly(OS_TICKS_PER_SEC);
		//PayinRPTAPI(2,0,0);//上报PC端
		g_coinAmount = 0;
		g_billAmount = 0;
		vmcStatus = VMC_END;
	}
}


/*********************************************************************************************************
** Function name:     	VendoutSIMPLEInd
** Descriptions:	    PC机发送出货,并产生状态变化
** input parameters:    
** output parameters:   无
** Returned value:      1成功,0失败
*********************************************************************************************************/
uint8_t VendoutSIMPLEInd(uint16_t columnNo)
{
	uint8_t ChuhuoRst = 0;

	DispChuhuoPagePC();
	
	TracePC("\r\n %dAppUboxVendout",OSTimeGet());	
	//ActionRPTAPI(1,0,30,columnNo%100, Type,PriceSale,GetAmountMoney());
	ChuhuoRst = ChannelAPIProcess(columnNo%100,CHANNEL_OUTGOODS,columnNo/100);	
	//2产生状态变化
	if(GetAmountMoney())
	{					
		vmcStatus = VMC_SALE;
		Timer.SaleTimer = SaleTimeSet(0);
		if(vmcStatus == VMC_SALE)
		{
			//Trace("\r\n 1money=%ld",GetAmountMoney());
			if(g_billAmount< MoneyMaxin)							
				BillCoinCtr(1,1,0);
			//Trace("\r\n 2money=%ld",GetAmountMoney());
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			DispSalePage(0,hefangMode);
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			SaleSelectionKeyAPI(GetAmountMoney());
		}
	}							
	else
	{
		BillCoinCtr(1,1,0);
		channelInput = 0;
		channelMode = 0;
		memset(BinNum,0,sizeof(BinNum));
		memset(ChannelNum,0,sizeof(ChannelNum));
		//Trace("\r\n 3money=%ld",GetAmountMoney());
		OSMboxAccept(g_CoinMoneyBackMail);
		LCDClrScreen();
		vmcStatus = VMC_END;
	}
	return ChuhuoRst;
	
}

/*********************************************************************************************************
** Function name:     	GetmoneySIMPLEInd
** Descriptions:	    PC机发送金额,并产生状态变化
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void GetmoneySIMPLEInd(uint16_t payInMoney)
{
	uint32_t g_yuanAmount = 0;

	g_yuanAmount = GetAmountMoney();
	g_readerAmount=payInMoney;
	//2产生状态变化
	if(GetAmountMoney())
	{					
		vmcStatus = VMC_SALE;
		Timer.SaleTimer = SaleTimeSet(0);
		if(vmcStatus == VMC_SALE)
		{
			//Trace("\r\n 1money=%ld",GetAmountMoney());
			if(g_billAmount< MoneyMaxin)							
				BillCoinCtr(1,1,0);
			//Trace("\r\n 2money=%ld",GetAmountMoney());
			channelInput = 0;
			channelMode = 0;
			memset(BinNum,0,sizeof(BinNum));
			memset(ChannelNum,0,sizeof(ChannelNum));
			//Trace("\r\n 3money=%ld",GetAmountMoney());
			OSMboxAccept(g_CoinMoneyBackMail);
			LCDClrScreen();
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			DispSalePage(0,hefangMode);
			//Trace("\r\n 4money=%ld",GetAmountMoney());
			SaleSelectionKeyAPI(GetAmountMoney());
		}
	}							
	else
	{
		BillCoinCtr(1,1,0);
		channelInput = 0;
		channelMode = 0;
		memset(BinNum,0,sizeof(BinNum));
		memset(ChannelNum,0,sizeof(ChannelNum));
		//Trace("\r\n 3money=%ld",GetAmountMoney());
		OSMboxAccept(g_CoinMoneyBackMail);
		if(g_yuanAmount>0)
		{
			LCDClrScreen();
			vmcStatus = VMC_END;
		}
	}
	
}

/*********************************************************************************************************
** Function name:     	GetmoneySIMPLEInd
** Descriptions:	    PC机发送商品单价金额,并产生状态变化
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void PriceSIMPLEInd(uint16_t payInMoney)
{
	uint16_t debtMoney=0;
	char strlanguage[30] = {0},streng[30] = {0};
	
	//1.显示单价	
	Timer.ChaxunTimer = 5;
	if(GetAmountMoney() == 0)
	{
		DispChaxunFreePage();
	}
	debtMoney = payInMoney;
	switch(SystemPara.DecimalNum) 
	{
	  case 2://以分为单位
		  sprintf(strlanguage,"%s%02d.%02d",BUSINESS[SystemPara.Language][3],debtMoney/100,debtMoney%100);
		  sprintf(streng,"%s%02d.%02d",BUSINESS[1][3],debtMoney/100,debtMoney%100); 
		  break;

	  case 1://以角为单位
		  debtMoney /= 10;
		  sprintf(strlanguage,"%s%d.%d",BUSINESS[SystemPara.Language][3],debtMoney/10,debtMoney%10);
		  sprintf(streng,"%s%d.%d",BUSINESS[1][3],debtMoney/10,debtMoney%10);
		  break;
	  
	  case 0://以元为单位
		  sprintf(strlanguage,"%s%d",BUSINESS[SystemPara.Language][3],debtMoney/100);
		  sprintf(streng,"%s%d",BUSINESS[1][3],debtMoney/100);
		  break;
	}	
	//strcpy(streng,BUSINESS[1][3]);
	DispBusinessText(strlanguage,streng,"","");
	OSTimeDly(OS_TICKS_PER_SEC*3);
	
	
	//2超时退出查询页面
	if(Timer.ChaxunTimer == 0)
	{
		if(GetAmountMoney() > 0)
		{				
			vmcStatus = VMC_SALE;
		}
		else
		{
			ClearDealPar(); 	
			CLrBusinessText();
			vmcStatus = VMC_FREE;
		}
		rstTime();
	}
	if(vmcStatus == VMC_SALE)
	{	
		DispSalePage(0,hefangMode);					
	}
	else if(vmcStatus == VMC_FREE)
	{
		LCDClrScreen();
		rstTime();
	}
}

/*********************************************************************************************************
** Function name:     	TxtSIMPLEInd
** Descriptions:	    PC机发送字符串,并产生状态变化
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TxtSIMPLEInd(char *disp)
{
	//uint16_t debtMoney=0;
	char strlanguage[30] = {0},streng[30] = {0};
	//2产生状态变化
	if(GetAmountMoney())
	{
		GetmoneySIMPLEInd(0);
	}
	sprintf(strlanguage,"%s",disp);
	sprintf(streng,"%s",disp);
	LCDPrintf(80,LINE15,0,SystemPara.Language,streng);
	
	
}


/*********************************************************************************************************
** Function name:     	BusinessProcess
** Descriptions:	    交易流程
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void BusinessProcess(void *pvData)
{	
	uint8_t keyValue = 0;//按键值
	uint8_t moneyGet = 0;//是否已经有钱了
	uint8_t haveSale = 0;//0第一次购买,1第二次以上购买	
	uint32_t debtMoney;	
	uint8_t ChuhuoRst = 0;
	uint8_t keyMode = 0;
	
	
	//OSTimeDly(OS_TICKS_PER_SEC);
	Timer.DispFreeTimer = 0;
	Timer.CheckDevStateTimer = 0;	
	Timer.GetTubeTimer = 0;
	Timer.ChaxunTimer = 0;
	//Trace("\r\n 3.de=%ld,%ld,%ld",LogParaDetail.IncomeBill,LogParaDetail.IncomeCoin,LogParaDetail.IncomeCard);
#if 0
	//是否开启压缩机和展示灯
	if(UserPara.CompressorCtr.flag==1)
	{
		API_ACDCHandle(UserPara.LEDCtr.flag,1,OPENCOMPRESSOR);
	}
	else
	if(UserPara.CompressorCtr.flag==0)
	{
		API_ACDCHandle(UserPara.LEDCtr.flag,1,CLOSECOMPRESSOR);
	}
	if(UserPara.LEDCtr.flag==1)
	{
		API_ACDCHandle(UserPara.CompressorCtr.flag,1,OPENLIGHT);
	}
	else
	if(UserPara.LEDCtr.flag==0)
	{
		API_ACDCHandle(UserPara.CompressorCtr.flag,1,CLOSELIGHT);
	}
	Trace("ACDCFlag=%d  %d\r\n",UserPara.CompressorCtr.flag,UserPara.LEDCtr.flag);
#endif

	//5.是否进入维护状态
	if(ReturnMaintainKeyValue(1))
	{
		BillCoinCtr(2,2,2);
		vmcStatus = VMC_WEIHU;
	}
	PriceMaxin = ChannelGetMaxGoodsPrice(1);
	//设置空闲状态下展示灯的开关
	if(SystemPara.threeSelectKey==1)
		FreeSelectionKeyValueAPI();
	else
		FreeSelectionKeyAPI(1);
	gameLedControl(1);//打开游戏灯
	
	
	LCDClrScreen();
	while(1)
	{	
		//顶级轮训PC管理任务add by yoc 2014.04.22
		mainTaskPollPC(vmcStatus);
		
		switch(vmcStatus)
		{
			case VMC_FREE:
				checkTaskDevSignal();
				if(GOCCHECKTIMEOUT>200)
				{
					GOCCHECKTIMEOUT = 0;
					//ChannelGetGocState(1);
				}

				//1.显示空闲页面
				if(Timer.DispFreeTimer==0)
				{
					Timer.DispFreeTimer = 5;
					DispFreePage();
					WaitForPCInit();
				}
				//2.轮询按键
				keyValue = KeyValueAPI(0,&keyMode);
				if(keyValue)
				{
					//无效，不查询盒饭柜单价
					if((keyValue == '<')||(keyValue == '>'))
					{
						channelInput = 0;
						channelMode = 0;
					}
					else
					{
						Timer.ChaxunTimer = 5;
						vmcStatus = VMC_CHAXUN;
					}
				}
				//3.轮询硬币器可找零硬币
				if(Timer.GetTubeTimer==0)
				{
					Timer.GetTubeTimer = 5;
					UnpdateTubeMoney();
				}
				//3.轮询投纸币和硬币金额	
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					ClrTuibiAPI();
					LCDClrScreen();
					DispSalePage(haveSale,hefangMode);
					TraceBill("\r\n App2amount=%ld",GetAmountMoney());
					LogBeginTransAPI();//记录到明细日志中
					Timer.SaleTimer = SaleTimeSet(haveSale);
					SaleSelectionKeyAPI(GetAmountMoney());
					vmcStatus = VMC_SALE;
				}
				//4.检测设备故障状态
				if(Timer.CheckDevStateTimer==0)
				{
					Timer.CheckDevStateTimer = 5;
					CheckDeviceState();					
					if(IsErrorState())
					{
						if((SystemPara.PcEnable==CRUBOX_PC)&&(LogPara.offLineFlag == 0))
						{
						}
						else
						{
							Timer.DispFreeTimer=0;
							TracePC(">>status2");
							StatusRPTAPI();
							OSTimeDly(OS_TICKS_PER_SEC/2);
							LCDClrScreen();
							BillCoinCtr(2,2,2);
							rstTime();
							vmcStatus = VMC_ERROR;
						}
					}					
				}
				//5.是否进入维护状态
				if(ReturnMaintainKeyValue(1))
				{
					BillCoinCtr(2,2,2);
					vmcStatus = VMC_WEIHU;
				}
				//6.自检压缩机和展示灯
				ACDCTimingHandle(1);
				//7.检查pc机轮询
				PollAPI(GetAmountMoney());
				//8.接近感应检测 add by yoc 2013.11.13
				pollHuman(ReadCloseHumanKeyValue());
				//9轮询大门传感器 add by yoc 2013.11.13
				pollDoorAPI(0);
				//10.轮询游戏按键
				ReadGameKeyValueAPI();
				//11.轮询退币按钮，有按下蜂鸣器响一下
				IsTuibiAPI();
				break;
			case VMC_CHAXUN:
				//1.轮询按键
				if(keyValue)
				{
					TraceSelection("\r\n AppKey2=%d",keyValue);
					Timer.ChaxunTimer = 5;
					if(GetAmountMoney() == 0)
					{
						DispChaxunFreePage();
					}
					DispChaxunPage(&keyValue,&keyMode);
				}
				keyValue = KeyValueAPI(1,&keyMode);
				
				//2超时退出查询页面
				if(Timer.ChaxunTimer == 0)
				{
					keyValue = 'C';
					keyMode = 2;
					rstTime();
				}
				if(vmcStatus == VMC_SALE)
				{	
					DispSalePage(haveSale,hefangMode);					
				}
				else if(vmcStatus == VMC_FREE)
				{
					LCDClrScreen();
					rstTime();
				}
				break;
			case VMC_CHAXUNHEFANG:
				//1.轮询按键
				if(keyValue)
				{
					Timer.ChaxunTimer = 20;
					if(GetAmountMoney() == 0)
					{
						DispChaxunFreePage();
					}
					DispChaxunHefangPage(&keyValue);
				}
				keyValue = KeyValueAPI(1,&keyMode);
				
				//2超时退出查询页面
				if(Timer.ChaxunTimer == 0)
				{
					keyValue = 'C';
					keyMode = 2;
				}				
				if(vmcStatus == VMC_SALE)
				{
					DispSalePage(haveSale,hefangMode);
				}
				else if(vmcStatus == VMC_FREE)
				{
					LCDClrScreen();
				}
				break;
			case VMC_SALE:
				//1.轮询投纸币和硬币金额
				//i=0;
				//Trace("\r\n u=%d",i++);
				//Trace("\r\n VMC_SALE");
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					DispSalePage(haveSale,hefangMode);
					Timer.SaleTimer = SaleTimeSet(haveSale);
					SaleSelectionKeyAPI(GetAmountMoney());
					TraceBill("\r\n App2amount=%ld",GetAmountMoney());	 				
				}
				//2.轮询按键
				//Trace("\r\n u=%d",i++);
				keyValue = KeyValueAPI(0,&keyMode);
				//keyValue = 0;
				if(keyValue)
				{			
					TraceSelection("\r\n AppKey1=%d",keyValue);
					Timer.ChaxunTimer = 5;
					vmcStatus = VMC_CHAXUN;	
					break;
				}
				//Trace("\r\n u=%d",i++);
				//有按下退币按键
				if(
					//有按下退币按键     或者交易时间结束
					(IsTuibiAPI()||(IsSaleTimeOver()) )
					//      强制购买                或者已经交易过一次以上了 
					&&((UserPara.TransEscape == 0)||(haveSale == 1))
					//插入读卡器按退币无效
					&&(g_readerAmount == 0)
				)
				{
					if(SystemPara.PcEnable!=CRUBOX_PC)
					{
						BillCoinCtr(2,2,0);
						vmcStatus = VMC_PAYOUT;
					}
				}
				//Trace("\r\n u=%d",i++);
				//显示倒计时
				if( 
					(SystemPara.SaleTime < 255) 
					//      允许不交易退币          或者已经交易过一次以上了
					&&((UserPara.TransEscape == 0)||(haveSale == 1))
					//插入读卡器按退币无效
					&&(g_readerAmount == 0)
				)
				{
					DispSaleTime();
				}
				if(moneyGet == 2)
				{					
					vmcStatus = VMC_END;
				}
				//7.检查pc机轮询
				PollAPI(GetAmountMoney());
				//8.轮询游戏按键
				ReadGameKeyValueAPI();
				//9.轮询设备故障状态
				CheckDeviceState();
				break;
			case VMC_OVERVALUE:
				break;
			case VMC_XUANHUO:
				IsCanSale();				
				break;
			case VMC_WUHUO:
				break;
			case VMC_LESSMONEY:
				DispCannotBuyPage(vmcPrice);
				Timer.SaleTimer = SaleTimeSet(haveSale);
				vmcStatus = VMC_SALE;
				channelInput = 0;
				channelMode = 0;
				memset(BinNum,0,sizeof(BinNum));
				memset(ChannelNum,0,sizeof(ChannelNum));
				OSMboxAccept(g_CoinMoneyBackMail);
				vmcPrice = 0;
				DispSalePage(haveSale,hefangMode);
				break;
			case VMC_CHANGESHORT:
				DispCannotBuyAsPayoutPage();
				if(GetAmountMoney())
				{
					Timer.SaleTimer = SaleTimeSet(haveSale);
					vmcStatus = VMC_SALE;
					channelInput = 0;
					channelMode = 0;
					memset(BinNum,0,sizeof(BinNum));
					memset(ChannelNum,0,sizeof(ChannelNum));
					OSMboxAccept(g_CoinMoneyBackMail);
					vmcPrice = 0;
					DispSalePage(haveSale,hefangMode);
				}
				else
				{
					vmcStatus = VMC_END;
				}
				break;
			case VMC_READVENDFAIL:
				DispReaderVendoutFail();
				vmcStatus = VMC_SALE;
				channelInput = 0;
				channelMode = 0;
				memset(BinNum,0,sizeof(BinNum));
				memset(ChannelNum,0,sizeof(ChannelNum));
				OSMboxAccept(g_CoinMoneyBackMail);
				vmcPrice = 0;
				DispSalePage(haveSale,hefangMode);
				break;	
			case VMC_CHUHUO:
				BillCoinCtr(2,2,0);
				DispChuhuoPage();
					

				//ActionRPTAPI(1,0,30,vmcColumn%100, 0,vmcPrice,GetAmountMoney());
                //OSTimeDly(OS_TICKS_PER_SEC / 2);
				ChuhuoRst = ChannelAPIProcess(vmcColumn%100,CHANNEL_OUTGOODS,vmcColumn/100);		
				if(ChuhuoRst==1)
				{
					SaleCostMoney(vmcPrice);//扣款
				
					//Trace("\r\n money=%d\r\n",GetAmountMoney());	
					
					//记录日志卡消费金额
					if(g_readerAmount > 0)
					{
						LogTransactionAPI(vmcPrice,transMul++,ChannelNum,2);//记录卡日志
					}
					else
					{
						LogTransactionAPI(vmcPrice,transMul++,ChannelNum,0);//记录日志
						ReaderDevCashSaleAPI(vmcPrice);
					}
					VendoutRPTAPI( 0, vmcColumn/100,vmcColumn%100, 0, vmcPrice,GetAmountMoney(), ChannelGetParamValue(vmcColumn%100,2,vmcColumn/100) );
					vmcStatus = VMC_QUHUO;
				}
				else
				{
					//向读卡器发送交易失败
					if(g_readerAmount > 0)
					{
						TraceReader("\r\n AppCHuhuoFail");
						ReaderDevVendoutResultAPI(2);
					}	
					if(SystemPara.PcEnable == 1 || SystemPara.PcEnable == 3)
					{
						VendoutRPTAPI( CHUHUO_ERR, 1,vmcColumn%100, 0, vmcPrice,GetAmountMoney(), ChannelGetParamValue(vmcColumn%100,2,vmcColumn/100) );
					}
					else
						VendoutRPTAPI( 2, vmcColumn/100,vmcColumn%100, 0, vmcPrice,GetAmountMoney(), ChannelGetParamValue(vmcColumn%100,2,vmcColumn/100) );
					vmcStatus = VMC_CHUHUOFAIL;
				}
				vmcPrice = 0;
				break;
			case VMC_QUHUO:
				DispQuhuoPage();
//				if(SystemPara.ChannelType == 2)
//				{
//					while(1)
//					{
//						ChuhuoRst = LiftTableProcess(vmcColumn/100,vmcColumn%100,LIFTTABLE_TAKEGOODS);	
//						if(ChuhuoRst == 1)	
//							break;
//					}
//				}
				//多次购买
				if( (UserPara.TransMultiMode == 1)&&(GetAmountMoney()) )
				{					
					vmcStatus = VMC_SALE;
					haveSale = 1;
					Timer.SaleTimer = SaleTimeSet(haveSale);
					if(vmcStatus == VMC_SALE)
					{
						//Trace("\r\n 1money=%ld",GetAmountMoney());
						if(g_billAmount< MoneyMaxin)						
							BillCoinCtr(1,1,0);
						else
							BillCoinCtr(0,1,0);	
						//Trace("\r\n 2money=%ld",GetAmountMoney());
						channelInput = 0;
						channelMode = 0;
						memset(BinNum,0,sizeof(BinNum));
						memset(ChannelNum,0,sizeof(ChannelNum));
						//Trace("\r\n 3money=%ld",GetAmountMoney());
						OSMboxAccept(g_CoinMoneyBackMail);
						LCDClrScreen();
						//Trace("\r\n 4money=%ld",GetAmountMoney());
						DispSalePage(haveSale,hefangMode);
						//Trace("\r\n 4money=%ld",GetAmountMoney());
						SaleSelectionKeyAPI(GetAmountMoney());
						keyValue = KeyValueAPI(0,&keyMode);
					}
				}
				else if(GetAmountMoney())
				{
					DispSalePage(haveSale,hefangMode);
					vmcStatus = VMC_PAYOUT;
				}
				else
				{
					vmcStatus = VMC_END;
				}
				break;
			case VMC_CHUHUOFAIL:
				
				DispChhuoFailPage();
				//多次购买
				if(GetAmountMoney())
				{					
					vmcStatus = VMC_SALE;
					haveSale = 1;
					Timer.SaleTimer = SaleTimeSet(haveSale);
					if(vmcStatus == VMC_SALE)
					{
						if(g_billAmount< MoneyMaxin)							
							BillCoinCtr(1,1,0);
						else
							BillCoinCtr(0,1,0);
						channelInput = 0;
						channelMode = 0;
						memset(BinNum,0,sizeof(BinNum));
						memset(ChannelNum,0,sizeof(ChannelNum));
						OSMboxAccept(g_CoinMoneyBackMail);
						LCDClrScreen();
						DispSalePage(haveSale,hefangMode);
					}
				}
				break;
			case VMC_PAYOUT:				
				DispPayoutPage();
				debtMoney = ChangerMoney();
				if(debtMoney)
				{
					DispIOUPage(debtMoney);
				}
				else
				{
					DispQuChangePage();
				}
				vmcStatus = VMC_END;
				break;
			case VMC_END:				
				DispEndPage();
				ClearDealPar();
				haveSale = 0;
				transMul = 0;
				vmcColumn = 0;	
				hefangMode = 0;
				Timer.GetTubeTimer = 0;
				BillCoinCtr(1,1,1);
				vmcChangeLow = 0;
				rstTime();
				PriceMaxin = ChannelGetMaxGoodsPrice(1);
				LogEndTransAPI();//记录到日志中
				if(SystemPara.threeSelectKey==1)
					FreeSelectionKeyValueAPI();
				else
					FreeSelectionKeyAPI(1);
				
				//UnpdateTubeMoney();
				LCDClrScreen();
				vmcStatus = VMC_FREE;
				break;
			case VMC_ERROR:	
				//1.显示故障页面
				if(Timer.DispFreeTimer==0)
				{
					Timer.DispFreeTimer = 30;
					DispErrPage();
				}	
				//2.检测设备是否恢复正常
				if(Timer.CheckDevStateTimer==0)
				{
					Timer.CheckDevStateTimer = 5;
					CheckDeviceState();
					//6.自检压缩机和展示灯
					ACDCTimingHandle(1);
					if(!IsErrorState())
					{	
						Timer.DispFreeTimer=0;
						LCDClrScreen();
						BillCoinCtr(1,1,1);
						OSTimeDly(OS_TICKS_PER_SEC/2);
						TracePC(">>status3");
						StatusRPTAPI();
						rstTime();
						vmcStatus = VMC_FREE;
					}
					PollAPI(GetAmountMoney());
				}
				//3.是否进入维护状态
				if(ReturnMaintainKeyValue(1))
				{
					vmcStatus = VMC_WEIHU;
				}
				//4.自检压缩机和展示灯
				//ACDCTimingHandle(1);
				//5.轮询硬币器可找零硬币
				//if(Timer.GetTubeTimer==0)
				//{
				//	Timer.GetTubeTimer = 5;
				//	UnpdateTubeMoney();
				//}
				//6.轮询投纸币和硬币金额	
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					ClrTuibiAPI();
					LCDClrScreen();
					DispSalePage(haveSale,hefangMode);
					TraceBill("\r\n App2amount=%ld",GetAmountMoney());
					LogBeginTransAPI();//记录到明细日志中
					vmcStatus = VMC_ERRORSALE;
				}
				break;
			case VMC_ERRORSALE:	
				//1.轮询投纸币和硬币金额
				//i=0;
				//Trace("\r\n u=%d",i++);
				//Trace("\r\n VMC_SALE");
				moneyGet = GetMoney();
				if(moneyGet == 1)
				{
					DispSalePage(haveSale,hefangMode);
					TraceBill("\r\n App2amount=%ld",GetAmountMoney());	 				
				}
				//有按下退币按键
				if(
					//有按下退币按键     
					IsTuibiAPI()
					//插入读卡器按退币无效
					&&(g_readerAmount == 0)
				)
				{
					BillCoinCtr(2,2,0);
					vmcStatus = VMC_ERRORPAYOUT;					
				}				
				if(moneyGet == 2)
				{
					vmcStatus = VMC_ERROREND;
				}
				//7.检查pc机轮询
				PollAPI(GetAmountMoney());
				break;
			case VMC_ERRORPAYOUT:
				DispPayoutPage();
				debtMoney = ChangerMoney();
				if(debtMoney)
				{
					DispIOUPage(debtMoney);
				}
				else
				{
					DispQuChangePage();
				}
				vmcStatus = VMC_ERROREND;
				break;
			case VMC_ERROREND:				
				DispEndPage();
				ClearDealPar();
				haveSale = 0;
				transMul = 0;
				vmcColumn = 0;
				LCDClrScreen();
				BillCoinCtr(1,1,0);
				LogEndTransAPI();//记录到日志中
				rstTime();
				vmcStatus = VMC_ERROR;
				break;	
			case VMC_WEIHU:
				FreeSelectionKeyAPI(0);	
				/*
				SetWeihuStatus(1);
				StatusRPTAPI();
				OSTimeDly(OS_TICKS_PER_SEC);
				ActionRPTAPI(5,1,0,0,0,0,0);
				*/
				if(SystemPara.PcEnable)
				{
					//ActionRPTAPI(5,1,0,0,0,0,0);
					SetWeihuStatus(1);
					if(SystemPara.EasiveEnable == 0)
					{
						OSTimeDly(OS_TICKS_PER_SEC);
						TracePC(">>status4");
						StatusRPTAPI();
						OSTimeDly(OS_TICKS_PER_SEC);
					}
				}
				
				//do
				{
					//7.检查pc机轮询
					PollAPI(GetAmountMoney());
					MaintainUserProcess((void*)0);
				}
				//while(ReadMaintainKeyValue());
				Timer.DispFreeTimer=0;
				LCDClrScreen();
				BillCoinCtr(1,1,1);
				vmcChangeLow = 0;
				rstTime();
				if(SystemPara.PcEnable)
				{
					ActionRPTAPI(5,0,0,0,0,0,0);
					SetWeihuStatus(0);
					if(SystemPara.EasiveEnable == 0)
					{
						OSTimeDly(OS_TICKS_PER_SEC);
						TracePC(">>status5");
						StatusRPTAPI();
						OSTimeDly(OS_TICKS_PER_SEC);
					}
					//globalSys.pcInitFlag = 0;
				}
				PriceMaxin = ChannelGetMaxGoodsPrice(1);
				//设置空闲状态下展示灯的开关
				if(SystemPara.threeSelectKey==1)
					FreeSelectionKeyValueAPI();
				else
					FreeSelectionKeyAPI(1);
				
				vmcStatus = VMC_FREE;
				break;
		}		
		OSTimeDly(OS_TICKS_PER_SEC / 100);
	}	
}
/**************************************End Of File*******************************************************/
