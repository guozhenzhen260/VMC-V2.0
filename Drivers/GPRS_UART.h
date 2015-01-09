#ifndef _GPRS_UART_H
#define _GPRS_UART_H


unsigned char GPRS_SendCmd(char *Cmd,unsigned char len);

unsigned char GPRS_RecvAck(unsigned char *rBuf,unsigned char *Len);

unsigned char GPRS_MissionProcess(char *Cmd,unsigned char len,unsigned char *rBuf,unsigned char *rLen);

unsigned char GPRS_SendData(char *Cmd,unsigned char len,unsigned char *rBuf,unsigned char *rLen);

#endif
