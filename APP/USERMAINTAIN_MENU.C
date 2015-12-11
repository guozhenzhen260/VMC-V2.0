/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           USERMAINTAIN_MENU.C
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        用户模式主菜单                  
**------------------------------------------------------------------------------------------------------
** Created by:          gzz 
** Created date:        2013-03-04
** Version:              
** Descriptions:        The original version       
********************************************************************************************************/


/**************************************密码输入菜单****************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//密码模式提示
	char *EnterPin[MAXLANGUAGETYPE];				//提示输入密码
	char *PinError[MAXLANGUAGETYPE];				//密码错误提示
	char *ReturnCapion[MAXLANGUAGETYPE];			//返回提示
} PASSWORDNMENULIST;

const PASSWORDNMENULIST PasswordMenuList = 
{
	{
		"输入密码",
		"Input password",
		"Input password",
		"SIFRE GIR",
		"SENHA",
		"Entrer mot de passe"
	},
	{
		"请输入密码：",
		"Pls input password:",
		"Pls input password:",
		"LUTFEN SIFRE GIRIN:",
		"FAVOR INSERIR A SENHA:",
		"Saisir mot de passe:"
	},
	{
		"密码错误！！",
		"Password Error!",
		"Password Error!",
		"Password Error!",
		"Password Error!",
		"Password Error!"
	},
	{
		"请按“取消”返回",
		"Press“Cancel” to return",
		"Press“Cancel” to return",
		"IPTAL E BAS GERI DON",
		"PRESSIONE CANCELAR",
		"Appuyez Cancel pour retourn"
	}
};

/**************************************确认取消页面****************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//确认取消页面提示
	char *SureCapion[MAXLANGUAGETYPE];				//确认取消页面提示
	char *OKPin[MAXLANGUAGETYPE];					//确认按键
	char *CancelPin[MAXLANGUAGETYPE];				//取消按键	
} OKNMENULIST;

const OKNMENULIST OKMenuList = 
{
	{
		"请选择",
		"Select",
		"Select",
		"Select",
		"SELECIONAR",
		"Select"
	},
	{
		"确认吗?",
		"Are you sure?",
		"Are you sure?",
		"EMIN MISINIZ?",
		"VOC\x85 TEM CERTEZA?",
		"Eteq vous sur?"
	},
	{
		"确认",
		"Enter",
		"Enter",
		"GIRIS",
		"PRESSIONE",
		"Entrez"
	},
	{
		"取消",
		"Cancel",
		"Cancel",
		"IPTAL",
		"CANCELAR",
		"Cancel"
	}
};


/*************************************用户模式维护主菜单按键选择************************************************/
typedef struct
{
	unsigned char KeyCurrent;						//当前菜单索引号
	unsigned char ExtSelfCheck;						//退出设备自检菜单并返回维护主菜单
	unsigned char PageNumb;							//菜单页索引
	void (*TestOperate)();							//当前状态应该执行的功能操作
} USERMAINTAINMAINMENU;											//维护主菜单
USERMAINTAINMAINMENU UserMaintainMainMenu;									//维护主菜单结构体

/**************************************用户模式维护主菜单****************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//自检模式提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *Record[MAXLANGUAGETYPE];		
	char *Error[MAXLANGUAGETYPE];
	char *System[MAXLANGUAGETYPE];
	char *Column[MAXLANGUAGETYPE];	
	char *Trace[MAXLANGUAGETYPE];	
} USERMAINTAINMAINMENULIST;

const USERMAINTAINMAINMENULIST UserMaintainMenuList = 
{
	{
		"维护菜单",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"Code de Maintenance"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 交易记录",
		"1.Sales record",
		"1.Запись запроса",
		"1.SATIS KAYITLARI",
		"1.VENDAS",
		"1.Rapport des ventes"
	},
	{
		"② 故障查询",
		"2.Error inqiry",	
		"2.Код ошибки",
		"2.HATALI GIRIS",
		"2.ERRO CONSULTADO",
		"2.Erreur sur requisition"
	},
	{
		"③ 交易设置",
		"3.Purchase setting",
		"3.Система",
		"3.SATIN ALIM AYARI",
		"3.COMPRA",
		"3.Parametre des achats"
	},
	{
		"④ 货道管理",
		"4.Column",	
		"4.Колонка",
		"4.SUTUN",
		"4.BANDEJA",
		"4.Colonne"
	},
	{
		"⑤ 调试模式",
		"5.Debug mode",
		"5.Trace",
		"5.HATA KONTROL",
		"5.MODO DE AJUSTE",
		"5.Mode debogage"
	}
};







/**************************************错误查询菜单****************************************************/
typedef struct
{
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	//纸币器故障提示	
	char *BillCapion[MAXLANGUAGETYPE];				
	char *BillCommunicate[MAXLANGUAGETYPE];	//1代表纸币器通讯不上	
	char *Billmoto[MAXLANGUAGETYPE];		//1代表马达故障
	char *Billsensor[MAXLANGUAGETYPE];		//1代表传感器故障
	char *Billromchk[MAXLANGUAGETYPE];		//1代表rom出错
	char *Billjam[MAXLANGUAGETYPE];			//1代表投币卡币
	char *BillremoveCash[MAXLANGUAGETYPE];	//1移除纸币钞箱
	char *BillcashErr[MAXLANGUAGETYPE];		//1纸币钞箱故障
	char *Billdisable[MAXLANGUAGETYPE];		//1纸币因为各种原因被禁能了
	char *BillCoinEmp[MAXLANGUAGETYPE];		//1硬币找零不足
	//硬币器故障提示
	char *CoinCapion[MAXLANGUAGETYPE];					
	char *CoinCommunicate[MAXLANGUAGETYPE];	//1代表硬币器通讯不上	
	char *Coinsensor[MAXLANGUAGETYPE];		//1代表传感器故障
	char *Cointubejam[MAXLANGUAGETYPE];		//1代表出币口卡币
	char *Coinromchk[MAXLANGUAGETYPE];		//1代表rom出错
	char *Coinrouting[MAXLANGUAGETYPE];		//1代表进币通道出错
	char *Coinjam[MAXLANGUAGETYPE];			//1代表投币卡币
	char *CoinremoveTube[MAXLANGUAGETYPE];	//1移除硬币斗
	char *Coindisable[MAXLANGUAGETYPE];		//1硬币因为各种原因被vmc禁能了
	char *CoinunknowError[MAXLANGUAGETYPE];	//1硬币其他各种故障
	//找零器故障提示
	char *HopperCapion[MAXLANGUAGETYPE];					
	char *Hopper1State[MAXLANGUAGETYPE];	//Hopper1状态，0正常，1缺币，2故障	
	char *Hopper2State[MAXLANGUAGETYPE];	//Hopper2状态，0正常，1缺币，2故障
	char *Hopper3State[MAXLANGUAGETYPE];	//Hopper3状态，0正常，1缺币，2故障
	//货道故障提示
	char *GOCCapion[MAXLANGUAGETYPE];
	char *GOCError[MAXLANGUAGETYPE];	    //主箱体GOC状态，0正常，1故障
	char *GOCErrorSub[MAXLANGUAGETYPE];	    //从箱体GOC状态，0正常，1故障
	char *ColBoardError[MAXLANGUAGETYPE];	//主箱体货道板状态，0正常，1故障
	char *ColBoardErrorSub[MAXLANGUAGETYPE];//从箱体货道板状态，0正常，1故障
	//升降机故障提示
	char *Error_FMD[MAXLANGUAGETYPE];	//取货电机故障
	char *Error_FUM[MAXLANGUAGETYPE];	//上下电机故障
	char *Error_GOC[MAXLANGUAGETYPE];	    //出货确认对射板故障
	char *Error_FLD[MAXLANGUAGETYPE];		//层架检测传感器故障
	char *Error_FOD[MAXLANGUAGETYPE];		//起始位置传感器故障

	char *Error_UDS[MAXLANGUAGETYPE];		//用户取货门故障
	char *Error_GCD[MAXLANGUAGETYPE];	    //货道驱动板故障
	char *Error_SOVB[MAXLANGUAGETYPE];		//出货机构忙
	char *Error_SOMD1[MAXLANGUAGETYPE];		//机器大门状态
	char *Error_SOEC[MAXLANGUAGETYPE];			//收到错误数据包

	char *Error_SFHG[MAXLANGUAGETYPE];		//升降台取货
	//char *ElevatStart[MAXLANGUAGETYPE];		//升降机是否在原点
	char * Error_SOFP[MAXLANGUAGETYPE];	//货柜门是否打开取货
	//char *MotorBusy[MAXLANGUAGETYPE];		//电机忙
	char *Error_SOMD2[MAXLANGUAGETYPE];	 //取货门没关
	char *Emp_Gol[MAXLANGUAGETYPE];	 //货道全部不可用
	//PC故障提示
	char *PCCapion[MAXLANGUAGETYPE];
	char *scaleError[MAXLANGUAGETYPE];		//小数点位数
	char *offLineError[MAXLANGUAGETYPE];	//PC离线
	//设备正常
	char *DeviceOK[MAXLANGUAGETYPE];	//设备正常
} USERMAINTAINERRORMENULIST;

const USERMAINTAINERRORMENULIST UserMaintainErrorMenuList = 
{		
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"纸币器故障列表",
		"Bill acceptor fault list",
		"Приемнк купюр",
		"KAGIT PARA HATASI",
		"LISTA ERRADA DE NOTAS",
		"Bill acceptor fault list"
	},
	{
		"① 纸币器通讯不上",
		"1.Comminication failure",
		"1.Запись запроса",
		"1.BAGLANTI HATASI",
		"1.FALHA NA COMUNICA\x83\x82\O",
		"1.Echec Comminication"
	},
	{
		"② 马达故障",
		"2.Motor error",	
		"2.Код ошибки",
		"2.MOTOR HATASI",
		"2.ERRO MOTOR",
		"2.Erreur du moteur"
	},
	{
		"③ 传感器故障",
		"3.Sensor error",
		"3.Система",
		"3.SENSOR HATASI",
		"3.ERRO SENSOR",
		"3.Erreur du capteur"
	},
	{
		"④ rom出错",
		"4.Rom error",	
		"4.Колонка",
		"4.ROM HATASI",
		"4.ERRO ROM",
		"4.Eruur du Rom"
	},
	{
		"⑤ 投币卡币",
		"5.Bill jammed",
		"5.Добавить полную еду",
		"5.KAGIT PARA SIKISMASI",
		"5.NOTA AMASSADA",
		"5.Bourrage"
	},
	{
		"⑥ 移除纸币钞箱",
		"6.Stacker unlocked",
		"6.Система",
		"6.KASA KILITLENMEMIS",
		"6.EMPILHADOR DESBLOQUEADO",
		"6.Empileur ouvert"
	},
	{
		"⑦ 纸币钞箱故障",
		"7.Stacker error",	
		"7.Колонка",
		"7.KASA HATASI",
		"7.ERRO EMPILHADOR",
		"7.Erreur d empileur"
	},
	{
		"⑧ 纸币因为各种原因被禁能",
		"8.Bill acceptor disabled",
		"8.Система",
		"8.KAGIT PARA AKTIF DEGIL",
		"8.RECEPTOR NOTA DESATIVADO",
		"8.L accepteur de Bill hors de service"
	},
	{
		"⑨ 硬币不足",
		"9. Coin empty",
		"9.Coin empty",
		"9.Coin empty",
		"9.Coin empty",
		"9.Coin empty"
	},
	{
		"硬币器故障列表",
		"Coin acceptor Error list",
		"Приемник монет",
		"BOZUK PARA HATASI",
		"RECEPTOR MOEDA DESATIVADO",
		"Coin acceptor Error list"
	},
	{
		"① 硬币器通讯不上",
		"1.Communication failure",
		"1.Запись запроса",
		"1.BAGLANTI HATASI",
		"1.FALHA COMUNICA\x83\x82\O",
		"1.Echec de communication"
	},
	{
		"② 传感器故障",
		"2.Sensor error",	
		"2.Код ошибки",
		"2.SENSOR HATASI",
		"2.ERRO SENSOR",
		"2.Erreur du capteur"
	},
	{
		"③ 出币口卡币",
		"3.Coin exit jammed",
		"3.Система",
		"3.PARA CIKISI SIKISMIS",
		"3.MOEDA PRESA",
		"3.Bourrage a la Sortie de piece"
	},
	{
		"④ rom出错",
		"4.rom error",	
		"4.Колонка",
		"4.ROM HATASI",
		"4.ERRO ROM",
		"4.erreur du Rom"
	},
	{
		"⑤ 进币通道出错",
		"5.coin channel error",
		"5.Добавить полную еду",
		"5.PARA KANALI HATASI",
		"5.ERRO CANAL DA MOEDA",
		"5.erreur du canal de transmission"
	},
	{
		"⑥ 投币卡币",
		"6.Coin entrance jammed",
		"6.Система",
		"6.PARA GIRISI SIKISMIS",
		"6.ENTRADA MOEDA EMPERRADA",
		"6.Bourrage"
	},
	{
		"⑦ 移除硬币斗",
		"7.Coin holder removed",	
		"7.Колонка",
		"7.PARA TUTUCU SIKISMIS",
		"7.REMO\x83\x82\O DE MOEDA",
		"7.Detenteur enleve"
	},
	{
		"⑧ 硬币器被vmc禁能",
		"8.Disabled by vmc",
		"8.Система",
		"8.PARA MEK IPTAL",
		"8.DESATIVADO POR VMC",
		"8.Mis hors de service par vmc"
	},
	{
		"⑨ 硬币器其他故障=",
		"9. Other errors=",
		"9.Система=",
		"9.DIGER HATALAR=",
		"9.OUTROS ERROS=",
		"9.autres erreurs="
	},
	{
		"找零器故障列表",
		"Changer Error list",
		"Изменение",
		"BOZUK PARA M HATA LISTESI",
		"ERRO NO TROCADOR",
		"Liste d erreur du monnayeur"
	},
	{
		"① Hopper1:",
		"1.Hopper1",
		"1.Hopper1",
		"1.Hopper1",
		"1.HOPPER 1",
		"1.Tremie 1"
	},
	{
		"② Hopper2:",
		"2.Hopper2",	
		"2.Hopper2",
		"2.Hopper2",
		"2.HOPPER 2",
		"2.Tremie 1"
	},
	{
		"③ Hopper3:",
		"3.Hopper3",
		"3.Hopper3",
		"3.Hopper3",
		"3.HOPPER 3",
		"3.Tremie 1"
	},
	{
		"货道故障列表",
		"Column Error list",
		"GOC",
		"SUTUN HATALARI",
		"ERRO NA LISTA DA BANDEJA",
		"Liste d Erreur des colonnes"
	},
	{
		"① 主箱体出货检测故障",
		"1.Main cabinet GOC error",
		"1.A GOC Error",
		"1.ANA KABIN GOC HATASI",
		"1.ERRO GOC NO ARM\x81\RIO CENTRAL",
		"1.erreur Cabine principal GOC"
	},
	{
		"② 从箱体出货检测故障",
		"2.Sub-cabinet GOC error",
		"2.B GOC Error",
		"2.KOLE KABIN GOC HATASI",
		"2.ERRO GOC NO SUB ARM\x81\RIO",
		"2.erreur Sous-cabine GOC"
	},
	{
		"③ 主箱体货道板故障",
		"3.Main cabinet dispensing board error",
		"3.A Board Error",
		"3.ANA KABIN KART HATA",
		"3.ERRO NO ARMARIO CENTRAL",
		"3.erreur syst de distribution cabine principale"
	},
	{
		"④ 从箱体货道板故障",
		"4.Sub-cabinet dispensing board error",
		"4.B Board Error",
		"4.KOLE KABIN KART HATA",
		"4.ERRO NO DISPENSER DO SUB CENTRAL",
		"4.erreur syst de distribution sous cabine"
	},
	{
		"① 取货电机故障",
		"1.Collect motor error",
		"1.Запись запроса",
		"1.Kayxf1x31t sorgulama",
		"1.COLETAR ERRO MOTOR",
		"1.Collect motor error"
	},
	{
		"② 上下电机故障",
		"2.Elevator motor error",	
		"2.Код ошибки",
		"2.Hata kodu",
		"2.MOTOR DO ELEVADOR COM ERRO",
		"2.Elevator motor error"
	},
	{
		"③ 出货确认板故障",
		"3.GOC board error",
		"3.Система",
		"3.Sistem parametreleri",
		"3.ERRO DE PLACA GOC",
		"3.GOC board error"
	},
	{
		"④ 层架检测传感器故障",
		"4.Tray sensor error",	
		"4.Колонка",
		"4.Sutun parametre",
		"4.SENSOR DA BANDEIJA C/ ERRO",
		"4.Tray sensor error"
	},
	{
		"⑤ 开始位置传感器故障",
		"5.Start sensor error",
		"5.Добавить полную еду",
		"5.xf0xdcrxf0xfcn girin",
		"5.SENSOR DE INICIALIZAR C/ ERRO",
		"5.Start sensor error"
	},
	{
		"① 用户取货门故障",
		"1.Collect door error",
		"1.Запись запроса",
		"1.Kayxf1x31t sorgulama",
		"1.COLETOR DE PORTA COM ERRO",
		"1.Collect door error"
	},
	{
		"② 货道驱动板故障",
		"2.Dispensing board error",	
		"2.Код ошибки",
		"2.Hata kodu",
		"2.ERRO DA PLACA DISTRIBUICAO",
		"2.Dispensing board error"
	},
	{
		"③ 出货机构忙",
		"3.Dispensing mech busy",
		"3.Система",
		"3.Sistem parametreleri",
		"3.APLICADORA MECH OCUPADO",
		"3.Dispensing mech busy"
	},
	{
		"④ 取货门没关",
		"4.Machine door status",	
		"4.Колонка",
		"4.Sutun parametre",
		"4.SITUACAO DAS PORTAS MAQUINA",
		"4.Machine door status"
	},
	{
		"⑤ 收到错误数据包",
		"5.Incorrect data received",
		"5.Добавить полную еду",
		"5.xf0xdcrxf0xfcn girin",
		"5.DADOS INCORRETOS RECEBIDOS",
		"5.Incorrect data received"
	},
	{
		"① 升降台取货",
		"1.Elevator collecting",
		"1.Запись запроса",
		"1.Kayxf1x31t sorgulama",
		"1.ELEVADOR COLETA",
		"1.Elevator collecting"
	},	
	{
		"② 货柜门是否打开取货",
		"2.Collect door open",
		"2.Система",
		"2.Sistem parametreleri",
		"2.PORTA ABERTA",
		"2.Collect door open"
	},	
	{
		"③ 货柜大门没关",
		"3.Collect door not closed ",
		"3.Добавить полную еду",
		"3.xf0xdcrxf0xfcn girin",
		"3.COLETOR DE PORTA ABERTO",
		"3.Collect door not closed "
	},
	{
		"④ 全货道故障",
		"4.Column Empty",
		"4.Column Empty",
		"4.xf0xdcrxf0xfcn girin",
		"4.COLETOR DE PORTA ABERTO",
		"4.Column Empty"
	},
	{
		"PC故障列表",
		"PC Error list",
		"PC",
		"PC HATALARI",
		"ERRO PC",
		"PC Error list"
	},
	{
		"① 小数点位数",
		"1.Decimal Digit Incorrect",
		"1.Decimal Digit Incorrect",
		"1.Decimal Digit Incorrect",
		"1.Decimal Digit Incorrect",
		"1.Decimal Digit Incorrect"
	},	
	{
		"② 工控机离线",
		"2.PC Communication Failed",
		"2.PC Communication Failed",
		"2.PC Communication Failed",
		"2.PC Communication Failed",
		"2.PC Communication Failed"
	},
	{
		"正常",
		"OK",
		"Хорошо",
		"OK",
		"OK",
		"OK"
	}
};


/*************************************用户模式维护主菜单按键选择************************************************/
typedef struct
{
	unsigned char KeyCurrent;						//当前菜单索引号
	unsigned char ExtSelfCheck;						//退出设备自检菜单并返回维护主菜单
	unsigned char PageNumb;							//菜单页索引
	void (*TestOperate)();							//当前状态应该执行的功能操作
} LOGMAINTAINMAINMENU;											//维护主菜单
LOGMAINTAINMAINMENU LogMaintainMainMenu;									//维护主菜单结构体


/*************************************交易记录菜单选择************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//交易记录提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *Record[MAXLANGUAGETYPE];		
	char *ColumnRecord[MAXLANGUAGETYPE];
	char *ClearRecord[MAXLANGUAGETYPE];
	char *RecordTotal[MAXLANGUAGETYPE];
	char *RecordPrint[MAXLANGUAGETYPE];
	char *RecordIng[MAXLANGUAGETYPE];
	char *CancelPin[MAXLANGUAGETYPE];
	char *printTrade[MAXLANGUAGETYPE];
} LOGMAINTAINMAINMENULIST;

const LOGMAINTAINMAINMENULIST LogMaintainMenuList = 
{
	{
		"交易记录",
		"Transaction record",
		"Transaction record",
		"SATIS BILGILERI",
		"VENDAS",
		"Rapport des ventes"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 交易记录",
		"1.Sales record",
		"1.Платежи",
		"1.SATIS RAPORU",
		"1.VENDAS",
		"1.Rapport des ventes"
	},
	{
		"② 单货道统计",
		"2.Column record",	
		"2.Столбец транзакций:",
		"2.SUTUN RAKAMLARI",
		"2.BANDEJA",
		"2.Rapport des colonnes"
	},
	{
		"③ 清除交易记录",
		"3.Clear record",
		"3.Очистить запись",
		"3.KAYITLARI SIL",
		"3.REGISTRO APAGADO",
		"3.Effacer les rapports"
	},
	{
		"④ 总交易记录",
		"4.Total sales record",	
		"4.квартал Платежи",
		"4.TOPLAM SATIS TUTARI",
		"4.VENDA TOTAL"	,
		"4.Rapport global des ventes"	
	},
	{
		"⑤ 交易记录明细",
		"5.Sales detail",	
		"5.Колонка",
		"5.SATIS DETAYLARI",
		"5.DETALHE DAS VENDAS",
		"5.Detail des ventes"
	},
	{
		"正在打印,请稍等...",
		"Under printing, pls wait...",	
		"Print,Please Wait...",
		"Print,Please Wait...",
		"IMPRIMINDO, AGUARDE...",
		"IMPRIMINDO, AGUARDE..."
	},
	{
		"返回",
		"Under printing, pls wait...",	
		"Print,Please Wait...",
		"Print,Please Wait...",
		"IMPRIMINDO, AGUARDE...",
		"IMPRIMINDO, AGUARDE..."
	},
	{
	"打印交易记录",
	"Under printing, pls wait...",	
	"Print,Please Wait...",
	"Print,Please Wait...",
	"IMPRIMINDO, AGUARDE...",
	"IMPRIMINDO, AGUARDE..."
	}
};


/*************************************区域交易记录,总交易记录菜单选择************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//交易记录提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *Income[MAXLANGUAGETYPE];	//总收币	
	char *NoteIncome[MAXLANGUAGETYPE];
	char *CoinsIncome[MAXLANGUAGETYPE];
	//char *TotalTrans[MAXLANGUAGETYPE];
	char *TotalChange[MAXLANGUAGETYPE];
	//char *SuccessNum[MAXLANGUAGETYPE];
	char *DoubtNum[MAXLANGUAGETYPE];
	char *Iou[MAXLANGUAGETYPE];
	//char *Card[MAXLANGUAGETYPE];

	
	//add by yoc 2013.9.11
	/*
	char *batch_num[MAXLANGUAGETYPE];
	char *batch_money[MAXLANGUAGETYPE];
	char *batch_cash_num[MAXLANGUAGETYPE];
	char *batch_cash_money[MAXLANGUAGETYPE];
	char *batch_game_num[MAXLANGUAGETYPE];
	char *batch_game_money[MAXLANGUAGETYPE];
	char *batch_onecard_num[MAXLANGUAGETYPE];
	char *batch_onecard_money[MAXLANGUAGETYPE];
	char *batch_unioncard_num[MAXLANGUAGETYPE];
	char *batch_unioncard_money[MAXLANGUAGETYPE];
	char *batch_pc_num[MAXLANGUAGETYPE];
	char *batch_pc_money[MAXLANGUAGETYPE];
	*/
	char *total_num[MAXLANGUAGETYPE];
	char *total_money[MAXLANGUAGETYPE];
	char *total_cash_num[MAXLANGUAGETYPE];
	char *total_cash_money[MAXLANGUAGETYPE];
	char *total_card_num[MAXLANGUAGETYPE];
	char *total_card_money[MAXLANGUAGETYPE];
	char *total_pc_num[MAXLANGUAGETYPE];
	char *total_pc_money[MAXLANGUAGETYPE];
	char *total_game_num[MAXLANGUAGETYPE];
	char *total_game_money[MAXLANGUAGETYPE];
	
	
} LOGTRANSMENULIST;

const LOGTRANSMENULIST LogTransMenuList = 
{
	{
		"交易记录",
		"Sales record",
		"Sales record",
		"SATIS RAKAMLARI",
		"VENDAS",
		"Rapport des ventes"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"1.货币总收入:",
		"1.Total income:",
		"1.Суммарная выручка:",
		"1.TOPLAM SATIS:",
		"1.RECEITA TOTAL:",
		"1.Encaissement total:"
	},
	{
		"2.纸币收入  :",
		"2.Bill income:",	
		"2.Принято купюр:",
		"2.KAGIT PARA SATTIS:",
		"2.NOTAS RECEBIDAS:",
		"2.Encaissement billets:"
	},
	{
		"3.硬币收入  :",
		"3.Coin income:",
		"3.Принято монет:",
		"3.MADENI PARA SATIS:",
		"3.MOEDAS RECEBIDAS:",
		"3.Encaissement pieces:"
	},	
	{
		"4.找零总金额:",
		"4.Total change:",	
		"4.Всего разменов:",
		"4.TOPLAM PARA USTU:",
		"4.TROCO TOTAL:",
		"4.Total des monnaies:"
	},	
	{
		"5.疑问交易次数:",
		"5.Doubtful deal qty:",	
		"5.Кол-во сомнений:",
		"5.SUPHELI SATIS:",
		"5.D\x84\VIDA NA QUANTIDADE:",
		"5.douteuse:"
	},
	{
		"5.欠条总额:",
		"5.Total IOU amount:",
		"5.IOU:",
		"5.TOPLAM BORC MIKTARI:",
		"5.VALOR TOTAL DE IOU:",
		"5.transaction espece:"
	},
	{
		"6.总交易次数:",
		"6.Total trade num:",
		"6.Total trade num:",
		"6.TOPLAM SATIS NO:",
		"6.Total trade num:",
		"6.Total trade num:"
	},
	{
		"7.总交易金额:",
		"7.Total trade amount:",
		"7.Total trade amount:",
		"7.TOPLAM MIKTAR:",
		"7.Total trade amount:",
		"7.Total trade amount:"
	},
	{
		"9.现金交易次数:",
		"9.Cash trade num:",
		"9.Cash trade num:",
		"9.Cash trade num:",
		"9.Cash trade num:",
		"9.Cash trade num:"
	},
	{
		"10.现金交易金额:",
		"10.Cash trade amount:",
		"10.Cash trade amount:",
		"10.Cash trade amount:",
		"10.Cash trade amount:",
		"10.Cash trade amount:"
	},
		{
		"11.卡交易次数:",
		"11.Card trade num:",
		"11.Card trade num:",
		"11.Card trade num:",
		"11.Card trade num:",
		"11.Card trade num:"
	},
	{
		"12.卡交易金额:",
		"12.Card trade amount:",
		"12.Card trade amount:",
		"12.Card trade amount:",
		"12.Card trade amount:",
		"12.Card trade amount:"
	},
	{
		"13.在线交易次数:",
		"13.Online trade num:",
		"13.Online trade num:",
		"13.Online trade num:",
		"13.Online trade num:",
		"13.Online trade num:"
	},
	{
		"14.在线交易金额:",
		"14.Online trade amount:",
		"14.Online trade amount:",
		"14.Online trade amount:",
		"14.Online trade amount:",
		"14.Online trade amount:"
	},
	{
		"15.游戏交易数量:",
		"15.Game trade num:",
		"15.Game trade num:",
		"15.Game trade num:",
		"15.Game trade num:",
		"15.Game trade num:"
	},
	{
		"16.游戏交易金额:",
		"16.Game trade amount:",
		"16.Game trade amount:",
		"16.Game trade amount:",
		"16.Game trade amount:",
		"16.Game trade amount:"
	}
	
		
};



/*************************************交易设置菜单按键选择************************************************/
typedef struct
{
	unsigned char KeyCurrent;						//当前菜单索引号
	unsigned char ExtSelfCheck;						//退出设备自检菜单并返回维护主菜单
	unsigned char PageNumb;							//菜单页索引
	void (*TestOperate)();							//当前状态应该执行的功能操作
} USERTRADEMAINMENU;											//维护主菜单
USERTRADEMAINMENU UserTradeMainMenu;									//维护主菜单结构体


/*************************************交易设置菜单选择************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//交易记录提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *RTC[MAXLANGUAGETYPE];		
	char *Compressor[MAXLANGUAGETYPE];
	char *LED[MAXLANGUAGETYPE];
	char *Multivend[MAXLANGUAGETYPE];
	char *Refund[MAXLANGUAGETYPE];
	char *BillValidatorTest[MAXLANGUAGETYPE];		
	char *CoinAcceptorTest[MAXLANGUAGETYPE];
	char *CoinChangerTest[MAXLANGUAGETYPE];	
	char *PCSet[MAXLANGUAGETYPE];
	char *MaxValue[MAXLANGUAGETYPE];
	char *BillValidatorType[MAXLANGUAGETYPE];
	char *CoinAcceptorType[MAXLANGUAGETYPE];
	char *CashlessDeviceType[MAXLANGUAGETYPE];
	char *SaleTime[MAXLANGUAGETYPE];
	char *ColumnTime[MAXLANGUAGETYPE];
	char *box_hot[MAXLANGUAGETYPE];
	char *box_chuchou[MAXLANGUAGETYPE];
	char *BillEnableValue[MAXLANGUAGETYPE];	
	
} USERTRADEMAINMENULIST;

const USERTRADEMAINMENULIST UserTradeMenuList = 
{
	{
		"交易设置",
		"Purchase setting",
		"Purchase setting",
		"SATIS AYARLARI",
		"PROGRAMA\x83\x82\O DE COMPRA",
		"Parametre des ventes"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 时间设置",
		"1.Time",
		"1.установка времени",
		"1.ZAMAN",
		"1.HOR\x81\RIO",
		"1.Temps"
	},
	{
		"② 制冷控制",
		"2.Refrigeration",	
		"2.регулирование температуры",
		"2.SOGUTUCU",
		"2.REFRIGERA\x83\x82\O",
		"2.Refrigeration"
	},
	{
		"③ 照明控制",
		"3.LIGHT",
		"3.LED температуры",
		"3.ISIK",
		"3.LUZ",
		"3.LUMIERE"
	},
	{
		"④ 多次购买:",
		"4.Multi-vend:",	
		"4.Мулти решение:",
		"4.COKLU SATIS ACIK:",
		"4.MULTI-VENDA:",
		"4.Vente multiple:"
	},
	{
		"⑤ 强制购买:",
		"5.Force-vend:",	
		"5.переопределение:",
		"5.ZORUNLU SATIS:",
		"5.ENERGIA-VENDA:",
		"5.Force de vente:"
	},
	{
		"① 纸币器测试",
		"1.Bill acceptor test",
		"1.Приемнк купюр: ",
		"1.KAGIT PARA TESTI ",
		"1.TESTE RECEPTOR DE NOTA",
		"1.Billet"
	},
	{
		"② 硬币器测试",
		"2.Coin acceptor test",
		"2.Приемник монет: ",
		"2.BOZUK PARA TESTI ",
		"2.TESTE RECEPTOR MOEDA",
		"2.piece"
	},
	{
		"③ 找零器测试",
		"3.Coin changer test",
		"3.Изменение 1UINT: ",
		"3.PARA BOZMA TESTI ",
		"3.TESTE TROCADOR MOEDA",
		"3.Test du monnayeur"
	},
	{
		"④ 工控机设置",
		"4.PC set",
		"4.PC set",
		"4.PC set",
		"4.PC set",
		"4.PC set"
	},
	{
		"④ 纸币上限:",
		"4.MaxValue:",
		"4.MaxValue:",
		"4.MAX KAGIT PARA:",
		"4.MaxValue:",
		"4.MaxValue:"
	},
	{
		"① 纸币器类型:",
		"1.BillValidatorType:",	
		"1.BillValidatorType:",
		"1.KAGIT PARA AYARI:",
		"1.BillValidatorType:",
		"1.BillValidatorType:"
	},
	{
		"② 硬币器类型:",
		"2.CoinAcceptorType:",	
		"2.CoinAcceptorType:",
		"2.BOZUK PARA AYARI:",
		"2.CoinAcceptorType:",
		"2.CoinAcceptorType:"
	},
	{
		"③ 读卡器类型:",
		"3.CashlessDeviceType:",
		"3.CashlessDeviceType:",
		"3.KART OKUYUCU AYARI:",
		"3.CashlessDeviceType:",
		"3.CashlessDeviceType:"
	},
	{
		"④ 交易时间:",
		"4.SaleTime:",	
		"4.SaleTime:",
		"4.SaleTime:",
		"4.SaleTime:",
		"4.SaleTime:"
	},
	{
		"⑤ 出货时间:",
		"5.ColumnTime:",	
		"5.ColumnTime:",
		"5.ColumnTime:",
		"5.ColumnTime:",
		"5.ColumnTime:"
	},
	{
		"① 柜内加热",
		"1.hot:",
		"1.CashlessDeviceType:",
		"1.KART OKUYUCU AYARI:",
		"1.CashlessDeviceType:",
		"1.hot:"
	}
		,
	{
		"② 柜内除臭",
		"2.chuchou:",
		"2.CashlessDeviceType:",
		"2.KART OKUYUCU AYARI:",
		"2.CashlessDeviceType:",
		"2.chuchou:"
	},
	{
		"③ 纸币器上限:",
		"3.BillEnableValue:",
		"3.BillEnableValue:",
		"3.MAX KAGIT PARA:",
		"3.BillEnableValue:",
		"3.BillEnableValue:"
	}
};

/*************************************时间设置菜单选择************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//交易记录提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *year[MAXLANGUAGETYPE];		
	char *month[MAXLANGUAGETYPE];
	char *day[MAXLANGUAGETYPE];
	char *hour[MAXLANGUAGETYPE];
	char *minute[MAXLANGUAGETYPE];
	char *week[MAXLANGUAGETYPE];
} RTCMAINMENULIST;

const RTCMAINMENULIST RtcMenuList = 
{
	{
		"时间设置",
		"Time",
		"Time",
		"ZAMAN",
		"HOR\x81\RIO",
		"Temps"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 年:   ",
		"1.Year:   ",
		"1.Год:   ",
		"1.YIL:   ",
		"1.ANO:   ",
		"1.Annee:   "
	},
	{
		"② 月:   ",
		"2.Month:  ",	
		"2.Месяц:   ",
		"2.AY:   ",
		"2.M\x85\S:   ",
		"2.Mois:  "
	},
	{
		"③ 日:   ",
		"3.Day:    ",
		"3.День:   ",
		"3.GUN:   ",
		"3.DIA:   ",
		"3.Jour:    "
	},
	{
		"④ 时:   ",
		"4.Hour:   ",	
		"4.Часы:   ",
		"4.SAAT:   ",
		"4.HORA:  ",
		"4.Heure:   "
	},
	{
		"⑤ 分:   ",
		"5.Minute: ",	
		"5.Минуты:   ",
		"5.DAKIKA:   ",
		"5.MINUTO:",
		"5.Minute: "
	},
	{
		"① 星期:   ",
		"1.Week: ",	
		"1.неделя:   ",
		"1.HAFTA:   ",
		"1.semana:",
		"1.Week: "
	}
};



/*************************************设备状态值选择************************************************/
typedef struct
{
	char *EMPTY[MAXLANGUAGETYPE];
	char *ERR[MAXLANGUAGETYPE];	
	char *UNSET[MAXLANGUAGETYPE];	
} ERRORSTATETO;

const ERRORSTATETO ErrorStateTo = 
{
	{
		"缺币",
		"Noenough",
		"Достаточно",
		" yetersiz",
		"Noenough"
	},	
	{
		"故障",
		"Error",
		"Ошибка",
		"Hata",
		"Error"
	},	
	{
		"未配",
		"UnKnow",
		"UnKnow",
		"UnKnow",
		"UnKnow"
	}
};

/*************************************输入菜单值选择************************************************/
typedef struct
{
	char *Choose[MAXLANGUAGETYPE];		
	char *ON[MAXLANGUAGETYPE];
	char *OFF[MAXLANGUAGETYPE];	
	char *UNSET[MAXLANGUAGETYPE];	
} USERCHOOSE;

const USERCHOOSE UserChoose = 
{
	{
		"1-开启,0-关闭",
		"1-ON,0-OFF",
		"1-ON,0-OFF",
		"1-ON,0-OFF",
		"1-ON,0-OFF",
		"1-ON,0-OFF"
	},	
	{
		"开启",
		"ON",
		"ON",
		"ON",
		"LIGADO",
		"ON"
	},
	{
		"关闭",
		"OFF",
		"OFF",
		"OFF",
		"DESLIGADO",
		"OFF"
	}
		,
	{
		"未配",
		"UnKnow",
		"UnKnow",
		"UnKnow",
		"UnKnow",
		"UnKnow"
	}
};


/**************************************工控机设置菜单****************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//自检模式提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *VPAddCoin[MAXLANGUAGETYPE];		    //补硬币完成
	char *VPSynGoodsCol[MAXLANGUAGETYPE];       //全系统同步
	char *VPAddBill[MAXLANGUAGETYPE];           //取纸币完成 
	char *VPSuccess[MAXLANGUAGETYPE];       //操作成功
	char *VPFail[MAXLANGUAGETYPE];           //操作失败
	char *VPSyning[MAXLANGUAGETYPE];       //全系统同步
	char *VPBuhuo[MAXLANGUAGETYPE];       //补货完成
	char *VPBuhuoing[MAXLANGUAGETYPE];       //正在补货
} USERUBOXPCMENULIST;

const USERUBOXPCMENULIST UserUboxPCMenuList = 
{
	{
		"工控机设置",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
		"MAINTAIN MODE",
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 补硬币完成",
		"1.Sales record",
		"1.Запись запроса",
		"1.Kayxf1x31t sorgulama",
		"1.VENDAS",
		"1.Sales record"
	},
	{
		"② 全系统同步",
		"2.Error inqiry",	
		"2.Код ошибки",
		"2.Hata kodu",
		"2.ERRO CONSULTADO",
		"2.Error inqiry"
	},
	{
		"③ 取纸币完成",
		"3.Purchase setting",
		"3.Система",
		"3.Sistem parametreleri",
		"3.COMPRA",
		"3.Purchase setting"
	},
	{
		"操作成功",
		"SUCCESSFUL",	
		"SUCCESSFUL",
		"SUCCESSFUL",
		"SUCCESSFUL",
		"SUCCESSFUL"
	},
	{
		"操作失败",
		"FAIL",
		"FAIL",
		"FAIL",
		"FAIL",
		"FAIL"
	},
	{
		"正在全系统同步...",
		"Busy...",
		"Busy...",
		"Busy...",
		"Busy...",
		"Busy..."
	},
	{
		"④ 补货完成",
		"4.AddColumn",
		"4.AddColumn",
		"4.AddColumn",
		"4.AddColumn",
		"4.AddColumn"
	},
	{
		"正在补货...",
		"Busy...",
		"Busy...",
		"Busy...",
		"Busy...",
		"Busy..."
	}	
};



/*************************************调试菜单选择************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//交易记录提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	char *billTrace[MAXLANGUAGETYPE];		
	char *coinTrace[MAXLANGUAGETYPE];
	char *changelTrace[MAXLANGUAGETYPE];
	char *readerTrace[MAXLANGUAGETYPE];
	char *channelTrace[MAXLANGUAGETYPE];
	char *compressorTrace[MAXLANGUAGETYPE];
	char *ledTrace[MAXLANGUAGETYPE];	
	char *selectTrace[MAXLANGUAGETYPE];
	char *PCTrace[MAXLANGUAGETYPE];
	char *TraceFlag[MAXLANGUAGETYPE];
} USERTRACEMAINMENULIST;

const USERTRACEMAINMENULIST UserTraceMenuList = 
{
	{
		"调试设置",
		"Debug mode",
		"Debug mode",
		"AYAR MODU",
		"MODO DE AJUSTE",
		"AYAR MODU"
	},	
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<PR\x80\XIMA P\x81\GINA>",
		"<Page suivante>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<P\x81\GINA ANTERIOR>",
		"<Page avant>"
	},
	{
		"① 纸币器监控:",
		"1.Bill acceptor:",
		"1.купюр:",
		"1.KAGIT PARA ACIK:",
		"1.RECEPTOR DE NOTA:",
		"1.Bill acceptor:"
	},
	{
		"② 硬币器监控:",
		"2.Coin acceptor:",	
		"2.ионет:",
		"2.BOZUK P KAPALI:",
		"2.RECEPTOR DE MOEDA:",
		"2.Coin acceptor:"
	},
	{
		"③ 找零器监控:",
		"3.Changer:",
		"3.Изменение:",
		"3.BOZUK P ACIK:",
		"3.TROCA:",
		"3.Changer:"
	},
	{
		"④ 读卡器监控:",
		"4.Card reader:",	
		"4.Читатель типа:",
		"4.KART OKUYUCU ACIK:",
		"4.LEITURA CART\x82\O:",
		"4.Card reader:"
	},
	{
		"⑤ 货道监控:",
		"5.Column:",	
		"5.Тест колонки:",
		"5.SUTUN ACIK:",
		"5.BANDEJA:",
		"5.Column:"
	},
	{
		"① 压缩机监控:",
		"1.Temp. controller:",
		"1.регулирование:",
		"1.ISI KONTROLU KAPALI:",
		"1.TEMPERATURA:",
		"1.Temp. controller:"
	},
	{
		"② 展示灯监控:",
		"2.Light:",	
		"2.LED:",
		"2.ISIK ACIK:",
		"2.LUZ:",
		"2.Light:"
	},
	{
		"③ 选货按键监控:",
		"3.SelectKey:",
		"3.Изменение:",
		"3.SelectKey:",
		"3.SelectKey:",
		"3.SelectKey:"
	},
	{
		"④ PC监控:",
		"4.PC:",
		"4.PC:",
		"4.PC:",
		"4.PC:",
		"4.PC:"
	},
	{
		"⑤ 监控:",
		"5.TraceFlag:",	
		"5.Тест колонки:",
		"5.ACIK:",
		"5.BANDEJA:",
		"5.TraceFlag:"
	}
};






/**************************************End Of File*******************************************************/

