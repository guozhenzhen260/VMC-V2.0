
#ifndef _COL_API_H_
#define _COL_API_H_
#include "..\Uart1Driver\FS_billChanger.h"

#define COL_POLL_IDLE		0x00
#define COL_POLL_BUSY		0x01
#define COL_POLL_FAIL		0x02
#define COL_POLL_SUC		0x04
#define COL_POLL_ERR		0x05
#define COL_POLL_JUSTRESET	0x06


#define COL_BIT_COOL	(0x01U << 0)
#define COL_BIT_LIGHT	(0x01U << 1)
#define COL_BIT_HOT		(0x01U << 3)



typedef struct _st_column_{
	uint8 empty;
}ST_COLUMN;


typedef struct _st_bin_{
	uint8 ishot;
	uint8 iscool;
	uint8 islight;
	uint8 isemptyCtrl;
	uint8 sum;
	uint8 coolTemp;
	uint8 hotTemp;
	uint8 sensorFault;
	uint8 id_len;
	uint8 type;
	uint8 mcb_ver;
	uint8 svc_ver;
	ST_COLUMN col[88];
}ST_BIN;


typedef struct _col_ctrl_{
	unsigned char cool; //制冷开关 0关 1开
	unsigned char light;//照明开关 0关 1开
	unsigned char hot;//加热开关 0关 1开
	unsigned char coolTemp; //制冷温度
	unsigned char hotTemp;//加热温度

}COL_CTRL;


unsigned char COL_driver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char *Result);



#endif

