/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CHANNEL
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        货道控制菜单界面                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
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
#include "CHANNEL.h"
#include "LANGUAGE.h"
#include "LIFTTABLE.h"
 

extern uint8_t AdminRPTWeihu(uint8_t adminType,uint8_t Column,uint8_t ColumnSum);





const UI_AISLE_ADMIN ui_aisle_admin = {
	
	{
		"货道属性设置",
		"Cabinet setting",
		"Cabinet setting",
		"Cabinet setting",
		"CONFIGURA\x83\x82\O DE ATRIBUTO DE ARMARIO"
	},
	{
		"请输入箱柜编号:",
		"Input cabinet code:",
		"Input cabinet code:",
		"Input cabinet code:",
		"CODIGO DO ARMARIO:"
	},
	{
		"A 柜",
		"A cabinet",
		"A cabinet",
		"A cabinet",
		"A ARMARIO"
	},
	{
		"B 柜",
		"B cabinet",
		"B cabinet",
		"B cabinet",
		"B ARMARIO"
	},
	{
		"盒饭柜",
		"CVS Cabinet",
		"CVS Cabinet",
		"CVS Cabinet",
		"CVS Cabinet"
	},
	{
		"货道初始化",
		"Column initialize",
		"Column initialize",
		"Column initialize",
		"COLUNA INICIALIZAR"
	},
	{
		"初始化成功",
		"Initialization Succeed",
		"Column initialize",
		"Column initialize",
		"COLUNA INICIALIZAR"
	},
	{
		"层架管理",
		"Tray Manage",
		"Tray Manage",
		"Tray Manage",
		"GERENCIADOR DE BANDEIJA"
	},
	{
		"请输入层编号:",
		"Input tray code:",
		"Input tray code:",
		"RAF KODUNU GIR:",
		"INSIRA C\x80\DIGO DA BANDEJA:"
	},
	{
		"货道管理",
		"Column Manage",
		"Column Manage",
		"SUTUN",
		"COLUNA DE BANDEIJA"
	},
	{
		"请输入货道号:",
		"Input column code:",
		"Input column code:",
		"SUTUN KODUNU GIR:",
		"C\x80\DIGO DA BANDEJA:"
	},
	{
		"货道不存在! ",
		"Column N/A!",
		"Column N/A!",
		"NUMARA!",
		"COLUNA N\x82\O DISPONIVEL!"
	},
	{
		"货道当前状态 :",
		"Column status:",
		"Column status:",
		"SUTUN STATUS:",
		"SITUA\x83\x82\O DA BANDEJA:"
	},
	{
		"最大存货量:",
		"Capacity:",
		"Capacity: %s ",
		"Capacity: %s ",
		"CAPACIDADE: %s "
	},
	{
		"层状态:",
		"Tray Status:",
		"Tray Status:",
		"Tray Status:",
		"BENDEIJA SITUA\x83\x82\O:"
	},
	{
		"开启",
		"ON",
		"ON",
		"ON",
		"ATIVAR"
	},
	{
		"关闭",
		"OFF",
		"OFF",
		"OFF",
		"DESLIGADO"
	},
	{
		"确定",
		"YES",
		"YES",
		"GIRIS",
		"ENTER"
	},
	{
		"取消",
		"CANCEL",
		"CANCEL",
		"IPTAL",
		"CANCELAR"
	},
	{
		"①","1.","1.","1.","1."	
	},
	{
		"②","2.","2.","2.","2."	
	},
	{
		"③","3.","3.","3.","3."
	},
	{
		"④","4.","4.","4.","4."		
	},
	{
		"⑤","5.","5.","5.","5."
	}


	,
	{
		"货道参数设置",
		"Column Param Set",
		"Column Param Set",
		"SUTUN DEGER",
		"ATRIBUTO DA BANDEJA"
	},
	{
		"货道属性 ",
		"Column attribute",
		"Column attribute",
		"SUTUN DEGER",
		"ATRIBUTO DA BANDEJA"
	},
	{
		"添货",
		"Load products",
		"Load products",
		"URUN YUKLE",
		"CARREGANDO PRODUTOS"
	},
	{
		"查看货道状态",
		"Check column status",
		"Check column status",
		"SUTUN KONTROL",
		"SITUA\x83\x82\O DAS BANDEJAS"
	},
	{
		"货道检测",
		"Column self-test",
		"Column self-test",
		"SUTUN TEST ",
		"TESTE DE BANDEJA"
	}
	,
	{
		"拷机查询",
		"Kaoji Check",
		"Column self-test",
		"SUTUN TEST ",
		"TESTE DE BANDEJA"
	}
	,
	{
		"货道商品单价:",
		"Unit price:",
		"Unit price:",
		"BIRIM FIYAT:",
		"PRE\x83\O UNIT\x81\RIO:"
	},
	{
		"货道储货数量 :",
		"Column capacity:",
		"Column capacity:",
		"SUTUN KAPASITESI:",
		"CAPACIDADE DA BANDEJA:"
	},
	{
		"成功交易次数:",
		"Success deal qty:",
		"Success deal qty:",
		"BASARILI SATIS:",
		"%d SUCESSO :"
	},
	{
		"货道选货按键:",
		"Column SelectNum:",
		"Column SelectNum:",
		"Column SelectNum:",
		"Column SelectNum:"
	},
	{
		"货道商品编号:",
		"Column Goods Num:",
		"Column Goods Num:",
		"Column Goods Num:",
		"Column Goods Num:"
	},
	{
		"单货道添货",
		"Load for column",
		"Load for column",
		"SUTUN DOLDUR",
		"CARREGANDO A BANDEJA"
	},
	{
		"层货道添货",
		"Load for tray",
		"Load for tray",
		"RAF DOLDUR",
		"CARREGANDO A BANDEJA"
	},
	{
		"全货道添货",
		"Load for all columns",
		"Load for all columns",
		"TUM SUTUNLARI DOLDUR",
		"CARREGANDO TODAS AS BANDEJAS"
	},
	{
		"请选择操作:",
		"Pls choose:",
		"Pls choose:",
		"LUTFEN SECIN:",
		"POR FAVOR ESCOLHA:"
	},
	{
		"确定层添货?",
		"Load for the tray?",
		"Load for the tray?",
		"DOLDUR?",
		"CARREGANDO PARA %d BANDEIJA?"
	},
	{
		"查看货道状态",
		"Check column status",
		"Check column status",
		"SUTUN KONTROL",
		"SITUA\x83\x82\O DAS BANDEJAS"
	},
	{
		"故障货道:",
		"Errory column",
		"Errory column",
		"HATALI SUTUN",
		"ERRO BANDEJA"
	},
	{
		"缺货货道:",
		"Empty column",
		"Empty column",
		"BOS SUTUN",
		"BANDEJA VAZIA"
	},
	{
		"单价为0的货道:",
		"Price in 0 column",
		"Price in 0 column",
		"FIYATSIZ SUTUN",
		"PRE\x83\O NA BANDEJA 0"
	},
	{
		"正常货道:",
		"Normal Column",
		"Normal Column",
		"NORMAL SUTUN",
		"BANDEJA NORMAL"
	},
	{
		"全货道测试页面",
		"Self-testing",
		"Self-testing",
		"TEST EDILIYOR LUTFEN BEKLEYIN",
		"FAVOR AGUARDAR"
	},
	{
		"多层货道测试页面",
		"Self-testing",
		"Self-testing",
		"TEST EDILIYOR LUTFEN BEKLEYIN",
		"FAVOR AGUARDAR"
	},
	{
		"多货道测试页面",
		"Self-testing",
		"Self-testing",
		"TEST EDILIYOR LUTFEN BEKLEYIN",
		"FAVOR AGUARDAR"
	},
	{
		"单货道测试页面",
		"Self-testing",
		"Self-testing",
		"TEST EDILIYOR LUTFEN BEKLEYIN",
		"FAVOR AGUARDAR"
	}

		
		,
	{
		" 正在检测%d货道, 请稍等...",
		" Self-testing %d,pls wait...",
		" Self-testing %d,pls wait...",
		" TEST EDILIYOR LUTFEN BEKLEYIN",
		" EM TESTE %d,AGUARDE..."
	},
	{
		" 货道%d检测: 无货! ",
		" Column %d: Empty!",
		" Column %d: Empty!",
		" SUTUN BOS!",
		" BANDEJA %d: VAZIA!"
	},
	{
		" 货道%d检测: 正常! ",
		" Column %d: Normal!",
		" Column %d: Normal!",
		" SUTUN NORMAL!",
		" BANDEJA %d: NORMAL!"
	},
	{
		" 货道%d检测: 故障! ",
		" Column %d: Error!",
		" Column %d: Error!",
		" SUTUN HATALI!",
		" BANDEJA %d: ERRO!"
	},
	{
		" 货道%d检测: 电机未到位! ",
		" Column %d: MOTO ERR!",
		" Column %d: MOTO ERR!",
		" SUTUN HATALI!",
		" BANDEJA %d: ERRO!"
	},
	{
		" 货道%d不存在! ",
		" Column: nonentity!",
		" Column: nonentity!",
		" NUMARA!",
		" Column: nonentity!"
	},
	{
		" 货道%d检测: 通信故障! ",
		" Column %d:Communication failure!",
		" Column %d:Communication failure!",
		" SUTUN BAGLANTI HATASI!",
		" BANDEJA %d:ERRO COMUNICA\x83\x82O!"
	}
};






/*****************************************************************************
** Function name:	ChannelBinStr	  
** Descriptions:	获取货柜号 字符	  													 			
** parameters:							
** Returned value:	柜号字符
** 
*******************************************************************************/
char * ChannelBinStr(const unsigned char cabinetNo)
{
	unsigned char lang = SystemPara.Language;
	if(cabinetNo == 1)
		return ui_aisle_admin.A_Bin[lang];
	else if(cabinetNo == 2)
		return ui_aisle_admin.B_Bin[lang];
	else
		return NULL;
}

/*****************************************************************************
** Function name:	ChannelStateLookUp	  
** Descriptions:	查询货道当前状态changed by yoc 2014.04.07	  													 			
** parameters:							
** Returned value:	无
** 
*******************************************************************************/
void ChannelStateLookUp(unsigned char cabinetNo)
{
	unsigned char i,j,key;
	unsigned char hdBad[80]={0},hdOK[80]={0},hdEmpty[80]={0},hdZero[80]={0};
	unsigned char badNum = 0,okNum = 0,emptyNum = 0,zeroNum = 0,tempI,tempJ;
	unsigned char *ptr[8];
	char *strPtr[8],tempStr[5][30];
	unsigned char topFlush = 1,topReturnFlag = 0,pageNo= 0,maxPage = 0,lang;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	lang = SystemPara.Language;
	if(cabinetNo==1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo==2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_hd("ReadChannel cabinetNo is err %d\r\n",cabinetNo);
		return;
	}

	//初始化各个列表
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		if(!stHuodaoPtr[i].openFlag)
			continue;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(stHuodaoPtr[i].huodao[j].openFlag == 0 || !stHuodaoPtr[i].huodao[j].logicNo)
				continue;
			if(stHuodaoPtr[i].huodao[j].price == 0)		
				hdZero[zeroNum++] = stHuodaoPtr[i].huodao[j].logicNo;
			if(stHuodaoPtr[i].huodao[j].state == HUODAO_STATE_FAULT)
				hdBad[badNum++] = stHuodaoPtr[i].huodao[j].logicNo;
			if(stHuodaoPtr[i].huodao[j].state == HUODAO_STATE_NORMAL)
				hdOK[okNum++] = stHuodaoPtr[i].huodao[j].logicNo;	
			if(stHuodaoPtr[i].huodao[j].state == HUODAO_STATE_EMPTY)
				hdEmpty[emptyNum++] = stHuodaoPtr[i].huodao[j].logicNo;
			
		}
	}

	if(zeroNum && zeroNum <= 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnPriceZero[lang];
		ptr[maxPage++] = hdZero;
	}
	else if(zeroNum > 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnPriceZero[lang];
		strPtr[maxPage + 1] = ui_aisle_admin.columnPriceZero[lang];
		ptr[maxPage] = hdZero;
		ptr[maxPage + 1] = &hdZero[40];
		maxPage +=2;
	}
	
	if(badNum && badNum <= 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnBad[lang];
		ptr[maxPage++] = hdBad;
	}
	else if(badNum > 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnBad[lang];
		strPtr[maxPage + 1] = ui_aisle_admin.columnBad[lang];
		ptr[maxPage] = hdBad;
		ptr[maxPage + 1] = &hdBad[40];
		maxPage +=2;
	}
	
	if(okNum && okNum <= 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnlOK[lang];
		ptr[maxPage++] = hdOK;
	}
	else if(okNum > 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnlOK[lang];
		strPtr[maxPage + 1] = ui_aisle_admin.columnlOK[lang];
		ptr[maxPage] = hdOK;
		ptr[maxPage + 1] = &hdOK[40];
		maxPage +=2;
	}
	
	if(emptyNum && emptyNum <= 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnEmpty[lang];
		ptr[maxPage++] = hdEmpty;
	}
	else if(emptyNum > 40)
	{
		strPtr[maxPage] = ui_aisle_admin.columnEmpty[lang];
		strPtr[maxPage + 1] = ui_aisle_admin.columnEmpty[lang];
		ptr[maxPage] = hdEmpty;
		ptr[maxPage + 1] = &hdEmpty[40];
		maxPage +=2;
	}
	if(!maxPage)
		return;
	print_hd("okNum =%d %d %d %d\r\n",okNum,maxPage,ptr[0][0],ptr[0][1]);
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
			LCDPrintf(8,1,0,lang," %s",strPtr[pageNo]);
			memset((void *)tempStr,'\0',30 * 5);
			tempJ = 0;
			tempI = 0;
			for(i = 0;i < 40 && ptr[pageNo][i];i++)
			{	
				tempI = (i / 8);
				tempJ = (i % 8) * 3;
				snprintf(&tempStr[tempI][tempJ],4,"%02d,",ptr[pageNo][i]);
				print_hd("%d:%s\r\n",tempI,tempStr[tempI]);
			
			}
			switch(tempI)
			{
				case 4:
					LCDPrintf(8,13,0,lang," %s",tempStr[4]);
				case 3:
					LCDPrintf(8,11,0,lang," %s",tempStr[3]);
				case 2:
					LCDPrintf(8,9,0,lang," %s",tempStr[2]);
				case 1:
					LCDPrintf(8,7,0,lang," %s",tempStr[1]);
				case 0:
					LCDPrintf(8,5,0,lang," %s",tempStr[0]);
					break;
				default:
					break;
			}				
			
		}

		key = ReadKeyValue();
		switch(key)
		{
			case '>':
				pageNo = (pageNo >= (maxPage - 1)) ? 0 : pageNo + 1;
				topFlush = 1;
				break;
			case '<':
				pageNo = (pageNo == 0) ? (maxPage - 1) : pageNo - 1;
				topFlush = 1;
				break;
			case 'C':
				topReturnFlag = 1;
				break;
			default:
				break;		
		}

		if(topReturnFlag)
			return;	
		
	}

	
}


/*****************************************************************************
** Function name:	ChannelAPIProcess	
**
** Descriptions:	根据货道出货操作结果对商品余量和货道状态进行更新	
**					
**														 			
** parameters:		
					logicnum:逻辑货道编号
					Binnum：箱柜编号
					HandleType:操作类型	CHANNEL_OUTGOODS出货
					
					
** Returned value:	0,出货失败;1,成功；2,货道单价为0；3,该货道无货;4,货道故障
					5,无该货道;6,金额不足;7.因为GOC故障不能出货
** 
*******************************************************************************/
unsigned char ChannelAPIProcess(unsigned char LogicNum,unsigned char HandleType,unsigned char Binnum)
{
	uint8_t HandleRst = 0,physicnum=0xff;
	uint8_t count=0;
	uint32_t success=0;

	
	//by yoc
	if(Binnum>2 && SystemPara.hefangGui)
	{
		HandleRst = hefanOutGoodsHandle(LogicNum,HandleType,Binnum-3);
		//add by yoc  hefan
		save_hefan_huodao_info(Binnum-2);
		return HandleRst;
	}
	
	if(((SystemPara.Channel > 1)&&(Binnum==1))||
		((SystemPara.SubChannel > 1)&&(Binnum==2)))
	{
		HandleRst = LiftTableProcess(Binnum,LogicNum,HandleType);
		if(SystemPara.CunhuoKou==1)
		{
			count = ChannelGetParamValue(LogicNum,2,Binnum);
			if(count)
			{
				count -= 1;
				ChannelSetParam(LogicNum,Binnum,CHANNELCOUNT,count,0);
				if(count==0)
						ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
			}
			else
		   		ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
		}
		if(HandleRst==1)
		{
			success = ChannelGetParamValue(LogicNum,6,Binnum);
			success += 1;
			ChannelSetParam(LogicNum,Binnum,CHANNELSUCCESSCOUNT,success,success);
			if(SystemPara.CunhuoKou==1)
			{
				//count = ChannelGetParamValue(LogicNum,2,Binnum);
				//if(count)
				//{
				//	count -= 1;
				//	ChannelSetParam(LogicNum,Binnum,CHANNELCOUNT,count,0);
				//	if(count==0)
				//			ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
				//}
				//else
			    //	ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
			}
			else
				ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,1,0);
			return 1;
		}
		else
			return 0;
	}
	else
	{
		//因为GOC故障不能出货
		if(SystemPara.GOCIsOpen)
		{
			if(((stChannelErrForm.GOCErrFlag[0]==1)&&(Binnum==1))||
				((stChannelErrForm.GOCErrFlag[1]==1)&&(Binnum==2)))
			{
				return 7;
			}
		}
		//change by liya
		physicnum = ChannelGetPhysicNum(LogicNum,Binnum);
		HandleRst = ChannelAPIHandle(physicnum,HandleType,SystemPara.GOCIsOpen,Binnum);	
		TraceChannel("outgoodsRst=%x\r\n",HandleRst);
		if(SystemPara.CunhuoKou==1)
		{
			count = ChannelGetParamValue(LogicNum,2,Binnum);
			if(count)
			{
				count -= 1;
				ChannelSetParam(LogicNum,Binnum,CHANNELCOUNT,count,0);
				if(count==0)
						ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
			}
			else
		   		ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
		}
		if(HandleRst == 0)
		{
			if(CHANNEL_OUTGOODS == HandleType)
			{
				if(SystemPara.CunhuoKou==1)
				{
					//count = ChannelGetParamValue(LogicNum,2,Binnum);
					//if(count)
					//{
					//	count -= 1;
					//	ChannelSetParam(LogicNum,Binnum,CHANNELCOUNT,count,0);
					//	if(count==0)
					//			ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
					//}
					//else
				   	//	ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,3,0);
				}
				else
					ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,1,0);
				success = ChannelGetParamValue(LogicNum,6,Binnum);
				TraceChannel("Last_Success=%d\r\n",success);
				success += 1;
				TraceChannel("Now_Success=%d\r\n",success);
				ChannelSetParam(LogicNum,Binnum,CHANNELSUCCESSCOUNT,success,success);
			}
			ChannelSaveParam();
			return 1;
		}
		else
		if(HandleRst == 4)
		{
			ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,1,0);
		}
		else
		if((HandleRst==3))
		{
			ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,2,0);	
		}
		else
		if((HandleRst==1))
		{
			ChannelSetParam(LogicNum,Binnum,CHANNELSTATE,2,0);	
		}
	}
	ChannelSaveParam();
	return 0;	
}


/*****************************************************************************
** Function name:	ChannelKaojiReadFlash	
** Descriptions:	货道拷机数据读取											 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
unsigned char ChannelKaojiReadFlash(unsigned char cabinet)
{
	unsigned char buf[512];
	unsigned short crc,len,page;
	if(cabinet == 0 || cabinet > 2)
		return 0;
	page = (cabinet == 1) ? PAGE_BIN1_HUODAO_KAOJI : PAGE_BIN2_HUODAO_KAOJI;	
	AT45DBReadPage(page,buf);
	
	len = sizeof(ST_HUODAO_KAOJI);
	if(len > 500) return 0;
	crc = CrcCheck(buf,len);
	if(crc == INTEG16(buf[len],buf[len + 1]))
	{
		memcpy((void *)&stHuodaoKaoji[cabinet - 1],buf,len);
		return 1;
	}
	return 0;		
}

/*****************************************************************************
** Function name:	ChannelKaojiWriteFlash	
** Descriptions:	货道拷机数据写入											 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
unsigned char ChannelKaojiWriteFlash(unsigned char cabinet)
{
	uint8_t *ptr,buf[512] = {0};
	uint16_t crc,len,page;
	if(cabinet == 0 || cabinet > 2)
		return 0;
	page = (cabinet == 1) ? PAGE_BIN1_HUODAO_KAOJI : PAGE_BIN2_HUODAO_KAOJI;

	len = sizeof(ST_HUODAO_KAOJI);
	ptr = (uint8_t *)&stHuodaoKaoji[cabinet - 1];
	memcpy(buf,ptr,len);
	crc = CrcCheck(buf,len);
	buf[len] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	return AT45DBWritePage(page,buf);



}



/*****************************************************************************
** Function name:	ChannelKaojiCheckPage	
** Descriptions:	货道拷机查询页面											 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelKaojiCheckPage(unsigned char cabinet)
{
	unsigned char topFlush = 1,returnFlag = 0,page = 1,pageNum = 1,key,i,j,k,logicNo;
	unsigned char huodao[80] = {0},index = 0,lang;

	char stringLcd[5][30];
	lang = SystemPara.Language;

    ChannelKaojiReadFlash(cabinet);
	for(i = 0;i < 8;i++)
	{
		for(j = 0;j < 10;j++)
		{
			if(stHuodaoKaoji[cabinet - 1].faultTimes[i][j] != 0)
				huodao[index++] = (i + 1) * 10 + ((j == 9) ? 0 : (j + 1));	
		}
	}
	pageNum =  index / 5 + 1;

	while(1)
	{
		if(topFlush)
		{
			topFlush =0;
			LCDClrArea(1,4,238,14);

			if(index == 0)//烤鸡成功
			{
				LCDPrintf(8, 1,0,lang," %s拷机次数: %d",ChannelBinStr(cabinet),stHuodaoKaoji[cabinet - 1].times);
				LCDPrintf(8, 5,0,lang," 货道拷机正常");
				LCDPrintf(8, 9,0,lang," 清除记录按 “1”"); 	
			}
			else
			{
				LCDPrintf(8, 1,0,lang,"货道 故障次数");				
				for(k = 0;k < 5;k++)
				{
					logicNo =  huodao[(page - 1) * 5 + k];
					memset(stringLcd[k],' ',30);
					if(logicNo == 0)
					{
					    sprintf((char *)&stringLcd[k][0],"%10.10s"," ");										
					}
					else
					{
						i =  logicNo / 10 - 1;
						j =  (logicNo % 10 == 0) ?  9: (logicNo % 10 - 1);
						sprintf((char *)&stringLcd[k][0],"%2.2d%2.2s%4d%2.2s",logicNo," ",
											stHuodaoKaoji[cabinet - 1].faultTimes[i][j]," ");				
					}
				}
				
				for(k = 0; k < 5;k++)
				{					
					switch(k)
					{
						case 0:
							sprintf((char *)&stringLcd[k][10],"%-12.12s%-4d","拷机次数:",
									stHuodaoKaoji[cabinet - 1].times);
							break;
						case 1:
							sprintf((char *)&stringLcd[k][10],"%-18.18s","清除记录按“1”");
							break;
						case 2:
							sprintf((char *)&stringLcd[k][10],"%-18.18s","              ");
							break;
						case 3:
							if(page > 1 || (page == pageNum && pageNum > 1))
								sprintf((char *)&stringLcd[k][10],"%12.12s","  “上一页”");
							break;
						case 4:
							if(page < pageNum)
								sprintf((char *)&stringLcd[k][10],"%12.12s","  “下一页”");
							break;
						
						default:
							break;
					}				
					LCDPrintf(8,5 + k * 2,0,lang," %s",stringLcd[k]);
				}		
			}			
			
		}
		key = ReadKeyValue();
		OSTimeDly(5);
		switch(key)
		{
			case '1':
				LCDClrArea(1,4,238,14);
				LCDPrintf(8, 1,0,lang," 拷机记录清除页面");
				LCDPrintf(8, 5,0,lang," 确定要清除记录吗?");
				LCDPrintf(8, 7,0,lang," (清除前请记录故障货道)");
				LCDPrintf(8, 11,0,lang,"  确定      取消");
				while(1)
				{
					key = ReadKeyValue();
					OSTimeDly(5);	
					if(key == 'E')
					{
						ChannelKaojiClear(cabinet);
						LCDClrArea(1,4,238,14);
						LCDPrintf(8, 11,0,lang," 成功清除拷机记录!");
						OSTimeDly(200);
						return;
					}
					else if(key == 'C')
						break;
				}
				topFlush =1;
				break;
			case '>':
				page = (page == pageNum) ? page : page + 1;
				topFlush =1;
				break;
			case '<':
				page = (page == 1) ? 1 : page - 1;
				topFlush =1;
				break;
			case 'C':
				returnFlag = 1;
				break;
			default:
				break;

		}

		if(returnFlag == 1)
			return;
	

	}					
}



/*****************************************************************************
** Function name:	ChannelKaojiNotEmpty	
** Descriptions:	有烤鸡记录											 			
** parameters:		无				
** Returned value:	1 有  0无
*******************************************************************************/
unsigned short ChannelKaojiNotEmpty(unsigned char cabinet)
{
	ChannelKaojiReadFlash(cabinet);
	return stHuodaoKaoji[cabinet - 1].times;				
}

/*****************************************************************************
** Function name:	ChannelKaojiAddLog	
** Descriptions:	添加烤鸡记录											 			
** parameters:		cabinet 柜号   logicNo:逻辑货到号 99表示增加烤鸡次数				
** Returned value:	1 有  0无
*******************************************************************************/
void ChannelKaojiAddLog(unsigned char cabinet,unsigned char logicNo)
{
	unsigned char level,column;
	if(cabinet == 0 || cabinet > 2 || logicNo == 0 )
		return;
	if(logicNo == 99)
		stHuodaoKaoji[cabinet - 1].times++;
	else
	{
		level = logicNo / 10 - 1;
		column = (logicNo % 10 == 0) ? 9 : logicNo % 10 - 1;
		stHuodaoKaoji[cabinet - 1].faultTimes[level][column]++;		
	}	
	ChannelKaojiWriteFlash(cabinet);		
						
}


/*****************************************************************************
** Function name:	ChannelKaojiClear	
** Descriptions:	清除烤鸡记录										 			
** parameters:		无				
** Returned value:
*******************************************************************************/
void ChannelKaojiClear(unsigned char cabinet)
{
	memset((void *)&stHuodaoKaoji[cabinet - 1],0,sizeof(stHuodaoKaoji[cabinet - 1]));
	ChannelKaojiWriteFlash(cabinet);				
}





/*****************************************************************************
** Function name:	ChannelOneTestPage	
** Descriptions:	单货道测试页面	changed by yoc													 			
** parameters:		cabinetNo-柜号 	logicNo 逻辑货道号 titleType 测试方式 			
** Returned value:	1 正常 0故障  0xFF 货道不存在
*******************************************************************************/
unsigned char ChannelOneTestPage(unsigned char cabinetNo,unsigned char logicNo,unsigned char titleType)
{
	unsigned char physicNo,rst,lang;
	char *strTestTitle;
	lang = SystemPara.Language;

	if(titleType == 99)//全货道测试
		strTestTitle = ui_aisle_admin.column99Test[lang];
	else if(titleType == 98)//
		strTestTitle = ui_aisle_admin.column98Test[lang];
	else if(titleType == 97)//
		strTestTitle = ui_aisle_admin.column97Test[lang];
	else 
		strTestTitle = ui_aisle_admin.columnOneTest[lang];

	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,lang," %s%s",ChannelBinStr(cabinetNo),strTestTitle);	

	
	physicNo = ChannelGetPhysicNum(logicNo,cabinetNo);
	if(physicNo == 0xFF)
	{
		LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.noColumn[lang],logicNo);//显示“无此货到”
		OSTimeDly(150);
		return 0xFF;
	}	
	
	//显示“请稍等...”
	LCDPrintf(8,5,0,lang,ui_aisle_admin.columnCheckWait[SystemPara.Language],logicNo);
	rst = ChannelAPIHandle(physicNo,CHANNEL_OUTGOODS,SystemPara.GOCIsOpen,cabinetNo);
	switch(rst)
	{
		case 1://显示“货道故障”
			LCDPrintf(8,9,0,lang,ui_aisle_admin.columnTestBad[lang],logicNo);
			break;
		case 3://显示“电机未到位”	
			LCDPrintf(8,9,0,lang,ui_aisle_admin.columnTestMOTO[lang],logicNo);
			break;
		case 4://显示“出货检测未检测到货物”
			LCDPrintf(8,9,0,lang,ui_aisle_admin.columnTestEmpty[lang],logicNo);
			break;
		case 5://显示“通信故障”
			LCDPrintf(8,9,0,lang,ui_aisle_admin.columnCommunFail[lang],logicNo);
			break;
		case 0://显示“货道正常”
			LCDPrintf(8,9,0,lang,ui_aisle_admin.columnTestOK[lang],logicNo);
			break;
		default:
			break;
	}
	if(rst == 0 || rst == 4)
	{
		if(hd_get_by_logic(cabinetNo,logicNo,HUODAO_TYPE_STATE) != HUODAO_STATE_EMPTY)
			ChannelSetParam(logicNo,cabinetNo,CHANNELSTATE,1,0);
	}	
	else
	{
		ChannelSetParam(logicNo,cabinetNo,CHANNELSTATE,2,0);
	}				
	OSTimeDly(150);
	return (rst == 0 || rst == 4);
	
}
			
	
/*****************************************************************************
** Function name:	ChannelSomeTest 
** Descriptions:	部分货道测试	changed by yoc																
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelSomeTest(unsigned char cabinetNo,unsigned char *columnBuf,
							unsigned char columnNum,unsigned char titleType)
{
	unsigned char logicNo = 0,rst,i = 0;
	
	while(1)
	{	
		logicNo = columnBuf[i]; 
		if(i == 0)
			ChannelKaojiAddLog(cabinetNo,99);
		rst = ChannelOneTestPage(cabinetNo,logicNo,titleType);
		if(rst == 0)
			ChannelKaojiAddLog(cabinetNo,logicNo);
		i = (i < (columnNum - 1)) ? i + 1: 0;
		
		if(ReadKeyValue() == 'C')
		{
			return;
		}	
	}	
}


/*****************************************************************************
** Function name:	ChannelSomeColumnTest	
** Descriptions:		部分货道测试	changed by yoc													 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelSomeColumnTest(const unsigned char cabinetNO,unsigned char titleType)
{
	
	unsigned char topFlush = 1,returnFlag = 0,keyFlush = 1;
	unsigned char key,i,j;
	unsigned char columnBuf[40] = {0},columnNum = 0,logicNo = 0,index;
	char str[30] = {' '};
	unsigned char lang = 0;
	while(1)
	{
		if(topFlush || keyFlush)
		{	
			if(topFlush)
			{
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,lang," %s%s",ChannelBinStr(cabinetNO),ui_aisle_admin.column97Test[lang]);
			}
			topFlush = 0;
			keyFlush = 0;
			index = 0;
			for(j = 0;j < 5;j++)
			{
				memset(str,' ',30);
				for(i = 0;i < 8 && index < columnNum;i++)
				{
					sprintf((char *)&str[i * 3],"%2.2d ",columnBuf[index++]);
				}
				if(i < 8 && logicNo && index == columnNum)
				{
					sprintf((char *)&str[i * 3],"%1.1d",logicNo);
					index++;
				}
				LCDPrintf(8,5 + j * 2,0,lang," %s",str);
			}
			
			
		}
		
		key = ReadKeyValue();
		switch(key)
		{
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':case '9':case '0':
				logicNo = logicNo * 10 + key - '0';
				if(logicNo >= 10)
				{					
					if(columnNum <  40 && hd_get_by_logic(cabinetNO,logicNo,HUODAO_TYPE_OPEN))
					{
						columnBuf[columnNum++] = logicNo;
					}
					logicNo = 0;
				}
				keyFlush = 1;
				break;
			case 'E':
				if(columnNum > 0)
					ChannelSomeTest(cabinetNO,columnBuf,columnNum,titleType);
				topFlush = 1;
				break;
			case 'C':
				if(logicNo)
					logicNo = 0;
				else if(columnNum)
					columnNum--;
				else
					returnFlag = 1;
				keyFlush = 1;
				break;
			case 'D':
				logicNo = 0;
				columnNum = 0;
				keyFlush = 1;
				break;
		}


		OSTimeDly(5);
		if(returnFlag)
			return;
		
		
	}

}



/*****************************************************************************
** Function name:	ChannelSomeColumnTest	
** Descriptions:		部分层号测试	changed by yoc													 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelSomeRowTest(const unsigned char cabinetNo,unsigned char titleType)
{
	unsigned char topFlush = 1,returnFlag = 0;
	unsigned char rowNum = 0,key,i,j;
	unsigned char rowBuf[8] = {0},columnBuf[80] = {0},columnNum = 0,logicNo = 0;
	char str[30] = {' '};
	unsigned char lang = 0;
	while(1)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s%s",ChannelBinStr(cabinetNo),ui_aisle_admin.column98Test[lang]);
			memset(str,' ',30);		
			for(i = 0;i < rowNum && i < 4;i++)
			{
				sprintf(&str[i * 4],"%1.1d层 ",rowBuf[i]);
			}
			LCDPrintf(8,5,0,lang," %s",str);

			if(rowNum > 4)
			{
				memset(str,' ',30);	
				for(i = 0;i < (rowNum - 4) && i < 4;i++)
				{
					sprintf(&str[i * 4],"%1.1d层 ",rowBuf[i + 4]);
				}
				LCDPrintf(8,7,0,lang," %s",str);
			}
			
		}
		key = ReadKeyValue();
		switch(key)
		{
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
				for(i = 0;i < rowNum;i++)
				{
					if(rowBuf[i] == (key - '0'))
						break;
				}
				if(i >= rowNum && rowNum < 8 && hd_get_by_logic(cabinetNo,(key - '0') * 10 + 1,HUODAO_TYPE_OPEN))
				{
					rowBuf[rowNum++] = key - '0';
					topFlush = 1;
				}
				break;
			case 'E':
				columnNum = 0;
				for(i = 0;i < rowNum;i++)
				{
					for(j = 0;j < 10;j++)
					{
						logicNo = (rowBuf[i])*10 + ((j == 9) ? 0 : (j + 1));
						if(hd_get_by_logic(cabinetNo,logicNo,HUODAO_TYPE_OPEN))
							columnBuf[columnNum++] = logicNo; 
					}	
				}
				ChannelSomeTest(cabinetNo,columnBuf,columnNum,titleType);
				topFlush = 1;
				break;
			case 'C':
				if(rowNum)
					rowNum--;
				else
					returnFlag = 1;
				topFlush = 1;
				break;
			case 'D':
				rowNum = 0;
				topFlush = 1;
				break;
			default:
				break;
		}


		OSTimeDly(5);
		if(returnFlag)
			return;
		
		
	}
}

/*****************************************************************************
** Function name:	ChannelAllColumnTest	
** Descriptions:	全货道测试页面												 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelAllColumnTest(const unsigned char cabinetNo,unsigned char titleType)
{
	unsigned char columnBuf[80],max,i = 0,logicNo,rst,binIndex = 0;
	while(1)
	{	
		if(i == 0)//开测
		{	
			if(SystemPara.SubBinOpen == 0)
			{
				if(binIndex == 0)
				{
					binIndex = cabinetNo;
					max = ChannelGetAllOpenColumn(binIndex,columnBuf);
				}
			}
			else//拖副柜
			{
				binIndex = (binIndex == 0) ? cabinetNo : ((binIndex == 1) ? 2 : 1);//切柜子
				max = ChannelGetAllOpenColumn(binIndex,columnBuf);	
			}			
			ChannelKaojiAddLog(binIndex,99);
		}
		logicNo = columnBuf[i];				
		rst = ChannelOneTestPage(binIndex,logicNo,99);
		if(rst == 0)
			ChannelKaojiAddLog(binIndex,logicNo);
		
		i = (i < (max - 1)) ? i + 1: 0;
		if(ReadKeyValue() == 'C')
		{
			break;
		}
	}
	
}


/*****************************************************************************
** Function name:	Channel_Test	
** Descriptions:	货道测试	changed by yoc													 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void ChannelTest(unsigned char cabinetNo)
{
	unsigned char key,topFlush = 1,topReturn = 0,enterSub = 0;
	unsigned char logicNo = 0,lang;
	lang = SystemPara.Language;
	
	while(1)
	{
		if(topFlush)
		{
			topFlush = 0;	
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s%s",ChannelBinStr(cabinetNo),ui_aisle_admin.columnTest[lang]);
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enterColumNo[lang]);			
		}

		key = ReadKeyValue();
		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
				logicNo = logicNo * 10 + key - '0';
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],logicNo);	
				if(logicNo >= 10)
					enterSub = 1;
				break;
			case 'C':
				if(logicNo)
				{
					topFlush = 1;
					logicNo = 0;
				}
				else
					topReturn = 1;
				break;
			case 'E':
				enterSub = 1;
				break;
			default:
				break;
		}

		if(topReturn)
			return;

		//进入子菜单
		if(enterSub)
		{
			enterSub = 0;	
			if(logicNo == 99)//全货道测试
			{	
				ChannelAllColumnTest(cabinetNo,99);								
			}
			else if(logicNo == 98)//单层货道测试
			{
				ChannelSomeRowTest(cabinetNo,98);
			}
			else if(logicNo == 97)//部分货道测试
			{
				ChannelSomeColumnTest(cabinetNo,97);
			}
			else //单货道测试
			{
				ChannelOneTestPage(cabinetNo,logicNo,0);
			}

			logicNo = 0;
			topFlush = 1;
		}
			
	}	
}





/*****************************************************************************
** Function name:	nPrintf	
** Descriptions:	将整型数据添加到一个数组中打印															 			
** parameters:		dispnum——需要打印出来的整型数据
					buf——字符串数组					
** Returned value:	无
*******************************************************************************/
void nPrintf(uint32_t dispnum,char *buf)
{  
	char strnum[10];
	
	sprintf(strnum,"%d",dispnum);
	strcpy(buf,strnum);
}


/*****************************************************************************
** Function name:	ChannelReadChannelParam	
** Descriptions:	维护模式下查看货道参数														 			
** parameters:		Bin_箱柜编号；logicnum——货道逻辑编号				
** Returned value:	0——完成
*******************************************************************************/
unsigned char ChannelReadChannelParam(unsigned char Bin,unsigned char logicnum)
{
	ST_LEVEL_HUODAO *stHUodaoPtr;
	unsigned char i,j,flag,index;
	unsigned char key,topFlush = 1,topReturnFlag = 0,enterEdit = 0,editFlush = 1,editChanged = 0;
	unsigned int  tempVlaue = 0;
	unsigned char huodao[80] = {0},max,curIndex,lang;
	lang = SystemPara.Language;
	if(Bin==1)
	{
		stHUodaoPtr = stHuodao;
	}
	else if(Bin==2 && SystemPara.SubBinOpen)
	{
		stHUodaoPtr = stSubHuodao;
	}
	else
	{
		TraceChannel("ReadChannel cabinetNo is err %d\r\n",Bin);
		return 0;
	}
	
	flag = hd_get_index(Bin,logicnum,1,&i,&j);
	if(!flag)
	{
		return 0;
	}
	
	//好了进入正常货道
	max = ChannelGetAllOpenColumn(Bin,huodao);
	for(index = 0;index < max;index++)
	{
		if(huodao[index] == logicnum)
		{
			curIndex = index;
			break;
		}	
	}
	
	topFlush = 1;
	while(1)
	{

		
		if(topFlush)//刷新页面
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],logicnum);
			LCDPrintf(8,5,0,lang," %s%s%2d.%02d",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnPrice[lang],stHUodaoPtr[i].huodao[j].price/100,stHUodaoPtr[i].huodao[j].price%100);
			LCDPrintf(8,7,0,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.columnCount[lang],stHUodaoPtr[i].huodao[j].count);
			LCDPrintf(8,9,0,lang," %s%s%d",ui_aisle_admin.no_3[lang],ui_aisle_admin.columnStatus[lang],stHUodaoPtr[i].huodao[j].state);
			//优先显示选货按键号
			if((SystemPara.PcEnable != 0)&&(SystemPara.UserSelectKeyBoard==1))
			{
				LCDPrintf(8,11,0,lang," %s%s%d",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnSelectNo[lang],stHUodaoPtr[i].huodao[j].selectNo);
				LCDPrintf(8,13,0,lang," %s%s%d",ui_aisle_admin.no_5[lang],ui_aisle_admin.columnID[lang],stHUodaoPtr[i].huodao[j].id);	
			}
			else if(SystemPara.PcEnable)
			{
				LCDPrintf(8,11,0,lang," %s%s%d",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnID[lang],stHUodaoPtr[i].huodao[j].id);	
		
			}
			else if(SystemPara.UserSelectKeyBoard==1)
			{
				LCDPrintf(8,11,0,lang," %s%s%d",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnSelectNo[lang],stHUodaoPtr[i].huodao[j].selectNo);
			}
		}


		key = ReadKeyValue();
		switch(key)
		{
			case '1'://单价
				enterEdit = 1;
				break;
			case '2'://剩余量
				enterEdit = 2;
				break;
			case '4'://ID /selectNo
				if(SystemPara.UserSelectKeyBoard==1)
					enterEdit = 4; //selectNo
				else if(SystemPara.PcEnable)
					enterEdit = 5;//ID
				else
					enterEdit = 0;
				break;
			case '5':
				if(SystemPara.PcEnable && SystemPara.UserSelectKeyBoard)
					enterEdit = 6;
				else
					enterEdit = 0;
				break;

			case 'C':
				if(enterEdit)
				{
					enterEdit = 0;
					tempVlaue = 0;
					topFlush = 1;
				}
				else
					topReturnFlag = 1;
				break;
			case 'E':
				break;

			case '>':
				curIndex = (curIndex < (max - 1)) ? ++curIndex : 0;
				logicnum = huodao[curIndex];
				if(hd_get_index(Bin,logicnum,1,&i,&j))
					topFlush = 1;
				break;
			case '<':
				curIndex = (curIndex) ? --curIndex : max - 1;
				logicnum = huodao[curIndex]	;
				if(hd_get_index(Bin,logicnum,1,&i,&j))
					topFlush = 1;
				break;
			
			default:
				break;
				
		}

		if(topReturnFlag)
			return 1;

		
		//进入编辑菜单
		editFlush = 1;
		while(enterEdit)
		{
			if(editFlush)
			{
				editFlush = 0;
				switch(enterEdit)
				{
					case 1:
						if(editChanged)
							stHUodaoPtr[i].huodao[j].price= tempVlaue;	
						else
							LCDPrintf(8,5,1,lang," %s%s%2d.%02d",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnPrice[lang],tempVlaue/100,tempVlaue%100);
					
						break;
					case 2:
						if(editChanged)
						{
							stHUodaoPtr[i].huodao[j].count= tempVlaue;
							
							if(tempVlaue && stHUodaoPtr[i].huodao[j].state == HUODAO_STATE_EMPTY)
								stHUodaoPtr[i].huodao[j].state = HUODAO_STATE_NORMAL;
							else if((tempVlaue == 0) && stHUodaoPtr[i].huodao[j].state == HUODAO_STATE_NORMAL)
								stHUodaoPtr[i].huodao[j].state = HUODAO_STATE_EMPTY;
						}
							
						else
							LCDPrintf(8,7,1,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.columnCount[lang],tempVlaue);
						break;
					case 4:
						if(editChanged)
							stHUodaoPtr[i].huodao[j].selectNo= tempVlaue;
						else
							LCDPrintf(8,11,1,lang," %s%s%d",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnSelectNo[lang],tempVlaue);
						break;
					case 5:
						if(editChanged)
							stHUodaoPtr[i].huodao[j].id= tempVlaue;
						else
							LCDPrintf(8,11,1,lang," %s%s%d",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnID[lang],tempVlaue);	
						break;
					case 6:
						if(editChanged)
							stHUodaoPtr[i].huodao[j].id= tempVlaue;
						else
							LCDPrintf(8,13,1,lang," %s%s%d",ui_aisle_admin.no_5[lang],ui_aisle_admin.columnID[lang],tempVlaue);	
						break;
				}

				if(editChanged)
				{
					enterEdit = 0;
					editChanged = 0;
					topFlush = 1;
					tempVlaue = 0;
				}
				
			}
			//如果是选货按键号 则应该响应拍拍和键盘
			key = GetSelectKeyAPI();
			if(enterEdit == 4 && key)//选货按键
			{
				tempVlaue = key;
				editFlush = 1;
			}
			else
			{
				key = ReadKeyValue();//读取小键盘数
				switch(key)
				{
					case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
						tempVlaue = tempVlaue * 10 + key - '0';
						editFlush = 1;
						break;
					case 'C':
						if(tempVlaue)
						{
							tempVlaue = 0;
							editFlush = 1;
						}
						else
						{
							enterEdit = 0;
							topFlush = 1;
						}
							
						break;
					case 'E'://确定编辑
						editChanged = 1;
						editFlush = 1;
						break;
					default:
						break;
						
				}
				
			}
			
		}	
		
	}

	
}

/*****************************************************************************
** Function name:	ChannelParamSet	
**
** Descriptions:	维护模式下货道交易参数设置	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelParamSet(unsigned char cabinetNo)
{
	uint8_t key;
	unsigned char logicNo = 0;
	unsigned char topReturnFlag = 0,topFlush = 1,enterFlag = 0;
	unsigned char lang = SystemPara.Language;
	char *binNoStr;
	if(cabinetNo == 1)
		binNoStr = ui_aisle_admin.A_Bin[lang];
	else if(cabinetNo == 2)
		binNoStr = ui_aisle_admin.B_Bin[lang];
	else
		return;
	
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
			LCDPrintf(8,1,0,lang," %s%s",binNoStr,ui_aisle_admin.columnUserManage[lang]);	
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enterColumNo[lang]);
		}
		key = ReadKeyValue();
		
		OSTimeDly(5);
		if(topReturnFlag)
		{
			return;
		}

		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
				logicNo = logicNo * 10 + key - '0';
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],logicNo);
				if(logicNo>=10)
					enterFlag = 1;	
				break;	
			case 'C':
				if(logicNo)
				{
					logicNo = 0;
					topFlush = 1;
				}	
				else
					topReturnFlag = 1;
				break;
			case 'E':
				enterFlag = 1;
				break;
				
		}

		if(enterFlag)
		{
			enterFlag = 0;
			if(!ChannelReadChannelParam(cabinetNo,logicNo))
			{
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,7,0,lang," %s",ui_aisle_admin.noColumn[lang]);
				OSTimeDly(200);
			}	
			logicNo = 0;
			topFlush = 1;
		}
		
	}
}

/*****************************************************************************
** Function name:	ChannelLookUpSuccessCount	
** Descriptions:	查看成功交易次数														 			
** parameters:		无					
** Returned value:	无
*******************************************************************************/
void ChannelLookUpSuccessCount()
{
	uint8_t key;
	uint8_t topFlush = 1,returnFlag = 0;
	char *binNoStr;
	unsigned char lang,cabinetNO = 1,logicNo = 0,enterSub = 0,subFlush = 1;
	unsigned int success = 0;
	lang = SystemPara.Language;
	binNoStr = ui_aisle_admin.A_Bin[lang];
	while(SystemPara.SubBinOpen || SystemPara.hefangGui)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.columnUserManage[lang]);
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
			binNoStr = ui_aisle_admin.A_Bin[lang];
			cabinetNO = 1;
			break;
		}	
		else  if(key == '2')
		{
			if(SystemPara.SubBinOpen)
			{
				cabinetNO = 2;
				binNoStr = ui_aisle_admin.B_Bin[lang];
				break;
			}
			else if(SystemPara.hefangGui)
				hefanMannage();
			topFlush = 1;	
			
		}
		else if(key == '3')
		{
			if(SystemPara.hefangGui)
				hefanMannage();
			topFlush = 1;	
		}
		else if(key == 'C')
			return;



		
	}
	  binNoStr = binNoStr;
	topFlush = 1;
	while(1)
	{
		if(topFlush)
		{
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.columnSuc[lang]);
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enterColumNo[lang]);
			
			topFlush = 0;
			logicNo = 0;
		}
		key = GetKeyInValue();
		switch(key)
		{
			case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
				logicNo = logicNo * 10 + key;
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],logicNo);
				if(logicNo > 10)
				{
					enterSub = 1;
					subFlush = 1;
				}				
				break;
			case 0x0e:
				if(logicNo)
				{
					logicNo = 0;
					topFlush = 1;
				}
				else
					returnFlag = 1;
				break;
			case 0x0f:
				enterSub = 1;
				subFlush = 1;
				break;
		}
		OSTimeDly(10);
		if(returnFlag)
			return;
		
		while(enterSub)
		{
			key = ReadKeyValue();
			if(key == 'C')
			{
				enterSub = 0;
				topFlush = 1;
				logicNo = 0;
				break;
			}

			if(subFlush)
			{
				subFlush = 0;
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,lang," %d %s",logicNo,ui_aisle_admin.columnSuc[lang]);
				success = hd_get_by_logic(cabinetNO,logicNo,HUODAO_TYPE_SUC_COUNT);
				//Trace("cabint = %d,logic= %d suc =%d\r\n",cabinetNO,logicNo,success);
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.columnSuc[lang],success);
			}
		}
		
	}	


	
}

/*****************************************************************************
** Function name:	ChannelClearSuccesCount	
** Descriptions:	清楚成功交易次数														 			
** parameters:		无					
** Returned value:	无
*******************************************************************************/
void ChannelClearSuccesCount()
{
	uint8_t i,j;

	for(i=0;i<HUODAO_MAX_ROW;i++)
	{
		for(j=0;j<HUODAO_MAX_COLUMN;j++)
			stHuodao[i].huodao[j].sucCount = 0;
	}
	if(SystemPara.SubBinOpen==1)
	{
		for(i=0;i<8;i++)
		{
			for(j=0;j<10;j++)
				stSubHuodao[i].huodao[j].sucCount = 0;
		}
	}
	ChannelSaveParam();
}

/*****************************************************************************
** Function name:	ChannelOneAddGoods	
**
** Descriptions:	给指定货道添货	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelOneAddGoods(unsigned char Binnum)
{
	uint8_t key,logicNo = 0,j;	
	uint8_t state=0;
	unsigned char flushFlag = 1,returnFlag = 0,lang,enterSub = 0;
	lang = SystemPara.Language;
	while(1)
	{
		if(flushFlag)
		{
			flushFlag = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.enterColumNo[lang]);
			LCDPrintf(8,5,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.yes[lang]);
			LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.cancel[lang]);
		}
		
		key = ReadKeyValue();
		switch(key)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
				logicNo = logicNo * 10 + key - '0';				
				LCDPrintf(8,1,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],logicNo);				
				if(logicNo >= 10)
					enterSub = 1;
				break;
			case 'E':
				enterSub = 1;
				break;
			case 'C':
				if(logicNo)
				{
					logicNo = 0;
					flushFlag = 1;
				}
				else
					returnFlag = 1;
				break;
			default:
				break;
				

		}

		if(enterSub)
		{
			if(nt_huodao_exsit(Binnum,logicNo))
			{
				LCDClrArea(1,4,238,14);
				j = ChannelGetParamValue(logicNo,4,Binnum);
				if(SystemPara.PcEnable==0)
				{
					ChannelSetParam(logicNo,Binnum,CHANNELCOUNT,j,0);
					state = ChannelGetParamValue(logicNo,3,Binnum);
					if(state == 3 || state == 1)
						ChannelSetParam(logicNo,Binnum,CHANNELSTATE,1,0);
				}
				else
				{
					if(AdminRPTWeihu(2,logicNo,j))
					{
						ChannelSetParam(logicNo,Binnum,CHANNELCOUNT,j,0);
						state = ChannelGetParamValue(logicNo,3,Binnum);
						if(state == 3 || state == 1)
							ChannelSetParam(logicNo,Binnum,CHANNELSTATE,1,0);
					}
				}
				OSTimeDly(200);
				
			}
			else //不存在的柜子
			{
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.noColumn[lang]);
				OSTimeDly(200);
				
			}
			flushFlag = 1;
			logicNo = 0;
			enterSub = 0;
				
		}
		
		if(returnFlag)
		{
			return;
		}
		
	}


}


/*****************************************************************************
** Function name:	ChannelLevelAddGoods	
**
** Descriptions:	给某层的货到添货	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelLevelAddGoods(unsigned char Binnum)
{
	uint8_t key,levelNo = 0;
	unsigned char i,j,lang,flush = 1,returnFlag = 0,subFlush = 1,enterSub = 0;
	lang = SystemPara.Language;
	
	while(1)
	{
		if(flush)
		{
			flush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.enterLevelNo[lang]);	
		}
		key = GetKeyInValue();
		switch(key)
		{
			case 0x01:case 0x02:case 0x03:case 0x04:case 0x05:case 0x06:case 0x07:case 0x08:
				levelNo = key;
				LCDPrintf(8,1,0,lang," %s%d",ui_aisle_admin.enterLevelNo[lang],levelNo);	
				enterSub = 1;
				subFlush = 1;
				break;
			case 0x0e:
				if(levelNo)
				{
					levelNo = 0;
					flush = 1;
				}	
				else
					returnFlag = 1;
				break;
			case 0x0f:
				enterSub = 1;
				subFlush = 1;
				break;
		}

		if(returnFlag)
			return;
		
		while(enterSub)
		{
			if(subFlush)
			{
				subFlush = 0;
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.sureAddLevel[lang]);
				LCDPrintf(8,5,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.yes[lang]);
				LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.cancel[lang]);
			}

			key = ReadKeyValue();
			if(key == '1' || key == 'E')
			{
				//if(SystemPara.PcEnable== 0)
				if(1)
				{
					//获取真实的层号
					if(hd_get_index(Binnum,levelNo * 10 + 1,1,&i,&j))
					{
						hd_add_goods(Binnum,i + 1,2);	
					}	
				}
				else
				{
					AdminRPTWeihu(8,levelNo,0); 
				}
				flush = 1;
				enterSub = 0;
				levelNo = 0;
			}
			else if(key == '2' || key == 'C')
			{
				flush = 1;
				enterSub = 0;
			}
		
		}
		
	}	
}


/*****************************************************************************
** Function name:	ChannelAllLevelAddGoods	
**
** Descriptions:	给所有层的货到添货	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelAllLevelAddGoods(unsigned char Binnum)
{
	uint8_t key;
	unsigned char lang = SystemPara.Language;
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.columnAllAdd[lang]);
	LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.yes[lang]);
	LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.cancel[lang]);
	while(1)
	{
		key = GetKeyInValue();
		if(key == 1 || (key == 0x0F))
		{
			//if(SystemPara.PcEnable==0)
			if(1)
			{
				hd_add_goods(Binnum,0,3);
			}
			else
			{
				AdminRPTWeihu(1,0,0);
			}
			break;
		}
		else
		if((key == 2)||(key == 0x0e))
		{
			break;
		}	
	}
}

/*****************************************************************************
** Function name:	ChannelAddGoods	
** Descriptions:	货道添货操作														 			
** parameters:		无				
** Returned value:	无 
*******************************************************************************/
void ChannelAddGoods(unsigned char cabinetNo)
{
	uint8_t key;
	unsigned char topFlush = 1,topReturnFlag = 0;
	unsigned char lang;
	lang = SystemPara.Language;
	while(1)
	{
		if(topFlush)
		{
			topFlush = 0;	
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.columnSelect[lang]);
			LCDPrintf(8,5,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnOneAdd[lang]);
			LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.columnLevelAdd[lang]);
			LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_3[lang],ui_aisle_admin.columnAllAdd[lang]);
			
		}
		
		key = ReadKeyValue();
		switch(key)
		{
			case '1':
				ChannelOneAddGoods(cabinetNo);
				topFlush = 1;
				break;
			case '2':
				ChannelLevelAddGoods(cabinetNo);
				topFlush = 1;
				break;
			case '3':				
				ChannelAllLevelAddGoods(cabinetNo);
				topFlush = 1;
				break;
			case 'C':
				topReturnFlag = 1;
				break;
			default:
				break;
		}

		if(topReturnFlag)
			return;
	}
}
							   
/*****************************************************************************
** Function name:	ChannelManage	
**
** Descriptions:	货道管理	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelManage()
{
	unsigned char key,cabinetNO = 1;
	unsigned char topFlush = 1,topReturnFlag = 0;
	char *binNoStr;
	unsigned char lang,columnType = 1;
	lang = SystemPara.Language;
	binNoStr = ui_aisle_admin.A_Bin[lang];

	cabinetNO = 1;
	columnType = (SystemPara.Channel >= 2) ? 2 : 1; 
	
	while(SystemPara.SubBinOpen || SystemPara.hefangGui)
	{
		if(topFlush)
		{
			topFlush = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.columnUserManage[lang]);
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
			binNoStr = ui_aisle_admin.A_Bin[lang];
			cabinetNO = 1;
			if(SystemPara.Channel >= 2)
				columnType = 2;
			break;
		}	
		else  if(key == '2')
		{
			if(SystemPara.SubBinOpen)
			{
				cabinetNO = 2;
				binNoStr = ui_aisle_admin.B_Bin[lang];
				if(SystemPara.SubChannel >= 2)
					columnType = 2;
				break;
			}
			else if(SystemPara.hefangGui)
				hefanMannage();
			topFlush = 1;	
			
		}
		else if(key == '3')
		{
			if(SystemPara.hefangGui)
				hefanMannage();
			topFlush = 1;	
		}
		else if(key == 'C')
			return;
	}
	
	
	topFlush = 1;
	while(1)
	{
		if(topFlush)
		{
			topFlush =0;
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s%s",binNoStr,ui_aisle_admin.columnUserManage[lang]);
			LCDPrintf(8,5,0,lang," %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnAtrib[lang]);
			LCDPrintf(8,7,0,lang," %s%s",ui_aisle_admin.no_2[lang],ui_aisle_admin.columnAdd[lang]);
			LCDPrintf(8,9,0,lang," %s%s",ui_aisle_admin.no_3[lang],ui_aisle_admin.columnLook[lang]);
			LCDPrintf(8,11,0,lang," %s%s",ui_aisle_admin.no_4[lang],ui_aisle_admin.columnTest[lang]);
			if(ChannelKaojiNotEmpty(cabinetNO))
				LCDPrintf(8,13,0,lang," %s%s",ui_aisle_admin.no_5[lang],ui_aisle_admin.columnKaoji[lang]);
		}
		
		key = ReadKeyValue();
		switch(key)
		{
			case '1':
				ChannelParamSet(cabinetNO);
				ChannelSaveParam();	
				topFlush = 1;
				break;
			case '2':
				ChannelAddGoods(cabinetNO);
				topFlush = 1;
				break;
			case '3':
				ChannelStateLookUp(cabinetNO);
				topFlush = 1;
				break;
			case '4':
				if(columnType == 2)
					LiftTableTest(cabinetNO);	
				else
					ChannelTest(cabinetNO);
				topFlush = 1;
				break;
			case '5':
				if(ChannelKaojiNotEmpty(cabinetNO))
					ChannelKaojiCheckPage(cabinetNO);
				topFlush = 1;
				break;
			case 'C':
				ChannelSaveParam();
				topReturnFlag = 1;
				break;


				
		}

		if(topReturnFlag)
			return;
		
	}
}


/*****************************************************************************
** Function name:	nt_huodao_rest	
** Descriptions:	货道出厂格式化重置														 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/
void nt_huodao_rest(unsigned char cabinetNo)
{
	uint8_t key=0xff;
	unsigned char lang = SystemPara.Language;
	char *binNoStr;
	if(cabinetNo == 1)
		binNoStr = ui_aisle_admin.A_Bin[lang];
	else if(cabinetNo == 2)
		binNoStr = ui_aisle_admin.B_Bin[lang];
	else
		return;
	
	LCDClrScreen();
	LCDDrawRectangle(0,0,239,15);
	LCDDrawRectangle(0,0,239,3);
	LCDClrArea(1,4,238,14);
	LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.columnInit[lang]);
	LCDPrintf(8,7,0,lang," %s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.yes[lang]);
	LCDPrintf(8,9,0,lang," %s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.cancel[lang]);
	while(1)
	{
		key = GetKeyInValue();
		if(key == 0x01)
		{
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.columnInit[lang]);
			LCDPrintf(8,5,0,lang,"    %s",ui_aisle_admin.columnInitOk[lang]);
			hd_init(cabinetNo);
			ChannelSaveParam();
			OSTimeDly(200);
			break;	
		}
		else if((key == 0x02)||(key == 0x0e))
			break;
	}	
}

/*****************************************************************************
** Function name:	nt_huodao_level_managment	
**
** Descriptions:	货道层属性设置	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void nt_huodao_level_managment(unsigned char cabinetNo)
{
	uint8_t key;
	unsigned char levelNo = 0,returnFlag = 0,flushFlag = 1,enterSubMenu = 0,subFlush = 1
				;
	unsigned char changedFlag = 0,lang;
	char *binNoStr;
	if(cabinetNo == 1)
		binNoStr = ui_aisle_admin.A_Bin[lang];
	else if(cabinetNo == 2)
		binNoStr = ui_aisle_admin.B_Bin[lang];
	else
		return;
	lang = SystemPara.Language;
	while(1)//根菜单
	{
		if(flushFlag)
		{
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.levelManage[lang]);
			LCDPrintf(8,5,0,lang,ui_aisle_admin.enterLevelNo[lang]);
			flushFlag = 0;	
		}
	
	
		key = ReadKeyValue();
		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':
				levelNo = key - '0';
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.enterLevelNo[lang],levelNo);
				if(levelNo && levelNo <= 8)
				{
					enterSubMenu = 1;
					subFlush = 1;
				}
				break;
			case 'E':
				if(levelNo)
				{
					enterSubMenu = 1;
					subFlush = 1;
				}
				break;
			case 'C':
				if(levelNo)
				{
					flushFlag = 1;
					levelNo = 0;
				}	
				else
					returnFlag = 1;
				break;		
		}

		OSTimeDly(5);
		if(returnFlag)
		{
			if(changedFlag)
				ChannelSaveParam();
			return;
		}
			

		//enter subMenu
		while(enterSubMenu)
		{
			if(subFlush)
			{
				subFlush = 0;
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.levelManage[lang]);

				if(nt_get_level_state(cabinetNo,levelNo) == 1)
				{
					LCDPrintf(8,5,0,lang," %d %s %s",levelNo,ui_aisle_admin.levelState[lang],ui_aisle_admin.open[lang]);	
				}
				else
					LCDPrintf(8,5,0,lang," %d %s %s",levelNo,ui_aisle_admin.levelState[lang],ui_aisle_admin.close[lang]);		

				LCDPrintf(8,7,0,lang," %s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.open[lang]);		
				LCDPrintf(8,9,0,lang," %s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);		
				
			}

			key = ReadKeyValue();
			switch(key)
			{
				case '1'://open
					subFlush = 1;
					changedFlag = 1;
					ChannelExChannge(levelNo,2,1,cabinetNo);
					
					break;
				case '2'://close
					subFlush = 1;
					changedFlag = 1;
					ChannelExChannge(levelNo,2,2,cabinetNo);
	
					break;
				case 'C':
					enterSubMenu = 0;
					levelNo = 0;
					flushFlag = 1;
					break;
				default:
					break;
			}

			OSTimeDly(5);
			
		}
			
	}	

}
  
  /*****************************************************************************
  ** Function name:   nt_huodao_set_page 
  **
  ** Descriptions:	  货道属性设置	  页面
  **				  
  **																  
  ** parameters:	  无
					  
  ** Returned value:  无
  ** 
  *******************************************************************************/
  void nt_huodao_set_page(unsigned char cabinetNo)
  {
	 uint8_t key;
	unsigned char flushFlag = 1,returnFlag = 0,huodaoNo = 0,enterSubMenu = 0,subFlush =0;
	unsigned char maxCount = 0,thReturnFlag = 0,changedFlag = 0,lang;

	char *binNoStr;
	if(cabinetNo == 1)
		binNoStr = ui_aisle_admin.A_Bin[lang];
	else if(cabinetNo == 2)
		binNoStr = ui_aisle_admin.B_Bin[lang];
	else
		return;
	lang = SystemPara.Language;

	while(1)
	{
		if(flushFlag)
		{
			flushFlag = 0;
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.columnmanage[lang]);	
			LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enterColumNo[lang]);
				
		}
		key = ReadKeyValue();
		switch(key)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
				huodaoNo = huodaoNo * 10 + key - '0';
				LCDPrintf(8,5,0,lang," %s%d",ui_aisle_admin.enterColumNo[lang],huodaoNo);
				if(huodaoNo >= 10)
				{
					if(nt_huodao_exsit_by_physic(cabinetNo,huodaoNo))
					{
						subFlush = 1;
						enterSubMenu = 1;
					}
					else
					{
						LCDClrArea(1,4,238,14);
						LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.noColumn[SystemPara.Language]);
						OSTimeDly(200);
						huodaoNo = 0;
						subFlush = 1;
						flushFlag = 1;
					}		
				}
				break;

			case 'C':
				if(huodaoNo)
				{
					flushFlag = 1;
					huodaoNo = 0;
				}
				else 
					returnFlag = 1;
				break;
			case 'E':
			
				if(nt_huodao_exsit_by_physic(cabinetNo,huodaoNo))
				{
					subFlush = 1;
					enterSubMenu = 1;
				}
				else
				{
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.noColumn[SystemPara.Language]);
					OSTimeDly(200);
					huodaoNo = 0;
					subFlush = 1;
					flushFlag = 1;
				}	
				break;
				
			default:
				break;
		}

		OSTimeDly(5);
		if(returnFlag)
		{	
			if(changedFlag)
				ChannelSaveParam();
			return;
		}
			
		
		//enter subMenu
		while(enterSubMenu)
		{
			if(subFlush)
			{
				subFlush = 0;
				LCDClrScreen();
				LCDDrawRectangle(0,0,239,15);
				LCDDrawRectangle(0,0,239,3);
				LCDClrArea(1,4,238,14);
				
				LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.columnmanage[lang]);
				if(nt_get_huodoa_ON_OFF(cabinetNo,huodaoNo) == 1)
				  LCDPrintf(8,5,0,lang," %s%s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnStatus[lang],ui_aisle_admin.open[lang]);
				else
				  LCDPrintf(8,5,0,lang," %s%s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnStatus[lang],ui_aisle_admin.close[lang]);
				LCDPrintf(8,7,0,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.maxCount[lang],GetColumnStateByPhysic(cabinetNo,huodaoNo,4));

			}

			key = ReadKeyValue();
			switch(key)
			{
				case '1'://配置开关
					LCDPrintf(8,5,1,lang," %s%s %s%s %s%s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnStatus[lang],ui_aisle_admin.no_1[lang],
						ui_aisle_admin.open[lang],ui_aisle_admin.no_2[lang],ui_aisle_admin.close[lang]);
					while(1)
					{
						key = ReadKeyValue();
						if(key == '1')
						{
							subFlush = 1;
							ChannelExChannge(huodaoNo,1,1,cabinetNo);
							changedFlag = 1;
							break;
						}
						else if(key == '2')
						{
							subFlush = 1;
							ChannelExChannge(huodaoNo,1,2,cabinetNo);
							changedFlag = 1;
							break;
						}
						else if(key == 'C')
						{
							subFlush = 1;
							break;
						}
							
					}
					break;
				case '2'://配置储货量
					maxCount = 0;
					LCDPrintf(8,7,1,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.maxCount[lang],0);
					while(1)
					{
						
						key = ReadKeyValue();
						switch(key)
						{
							case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
								maxCount = maxCount * 10 + key - '0';
								LCDPrintf(8,7,1,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.maxCount[lang],maxCount);
								break;
				
							case 'C':
								
								if(maxCount)
								{
									maxCount = 0;	
									LCDPrintf(8,7,1,lang," %s%s%d",ui_aisle_admin.no_2[lang],ui_aisle_admin.maxCount[lang],GetColumnStateByPhysic(cabinetNo,huodaoNo,4));
								}
								else 
									thReturnFlag= 1;
								break;
							case 'E':
								if(maxCount)
								{	
									SetColumnParamByPhysic(cabinetNo,huodaoNo,4,maxCount);
									thReturnFlag = 1;
									changedFlag = 1;
								}	
								break;
								
							default:
								break;
						}
						if(thReturnFlag)
						{
							subFlush = 1;
							thReturnFlag = 0;
							break;
						}
						
					}
					break;
				case 'C':
					enterSubMenu = 0;
					flushFlag = 1;
					huodaoNo = 0;
					break;
				//新增翻页功能
				case '<':
					subFlush = 1;
						
					break;
				case '>':
					subFlush = 1;
					break;
				default:
					break;
			}
		}
	
	}

	
  }


/*****************************************************************************
** Function name:	ChannelAttributeManage	
** Descriptions:	      货道属性管理	超级管理入口													 			
** parameters:		无					
** Returned value:	无
*******************************************************************************/
void ChannelAttributeManage()
{
	uint8_t key=0xff;
	unsigned char cabinetNo  = 0,flush = 1,returnFlag = 0; 
	unsigned char lang;
	char *binNoStr;
	lang = SystemPara.Language;
	
	if(SystemPara.SubBinOpen)
	{
		LCDClrScreen();
		LCDDrawRectangle(0,0,239,15);
		LCDDrawRectangle(0,0,239,3);
		LCDClrArea(1,4,238,14);
		LCDPrintf(8,1,0,lang," %s",ui_aisle_admin.cabinetAdminMenu[lang]);
		LCDPrintf(8,5,0,lang," %s",ui_aisle_admin.enternCabinetNo[lang]);
		LCDPrintf(8,7,0,lang," %s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.A_Bin[lang]);
		LCDPrintf(8,9,0,lang," %s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.B_Bin[lang]);

		while(1)
		{
			key = ReadKeyValue();
			switch(key)
			{
				case '1':
					cabinetNo = 1;
					binNoStr = ui_aisle_admin.A_Bin[lang];
					break;
				case '2':
					cabinetNo = 2;
					binNoStr = ui_aisle_admin.B_Bin[lang];
					break;
				case 'C':
					return;
				default:
					break;				
			}
			if(cabinetNo)
				break;
		}
	}
	else
		cabinetNo = 1;
	binNoStr = binNoStr;
	while(1)
	{
		
		if(flush)
		{
			flush = 0;	
			LCDClrScreen();
			LCDDrawRectangle(0,0,239,15);
			LCDDrawRectangle(0,0,239,3);
			LCDClrArea(1,4,238,14);
			LCDPrintf(8,1,0,lang," %s %s",binNoStr,ui_aisle_admin.cabinetAdminMenu[lang]);
			LCDPrintf(8,5,0,lang," %s %s",ui_aisle_admin.no_1[lang],ui_aisle_admin.columnInit[lang]);
			LCDPrintf(8,7,0,lang," %s %s",ui_aisle_admin.no_2[lang],ui_aisle_admin.levelManage[lang]);
			LCDPrintf(8,9,0,lang," %s %s",ui_aisle_admin.no_3[lang],ui_aisle_admin.columnmanage[lang]);
		}
		
		key = GetKeyInValue();
		switch(key)
		{
			case 0x01:
				nt_huodao_rest(cabinetNo);
				flush = 1;
				break;
			case 0x02:
				nt_huodao_level_managment(cabinetNo);
				flush = 1;
				break;
			case 0x03:
				nt_huodao_set_page(cabinetNo);
				flush = 1;
				break;
			case 0x0e:
				ChannelSaveParam();	
				returnFlag = 1;
				break;
			default:
				break;
		}

		if(returnFlag)
			return;
	}
}

/*****************************************************************************
** Function name:	ChannelCheckCountOrPrice	
** Descriptions:	检测商品余量是否为0，投入金额是否足够出货														 			
** parameters:		Logicnum:逻辑编号
					Binnum:箱柜号
					Type：操作类型：CHECKCOUNT——查询余量
									CHECKPRICE——检测金额是否足够出货
					price：客户投入的金额
					
** Returned value:	1：可服务  0：不可服务
*******************************************************************************/
unsigned char ChannelCheckCountOrPrice(unsigned char Logicnum,unsigned char Binnum,unsigned char Type,unsigned int price)
{
	uint8_t count=0;
	uint32_t nprice=0;
	//change by liya
	if(Type == CHECKCOUNT)
	{
		count = (uint8_t)ChannelGetParamValue(Logicnum,2,Binnum);
		if(count>0)
			return 1;
		else
			return 0;
	}
	else if(Type == CHECKPRICE)
	{
		price = ChannelGetParamValue(Logicnum,1,Binnum);
		if(nprice > price)
			return 0;
		else
			return 1;	
	}
	return 0;
}


/*****************************************************************************
** Function name:	ChannelGetMaxGoodsPrice	
** Descriptions:	检测最大的商品单价															 			
** parameters:							
** Returned value:	商品单价
*******************************************************************************/
unsigned int ChannelGetMaxGoodsPrice(unsigned char Binnum)
{
	uint8_t i,j;
	uint32_t price=0;
	
	for(i=0;i<8;i++)
	{
		for(j=0;j<10;j++)
		{
			if(Binnum == 1)
			{
				if(stHuodao[i].huodao[j].openFlag && stHuodao[i].huodao[j].state == HUODAO_STATE_NORMAL)
				{
					if(stHuodao[i].huodao[j].price > price)
						price = stHuodao[i].huodao[j].price;
				}
				else
					continue;
			}
			else if(Binnum == 2)
			{
				if(stSubHuodao[i].huodao[j].openFlag && stSubHuodao[i].huodao[j].state == HUODAO_STATE_NORMAL)
				{
					if(stSubHuodao[i].huodao[j].price > price)
						price = stSubHuodao[i].huodao[j].price;
				}
				else
					continue;
			}
		}
	}
	return price;	
	
}


/*****************************************************************************
** Function name:	TestColumnByPC	
**
** Descriptions:	通过PC测试货道	
**					
**														 			
** parameters:		Bin--箱柜号
					LogicNum--逻辑货道号
					
** Returned value:	0:成功;1:故障;2:货道不存在;3:电机未到位;4:出货失败 5:通信故障
** 
*******************************************************************************/
unsigned char TestColumnByPC(unsigned char Bin,unsigned LogicNum)
{
	unsigned char address=0;
	unsigned char i,j,flag=0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(Bin == 1)
		stHuodaoPtr = stHuodao;
	else if(Bin == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(Bin);
		return 0;
	}
	
	flag = hd_get_index(Bin,LogicNum,1,&i,&j);
	if(flag==0)
		return 2;
	address = stHuodaoPtr[i].huodao[j].physicAddr;
	flag = ChannelAPIHandle(address,CHANNEL_OUTGOODS,SystemPara.GOCIsOpen,Bin);
	return flag;
	
}

unsigned char ColumnTestByPcPost(unsigned char Bin,unsigned LogicNum)
{
	unsigned char address=0;
	unsigned char i,j,flag=0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(Bin == 1)
		stHuodaoPtr = stHuodao;
	else if(Bin == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(Bin);
		return 0;
	}
	
	flag = hd_get_index(Bin,LogicNum,1,&i,&j);
	if(flag==0)
		return 2;
	address = stHuodaoPtr[i].huodao[j].physicAddr;
	
	TestColumnPost(CHANNEL_OUTGOODS,SystemPara.GOCIsOpen,address,Bin);

	return 1;

	
}

unsigned char ColumnTestByPcPend()
{
	unsigned char Rst=0;

	Rst = TestColumnPend(SystemPara.GOCIsOpen);
	return Rst;
}



/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 													  
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------
										修改记录
1.日期：2014.08.10 修改人：yoc 													  
  内容：增加拷机结果查询 函数均已ChannelKaoji开头
  			修改函数 ChannelTest 增加单层货道测试
--------------------------------------------------------------------------------*/



