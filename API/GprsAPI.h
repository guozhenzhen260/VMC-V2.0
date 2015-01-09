#ifndef _GPRSAPI_H
#define _GPRSAPI_H



struct STGPRS
{
	//模块通信状态
	unsigned char Communication;
	//当前场景设置成功与否
	unsigned char QiFGCNT;
	//设置模块GPRS连接是否成功
	unsigned char QiCSGP;
	//设置模块数据传输为穿透模式
	unsigned char QiMODE;
	//检测通信卡的PIN是否解密
	unsigned char QiCPIN;
	//模块的当前信号强度
	unsigned char QiCSQ;
	//模块是否成功注册GSM网络
	unsigned char QiCREG;
	//模块是否成功注册GPRS网络
	unsigned char QiCGREG;
	//模块的当前状态
	unsigned char QiSTAT;
	//启动任务并设置接入点APN、用户名、密码是否成功
	unsigned char QiREGAPP;
	//激活场景是否成功
	unsigned char QiACT;
	//获得本地IP是否成功
	unsigned char QiLOCIP;
	//设置单路或多路连接模式：0——单路 1——多路
	unsigned char QiMUX;
	//当前与服务器连接情况
	unsigned char QiOPEN;
	//本地IP
	unsigned char PlaceIP[15];
	//连接服务器IP
	unsigned char ServerIP[15];
	//连接服务器IP地址长度
	unsigned char ServerIPLen;
	//服务器端口
	unsigned char ServerPort[5];
	//服务器端口地址长度
	unsigned char ServerPortLen;
	//模块当前状态
	//1:IP INITIAL        2:IP START        3:IP CONFIG
	//4:IP IND            5:IP GPRSACK      6:IP STATUS
	//7:TCP CONNECTING    8:UDP CONNECTING  9:IP CLOSE
	//10:CONNECT OK       11:PDP DEACT
	unsigned char State;
};

extern struct STGPRS stGprsStruct;

unsigned char GPRS_Init(void);

unsigned char GPRS_PutInIP(unsigned char type,unsigned char *IpBuf,unsigned char *PortBuf);

extern unsigned char GPRS_QISEND(unsigned char *sbuf,unsigned char len);

extern unsigned char GPRS_QIOPEN(unsigned char *IPbuf,unsigned char *Port,unsigned char Iplen,unsigned char PortLen);

unsigned char GPRS_SendReport(unsigned char *Reportbuf,unsigned char len);

unsigned char GPRS_GetAckReport(unsigned char *buf,unsigned char *len);

unsigned char GPRS_QISTATE(void);

unsigned char GPRS_QIRD(unsigned char *AckBuf);

unsigned char GPRS_QINDI(void);

unsigned char GPRS_QIPOWERDOWN(void);

unsigned char GPRS_QICLOSE(void);

unsigned char GPRS_QIDEACT(void);

unsigned char GPRS_QIACT(void);
unsigned char mvc_gprs_init(void);



#endif
