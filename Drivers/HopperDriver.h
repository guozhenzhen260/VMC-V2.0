#ifndef _HOPPERACCEPTER_H
#define _HOPPERACCEPTER_H


//等待Hopper操作回复定时器
//extern volatile unsigned int HpHandleTimer;

/************************************************函数接口*****************************************************/

void HopperAccepter_CheckHPOk(void);

unsigned char HopperAccepter_Handle(unsigned char DeviceCmdType,unsigned char Address,unsigned char *nbuf);

unsigned char HopperAccepter_HpOutHandle(unsigned char Count,unsigned char HpNum,unsigned char *HpOutNum);

void HopperAccepter_GetState(unsigned char DevType);


#endif
