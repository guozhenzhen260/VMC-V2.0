#include "..\config.h"

struct STGPRS stGprsStruct;
volatile unsigned int GPRSHANDLEWAITTIMEOUT;


/*****************************************************************************
** Function name:	GPRS_Init	
**
** Descriptions:	GPRS模块初始化	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char ComparisonSTR(unsigned char *abuf,unsigned char *bbuf)
{
	unsigned char i=0,j=0;
	unsigned char len1=0,len2=0;

	len1 = strlen((char *)abuf);
	len2 = strlen((char *)bbuf);
	for(i=0;i<len1;i++)
	{
		if(abuf[i]==bbuf[j])
		{
			j++;
			if(j==len2)
				break;
		}
	}
	if(i>=len1)
		return 0;
	else
		return 1;
}


/*****************************************************************************
** Function name:	GPRS_AT	
**
** Descriptions:	GPRS模块AT检测是否通讯正常
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——正常     0——失败
** 
*******************************************************************************/
unsigned char GPRS_AT()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT",2,rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.Communication = 1;
		Trace("AT test OK...\r\n");
		return 1;
	}
	else
	{
		stGprsStruct.Communication = 0;
		return 0;	
	}	
}


/*****************************************************************************
** Function name:	GPRS_Init	
**
** Descriptions:	GPRS模块关闭回显	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功  0——失败
** 
*******************************************************************************/
unsigned char GPRS_ATE0()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("ATE0",4,rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;	
}


/*****************************************************************************
** Function name:	GPRS_CPIN	
**
** Descriptions:	GPRS模块检测通讯卡片PIN是否解密
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功    0——失败
** 
*******************************************************************************/
unsigned char GPRS_CPIN()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;
	
	GPRS_MissionProcess("AT+CPIN?",8,rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"+CPIN: READY");
	Trace("Init..OK...3\r\n");
	if(flag == 1)
	{
		stGprsStruct.QiCPIN = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiCPIN = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_IPR	
**
** Descriptions:	GPRS模块设置波特率并保存
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功      0——失败
** 
*******************************************************************************/
unsigned char GPRS_IPR()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+IPR=9600&W",strlen("AT+IPR=9600&W"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	Trace("Init..OK...4\r\n");
	if(flag == 1)
		return 1;
	else
		return 0;	
}


/*****************************************************************************
** Function name:	GPRS_CSQ	
**
** Descriptions:	GPRS模块获得当前模块通讯信号强度	
**					
**														 			
** parameters:		Signal——信号强度
					
					
** Returned value:	1——成功       0——失败
** 
*******************************************************************************/
unsigned char GPRS_CSQ(unsigned char *Signal)
{
	unsigned char rbuf[128]={0},rlen=0,j=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+CSQ",strlen("AT+CSQ"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"+CSQ: ");
	if(flag==1)
	{
		for(j=0;j<strlen((char *)rbuf);j++)
		{
			if((rbuf[j]=='Q')&&(rbuf[j+1]==':')&&(rbuf[j+2]==' '))
			{
				*Signal = (rbuf[j+3]%48)*10+rbuf[j+4]%48;
			}
		}
		stGprsStruct.QiCSQ = 1;
		Trace("Init..OK...5\r\n");
		return 1;
	}
	else
	{
		stGprsStruct.QiCSQ = 0;
		return 0;
	}
}


/*****************************************************************************
** Function name:	GPRS_CREG	
**
** Descriptions:	GPRS模块是否连接上GSM网络
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功    0——失败
** 
*******************************************************************************/
unsigned char GPRS_CREG()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+CREG?",strlen("AT+CREG?"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"+CREG:0,1");
	if(flag == 1)
	{
		stGprsStruct.QiCREG = 1;
		return 1;
	}
	else
	{
		flag = ComparisonSTR(rbuf,"+CREG:0,5");
		if(flag == 1)
		{
			stGprsStruct.QiCREG = 1;
			return 1;	
		}
	}
	stGprsStruct.QiCREG = 0;
	return 0;	
}


/*****************************************************************************
** Function name:	GPRS_CGREG	
**
** Descriptions:	GPRS模块检测模块是否注册上GPRS网络	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功     0——失败
** 
*******************************************************************************/
unsigned char GPRS_CGREG()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+CGREG?",strlen("AT+CGREG?"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"+CGREG:0,1");
	if(flag == 1)  
	{
		stGprsStruct.QiCGREG = 1;
		return 1;
	}
	else
	{
		flag = ComparisonSTR(rbuf,"+CGREG:0,5");
		if(flag == 1)
		{
			stGprsStruct.QiCGREG = 1;
			return 1;	
		}
	}
	stGprsStruct.QiCGREG = 0;
	return 0;	
}


/*****************************************************************************
** Function name:	GPRS_QIFGCNT	
**
** Descriptions:	GPRS模块设置当前场景	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功   0——失败
** 
*******************************************************************************/
unsigned char GPRS_QIFGCNT()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIFGCNT=0",strlen("AT+QIFGCNT=0"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		Trace("set FGCNT OK...\r\n");
		stGprsStruct.QiFGCNT = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiFGCNT = 0;
		return 0;	
	}
}

/*****************************************************************************
** Function name:	GPRS_QINDI	
**
** Descriptions:	GPRS模块设置是否缓存接收到的数据	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功   0——失败
** 
*******************************************************************************/
unsigned char GPRS_QINDI()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QINDI=1",strlen("AT+QINDI=1"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		//stGprsStruct.QiFGCNT = 1;
		return 1;
	}
	else
	{
		//stGprsStruct.QiFGCNT = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_QICSGP	
**
** Descriptions:	GPRS模块设置GPRS的APN	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功      0——失败
** 
*******************************************************************************/
unsigned char GPRS_QICSGP()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QICSGP=1,\"CMNET\"",strlen("AT+QICSGP=1,\"CMNET\""),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiCSGP = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiCSGP = 0;
		return 0;	
	}
}

/*****************************************************************************
** Function name:	GPRS_QIMODE	
**
** Descriptions:	GPRS模块设置数据为非传透模式	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功            0——失败
** 
*******************************************************************************/
unsigned char GPRS_QIMODE()
{
	unsigned char rbuf[128]={0},rlen=0;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIMODE=0",strlen("AT+QIMODE=0"),rbuf,&rlen);
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiMODE = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiCSGP = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_QISTAT	
**
** Descriptions:	GPRS模块获取当前模块状态	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功    0——失败
** 
*******************************************************************************/
unsigned char GPRS_QISTAT()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QISTAT",strlen("AT+QISTAT"),rbuf,&rlen);
	Trace("\r\nSTAT");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiSTAT = 1;
		flag = ComparisonSTR(rbuf,"STATE:");
		if(flag == 1)
		{
			if(ComparisonSTR(rbuf,"IP INITIAL"))
				stGprsStruct.State = 1;	
			else
			if(ComparisonSTR(rbuf,"IP START"))
				stGprsStruct.State = 2;
			else
			if(ComparisonSTR(rbuf,"IP CONFIG"))
				stGprsStruct.State = 3;
			else
			if(ComparisonSTR(rbuf,"IP IND"))
				stGprsStruct.State = 4;
			else
			if(ComparisonSTR(rbuf,"IP GPRSACK"))
				stGprsStruct.State = 5;
			else
			if(ComparisonSTR(rbuf,"IP STATUS"))
				stGprsStruct.State = 6;
			else
			if(ComparisonSTR(rbuf,"TCP CONNECTING"))
				stGprsStruct.State = 7;
			else
			if(ComparisonSTR(rbuf,"UDP CONNECTING"))
				stGprsStruct.State = 8;
			else
			if(ComparisonSTR(rbuf,"IP CLOSE"))
				stGprsStruct.State = 9;
			else
			if(ComparisonSTR(rbuf,"CONNECT OK"))
				stGprsStruct.State = 10;
			else
			if(ComparisonSTR(rbuf,"PDP DEACT"))
				stGprsStruct.State = 11;
			Trace("\r\n");
			for(i=0;i<rlen;i++)
				Trace("%c",rbuf[i]);
			Trace("\r\n");
			stGprsStruct.QiSTAT = 1;
			return 1;	
		}
		return 1;
	}
	stGprsStruct.QiSTAT = 0;
	return 0;	
}

/*****************************************************************************
** Function name:	GPRS_QIREGAPP	
**
** Descriptions:	GPRS模块启动任务，并设置接入点APN、用户名和密码
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QIREGAPP()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIREGAPP=\"CMNET\"",strlen("AT+QIREGAPP=\"CMNET\""),rbuf,&rlen);
	Trace("\r\nREGAPP");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiREGAPP = 1;
		return 1;
	}
	stGprsStruct.QiREGAPP = 0;
	return 0;	
}


/*****************************************************************************
** Function name:	GPRS_QIACT	
**
** Descriptions:	GPRS模块激活移动场景
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——激活成功      0——激活失败
** 
*******************************************************************************/
unsigned char GPRS_QIACT()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIACT",strlen("AT+QIACT"),rbuf,&rlen);
	Trace("\r\nACT");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiACT = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiACT = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_QILOCIP	
**
** Descriptions:	GPRS模块获取本地IP地址	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——成功    0——失败
** 
*******************************************************************************/
unsigned char GPRS_QILOCIP()
{
	unsigned char rbuf[128]={0},rlen=0,i,j;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QILOCIP",strlen("AT+QILOCIP"),rbuf,&rlen);
	Trace("\r\nLOCIP");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"\n");
	if(flag == 1)
	{
		j = 0;
		stGprsStruct.QiLOCIP = 1;
		for(i=0;i<rlen;i++)
		{
			if((rbuf[i]>='0'&&rbuf[i]<='9')||(rbuf[i]=='.'))
			{
				stGprsStruct.PlaceIP[j] = rbuf[i];	
				j++;
			}
		}
		return 1;
	}
	else
	{
		stGprsStruct.QiLOCIP = 0;
		return 0;
	}	
}



/*****************************************************************************
** Function name:	GPRS_QIMUX	
**
** Descriptions:	GPRS模块查询连接方式
**					
**														 			
** parameters:		无
					
					
** Returned value:	1——通信成功       0——通信失败
** 
*******************************************************************************/
unsigned char GPRS_QIMUX()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIMUX?",strlen("AT+QIMUX?"),rbuf,&rlen);
	Trace("\r\nMUX");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiMUX = 1;
		return 1;
	}
	else
	{
		stGprsStruct.QiMUX = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_QIOPEN	
**
** Descriptions:	GPRS模块连接服务器	
**					
**														 			
** parameters:		*IPbuf——服务器IP地址
					*Port——服务器端口号
					
** Returned value:	1——连接成功     0——连接失败
** 
*******************************************************************************/
unsigned char GPRS_QIOPEN(unsigned char *IPbuf,unsigned char *Port,unsigned char Iplen,unsigned char PortLen)
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0,sendbuf[128]={0};

//	Trace("IP_%d:",Iplen);
//	for(i=0;i<Iplen;i++)
//		Trace("%c",IPbuf[i]);
//	Trace("\r\n");
//	Trace("PORT_%d:",PortLen);
//	for(i=0;i<PortLen;i++)
//		Trace("%c",Port[i]);
//	Trace("\r\n");
	strcpy((char *)sendbuf,"AT+QIOPEN=\"TCP\",");
	strcat((char *)sendbuf,"\"");
	strncat((char *)sendbuf,(char *)IPbuf,Iplen);
	strcat((char *)sendbuf,"\",");
	strncat((char *)sendbuf,(char *)Port,PortLen);
	//Trace("IP_PORT:");
	//for(i=0;i<strlen(sendbuf);i++)
	//	Trace("%c",sendbuf[i]);
	//Trace("\r\n");
	GPRS_MissionProcess((char *)sendbuf,strlen((char *)sendbuf),rbuf,&rlen);
	Trace("\r\nOPEN");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"CONNECT OK");
	if(flag == 1)
	{
		stGprsStruct.QiOPEN = 1;
		return 1;
	}
	else
	{
		if(ComparisonSTR(rbuf,"ALREADY CONNECT"))
		{
			stGprsStruct.QiOPEN = 1;
			return 1;	
		}
		stGprsStruct.QiOPEN = 0;
		return 0;	
	}
}


/*****************************************************************************
** Function name:	GPRS_QISEND	
**
** Descriptions:	GPRS模块发送数据	
**					
**														 			
** parameters:		*sbuf——发送的数据缓存
					len——发送数据的长度
					
** Returned value:	发送成功——1   发送失败——0
** 
*******************************************************************************/
unsigned char GPRS_QISEND(unsigned char *sbuf,unsigned char len)
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;
	unsigned char sendbuf[128]={0},nbuf[4]={0};

	strcat((char *)sendbuf,"AT+QISEND=");
	sprintf((char *)nbuf,"%d",len);
	strncat((char *)sendbuf,(char *)nbuf,strlen((char *)nbuf));
	//Trace("sendbuf:");
//	for(i=0;i<strlen(sendbuf);i++)
	//	Trace("%c",sendbuf[i]);
	//Trace("\r\n");
	GPRS_MissionProcess((char *)sendbuf,strlen((char *)sendbuf),rbuf,&rlen);
	//for(i=0;i<rlen;i++)
	//	Trace("%c",rbuf[i]);
	//Trace("\r\n");
	flag = ComparisonSTR(rbuf,">");
	if(flag == 1)
	{
		GPRS_SendData((char *)sbuf,len,rbuf,&rlen);
		Trace("\r\nSEND2");
		for(i=0;i<rlen;i++)
			Trace("%c",rbuf[i]);
		Trace("\r\n");
		flag = ComparisonSTR(rbuf,"SEND OK");
		if(flag == 1)
			return 1;
	}
	return 0;	
}


/*****************************************************************************
** Function name:	GPRS_QISTATE	
**
** Descriptions:	GPRS模块检测模块当前状态	
**					
**														 			
** parameters:		无
					
					
** Returned value:	1：通信成功、0通信失败
** 
*******************************************************************************/
unsigned char GPRS_QISTATE()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QISTATE",strlen("AT+QISTATE"),rbuf,&rlen);
	Trace("\r\nSTATE");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		stGprsStruct.QiSTAT = 1;
		flag = ComparisonSTR(rbuf,"STATE:");
		if(flag == 1)
		{
			if(ComparisonSTR(rbuf,"IP INITIAL"))
				stGprsStruct.State = 1;	
			else
			if(ComparisonSTR(rbuf,"IP START"))
				stGprsStruct.State = 2;
			else
			if(ComparisonSTR(rbuf,"IP CONFIG"))
				stGprsStruct.State = 3;
			else
			if(ComparisonSTR(rbuf,"IP IND"))
				stGprsStruct.State = 4;
			else
			if(ComparisonSTR(rbuf,"IP GPRSACK"))
				stGprsStruct.State = 5;
			else
			if(ComparisonSTR(rbuf,"IP STATUS"))
				stGprsStruct.State = 6;
			else
			if(ComparisonSTR(rbuf,"TCP CONNECTING"))
				stGprsStruct.State = 7;
			else
			if(ComparisonSTR(rbuf,"UDP CONNECTING"))
				stGprsStruct.State = 8;
			else
			if(ComparisonSTR(rbuf,"IP CLOSE"))
				stGprsStruct.State = 9;
			else
			if(ComparisonSTR(rbuf,"CONNECT OK"))
				stGprsStruct.State = 10;
			else
			if(ComparisonSTR(rbuf,"PDP DEACT"))
				stGprsStruct.State = 11;
			Trace("\r\n");
			for(i=0;i<rlen;i++)
				Trace("%c",rbuf[i]);
			Trace("\r\n");
			stGprsStruct.QiSTAT = 1;
			return stGprsStruct.State;	
		}
		return 0x1f;
	}
	stGprsStruct.QiSTAT = 0;
	return 0;	
}

/*****************************************************************************
** Function name:	GPRS_QIPOWERDOWN	
**
** Descriptions:	GPRS模块关机操作	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QIPOWERDOWN()
{
	unsigned char rbuf[128]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QPOWD=1",strlen("AT+QPOWD=1"),rbuf,&rlen);
	Trace("\r\nQPOWD");
	for(i=0;i<rlen;i++)
		Trace("%c",rbuf[i]);
	Trace("\r\n");
	flag = ComparisonSTR(rbuf,"NORMAL POWER DOWN");
	if(flag == 1)
		return 1;
	else
		return 0;
}


/*****************************************************************************
** Function name:	GPRS_QIRD	
**
** Descriptions:	GPRS模块读取缓存中的数据	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QIRD(unsigned char *AckBuf)
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIRD=0,1,0,512",strlen("AT+QIRD=0,1,0,512"),rbuf,&rlen);
	Trace("\r\nQIRD_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
	{
		strcpy((char *)AckBuf,(char *)rbuf);
		return 1;
	}
	else
		return 0;
}

/*****************************************************************************
** Function name:	GPRS_QIHEAD	
**
** Descriptions:	GPRS模块配置接收时是否显示IP头	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QIHEAD()
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIHEAD=1",strlen("AT+QIHEAD=1"),rbuf,&rlen);
	Trace("\r\nQIHEAD_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;
}

/*****************************************************************************
** Function name:	GPRS_QISHOWPT	
**
** Descriptions:	GPRS模块接收数据时是否显示通信协议	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QISHOWPT()
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QISHOWPT=1",strlen("AT+QISHOWPT=1"),rbuf,&rlen);
	Trace("\r\nQISHOWPT_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;
}


/*****************************************************************************
** Function name:	GPRS_QISHOWLA	
**
** Descriptions:	GPRS模块接收数据时是否显示数据发送的目的IP地址	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QISHOWLA()
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QISHOWLA=1",strlen("AT+QISHOWLA=1"),rbuf,&rlen);
	Trace("\r\nQISHOWLA_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;
}


/*****************************************************************************
** Function name:	GPRS_QICLOSE	
**
** Descriptions:	GPRS模块断开本次连接	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QICLOSE()
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QICLOSE",strlen("AT+QICLOSE"),rbuf,&rlen);
	Trace("\r\nQICLOSE_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;
}

/*****************************************************************************
** Function name:	GPRS_QIDEACT	
**
** Descriptions:	GPRS模块关闭GPRS场景	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_QIDEACT()
{
	unsigned char rbuf[512]={0},rlen=0,i;
	unsigned char flag=0;

	GPRS_MissionProcess("AT+QIDEACT",strlen("AT+QIDEACT"),rbuf,&rlen);
	Trace("\r\nQIDEACT_%d",rlen);
	for(i=0;i<rlen;i++)
		Trace(" %c",rbuf[i]);
	Trace("End\r\n");
	flag = ComparisonSTR(rbuf,"OK");
	if(flag == 1)
		return 1;
	else
		return 0;
}

/*****************************************************************************
** Function name:	GPRS_Init	
**
** Descriptions:	GPRS模块初始化	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_Init()
{
	unsigned char signal=0,i;
	unsigned char rst = 0;

	for(i=0;i<10;i++)
	{
		GPRS_QICLOSE();
		OSTimeDly(10);
		GPRS_QIDEACT();
		OSTimeDly(10);
		//检测模块通信是否正常
		rst = GPRS_AT();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-1");
		//if(rst != 1)
		//	continue;
		OSTimeDly(2);
		//关闭模块回显功能
		rst = GPRS_ATE0();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-2");
		//if(rst != 1)
		//	continue;
		OSTimeDly(2);
		//设置通讯固定波特率
		rst = GPRS_IPR();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-3");
		//if(rst != 1)
		//	continue;
		OSTimeDly(2);
		
		//配置前置场景
		rst = GPRS_QIFGCNT();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-4");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//设置GPRS的APN
		rst = GPRS_QICSGP();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-5");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//将模块数据传输设置为非透传模式
		rst = GPRS_QIMODE();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-6");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//检测通讯卡PIN是否已经解密
		rst = GPRS_CPIN();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-7");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//检测GPRS模块信号强度
		rst = GPRS_CSQ(&signal);
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-8");
		//if(rst != 1)
		//	continue;
		OSTimeDly(2);
		//检测模块是否注册上GSM网络
		rst = GPRS_CREG();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-9");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//检测模块是否注册上GPRS网络
		rst = GPRS_CGREG();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-10");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//查询当前状态
		rst = GPRS_QISTAT();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-11");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//启动任务并设置接入点APN、用户名、密码
		rst = GPRS_QIREGAPP();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-12");
		if(rst != 1)
			continue;
		OSTimeDly(10);
		//激活移动场景
		rst = GPRS_QIACT();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-13");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		//获取本地IP
		rst = GPRS_QILOCIP();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-14");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		GPRS_QINDI();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-15");
		OSTimeDly(2);
		GPRS_QISHOWLA();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-16");
		OSTimeDly(2);
		GPRS_QISHOWPT();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-17");
		OSTimeDly(2);
		GPRS_QIHEAD();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-18");
		OSTimeDly(2);
		//设置单路连接模式
		rst = GPRS_QIMUX();
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-19");
		if(rst != 1)
			continue;
		OSTimeDly(2);
		stGprsStruct.ServerIPLen = 13;
		strncpy((char *)stGprsStruct.ServerIP,"124.72.45.92",13);
		stGprsStruct.ServerPortLen = 4;
		strncpy((char *)stGprsStruct.ServerPort,"8465",4);
		//连接服务器
		rst = GPRS_QIOPEN(stGprsStruct.ServerIP,stGprsStruct.ServerPort,stGprsStruct.ServerIPLen,stGprsStruct.ServerPortLen);
		LCDNumberFontPrintf(40,LINE15,2,"GPRSAccepter-20");
		//if(rst != 1)
		//	continue;
		OSTimeDly(2);
		Trace("\r\nCommunication=%d\r\n",stGprsStruct.Communication);
		Trace("\r\nQiFGCNT=%d\r\n",stGprsStruct.QiFGCNT);
		Trace("\r\nQiCSGP=%d\r\n",stGprsStruct.QiCSGP);
		Trace("\r\nQiMODE=%d\r\n",stGprsStruct.QiMODE);
		Trace("\r\nQiCPIN=%d\r\n",stGprsStruct.QiCPIN);
		Trace("\r\nQiCSQ=%d\r\n",stGprsStruct.QiCSQ);
		Trace("\r\nQiCREG=%d\r\n",stGprsStruct.QiCREG);
		Trace("\r\nQiCGREG=%d\r\n",stGprsStruct.QiCGREG);
		Trace("\r\nQiSTAT=%d\r\n",stGprsStruct.QiSTAT);
		Trace("\r\nQiREGAPP=%d\r\n",stGprsStruct.QiREGAPP);
		Trace("\r\nQiACT=%d\r\n",stGprsStruct.QiACT);
		Trace("\r\nQiLOCIP=%d\r\n",stGprsStruct.QiLOCIP);
		Trace("\r\nQiMUX=%d\r\n",stGprsStruct.QiMUX);
		Trace("\r\nQiOPEN=%d\r\n",stGprsStruct.QiOPEN);
		Trace("\r\nPlaceIP=%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",stGprsStruct.PlaceIP[0],stGprsStruct.PlaceIP[1],stGprsStruct.PlaceIP[2],stGprsStruct.PlaceIP[3],stGprsStruct.PlaceIP[4]
		,stGprsStruct.PlaceIP[5],stGprsStruct.PlaceIP[6],stGprsStruct.PlaceIP[7],stGprsStruct.PlaceIP[8],stGprsStruct.PlaceIP[9],stGprsStruct.PlaceIP[10],stGprsStruct.PlaceIP[11]
		,stGprsStruct.PlaceIP[12],stGprsStruct.PlaceIP[13],stGprsStruct.PlaceIP[14]);
		Trace("\r\nState=%d\r\n",stGprsStruct.State);
		break;
	}
	return (i < 10) ? 1 : 0;
}

/*****************************************************************************
** Function name:	GPRS_PutInIP	
**
** Descriptions:	GPRS模块输入IP地址或端口地址
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
/*
unsigned char GPRS_PutInIP(unsigned char type,unsigned char *IpBuf,unsigned char *PortBuf)
{
	unsigned char j=0,max=0;
	unsigned char keyValue=0;
	unsigned char ip[15]={0},port[5]={0};

	if(type==0)
	{
		max = 15;
	}
	else
	{
		max = 5;
	}
	Trace("keyvalue=%d\r\n",keyValue);
	while(1)
	{
		keyValue = KeyValueAPI(0);
		switch(keyValue)
		{
			case '0':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '1':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '2':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '3':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '4':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '5':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '6':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '7':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '8':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '9':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case '.':
				if(j>=max)
					break;
				if(type==0)
					ip[j++] = keyValue;
				else
					port[j++] = keyValue;
				break;
			case 'C':					  
				//if(type==0)
				//	memset(IpBuf,0x00,sizeof(IpBuf));
				//else
				//	memset(PortBuf,0x00,sizeof(PortBuf));
				return 0;
			case 'D':
				if(type == 0)
					strncpy(IpBuf,ip,j);
				else
					strncpy(PortBuf,port,j);
				return j;
		}
	}
}
*/

/*****************************************************************************
** Function name:	GPRS_SendReport	
**
** Descriptions:	GPRS模块发送报文	
**					
**														 			
** parameters:		无
					
					
** Returned value:	0——失败   1——成功   2——M35通信失败
** 
*******************************************************************************/
unsigned char GPRS_SendReport(unsigned char *Reportbuf,unsigned char len) //changed by yoc 2013.09.03
{
	unsigned char rst=0,i;
	for(i = 0;i < 10;i++)
	{
		OSTimeDly(50);
		rst = GPRS_QISTATE();	
		switch (rst)
		{
			case 0:
				return 2;
			
			case 7://IPconnecting...continue waiting....GPRS_status 		
				break;
			case 9://IPclose  ...open GPRS again
				GPRS_QIOPEN(stGprsStruct.ServerIP,stGprsStruct.ServerPort,
							stGprsStruct.ServerIPLen,stGprsStruct.ServerPortLen);
				break;	
			case 10://IPconnected 
				return GPRS_QISEND(Reportbuf,len);	//发送数据
			
			default:
				GPRS_QICLOSE();
				break;
		}
	}
	return 0;

}

/*****************************************************************************
** Function name:	GPRS_GetAckReport	
**
** Descriptions:	GPRS模块接收应答报文	
**					
**														 			
** parameters:		无
					
					
** Returned value:	无
** 
*******************************************************************************/
unsigned char GPRS_GetAckReport(unsigned char *buf,unsigned char *len)
{
	unsigned char rbuf[512]={0};
	unsigned char flag = 0;
	unsigned int i=0,j=0,m=0;

	flag = GPRS_QIRD(rbuf);
	if(flag)
	{
		for(i=0;i<strlen((char *)rbuf);i++)
		{
			if((rbuf[i]=='T')&&(rbuf[i+1]=='C')&&(rbuf[i+2]=='P'))
			{
				//Trace("tcp1=%x %x %x %x %x %x\r\n",rbuf[i+2],rbuf[i+3],rbuf[i+4],rbuf[i+5],rbuf[i+6],rbuf[i+7]);
				//Trace("tcp2=%x %x %x %x %x %x\r\n",rbuf[i+8],rbuf[i+9],rbuf[i+10],rbuf[i+11],rbuf[i+12],rbuf[i+13]);
				i+=4;
				for(;i<strlen((char *)rbuf);)
				{
					if(rbuf[i]!=0x0d)
						i++;
					else
					if(rbuf[i+1]==0x0a)
					{
						m = i+2;
						break;
					}
				}
			}
		}	
		//Trace("m==%d\r\n",m);
		//m+=4;
		while(m<strlen((char *)rbuf))
		{
			//Trace("m=%d_j=%d_%c\r\n",m,j,rbuf[m]);
			if((rbuf[m]!=0x0d)&&(rbuf[m+1]!=0x0a))
				buf[j++] = rbuf[m];
			else
				break;
			m++;
		}
		//Trace("j==%d\r\n",j);
		*len = j;
	}
	if(j)
		return 1;	
	else
		return 0;
}


unsigned char mvc_gprs_init(void)
{
	unsigned char err;
	GPRS_QICLOSE();
	OSTimeDly(10);
	GPRS_QIDEACT();
	OSTimeDly(10);
	err = GPRS_Init();		
	OSTimeDly(OS_TICKS_PER_SEC * 8);
	while(!GPRS_QIOPEN(stGprsStruct.ServerIP,stGprsStruct.ServerPort,stGprsStruct.ServerIPLen,stGprsStruct.ServerPortLen))
	{
		Trace("Opening. retry..\r\n");
		OSTimeDly(3000/5);	
	}
	return err;

}

