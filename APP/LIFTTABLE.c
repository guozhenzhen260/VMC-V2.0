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
		" texte do elevador"
	},
	{
		" 无%d货道",
		" Column %d N/A!",
		" Column %d N/A!",
		" Column %d N/A!",
		" COLUNA N\x82\O DISPONIVEL!"
	},
	{
		" 请取货...",
		" Pls Take Goods",
		" Pls Take Goods",
		" Pls Take Goods",
		" PEGAR PRODUTOS"
	},
	{
		" 取货门:打开",
		" Door: Open",
		" Door: Open",
		" Door: Open",
		" COLETOR DE PORTA COM:ATIVAR"
	},
	{
		" 取货门  关闭",
		" Door: Close",
		" Door: Close",
		" Door: Close",
		" COLETOR DE PORTA COM:DESATIVAR"
	},
	{
		" 正在检测%d货道,请稍等...",
		" Self-testing %d,pls wait...",
		" Self-testing %d,pls wait...",
		" Self-testing %d,pls wait...",
		" %d FAVOR AGUARDAR..."
	},
	{
		" 取货电机故障",
		" Tack Goods Motor Error",
		" Tack Goods Motor Error",
		" Tack Goods Motor Error",
		" COLETAR ERRO MOTOR"
	},
	{
		" 升降台电机故障",
		" Lift Table Motor Error",
		" Lift Table Motor Error",
		" Lift Table Motor Error",
		" MOTOR DO ELEVADOR COM ERRO"
	},
	{
		" 出货确认对射板故障",
		" GOC Error",
		" GOC Error",
		" GOC Error",
		" ERRO DE PLACA GOC"
	},
	{
		" 层架传感器故障",
		" Level Sensor Error",
		" Level Sensor Error",
		" Level Sensor Error",
		" SENSOR DA BANDEIJA COM ERRO"
	},
	{
		" 起始位置传感器故障",
		" Base Point Sensor Error",
		" Base Point Sensor Error",
		" Base Point Sensor Error",
		" SENSOR DE INICIALIZAR COM ERRO"
	},
	{
		" 取货门故障",
		" Tack Goods Door Error",
		" Tack Goods Door Error",
		" Tack Goods Door Error",
		" COLETOR DE PORTA COM ERRO"
	},
	{
		" 货道驱动板故障",
		" Column Drive Error",
		" Column Drive Error",
		" Column Drive Error",
		" ERRO DA PLACA DE DISTRIBUI\x83\x82\O"
	},
	{
		" 出货机构忙",
		" Vending Busy",
		" Vending Busy",
		" Vending Busy",
		" APLICADORA MECH OCUPADO"
	},
	{
		" 售货机大门没关",
		" Vending Door Is Open",
		" Vending Door Is Open",
		" Vending Door Is Open",
		" SITUA\x83x82\O DAS PORTS DA MAQUINA"
	},
	{
		" 收到错误数据包",
		" Recve Error Pack",
		" Recve Error Pack",
		" Recve Error Pack",
		" DADOS INCORRETOS RECEBIDOS"
	},
	{
		" 升降台未取到货",
		" Lift Table Not Get Goods",
		" Lift Table Not Get Goods",
		" Lift Table Not Get Goods",
		" ELEVADOR NO COLETA"
	},
	{
		" 升降机不在原点",
		" Lift Table Not At Base",
		" Lift Table Not At Base",
		" Lift Table Not At Base",
		" ELEVADOR NO PONTO INICIAL"
	},
	{
		" 取货门未关闭",
		" Goods Door Is Open",
		" Goods Door Is Open",
		" Goods Door Is Open",
		" PORTA ABERTA"
	},
	{
		" 请输入货道号:",
		" Input column code:",
		" Input column code:",
		" Input column code:",
		" C\x80\DIGO DA BANDEJA:"
	},
	{
		" 出货失败!",
		" Out Goods Fail!",
		" Out Goods Fail!",
		" Out Goods Fail!",
		" FALHA!"
	},
	{
		"请选择箱柜: ",
		"Please Enter Bin Num:",
		"Please Enter Bin Num:",
		"Please Enter Bin Num:",
		"CODIGO DO ARMARIO:"
	},
	{
		"① A 柜",
		"1.A cabinet",
		"1.A cabinet",
		"1.A cabinet",
		"1.A ARMARIO"
	},
	{
		"② B 柜",
		"2.B cabinet",
		"2.B cabinet",
		"2.B cabinet",
		"2.B ARMARIO"
	},
	{
		" 货未取走",
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
		" Data Error"
	},
	{
		" 无货",
		" Empty",
		" Empty",
		" Empty",
		" Empty"
	},
	{
		" 卡货",
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
		" Door Not Open"
	},
	{
		" 升降结构故障",
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
	switch(HandleType)
	{
		//出货
		case LIFTTABLE_OUTHANDLE:
			//查询状态是否正常，正常出货
			res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
			TraceChannel("res==%d  rseult1=%d  result2=%d\r\n",res,Result[0],Result[1]);
			if(res==1)
			{
				if((Result[5]==0x00))		
				{
					//进行出货操作
					res = LiftTableHandle(CHANNEL_OUTGOODS,PhysicNum,level,Binnum,Result);
					if(res)
					{
						OSTimeDly(10);
						LIFTTABLETIMER = 3000+level*200;
						while(LIFTTABLETIMER)
						{
							//检测出货结果
							res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
							TraceChannel("res=%d\r\n",res);
							if((Result[5])==0x00)
							{
								FailFlag = res;
								TraceChannel("Out_OVER_Break===%x\r\n",Result[1]);
								break;
							}
							OSTimeDly(100);
						}
						if(LIFTTABLETIMER)
						{
							if((Result[6]!=0)&&(Result[7]!=0))
								return 2;
							else
							if((Result[6]!=0)&&(Result[7]==5))
								return 1;
							else
								return 1;
						}
						else										   	
							return 2;
					}
				}
				else
					return 0;
			}
			break;
		//查询状态
		case LIFTTABLE_CHECKHANDLE:
			res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
			if(res == 1)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case LIFTTABLE_OUTGOODSRST:
			res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_RESET:
			res = LiftTableHandle(MACHINE_RESET,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_LIGHTOPEN:
			res = LiftTableHandle(MACHINE_LIGHTOPEN,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_LIGHTCLOSE:
			res = LiftTableHandle(MACHINE_LIGHTCLOSE,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_OPENDOOR:
			res = LiftTableHandle(MACHINE_OPENDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_CLOSEDOOR:
			res = LiftTableHandle(MACHINE_CLOSEDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_INFORMATION:
			res = LiftTableHandle(MACHINE_INFORMATION,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
	}
	return 3;
}


/*****************************************************************************
** Function name:	LiftTable2Process	
** Descriptions:														 			
** parameters:		无				
** Returned value:	无
*******************************************************************************/

unsigned char LiftTable2Process(unsigned char Binnum,const unsigned char ChannelNum,unsigned char HandleType)
{
	uint8_t Result[36]={0},res=0,rcx = 5;
	uint8_t PhysicNum=0xff,level=0;
	unsigned short takeTimeOut = 10;

	if(!hd_get_by_logic(Binnum,ChannelNum,HUODAO_TYPE_OPEN))//货道不存在
		return 0xFF;
	TraceChannel("\r\nLift start outgoods-huodao:%d\r\n",ChannelNum);
	if(ChannelNum == 0x00)
		return 0xff;
	level = ChannelNum / 10 ;
	PhysicNum = (ChannelNum % 10 == 0) ? 10: ChannelNum % 10;
	switch(HandleType)
	{
		//出货
		case LIFTTABLE_OUTHANDLE:
			//查询状态是否正常，正常出货
			rcx = 10;
			while(rcx--)
			{
				res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
				if(res==1 && (Result[5]==0x00))
					break;
				OSTimeDly(200);
			}
			
			TraceChannel("\r\nres==%d  Result[0]=%d  Result[1]=%d Result[5]=%d\r\n",
				res,Result[0],Result[1],Result[5]);
	
			if(res==1 && (Result[5]==0x00) && (Result[6]==0x00))
			{
					while(1)
					{
							//进行出货操作
							res = LiftTableHandle(CHANNEL_OUTGOODS,PhysicNum,level,Binnum,Result);
							if(res)
							{
								OSTimeDly(10);
								//5000 * 10  = 50000 
								LIFTTABLETIMER = 5000+(8 - level)*400;//检测出货结果超时
								while(LIFTTABLETIMER)
								{
									//检测出货结果
									res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
									TraceChannel("res=%d\r\n",res);
									if(((Result[5])==0x00) && (res != 0xFF))
									{
										FailFlag = res;
										TraceChannel("Out_OVER_Break===%x\r\n",Result[1]);
										break;
									}
									OSTimeDly(200);
								}
								if(LIFTTABLETIMER)
								{
									if((Result[6]!=0)&&(Result[7]!=0))
									{
										TraceChannel("Result[6] = %d,Result[7] = %d\r\n",
											Result[6],Result[7]);
										if(Result[7] == 5 && takeTimeOut)//货未取走
										{
											OSTimeDly(400);
											takeTimeOut--;
											continue;
										}
										return 2;
									}	
									else
										return 1;
								}
								else										   	
									return 2;
								
							}
							else
								return 0;		
						}
					
			}
			else
				return 0;					
			break;
		//查询状态
		case LIFTTABLE_CHECKHANDLE:
			res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
			if(res == 1)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case LIFTTABLE_OUTGOODSRST:
			res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_RESET:
			res = LiftTableHandle(MACHINE_RESET,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_LIGHTOPEN:
			res = LiftTableHandle(MACHINE_LIGHTOPEN,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_LIGHTCLOSE:
			res = LiftTableHandle(MACHINE_LIGHTCLOSE,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_OPENDOOR:
			res = LiftTableHandle(MACHINE_OPENDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_CLOSEDOOR:
			res = LiftTableHandle(MACHINE_CLOSEDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_INFORMATION:
			res = LiftTableHandle(MACHINE_INFORMATION,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
	}
	return 3;
}




unsigned char LiftTable3Process(unsigned char Binnum,const unsigned char ChannelNum,unsigned char HandleType)
{
	uint8_t Result[36]={0},res=0,rcx = 5;
	uint8_t PhysicNum=0xff,level=0;
	unsigned short takeTimeOut = 10;

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
	switch(HandleType)
	{
		case LIFTTABLE_OUTHANDLE://出货
			//查询状态是否正常，正常出货
			rcx = 10;
			while(rcx--)
			{
				TraceChannel("Check the lift remainTimes=%d\r\n",rcx);
				res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
				if(res==1 && (Result[5]==0x00))
					break;
				OSTimeDly(200);
			}
			
			TraceChannel("Check[over]:res==%d  Result[0]=%d  Result[1]=%d Result[5]=%d\r\n",
				res,Result[0],Result[1],Result[5]);
	
			if(res==1 && (Result[5]==0x00) && (Result[6]==0x00))
			{
				while(1)
				{
					//进行出货操作
					TraceChannel("OUTGOODS: level=%d,column=%d\r\n",level,PhysicNum);
					res = LiftTableHandle(CHANNEL_OUTGOODS,PhysicNum,level,Binnum,Result);
					if(res)
					{
						OSTimeDly(10);
						//5000 * 10  = 50000 
						LIFTTABLETIMER = 5000+(8 - level)*400;//检测出货结果超时
						while(LIFTTABLETIMER)
						{
							//检测出货结果
							TraceChannel("CHECKOUTRESULT:\r\n");
							res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
							if(((Result[5])==0x00) && (res != 0xFF))
							{
								FailFlag = res;
								TraceChannel("Out_OVER_Break===%x\r\n",Result[1]);
								break;
							}
							OSTimeDly(200);
						}
						if(LIFTTABLETIMER)
						{
							if((Result[6]!=0)&&(Result[7]!=0))
							{
								TraceChannel("Result[6] = %d,Result[7] = %d\r\n",
									Result[6],Result[7]);
								if(Result[7] == 5 && takeTimeOut)//货未取走
								{
									OSTimeDly(400);
									takeTimeOut--;
									continue;
								}
								return 2;
							}	
							else
								return 1;
						}
						else										   	
							return 2;
						
					}
					else
						return 0;		
				}
					
			}
			else
				return 0;					
			break;
		//查询状态
		case LIFTTABLE_CHECKHANDLE:
			res = LiftTableHandle(CHANNEL_CHECKSTATE,0,0,Binnum,Result);
			if(res == 1)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case LIFTTABLE_OUTGOODSRST:
			res = LiftTableHandle(CHANNEL_CHECKOUTRESULT,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_RESET:
			res = LiftTableHandle(MACHINE_RESET,0,0,Binnum,Result);
			if(res==1)
			{
				return 1;
			}
			else
				return 0;
		case MACHINE_LIGHTOPEN:
			res = LiftTableHandle(MACHINE_LIGHTOPEN,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_LIGHTCLOSE:
			res = LiftTableHandle(MACHINE_LIGHTCLOSE,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_OPENDOOR:
			res = LiftTableHandle(MACHINE_OPENDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_CLOSEDOOR:
			res = LiftTableHandle(MACHINE_CLOSEDOOR,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		case MACHINE_INFORMATION:
			res = LiftTableHandle(MACHINE_INFORMATION,0,0,Binnum,Result);
			if(res)
			{
				if(Result[0]==0x00)
					return 1;
				else
					return 0;
			}
			else
				return 0;
	}
	return 3;
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
	DEVICESTATEPACK *RecvBoxPack;
	uint8_t err;
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
					else if(rst == 3)
					{
						TraceChannel("Flow....1\r\n");
						//货道板通讯故障
						LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGCD[lang]);		
					}		
					else if(rst == 0)
					{
						TraceChannel("Flow....2\r\n");
						//存在故障
						OSMboxPost(g_DeviceStateMail,&RecvBoxPack);
						//等待邮箱返回
						RecvBoxPack = OSMboxPend(g_DeviceStateBackMail,1000,&err);
						if(err == OS_NO_ERR)
						{
							TraceChannel("Flow....3\r\n");
							if(RecvBoxPack->Error_FMD == 1)
							{
								//取货电机故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableFMD[lang]);
							}
							if(RecvBoxPack->Error_FUM == 1)
							{
								//上下电机故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableFUM[lang]);
							}
							if(RecvBoxPack->Error_GOC == 1)
							{
								//出货确认板故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGAD[lang]);
							}
							if(RecvBoxPack->Error_FLD == 1)
							{
								//层架检测器故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableFLD[lang]);
							}
							if(RecvBoxPack->Error_FOD == 1)
							{
								//起始位置传感器
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableFOD[lang]);
							}
							if(RecvBoxPack->Error_UDS == 1)
							{
								//用户取货门故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableUDS[lang]);
							}
							if(RecvBoxPack->Error_GCD == 1)
							{
								//货道驱动板故障
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGCD[lang]);
							}
							if(RecvBoxPack->Error_SOVB == 1)
							{
								//出货机构忙
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOVB[lang]);
							}
							if(RecvBoxPack->Error_SOMD1 == 1)
							{
								//售货机大门没关
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOMD1[lang]);
							}
							if(RecvBoxPack->Error_SOEC == 1)
							{
								//收到错误数据包
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOEC[lang]);
							}
							if(RecvBoxPack->Error_SFHG == 1)
							{
								//升降台正确取货
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSFHG[lang]);
							}
							if(RecvBoxPack->Error_SOFP == 1)
							{
								//升降台没在原点
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOFP[lang]);
							}
							if(RecvBoxPack->Error_SOMD2 == 1)
							{
								//取货门开启
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOMD2[lang]);
							}
						}
					}
					else
					{
						TraceChannel("Flow....4\r\n");
						if(rst == 1)
						{
							TraceChannel("Flow....5\r\n");
							//正常
							LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGetGoods[lang]);
							ChannelSetParam(logicNo,binnum,CHANNELSTATE,1,0);
						}
						else
						if(rst == 2)
						{
							TraceChannel("Flow....6\r\n");
							if(FailFlag == 2)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableDataErr[lang]);
							else
							if(FailFlag == 3)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableNotGoods[lang]);
							else
							if(FailFlag == 4)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableHang[lang]);
							else
							if(FailFlag == 5)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableDoorNotOpen[lang]);
							else
							if(FailFlag == 6)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableGoodsNotTake[lang]);
							else
							if(FailFlag == 7)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableSOVB[lang]);
							else
							if(FailFlag == 8)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableError[lang]);
							else
							if(FailFlag == 9)
								LCDPrintf(8,5,0,lang,LogoLiftTable.LiftTableOtherErr[lang]);
						
							//出货失败
							ChannelSetParam(logicNo,binnum,CHANNELSTATE,1,0);
						}
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



