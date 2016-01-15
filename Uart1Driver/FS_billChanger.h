#ifndef _FS_BILL_CHANGER_H_
#define _FS_BILL_CHANGER_H_


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef char int8;
typedef short int16;
typedef int int32;

#define FS_TIMER()	do{if(fs_timer){fs_timer--;}if(fs_reqTimer){fs_reqTimer--;}if(fs_taskTimer){fs_taskTimer--;}}while(0)





typedef struct{
	uint8 status;//状态 0正常 1初始化失败  2通信故障  其他故障 详细故障码见 下一字节 errCode 
	uint16 errCode; // 为故障码
	uint8 box[4]; // 0正常 1 缺币  2钞箱被移走  3禁用
}FS_STATUS;





extern volatile unsigned int fs_timer ;
extern volatile unsigned int fs_reqTimer ;
extern volatile unsigned int fs_taskTimer;


uint32 FS_dispense(uint32 payAmount);  //找零 参数为金额  返回 已找零金额  分为单位
FS_STATUS *FS_getStatus(void);	 //读取状态 



extern unsigned char FS_poll(void);
extern void FS_init(void);
void FS_mainTask(void);
#endif
