#include "..\config.h"
#include "LIFTTABLE.h"

extern void nPrintf(uint32_t dispnum,char *buf);
unsigned char FailFlag = 0;

typedef struct 
{
	char *LiftTableTest[MAXLANGUAGETYPE];
	char *LiftTableNotHave[MAXLANGUAGETYPE];
	char *LiftTableGetGoods[MAXLANGUAGETYPE];
	char *LiftTableDoorOpen[MAXLANGUAGETYPE];
	char *LiftTableDoorClose[MAXLANGUAGETYPE];
	char *LiftTableOutGoods[MAXLANGUAGETYPE];
	char *LiftTableFMD[MAXLANGUAGETYPE];
	char *LiftTableFUM[MAXLANGUAGETYPE];
	char *LiftTableGAD[MAXLANGUAGETYPE];
	char *LiftTableFLD[MAXLANGUAGETYPE];
	char *LiftTableFOD[MAXLANGUAGETYPE];
	char *LiftTableUDS[MAXLANGUAGETYPE];
	char *LiftTableGCD[MAXLANGUAGETYPE];
	char *LiftTableSOVB[MAXLANGUAGETYPE];
	char *LiftTableSOMD1[MAXLANGUAGETYPE];
	char *LiftTableSOEC[MAXLANGUAGETYPE];
	char *LiftTableSFHG[MAXLANGUAGETYPE];
	char *LiftTableSOFP[MAXLANGUAGETYPE];
	char *LiftTableSOMD2[MAXLANGUAGETYPE];
	char *LiftTableNum[MAXLANGUAGETYPE];
	char *LiftTableFail[MAXLANGUAGETYPE];
	char *LiftTableBin[MAXLANGUAGETYPE];
	char *LiftTableABin[MAXLANGUAGETYPE];
	char *LiftTableBBin[MAXLANGUAGETYPE];
	char *LiftTableGoodsNotTake[MAXLANGUAGETYPE];
	char *LiftTableDataErr[MAXLANGUAGETYPE];
	char *LiftTableNotGoods[MAXLANGUAGETYPE];
	char *LiftTableHang[MAXLANGUAGETYPE];
	char *LiftTableDoorNotOpen[MAXLANGUAGETYPE];
	char *LiftTableError[MAXLANGUAGETYPE];
	char *LiftTableOtherErr[MAXLANGUAGETYPE];
}LOGOLIFTTABLEDISP;

const LOGOLIFTTABLEDISP LogoLiftTable=
{
	{
		" 升降机测试",
		" elevator Test",
		" Lift Table Test",
		" Lift Table Test",
		" texte do elevador",
		" elevator Test"
	},
	{
		" 无%d货道",
		" Column %d N/A!",
		" Column %d N/A!",
		" Column %d N/A!",
		" COLUNA N\x82\O DISPONIVEL!",
		" Colonne %d N/A!"
	},
	{
		" 请取货...",
		" Pls Take Goods",
		" Pls Take Goods",
		" Pls Take Goods",
		" PEGAR PRODUTOS",
		" Pls Take Goods"
	},
	{
		" 取货门:打开",
		" Door: Open",
		" Door: Open",
		" Door: Open",
		" COLETOR DE PORTA COM:ATIVAR",
		" Door: Open"
	},
	{
		" 取货门  关闭",
		" Door: Close",
		" Door: Close",
		" Door: Close",
		" COLETOR DE PORTA COM:DESATIVAR",
		" Door: Close"
	},
	{
		" 正在检测%d货道,请稍等...",
		" Self-testing %d,pls wait...",
		" Self-testing %d,pls wait...",
		" Self-testing %d,pls wait...",
		" %d FAVOR AGUARDAR...",
		" Self-testing %d,pls wait..."
	},
	{
		" 取货电机故障",
		" Tack Goods Motor Error",
		" Tack Goods Motor Error",
		" Tack Goods Motor Error",
		" COLETAR ERRO MOTOR",
		" Defaut sur Moteur d entrainement"
	},
	{
		" 升降台电机故障",
		" Lift Table Motor Error",
		" Lift Table Motor Error",
		" Lift Table Motor Error",
		" MOTOR DO ELEVADOR COM ERRO",
		" Defaut sur Moteur de l ascenseur"
	},
	{
		" 出货确认对射板故障",
		" GOC Error",
		" GOC Error",
		" GOC Error",
		" ERRO DE PLACA GOC",
		" GOC Error"
	},
	{
		" 层架传感器故障",
		" Level Sensor Error",
		" Level Sensor Error",
		" Level Sensor Error",
		" SENSOR DA BANDEIJA COM ERRO",
		" Level Sensor Error"
	},
	{
		" 起始位置传感器故障",
		" Base Point Sensor Error",
		" Base Point Sensor Error",
		" Base Point Sensor Error",
		" SENSOR DE INICIALIZAR COM ERRO",
		" Base Point Sensor Error"
	},
	{
		" 取货门故障",
		" Tack Goods Door Error",
		" Tack Goods Door Error",
		" Tack Goods Door Error",
		" COLETOR DE PORTA COM ERRO",
		" Defaut sur Boite de service"
	},
	{
		" 货道驱动板故障",
		" Column Drive Error",
		" Column Drive Error",
		" Column Drive Error",
		" ERRO DA PLACA DE DISTRIBUI\x83\x82\O",
		" Defaut sur Tableau de bord des colonnes"
	},
	{
		" 出货机构忙",
		" Vending Busy",
		" Vending Busy",
		" Vending Busy",
		" APLICADORA MECH OCUPADO",
		" Vending Busy"
	},
	{
		" 售货机大门没关",
		" Vending Door Is Open",
		" Vending Door Is Open",
		" Vending Door Is Open",
		" SITUA\x83x82\O DAS PORTS DA MAQUINA",
		" Vending Door Is Open"
	},
	{
		" 收到错误数据包",
		" Recve Error Pack",
		" Recve Error Pack",
		" Recve Error Pack",
		" DADOS INCORRETOS RECEBIDOS",
		" Recve Error Pack"
	},
	{
		" 升降台未取到货",
		" Lift Table Not Get Goods",
		" Lift Table Not Get Goods",
		" Lift Table Not Get Goods",
		" ELEVADOR NO COLETA",
		" Lift Table Not Get Goods"
	},
	{
		" 升降机不在原点",
		" Lift Table Not At Base",
		" Lift Table Not At Base",
		" Lift Table Not At Base",
		" ELEVADOR NO PONTO INICIAL",
		" Lift Table Not At Base"
	},
	{
		" 取货门未关闭",
		" Goods Door Is Open",
		" Goods Door Is Open",
		" Goods Door Is Open",
		" PORTA ABERTA",
		" Goods Door Is Open"
	},
	{
		" 请输入货道号:",
		" Input column code:",
		" Input column code:",
		" Input column code:",
		" C\x80\DIGO DA BANDEJA:",
		" Entrer le code colonne:"
	},
	{
		" 出货失败!",
		" Out Goods Fail!",
		" Out Goods Fail!",
		" Out Goods Fail!",
		" FALHA!",
		" Out Goods Fail!"
	},
	{
		"请选择箱柜: ",
		"Please Enter Bin Num:",
		"Please Enter Bin Num:",
		"Please Enter Bin Num:",
		"CODIGO DO ARMARIO:",
		"Please Enter Bin Num:"
	},
	{
		"① A 柜",
		"1.A cabinet",
		"1.A cabinet",
		"1.A cabinet",
		"1.A ARMARIO",
		"1.Cabine A"
	},
	{
		"② B 柜",
		"2.B cabinet",
		"2.B cabinet",
		"2.B cabinet",
		"2.B ARMARIO",
		"1.Cabine B"
	},
	{
		" 货未取走",
		" Goods Not Take",
		" Goods Not Take",
		" Goods Not Take",
		" Goods Not Take",
		" Goods Not Take"
	},
	{
		" 数据错误",
		" Data Error",
		" Data Error",
		" Data Error",
		" Data Error",
		" Data Error"
	},
	{
		" 无货",
		" Empty",
		" Empty",
		" Empty",
		" Empty",
		" vide"
	},
	{
		" 卡货",
		" Goods Hang",
		" Goods Hang",
		" Goods Hang",
		" Goods Hang",
		" Goods Hang"
	},
	{
		" 取货门未开启",
		" Door Not Open",
		" Door Not Open",
		" Door Not Open",
		" Door Not Open",
		" Door Not Open"
	},
	{
		" 升降结构故障",
		" LiftTable Error",
		" LiftTable Error",
		" LiftTable Error",
		" LiftTable Error",
		" LiftTable Error"
	},
	{
		" 其他故障",
		" Other Error",
		" Other Error",
		" Other Error",
		" Other Error",
		" Other Error"
	}
}; 

volatile unsigned int LIFTTABLETIMER;

extern void nPrintf(uint32_t dispnum,char *buf);

unsigned char lift_vend_handle(unsigned char Binnum,unsigned char levelNo,unsigned char columnNo)
{
	 return 0;
}


unsigned char LiftTable1Process(unsigned char Binnum,unsigned char ChannelNum,unsigned char HandleType)
{
	uint8_t Result[36]={0},res=0;
	uint8_t PhysicNum=0xff,level=0;

	PhysicNum = ChannelGetPhysicNum(ChannelNum,Binnum);
	TraceChannel("PhysicNum==%d\r\n",PhysicNum);
	if(PhysicNum == 0x00)
		return 0xff;
	level = ChannelGetLevelNum(Binnum,ChannelNum);
	res = LiftTableHandle(HandleType,PhysicNum,level,Binnum,Result);
	return res;
}


/*****************************************************************************
** Function name:	LiftTable2Process	
** Descriptions:														 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/

unsigned char LiftTable2Process(unsigned char Binnum,const unsigned char ChannelNum,unsigned char HandleType)
{
	uint8_t Result[36]={0},res=0;
	uint8_t PhysicNum=0xff,level=0;
	

	if(!hd_get_by_logic(Binnum,ChannelNum,HUODAO_TYPE_OPEN))//货道不存在
		return 0xFF;
	TraceChannel("\r\nLift start outgoods-huodao:%d\r\n",ChannelNum);
	if(ChannelNum == 0x00)
		return 0xff;
	level = ChannelNum / 10 ;
	PhysicNum = (ChannelNum % 10 == 0) ? 10: ChannelNum % 10;
	res = LiftTableHandle(HandleType,PhysicNum,level,Binnum,Result);
	return res;
}




unsigned char LiftTable3Process(unsigned char Binnum,const unsigned char ChannelNum,unsigned char HandleType)
{
	uint8_t Result[36]={0},res=0;
	uint8_t PhysicNum=0xff,level=0;


	if(!hd_get_by_logic(Binnum,ChannelNum,HUODAO_TYPE_OPEN))//货道不存在
	{
		TraceChannel("LiftTable3Process:cabinet=%d,column=%d is not open!\r\n",Binnum,ChannelNum);
		return 0xFF;
	}
	
	TraceChannel("\r\n\r\nLiftTable3Process[Outgoods] huodao:%d\r\n",ChannelNum);
	
	PhysicNum = hd_physicNo_by_logicNo(Binnum,ChannelNum);
	if(PhysicNum == 0)
	{	
		TraceChannel("LiftTable3Process:cabinet=%d,physicNo =%d is not open!\r\n",Binnum,PhysicNum);
		return 0xFF;
	}
	level = PhysicNum / 10 ;
	PhysicNum = PhysicNum % 10;
	res = LiftTableHandle(HandleType,PhysicNum,level,Binnum,Result);
	return res;
}

unsigned char LiftTableProcess(unsigned char Binnum,unsigned char ChannelNum,unsigned char HandleType)
{
	unsigned char ret = 0;
	liftSetReady(0);
	if((Binnum == 1 && SystemPara.Channel == 2) ||
		(Binnum == 2 && SystemPara.SubChannel == 2))
		ret = LiftTable1Process(Binnum,ChannelNum,HandleType);
	else  if((Binnum == 1 && SystemPara.Channel == 3) ||
		(Binnum == 2 && SystemPara.SubChannel == 3))
		ret = LiftTable2Process(Binnum,ChannelNum,HandleType);
	else
		ret = LiftTable3Process(Binnum,ChannelNum,HandleType);

	liftSetReady(1);

	return ret;
	
}

void LiftTableTest(unsigned char binnum)
{
	uint8_t key=0xff;
	uint8_t rst=0;

	//char mstr[2]={0};

	unsigned char  topFlush = 1,topReturnFlag = 0,enterSub = 0; 
	unsigned char lang,logicNo = 0,max,i,columnBuf[80] = {0};
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
			LCDPrintf(8,1,0,lang,LogoLiftTable.LiftTableTest[lang]);
			LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableNum[lang]);	
			logicNo = 0;
		}
			
		key = ReadKeyValue();
		switch(key)
		{
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':
				logicNo  = logicNo * 10 + key - '0';
				LCDPrintf(8,5,0,lang,"%s%d",LogoLiftTable.LiftTableNum[lang],logicNo);	
				if(logicNo >= 10)
				{
					enterSub = 1;
				}
					
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
				enterSub = 1;
				break;
			default:
				break;
		}
		if(topReturnFlag)
		{
			return;
		}

		if(enterSub)
		{
			enterSub = 0;
			topFlush = 1;
			
			if(logicNo == 99)//全货道测试
			{
				max = ChannelGetAllOpenColumn(binnum,columnBuf);
				i = 0;
				logicNo = columnBuf[0];
				//显示“正在检测XX货道...”
				LCDPrintf(8,1,0,lang,LogoLiftTable.LiftTableTest[lang]);
			}
			else //单货道测试
			{
				//显示“正在检测XX货道...”
				LCDPrintf(8,1,0,lang,LogoLiftTable.LiftTableTest[lang]);
				max = 0;
			}
				
			
			do
			{
				//physicNo = ChannelGetPhysicNum(logicNo,binnum);
				
				if(hd_get_by_logic(binnum,logicNo,HUODAO_TYPE_OPEN))
				{
					
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,1,0,lang,LogoLiftTable.LiftTableTest[lang]);
					LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableOutGoods[lang],logicNo);
					rst = LiftTableProcess(binnum,logicNo,LIFTTABLE_OUTHANDLE);
					LCDClrArea(1,4,238,14);
					LCDPrintf(8,1,0,lang,LogoLiftTable.LiftTableTest[lang]);
					if(rst == 0xff)
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableNotHave[lang]);
					else if(rst == 0x1F){
						TraceChannel("Flow....1\r\n");
						//货道板通讯故障
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGCD[lang]);		
					}	
					else if(rst == 0x11){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOVB[lang]);
					}
					else if(rst == 0x12){//忙
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGCD[lang]);
					}
					// 2：数据错误 3：无货 4：卡货 5：取货门未开启 6：货物未取走 7：未定义错误	0xff：通信失败
					else if(rst == 1){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGetGoods[lang]);
						ChannelSetParam(logicNo,binnum,CHANNELSTATE,1,0);
					}
					else if(rst == 2){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableDataErr[lang]);
					}
					else if(rst == 3){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableNotGoods[lang]);
					}
					else if(rst == 4){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableHang[lang]);
					}
					else if(rst == 5){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableDoorNotOpen[lang]);
				
					}
					else if(rst == 6){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGoodsNotTake[lang]);

					}
					else if(rst == 7){
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOVB[lang]);
					}
					else
					{
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableOtherErr[lang]);
						
					}

						
					if(max && (i < (max - 1)))
						i++;
					else
						i = 0;
					logicNo = columnBuf[i];
					if(ReadKeyValue() == 'C' && max)
					{
						break;
					}
					
					OSTimeDly(150);		
				}
				else
				{
					//显示“无此货到”
					LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableNotHave[lang],logicNo);
					OSTimeDly(150);		
					break;
				}

			}while(max);
									
			
		}
	}
	
}
	

unsigned char LiftTableTestByPcPost(unsigned char Bin,unsigned char LogicNum,unsigned char HandleType)
{
	unsigned char row=0,low=0,flag=0;
	unsigned char address=0,level;
	
	flag = ChannelGetAddressLOWandRow(1,Bin,LogicNum,&row,&low);
	if(flag==0)
		return 2;
	if(Bin==1)
		address = stHuodao[row].huodao[low].physicAddr;
	else
		address = stSubHuodao[row].huodao[low].physicAddr;
	flag = ChannelGetAddressLOWandRow(2,Bin,address,&row,&low);
	if(flag==0)
		return 2;

	level = address /10 + 1;
	address = address % 10;
	TestLiftTablePost(HandleType,address,level,Bin);	
	return 1;
}




//0-故障 1-正常 2—失败	 0xff-邮箱接收失败
unsigned char LiftTableTestByPcPend(unsigned char HandleType)
{
	unsigned char Rst=0,Result[36]={0};

	Rst = TestLiftTablePend(Result);
	//去掉头的偏移量为5 
	if(Rst==1)
	{	
		if(HandleType == LIFTTABLE_OUTHANDLE)
		{
			return 1;		
		}
		else if(HandleType == LIFTTABLE_CHECKHANDLE)
		{
			if(Result[5]==0x00)//整机状态
				return 1;
			else
				return 0;
		}
		else if(HandleType == LIFTTABLE_OUTGOODSRST)
		{
			if(Result[5]==0x00)//出货结束
			{
				if(Result[6] == 0)//出货成功
					return 1;
				else//出货失败
					return (Result[7] + 0x10);
			}		
		}
	}
	return 0xff;
}



