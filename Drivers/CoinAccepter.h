#ifndef _COINACCEPTER_H
//#define COINDEV_MDB		2				//当前系统识别到的硬币器类型为MDB

typedef struct	
{
	uint8_t Communicate;//1代表硬币器通讯不上
	uint8_t sensor;//1代表传感器故障
	uint8_t tubejam;//1代表出币口卡币
	uint8_t romchk;//1代表rom出错
	uint8_t routing;//1代表进币通道出错
	uint8_t jam;//1代表投币卡币	
	uint8_t removeTube;//1硬币斗移开
	//使用扩展指令检查
	uint8_t disable;//1硬币因为各种原因被vmc禁能了
	uint8_t unknowError;//非0硬币其他各种故障
	uint8_t unknowErrorLow;//硬币其他各种故障的子状态码
	
}MDBCOINERROR;
extern MDBCOINERROR MdbCoinErr;



extern uint8_t CoinDevInit(void);


extern void CoinDevDisable(void);

extern void CoinDevEnable(void);

extern unsigned char CoinDevProcess(uint32_t *RecvMoney,unsigned char *CoinType,unsigned char *coinOptBack);

extern void CoinDevProcessExpanse(void);
#endif
