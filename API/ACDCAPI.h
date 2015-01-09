#ifndef _ACDCAPI_H
#define _ACDCAPI_H

#define OPENCOMPRESSOR				(1)
#define OPENLIGHT					(2)
#define CLOSECOMPRESSOR				(3)
#define CLOSELIGHT					(4)
#define OPENCOMPRESSORANDLIGHT		(5)
#define CLOSECOMPRESSORANDLIGHT		(6)



#define ACDC_MAX_HOT_TEMP			550 //55度
#define ACDCHANDLE					(0x51)

/*********************************************************************************************************
		ACDC控制类型宏    
*********************************************************************************************************/
#define ACDC_TYPE_LIGHT				(0x0001)
#define ACDC_TYPE_COMPRESSOR		(0x0002)
#define ACDC_TYPE_HOT				(0x0004)



unsigned char acdc_status_API(unsigned char cabinet,unsigned char type);

unsigned char acdc_handle_API(unsigned char binnum,unsigned char ledFlag,unsigned char cprFlag);
unsigned char API_ACDCHandle(unsigned char flag,unsigned char binnum,unsigned char HandleType);
unsigned char acdc_cpr_API(unsigned char binnum,unsigned char cprFlag);
unsigned char acdc_led_API(unsigned char binnum,unsigned char ledFlag);
unsigned char acdc_hot_API(unsigned char binnum,unsigned short temp);
unsigned char acdc_chuchou_API(unsigned char binnum,unsigned char flag);
unsigned short acdc_getTemp_API(unsigned char binnum);

#endif
