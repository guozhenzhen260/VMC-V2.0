#ifndef _BILLACCEPTER_H
//#define BILLDEV_MDB		2				//当前系统识别到的纸币器类型为MDB

typedef struct	
{
	uint8_t Communicate;//1代表纸币器通讯不上
	uint8_t moto;//1代表马达故障
	uint8_t sensor;//1代表传感器故障
	uint8_t romchk;//1代表rom出错
	uint8_t jam;//1代表投币卡币
	uint8_t removeCash;//1移除纸币钞箱
	uint8_t cashErr;//1纸币钞箱故障
	uint8_t disable;//1纸币因为各种原因被禁能了
}MDBBILLERROR;
extern MDBBILLERROR MdbBillErr;


extern void BillDevSellect(void);

extern uint8_t BillDevInit(void);

extern void BillDevDisable(void);

extern void BillDevEnable(void);

extern unsigned char BillDevProcess(uint32_t *RecvMoney,unsigned char *BillType,unsigned char billOpt,unsigned char *billOptBack,uint32_t RecyPayoutMoney,uint8_t RecyPayoutNum);

extern uint8_t BillDevEscrow(void);

extern uint8_t BillDevReturn(void);

extern uint8_t RecyclerDevInit(void);


#endif
