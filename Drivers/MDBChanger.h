#ifndef _CHANGERACCEPTER_H

#define CHANGERDEV_HOPPER	1				//当前系统识别到的硬币器类型为Hopper
#define CHANGERDEV_MDB		2				//当前系统识别到的硬币器类型为MDB



extern unsigned char ChangePayoutProcessLevel3(uint32_t PayMoney,unsigned char PayoutNum[16]);
extern unsigned char ChangePayoutProcessLevel2(unsigned char PayType,unsigned char PayNum, unsigned char PayoutNum[8]);
extern void ChangeGetTubes(void);

#endif
