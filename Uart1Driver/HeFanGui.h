#ifndef _HEFANGUI_H
#define _HEFANGUI_H

extern unsigned char HeFanGuiRecvAckDevice(void);

extern unsigned char HeFanGuiUart(unsigned char Binnum,unsigned char BinAdd,unsigned char Cmd,
							unsigned char *Data,unsigned char len,unsigned char *Ack);

extern unsigned char HeFanGuiDriver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char *Result);



#endif
