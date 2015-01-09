#include "..\config.h"

volatile unsigned int GPRSWAITTIMEOUT=0;

unsigned char GPRS_SendCmd(char *Cmd,unsigned char len)
{
	unsigned char buf[128]={0};

	uart3_clr_buf();
	strncpy((char *)buf,Cmd,len);
	strcat((char *)buf,"\r");
	Uart3PutStr(buf,len+1);
	return 1;
}


unsigned char GPRS_RecvAck(unsigned char *rBuf,unsigned char *Len)
{
	unsigned char i=0;

	while(Uart3BuffIsNotEmpty())
	{
		*rBuf++ = Uart3GetCh();
		i++;
	}
	if(i!=0)
	{
		*Len = i;
		return 1;
	}
	else
	{
		*Len = 0;
		return 0;
	}
}


unsigned char GPRS_MissionProcess(char *Cmd,unsigned char len,unsigned char *rBuf,unsigned char *rLen)
{
	unsigned char rst = 0;

	GPRS_SendCmd(Cmd,len);
	OSTimeDly(50);
	GPRSWAITTIMEOUT = 1000;
	while(GPRSWAITTIMEOUT)
	{
		rst = GPRS_RecvAck(rBuf,rLen);
		if(rst == 1)
			break;	
	}
	if(GPRSWAITTIMEOUT)	
		return 1;
	else
		return 0;
}

unsigned char GPRS_SendData(char *Cmd,unsigned char len,unsigned char *rBuf,unsigned char *rLen)
{
	unsigned char rst = 0;
	unsigned char buf[512]={0};

	uart3_clr_buf();
	strcpy((char *)buf,Cmd);
	//strcat();
	Uart3PutStr(buf,strlen((char *)buf));
	Uart3PutChar(0x1a);
	OSTimeDly(50);
	GPRSWAITTIMEOUT = 1000;
	while(GPRSWAITTIMEOUT)
	{
		rst = GPRS_RecvAck(rBuf,rLen);
		if(rst == 1)
			break;	
	}
	if(GPRSWAITTIMEOUT)	
		return 1;
	else
		return 0;
}





