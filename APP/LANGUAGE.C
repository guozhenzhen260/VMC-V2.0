/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LANGUAGE.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        菜单语言                    
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#include "..\config.h"



/*************************************维护界面货道菜单************************************************/
/*
const char *     Menu_Channel[LANGUAGENO][5]=
{
	 {
	 "1.货道编号: ",
	 "2.单价:     ",
	 "3.商品余量: ",
	 "4.货道状态: ",
	 "5.商品编号: "	 
	 },
	 {
	 "1.Column code:",
	 "2.Price:",
	 "3.Goods remain:",
	 "4.Column status:",
	 "5.Goods NO:"
	 },
	 //change by liya 2012-10-16
	 {
	 "1.Код колонки: ",
	 "2.Цена: ",
	 "3.Остатки еды: ",
	 "4.Статус колонки: ",
	 "5. Номер еды: "
	 },
	 //土耳其语;by gzz 20121115
	 {
	 	"1 S\xf0\xfctun kodu:",
		"2.Fiyat:",
		"3.Kalan \xf0\xfcr\xf0\xfcn:",
		"4 S\xf0\xfctun  durumu:",
		"5.\xf0\xdcr\xf0\xfcn numaras\xf1\x31:"
	 }
};
*/










/*************************************交易界面空闲界面************************************************/
const char *     BUSINESS[MAXLANGUAGETYPE][10]=
{
	 {
	 "输入商品编号:",
	 "输入错误",
	 "禁能",
	 "单价:",
	 "故障",
	 "缺货", 
	 "  欢迎光临",
	 "货道",
	  "无零币",
	  "本机不收纸币"
	 },
	 {
	 "Input item code:",
	 "Invalid choice",
	 "Disabled",
	 "Price:",
	 "Error",
	 "Empty",
	 "  Welcome",
	 "Channel"
	 },
	 {
	 "Введите код позиции:",
	 "Неправильный выбор",
	 "Выключено",
	 "Price:",
	 "Ошибка",
	 "Выдача",
	 "  Welcome",
	 "Канал",
	 "Welcome",
	 "Канал"
	 },
	 {
	 "URUN KODU GIRINIZ:",
	 "GECERSIZ SECIM",
	 "DISABLE",
	 "FIYAT:",
	 "HATALI",
	 "BITTI",
	 "HOSGELDINIZ",
	 "SUTUN",
	 "Welcome",
	 "Канал"
	 },
	 {
	 "ENSERIR CODIGO:",
	 "ESCOLHA INVALIDA",
	 "SEM FUNCIONAMENTO",
	 "VALOR:",
	 "ERRO",
	 "VAZIO",
	 "  BENVINDO",
	 "BANDEJA",
	 "Welcome",
	 "Канал"
	 },
	 {
	 "produit:",
	 "Choix invalide",
	 "Hors service",
	 "Montant:",
	 "Erreur",
	 "Vide",
	 "  Bienvenue",
	 "Colonne",
	 "Empty",
	 "Inserez un billet plus petit SVP"
	 }
};

/*************************************交易界面故障界面************************************************/
const char  *BUSINESSERROR[MAXLANGUAGETYPE][23] =
{
   {
	  "暂停服务",   
	  "故障码:	",
	  "商品已售完",
	  "找零器故障",
	  "纸币器故障",
	  "硬币器故障",
	  "找币器缺币",
	  "所有货道无货",
	  "所有货道故障",
	  "货道板故障",
	  "货道不可用",
	  "系统参数故障",
	  "LCD 故障",
	  "货道单价异常",
	  "出货确认故障",
	  "Flash故障",
	  "时钟故障",
	  "取货机构故障",
	  "货柜门没关",
	  "故障",
	  "注意",
	  "本机暂无法找零,请谨慎选购",
	  "无零币,本机不收纸币"
	  
   },
   {	
	  "Sorry, out of service",
	  "Err code: ",
	  "Channel err",
	  "Hopper is wrong",
	  "Bill err",
	  "Coin err",
	  "Hopper empty",
	  "Column empty",
	  "column err",
	  "column err",
	  "column err",
	  "system err",
	  "LCD err",
	  "price err",
	  "GOC err",
	  "Flash err",
	  "RTC err",
	  "FGP err",
	  "Door Open",
	  "Sorry",
	  "Caution",
	  "Unable to Give Change Now",
	  "Bill is disable"
   },
   //change by liya 2012-10-16
   {
	  "Не работает",
	  "Код ошибки:",
	  "Ошибка канала",
	  "Хоппер ",
	  "Билл ",
	  "Монета",
	  "Отсутствие валюты",
	  "проданный",
	  "неисправность",
	  "неисправность",
	  "неисправность",
	  "параметры системы",
	  "LCD",
	  "Цена неправильно",
	  "GOC",
	  "Flash",
	  "RTC",
	  "FGP",
	  "двери контейнера",
	  "извините",
	  "извините",
	  "извините",
	  "извините"
   },
   //土耳其语;by gzz 20121115
   {
	  "SERVIS DISI",   
	  "HATA KODU",
	  "f0dcrf0fcn satf131ldf131",
	  "Def11fif15fiklif11fi hatasf131 tutun",
	  "Bill yetmezlif11fi",
	  "Coin hatasf131",
	  "Paralar eksik sikke bul",
	  "Tf0fcm gf0F6nderiler hif0e7bir mal road",
	  "Tf0fcm Kargo Karayolu fay",
	  "Kargo Karayolu kartf131 arf131zasf131",
	  "Kargo Karayolu kullanf131lamf131yor",
	  "Sistem parametreleri arf131zasf131",
	  "LCD arf131za",
	  "Fiyat anormal",
	  "GOC arf131za",
	  "Flash arxf1x31za",
	  "RTC arxf1x31za",
	  "FGP arxf1x31za",
	  "Kapxf1x31 kapalxf1x31 dexf1x1fil Pick",
	  "SERVIS DISI",
	  "\xf0\xfczg\xf0\xfcn\xf0\xfcm",
	  "\xf0\xfczg\xf0\xfcn\xf0\xfcm",
	  "P.USTU YOK SADECE BOZUK P"
   },
   {	
	  "DESLIGADO",
	  "CODIGO ERRADO: ",
	  "Channel err",
	  "Hopper is wrong",
	  "Bill err",
	  "Coin err",
	  "Hopper empty",
	  "Column empty",
	  "column err",
	  "column err",
	  "column err",
	  "system err",
	  "LCD err",
	  "price err",
	  "GOC err",
	  "Flash err",
	  "RTC err",
	  "FGP err",
	  "Door Open",
	  "Sorry",
	  "Sorry",
	  "Sorry",
	  "Sorry"
   },
   {
	  "Desole, hors service",   
	  "Erreur:	",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Erreur",
	  "Desole",
	  "Desole",
	  "Monnaie insuffisante",
	  "Monnaie insuffisante"
	  
   }
};


/*************************************交易界面交易界面************************************************/
const char  *BUSINESSDEAL[MAXLANGUAGETYPE][20] =
{
   {	  
	  "金额",
	  "余额",
	  "请插入更小面额纸币",
	  "金额不足,",
	  "请按键选购",
	  "继续选购",
	  "找零退币",
	  "输入商品编号:",
	  "输入错误",
	  "故障",
	  "缺货",
	  "正在出货..",
	  "选购成功, 请取货",
	  "商品已售完",
	  "零钱不足",
	  "正在扣款,请稍后..",
	  "扣款失败",
	  "计时:",
	  "请输入柜子号:",
	  "柜,请输入格子号:"
	  
   },
   {
   	  "Amount",	
	  "Balance",
	  "Pls insert a smaller bill",
	  "Insufficient fund,",
	  "Pls choose the product",
	  "Continue",
	  "Coin refund",
	  "Input Item code:",
	  "Invalid choice",
	  "Error",
	  "Empty",
	  "Dispensing..",
	  "Taking the goods",
	  "Sold out",
	  "Insufficient change",
	  "Debiting, please wait ... ",
	  "Debit failed",
	  "Time:",
	  "Input Cabinet No.:",
	  ",Input Box No.:"
	  
	  
   },
   //change by liya 2012-10-16
   {
   	  "Баланс",
	  "Баланс",
	  "Вставьте поменьше",
	  "Найдено несоотв,",
	  "Пожалуйста, нажмите купить",
	  "продолжать",
	  "CASHOUT"	 ,
	  "Введите код позиции:",
	  "Неправильный выбор",
	  "Ошибка",
	  "Выдача",
	  "Захват еды..",
	  "Заберите еду",
	  "Продано",
	  "Изменить нехватки",
	  "дебетуется..",
	  "Fail",
	  "Time:",
	  "Input Bin No.:",
	  ",Input Box No.:"
	   
   },
   //土耳其语;by gzz 20121115
   {
   	  "TUTAR",
	  "BAKIYE",
	  "GECERLI KAGIT PARA GIRINIZ",
	  "YETERSIZ BAKIYE",
	  "LUTFEN URUN SECINIZ",
	  "DEVAM",
	  "PARA IADE",
	  "xf0xdcrxf0xfcn kodunu giriniz:",
	  "GECERSIZ SECIM",
	  "HATA",
	  "BOS",
	  "DAGITIM",
	  "URUNU ALINIZ",
	  "URUN YOK",
	  "BOZUK PARA YETERSIZ",
	  "borxf0xe7 olmak..",
	  "Fail",
	  "Time:",
	  "Input Bin No.:",
	  ",Input Box No.:"
   },
   {
   	  "VALOR",	
	  "SALDO",
	  "POR FAVOR ENSERIR MENOR NOTA",
	  "SALDO INSUFICIENTE,",
	  "ESCOLHA O PRODUTO",
	  "CONTINUE",
	  "TROCO MOEDA",
	  "ENSERIR CODIGO:",
	  "ESCOLHA INVALIDA",
	  "ERRO",
	  "VAZIO",
	  "PREPARANDO PARA SAIR",
	  "PEGAR PRODUTOS",
	  "SEM PRODUTO",
	  "TROCO  INSUFICIENTE",
	  "COBRANDO, OBRIGADO ... ",
	  "FALHA NA COBRAN\x83\A",
	  "TEMPO:",
	  "Input Bin No.:",
	  ",Input Box No.:" 
	  
   },
   {	  
	  "Comparaison",
	  "Comparaison",
	  "Inserez un billet plus petit SVP",
	  "Fonds insuffisant,",
	  "Choisissez le produit SVP",
	  "Continuez",
	  "Retour",
	  "produit:",
	  "Choix invalide",
	  "Erreur",
	  "Vide",
	  "Distribution en cours..",
	  "Prenez votre marchandise SVP",
	  "Finalisation",
	  "Fonds insuffisan",
	  "Distribution en cours..",
	  "fail",
	  "Time:",
	  "Choisissez le produit SVP:",
	  "Choisissez le produit SVP:"
	  
   }
};

/*************************************交易界面找零界面************************************************/
const char  *BUSINESSCHANGE[MAXLANGUAGETYPE][5] =
{
   {
      "正在找零",	  
	  "请致电客服热线,欠条:",
	  "请取零钱",
	  "品味旅途，收藏记忆",
	  "   谢谢惠顾!"
   },
   {
      "Changing",
	  "IOU:",	  
	  "Please take the changes",
	  "Thank You",
	  " "
   },
   //change by liya	2012-10-16
   {
      "Замена:",
	  "я долен Вам: ",	 
	  "Набор монет",
	  "Спасибо",
	  "   Спасибо"
	  
   },
   //土耳其语;by gzz 20121115
   {
      "BEKLEYINIZ",
	  "LUTFEN SERVISI ARAYIN: ",	 
	  "LUTFEN PARA USTUNU ALINIZ",
	  "TESEKKUR EDERIZ",
	  "TESEKKUR EDERIZ"
   },
   {
      "TROCANDO",
	  "IOU:",	  
	  "POR FAVOR PEGUE O TROCO",
	  "OBRIGADO",
	  "   OBRIGADO"
   },
   {
      "Changement en cours",	  
	  "Appelez le service technique SVP:",
	  "Prenez votre monnaie SVP",
	  "Merci",
	  "   Merci!"
   }
};




