/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           COMPRESSORANDLIGHT.c
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        展示灯压缩机UI                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "ACDC.h"
#include "..\config.h"
#include "CHANNEL.h"

static unsigned short curSetTemp = 0,tempFlush = 0,chuchouFlush = 0;//当前加热设置温度

typedef struct _ui_acdc_man_{
	
	char *manage[MAXLANGUAGETYPE];//"压缩机展示灯管理"
	char *lightManage[MAXLANGUAGETYPE];//"展示灯操作",
	char *cprtManage[MAXLANGUAGETYPE];//"压缩机操作"
	char *hotManage[MAXLANGUAGETYPE];//"加热操作",
	char *chuchouManage[MAXLANGUAGETYPE];//"除臭操作"
	char *timerManage[MAXLANGUAGETYPE];//"定时功能",
	char *startTime[MAXLANGUAGETYPE];//"开启时间: "
	char *stopTime[MAXLANGUAGETYPE];//"关闭时间: "
	char *setErr[MAXLANGUAGETYPE];//"错误: 开启时间大于关闭时间! "
	char *timerSet[MAXLANGUAGETYPE];//" 定时设定",
	char *timerStarting[MAXLANGUAGETYPE];//"定时"
	char *operating[MAXLANGUAGETYPE];//"正在操作。。"
	char *operateSuc[MAXLANGUAGETYPE];//"操作成功"
	char *operateFail[MAXLANGUAGETYPE];//"操作失败"
	
	char *setTemp[MAXLANGUAGETYPE];//"设置温度"
	char *curTemp[MAXLANGUAGETYPE];//"当前温度"
	char *durTime[MAXLANGUAGETYPE];//"时间段"

	
}UI_ACDC_MAN;

const UI_ACDC_MAN ui_acdc_man ={

	
	{
		"压缩机展示灯管理",
		"ACDC Manage",
		"ACDC Manage",
		"ACDC Manage",
		"ACDC Manage"
	},
	{
		"展示灯操作",
		"Light Handle",
		"Light Handle",
		"ISIK",
		"LUZ"
	},
	{
		"压缩机操作",
		"Compressor Handle",
		"Compressor Handle",
		"SOGUTUCU",
		"REFRIGERA\x83\x82\O"
	},
	{
		"加热操作",
		"Hot Handle",
		"Hot Handle",
		"ISIK",
		"LUZ"
	},
	{
		"除臭操作 ",
		"Chuchou Handle",
		"Handle",
		"SOGUTUCU",
		"REFRIGERA\x83\x82\O"
	},
	{
		"定时功能",
		"Timing operation",
		"Timing Manage",
		"ZAMANLAMA AYARI",
		"TEMPO DE OPERA\x83\x82\O"
	},
	{
		"开启时间: ",
		"Start Time:",
		"Start Time:",
		"ACILIS ZAMANI:",
		"HORA DE ABERTURA:"
	},
	{
		"关闭时间: ",
		"Close Time:",
		"Close Time:",
		"KAPANIS ZAMANI:",
		"HORA DE FECHAMENTO:"
	},
	{
		"错误: 开启时间大于关闭时间! ",
		"Error:Start Time Large than End!",
		"Error:Start Time Large than End!",
		"HATALI GIRIS!",
		"ERRO HORA ABERT.+TARDE HORA FECHA"
	},
	{
		" 定时设定",
		" Time Set",
		" Time Set",
		" Time Set",
		" Time Set",
	},
	{
		"定时",
		"Timing",
		"Timing",
		"Timing",
		"Timing",
	},
	{
		"正在操作。。",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	},
	{
		"操作成功",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	},
	{
		"操作失败",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	}
	,
	{
		"设置温度",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	},
	{
		"当前温度",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	},
	{
		"时间段",
		"operating..",
		"operating..",
		"operating..",
		"operating..",
	}


};

/*********************************************************************************************************
** Function name:     	cabinetSelectPage
** Creater By		:	yoc 2014.05.07
** Descriptions:	 柜号选择
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char cabinetSelectPage(void)
{
		return 1;
}



/*********************************************************************************************************
** Function name:     	acdc_hotTimerSetPage
** Creater By		:	yoc 2014.05.07
** Descriptions:	  加热设置页面
** input parameters:    cabinet 柜号
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void acdc_hotTimerSetPage(unsigned char cabinet,const unsigned char no)
{
	unsigned char topFlushFlag = 1,topReturnFlag = 0,enterSub = 0,subFlushFlag = 0;
	unsigned char lang,key;
	unsigned char cursorIndex = 0;
	char buf[4] = {' '},dis_buf[4];
	unsigned short temp = 0;
	ST_HOT_DEV *stDevPtr;

	
	if(cabinet == 1)
	{
		stDevPtr = &UserPara.box1Hot;
	}
	else if(cabinet == 2)
	{	
		stDevPtr = &UserPara.box2Hot;
	}
	else
		return;
		
	lang = SystemPara.Language;
	while(1)
	{
		if(topFlushFlag)
		{
			topFlushFlag = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			if(stDevPtr->flag & (0x01 << (no + 1)))
				LCDPrintf(8,1,0,lang,"%s %s",ui_acdc_man.timerManage[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,1,0,lang,"%s %s",ui_acdc_man.timerManage[lang],ui_aisle_admin.close[lang]);

			LCDPrintf(8,5,0,lang,"%s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.open[lang]);
			LCDPrintf(8,7,0,lang,"%s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);

			if(stDevPtr->flag & (0x01 << (no + 1)))
			{
				LCDPrintf(8,9,0,lang,"%s %s %02d:%02d",ui_aisle_admin.no_3[lang],ui_acdc_man.startTime[lang],
												stDevPtr->HourOn[no - 1],stDevPtr->MinOn[no - 1]);
				LCDPrintf(8,11,0,lang,"%s %s %02d:%02d",ui_aisle_admin.no_4[lang],ui_acdc_man.stopTime[lang],
													stDevPtr->HourOff[no - 1],stDevPtr->MinOff[no - 1]);				
				LCDPrintf(8,13,0,lang,"%s %s:  %02d.%02d ℃",ui_aisle_admin.no_5[lang],ui_acdc_man.setTemp[lang],
													stDevPtr->setTemp[no - 1] / 10,(stDevPtr->setTemp[no - 1] % 10) * 10);	
			
			}
			
						
		}

		if(topReturnFlag)
		{
			return ;
		}

		key = ReadKeyValue();
		OSTimeDly(5);
		
		switch(key)
		{
			case '1':
				stDevPtr->flag |= (0x01 << (no + 1));
				topFlushFlag = 1;
				break;

			case '2':
				stDevPtr->flag &= ~(0x01 << (no + 1));
				topFlushFlag = 1;
				break;
			case '3':	
				if(!(stDevPtr->flag & (0x01 << (no + 1))))
					break;
				subFlushFlag = 1;
				enterSub = 1;
				cursorIndex = 0;
				buf[0] = stDevPtr->HourOn[no - 1]  / 10 + '0';
				buf[1] = stDevPtr->HourOn[no - 1]  % 10 + '0';
				buf[2] = stDevPtr->MinOn[no - 1]   / 10 + '0';
				buf[3] = stDevPtr->MinOn[no - 1]   % 10 + '0';
				break;
			case '4':
				if(!(stDevPtr->flag & (0x01 << (no + 1))))
				break;
				subFlushFlag = 1;
				enterSub = 3;
				cursorIndex = 0;
				buf[0] = stDevPtr->HourOff[no - 1]	/ 10 + '0';
				buf[1] = stDevPtr->HourOff[no - 1]	% 10 + '0';
				buf[2] = stDevPtr->MinOff[no - 1]	/ 10 + '0';
				buf[3] = stDevPtr->MinOff[no - 1]	% 10 + '0';
				
				break;
			case '5'://设置温度
				if(!(stDevPtr->flag & (0x01 << (no + 1))))
					break;
				temp = 0;
				subFlushFlag = 1;
				enterSub = 1;
				while(enterSub)
				{
					if(subFlushFlag)
					{
						subFlushFlag = 0;
						LCDPrintf(8,13,1,lang,"%s %s:  %02d.%02d ℃",ui_aisle_admin.no_5[lang],ui_acdc_man.setTemp[lang],
												temp / 10,(temp % 10) * 10);	
					}
					key = ReadKeyValue();
					OSTimeDly(5);
					switch(key)
					{
						case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
							temp = temp * 10 + (key - '0');
							subFlushFlag = 1;
							break;
						case 'C':
							enterSub = 0;
							topFlushFlag = 1;
							break;
						case 'E'://确定编辑
							stDevPtr->setTemp[no - 1] = ((temp >= ACDC_MAX_HOT_TEMP) ?ACDC_MAX_HOT_TEMP  : temp);
							TraceCompress("temp: %d   %d\r\n",stDevPtr->setTemp[no - 1],temp);
							
							enterSub = 0;
							topFlushFlag = 1;
							break;
						default:
							break;
					}
				}
				break;
			case 'C':
				enterSub = 0;
				topReturnFlag = 1;
				break;
			default:
				break;
				
		}
		
		while(enterSub)//编辑界面
		{
			if(subFlushFlag)
			{
				subFlushFlag = 0;
				dis_buf[0] = (cursorIndex == 0) ? '-': buf[0];
				dis_buf[1] = (cursorIndex == 1) ? '-': buf[1];
				dis_buf[2] = (cursorIndex == 2) ? '-': buf[2];
				dis_buf[3] = (cursorIndex == 3) ? '-': buf[3];
				{
					if(enterSub < 3)
					{	
						LCDPrintf(8,9,1,lang,"%s %s %c%c:%c%c",ui_aisle_admin.no_3[lang],ui_acdc_man.startTime[lang],
							dis_buf[0],dis_buf[1],dis_buf[2],dis_buf[3]);
					}
					else	
						LCDPrintf(8,11,1,lang,"%s %s %c%c:%c%c",ui_aisle_admin.no_4[lang],ui_acdc_man.stopTime[lang],
							dis_buf[0],dis_buf[1],dis_buf[2],dis_buf[3]);	
				}
						
			}

			key = ReadKeyValue();
			OSTimeDly(5);
			switch(key)
			{
				case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
					

					if(cursorIndex == 0 && (key - '0') >= 3)
						break;
					if(cursorIndex == 1 && (buf[0] == '2') && ((key - '0') >= 4))
						break;
					if(cursorIndex == 2 && (key - '0') >= 6)
						break;
					
					buf[cursorIndex] = key;
					if(cursorIndex == 3)
					{
						if(enterSub < 3)
						{
							stDevPtr->HourOn[no - 1] = (buf[0] - '0') * 10 + (buf[1] - '0');
							stDevPtr->MinOn[no - 1] = (buf[2] - '0') * 10 + (buf[3] - '0');
						}
						else
						{
							stDevPtr->HourOff[no - 1] = (buf[0] - '0') * 10 + (buf[1] - '0');
							stDevPtr->MinOff[no - 1] = (buf[2] - '0') * 10 + (buf[3] - '0');
						}
						enterSub = 0;
						topFlushFlag = 1;
						break;
					}
					cursorIndex = (++cursorIndex) % 4;
					subFlushFlag = 1;
					break;
				case '>':
					cursorIndex = (++cursorIndex) % 4;
					break;
				case '<':
					cursorIndex = (cursorIndex) ? (--cursorIndex): 3;	
					break;
				case 'C':
					enterSub = 0;
					topFlushFlag = 1;	
					break;
				case 'E'://确定编辑
					if(enterSub < 3)
					{
						stDevPtr->HourOn[no - 1] = (buf[0] - '0') * 10 + (buf[1] - '0');
						stDevPtr->MinOn[no - 1] = (buf[2] - '0') * 10 + (buf[3] - '0');
					}
					else
					{
						stDevPtr->HourOff[no - 1] = (buf[0] - '0') * 10 + (buf[1] - '0');
						stDevPtr->MinOff[no - 1] = (buf[2] - '0') * 10 + (buf[3] - '0');
					}
					enterSub = 0;
					topFlushFlag = 1;
					break;
			}

		}
		
		
	}

}



/*********************************************************************************************************
** Function name:     	acdc_hotTimerHandlePage
** Creater By		:	yoc 2014.05.07
** Descriptions:	   加热定时操作
** input parameters:    cabinet 柜号type 1压缩机  2展示灯
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void acdc_hotTimerHandlePage(unsigned char cabinet)
{

	unsigned char topFlushFlag = 1,topReturnFlag = 0;
	unsigned char lang,key;

	
	ST_HOT_DEV *hotPtr;
	if(cabinet == 1)
	{
		hotPtr = &UserPara.box1Hot;
	}
	else if(cabinet == 2)
	{	
		hotPtr = &UserPara.box2Hot;
	}
	else
		return;

	lang = SystemPara.Language;
#if 1

	while(1)
	{

		if(topFlushFlag)
		{
			topFlushFlag = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);

			LCDPrintf(8,1,0,lang,"%s",ui_acdc_man.timerManage[lang]);
			if(hotPtr->flag & (0x01 << 2))
				LCDPrintf(8,5,0,lang,"%s %s1:%s",ui_aisle_admin.no_1[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,5,0,lang,"%s %s1:%s",ui_aisle_admin.no_1[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.close[lang]);

			if(hotPtr->flag & (0x01 << 3))
				LCDPrintf(8,7,0,lang,"%s %s2:%s",ui_aisle_admin.no_2[lang],
										ui_acdc_man.durTime[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,7,0,lang,"%s %s2:%s",ui_aisle_admin.no_2[lang],
										ui_acdc_man.durTime[lang],ui_aisle_admin.close[lang]);
			if(hotPtr->flag & (0x01 << 4))
				LCDPrintf(8,9,0,lang,"%s %s3:%s",ui_aisle_admin.no_3[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,9,0,lang,"%s %s3:%s",ui_aisle_admin.no_3[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.close[lang]);
			if(hotPtr->flag & (0x01 << 5))
				LCDPrintf(8,11,0,lang,"%s %s4:%s",ui_aisle_admin.no_4[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,11,0,lang,"%s %s4:%s",ui_aisle_admin.no_4[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.close[lang]);
			if(hotPtr->flag & (0x01 << 6))
				LCDPrintf(8,13,0,lang,"%s %s5:%s",ui_aisle_admin.no_5[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,13,0,lang,"%s %s5:%s",ui_aisle_admin.no_5[lang],
									ui_acdc_man.durTime[lang],ui_aisle_admin.close[lang]);
						
		}

		if(topReturnFlag)
		{
			return ;
		}

		key = ReadKeyValue();
		OSTimeDly(5);
		
		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':
				acdc_hotTimerSetPage(cabinet,(key - '0'));
				
				if((hotPtr->flag & 0x7C) || (hotPtr->flag & 0x01) )
				{
					hotPtr->flag |= 0x02;
				}	
				else
					hotPtr->flag &= ~0x02;
				tempFlush = 1;
				topFlushFlag = 1;
				break;
			case 'C':

				topReturnFlag = 1;
				break;
			default:
				break;
				
		}	
	}
#endif	
	

	

}



/*********************************************************************************************************
** Function name:     	acdc_timerHandlePage
** Creater By		:	yoc 2014.05.07
** Descriptions:	   压缩机和展示灯定时操作页面
** input parameters:    cabinet 柜号type 1压缩机  2展示灯3 加热 4除臭
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char acdc_timerHandlePage(unsigned char cabinet,unsigned char type)
{
	unsigned char topFlushFlag = 1,topReturnFlag = 0,enterSub = 0,subFlushFlag = 0;
	unsigned char lang,key,tempState,changed = 0;
	unsigned char cursorIndex = 0;
	char buf[4] = {' '},dis_buf[4];

	DEVICECONTROLTIME *stDevPtr;

	
	if(cabinet == 1)
	{
		if(type == 1)
			stDevPtr = &UserPara.CompressorCtr;
		else if(type == 2)//展示灯
			stDevPtr = &UserPara.LEDCtr;	
		else if(type == 4)//除臭
			stDevPtr = &UserPara.box1Chuchou;
		else 
			return changed;
	}	
	else
	{
		if(type == 1)
			stDevPtr = &UserPara.CompressorCtr2Bin;
		else if(type == 2)//展示灯
			stDevPtr = &UserPara.LEDCtr2Bin;	
		else if(type == 4)//除臭
			stDevPtr = &UserPara.box2Chuchou;
		else 
			return changed;
	}
		
	lang = SystemPara.Language;
	while(1)
	{
		if(topFlushFlag)
		{
			topFlushFlag = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			if(stDevPtr->flag & 0x02)
				LCDPrintf(8,1,0,lang,"%s %s",ui_acdc_man.timerManage[lang],ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,1,0,lang,"%s %s",ui_acdc_man.timerManage[lang],ui_aisle_admin.close[lang]);

			LCDPrintf(8,5,0,lang,"%s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.open[lang]);
			LCDPrintf(8,7,0,lang,"%s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);
			if(stDevPtr->flag & 0x02)
			{
				LCDPrintf(8,9,0,lang,"%s %s %02d:%02d",ui_aisle_admin.no_3[lang],ui_acdc_man.startTime[lang],
													stDevPtr->HourOn,
													stDevPtr->MinOn);
				LCDPrintf(8,11,0,lang,"%s %s %02d:%02d",ui_aisle_admin.no_4[lang],ui_acdc_man.stopTime[lang],
													stDevPtr->HourOff,
													stDevPtr->MinOff);	
			}
						
		}

		if(topReturnFlag)
		{
			return changed;
		}

		key = ReadKeyValue();
		OSTimeDly(5);
		
		switch(key)
		{
			case '1':case '2':
				tempState = (key == '1')? 1 : 0;	
				stDevPtr->flag = (tempState << 1);
				topFlushFlag = 1;
				break;
			case '3':
				if(stDevPtr->flag & 0x02)
				{
					subFlushFlag = 1;
					enterSub = 1;
					cursorIndex = 0;
					buf[0] = stDevPtr->HourOn  / 10 + '0';
					buf[1] = stDevPtr->HourOn  % 10 + '0';
					buf[2] = stDevPtr->MinOn   / 10 + '0';
					buf[3] = stDevPtr->MinOn   % 10 + '0';
				}
				
				break;
			case '4':
				if(stDevPtr->flag & 0x02)
				{
					subFlushFlag = 1;
					enterSub = 3;
					cursorIndex = 0;
					buf[0] = stDevPtr->HourOff  / 10 + '0';
					buf[1] = stDevPtr->HourOff  % 10 + '0';
					buf[2] = stDevPtr->MinOff   / 10 + '0';
					buf[3] = stDevPtr->MinOff   % 10 + '0';
				}
				break;
			case 'C':
				enterSub = 0;
				topReturnFlag = 1;
				break;
			default:
				break;
				
		}
		
		while(enterSub)//编辑界面
		{
			if(subFlushFlag)
			{
				subFlushFlag = 0;
				dis_buf[0] = (cursorIndex == 0) ? '-': buf[0];
				dis_buf[1] = (cursorIndex == 1) ? '-': buf[1];
				dis_buf[2] = (cursorIndex == 2) ? '-': buf[2];
				dis_buf[3] = (cursorIndex == 3) ? '-': buf[3];
				{
					if(enterSub < 3)
					{	
						LCDPrintf(8,9,1,lang,"%s %s %c%c:%c%c",ui_aisle_admin.no_3[lang],ui_acdc_man.startTime[lang],
							dis_buf[0],dis_buf[1],dis_buf[2],dis_buf[3]);
					}
					else	
						LCDPrintf(8,11,1,lang,"%s %s %c%c:%c%c",ui_aisle_admin.no_4[lang],ui_acdc_man.stopTime[lang],
							dis_buf[0],dis_buf[1],dis_buf[2],dis_buf[3]);	
				}
						
			}

			key = ReadKeyValue();
			OSTimeDly(5);
			switch(key)
			{
				case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
					

					if(cursorIndex == 0 && (key - '0') >= 3)
						break;
					if(cursorIndex == 1 && (buf[0] == '2') && ((key - '0') >= 4))
						break;
					if(cursorIndex == 2 && (key - '0') >= 6)
						break;
					
					buf[cursorIndex] = key;
					if(cursorIndex == 3)
					{
						if(enterSub < 3)
						{
							stDevPtr->HourOn = (buf[0] - '0') * 10 + (buf[1] - '0');
							stDevPtr->MinOn = (buf[2] - '0') * 10 + (buf[3] - '0');
						}
						else
						{
							stDevPtr->HourOff = (buf[0] - '0') * 10 + (buf[1] - '0');
							stDevPtr->MinOff = (buf[2] - '0') * 10 + (buf[3] - '0');
						}
						enterSub = 0;
						topFlushFlag = 1;
						break;
					}
					cursorIndex = (++cursorIndex) % 4;
					subFlushFlag = 1;
					break;
				case '>':
					cursorIndex = (++cursorIndex) % 4;
					break;
				case '<':
					cursorIndex = (cursorIndex) ? (--cursorIndex): 3;	
					break;
				case 'C':
					enterSub = 0;
					topFlushFlag = 1;	
					break;
				case 'E'://确定编辑
					if(enterSub < 3)
					{
						stDevPtr->HourOn = (buf[0] - '0') * 10 + (buf[1] - '0');
						stDevPtr->MinOn = (buf[2] - '0') * 10 + (buf[3] - '0');
					}
					else
					{
						stDevPtr->HourOff = (buf[0] - '0') * 10 + (buf[1] - '0');
						stDevPtr->MinOff = (buf[2] - '0') * 10 + (buf[3] - '0');
					}
					acdc_chuchou_API(cabinet,stDevPtr->flag & 0x01);

					enterSub = 0;
					topFlushFlag = 1;
					changed = 1;
					break;
			}

		}
		
		
	}
}

/*********************************************************************************************************
** Function name:     	acdc_manage
** Creater By		:	yoc 2014.05.07
** Descriptions:	   展示灯压缩机管理操作页面
** input parameters:    type 1压缩机  2展示灯3加热 4除臭
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void acdc_manage(const unsigned char type)
{
	unsigned char topFlush = 1,topReturn = 0,changed = 0;
	unsigned char key,lang,cabinetNO = 1,flag,rst;
	char *acdcTitleStr;
	DEVICECONTROLTIME *stDevPtr;
	lang= SystemPara.Language;
	
	while(SystemPara.SubBinOpen || SystemPara.hefangGui)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_acdc_man.manage[lang]);
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enternCabinetNo[lang]);
			LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.A_Bin[lang]);
			if(SystemPara.SubBinOpen && SystemPara.hefangGui)
			{
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.B_Bin[lang]);
				LCDPrintf(8,11,0,lang," %s%s",ui_aisle_admin.no_3[lang],ui_aisle_admin.hefan[lang]);
			}
			else if(SystemPara.SubBinOpen)
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.B_Bin[lang]);
			else if(SystemPara.hefangGui)
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.hefan[lang]);
			
		}
		key = ReadKeyValue();
		OSTimeDly(5);
		if(key == '1')
		{
		
			cabinetNO = 1;
			break;
		}	
		else  if(key == '2')
		{
			if(SystemPara.SubBinOpen)
			{
				cabinetNO = 2;
		
				break;
			}
			else if(SystemPara.hefangGui)
				return;
			topFlush = 1;	
			
		}
		else if(key == '3')
		{
			if(SystemPara.hefangGui)
				return;
			topFlush = 1;	
		}
		else if(key == 'C')
			return;
	}

	if(cabinetNO == 1)
	{
		if(type == 1)//压缩机
		{
			acdcTitleStr = ui_acdc_man.cprtManage[lang];
			stDevPtr = &UserPara.CompressorCtr;
		}	
		else if(type == 2)//展示灯
		{
			acdcTitleStr = ui_acdc_man.lightManage[lang];
			stDevPtr = &UserPara.LEDCtr;
		}	
		else if(type == 4)//除臭
		{
			acdcTitleStr = ui_acdc_man.chuchouManage[lang];
			stDevPtr = &UserPara.box1Chuchou;
		}
		else
			return;
		
	}	
	else if(cabinetNO == 2)
	{
		if(type == 1)
		{
			acdcTitleStr = ui_acdc_man.cprtManage[lang];
			stDevPtr = &UserPara.CompressorCtr2Bin;
		}	
		else if(type == 2)
		{
			acdcTitleStr = ui_acdc_man.lightManage[lang];
			stDevPtr = &UserPara.LEDCtr2Bin;
		}
		else if(type == 4)//除臭
		{
			acdcTitleStr = ui_acdc_man.chuchouManage[lang];
			stDevPtr = &UserPara.box2Chuchou;
		}
		else
			return;
			
	}
	else
	{
		return;
	}

	
	topFlush = 1;
	while(1)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			if(stDevPtr->flag & (0x01 << 1))
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.timerStarting[lang]);
			else if(stDevPtr->flag & 0x01)
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_aisle_admin.close[lang]);
			
			LCDPrintf(8,5,0,lang,"%s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.open[lang]);
			LCDPrintf(8,7,0,lang,"%s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);
			LCDPrintf(8,9,0,lang,"%s %s",ui_aisle_admin.no_3[lang],ui_acdc_man.timerManage[lang]);	

			

		}

		if(topReturn)
			return;

		key = ReadKeyValue();
		OSTimeDly(5);
		switch(key)
		{
			case '1':case '2':
				flag = (key == '1') ? 1 : 0;
				
				
				TraceCompress("acdc man type = %d %d\r\n",type,stDevPtr->flag);
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.operating[lang]);
				OSTimeDly(100);
				if(type == 4)
					rst = acdc_chuchou_API(cabinetNO,flag & 0x01);
				else
				{	
					stDevPtr->flag = flag;
					if(cabinetNO == 1)
						rst = acdc_handle_API(1,UserPara.LEDCtr.flag & 0x01,UserPara.CompressorCtr.flag & 0x01);							
					else	
						rst = acdc_handle_API(2,UserPara.LEDCtr2Bin.flag & 0x01,UserPara.CompressorCtr2Bin.flag & 0x01);
				}
				if(rst)
				{
					stDevPtr->flag = flag;
					LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.operateSuc[lang]);
				
				}
				else
					LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.operateFail[lang]);
				
				OSTimeDly(200);	
					
				
				topFlush = 1;	
				break;
				
			case '3':
				changed = acdc_timerHandlePage(cabinetNO,type);
				if(changed == 1)
				{
					chuchouFlush = 1;	
				}
				topFlush = 1;
				break;
			case 'C':
				topReturn = 1;
				break;
			case 'E':
				topReturn = 1;
				break;
			default:
				break;
		}
		
	}
		
	
}

void ACDCManageLight(void)
{
	acdc_manage(2);
}

void ACDCManageCompressor(void)
{
	acdc_manage(1);
}


//除臭界面
void acdc_controlChuchou(void)
{
	acdc_manage(4);
//	WriteUserSystemPara(UserPara);
}


//加热界面
void acdc_controlHot(void)
{
	unsigned char topFlush = 1,topReturn = 0,enterSub =  0;
	unsigned char key,lang,cabinetNO = 1,flag,rst;
	char *acdcTitleStr;
	unsigned short temp;
	ST_HOT_DEV *stDevPtr;
	lang= SystemPara.Language;


	
	while(SystemPara.SubBinOpen || SystemPara.hefangGui)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_acdc_man.manage[lang]);
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enternCabinetNo[lang]);
			LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.A_Bin[lang]);
			if(SystemPara.SubBinOpen && SystemPara.hefangGui)
			{
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.B_Bin[lang]);
				LCDPrintf(8,11,0,lang," %s%s",ui_aisle_admin.no_3[lang],ui_aisle_admin.hefan[lang]);
			}
			else if(SystemPara.SubBinOpen)
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.B_Bin[lang]);
			else if(SystemPara.hefangGui)
				LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.hefan[lang]);
			
		}
		key = ReadKeyValue();
		OSTimeDly(5);
		if(key == '1')
		{
		
			cabinetNO = 1;
			break;
		}	
		else  if(key == '2')
		{
			if(SystemPara.SubBinOpen)
			{
				cabinetNO = 2;
			
				break;
			}
			else if(SystemPara.hefangGui)
				return;
			topFlush = 1;	
			
		}
		else if(key == '3')
		{
			if(SystemPara.hefangGui)
				return;
			topFlush = 1;	
		}
		else if(key == 'C')
			return;
	}

	

	if(cabinetNO == 1)
	{
		acdcTitleStr = ui_acdc_man.hotManage[lang];
		stDevPtr = &UserPara.box1Hot;	
	}	
	else if(cabinetNO == 2)
	{
		acdcTitleStr = ui_acdc_man.hotManage[lang];
		stDevPtr = &UserPara.box2Hot;	
			
	}
	else
	{
		return;
	}

	
	topFlush = 1;
	while(1)
	{
		if(topFlush)
		{
			stDevPtr->curTemp = acdc_getTemp_API(cabinetNO); 
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			if(stDevPtr->flag & 0x02)
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.timerStarting[lang]);
			else if(stDevPtr->flag & 0x01)
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_aisle_admin.open[lang]);
			else
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_aisle_admin.close[lang]);

			LCDPrintf(8,5,0,lang,"%s: %02d.%02d ℃",ui_acdc_man.curTemp[lang],
					stDevPtr->curTemp / 10,(stDevPtr->curTemp % 10) * 10);
			
			LCDPrintf(8,7,0,lang,"%s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.open[lang]);
			LCDPrintf(8,9,0,lang,"%s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);

			LCDPrintf(8,11,0,lang,"%s %s",ui_aisle_admin.no_3[lang],ui_acdc_man.timerManage[lang]);	

			if(stDevPtr->flag & 0x01)
				LCDPrintf(8,13,0,lang,"%s %s:  %02d.%02d ℃",ui_aisle_admin.no_4[lang],ui_acdc_man.setTemp[lang],
												stDevPtr->setTemp1 / 10,(stDevPtr->setTemp1 % 10) * 10 );	
			
			

		}

		if(topReturn)
		{
			//WriteUserSystemPara(UserPara);
			return;
		}
			

		key = ReadKeyValue();
		OSTimeDly(5);
		switch(key)
		{
			case '1':case '2':
				flag = (key == '1') ? 1 : 0;				
				LCDPrintf(8,1,0,lang,"%s %s",acdcTitleStr,ui_acdc_man.operating[lang]);					
				rst = acdc_hot_API(cabinetNO,(flag & 0x01) * (ACDC_MAX_HOT_TEMP / 2));
				if(rst)
				{
					stDevPtr->flag &= ~0x03;
					stDevPtr->flag |= flag;		
					stDevPtr->flag |= ((flag && (stDevPtr->flag & 0x7C)) ? 0x02 : 0);
					stDevPtr->setTemp1 = ACDC_MAX_HOT_TEMP / 2;
				}

				
				topFlush = 1;	
				break;
				
			case '3':
				acdc_hotTimerHandlePage(cabinetNO);
				topFlush = 1;
				break;
			case '4'://设置温度
				if(!(stDevPtr->flag & 0x01))
					break;
				temp = 0;
				topFlush = 1;
				enterSub = 1;
				while(enterSub)
				{
					if(topFlush)
					{
						topFlush = 0;
						LCDPrintf(8,13,1,lang,"%s %s:  %02d.%02d ℃",ui_aisle_admin.no_4[lang],ui_acdc_man.setTemp[lang],
												temp / 10,(temp % 10) * 10);	
					}
					key = ReadKeyValue();
					OSTimeDly(5);
					switch(key)
					{
						case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
							temp = temp * 10 + (key - '0');
							topFlush = 1;
							break;
						case 'C':
							enterSub = 0;
							topFlush = 1;
							break;
						case 'E'://确定编辑
							stDevPtr->setTemp1 = ((temp >= ACDC_MAX_HOT_TEMP) ?ACDC_MAX_HOT_TEMP  : temp);
							TraceCompress("temp: %d   %d\r\n",stDevPtr->setTemp1,temp);
							if((stDevPtr->flag & 0x01) || (stDevPtr->flag & 0x02))
								acdc_hot_API(cabinetNO,stDevPtr->setTemp1);
							enterSub = 0;
							topFlush = 1;
							break;
						default:
							break;
					}
					
					
					
				}
				break;
			case 'C':
				topReturn = 1;
				break;
			case 'E':
				topReturn = 1;
				break;
			default:
				break;
		}
		
	}
}



/*********************************************************************************************************
** Function name:     	acdc_timerHandle
** Creater By		:	yoc 2014.05.07
** Descriptions:	   压缩机和展示灯定时操作
** input parameters:    cabinet 柜号type 1压缩机  2展示灯
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void acdc_timerHandle(unsigned char cabinet)
{
	unsigned short curTime,timerON,timerOFF,rst;
	unsigned char ledFlag = 0,cprFlag = 0;

	DEVICECONTROLTIME *ledPtr,*cprPtr;
	if(cabinet == 1)
	{
		ledPtr = &UserPara.LEDCtr;
		cprPtr = &UserPara.CompressorCtr;
	}
	else if(cabinet == 2)
	{
		ledPtr = &UserPara.LEDCtr2Bin;
		cprPtr = &UserPara.CompressorCtr2Bin;
	}
	else
		return;

	if(!(ledPtr->flag & 0x02) && !(cprPtr->flag & 0x02))
		return;
	
	curTime = RTCData.hour * 60 + RTCData.minute;//将时间转换成分钟数

	
	if(ledPtr->flag & 0x02)
	{
		timerON = ledPtr->HourOn * 60 + ledPtr->MinOn;
		timerOFF = ledPtr->HourOff * 60 + ledPtr->MinOff;
		if(timerOFF > timerON)
		{
			if((curTime >= timerON) && (curTime <= timerOFF))//当前时间在定时范围内
				ledFlag = 1;
		}
		else
		{
			if(!((curTime >= timerOFF) && (curTime <= timerON)))
				ledFlag = 1;
				
		}
		
	}
	else
		ledFlag = ledPtr->flag & 0x01;

	if(cprPtr->flag & 0x02)
	{
		timerON = cprPtr->HourOn * 60 + cprPtr->MinOn;
		timerOFF = cprPtr->HourOff * 60 + cprPtr->MinOff;
		if(timerOFF > timerON)
		{
			if((curTime >= timerON) && (curTime <= timerOFF))//当前时间在定时范围内
				cprFlag = 1;
		}
		else
		{
			if(!(curTime >= timerOFF) && (curTime <= timerON))
				cprFlag = 1;
				
		}
	}
	else
		cprFlag = cprPtr->flag & 0x01;
	
	TraceCompress("ledFlag=%d,cprFlag=%d\r\n",ledFlag,cprFlag);
	if((ledFlag != (ledPtr->flag & 0x01)) || (cprFlag != (cprPtr->flag & 0x01)))
	{
		TraceCompress("acdc_handle_API:%d %d\r\n",ledFlag,cprFlag);
		rst = acdc_handle_API(cabinet,ledFlag,cprFlag);
		if(rst)
		{
			ledPtr->flag &= ~0x01;
			ledPtr->flag |= (ledFlag & 0x01);

			cprPtr->flag &= ~0x01;
			cprPtr->flag |= (cprFlag & 0x01);
		}
	}
		

	
}


/*********************************************************************************************************
** Function name:     	acdc_chuchouTimerHandle
** Creater By		:	yoc 2014.05.07
** Descriptions:	   	除臭定时
** input parameters:    cabinet 柜号
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void acdc_chuchouTimerHandle(unsigned char cabinet)
{
	unsigned short curTime,timerON,timerOFF,rst;
	unsigned char chuchouFlag = 0;
	DEVICECONTROLTIME *chuchouPtr;
	if(cabinet == 1)
	{
		chuchouPtr = &UserPara.box1Chuchou;
	}
	else if(cabinet == 2)
	{
		chuchouPtr = &UserPara.box2Chuchou;
	}
	else
		return;

	if(!(chuchouPtr->flag & 0x02) )
		return;
	
	curTime = RTCData.hour * 60 + RTCData.minute;//将时间转换成分钟数
	
	if(chuchouPtr->flag & 0x02)
	{
		timerON = chuchouPtr->HourOn * 60 + chuchouPtr->MinOn;
		timerOFF = chuchouPtr->HourOff * 60 + chuchouPtr->MinOff;
		if(timerOFF > timerON)
		{
			if((curTime >= timerON) && (curTime <= timerOFF))//当前时间在定时范围内
				chuchouFlag = 1;
		}
		else
		{
			if(!((curTime >= timerOFF) && (curTime <= timerON)))
				chuchouFlag = 1;			
		}	
	}
	else
		chuchouFlag = chuchouPtr->flag & 0x01;
	//TraceCompress("ledFlag=%d,cprFlag=%d\r\n",ledFlag,cprFlag);
	if((chuchouFlag != (chuchouPtr->flag & 0x01)) || (chuchouFlush == 1))
	{
		TraceCompress("acdc_chuchou_API:%d %d\r\n",chuchouFlag,chuchouPtr->flag);
		rst = acdc_chuchou_API(cabinet,chuchouFlag);
		if(rst)
		{
			chuchouPtr->flag &= ~0x01;
			chuchouPtr->flag |= (chuchouFlag & 0x01);
			chuchouFlush = 0;
		}
	}
		

	
}


/*********************************************************************************************************
** Function name:     	acdc_hotTimerHandle
** Creater By		:	yoc 2014.05.07
** Descriptions:	   加热定时操作
** input parameters:    cabinet 柜号
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void acdc_hotTimerHandle(unsigned char cabinet)
{
	unsigned short curTime,timerON,timerOFF,rst,getTemp = 1;
	unsigned char hotFlag = 0,i;
	ST_HOT_DEV *hotPtr;
	if(cabinet == 1)
	{
		hotPtr = &UserPara.box1Hot;
	}
	else if(cabinet == 2)
	{	
		hotPtr = &UserPara.box2Hot;
	}
	else
		return;

	if(!(hotPtr->flag & 0x02) )//没开启定时器
		return;
	
	curTime = RTCData.hour * 60 + RTCData.minute;//将时间转换成分钟数

	for(i = 0;i < 5;i++)
	{
		if(!(hotPtr->flag & (0x01 << (i + 2))))
			continue;
			
		timerON = hotPtr->HourOn[i] * 60 + hotPtr->MinOn[i];
		timerOFF = hotPtr->HourOff[i] * 60 + hotPtr->MinOff[i];
		if(timerOFF > timerON)
		{
			if((curTime >= timerON) && (curTime <= timerOFF))//当前时间在定时范围内
			{
				hotFlag = 1;
				getTemp = hotPtr->setTemp[i];
				break;
			}
		}
		else
		{
			if(!((curTime >= timerOFF) && (curTime <= timerON)))
			{
				hotFlag = 1;
				getTemp = hotPtr->setTemp[i];
				break;
			}	
		}
	}

	getTemp = (hotFlag == 0) ? 0 : getTemp; 

	TraceCompress("getTemp = %d,curSetTemp=%d,hotFlag = %d\r\n",getTemp,curSetTemp,hotFlag);
	if((tempFlush == 1) || (curSetTemp != getTemp))
	{
		rst = acdc_hot_API(cabinet,getTemp);
		if(rst)
		{
			curSetTemp = getTemp;
			tempFlush = 0;
		}
	
	}	
#if 0
	
	if(hotPtr->curTemp > (getTemp + 3))//关
	{
		if(hotPtr->flag & 0x01)//确认是开了
		{
			TraceCompress("open hot:the temp=%d\r\n",getTemp);
			rst = acdc_hot_API(cabinet,0);
			if(rst)
			{
				hotPtr->flag &= ~0x01;
			}
		}
		
	}
	else if(getTemp > 3 && hotPtr->curTemp < (getTemp - 3))//开
	{
		if(hotPtr->flag & 0x01)//已经开了
			return;
		TraceCompress("Close hot:the temp=%d\r\n",getTemp);
		rst = acdc_hot_API(cabinet,1);
		if(rst)
		{
			hotPtr->flag |= 0x01;
		}
	}
#endif
	
}


void ACDCTimingHandle(unsigned char Binnum)
{
	static unsigned char acdc_init = 0;
	//上电同步一次
	if(acdc_init == 0){
		acdc_handle_API(1,UserPara.LEDCtr.flag & 0x01,UserPara.CompressorCtr.flag & 0x01);							
		if(SystemPara.SubBinOpen)
			acdc_handle_API(2,UserPara.LEDCtr2Bin.flag & 0x01,UserPara.CompressorCtr2Bin.flag & 0x01);
		acdc_init = 1;
	}


	acdc_timerHandle(1);
	if(SystemPara.SubBinOpen)
		acdc_timerHandle(2);
	acdc_hotTimerHandle(Binnum);
	acdc_chuchouTimerHandle(Binnum);
	
	
}





/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/



