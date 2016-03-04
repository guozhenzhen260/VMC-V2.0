/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        ÏµÍ³²ÎÊı¶¨Òå¼°Í¨ÓÃº¯Êı²¿·Ö                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __COMMON_H 
#define __COMMON_H

#define MAXLANGUAGETYPE 6 //ÏµÍ³Ö§³Ö×î´óµÄÓïÑÔÖÖÀà£¬Ä¿Ç°Ôİ¶¨5ÖÖ£¬0ÖĞÎÄ£¬1Ó¢ÎÄ£¬2ÍÁ¶úÆä£¬3¶íÂŞË¹£¬4ÆÏÌÑÑÀ,5·¨Óï

#define OFF_COINACCEPTER         0//¹ØÓ²±ÒÆ÷
#define PARALLEL_COINACCEPTER    1//²¢ĞĞÂö³åÓ²±ÒÆ÷
#define MDB_COINACCEPTER         2//MDBÓ²±ÒÆ÷
#define SERIAL_COINACCEPTER      3//´®ĞĞÂö³åÓ²±ÒÆ÷

#define OFF_BILLACCEPTER         0//¹ØÖ½±ÒÆ÷
#define MDB_BILLACCEPTER         2//MDBÖ½±ÒÆ÷

#define NO_ITLBILLACCEPTER       0//²»ÊÇITLÖ½±ÒÆ÷
#define ITL_BILLACCEPTER         1//ÊÇITLÖ½±ÒÆ÷

#define OFF_BILLRECYCLER         0//¹ØÑ­»·Ö½±ÒÕÒÁãÆ÷
#define MDB_BILLRECYCLER         2//MDBÑ­»·Ö½±ÒÕÒÁãÆ÷
#define FS_BILLRECYCLER            3//¸»Ê¿Ö½±ÒÕÒÁãÆ÷

#define OFF_CHANGER       		 0//¹ØÕÒÁãÆ÷
#define HOPPER_CHANGER           1//HopperÕÒÁãÆ÷
#define MDB_CHANGER              2//MDBÕÒÁãÆ÷

#define OFF_READERACCEPTER		 0//¹Ø¶Á¿¨Æ÷
#define MDB_READERACCEPTER       2//MDB¶Á¿¨Æ÷

#define OFF_PC			         0//¹ØPCÍ¨Ñ¶
#define ZHIHUI_PC			     1//Ò»ÃùÖÇ»ÛpcÍ¨Ñ¶
#define UBOX_PC			         2//ÓÑ±¦pcÍ¨Ñ¶
#define GPRS_PC					 3 //GPRS
#define SIMPUBOX_PC			     4//ĞÂÓÑ±¦pcÍ¨Ñ¶
#define CRUBOX_PC			     5//ÓÑ±¦³ÉÈËpcÍ¨Ñ¶

#define OFF_KEY			         0//ÆÕÍ¨¼üÅÌ°´¼ü
#define SELECT_KEY			     1//Ñ¡»õ°´¼ü

#define TEMPERATURE			     1//¿ªÆô¼ÓÈÈÄ£¿é

#define OFF_GEZI         		 0//¹Ø¸ñ×Ó¹ñ
#define SERIAL_GEZI    			 1//´®¿Ú¸ñ×Ó¹ñ
#define MDB_GEZI         		 2//MDB¸ñ×Ó¹ñ


extern RTC_DATE RTCData;


/******************************************ÆÁÄ»ÏÔÊ¾ĞĞÊı*****************************************************/
#define LINE1                    0
#define LINE2                    1
#define LINE3                    2
#define LINE4                    3
#define LINE5                    4
#define LINE6                    5
#define LINE7                    6
#define LINE8                    7
#define LINE9                    8
#define LINE10                   9
#define LINE11                   10
#define LINE12                   11
#define LINE13                   12
#define LINE14                   13
#define LINE15                   14
#define LINE16                   15

/******************************************¹¤³ÌÏµÍ³²ÎÊı*****************************************************/
typedef struct	
{
	uint8_t		SoftwareVersion;				//Èí¼ş°æ±¾
	uint8_t		HardwareVersion;				//Ó²¼ş°æ±¾
	uint8_t     Language;						//ÓïÑÔÑ¡Ôñ	
	uint8_t		MaintainPassword[6];			//Î¬»¤ÃÜÂë
	uint8_t		AdminPassword[8];				//¹ÜÀíÔ±ÃÜÂë
	uint8_t		MechineSerialNo[10];			//Éè±¸ĞòÁĞºÅ
	uint8_t		MechineDeviceType[6];			//Éè±¸ĞÍºÅ,ÈçEV7636			31
	//Ó²±ÒÆ÷,ÈçMDBÓ²±ÒÆ÷¡¢´®ĞĞÂö³åÓ²±ÒÆ÷
	uint8_t		CoinAcceptorType;				//Ó²±Ò½ÓÊÕÆ÷ÀàĞÍ
	//ÕÒÁãÆ÷,ÈçMDBÕÒÁãÆ÷£¬Hopper
	uint8_t		CoinChangerType;				//Ó²±ÒÕÒÁãÆ÷ÀàĞÍ
	//Ö½±ÒÆ÷,ÈçMDBÖ½±ÒÆ÷Æ÷£¬SSP½Ó¿ÚÖ½±ÒÆ÷
	uint8_t		BillValidatorType;				//Ö½±Ò½ÓÊÕÆ÷ÀàĞÍ
	uint8_t		BillITLValidator;				//Õë¶ÔITLÖ½±Ò½ÓÊÕÆ÷,ÒòÎªÔİ´æÊ±ĞèÒª²»¶Ï·¢pollÖ¸Áî,Òò´Ë³ö»õÊ±¸ù¾İÇé¿öÒªÍË³öÔİ´æÖ½±ÒÀàĞÍ
	//Ö½±ÒÕÒÁã,ÈçMDB ÕÒ±ÒÆ÷
	uint8_t		BillRecyclerType;				//Ö½±ÒÕÒ±ÒÆ÷ÀàĞÍ
	//·ÇÏÖ½ğÉè±¸£¬ÈçMDB¶Á¿¨Æ÷£¬¹«½»RFID¿¨µÈ
	uint8_t		CashlessDeviceType;				//·ÇÏÖ½ğÉè±¸ÀàĞÍ
	uint8_t     DecimalNum;						//ÏÔÊ¾½ğ¶î½çÃæµÄĞ¡ÊıµãÎ»Êı,2·Ö,1½Ç,0Ôª
	//Í¨Ñ¶Éè±¸£¬ÈçMDB Communications Gateway
	uint8_t		DecimalNumExt;					//Ğ¡ÊıµãÎ»ÊıÀ©Õ¹,Ç°ÌáDecimalNum±ØĞëÎª0; 0ÊÇÔª,1ÊÇ10Ôª,2ÊÇ100Ôª,3ÊÇ1000Ôª,4ÊÇ10000Ôª
	//ÑéÖ¤Ä£¿é£¬ÈçMDB Age Verification Device
	uint8_t		GeziDeviceType;				    //1´ú±íÖ»ÓĞ¸ñ×Ó¹ñ£¬Ã»ÓĞÖ÷¸±»õµÀ¹ñ¡£0ÊÇÓĞÖ÷¸±»õµÀ¹ñ
	//ÏÔÊ¾Éè±¸£¬ÈçÁ½ĞĞÆÁ¡¢ËÄĞĞÆÁ¡¢µãÕóÆÁ
	uint8_t 	DisplayMoudleType;				//ÏÔÊ¾Æ÷ÀàĞÍ
	uint8_t     Logo;							//LogoÑ¡Ôñ	
	//Ñ¡»õ°´¼ü°å
	uint8_t		UserSelectKeyBoard;				//ÓÃ»§Ñ¡»õ°´¼ü				42
	uint8_t     GOCIsOpen;						//³ö»õÈ·ÈÏ¿ªÆô±êÖ¾
	uint8_t		SaleTime;                       //½»Ò×µ¹¼ÆÊ±
	uint8_t		PcEnable;                       //¿ªÆôpcÍ¨Ñ¶
	//Ö÷¹ñµÄ»õµÀÅÅÁĞÀàĞÍ£º0¡ª¡ª8x8   1¡ª¡ª8x10
	uint8_t     ChannelType;					//Ö÷¹ñ»õµÀÀàĞÍ
	//ÊÇ·ñ¿ªÆô¸±¹ñ±êÖ¾   1¡ª¡ª¿ªÆô£¬0¡ª¡ª¹Ø±Õ
	uint8_t     SubBinOpen;						//ÊÇ·ñ¿ªÆô¸±¹ñ
	//¸±¹ñµÄ»õµÀÅÅÁĞÀàĞÍ:0¡ª¡ª8x8    1¡ª¡ª8*10
	uint8_t     SubBinChannelType;				//¸±¹ñ»õµÀÅÅÁĞÀàĞÍ
	uint8_t     CunhuoKou;				        //³ö»õ¿Û´æ»õÊıÁ¿1¿Û³ı,0²»¿Û³ı
	uint8_t     EasiveEnable;				    //ÔÚPcEnable=2,´ú±íÊ¹ÓÃÓÑ±¦Ğ­ÒéµÄÊ±ºò£¬Õâ¸ö=1,´ú±í×ªÎª¿¥ÅôĞ­Òé
	uint32_t    MaxValue;				    	//Ñ¡ÔñµÄ×î¸ß½»Ò×ÉÏÏŞ
	uint8_t 	Channel;		                //Ö÷¹ñ»õµÀÖÖÀà 0 :¹Ø±Õ1 µ¯»É 2ÀÏÊ½Éı½µ»ú3ĞÂÊ½Éı½µ»ú+´«ËÍ4Éı½µ»ú+µ¯»É
	uint8_t		SubChannel;                     //¸±¹ñ»õµÀÖÖÀà0 :¹Ø±Õ1 µ¯»É 2ÀÏÊ½Éı½µ»ú3ĞÂÊ½Éı½µ»ú+´«ËÍ4Éı½µ»ú+µ¯»É
	//Ö½±ÒÆ÷ÃæÖµ
	uint32_t BillValue[8];//¸÷¸öÍ¨µÀÃæÖµ	
	//Ó²±ÒÆ÷ÃæÖµ
	uint32_t CoinValue[8];	     //¸÷¸öÍ¨µÀÃæÖµ
	//HopperÕÒÁãÆ÷ÃæÖµ
	uint32_t HopperValue[3];	     //¸÷¸öÍ¨µÀÃæÖµ
	//½»Ò×¼ÇÂ¼´æ´¢×î¸ßÒ³
	//uint8_t		VendingLogMaxPage[2];			//½»Ò×¼ÇÂ¼´¢´æ×î¸ßÒ³,¹Ì¶¨Îª4000
	//Ö¸¶¨½»Ò××Ü¼ÇÂ¼´æ´¢Ò³Ãæ
	//uint8_t		VendingLogTotalPage[2];			//Ö¸¶¨½»Ò×Í³¼Æ×Ü¼ÇÂ¼µÄ´æ´¢Ò³Ãæ(4060~4080) 		46
	//Reserve
	//uint8_t		ReserveByte[466];				//±£Áô×Ö½Ú(0x0Ìî³ä)£¬ºóĞøÀ©Õ¹				 	510
	//================add by yoc 2013.12.16=============================
	uint32_t  macSN; //»úÆ÷ĞÍºÅ
	uint16_t  armSN[2];//Ö÷¿Ø°å°æ±¾ºÅ
	uint8_t   vmcType; //»úÆ÷ÀàĞÍ 0X01ÒûÁÏ»ú£¬0X11Ê³Æ·»ú£¬0X21
	uint8_t   vmcRow;//»õµÀÅÅÊı
	uint8_t   vmcColumn; //»õµÀÁĞÊı	
	uint8_t   aisleSize; //»õµÀ×ÜÊı
	uint8_t   threeSelectKey;//ÊÇ·ñÊ¹ÓÃÈıÌõµÄÑ¡»õ°´¼ü°å,1ÊÇÊ¹ÓÃÈıÌõµÄ,0ÊÇÊ¹ÓÃ6ÌõµÄ
	uint8_t   hefangGui;//ÊÇ·ñ¿ªÆôºĞ·¹¹ñ
	uint8_t   hefangGuiKou;	//ºĞ·¹¹ñ³ö»õ¿Û´æ»õÊıÁ¿1¿Û³ı,0²»¿Û³ı;
	//Ö½±ÒÆ÷ÕÒ±ÒÃæÖµ
	uint32_t RecyclerValue[7];//¸÷¸öÍ¨µÀÃæÖµ
	uint32_t  RecyclerMoney; //Ö½±ÒÆ÷ÕÒ±ÒÃæÖµ
	uint8_t   XMTTemp;//ÊÇ·ñÊ¹ÓÃÎÂ¿ØÆ÷,1ÊÇÊ¹ÓÃ
	uint32_t  BillEnableValue;				   //Ñ¡ÔñµÄ¿ªÖ½±ÒÆ÷½»Ò×ÉÏÏŞ,0´ú±íÓÀÔ¶²»¹ØÖ½±ÒÆ÷
	uint8_t   ColumnTime; //ÉÏ±¨ÓÑ±¦×î´ó³ö»õÊ±¼ä	
	uint8_t   HpEmpCoin; //µ±hopperÈ±±ÒÊ±,ÊÇ·ñÆôÓÃÓ²±ÒÆ÷,1ÆôÓÃ,0½ûÓÃ
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé

}	SYSTEMPARAMETER;							//ÏµÍ³²ÎÊı±£´æÔÚSPI_FLASHµÄ4095Ò³¼´×îºóÒ»Ò³
extern SYSTEMPARAMETER SystemPara;


/******************************************Éè±¸¿ØÖÆÊ±¼ä²ÎÊı*****************************************************/
typedef struct	
{
uint8_t DeviceOn;
uint8_t HourOn;
uint8_t	MinOn;
uint8_t HourOff;
uint8_t	MinOff;
uint8_t flag;
uint8_t TimingFlag;
}DEVICECONTROLTIME;

/******************************************¼ÓÈÈÉè±¸¿ØÖÆÊ±¼ä²ÎÊı*****************************************************/
typedef struct	
{
uint8_t DeviceOn;
uint8_t HourOn[5];	 //¿ªÆôĞ¡Ê±
uint8_t	MinOn[5];	//¿ªÆô·Ö
uint8_t HourOff[5]; //¹Ø±ÕĞ¡Ê±
uint8_t	MinOff[5];	//¹Ø±Õ·Ö
uint16_t setTemp[5];//ÉèÖÃÎÂ¶È
uint16_t setTemp1;//ÉèÖÃÎÂ¶È


uint8_t flag;	 //  bit0: 0 ¹Ø±Õ×´Ì¬ 1¿ªÆô×´Ì¬ bit1:0¶¨Ê±×´Ì¬  1·Ç¶¨Ê±×´Ì¬
uint8_t TimingFlag;

uint16_t curTemp;//µ±Ç°ÎÂ¶È


}ST_HOT_DEV;




/******************************************ÓÃ»§ÏµÍ³²ÎÊı*****************************************************/
typedef struct	
{
	uint8_t     		TransMultiMode;				//ÔÊĞí¶à´Î½»Ò×
	uint8_t     		TransEscape;				//Ç¿ÖÆ¹ºÂò
	DEVICECONTROLTIME	CompressorCtr;		  		//Ñ¹Ëõ»ú¿ª¹Ø¿ØÖÆ
	DEVICECONTROLTIME	LEDCtr;		  				//Õ¹Ê¾µÆ¿ª¹Ø¿ØÖÆ
	DEVICECONTROLTIME   CompressorCtr2Bin;			//2¹ñÑ¹Ëõ»ú¿ØÖÆÊ±¼ä
	DEVICECONTROLTIME	LEDCtr2Bin;					//2¹ñÕ¹Ê¾µÆ¿ØÖÆÊ±¼ä
	ST_HOT_DEV			box1Hot;					//1¹ñ¼ÓÈÈ
	ST_HOT_DEV			box2Hot;					//2¹ñ¼ÓÈÈ
	DEVICECONTROLTIME	box1Chuchou;				//1¹ñ³ı³ô
	DEVICECONTROLTIME	box2Chuchou;				//2¹ñ³ı³ô

	//´ò¿ªµ÷ÊÔĞÅÏ¢
	uint8_t     		billTrace;				
	uint8_t     		coinTrace;				
	uint8_t     		changelTrace;				
	uint8_t     		readerTrace;				
	uint8_t     		channelTrace;				
	uint8_t     		compressorTrace;				
	uint8_t     		ledTrace;	
	uint8_t     		selectTrace;
	uint8_t     		PC;
	uint8_t     		TraceFlag;		//add by yoc 2013.10.16
	uint8_t     		BillRecyclerTrace;
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé
	

}	SYSUSERPARAMETER;							//ÏµÍ³²ÎÊı±£´æÔÚSPI_FLASHµÄ4095Ò³¼´×îºóÒ»Ò³
extern SYSUSERPARAMETER UserPara;


/******************************************×Ü½»Ò×ÈÕÖ¾²ÎÊı*****************************************************/
typedef struct	
{
	//ÇøÓòÏúÊÛÍ³¼Æ
	uint32_t	Income;		  					//×ÜÊÕ±ÒÖµ
	uint32_t	NoteIncome;	  					//Ö½±Ò×ÜÊÕ±ÒÖµ
	uint32_t	CoinsIncome;  					//Ó²±Ò×ÜÊÕ±ÒÖµ
	uint32_t	TotalTrans;	  					//½»Ò××Ü½ğ¶î
	uint32_t	TotalChange;  					//ÕÒÁã×Ü½ğ¶î
	uint32_t	CoinsChange[3];	 				//ÕÒÁãÆ÷³ö±ÒÊı
	uint32_t	SuccessNum;	  					//³É¹¦½»Ò××Ü´ÎÊı
	uint32_t	DoubtNum;	  					//ÒÉÎÊ½»Ò××Ü´ÎÊı
	uint32_t		Iou;	  					//Ç··Ñ×Ü¶î


	//PCÍ¨Ñ¶ÓÃ
	uint32_t   	vpSuccessNum;                    //È«²¿³ö»õÊıÁ¿
	uint32_t   	vpSuccessMoney;                  //È«²¿³ö»õ½ğ¶î
	uint32_t    vpCashNum;                       //ÏÖ½ğ³ö»õÊıÁ¿
	uint32_t    vpCashMoney;                     //ÏÖ½ğ³ö»õ½ğ¶î	
	uint32_t    vpGameNum;                 		 //ÓÎÏ·³ö»õÊıÁ¿ 
	uint32_t    vpGameMoney;              		 //ÓÎÏ·³ö»õ½ğ¶î
	//uint32_t	CardNum;  						//Ò»¿¨Í¨½»Ò×´ÎÊı
	//uint32_t	CardIncome;  					//Ò»¿¨Í¨Ïû·Ñ½ğ¶î
	uint32_t    vpCardNum;                       //ËùÓĞ¿¨³ö»õÊıÁ¿
	uint32_t    vpCardMoney;                     //ËùÓĞ¿¨³ö»õ½ğ¶î
	uint32_t    vpOnlineNum;                     //ÔÚÏß³ö»õÊıÁ¿
	uint32_t    vpOnlineMoney;                  //ÔÚÏß³ö»õ½ğ¶î
	
	//×ÜÏúÊÛÍ³¼Æ
	uint32_t	IncomeTotal;		  			//×ÜÊÕ±ÒÖµ
	uint32_t	NoteIncomeTotal;	  			//Ö½±Ò×ÜÊÕ±ÒÖµ
	uint32_t	CoinsIncomeTotal;  				//Ó²±Ò×ÜÊÕ±ÒÖµ
	uint32_t	TotalTransTotal;	  			//½»Ò××Ü½ğ¶î
	uint32_t	TotalChangeTotal;  				//ÕÒÁã×Ü½ğ¶î
	uint32_t	CoinsChangeTotal[3];	 		//ÕÒÁãÆ÷³ö±ÒÊı
	//PCÍ¨Ñ¶ÓÃ
	uint32_t   	vpSuccessNumTotal;              //È«²¿³ö»õÊıÁ¿
	uint32_t   	vpSuccessMoneyTotal;            //È«²¿³ö»õ½ğ¶î
	uint32_t    vpCashNumTotal;                 //ÏÖ½ğ³ö»õÊıÁ¿
	uint32_t    vpCashMoneyTotal;               //ÏÖ½ğ³ö»õ½ğ¶î
	uint32_t    vpGameNumTotal;                 //ÓÎÏ·³ö»õÊıÁ¿ 
	uint32_t    vpGameMoneyTotal;               //ÓÎÏ·³ö»õ½ğ¶î
	uint32_t    vpCardNumTotal;                 //Ë¢¿¨³ö»õÊıÁ¿
	uint32_t    vpCardMoneyTotal;               //Ë¢¿¨³ö»õ½ğ¶î
	uint32_t    vpOnlineNumTotal;               //ÔÚÏß³ö»õÊıÁ¿
	uint32_t    vpOnlineMoneyTotal;             //ÔÚÏß³ö»õ½ğ¶î

	//add by yoc zhihui 2013.12.16
	uint32_t    vpPC2NumTotal;               //»¥ÁªÍøÌá»õ£¬°üÀ¨Ö§¸¶±¦Ìá»õ
	uint32_t    vpPC2MoneyTotal;            
	uint32_t    vpPC3NumTotal;               //ÆäËûPCÖ§¸¶·½Ê½µÄ»ã×ÜĞÅÏ¢
	uint32_t    vpPC3MoneyTotal; 
	uint32_t	vpOneCardNumTotal;  		//Ò»¿¨Í¨½»Ò×´ÎÊı 
	uint32_t	vpOneCardMoneyTotal;  		//Ò»¿¨Í¨Ïû·Ñ½ğ¶î
	uint32_t    vpUCardNumTotal;             //ÒøÁª¿¨ ½»Ò×´ÎÊı 
	uint32_t    vpUCardMoneyTotal;             //ÒøÁª¿¨Ïû·Ñ½ğ¶î
	
	//------------------------------------------------
	//ÀëÏßÏúÊÛÊı¾İ
	uint8_t	 offLineFlag;                   //ÊÇ·ñÀëÏß´¦Àí 1ÀëÏßÄ£Ê½,0ÔÚÏßÄ£Ê½ 
	uint32_t   offLineNum;                    //ÀëÏß³ö»õÊıÁ¿
	uint32_t   offLineMoney;                  //ÀëÏß³ö»õ½ğ¶î
	uint16_t	offDetailPage;				  //ÀëÏßÏêÏ¸½»Ò×ÈÕÖ¾Ë÷ÒıÒ³
	//ÏêÏ¸½»Ò×ÈÕÖ¾Ë÷ÒıÒ³
	uint16_t	LogDetailPage;					//ÏêÏ¸½»Ò×ÈÕÖ¾Ë÷ÒıÒ³
	uint8_t    run_no;
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé	

}	LOGPARAMETER;							
extern LOGPARAMETER LogPara;

/******************************************½»Ò×ÈÕÖ¾Ã÷Ï¸²ÎÊı*****************************************************/
typedef struct	
{
	uint16_t		BeginYear;		//¿ªÊ¼½»Ò×Äê
	uint8_t			BeginMonth;		//¿ªÊ¼½»Ò×ÔÂ
	uint8_t			BeginDay;		//¿ªÊ¼½»Ò×ÈÕ
	uint8_t			BeginHour;		//¿ªÊ¼½»Ò×Ê±
	uint8_t			BeginMinute;	//¿ªÊ¼½»Ò×·Ö
	uint8_t			EndMinute;		//½áÊø½»Ò×·Ö
	//add by yoc 2013.9.11
	uint8_t			BeginSec;	//¿ªÊ¼½»Ò×Ãë
	uint8_t			EndSec;		//½áÊø½»Ò×Ãë
	
	uint16_t		IncomeBill;		//Í¶ÈëÖ½±Ò
	uint16_t		IncomeCoin;		//Í¶ÈëÓ²±Ò
	uint32_t		IncomeCard;		//Ë¢¿¨ÊÕÈë

	char			ColumnNo[24];	//³ö»õ»õµÀ±àºÅ
	uint8_t			GoodsNo[8];		//³ö»õÉÌÆ·±àºÅ
	uint32_t		PriceNo[8];      //³ö»õÉÌÆ·µ¥¼Û
	uint8_t			TransSucc[8];    //³ö»õ½á¹û³É¹¦»¹ÊÇÊ§°Ü,1³É¹¦,0Ê§°Ü
	/*************add by yoc 2013.9.11***********************/
	uint8_t			PayMode[8];//µ¥´Î½»Ò×ÀàĞÍ
	uint8_t			GoodsMin[8];//µ¥´Î³ö»õ·Ö Îª16½øÖÆ·ÇBCDÂë ÏÂÍ¬
	uint8_t			GoodsSec[8];//µ¥´Î³ö»õÃë	
	uint8_t 		run_no[8];//Á÷Ë®ºÅ
	uint8_t			TradeNum;//¶à´Î¹ºÂò´ÎÊı
	
	/*******************end add*************************/
	uint16_t		Change;			//ÕÒÁã
	//CRCCheck
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé	
}	LOGPARAMETERDETAIL;							
extern LOGPARAMETERDETAIL LogParaDetail;



/******************************************Ö½±ÒÆ÷Ó²±ÒÆ÷Éè±¸²ÎÊı*****************************************************/
struct STDevValue
{
    //Ö½±ÒÆ÷Ïà¹Ø²ÎÊı
    uint8_t  BillLevel;			  //Ö½±ÒÆ÷µÈ¼¶
	uint16_t BillCode;		  //¹ú¼Ò´úÂë
	uint32_t BillScale;			  //±ÈÀıÒò×Ó
	uint16_t BillDecimal;		  //10^Ğ¡ÊıÎ»Êı
	uint16_t BillStkCapacity;	  //´¢±ÒÈİÁ¿
	uint16_t BillSecurity;		  //°²È«µÈ¼¶
	uint8_t  BillEscrowFun;		  //Ôİ´æ¹¦ÄÜ
	uint32_t BillValue[16];//¸÷¸öÍ¨µÀÃæÖµ
	uint8_t billIDENTITYBuf[36];  //Identification
	
	//Ó²±ÒÆ÷Ïà¹Ø²ÎÊı
	uint32_t CoinValue[16];	     //¸÷¸öÍ¨µÀÃæÖµ
	uint8_t  CoinLevel;			 //Ó²±ÒÆ÷µÈ¼¶
	uint16_t CoinCode;			 //¹ú¼Ò´úÂë
	uint32_t  CoinScale;		 //±ÈÀıÒò×Ó
	uint16_t CoinDecimal;		 //10^Ğ¡ÊıÎ»Êı
	uint16_t CoinRouting;		 //Bit is set to indicate a coin type can be routed to the tube
	uint16_t CoinNum[16];	     //¸÷¸öÍ¨µÀ´æ±ÒÊıÁ¿
	uint8_t coinIDENTITYBuf[36];//Identification
	
	//HopperÕÒÁãÆ÷Ïà¹Ø²ÎÊı
	//uint32_t HopperValue[3];	     //¸÷¸öÍ¨µÀÃæÖµ

	//¶Á¿¨Æ÷Ïà¹Ø²ÎÊı
	uint8_t  ReaderLevel;			  //¶Á¿¨Æ÷µÈ¼¶
	//uint16_t ReaderCode;		  //¹ú¼Ò´úÂë
	uint8_t  ReaderScale;			  //±ÈÀıÒò×Ó
	uint16_t ReaderDecimal;		  //10^Ğ¡ÊıÎ»Êı
	uint8_t  ReaderRecTime;		  //×î´ó»ØÓ¦Ê±¼ä
	uint8_t  ReaderMiscelOpt;	  //ÊÇ·ñÖ§³ÖÆäËû¹¦ÄÜ£¬±ÈÈçÔÊĞíÍË»ØÇ®µÈµÈ
	uint8_t  ReaderCashSale;	  //1ÔÊĞíÏÖ½ğÏêÏ¸½»Ò×¼ÇÂ¼¶¼Ìá½»¸øËû

	//Ö½±ÒÑ­»·ÕÒÁãÆ÷
	uint32_t RecyclerValue[7];    //¸÷¸öÍ¨µÀÃæÖµ
	uint16_t RecyclerNum[7];	  //¸÷¸öÍ¨µÀ´æ±ÒÊıÁ¿
};
extern struct STDevValue stDevValue;

/******************************************Õ¹Ê¾Î»°´¼üÉè±¸²ÎÊı*****************************************************/
typedef struct	
{
	//Ñ¡»õ°´¼ü;by gzz 20121025
	uint8_t  sel1ReadyLed;               //°´¼ü°å1µÄ¡ÌµÆ
	uint8_t  sel1ErrLed;                 //°´¼ü°å1µÄ¡ÁµÆ
	uint8_t  sel2ReadyLed;               //°´¼ü°å2µÄ¡ÌµÆ
	uint8_t  sel2ErrLed;                 //°´¼ü°å2µÄ¡ÁµÆ
	uint8_t  sel3ReadyLed;               //°´¼ü°å3µÄ¡ÌµÆ
	uint8_t  sel3ErrLed;                 //°´¼ü°å3µÄ¡ÁµÆ
	uint8_t  sel4ReadyLed;               //°´¼ü°å4µÄ¡ÌµÆ
	uint8_t  sel4ErrLed;                 //°´¼ü°å4µÄ¡ÁµÆ
	uint8_t  sel5ReadyLed;               //°´¼ü°å5µÄ¡ÌµÆ
	uint8_t  sel5ErrLed;                 //°´¼ü°å5µÄ¡ÁµÆ
	uint8_t  sel6ReadyLed;               //°´¼ü°å6µÄ¡ÌµÆ
	uint8_t  sel6ErrLed;                 //°´¼ü°å6µÄ¡ÁµÆ
}SELECTKEYAPI;
extern SELECTKEYAPI selectKey;


/******************************************Ö½±ÒÆ÷Ó²±ÒÆ÷Í¨Ñ¶½á¹¹Ìå*****************************************************/
//Ö½±ÒÆ÷ÊÕµ½Ö½±Ò·µ»ØÃæÖµ
#define MBOX_BILLMONEY					0x01
//½ûÄÜÖ½±ÒÆ÷ÃüÁî
#define MBOX_BILLDISABLEDEV				0x02
//Ê¹ÄÜÖ½±ÒÆ÷ÃüÁî
#define MBOX_BILLENABLEDEV				0x03
//ÖØÖÃÖ½±ÒÆ÷ÃüÁî
#define MBOX_BILLRESETDEV				0x04
//Æô¶¯Ö½±ÒÆ÷ÃüÁî
#define MBOX_BILLINITDEV				0x05
//Ö½±ÒÆ÷Ñ¹³­ÃüÁî
#define MBOX_BILLESCROW					0x06
//·µ»ØÖ½±ÒÆ÷Ñ¹³­³É¹¦ÃüÁî
#define MBOX_BILLESCROWSUCC				0x07
//·µ»ØÖ½±ÒÆ÷Ñ¹³­Ê§°ÜÃüÁî
#define MBOX_BILLESCROWFAIL				0x08
//Ö½±ÒÆ÷ÍË±ÒÃüÁî
#define MBOX_BILLRETURN					0x09
//·µ»ØÖ½±ÒÆ÷ÍË±Ò³É¹¦ÃüÁî
#define MBOX_BILLRETURNSUCC				0x0A
//·µ»ØÖ½±ÒÆ÷ÍË±ÒÊ§°ÜÃüÁî
#define MBOX_BILLRETURNFAIL				0x0B

//·µ»ØÖ½±ÒÆ÷²ÎÊıÃüÁî
#define MBOX_BILLSETUPDEV				0x0C


//Ó²±ÒÆ÷ÊÕµ½Ó²±Ò·µ»ØÃæÖµ
#define MBOX_COINMONEY					0x0D
//½ûÄÜÓ²±ÒÆ÷ÃüÁî
#define MBOX_COINDISABLEDEV				0x0E
//Ê¹ÄÜÓ²±ÒÆ÷ÃüÁî
#define MBOX_COINENABLEDEV				0x0F
//Æô¶¯Ó²±ÒÆ÷ÃüÁî
#define MBOX_COININITDEV				0x10
//·µ»ØÓ²±ÒÆ÷°´ÏÂÍË±Ò¼üÍË±ÒÇëÇóÃüÁî
#define MBOX_COINRETURN					0x11
//·µ»ØÓ²±ÒÆ÷²ÎÊıÃüÁî
#define MBOX_COINSETUPDEV				0x12


//¶ÔÓÚlevel3µÄmdbÕÒÁãÆ÷ÕÒÁãÃüÁî
#define MBOX_MDBCHANGELEVEL3			0x13
//¶ÔÓÚlevel2µÄmdbÕÒÁãÆ÷ÕÒÁãÃüÁî
#define MBOX_MDBCHANGELEVEL2			0x14
//¶ÔÓÚµÃµ½ÕÒÁãÆ÷Óà±ÒÁ¿
#define MBOX_MDBCHANGETUBE				0x15
//¶ÔÓÚhopperµÄÕÒÁãÆ÷ÕÒÁãÃüÁî
#define MBOX_MDBCHANGEHOPPER			0x16
//ÕÒÁãÆ÷ÕÒÁã·µ»ØÃüÁî
#define MBOX_CHANGERBACKMONEY			0x17

//¶ÔÓÚHopperµÄÕÒÁãÃüÁî
#define MBOX_HOPPEROUT              	0x18
//¶ÔÓÚHopperµÄÕÒÁãÆ÷ÕÒÁã·µ»ØÃüÁî
#define MBOX_HOPPERBACKMONEY			0x19
//Æô¶¯Ê±¼à²âHopper×´Ì¬
#define MBOX_HOPPERINITSTATE            0x1A


//¶Á¿¨Æ÷ÉÏ´«¸øvmc
//¶Á¿¨Æ÷ÊÕµ½½ğ¶î·µ»ØÃæÖµ
#define MBOX_READERMONEY				0x1B
//¶Á¿¨Æ÷ÔÊĞí³ö»õ
#define MBOX_READERVENDOUT  			0x1C
//¶Á¿¨Æ÷¾Ü¾ø³ö»õ
#define MBOX_READERVENDDENY  			0x1D
//¶Á¿¨Æ÷°Î¿¨
#define MBOX_READERMONEYOUT				0x1E


//vmcÏÂ·¢¸ø¶Á¿¨Æ÷
//½ûÄÜ¶Á¿¨Æ÷ÃüÁî
#define MBOX_READERDISABLEDEV			0x1F
//Ê¹ÄÜ¶Á¿¨Æ÷ÃüÁî
#define MBOX_READERENABLEDEV			0x20
//Æô¶¯¶Á¿¨Æ÷ÃüÁî
#define MBOX_READERINITDEV				0x21
//¶Á¿¨Æ÷¿ÛÇ®ÃüÁî
#define MBOX_READERVENDREQUEST			0x22
//½»Ò×³É¹¦ÃüÁî
#define MBOX_READERVENDSUCCESS			0x23
//½»Ò×Ê§°ÜÃüÁî
#define MBOX_READERVENDFAIL 			0x24
//½áÊø½»Ò×Á÷³ÌÃüÁî
#define MBOX_READERVENDCOMP 			0x25
//¶Á¿¨Æ÷¼ÇÂ¼ÏÖ½ğÏêÏ¸½»Ò×¼ÇÂ¼ÃüÁî
#define MBOX_READERCASHSALE 			0x26


//Æô¶¯Ö½±ÒÑ­»·ÕÒÁãÆ÷ÃüÁî
#define MBOX_BILLRECYCLERDEV			0x27
//Ö½±ÒÆ÷°´ÕÅÊıÕÒ±Ò
#define MBOX_BILLRECYPAYOUTNUM			0x28
//Ö½±ÒÆ÷°´½ğ¶îÕÒ±Ò
#define MBOX_BILLRECYPAYOUTVALUE		0x29
//·µ»ØÖ½±ÒÆ÷ÕÒÁã³É¹¦ÃüÁî
#define MBOX_BILLRECYPAYOUTSUCC			0x2A
//·µ»ØÖ½±ÒÆ÷ÕÒÁãÊ§°ÜÃüÁî
#define MBOX_BILLRECYPAYOUTFAIL			0x2B




typedef struct
{
	//Ö½±ÒÆ÷
    uint8_t   BillCmd;						//Í¶±ÒÃüÁîÀàĞÍ
	uint32_t  BillMoney;					//Í¶±Ò½ğ¶î
	uint8_t   BillBack;						//¿ØÖÆÃüÁîÀàĞÍ
	uint8_t   BillBackCmd;					//Ö½±ÒÆ÷ÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ

	//Ó²±ÒÆ÷
    uint8_t   CoinCmd;						//Í¶±ÒÃüÁîÀàĞÍ
	uint32_t  CoinMoney;					//Í¶±Ò½ğ¶î
	uint8_t   CoinBack;						//¿ØÖÆÃüÁîÀàĞÍ
	uint8_t   CoinBackCmd;					//Ó²±ÒÆ÷ÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ

	//ÕÒÁãÆ÷
	uint8_t   PayoutCmd;					//ÕÒÁãÃüÁîÀàĞÍ,°üÀ¨MDB,Hopper
	uint32_t  PayoutMoney;					//ÕÒÁãÃüÁî½ğ¶î
	uint32_t  PayoutType;					//ÕÒÁãÃüÁîÍ¨µÀ,°üÀ¨MDB,Hopper
	uint32_t  PayoutNum;					//ÕÒÁãÃüÁîÃ¶Êı,°üÀ¨MDB,Hopper
	uint8_t   PayoutBackCmd;				//ÕÒÁã·µ»ØÃüÁîÀàĞÍ,°üÀ¨MDB,Hopper
	uint32_t  PayoutBackNum[16];			//MDBÕÒÁã·µ»Ø¸÷Í¨µÀÃ¶Êı
	uint8_t   SubBackType;					//HopperÕÒÁã·µ»ØÃüÁîÍ¨µÀ
	uint32_t  BkPackValue;					//HopperÕÒÁã·µ»ØÍ¨µÀÃ¶Êı

	//¶Á¿¨Æ÷
	//ÉÏ±¨²å¿¨Ö¸Áî
    uint8_t   ReaderCmd;					//²å¿¨,¶Á¿¨Æ÷ÉÏ´«¸øvmc²å¿¨ÃüÁîÀàĞÍ
	uint32_t  ReaderMoney;					//²å¿¨½»Ò×½ğ¶î
	
	//vmcÏòreader·¢ËÍÃüÁî
	uint8_t   ReaderBack;					//vmcÏÂ·¢¸ø¶Á¿¨Æ÷¿ØÖÆÃüÁîÀàĞÍ
	uint16_t  ReaderPrice;					//ÏúÊÛµ¥¼Û
	//reader·µ»ØÃüÁî
	uint8_t   ReaderBackCmd;				//¶Á¿¨Æ÷ÉÏ´«¸øvmcÃüÁîÀàĞÍ
	//»õµÀÖ¸Áî²ÎÊı
	uint8_t   Binnum;						//»ú¹ñ±àºÅ
	uint8_t   ChannelNum;					//»õµÀ±àºÅ
	uint8_t   GOCFlag;						//³ö»õ¼ì²â¿ªÆô±êÖ¾
	uint8_t   ChnlHandleType;				//»õµÀ²Ù×÷ÀàĞÍ
	uint8_t   HandleResult;					//²Ù×÷½á¹û
	uint8_t   ACDCHandleType;				//Ñ¹Ëõ»ú¿ØÖÆ²Ù×÷ÀàĞÍ
	uint16_t   ACDCData;						//Ñ¹Ëõ»ú²Ù×÷Êı¾İ
	uint8_t   LiftTablePack[10];   //change by yoc  
	uint8_t   LevelNum;						//²ãºÅ
	//ºĞ·¹¹ñÓÊÏäÍ¨ĞÅ²ÎÊı Add by liya 2014-01-20
	uint8_t   HeFanGuiHandle;				//ºĞ·¹¹ñ²Ù×÷ÀàĞÍ
	uint8_t   HeFanGuiNum;					//ºĞ·¹¹ñ¹ñºÅ
	uint8_t   HeFanGuiBuf[36];				//½ÓÊÕµ½µÄ×Ô¶¨ÒåÊı¾İ»º´æ
	uint8_t   HeFanGuiRst;					//ºĞ·¹¹ñ²Ù×÷½á¹û

	//Ö½±ÒÕÒÁãÑ­»·Æ÷
	uint32_t  RecyPayoutMoney;			    //ÕÒÁã½ğ¶î
	uint8_t   RecyPayoutNum;				//ÕÒÁãÊıÁ¿
	uint32_t  RecyPayoutMoneyBack;			//Êµ¼ÊÕÒÁã½ğ¶î
	
} MessagePack;	
extern MessagePack MsgAccepterPack;



extern OS_EVENT	*g_DeviceSem;
extern OS_EVENT *g_BillMoneyQ;
extern OS_EVENT *g_BillMoneyMail;
extern OS_EVENT *g_BillMoneyBackMail;
extern OS_EVENT *g_CoinMoneyBackMail;
extern OS_EVENT *g_CoinMoneyQ;
extern OS_EVENT *g_CoinMoneyMail;
extern OS_EVENT *g_ChangeMoneyMail;
extern OS_EVENT *g_ChangeBackMoneyMail;
extern OS_EVENT *g_ReaderMoneyMail;
extern OS_EVENT *g_ReaderBackMoneyMail;
extern OS_EVENT *g_ReaderBackCmdMail;
extern OS_EVENT *g_ChannelMail;
extern OS_EVENT *g_ChannelBackMail;
//ACDC¿ØÖÆÓÊÏä
extern OS_EVENT *g_ACDCMail;
extern OS_EVENT *g_ACDCBackMail;

extern OS_EVENT *g_LiftTableMail;
extern OS_EVENT *g_LiftTableBackMail;

//ºĞ·¹¹ñ×¨ÓÃÓÊÏä  add by liya 2014-01-20
extern OS_EVENT *g_HeFanGuiMail;
extern OS_EVENT *g_HeFanGuiBackMail;

/******************************************Éè±¸×´Ì¬Í¨Ñ¶½á¹¹Ìå*****************************************************/
//Ö½±ÒÆ÷ÊÕµ½Ö½±Ò·µ»ØÃæÖµ
#define MBOX_GETDEVSTATE					0x01
#define MBOX_RETURNDEVSTATE					0x02

typedef struct
{
	uint8_t   DevStateCmd;						//Í¶±ÒÃüÁîÀàĞÍ
	//»õµÀ°å¹ÊÕÏ
	unsigned char ErrorInit;                    //ÊÇ·ñÕıÔÚ³õÊ¼»¯,0ÕıÔÚ³õÊ¼»¯,1³õÊ¼»¯Íê³É
	//MDBÖ½±ÒÆ÷
	uint8_t BillCommunicate;//1´ú±íÖ½±ÒÆ÷Í¨Ñ¶²»ÉÏ
	uint8_t Billmoto;//1´ú±íÂí´ï¹ÊÕÏ
	uint8_t Billsensor;//1´ú±í´«¸ĞÆ÷¹ÊÕÏ
	uint8_t Billromchk;//1´ú±írom³ö´í
	uint8_t Billjam;//1´ú±íÍ¶±Ò¿¨±Ò
	uint8_t BillremoveCash;//1ÒÆ³ıÖ½±Ò³®Ïä
	uint8_t BillcashErr;//1Ö½±Ò³®Ïä¹ÊÕÏ,ÒÔ¼°³®ÏäÂú
	uint8_t Billdisable;//1Ö½±ÒÒòÎª¸÷ÖÖÔ­Òò±»½ûÄÜÁË
	uint8_t recyErr;//ÕÒÁãÄ£¿éÒì³£

	//MDBÓ²±ÒÆ÷
	uint8_t CoinCommunicate;//1´ú±íÓ²±ÒÆ÷Í¨Ñ¶²»ÉÏ
	uint8_t Coinsensor;//1´ú±í´«¸ĞÆ÷¹ÊÕÏ
	uint8_t Cointubejam;//1´ú±í³ö±Ò¿Ú¿¨±Ò
	uint8_t Coinromchk;//1´ú±írom³ö´í
	uint8_t Coinrouting;//1´ú±í½ø±ÒÍ¨µÀ³ö´í
	uint8_t Coinjam;//1´ú±íÍ¶±Ò¿¨±Ò
	uint8_t CoinremoveTube;//1ÒÆ³ıÓ²±Ò¶·
	uint8_t Coindisable;//1Ó²±ÒÒòÎª¸÷ÖÖÔ­Òò±»vmc½ûÄÜÁË
	uint8_t CoinunknowError;//1Ó²±ÒÆäËû¸÷ÖÖ¹ÊÕÏ
	uint8_t CoinunknowErrorLow;//Ó²±ÒÆäËû¸÷ÖÖ¹ÊÕÏµÄ×Ó×´Ì¬Âë

	//HopperÕÒÁãÆ÷
	uint8_t Hopper1State;//Hopper1×´Ì¬£¬0Õı³££¬1È±±Ò£¬2¹ÊÕÏ
	uint8_t Hopper2State;//Hopper2×´Ì¬£¬0Õı³££¬1È±±Ò£¬2¹ÊÕÏ
	uint8_t Hopper3State;//Hopper3×´Ì¬£¬0Õı³££¬1È±±Ò£¬2¹ÊÕÏ
	uint8_t Hopper1StateChk;//Hopper1¹ÊÕÏÂë
	uint8_t Hopper2StateChk;//Hopper2¹ÊÕÏÂë
	uint8_t Hopper3StateChk;//Hopper3¹ÊÕÏÂë

	//³ö»õ»ú¹¹
	uint8_t GOCError;	    //Ö÷ÏäÌåGOC×´Ì¬£¬0Õı³££¬1¹ÊÕÏ
	uint8_t GOCErrorSub;	//´ÓÏäÌåGOC×´Ì¬£¬0Õı³££¬1¹ÊÕÏ
	uint8_t ColBoardError;	    //Ö÷ÏäÌå»õµÀ°å×´Ì¬£¬0Õı³££¬1¹ÊÕÏ
	uint8_t ColBoardErrorSub;	//´ÓÏäÌå»õµÀ°å×´Ì¬£¬0Õı³££¬1¹ÊÕÏ
	//È¡»õµç»ú
	unsigned char Error_FMD;
	//ÉÏÏÂµç»ú
	unsigned char Error_FUM;
	//³ö»õÈ·ÈÏ¶ÔÉä°å
	unsigned char Error_GOC;
	//²ã¼Ü¼ì²â´«¸ĞÆ÷
	unsigned char Error_FLD;
	//ÆğÊ¼Î»ÖÃ´«¸ĞÆ÷
	unsigned char Error_FOD;
	//ÓÃ»§È¡»õÃÅ¹ÊÕÏ
	unsigned char Error_UDS;
	//»õµÀ°å¹ÊÕÏ
	unsigned char Error_GCD;
	//³ö»õ»ú¹¹Ã¦
	unsigned char Error_SOVB;
	//»úÆ÷´óÃÅ¹ÊÕÏ
	unsigned char Error_SOMD1;
	//ÊÕµ½´íÎóÊı¾İ°ü
	unsigned char Error_SOEC;
	//Éı½µÌ¨ÕıÈ·È¡»õ
	unsigned char Error_SFHG;
	//»õ¹ñÃÅÊÇ·ñ´ò¿ª
	unsigned char Error_SOFP;
	//È¡»õÃÅ
	unsigned char Error_SOMD2;
	//Ö÷ÏäÌåÊÇ·ñÒÑ¾­È«²¿¶¼²»¿ÉÓÃ
	unsigned char Emp_Gol;//1²»¿ÉÓÃ,0¿ÉÓÃ

	//Ö½±ÒÕÒÁãÆ÷
	FS_STATUS  *fsstatus;
}DEVICESTATEPACK;
extern DEVICESTATEPACK DeviceStatePack;
extern DEVICESTATEPACK DeviceStateBusiness;
typedef struct
{
	unsigned char Error_Card;//¿¨¹ÊÕÏ
}CARDSTATEPACK;
extern CARDSTATEPACK CardStatePack;

extern OS_EVENT *g_DeviceStateMail;
extern OS_EVENT *g_DeviceStateBackMail;


/******************************************HopperÍ¨Ñ¶½á¹¹Ìå*****************************************************/
//Èı¸öµØÖ·HopperµÄ³ö±Ò½ğ¶î±£´æµØÖ·
#define ADD_HP1					(32)
#define ADD_HP2					(64)
#define ADD_HP3					(96)
//Èı¸öµØÖ·HopperµÄµ¥Î»½ğ¶î´æ´¢µØÖ·
#define ADD_HPVALUE1			(1)
#define ADD_HPVALUE2			(2)
#define ADD_HPVALUE3			(3)
//µØÖ·Îª00µÄHopperÀàĞÍ
#define EVBDEV_HP1						(1)					
//µØÖ·Îª01µÄHopperÀàĞÍ			
#define EVBDEV_HP2						(2)					
//µØÖ·Îª10µÄHopperÀàĞÍ
#define EVBDEV_HP3						(3)					

//HopperµÄ²éÑ¯²Ù×÷Ö¸Áî
#define EVBHANDLE_HPQUERY				(0x51)
//HopperµÄ³ö±Ò²Ù×÷Ö¸Áî				
#define EVBHANDLE_HPOUTPUT				(0x50)				

struct EvbHopper
{
	//Ó²¼şÁ¬½ÓÉÏµÃË÷Òı±àºÅ
	unsigned char Index;
	//HpÅÅĞòÁĞ±íÖĞµÄ±àºÅ
	unsigned char Num;
	//ÉÏ´ÎÍ¨ĞÅºóµÃµ½µÄ×´Ì¬:0,¿ÉÓÃ;2,È±±Ò;3,¹ÊÕÏ;
	//·µ»ØµÄĞÅÏ¢£º0x00:Õı³£,0x01:Õı³£;0x20:È±±Ò;0x11:³ö±ÒÊ±¿¨±ÒÁË£»0x39:¹â²àÎŞ±ÒÊ±³ö²»ÁË±Ò£»0x28³ö±Ò¹ÊÕÏ,0x41:¹â²àÓĞ±ÒÊ±³ö±Ò³ö²»ÁË±Ò
	unsigned char State;
	//ÃæÖµ
	uint32_t Price;
	//ÎïÀí¹Ì¶¨µØÖ·
	unsigned char	Add; 
	//ÉÏ´Î²Ù×÷³ö±ÒÊıÁ¿
	unsigned char OutPutCount;
	//HpÍ¨Ñ¶¹ÊÕÏ²»¿ÉÓÃ±êÖ¾
	unsigned char DevBadFlag;
	unsigned int MemoryAdd;
	//Hp¹ÊÕÏ´úÂë
	//·µ»ØµÄĞÅÏ¢£º0x00:Õı³£;0x80:Í¨Ñ¶Ê§°Ü;0x01:µ¥¼ÛÎª0;0x20,0x29:È±±Ò;0x11:³ö±ÒÊ±¿¨±ÒÁË£»0x28Ò²ÊÇÒ»ÖÖ¹ÊÕÏ,0x39:¹â²àÎŞ±ÒÊ±³ö²»ÁË±Ò£»0x41:¹â²àÓĞ±ÒÊ±³ö±Ò³ö²»ÁË±Ò
	unsigned char StateChk;
};
//¶¨ÒåÈı¸öµØÖ·HopperµÄ½á¹¹Ìå
extern struct EvbHopper stEvbHp[3];
//HopperÕÒÁãµ¥Î»ÃæÖµ´óĞ¡ÉèÖÃ±êÖ¾£º1£ºÈı¸öHopperµÄÃæÖµ²»ÏàµÈ£»2£º000µØÖ·HopperÓë001µØÖ·HopperµØÖ·ÃæÖµÏàµÈ£¬Óë010²»ÏàµÈ
//							  	  3£º000µØÖ·HopperÓë010µØÖ·HopperµØÖ·ÃæÖµÏàµÈ£¬Óë001²»ÏàµÈ
//							  	  4£º001µØÖ·HopperÓë010µØÖ·HopperµØÖ·ÃæÖµÏàµÈ£¬Óë000²»ÏàµÈ	
//							  	  5£ºÈı¸öHopperµÄÃæÖµÏàµÈ
extern unsigned char HpValueFlag;


//add by yoc 2013.12.16
/***********************************Ò»ÃùÖÇ»Û*PCÍ¨Ñ¶½á¹¹Ìå*****************************************************/

//ÉÏ·¢ ±íÊ¾VMC _PC  ÏÂ·¢±íÊ¾ PC_VMC
//ÉÏ·¢Æô¶¯Ò»ÃùÖÇ»Û
#define MBOX_VMC_ZH_START		    	0x01
//ÉÏ·¢Æô¶¯GPRS
#define MBOX_VMC_GPRS_START		   		0x02
//vmcÉÏ·¢¸øPC»úÍ¶ÈëÓ²±Ò
#define MBOX_VMC_ZH_COIN		        0x03
//vmcÉÏ·¢¸øPC»úÍ¶ÈëÖ½±Ò
#define MBOX_VMC_ZH_BILL 		        0x04
//vmcÉÏ·¢¸øPC»úÔİ´æÖ½±Ò
#define MBOX_VMC_ZH_ESCROWIN 		    0x05
//vmcÉÏ·¢¸øPC»úÔİ´æÖ½±Ò³ö±Ò
#define MBOX_VMC_ZH_ESCROWOUT		    0x06
//vmcÉÏ·¢¸øPC»úÕÒÁã	  ¼´ 
#define MBOX_VMC_ZH_PAYOUT     		    0x07

//ÉÏ·¢ÀëÏß½»Ò×Êı¾İ
#define MBOX_VMC_ZH_OFFLINE_REPORT		0x08
//ÉÏ·¢³ö»õĞÅÏ¢
#define MBOX_VMC_ZH_TRADE			 0x09

//ÉÏ·¢°´¼ü Ñ¡»õ
#define MBOX_VMC_ZH_BUTTON			0x0B
//ÉÏ·¢½øÈëÎ¬»¤Ä£
#define MBOX_VMC_ZH_WEIHU			0x0C
//ÉÏ·¢Àë¿ªÎ¬»¤Ä£Ê½					
#define MBOX_VMC_ZH_NORMAL			0x0D
//ÇåÁã
#define MBOX_VMC_ZH_CLEAR 			0x0E
//ÉÏ·¢GPRSÍ¬²½Êı¾İ
#define MBOX_VMC_GPRS_CLEAR			0x0F

//ÉÏ·¢ACK
#define MBOX_VMC_ZH_ACK			0x10
//ÉÏ·¢NAK
#define MBOX_VMC_ZH_NAK			0x11
//Ò»´Î½»Ò×½áÊø
#define MBOX_VMC_ZH_TRADE_OVER 	0x12
//ÉÏ·¢ÈËÌå½Ó½ü
#define MBOX_VMC_ZH_HUMAN	   0x13
//ÉÏ·¢´óÃÅ´ò¿ª
#define MBOX_VMC_ZH_DOOR_OPEN   0x14
//ÉÏ·¢´óÃÅ¹Ø±Õ
#define MBOX_VMC_ZH_DOOR_CLOSE   0x15



//ÏÂ·¢ÀëÏß×´Ì¬
#define MBOX_ZH_VMC_OFFLINE			   0x01
//ÏÂ·¢ÔÚÏß×´Ì¬
#define MBOX_ZH_VMC_ONLINE			   0x02
//ÏÂ·¢³ö»õÖ¸Áî
#define MBOX_ZH_VMC_TRADE			   0x03
//ÏÂ·¢ÇåÁãÍê³É±êÖ¾
#define MBOX_ZH_VMC_CLEAR_OK		 0x04

/****************************************************************************
*
*add by yoc 2013.8.29
****************************************************************************/
//Ò»ÃùÖÇ»ÛÍ¨ĞÅÓÊÏä
typedef struct _message_zhihui{

	INT8U  vmc_type;//·¢¸øÖÇ»ÛµÄÓÊÏäÀàĞÍ
	INT8U  zh_type;//·¢¸øÖ÷¿Ø°åµÄÓÊÏäÀàĞÍ
	//±¾´Î½»Ò×ĞÅÏ¢	
	INT8U  zh_status;//Ò»ÃùÖÇ»Û×´Ì¬1±íÊ¾ÔÚÏß 0 ÀëÏß	
	INT16U trade_index;//µ±Ç°µÄ½»Ò×¼ÇÂ¼±£´æË÷Òı
	INT8U  usr_coming;//ÊÇ·ñÊÇµÚÒ»´Î
	INT8U  escrowInFlag;//Ôİ´æÖ½±Ò
	INT16U  recv_amount; //µ¥´ÎÍ¶Èë½ğ¶î
	INT16U  recv_all_amount;//×ÜÍ¶Èë½ğ¶î
	INT16U 	changed;//±¾´Î½»Ò×ÕÒÁãµÄ½ğ¶î
	INT8U  cur_logic_no;//±¾´Î½»Ò×µÄ»õµÀÂß¼­±àºÅ
	INT16U  cur_goods_price;//±¾´Î½»Ò×µÄÉÌÆ·µ¥¼Û
	INT8U  cur_goods_sn;//±¾´Î½»Ò×µÄÉÌÆ·±àºÅ
	INT8U  cur_trade_state;//³ö»õ²Ù×÷½á¹û
	
	INT8U  pay_mode;//±¾´Î½»Ò×Ö§¸¶·½Ê½
	
	//ÉèÖÃÉÌÆ·ĞòºÅ
	INT8U aisle_index;//ÎïÀí»õµÀ Ë÷Òı
	INT8U goodsID;//ÉÌÆ·ĞòºÅ Ê®½øÖÆ
	INT8U humanState;//ÈËÌå¸ĞÓ¦×´Ì¬
	
}MESSAGE_ZHIHUI;

//extern MESSAGE_ZHIHUI vmc_msg_pc;//·¢ËÍPC µÄÓÊÏä
extern OS_EVENT *g_PCMail;
extern OS_EVENT *g_PCBackMail;
extern unsigned char ZhIndex;//add by yoc 2013.10.10
extern MESSAGE_ZHIHUI vmc_zh_mbox[MBOX_ST_LEN];//add by yoc 2013.12.02
extern MESSAGE_ZHIHUI zh_vmc_mbox;

/******************************************end PCÍ¨Ñ¶½á¹¹Ìå*****************************************************/



/******************************************UboxPCÍ¨Ñ¶½á¹¹Ìå*****************************************************/
//vmcÏÂ·¢¸øPC»úÆô¶¯Ç©µ½ÃüÁî
#define MBOX_VMC_UBOXINITDEV			    0x01
//vmcÏÂ·¢¸øPC»úÍ¶ÈëÓ²±Ò
#define MBOX_VMCTOPC_PAYINCOIN		        0x02
//vmcÏÂ·¢¸øPC»úÍ¶ÈëÖ½±Ò
#define MBOX_VMCTOPC_PAYINBILL 		        0x03
//vmcÏÂ·¢¸øPC»úÔİ´æÖ½±Ò
#define MBOX_VMCTOPC_PAYINESCROWIN 		    0x04
//vmcÏÂ·¢¸øPC»úÔİ´æÖ½±Ò³ö±Ò
#define MBOX_VMCTOPC_PAYINESCROWOUT		    0x05
//vmcÏÂ·¢¸øPC»úÍ¶Èë¶Á¿¨Æ÷
#define MBOX_VMCTOPC_PAYINREADER		    0x06
//vmcÏÂ·¢¸øPC»úÕÒÁã
#define MBOX_VMCTOPC_PAYOUT     		    0x07
//pcÉÏ·¢¸øvmc»úÕÒÁãÖ¸Áî
#define MBOX_PCTOVMC_PAYOUTIND     		    0x08
//vmcÏÂ·¢¸øPCÍ¬ÒâACK
#define MBOX_VMCTOPC_ACK     		        0x09
//vmcÏÂ·¢¸øPC¾Ü¾øNAK
#define MBOX_VMCTOPC_NAK    		        0x0A
//pcÉÏ·¢¸øvmc»ú¿Û¿îÖ¸Áî
#define MBOX_PCTOVMC_COSTIND     		    0x0B
//vmcÏÂ·¢¸øPC»ú¿Û¿î
#define MBOX_VMCTOPC_COST       		    0x0C
//vmcÏÂ·¢¸øPC»ú°´¼ü
#define MBOX_VMCTOPC_BUTTON       		    0x0D
//vmcÏÂ·¢¸øPC»ú³ö»õĞÅÏ¢
#define MBOX_VMCTOPC_VENDOUT       		    0x0E
//pcÉÏ·¢¸øvmc»ú³ö»õÖ¸Áî
#define MBOX_PCTOVMC_VENDOUTIND     		0x0F
//pcÉÏ·¢¸øvmc»úÖØĞÂ¸´Î»Ö¸Áî
#define MBOX_PCTOVMC_RESETIND     		    0x10
//pcÉÏ·¢¸øvmc»úµÃµ½Éè±¸×´Ì¬Ö¸Áî
#define MBOX_PCTOVMC_STATUSIND     		    0x11
//ÏòPC»úÉÏ´«Éè±¸×´Ì¬Ö¸Áî
#define MBOX_VMCTOPC_STATUS     		    0x12
//ÏòPC»úÉÏ´«ActionĞÅÏ¢
#define MBOX_VMCTOPC_ACTION     		    0x13
//pcÉÏ·¢¸øvmc»úCONTROLÖ¸Áî
#define MBOX_PCTOVMC_CONTROLIND     	    0x14
//ÏòPC»úÉÏ´«AdminĞÅÏ¢
#define MBOX_VMCTOPC_ADMIN      		    0x15
//pcÉÏ·¢¸øvmc»úµÃµ½InfoÖ¸Áî
#define MBOX_PCTOVMC_INFOIND     		    0x16
//ÏòPC»úÉÏ´«ÏàÓ¦µÄInfoÖ¸Áî
#define MBOX_VMCTOPC_INFORPT     		    0x17
//pcÉÏ·¢¸øvmc»úPAYININDÖ¸Áî
#define MBOX_PCTOVMC_PAYININD     	           0x18
//ÏòPC»úÉÏ´«ÖØĞÂ¸´Î»Ö¸Áî
#define MBOX_PCTOVMC_RESETRPT     		    0x19


typedef struct
{
	//Ö½±ÒÆ÷
    uint8_t	PCCmd;						//vmcÏÂ·¢¸øPC¿ØÖÆÃüÁîÀàĞÍ
	uint8_t	PCBackCmd;					//PC»úÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ	
	
	uint16_t payInMoney;                //vmcÏÂ·¢¸øPCÍ¶±Ò½ğ¶î
	uint16_t payoutMoney;                //vmcÏÂ·¢¸øPCÕÒÁã×Ü½ğ¶î
	uint16_t payoutRemain;                //vmcÏÂ·¢¸øPCÕÒÁãÇ·¿î½ğ¶î
	uint32_t payAllMoney;               //vmcÏÂ·¢¸øPC×Ü¹²Í¶ÈëµÄ½ğ¶î
	uint8_t  payoutDev;					//vmcÏÂ·¢¸øPCÕÒÁãÉè±¸
	
	uint32_t changeMoney;               //pcÉÏ´«¸øvmcĞèÒªÕÒÁãµÄ½ğ¶î
	uint32_t costMoney;                 //pcÉÏ´«¸øvmcĞèÒª¿Û¿îµÄ½ğ¶î
	uint8_t Type;						//typeĞ­Òé×Ö½ÚµÄÖµ
	uint8_t Column;						//vmcÏÂ·¢¸øPC³ö»õ»õµÀ
	uint8_t status;                     //vmcÏÂ·¢¸øPC³ö»õ½á¹û
	uint8_t ColumnSum;                  //vmcÏÂ·¢¸øPC±¾»õµÀÊ£ÓàÉÌÆ·¸öÊı
	uint8_t method;                     //pcÉÏ´«¸øvmc³ö»õ·½Ê½1Í¨¹ıÉÌÆ·id³ö»õ,2Í¨¹ı»õµÀid³ö»õ
	uint8_t device;                     //pcÉÏ´«¸øvmc³ö»õÏä¹ñºÅ

	//ÏòPC»úÏÂ·¢Éè±¸×´Ì¬
	uint8_t check_st;                   //ÕÒÁãÆ÷×´Ì¬£º0=Õı³££¬1=±»Èí¼şÁÙÊ±½ûÓÃ£¬2=¹ÊÕÏ£¬3=Éè±¸²»´æÔÚ
	uint8_t bv_st;                      //Ö½±ÒÆ÷×´Ì¬£º0=Õı³££¬1=±»Èí¼şÁÙÊ±½ûÓÃ£¬2=¹ÊÕÏ£¬3=Éè±¸²»´æÔÚ
	uint8_t cc_st;                      //Ó²±ÒÆ÷×´Ì¬££º0=Õı³££¬1=±»Èí¼şÁÙÊ±½ûÓÃ£¬2=¹ÊÕÏ£¬3=Éè±¸²»´æÔÚ
	uint8_t vmc_st;                     //VMC×´Ì¬£¨´ú±íÕû¸öÊÛ»õ»ú£©£º0=Õı³££¬1= Õı³£»õµÀÉÌÆ·È«²¿ÊÛ¿Õ£¬»ò·ÇÊÛÂôÊ±¼ä£¬2=¹ÊÕÏ£¬3=Î¬»¤Ä£Ê½
	uint16_t change;                    //ÕÒÁãÁ¿×Ü½ğ¶î
	uint8_t tem_st;                     //»õ²Ö×´Ì¬ÉèÖÃÖµ£¬¹²Ö§³Ö4¸ö»õ²Ö
	uint16_t coinSum[6];                //Ã¿ÖÖÍ¨µÀÓ²±ÒµÄ´æ±ÒÊıÁ¿
	uint16_t recyclerSum[6];            //Ã¿ÖÖÍ¨µÀÖ½±ÒµÄ´æ±ÒÊıÁ¿

	uint8_t action;                     //Action¶¯×÷
	uint8_t value;                      //Action¶¯×÷µÄÖµ
	uint8_t second;                     //³ö»õ»òÕß³ö±ÒÔ¤¼ÆÊ±¼ä
	//Ê±¼ä
	uint16_t RTCyear;                     //Äê
	uint8_t  RTCmonth;                    //ÔÂ
	uint8_t  RTCday;                      //ÈÕ
	uint8_t  RTChour;                     //Ê±
	uint8_t  RTCmin;                      //·Ö
	uint8_t  RTCsecond;                   //Ãë
	uint8_t  RTCweek;                     //ÖÜ	
	//Ïòvmc·¢ºĞ·¹¹ñ¿ØÖÆÖ¸Áî
	uint8_t  Control_device;              //Ïä¹ñºÅ£¨3 ¨C N£©
	uint8_t  Control_type;                //¿ØÖÆÀàĞÍ0x53 £º±íÊ¾¼ÓÈÈ¿ØÖÆ 0x55£º±íÊ¾ÖÆÀä¿ØÖÆ 0x56±íÊ¾Õû»úÕÕÃ÷¿ØÖÆ
	uint8_t  Control_Huodao;              //Ïä¹ñµÄ»õµÀºÅ 
	uint8_t  cabinetNums;                 //Ä¿Ç°·ÇÆÕÍ¨¹ñµÄÊıÁ¿ 
	uint8_t  cabinetdata[30];             //ËùÓĞ·ÇÆÕÍ¨¹ñ×´Ì¬
	uint16_t wenduvalue;                  //ÎÂ¶ÈÉèÖÃµÄÖµ
	
	
} MessageUboxPCPack;	
#define UBOX_SIZE 7
extern MessageUboxPCPack MsgUboxPack[UBOX_SIZE];

extern uint8_t  g_Ubox_Index;

extern OS_EVENT *g_Ubox_VMCTOPCQ;
extern OS_EVENT *g_Ubox_VMCTOPCBackCMail;
extern OS_EVENT *g_Ubox_PCTOVMCQ;
extern OS_EVENT *g_Ubox_PCTOVMCBackQ;


/******************************************UboxSIMPLEPCÍ¨Ñ¶½á¹¹Ìå*****************************************************/
//pcÏÂ·¢¸øvmc»ú³ö»õÖ¸Áî
#define MBOX_SIMPLEPCTOVMC_VENDIND     	 0x01
//vmcÉÏ´«¸øPC»ú³ö»õĞÅÏ¢
#define MBOX_SIMPLEVMCTOPC_VENDOUT       0x02
//pcÏÂ·¢¸øvmc»úÏÔÊ¾Ö¸Áî
#define MBOX_SIMPLEPCTOVMC_DISPLAYIND    0x03
//vmcÉÏ´«¸øPC»ú°´¼ü¶ÔÓ¦ÉÌÆ·idĞÅÏ¢
#define MBOX_SIMPLEVMCTOPC_BUTTONRPT     0x04
//vmcÉÏ´«¸øPC»úAdminÖ¸Áî
#define MBOX_SIMPLEVMCTOPC_ADMINRPT      0x05
//pcÏÂ·¢¸øvmc»úAdminµÄ½á¹û
#define MBOX_SIMPLEVMCTOPC_RESULTIND	 0x06
//vmcÉÏ´«¸øPC»úGetAdminÖ¸Áî
#define MBOX_SIMPLEVMCTOPC_GETADMIN      0x07

typedef struct
{
	//Ö½±ÒÆ÷
    uint8_t	PCTOVMCCmd;						//pcÏÂ·¢¸øvmc¿ØÖÆÃüÁîÀàĞÍ
	uint8_t	VMCTOPCCmd;					    //vmcÉÏ±¨¸øPCÃüÁîÀàĞÍ	
	//³ö»õ
	uint8_t channel_id;					    //pcÏÂ·¢¸øvmc³ö»õ»õµÀ
	uint8_t channel_result;				    //vmcÉÏ±¨¸øPC³ö»õ½á¹û£¨0=³É¹¦£»1=Ê§°Ü£©
	//displayÏÔÊ¾
	uint8_t distype;   //ÏÔÊ¾ÀàĞÍ£¬0ÓÃ»§Óà¶î£¬1Ê±¼ä£¬2ÎÄ±¾£¬3ÉÌÆ·µ¥¼Û                  
	uint16_t payInMoney;//Í¶Èë½ğ¶î
	uint8_t  disp[5];//×Ö·û´®
	//ÉÏ±¨°´¼ü¶ÔÓ¦ÉÌÆ·id
	uint8_t btnchannel_id;                  //vmcÉÏ±¨¸øPC°´¼ü¶ÔÓ¦ÉÌÆ·id
	//admin
	uint8_t admintype;                      //vmcÉÏ±¨¸øPCËûµÄadminµÄÀàĞÍ1¼ÓÂúÈ«²¿»õµÀ£¬2¼ÓÂú²ã¼Ü»õµÀ£¬3µ¥»õµÀ²¹»õ£¬4×î³¤³ö»õÊ±¼ä£¬5×Ô¶¯ÍË±ÒÊ±¼ä
	uint8_t admincolumn;                    //vmcÉÏ±¨¸øPCËûµÄ²¹»õµÄ²ãºÅ»òÕß»õµÀºÅ
	uint8_t admincolumnsum;                 //vmcÉÏ±¨¸øPCËûµÄ²¹»õµÄÊıÁ¿
	uint8_t adminresult;                    //pcÏÂ·¢¸øvmcËûµÄadminµÄ½á¹û,0³É¹¦,ÆäËûÖµ¹ÊÕÏ	
} MessageSIMPLEUboxPCPack;	
#define SIMPLEUBOX_SIZE 7
extern MessageSIMPLEUboxPCPack MsgSIMPLEUboxPack[SIMPLEUBOX_SIZE];

extern uint8_t  g_SIMPLEUbox_Index;

extern OS_EVENT *g_SIMPLEUbox_VMCTOPCQ;
extern OS_EVENT *g_SIMPLEUbox_VMCTOPCBackCMail;
extern OS_EVENT *g_SIMPLEUbox_PCTOVMCQ;	
extern OS_EVENT *g_SIMPLEUbox_PCTOVMCBackCMail;


/******************************************Õ¹Ê¾Î»Í¨Ñ¶½á¹¹Ìå*****************************************************/
//Æô¶¯Ñ¡»õ°´¼ü°åÃüÁî
#define MBOX_SELECTKEYINIT			0x01
//vmcÏÂ·¢¸øÑ¡»õ°´¼ü°å°´¼üµÆÁÁÃüÁî
#define MBOX_SELECTLIGHT			0x02
//Ñ¡»õ°´¼ü°åÉÏ´«°´¼üÖµ
#define MBOX_SELECTVALUE		    0x03

typedef struct	
{
	uint8_t	KeyCmd;						//vmcÏÂ·¢¸øÑ¡»õ°´¼ü°å¿ØÖÆÃüÁîÀàĞÍ
	uint8_t	KeyBackCmd;					//Ñ¡»õ°´¼ü°åÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ
	//Ñ¡»õ°´¼ü;by gzz 20121025
	uint8_t  sel1ReadyLed;               //°´¼ü°å1µÄ¡ÌµÆ
	uint8_t  sel1ErrLed;                 //°´¼ü°å1µÄ¡ÁµÆ
	uint8_t  sel2ReadyLed;               //°´¼ü°å2µÄ¡ÌµÆ
	uint8_t  sel2ErrLed;                 //°´¼ü°å2µÄ¡ÁµÆ
	uint8_t  sel3ReadyLed;               //°´¼ü°å3µÄ¡ÌµÆ
	uint8_t  sel3ErrLed;                 //°´¼ü°å3µÄ¡ÁµÆ
	uint8_t  sel4ReadyLed;               //°´¼ü°å4µÄ¡ÌµÆ
	uint8_t  sel4ErrLed;                 //°´¼ü°å4µÄ¡ÁµÆ
	uint8_t  sel5ReadyLed;               //°´¼ü°å5µÄ¡ÌµÆ
	uint8_t  sel5ErrLed;                 //°´¼ü°å5µÄ¡ÁµÆ
	uint8_t  sel6ReadyLed;               //°´¼ü°å6µÄ¡ÌµÆ
	uint8_t  sel6ErrLed;                 //°´¼ü°å6µÄ¡ÁµÆ
	uint8_t  selectMode;                 //Ä£Ê½,0¿ÕÏĞ×´Ì¬,1½»Ò××´Ì¬
	uint8_t	 selectInput;                //°´¼üÖµ
}MessageKEYPack;
extern MessageKEYPack MsgKEYPack;

extern OS_EVENT *g_KEYMail;

typedef struct	
{
	uint8_t	KeyBackCmd;					//Ñ¡»õ°´¼ü°åÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ	
	uint8_t	 selectInput;                //°´¼üÖµ
}MessageKEYPackBack;
extern MessageKEYPackBack MsgKEYBackPack;
extern OS_EVENT *g_KEYBackMail;

/******************************************ÎÂ¿ØÆ÷Í¨Ñ¶½á¹¹Ìå*****************************************************/
//ÉèÖÃÎÂ¶ÈÃüÁî
#define MBOX_XMTSETTEMP 			0x01
//ÉèÖÃÎÂ¶È³É¹¦
#define MBOX_XMTTEMPOK			    0x02
//ÉèÖÃÎÂ¶ÈÊ§°Ü
#define MBOX_XMTTEMPFAIL		    0x03

typedef struct	
{
	uint8_t	KeyCmd;						//vmcÏÂ·¢ÎÂ¶ÈÃüÁî
	uint8_t	KeyBackCmd;					//ÎÂ¿ØÆ÷ÉÏ´«¸øvmcÉèÖÃ½á¹û
	//Ñ¡»õ°´¼ü;by gzz 20121025
	uint16_t  temparater;            	   //ÎÂ¶ÈÊıÁ¿
}MessageXMTPack;
extern MessageXMTPack MsgXMTPack;

extern OS_EVENT *g_XMTMail;
extern OS_EVENT *g_XMTBackMail;

/******************************************Ö½±ÒÕÒÁãÍ¨Ñ¶½á¹¹Ìå*****************************************************/
//Ö½±ÒÆ÷°´ÕÅÊıÕÒ±Ò
#define MBOX_FSBILLRECYPAYOUTNUM			0x01
//Ö½±ÒÆ÷°´½ğ¶îÕÒ±Ò
#define MBOX_FSBILLRECYPAYOUTVALUE		0x02
//·µ»ØÖ½±ÒÆ÷ÕÒÁã³É¹¦ÃüÁî
#define MBOX_FSBILLRECYPAYOUTSUCC			0x03
//·µ»ØÖ½±ÒÆ÷ÕÒÁãÊ§°ÜÃüÁî
#define MBOX_FSBILLRECYPAYOUTFAIL			0x04

typedef struct	
{
	uint8_t	BillBack;						//vmcÏÂ·¢ÎÂ¶ÈÃüÁî
	uint8_t   BillBackCmd;					//Ö½±ÒÆ÷ÉÏ´«¸øvmcÃüÁî¿ØÖÆµÄ·µ»ØÀàĞÍ
	uint32_t  RecyPayoutMoney;			    //ÕÒÁã½ğ¶î
	uint8_t   RecyPayoutNum;				//ÕÒÁãÊıÁ¿
	uint32_t  RecyPayoutMoneyBack;			//Êµ¼ÊÕÒÁã½ğ¶î
}MessageFSBillRecyclerPack;
extern MessageFSBillRecyclerPack MsgFSBillRecyclerPack;

extern OS_EVENT *g_FSBillRecyclerMail;
extern OS_EVENT *g_FSBillRecyclerBackMail;




/******************************************»õµÀ²ÎÊı*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé
} GOODSCHANNELPARAMETER;
/******************************************½»Ò×²ÎÊı*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé
} VENDINGPARAMETER;

/******************************************½»Ò×¼ÇÂ¼*****************************************************/
typedef struct	
{
	uint8_t		CrcCheck[2];					//CRCĞ£Ñé
} VENDINGLOG;

/******************************************Î»Í¼¸ñÊ½*****************************************************/
typedef struct
{
	unsigned char Width;			//Î»Í¼¿í¶È
	unsigned char Hight;			//Î»Í¼¸ß¶È
	const unsigned char *Data;		//Î»Í¼Êı¾İ
} BMP;
/******************************************¶¨Ê±Æ÷ÉèÖÃ*****************************************************/
typedef struct	
{
	uint8_t		PayoutTimer;					//³ö±ÒµÈ´ı¶¨Ê±Æ÷
	uint8_t		EscrowTimer;					//Ñ¹³­µÈ´ı¶¨Ê±Æ÷
	uint8_t		DispFreeTimer;					//½»Ò×½çÃæ¿ÕÏĞ½çÃæË¢ĞÂ¶¨Ê±Æ÷
	uint8_t		CheckDevStateTimer;				//¼ì²âÉè±¸¹ÊÕÏ×´Ì¬¶¨Ê±Æ÷
	uint8_t		GetTubeTimer;					//¼ì²âÉè±¸¹ÊÕÏ×´Ì¬¶¨Ê±Æ÷
	uint8_t		ChaxunTimer;					//½»Ò×½çÃæÊäÈë»õµÀ²éÑ¯¶¨Ê±Æ÷
	uint8_t		SaleTimer;					    //½»Ò×µ¹¼ÆÊ±¶¨Ê±Æ÷
	uint8_t		PCRecTimer;					    //PC´®¿Ú½ÓÊÕ¶¨Ê±Æ÷
	uint8_t		ActionPCTimer;					//UBOXµÄÎ¬»¤Ä£Ê½ÏÂAction¶¨Ê±Æ÷
	uint8_t		offLinePCTimer;				    //UBOXµÄÀëÏßÄ£Ê½ÏÂ¶¨Ê±Æ÷
	uint8_t     printTimer;                     //´òÓ¡½»Ò×¼ÇÂ¼¼ÆÊ±Æ÷
	uint8_t		selTimer;						//Ñ¡»õ°´¼ü½ÓÊÕ¶¨Ê±Æ÷
	uint16_t		weiTimer;						//ÍË³öÎ¬»¤¶¨Ê±Æ÷

	//add by yoc zhihui 2013.12.16
	uint8_t    	zhihui_recv_timeout;
	uint8_t		zhihui_button_timeout;
	uint8_t     zhihui_trade_time;
	uint8_t		pcm_handle_timeout;	//ÊÛ»õ»ú´¦ÀíPCÇëÇó³¬Ê± add by yoc newAisle
	
	uint8_t		getTempTimer;				  //¶ÁÈ¡ÎÂ¶È¶¨Ê±Æ÷
	uint8_t		mdb_col_timer;
} TIMER;
extern TIMER Timer;

/******************************************È«¾Ö±äÁ¿²ÎÊı*****************************************************/
typedef struct	
{
	uint8_t pcInitFlag;              
}GLOBALSYS;
extern GLOBALSYS globalSys;

/******************************************º¯ÊıÉùÃ÷*****************************************************/
void CreateMBox(void);
void LoadMechineSerialNo(unsigned char *Data);
void FactoryDefaultSystemPara(void);
void RstFactoryDefaultSystemPara(void);
uint8_t LoadDefaultSystemPara(uint8_t backup);
void WriteDefaultSystemPara(SYSTEMPARAMETER SysPara,uint8_t backup);
void ResetSystemPara(void);
uint8_t LoadUserSystemPara(uint8_t backup);
void WriteUserSystemPara(SYSUSERPARAMETER UPara,uint8_t backup);

unsigned char XorCheck(unsigned char *pstr,unsigned short len);
unsigned short CrcCheck(unsigned char *msg, unsigned short len);
char *PrintfMoney(uint32_t dispnum);

#endif
/**************************************End Of File*******************************************************/
