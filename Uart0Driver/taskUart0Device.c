
/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           pcControl
** Last modified Date:  2013-11-21
** Last Version:         
** Descriptions:        PC管理VMC通信                 
**------------------------------------------------------------------------------------------------------
** Created by:          yoc
** Created date:       2013-11-21
** Version:               V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"
#include "..\APP\channel.h"
#include "..\APP\LIFTTABLE.h"
#include "..\APP\BUSINESS.H"


//定义串口的几个函数指针
typedef void (*FUN_VOID_VOID_PTR)(void);
typedef unsigned char  (*FUN_CHAR_VOID_PTR)(void);
typedef void (*FUN_VOID_CHAR_INT_PTR)(unsigned char const *, unsigned int);

FUN_VOID_VOID_PTR 		pcm_uartClear;
FUN_CHAR_VOID_PTR 		pcm_uartGetCh;
FUN_CHAR_VOID_PTR 		pcm_uartNoEmpty;
FUN_VOID_CHAR_INT_PTR	pcm_uartPutStr; 


#define PCM_HEAD			0xEF//PC管理协议头标志
#define PCM_SHC_HEAD 		0xEC//PC管理短连接协议头标志


//VMC指令
#define ACK_VMCCOM 			0x00
#define NAK_VMCCOM 			0x01
#define VMC_PCM_POLL 		0x02
#define VMC_PCM_SEEK 		0x03


#define START_RPT 			0x82
#define TRADE_RPT 			0x83
#define CLEAR_RPT 			0x84
#define SYSPARA_RPT 		0x85
#define HUODAO_RPT 			0x86
#define FACTORRECOVER_RPT 	0x87
#define DATETIME_RPT 		0x88
#define STATUS_RPT 			0x89
#define DEBUG_RPT 			0x8A
#define FACTOR_SET_RPT 	    0x8B
#define ID_WRITE_RPT				0x8C
#define ID_READ_RPT				0x8D

//PC指令
#define ACK_PCCON  			0x80
#define	NAK_PCCON  			0x81
#define START_REQ 			0x82
#define TRADE_REQ 			0x83
#define CLEAR_REQ 			0x84
#define SYSPARA_REQ 		0x85
#define HUODAO_REQ 			0x86
#define FACTORRECOVER_REQ 	0x87
#define DATETIME_REQ 		0x88
#define STATUS_REQ 			0x89
#define DEBUG_REQ 			0x8A
#define FACTOR_SET_REQ 	    0x8B
#define ID_WRITE_REQ				0x8C
#define ID_READ_REQ				0x8D

static unsigned char mainTrace = 0;//总调试接口
static unsigned char snNo = 0,comErr = 0,shcSN = 0;//序列号 和串口错误数
static const unsigned char ack_t = 0x00,nak_t = 0x01,head = 4,
							ack = 1,nak = 2,msg = 3;

static unsigned char busyFlag = 0;
static unsigned char pcm_index = 0;
static unsigned char cur_head = PCM_HEAD,cur_port = 0;//当前包头当前串口号

#pragma arm section zidata = "RAM2" rwdata = "RAM2"
static OS_STK pcControlTaskStk[PC_CONTROL_TASK_STK_SIZE];
PCCONMSG vmc_pc_mail[PCM_MAX_SIZE];
PCCONMSG pc_vmc_mail;
PCM_MSG pcm_msg;

static OS_STK  SystemDatasBroadcastStk[STK_CHECK_TASK_SIZE];
#pragma arm section



PCM_MSG *pcUartMsg;

//PC下发邮箱
void *PCMSizeQ[PCM_MAX_SIZE];
OS_EVENT *pcm_vmc_event;
OS_EVENT *vmc_pcm_event;



#define MBOX_HUODAO_INIT		0x00//货到初始化
#define MBOX_HUODAO_TEST		0x01//货道测试
#define MBOX_HUODAO_OUTGOODS	0x02//货道出货

#define MBOX_RST				0x40//应答




void pcHuodaoReq(unsigned char len,unsigned char *data);
void pcTestAisleRpt(PCCONMSG *mail);
void pcInitHuoDaoRpt(unsigned type,unsigned char result);
void pcHuodaoOutGoodsRpt(PCCONMSG *mail);



/*********************************************************************************************************
** Function name:     pcEncodAmount
** Descriptions:      将32位金额编码成一字节数据
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char pcEncodAmount(unsigned int amount)
{

	unsigned char i = 0,value;
	if(amount == 0)
		return 0;
	while(!(amount % 10))
	{
		amount = amount / 10;
		i++;
	}
	switch(amount)
	{
		case 1:
			value = 1;
			break;
		case 2:
			value = 2;
			break;
		case 5:
			value = 5;
			break;
		default:
			value = 0;
			break;
	}
	if(value)
	{
		value = (i << 4) | (value & 0x0f);
		return value;
	}
	else
		return 0;

}
/*********************************************************************************************************
** Function name:     pcAnalysisAmount
** Descriptions:      将一字节数据解析为32位金额
** input parameters:    
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
static unsigned int pcAnalysisAmount(unsigned char data)
{

	unsigned int amount;
	unsigned char uint;
	if(data == 0)
		return 0;
	uint =  data >> 4;
	amount = data & 0x0f;
	
	while(uint)
	{
		amount = amount * 10;
		uint--;
	}
	return amount;
}


/*********************************************************************************************************
** Function name:     pcAmountToRatio
** Descriptions:     
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned int pcAmountToRatio(unsigned int amount)
{

	unsigned char i = 0,dec = SystemPara.DecimalNumExt;
	unsigned int value = amount;

	for(i = 0;i < dec;i++) value /= 10;
	
	return value;

}
/*********************************************************************************************************
** Function name:     pcAmountFromRatio
** Descriptions:     
** input parameters:    
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
static unsigned int pcAmountFromRatio(unsigned int amount)
{
	unsigned char i = 0,dec = SystemPara.DecimalNumExt;
	unsigned int value = amount;

	for(i = 0;i < dec;i++) value *= 10;
	
	return value;
}




/*********************************************************************************************************
** Function name:     pcControlRecv
** Descriptions:      串口接收协议包
** input parameters:   rLen接收长度，rData接收缓冲区指针
** output parameters:   无
** Returned value:      0 表示失败，  1 -- ack、2-- nak、3--msg 
*********************************************************************************************************/
static unsigned char pcControlRecv(unsigned short *rLen,unsigned char *rData)
{
	unsigned char index = 0,buf[10] = {0},temp,dLen,MT;
	unsigned short crc;

	if(!pcm_uartNoEmpty())
		return 0;
		
	temp = pcm_uartGetCh();
	//判断包头是否正确
	if(temp != PCM_HEAD)	
	{
		print_err(2);
		return 0;
	}
	rData[index++] = temp;

	dLen = pcm_uartGetCh();//len
	if(dLen < head) //验证包大小
	{
		print_err(2);
		return 0;
	}
	else
		rData[index++] = dLen;

	//接收余下数据
	Timer.printTimer= 20;
	while(Timer.printTimer)
	{
		if(pcm_uartNoEmpty())
		{
			rData[index++] = pcm_uartGetCh();
			if(index >= (dLen + 2))
				break;
		}
			
	}
	if(!Timer.printTimer)
	{
		print_warning(2);
		return 0;
	}
	
	crc = CrcCheck(rData,dLen); 	
	if(rData[dLen] != HUINT16(crc) || rData[dLen + 1] != LUINT16(crc)) //CRC校验不正确
	{	
		pcm_uartClear();				
		pcm_uartPutStr(rData,index); 
		return 0;
	}
		
	MT = rData[3];
	if(MT == ACK_PCCON)
		return ack;
	else if(MT == NAK_PCCON)
		return nak;
	else
	{
		buf[0] = PCM_HEAD;
		buf[1] = head;
		buf[2] = rData[2];
		buf[3] = ACK_VMCCOM;
		crc = CrcCheck(buf,head); 
		buf[4] = HUINT16(crc);
		buf[5] = LUINT16(crc);
		pcm_uartClear();
		OSSchedLock();
		pcm_uartPutStr(buf,head + 2);//立即回应邋ACk
		OSSchedUnlock();				
		
		*rLen = dLen;//SF LEN SN MT + DATA
		if(rData[2] != snNo)//序列号不一致抛弃但是还是会上报ack但不处理请求
		{
			return ack;
		}
		else
		{
			return msg;
		}
			
		
	}				
				
	
}



/*********************************************************************************************************
** Function name:     pcControlSend
** Descriptions:      发送协议
** input parameters:    type 协议类型MT，len发送长度，data发送缓冲区指针
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/
static unsigned char pcControlSend(unsigned char  type,unsigned short len,unsigned char *data)
{
	
	unsigned char i = 0,rcx,send_buf[150] = {0},rst;	
	unsigned short crc;	
	send_buf[0] = PCM_HEAD;
	send_buf[1] = len + head;
	if(type == ack_t || type == nak_t)
		send_buf[2] = snNo;
	else
		send_buf[2] = ++snNo;
	send_buf[3] = type;
	for(i = 0;i < len;i++)
	{
		send_buf[i+head] = *data++;
	}
	   
	crc = CrcCheck(send_buf,len+head);
	send_buf[len + head] =	HUINT16(crc);
	send_buf[len + head + 1] = LUINT16(crc);


	rcx = 2;
	while(rcx--)
	{
		pcm_uartClear();
		OSSchedLock();
		pcm_uartPutStr(send_buf,len+head + 2);
		OSSchedUnlock();
		if(type == ack_t || type == nak_t)
		{
			return ack;
		}
		Timer.printTimer= 20;
		while(Timer.printTimer)	/*接收ACK 超时则返回0*/
		{
			rst = pcControlRecv(&pcUartMsg->recv_len,pcUartMsg->recv_msg);
			if(rst)	
			{
				comErr = 0;
				return rst;
			}
		}
		PC_DELAY_POLL;		
	}
	comErr++;
	return 0;

	
}

/*********************************************************************************************************
** Function name:     pcShcRecv
** Descriptions:      串口接收协议包
** input parameters:   rLen接收长度，rData接收缓冲区指针
** output parameters:   无
** Returned value:      0 表示失败，  1 -- ack、2-- nak、3--msg 
*********************************************************************************************************/
static unsigned char pcShcRecv(unsigned short *rLen,unsigned char *rData)
{
	unsigned char index = 0,temp,dLen,MT;
	unsigned short crc;

	if(!pcm_uartNoEmpty())
		return 0;
		
	temp = pcm_uartGetCh();
	//判断包头是否正确
	if(temp != PCM_SHC_HEAD)	
	{
		print_err(2);
		return 0;
	}
	rData[index++] = temp;

	dLen = pcm_uartGetCh();//len
	if(dLen < head) //验证包大小
	{
		print_err(2);
		return 0;
	}
	else
		rData[index++] = dLen;

	//接收余下数据
	Timer.printTimer= 20;
	while(Timer.printTimer)
	{
		if(pcm_uartNoEmpty())
		{
			rData[index++] = pcm_uartGetCh();
			if(index >= (dLen + 2))
				break;
		}
			
	}
	if(!Timer.printTimer)
	{
		print_warning(2);
		return 0;
	}
	
	crc = CrcCheck(rData,dLen); 	
	if(rData[dLen] != HUINT16(crc) || rData[dLen + 1] != LUINT16(crc)) //CRC校验不正确
	{	
		pcm_uartClear();				
		pcm_uartPutStr(rData,index); 
		return 0;
	}
		
	MT = rData[3];
	if(MT == ACK_PCCON)
		return ack;
	else if(MT == NAK_PCCON)
		return nak;	
	else
		return 0;
	
	
}


/*********************************************************************************************************
** Function name:     pcShcSend
** Descriptions:      短连接发送协议
** input parameters:    type 协议类型MT，len发送长度，data发送缓冲区指针
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/
static unsigned char pcShcSend(unsigned char  type,unsigned short len,unsigned char *data)
{
	
	unsigned char i = 0,rcx,send_buf[150] = {0},rst;	
	unsigned short crc;
	send_buf[0] = PCM_SHC_HEAD;
	send_buf[1] = len + head;
	send_buf[2] = shcSN;
	send_buf[3] = type;
	for(i = 0;i < len;i++)
	{
		send_buf[i+head] = *data++;
	}
	   
	crc = CrcCheck(send_buf,len+head);
	send_buf[len + head] =	HUINT16(crc);
	send_buf[len + head + 1] = LUINT16(crc);
	rcx = 2;
	while(rcx--)
	{
		pcm_uartClear();
		OSSchedLock();
		pcm_uartPutStr(send_buf,len+head + 2);
		OSSchedUnlock();
		if(type == ack_t || type == nak_t)
		{
			return ack;
		}
		
		Timer.printTimer= 20;
		while(Timer.printTimer)	/*接收ACK 超时则返回0*/
		{
			rst = pcShcRecv(&pcUartMsg->recv_len,pcUartMsg->recv_msg);
			if(rst)	
			{
				comErr = 0;
				return rst;
			}
		}
		PC_DELAY_POLL;		
	}
	comErr++;
	return 0;

	
}


/*********************************************************************************************************
** Function name:     pcReply
** Descriptions:        pc回应VMC
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/

static unsigned char pcReply(uint8_t type,uint8_t len,uint8_t *data)
{
	if(cur_head == PCM_HEAD)
		return pcControlSend(type,len,data);
	else if(cur_head == PCM_SHC_HEAD)
		return pcShcSend(type,len,data);
	else
		return 0;
}






void pcShcReq(unsigned char type,unsigned short len,unsigned char *data)
{

	switch(type)
	{
		case HUODAO_REQ:
			pcHuodaoReq(len,data);
			break;
	}
	
}


/*********************************************************************************************************
** Function name:     pcSeekUart0
** Descriptions:        每秒读串口0
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/

static unsigned char pcSeekUart0()
{
	unsigned char i = 0,temp,dLen,MT,sn;
	unsigned short crc;
	if(Uart0BuffIsNotEmpty())//搜索串口0
	{
		//读取包头标志 
		temp = Uart0GetCh();	
		if((temp != PCM_HEAD) && (temp != PCM_SHC_HEAD))
		{

			ClrUart0Buff();
			print_log("msg head is not 0xEF or 0xEC!\r\n");
			return 0;
		}
		pcUartMsg->recv_msg[i++] = temp;	
		dLen = Uart0GetCh();//接收长度
		if(dLen < head)
		{
			return 0;
		}
		pcUartMsg->recv_msg[i++] = dLen;
			
		Timer.printTimer= 20;
		while(Timer.printTimer)
		{
			if(Uart0BuffIsNotEmpty())
			{
				pcUartMsg->recv_msg[i++] = Uart0GetCh();
				if(i >= (dLen + 2))
					break;
			}	
		}
		if(!Timer.printTimer)
		{
			ClrUart0Buff();
			return 0;
		}

		crc = CrcCheck(pcUartMsg->recv_msg,dLen);
		if(crc != INTEG16(pcUartMsg->recv_msg[dLen],pcUartMsg->recv_msg[dLen + 1]))
				return 0;

		//自此表示收到符合的协议包初始化串口函数指针
		pcm_uartClear = ClrUart0Buff;
		pcm_uartGetCh = Uart0GetCh;
		pcm_uartNoEmpty = Uart0BuffIsNotEmpty;
		pcm_uartPutStr = Uart0PutStr;

		
		MT = pcUartMsg->recv_msg[3];
		if((pcUartMsg->recv_msg[0] == PCM_HEAD) && (MT == START_REQ))
		{
			cur_head = PCM_HEAD;
			cur_port = 0;
			return 1;
		}
		else if(pcUartMsg->recv_msg[0] == PCM_SHC_HEAD)
		{
			cur_head = PCM_SHC_HEAD;
			cur_port = 0;
			
			if(shcSN == pcUartMsg->recv_msg[2] || busyFlag)//重包
			{	
				sn = shcSN;
				shcSN = pcUartMsg->recv_msg[2];
				pcShcSend(nak_t,0,NULL);
				shcSN = sn;
				
			}
			else
			{
				shcSN = pcUartMsg->recv_msg[2];
				pcShcSend(ack_t,0,NULL);	
				pcShcReq(MT,dLen - head,&pcUartMsg->recv_msg[4]);	
			}
			return 0;
		}

	}
	return 0;

	
}

/*********************************************************************************************************
** Function name:     pcSeekUart3
** Descriptions:        每秒读串口3
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/

static unsigned char pcSeekUart3()
{
	unsigned char i = 0,temp,dLen,MT,sn;
	unsigned short crc;
	if(SystemPara.PcEnable)
		return 0;
	if(Uart3BuffIsNotEmpty())//搜索串口0
	{
		//读取包头标志 
		temp = Uart3GetCh();	
		if((temp != PCM_HEAD) && (temp != PCM_SHC_HEAD))
		{
			uart3_clr_buf();
			print_log("msg head is not 0xEF or 0xEC!\r\n");
			return 0;
		}
		pcUartMsg->recv_msg[i++] = temp;	
		dLen = Uart3GetCh();//接收长度
		if(dLen < head)
		{
			return 0;
		}
		pcUartMsg->recv_msg[i++] = dLen;
			
		Timer.printTimer= 20;
		while(Timer.printTimer)
		{
			if(Uart3BuffIsNotEmpty())
			{
				pcUartMsg->recv_msg[i++] = Uart3GetCh();
				if(i >= (dLen + 2))
					break;
			}	
		}
		if(!Timer.printTimer)
		{
			uart3_clr_buf();
			return 0;
		}

		crc = CrcCheck(pcUartMsg->recv_msg,dLen);
		if(crc != INTEG16(pcUartMsg->recv_msg[dLen],pcUartMsg->recv_msg[dLen + 1]))
				return 0;

		//自此表示收到符合的协议包初始化串口函数指针
		pcm_uartClear = uart3_clr_buf;
		pcm_uartGetCh = Uart3GetCh;
		pcm_uartNoEmpty = Uart3BuffIsNotEmpty;
		pcm_uartPutStr = Uart3PutStr;
		
		
		MT = pcUartMsg->recv_msg[3];
		if((pcUartMsg->recv_msg[0] == PCM_HEAD) && (MT == START_REQ))
		{
			cur_head = PCM_HEAD;
			cur_port = 3;
			return 1;
		}
		else if(pcUartMsg->recv_msg[0] == PCM_SHC_HEAD)
		{
			cur_head = PCM_SHC_HEAD;
			cur_port = 3;
			if(shcSN == pcUartMsg->recv_msg[2] || busyFlag)//重包
			{	
				sn = shcSN;
				shcSN = pcUartMsg->recv_msg[2];
				pcShcSend(nak_t,0,NULL);
				shcSN = sn;
			}
			else
			{
				shcSN = pcUartMsg->recv_msg[2];
				pcShcSend(ack_t,0,NULL);	
				pcShcReq(MT,dLen - head,&pcUartMsg->recv_msg[4]);	
			}	
			return 0;
		}

	}
	return 0;

	
}


/*********************************************************************************************************
** Function name:     pcSeek
** Descriptions:        每秒读串口
** input parameters:    
** output parameters:   无
** Returned value:      0 表示失败 1、2成功 
*********************************************************************************************************/
static unsigned char pcSeek()
{
	unsigned char res;	
	unsigned char err;
	PCCONMSG *recvMsg;
	res = pcSeekUart0();
	if(res)
	{	
		return res;
	}	
	res =  pcSeekUart3();
	if(res)
	{
		return res;
	}

	//如果在处理请求 轮询 处理结果
	if(busyFlag)
	{
		recvMsg = OSQPend(vmc_pcm_event,500/5,&err);
		if(err == OS_NO_ERR)
		{
			switch(recvMsg->type)
			{
				//回应结果
				case MBOX_HUODAO_INIT:
					pcInitHuoDaoRpt(0x02,1);
					break;
				case MBOX_HUODAO_TEST:
					pcTestAisleRpt(recvMsg);
					break;
				case MBOX_HUODAO_OUTGOODS:
					pcHuodaoOutGoodsRpt(recvMsg);
					break;
				default:
					
					break;
			}
			busyFlag = 0;
			return 0;
			
		}
		if(!Timer.pcm_handle_timeout)
		{
			busyFlag = 0;
			err = 0xFF;
			
			pcReply(VMC_PCM_POLL,1,&err);
		}
		
	}
	
	
	return 0;
}







/*********************************************************************************************************
** Function name:     	pcSendTradeReady
** Descriptions:	 回应准备打印的信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static unsigned char pcSendTradeReady(unsigned char type)
{
	unsigned char index1 = 0;
	unsigned short maxPage;
	unsigned int temp;

	
	pcUartMsg->send_msg[index1++] = type;	
	maxPage = TRADE_PAGE_MAX;
	pcUartMsg->send_msg[index1++] = HUINT16(maxPage);
	pcUartMsg->send_msg[index1++] = LUINT16(maxPage);

	temp = LogPara.LogDetailPage;//当前索引页
	pcUartMsg->send_msg[index1++] = HUINT16(temp);
	pcUartMsg->send_msg[index1++] = LUINT16(temp);

	temp = pcAmountToRatio(LogPara.IncomeTotal);//总收币
	pcUartMsg->send_msg[index1++] = H0UINT32(temp);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	temp = pcAmountToRatio(LogPara.vpSuccessNumTotal);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	temp = pcAmountToRatio(LogPara.vpSuccessMoneyTotal);
	pcUartMsg->send_msg[index1++] = H0UINT32(temp);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	temp = pcAmountToRatio(LogPara.TotalChangeTotal);
	pcUartMsg->send_msg[index1++] = H0UINT32(temp);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	temp = pcAmountToRatio(LogPara.DoubtNum);
	pcUartMsg->send_msg[index1++] = H0UINT32(temp);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	temp = pcAmountToRatio(LogPara.Iou);
	pcUartMsg->send_msg[index1++] = H0UINT32(temp);
	pcUartMsg->send_msg[index1++] = H1UINT32(temp);
	pcUartMsg->send_msg[index1++] = L0UINT32(temp);
	pcUartMsg->send_msg[index1++] = L1UINT32(temp);

	PC_DELAY_500MS;
	pcReply(TRADE_RPT,index1,pcUartMsg->send_msg); //总交易
	return 1;		
			
}

/*********************************************************************************************************
** Function name:     	pcSendDetailTrade
** Descriptions:	    打印交易记录明细
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char pcSendDetailTrade(INT8U type,INT16U index1)
{	
	//char			//出货货道编号
	uint8_t	        i,j,pay_mode,buf[255] = {0};
	uint16_t index =0;
	unsigned int tempAmout;
	LOGPARAMETERDETAIL detailTrade;
	
	buf[index++] = type;
	if(!index1 || !ReadDetailTraceAPI((char *)&detailTrade,index1))
	{
		buf[index++] =  0;
		buf[index++] =  0;		
	}	
	else
	{
		buf[index++] =  HUINT16(index1);
		buf[index++] =  LUINT16(index1);
			/*
		*pay_mode 0:现金 1:一卡通支付 2银联卡支付 3PC出货 4支付宝 5游戏出货  8 0xEE其它
		*
		*/
		buf[index++] =  HEC_to_BCD(detailTrade.BeginYear/100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.BeginYear%100) ;
		buf[index++] =  HEC_to_BCD(detailTrade.BeginMonth);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginDay);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginHour);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginMinute);
		buf[index++] =  HEC_to_BCD(detailTrade.BeginSec);

		tempAmout = pcAmountToRatio(detailTrade.IncomeBill);
		buf[index++] = HUINT16(tempAmout);
		buf[index++] = LUINT16(tempAmout);

		tempAmout = pcAmountToRatio(detailTrade.IncomeCoin);
		buf[index++] = HUINT16(tempAmout);
		buf[index++] = LUINT16(tempAmout);
		tempAmout = pcAmountToRatio(detailTrade.IncomeCard);
		buf[index++] = HUINT16(tempAmout);
		buf[index++] = LUINT16(tempAmout);
		tempAmout = pcAmountToRatio(detailTrade.Change);
		buf[index++] = HUINT16(tempAmout);
		buf[index++] = LUINT16(tempAmout);
		buf[index++] = detailTrade.TradeNum;
		for(i = 0,j = 0;i < detailTrade.TradeNum;i++)
		{
			if(detailTrade.ColumnNo[j] == 'A')
			{
				buf[index++] = 1;
			}
			else if(detailTrade.ColumnNo[j] == 'B')
			{
				buf[index++] = 2;
			}
			else//其他情况
				buf[index++] = 1;
			
			buf[index++] = (INT8U)(detailTrade.ColumnNo[j+1] - '0') * 10 + (INT8U)(detailTrade.ColumnNo[j+2] - '0');
		
			buf[index++] = LUINT16(detailTrade.GoodsNo[i]);

			tempAmout = pcAmountToRatio(detailTrade.PriceNo[i]);
			buf[index++] = HUINT16(tempAmout);
			buf[index++] = LUINT16(tempAmout);
			
			pay_mode = detailTrade.PayMode[i];

			buf[index++] = pay_mode;
			buf[index++] = detailTrade.TransSucc[i];
			j += 3;
		
		}
		
	}
	
	PC_DELAY_500MS;
	pcReply(TRADE_RPT,index,buf);
	
	return 1;
	
}



/*********************************************************************************************************
** Function name:     	pcTradeReq
** Descriptions:	   处理PC交易请求
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcTradeReq(unsigned char len,unsigned char *data)
{
	unsigned char type,dataLen;
	if(len < 1)
	{
		print_warning(2);
		return;
	}
	dataLen = len - 1;
	type = data[0];
	switch(type)
	{
		case 0:		
			pcSendTradeReady(0);
			break;
		case 1:
			if(dataLen >= 2)
			{
				pcSendDetailTrade(1,INTEG16(data[1],data[2]));
			}			
			break;
		default:
			break;
	}
}




/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcClearReq(unsigned char len,unsigned char data)
{
	unsigned char mode,sbuf[2];
	if(len < 1)
		return ;

	mode = data;
	if(mode == 0)
	{
		pc_trade_info_clear(2);
	}
	else if(mode == 1)
	{
		pc_trade_info_clear(1);
	}

	PC_DELAY_500MS;
	sbuf[0] = mode;
	sbuf[1] = 0x01;
	pcReply(CLEAR_RPT,2,sbuf);
}


/*********************************************************************************************************
** Function name:     	pcSendSysPara
** Descriptions:	   发送系统参数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSendSysPara(unsigned char type)
{
	unsigned char buf[100],index1 = 0,i;
	unsigned int temp,tampAmount;
	unsigned char temp8;

	buf[index1++] = type;//type段
	buf[index1++] = SystemPara.Language;
	//add by yoc 2013 10.25  index1 + 8
	temp = SystemPara.macSN;
	index1+=4;
	buf[index1 - 1] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 2] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 3] = HEC_to_BCD(temp % 100);
	temp /= 100;
	buf[index1 - 4] = HEC_to_BCD(temp % 100);	

	buf[index1++] = 0x01;//SystemPara.armSN[0];
	buf[index1++] = 0x16;//SystemPara.armSN[1];
	
	buf[index1++] = SystemPara.vmcType;
	if(SystemPara.BillITLValidator == 1 && SystemPara.BillValidatorType == MDB_BILLACCEPTER)
	{		
		buf[index1++] = (SystemPara.BillRecyclerType == 2) ? 4: 3;
	}
	else
		buf[index1++] = SystemPara.BillValidatorType;
	
	buf[index1++] = SystemPara.CoinAcceptorType;
	buf[index1++] = SystemPara.CoinChangerType;
	buf[index1++] = SystemPara.CashlessDeviceType;
	for(i = 0;i < 8;i++)
	{
		tampAmount = pcAmountToRatio(SystemPara.BillValue[i]);
		buf[index1++] = pcEncodAmount(tampAmount);			
	}
	for(i = 0;i < 8;i++)
	{
		tampAmount = pcAmountToRatio(SystemPara.CoinValue[i]);
		buf[index1++] = pcEncodAmount(tampAmount);						
	}
	for(i = 0; i < 3;i++)
	{
		tampAmount = pcAmountToRatio(SystemPara.HopperValue[i]);
		buf[index1++] = pcEncodAmount(tampAmount);			
	}
	buf[index1++] = SystemPara.SaleTime;

	tampAmount = pcAmountToRatio(SystemPara.MaxValue);
	buf[index1++] = H0UINT32(tampAmount);
	buf[index1++] = H1UINT32(tampAmount);
	buf[index1++] = L0UINT32(tampAmount);
	buf[index1++] = L1UINT32(tampAmount);

	temp8 =   (SystemPara.GeziDeviceType == 0) ? SystemPara.Channel : 0;
	buf[index1++] = (temp8 << 4) | (SystemPara.ChannelType & 0x0f);//add by yoc 2013.10.8 
	buf[index1++] = (SystemPara.SubChannel << 4) | (SystemPara.SubBinChannelType & 0x0f);

	if(SystemPara.EasiveEnable == 1 && SystemPara.PcEnable == 2)
	{
		buf[index1++] = 4;
	}
	else
	 	buf[index1++] = SystemPara.PcEnable;

	buf[index1++] = SystemPara.DecimalNum;

	//B3
	buf[index1++] = (SystemPara.hefangGui & 0x01) |
					((SystemPara.threeSelectKey & 0x01) << 1)	|
					((SystemPara.hefangGuiKou & 0x01) << 2);
	//B2
	buf[index1++] = ((SystemPara.XMTTemp & 0x01) << 5) |
					(0 << 4) |
					((UserPara.LEDCtr.flag & 0x01) << 3) |
					((UserPara.LEDCtr2Bin.flag & 0x01) << 2) |
					((UserPara.CompressorCtr.flag & 0x01) << 1) |
					((UserPara.CompressorCtr2Bin.flag & 0x01) << 0)
					;


	//B1
	temp8 = (SystemPara.Logo == 0 ? 0 : 1);
	buf[index1++] = (SystemPara.CunhuoKou & 0x01) |
					((SystemPara.GOCIsOpen & 0x01) << 1) |
					((UserPara.TransEscape & 0x01) << 2) |
					((UserPara.TransMultiMode & 0x01) << 3) |
					((temp8 & 0x01) << 4) |
					((SystemPara.UserSelectKeyBoard & 0x01) << 5) |
					((SystemPara.SubBinOpen & 0x01) << 6);


	buf[index1++] = SystemPara.Logo;
	buf[index1++] = SystemPara.DecimalNumExt;//add by yoc 2014.10.15

	for(i = 0;i < 7;i++)
	{
		tampAmount = SystemPara.RecyclerValue[i];
		if(tampAmount)
		{			
			buf[index1 + 1] =  pcEncodAmount(pcAmountToRatio(tampAmount));
			break;
		}
	}

	index1++;

	tampAmount = pcAmountToRatio(SystemPara.BillEnableValue);
	buf[index1++] = H0UINT32(tampAmount);
	buf[index1++] = H1UINT32(tampAmount);
	buf[index1++] = L0UINT32(tampAmount);
	buf[index1++] = L1UINT32(tampAmount);
	
	
	PC_DELAY_500MS;
	pcReply(SYSPARA_RPT,index1,buf);
	
}


/*********************************************************************************************************
** Function name:     	pcSetSysPara
** Descriptions:	    设置系统参数
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSetSysPara(unsigned char type,unsigned char *buf)
{

	unsigned char index1 = 0,i,temp,rst[3] = {0};
	unsigned int mac_sn,tempAmount;


	SystemPara.DecimalNumExt = buf[44];

	
	SystemPara.Language = buf[index1++];

	mac_sn = (unsigned int)BCD_to_HEC(buf[index1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 1]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 2]);
	mac_sn = mac_sn * 100 +(unsigned int)BCD_to_HEC(buf[index1+ 3]);	
	
	SystemPara.macSN = mac_sn;
	index1 += 4;
	SystemPara.armSN[0] = buf[index1++];
	SystemPara.armSN[1] = buf[index1++];
	SystemPara.vmcType = buf[index1++];	

	temp = buf[index1++];
	if(temp == 3)
	{
		SystemPara.BillITLValidator = 1;
		SystemPara.BillValidatorType = MDB_BILLACCEPTER;
		SystemPara.BillRecyclerType = 0;
	}
	else if(temp == 4)
	{
		SystemPara.BillITLValidator = 1;
		SystemPara.BillValidatorType = MDB_BILLACCEPTER;
		SystemPara.BillRecyclerType = 2;
	}
	else
	{
		SystemPara.BillValidatorType = temp;
		SystemPara.BillITLValidator = 0;
		SystemPara.BillRecyclerType = 0;
	}
		

	SystemPara.CoinAcceptorType = buf[index1++];
	SystemPara.CoinChangerType = buf[index1++];
	SystemPara.CashlessDeviceType = buf[index1++];
	
	for(i = 0;i < 8;i++)
	{
		tempAmount = pcAnalysisAmount(buf[index1++]);
		SystemPara.BillValue[i] = pcAmountFromRatio(tempAmount);
				
	}
	for(i = 0;i < 8;i++)
	{
		tempAmount = pcAnalysisAmount(buf[index1++]);
		SystemPara.CoinValue[i] = pcAmountFromRatio(tempAmount);
	}
	for(i = 0; i < 3;i++)
	{
		tempAmount = pcAnalysisAmount(buf[index1++]);
		SystemPara.HopperValue[i] = pcAmountFromRatio(tempAmount);	
	}
	SystemPara.SaleTime = buf[index1++];

	tempAmount = INTEG32(buf[index1++],buf[index1++],buf[index1++],buf[index1++]);
	SystemPara.MaxValue = pcAmountFromRatio(tempAmount);
	

	temp = buf[index1++];
	SystemPara.Channel = (temp & 0xf0) >> 4;
	
	
	SystemPara.ChannelType = temp & 0x0f;
	
	temp = buf[index1++];
	SystemPara.SubChannel = (temp & 0xf0) >> 4;
	SystemPara.SubBinChannelType = temp & 0x0f;

	temp = buf[index1++];
	if(temp == 4)
	{
		SystemPara.EasiveEnable = 1;
		SystemPara.PcEnable = 2;
	}
	else
	{
		SystemPara.PcEnable = temp;
		SystemPara.EasiveEnable = 0;
	}	
	SystemPara.DecimalNum = buf[index1++];

	//B3
	temp = buf[index1++];//
	SystemPara.hefangGui = temp & 0x01;
	SystemPara.threeSelectKey = (temp >> 1) & 0x01;
	SystemPara.hefangGuiKou = (temp >> 2) & 0x01;

	//B2
	temp = buf[index1++];
	UserPara.CompressorCtr2Bin.flag = temp & 0x01;
	UserPara.CompressorCtr.flag = (temp >> 1) & 0x01;
	
	UserPara.LEDCtr2Bin.flag = (temp >> 2) & 0x01;
	UserPara.LEDCtr.flag = (temp >> 3) & 0x01;
	SystemPara.XMTTemp = (temp >> 5) & 0x01;
		
	//B1
	temp = buf[index1++];
	
	SystemPara.CunhuoKou = temp & 0x01;
	SystemPara.GOCIsOpen = 		(temp >> 1) & 0x01;
	UserPara.TransEscape = 	(temp >> 2) & 0x01;
	UserPara.TransMultiMode = (temp >> 3) & 0x01;
	//SystemPara.Logo = 			(temp >> 4) & 0x01;
	SystemPara.UserSelectKeyBoard = (temp >> 5) & 0x01;
	SystemPara.SubBinOpen = 	(temp >> 6) & 0x01;
	
	SystemPara.Logo = buf[index1++];
	
	SystemPara.DecimalNumExt = buf[index1++];

	index1++;//无法设置循环斗面值
	
	SystemPara.BillEnableValue = pcAmountFromRatio(INTEG32(buf[index1++],buf[index1++],buf[index1++],buf[index1++]));
	

	if(SystemPara.Channel == 0 && SystemPara.SubBinOpen == 0)
		SystemPara.GeziDeviceType = 1;
	else
		SystemPara.GeziDeviceType = 0;
	
	WriteDefaultSystemPara(SystemPara,0);

	UserPara.TraceFlag = mainTrace;
	WriteUserSystemPara(UserPara,0);
	PC_DELAY_500MS;
	
	rst[0] = type;
	rst[1] = 1;
	pcReply(SYSPARA_RPT,2,rst);
	
}

/*********************************************************************************************************
** Function name:     	pcSysParaReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char pcSysParaReq(unsigned char len,unsigned char *data)
{

	switch(data[0])
	{
		case 0x00:
			pcSendSysPara(0);
			break;
		case 0x01:
			pcSetSysPara(1,&data[1]);
			break;
	}

	return len;
}

/*********************************************************************************************************
** Function name:     	pcSendHefanHuoDao
** Descriptions:	    发送盒饭柜货道信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSendHefanHuoDao(unsigned char type,unsigned char *data)
{
	unsigned char buf[128] ={0},index = 0,i,physicNo,no,cabinet,HF_cabinet,j,sendType;
	unsigned char err;
	unsigned int temp32;

	cabinet = data[0];//柜号
	sendType = data[1];//发送方式
	no = data[2];//层号 or 货道号

	buf[index++] = type;
	buf[index++] = cabinet;
	buf[index++] = sendType;
	
	
	HF_cabinet = get_hefan_Index(cabinet);
	if(HF_cabinet == 0)//柜号不正确
	{
		buf[index++] = 0xEE;
		PC_DELAY_500MS;
		pcReply(HUODAO_RPT,index,buf);
		return ;
	}
	
	
	if(sendType == 0x01)//level
	{
		//第level层
		buf[index++] = no;	
		if(!no || no > 5)
		{
			buf[index++] = 0xFF;
			PC_DELAY_500MS;
			pcReply(HUODAO_RPT,index,buf);
			return;
		}
			

		if(no == 1)//获取第一层时将会与盒饭柜通信获取信息
		{
			err = HeFanGuiHandle(0,HEFANGUI_CHAXUN,HF_cabinet - 1);
			if(err == 1)
			{
				setHefanCabinetState(HF_cabinet,HEFAN_EXIST,1);//表示存在柜子
			}
			else
			{
				setHefanCabinetState(HF_cabinet,HEFAN_EXIST,0);
			}
		}
			
		for(i = (no - 1) * 20;i< no * 20;i++)
		{	
			//获取逻辑货道
			physicNo = i + 1;	
			temp32 = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_PRICE);
			temp32 = pcAmountToRatio(temp32);
			buf[index++] = L0UINT32(temp32);//单价
			buf[index++] = L1UINT32(temp32);//单价
			if(getHefanCabinetState(HF_cabinet,HEFAN_EXIST) == 1)
				buf[index++] = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_STATE);//状态
			else 
				buf[index++] = 0;
			
			buf[index++] = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_TRADE_NUM);//销售量			
			buf[index++] = (unsigned char)hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_ID);//ID									
		}			
						
	}
	else if(sendType == 0x02)//aisle
	{
			//获取逻辑货道
			physicNo = no;	
			temp32 = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_PRICE);
			temp32 = pcAmountToRatio(temp32);
			buf[index++] = L0UINT32(temp32);//单价
			buf[index++] = L1UINT32(temp32);//单价
			
			buf[index++] = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_STATE);//状态
			buf[index++] = hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_TRADE_NUM);//销售量			
			buf[index++] = (unsigned char)hefanGetParaVlaue(HF_cabinet,physicNo,HEFAN_HUODAO_ID);//ID									
	}
	else
	{
		print_warning(no);
		return;
	}

	print_log("index= %d\r\n",index);
	for(j = 0;j < index;j++)
		print_log("%x ",buf[j]);
	print_log("\r\n");
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index,buf);
}



/*********************************************************************************************************
** Function name:     	pcSendHuoDao
** Descriptions:	    发送货道信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSendHuoDao(unsigned char type,unsigned char *data)
{
	unsigned char buf[120] ={0},index = 0,i,physicNo,no,levelOpen,cabinet,j;
	unsigned int temp32;
	buf[index++] = type;
	
	cabinet = data[0];//柜号
	buf[index++] = data[0];
	buf[index++] = data[1];//发送方式
	no = data[2];//层号 or 货道号
	if(data[1] == 0x01)//level
	{
		//第level层
		buf[index++] = no;	
		levelOpen = GetLevelOrColumnState(cabinet,2,no);
	    buf[index++] = levelOpen;
		if(levelOpen)
		{
			for(i =0;i< 10;i++)
			{	
				//获取逻辑货道
				if(i >= 9)  			
					physicNo = no * 10;
				else
					physicNo = no * 10 + i + 1;				
				buf[index++] = physicNo;	//货道编号
				buf[index++] = FromPhysicGetLogicNum(cabinet,physicNo);
				buf[index++] = GetLevelOrColumnState(cabinet,1,physicNo);//货道开启标志
				temp32 = GetColumnStateByPhysic(cabinet,physicNo,1);
				temp32 = pcAmountToRatio(temp32);
				buf[index++] = L0UINT32(temp32);//单价
				buf[index++] = L1UINT32(temp32);//单价
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,3);//状态
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,2);//剩余数量			
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,4);//最大存货量
				buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,5);//销售量			
				buf[index++] = (unsigned char)GetColumnStateByPhysic(cabinet,physicNo,7);//ID									
			}			
		}
		else
		{
			for(i =0;i< 10;i++)
			{
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
				buf[index++] = 0;
			}		
			
		}				
		
	}
	else if(data[1] == 0x02)//aisle
	{
			physicNo = no ;
			buf[index++] = physicNo;	
			buf[index++] = FromPhysicGetLogicNum(cabinet,physicNo);
			buf[index++] = GetLevelOrColumnState(cabinet,1,physicNo);//货道开启标志			
			temp32 = GetColumnStateByPhysic(cabinet,physicNo,1);
			temp32 = pcAmountToRatio(temp32);
			buf[index++] = L0UINT32(temp32);//单价
			buf[index++] = L1UINT32(temp32);//单价
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,3);//状态
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,2);//剩余数量			
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,4);//最大存货量
			buf[index++] = GetColumnStateByPhysic(cabinet,physicNo,5);//销售量			
			buf[index++] = (unsigned char)GetColumnStateByPhysic(cabinet,physicNo,7);//ID		
	}
	else
	{
		print_warning(no);
		return;
	}

	print_log("index= %d\r\n",index);
	for(j = 0;j < index;j++)
		print_log("%x ",buf[j]);
	print_log("\r\n");
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index,buf);
		
}


/*********************************************************************************************************
** Function name:     	pcSetHefanHuodao
** Descriptions:	   设置盒饭柜货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void  pcSetHefanHuodao(unsigned char type,unsigned char *data)
{
	unsigned char sBuf[10] ={0},index = 0,i,physicNo,no,cabinet,HF_cabinet,index1 = 0,setType;
	unsigned int tempAmount;
	cabinet = data[index++];	//柜号
	setType = data[index++];//发送方式
	no = data[index++];//层号 or 货道号

	sBuf[index1++] = type;
	sBuf[index1++] = cabinet;
	sBuf[index1++] = setType;

	
	HF_cabinet = get_hefan_Index(cabinet);
	if(HF_cabinet == 0)
	{
		sBuf[index1++] = 0x00;
		PC_DELAY_500MS;
		pcReply(HUODAO_RPT,index1,sBuf);
		return;
	}

	sBuf[index1++] = 0x01;
	
	if(setType == 0x01)//level
	{
		if(!no || no > 5)
		{
			sBuf[index1++] = 0x00;
			PC_DELAY_500MS;
			pcReply(HUODAO_RPT,index1,sBuf);
			return;
		}
			
			
		for(i = (no - 1) * 20;i< no * 20;i++)
		{	
			//获取逻辑货道
			physicNo = i + 1;	
			tempAmount = pcAmountFromRatio(INTEG16(data[index++],data[index++]));
			hefanSetParaVlaue(HF_cabinet,physicNo,tempAmount,HEFAN_HUODAO_PRICE);	
			hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_STATE);//状态
			//hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_TRADE_NUM);//销售量			
			index++;
			hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_ID);//ID									
		}	


		
						
	}
	else if(setType == 0x02)//aisle
	{
			//获取逻辑货道
			physicNo = no;	
			tempAmount = pcAmountFromRatio(INTEG16(data[index++],data[index++]));
			hefanSetParaVlaue(HF_cabinet,physicNo,tempAmount,HEFAN_HUODAO_PRICE);		
			hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_STATE);//状态
			hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_TRADE_NUM);//销售量			
			hefanSetParaVlaue(HF_cabinet,physicNo,data[index++],HEFAN_HUODAO_ID);//ID												
	}
	else
	{
		print_warning(no);
		sBuf[index1++] = 0x00;
		return;
	}

	
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index1,sBuf);

}





/*********************************************************************************************************
** Function name:     	pcSetHuoDao
** Descriptions:	   设置货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void  pcSetHuoDao(unsigned char type,unsigned char *buf)
{
	unsigned char index = 0,index1 = 0,reqMode,sBuf[10] = {0},cabinet,no,
	aisleType,phsicyNo,i,j;
	unsigned int tempAmount;
	
	cabinet = buf[index++];
	reqMode = buf[index++];
	no = buf[index++];//层号 or 货道物理编号
	sBuf[index1++] = type;
	sBuf[index1++] = cabinet;
	sBuf[index1++] = reqMode;
	
	if(reqMode == 0x01)//level
	{
		aisleType = (SystemPara.ChannelType == 1) ? 10 : 8;
		for(i =0;i< aisleType;i++)
		{	
			print_log("cabinet = %d,index = %d\r\n",cabinet,index);
			for(j = 0; j < 7;j++)
				print_log("%x ",buf[index + j]);
			print_log("\r\n");
			
			phsicyNo = buf[index++];
			tempAmount = pcAmountFromRatio(INTEG16(buf[index++],buf[index++]));
			SetColumnParamByPhysic(cabinet,phsicyNo,1,tempAmount);
			SetColumnParamByPhysic(cabinet,phsicyNo,3,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,2,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,4,buf[index++]);
			SetColumnParamByPhysic(cabinet,phsicyNo,7,buf[index++]);
			
			
		
		}
		
	}
	else if(reqMode == 0x02)//aisle
	{	
		phsicyNo = no;	
		tempAmount = pcAmountFromRatio(INTEG16(buf[index++],buf[index++]));
		SetColumnParamByPhysic(cabinet,phsicyNo,1,tempAmount);
		SetColumnParamByPhysic(cabinet,phsicyNo,3,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,2,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,4,buf[index++]);
		SetColumnParamByPhysic(cabinet,phsicyNo,7,buf[index++]);		
		
	}
	else
	{
		print_warning(reqMode);
		return;
	}
	
	sBuf[index1++] = 0x01;
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index1,sBuf);

	
}



/*********************************************************************************************************
** Function name:     	pcInitHuoDaoRpt
** Descriptions:	   初始化货道结果上报
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcInitHuoDaoRpt(unsigned type,unsigned char result)
{
	unsigned char rst[3];
	rst[0] = type;
	
	rst[1] = result;
	pcReply(HUODAO_RPT,3,rst);
	busyFlag = 0;
	
}

/*********************************************************************************************************
** Function name:     	pcInitAisle
** Descriptions:	   初始化货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcInitHuoDao(unsigned char type,unsigned char *data)
{	
	unsigned char cabinet,rst[3];
	cabinet = data[0];
	
	rst[0] = type;
	rst[1] = cabinet;
#if 0
	busyFlag = 1;
	pcPost(MBOX_HUODAO_INIT);	
#else
	if(cabinet < 3)
	{
		hd_init(cabinet);
		rst[2] = 0x01;
	}	
	else
		rst[2] = hefanHuodaoInit(get_hefan_Index(cabinet));
	
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,3,rst);
#endif			
}




/*********************************************************************************************************
** Function name:     	pcSaveHuodao
** Descriptions:	   保存货道信息到FLASH
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcSaveHuodao(unsigned char type,unsigned char cabinet)
{
	unsigned char rst[4];
	if(cabinet < 3)
		ChannelSaveParam();
	else 
		save_hefan_huodao_info(cabinet);//添加盒饭柜保存
	
	rst[0] = type;
	rst[1] = cabinet;
	rst[2] = 0x01;
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,3,rst);	
}


/*********************************************************************************************************
** Function name:     	pcONOFFHuodao
** Descriptions:	   关闭货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

static void pcONOFFHuodao(unsigned char type, unsigned char *buf)
{
	unsigned char  i,j,index = 0,aisleType,leveOpen,logicNo,aisleOpen,rst[5],cabinet; 
	unsigned short temp;
	

	//柜号
	cabinet = buf[index++];
	if(cabinet == 1)
		aisleType = (SystemPara.ChannelType == 0) ? 8: 10;
	else if(cabinet == 2 && SystemPara.SubBinOpen)
		aisleType = (SystemPara.SubBinChannelType == 0) ? 8 : 10;
	else 
		return;

	for(i = 0; i < 8;i++)
	{
		temp = INTEG16(buf[index++],buf[index++]);
		leveOpen = (temp >> 15) & 0x01;
		if(leveOpen)
		{
			//old can't execute open
			ChannelExChannge(i + 1,2,1,cabinet);
		}	
		else
			ChannelExChannge(i + 1,2,2,cabinet);
		
		if(leveOpen)
		{
			for(j = 0;j < aisleType;j++)
			{				
				if(j >= 9)
					logicNo = (i + 1) * 10;
				else
					logicNo = (i + 1) * 10 + j + 1;						
				aisleOpen = (temp >> j) & 0x01;
				if(aisleOpen)
				{
					ChannelExChannge(logicNo,1,1,cabinet);	
				}
				else
				{
					ChannelExChannge(logicNo,1,2,cabinet);	
				}								
			}
		}				
	}
	ChannelSaveParam();	
	rst[0] = type;
	rst[1] = cabinet;
	rst[2] = 0x01;
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,3,rst);

	
}

static void pcAddGoods(unsigned char type,unsigned char *data)
{										  
	unsigned char addGoodsType,cabinet,no,err,
		    rbuf[10] = {0},index = 0;

	cabinet = data[0];	
	addGoodsType = data[1];
	no = data[2];
	rbuf[index++] = type;
	rbuf[index++] = cabinet;
	rbuf[index++] = addGoodsType;
	switch(addGoodsType)
	{
		case 0://all
			if(cabinet < 3)
				err = AddColumnGoods(cabinet,3,no);
			else
				err = hefanAddgoodsHandle(get_hefan_Index(cabinet),0);
			break;

		case 1://level
			err = AddColumnGoods(cabinet,2,no);
			break;

		case 2://aisle
			if(cabinet < 3)
				err = AddColumnGoods(cabinet,1,no);
			else
				err = hefanAddgoodsHandle(get_hefan_Index(cabinet),no);
			break;

		default:
			err = 0;
		break;
	}
	if(err)
	{
		if(cabinet < 3)
			ChannelSaveParam();
		else
			save_hefan_huodao_info(get_hefan_Index(cabinet));
		
		rbuf[index++] = 0x01;
	}
	else
	{
		rbuf[index++] = 0x00;
	}
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index,rbuf);
}

/*********************************************************************************************************
** Function name:     	pcTestAisle
** Descriptions:	   测试货道
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcTestAisle(unsigned char type,unsigned char *data)
{
	
	unsigned char cabinet;
	pc_vmc_mail.msg[0] = type;//类型
	pc_vmc_mail.msg[1] = data[0];//柜号
	pc_vmc_mail.msg[2] = data[1];	//逻辑号	
	
	busyFlag = 1;
	pcPost(MBOX_HUODAO_TEST);	
	if((cabinet == 1 && SystemPara.Channel > 1) ||
				(cabinet == 2 && SystemPara.SubChannel > 1 && SystemPara.SubBinOpen))
		Timer.pcm_handle_timeout = 120;
	else
		Timer.pcm_handle_timeout = 15;
	

}




/*********************************************************************************************************
** Function name:     	pcTestAisleRpt
** Descriptions:	   测试货道结果上报
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcTestAisleRpt(PCCONMSG *mail)
{
	unsigned char sendBuf[10] = {0};
		
	if(mail == NULL)
	{
		print_warning(2);
		busyFlag = 0;
		return;
	}
	else
	{
		sendBuf[0] = mail->msg[0];
		sendBuf[1] = mail->msg[1];
		sendBuf[2] = mail->msg[2];
		sendBuf[3] = mail->msg[3];;
		pcReply(HUODAO_RPT,4,sendBuf);
		busyFlag = 0;
		
#if 0
		cabinet = mail->msg[1];
		if((cabinet == 1 && SystemPara.Channel == 2) ||
			(cabinet == 2 && SystemPara.SubChannel == 2 && SystemPara.SubBinOpen))
		{
			err = LiftTableTestByPcPend(LIFTTABLE_OUTGOODSRST);
			if(err == 1)
			{
				result = 0;
			}
			else
				result = err;
			
			if(result != 0xFF)
			{
				sendBuf[0] = mail->msg[0];
				sendBuf[1] = mail->msg[1];
				sendBuf[2] = mail->msg[2];
				sendBuf[3] = result;
				if(result == 0)
				{
					if(hd_get_by_logic(mail->msg[1],mail->msg[2],HUODAO_TYPE_STATE) != HUODAO_STATE_EMPTY)
						hd_set_by_logic(mail->msg[1],mail->msg[2],HUODAO_TYPE_STATE,HUODAO_STATE_NORMAL);		
				}
				pcReply(HUODAO_RPT,4,sendBuf);	
				busyFlag = 0;
			}
		}
		else
		{
			result = ColumnTestByPcPend();	
			if(result != 0xFF)
			{
				sendBuf[0] = mail->msg[0];
				sendBuf[1] = mail->msg[1];
				sendBuf[2] = mail->msg[2];
				sendBuf[3] = result;	
				if(result == 0)
				{
					if(hd_get_by_logic(mail->msg[1],mail->msg[2],HUODAO_TYPE_STATE) != HUODAO_STATE_EMPTY)
						hd_set_by_logic(mail->msg[1],mail->msg[2],HUODAO_TYPE_STATE,HUODAO_STATE_NORMAL);
						
				}
				pcReply(HUODAO_RPT,4,sendBuf);	
				busyFlag = 0;
			}
		}
#endif

	}
	
}


/*********************************************************************************************************
** Function name:     	pcsendKeyNo
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcsendKeyNo(unsigned char type,unsigned char *data)
{

	unsigned char index = 0,rbuf[80] = {0},cabinet,recvIndex = 0,sendType,aisleNo;
	unsigned char i,j,physicNo;
	
	cabinet = data[recvIndex++];	
	sendType = data[recvIndex++];
	aisleNo = data[recvIndex++];

	//类型 0x07
	rbuf[index++] = type;
	//柜号
	rbuf[index++] = cabinet;
	//发送类型
	rbuf[index++] = sendType;
	if(sendType == 0x00)
	{
		//全货道选货按键板
		for(i = 0;i < 8;i++)
		{
			for(j =0;j< 10;j++)
			{	
				//获取物理货道
				if(j >= 9)  			
					physicNo = (i + 1) * 10;
				else
					physicNo = (i + 1) * 10 + j + 1;				
				rbuf[index++] = GetColumnStateByPhysic(cabinet,physicNo,6);
			}
		}	
	}
	else
	{
		aisleNo =aisleNo;
	}
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index,rbuf);
}


/*********************************************************************************************************
** Function name:     	pcHuodaoOutGoods
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcHuodaoOutGoods(unsigned char type,unsigned char *data)
{
	unsigned char cabinet;
	pc_vmc_mail.msg[0] = type;//类型
	pc_vmc_mail.msg[1] = data[0];//柜号
	pc_vmc_mail.msg[2] = data[1];	//逻辑号	
	busyFlag = 1;

	
	pcPost(MBOX_HUODAO_OUTGOODS);
	
	if((cabinet == 1 && SystemPara.Channel == 2) ||
				(cabinet == 2 && SystemPara.SubChannel == 2 && SystemPara.SubBinOpen))
		Timer.pcm_handle_timeout = 120;
	else
		Timer.pcm_handle_timeout = 30;

	
}


/*********************************************************************************************************
** Function name:     	pcHuodaoOutGoods
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcHuodaoOutGoodsRpt(PCCONMSG *mail)
{
	unsigned char sendBuf[10] = {0};
		
	if(mail == NULL)
	{
		print_warning(2);
		busyFlag = 0;
		return;
	}
	else
	{
		sendBuf[0] = mail->msg[0];
		sendBuf[1] = mail->msg[1];
		sendBuf[2] = mail->msg[2];
		sendBuf[3] = mail->msg[3];;
		pcReply(HUODAO_RPT,4,sendBuf);
		busyFlag = 0;
	}
}


/*********************************************************************************************************
** Function name:     	pcsetKeyNo
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcsetKeyNo(unsigned char type,unsigned char *data)
{
	unsigned char index = 0,rbuf[80] = {0},cabinet,recvIndex = 0,sendType,physicNo;
	unsigned char i,j;
	
	cabinet = data[recvIndex++];

	sendType = data[recvIndex++];
	
	rbuf[index++] = type;
	rbuf[index++] = cabinet;
	rbuf[index++] = sendType;
	if(sendType == 0x00)
	{
		//全货道选货按键板
		for(i = 0;i < 8;i++)
		{
			for(j =0;j< 10;j++)
			{	
				//获取逻辑货道
				if(j >= 9)  			
					physicNo = (i + 1) * 10;
				else
					physicNo = (i + 1) * 10 + j + 1;					
				SetColumnParamByPhysic(cabinet,physicNo,6,data[recvIndex++]);
			}
		}	
		rbuf[index++] = 0x01;
	}
	else
	{
		OSTimeDly(100/5);
	}
	PC_DELAY_500MS;
	pcReply(HUODAO_RPT,index,rbuf);
}


/*********************************************************************************************************
** Function name:     	pcSysParaReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcHuodaoReq(unsigned char len,unsigned char *data)
{
	unsigned char cabinetNo,type;
	if(len < 1)
	{
		print_warning(len);
		return;
	}

	type = data[0];
	cabinetNo = data[1];

	
	switch(type)
	{
		case 0x00:
			if(cabinetNo < 3)
				pcSendHuoDao(0x00,&data[1]);
			else
				pcSendHefanHuoDao(0x00,&data[1]);
			break;
		case 0x01:
			if(cabinetNo < 3)
				pcSetHuoDao(0x01,&data[1]);
			else
				pcSetHefanHuodao(0x01,&data[1]);
			break;
		case 0x02:
			pcInitHuoDao(0x02,&data[1]);
			break;
		case 0x03:
			pcSaveHuodao(0x03,data[1]);
			break;
		case 0x04:
			pcONOFFHuodao(0x04,&data[1]);
			break;
		case 0x05:
			pcAddGoods(0x05,&data[1]);
			break;
		case 0x06:
			pcTestAisle(0x06,&data[1]);
			break;
		case 0x07:
			pcsendKeyNo(0x07,&data[1]);
			break;
		case 0x08:
			pcsetKeyNo(0x08,&data[1]);
			break;
		case 0x0A:
			pcHuodaoOutGoods(0x0A,&data[1]);
			break;
		default:
			break;
	}
}

/*********************************************************************************************************
** Function name:     	pcFactorReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcFactorReq(void)
{
	unsigned char rst = 1;
	FactoryDefaultSystemPara();
	PC_DELAY_500MS;
	pcReply(FACTORRECOVER_RPT,1,&rst);
}


static void pcFactorSet(void)
{
	unsigned char rst = 1;
	RstFactoryDefaultSystemPara();
	PC_DELAY_500MS;
	pcReply(FACTOR_SET_RPT,1,&rst);
}


//烧写ID
static void pcIdSetReq(unsigned char buflen,unsigned char *buf)
{
	unsigned char rst = 1;
	unsigned char i,len;
	len = sizeof(stMacSn.id);
	
	for(i = 0;i < len;i++){
		stMacSn.id[i] = buf[i];
	}
	LOG_writeId(&stMacSn);
	PC_DELAY_500MS;
	pcReply(ID_WRITE_RPT,1,&rst);
}


static void pcIdReadReq(void)
{
	unsigned char rst = 1,buf[64] = {0},in = 0;
	unsigned char i,len;

	len = sizeof(stMacSn.id);
	if(LOG_readId(&stMacSn) == 1){
		for(i = 0,in = 0;i < len;i++){
			buf[in++] = stMacSn.id[i]; 
		}
	}
	else{
		in = len;
		memset(buf,0,sizeof(buf));
	}
	PC_DELAY_500MS;
	pcReply(ID_READ_RPT,in,buf);
}


/*********************************************************************************************************
** Function name:     	pcDateTimeReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcDateTimeReq(unsigned char len,unsigned char *buf)
{
	unsigned char type,index1 = 0,sbuf[10] = {0}, i;
	RTC_DATE rtc;

	for(i = 0;i < 10;i++)
	{
		print_log("%x ",buf[i]);
	}
	print_log("\r\n");
	

	if(len < 1)
	{
		print_warning(len);
		return; 		
	}
	type = buf[0];
	sbuf[index1++] = type;
	if(type == 0x00)//set Time
	{	
		
		rtc.year = (unsigned short)BCD_to_HEC(buf[1])*100+ BCD_to_HEC(buf[2]);
		rtc.month = BCD_to_HEC(buf[3]);
		rtc.day = BCD_to_HEC(buf[4]);
		rtc.hour = BCD_to_HEC(buf[5]);
		rtc.minute = BCD_to_HEC(buf[6]);
		rtc.second = BCD_to_HEC(buf[7]);
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
		rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
	//设置时间不准调度		
	OSSchedLock();
		RTCSetTime(&rtc);
		RTCReadTime(&RTCData);
	OSSchedUnlock();
		print_log("dateSet:%d,%d,%d,%d,%d,%d\r\n",rtc.year ,
			rtc.month,rtc.day,rtc.hour,rtc.minute,rtc.second);
		sbuf[index1++] = 0x01;
	#if 0	
		sbuf[index1++] =  HEC_to_BCD(rtc.year/100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.year%100) ;
		sbuf[index1++] =  HEC_to_BCD(rtc.month);
		sbuf[index1++] =  HEC_to_BCD(rtc.day);
		sbuf[index1++] =  HEC_to_BCD(rtc.hour);
		sbuf[index1++] =  HEC_to_BCD(rtc.minute);
		sbuf[index1++] =  HEC_to_BCD(rtc.second);
	#endif	
	}
	else if(type == 0x01)//get Time
	{
		sbuf[index1++] =  HEC_to_BCD(RTCData.year/100) ;
		sbuf[index1++] =  HEC_to_BCD(RTCData.year%100) ;
		sbuf[index1++] =  HEC_to_BCD(RTCData.month);
		sbuf[index1++] =  HEC_to_BCD(RTCData.day);
		sbuf[index1++] =  HEC_to_BCD(RTCData.hour);
		sbuf[index1++] =  HEC_to_BCD(RTCData.minute);
		sbuf[index1++] =  HEC_to_BCD(RTCData.second);
	}

	PC_DELAY_500MS;
	pcReply(DATETIME_RPT,index1,sbuf);
	
}


/*********************************************************************************************************
** Function name:     	pcStatusReq
** Descriptions:	    整机状态信息上报给PC机
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcStatusReq(unsigned char len,unsigned char *buf)
{
	unsigned char type,index1 = 0,rst[10] = {0},temp,temp1 = 0,err;
	len = len;
	type = buf[0];
	rst[index1++] = type;
	if(type == 0)//获取整机状态
	{
		//售货机状态
		if(DeviceStateBusiness.ErrorInit)
			rst[index1++] = ErrorStatus(4);//11 1GOC  2 bill 3 coin 4 vmc
		else
			rst[index1++] = 0x04; //正在初始化中
		//纸币器类型
		rst[index1++] = SystemPara.BillValidatorType;
		//纸币器状态
		err = ErrorStatus(2);
		if(err == 0)
		{
			temp = 0;
		}
		else if(err == 1)
		{
			temp = 0x80;
		}
		else 
		{				
			temp = (DeviceStateBusiness.BillCommunicate &0x01) |
				((DeviceStateBusiness.Billmoto & 0x01)<< 1) |
				((DeviceStateBusiness.Billsensor & 0x01)<< 2) |
				((DeviceStateBusiness.Billromchk & 0x01)<< 3)|
				((DeviceStateBusiness.Billjam & 0x01)<< 4) |
				((DeviceStateBusiness.BillremoveCash & 0x01)<< 5) |
				((DeviceStateBusiness.Billdisable & 0x01)<< 6) |
				(0 << 7); 
		}
		rst[index1++] = 0;//备用
		rst[index1++] = temp;

		//硬币器类型

		rst[index1++] = (SystemPara.CoinAcceptorType << 4) | 
						(SystemPara.CoinChangerType & 0x0F);
		//硬币器状态
		err = ErrorStatus(3);

		rst[index1] = 0;
		rst[index1+ 1] = 0;
		rst[index1+ 2] = 0;
		rst[index1+ 3] = 0;
		if(err)
		{
			rst[index1+ 3] = (DeviceStateBusiness.CoinCommunicate & 0x01) |
				((DeviceStateBusiness.Coinsensor & 0x01)<< 1) |
				((DeviceStateBusiness.Cointubejam & 0x01)<< 2) |
				((DeviceStateBusiness.Coinromchk & 0x01)<< 3) |
				((DeviceStateBusiness.Coinrouting & 0x01)<< 4) |
				((DeviceStateBusiness.CoinremoveTube & 0x01)<< 5) |
				((DeviceStateBusiness.Coindisable & 0x01)<< 6) 
				;
			rst[index1+ 3] |= (err == 1) ? 0x80 : 0;

			//第二字节
			rst[index1+ 2] = (DeviceStateBusiness.CoinunknowError )
								;
								
			if(SystemPara.CoinChangerType == 1)
			{
				rst[index1+ 1] = ((DeviceStateBusiness.Hopper1State == 1) & 0x01) |
				(((DeviceStateBusiness.Hopper1State == 2) & 0x01)<< 1) |
				(((DeviceStateBusiness.Hopper2State == 1) & 0x01)<< 2) |
				(((DeviceStateBusiness.Hopper2State == 2) & 0x01)<< 3) |
				(((DeviceStateBusiness.Hopper3State == 1) & 0x01)<< 4) |
				(((DeviceStateBusiness.Hopper3State == 2) & 0x01)<< 5) |
				((0)<< 6) |
				(0 << 7) 
				;
			}
		}
		index1 += 4;//偏移
			

		//主柜室运行状态
		rst[index1++] = 0;//备用
		rst[index1++] = 0;

		//主柜室故障状态
		if(SystemPara.Channel == 2)
		{
			temp = ((DeviceStateBusiness.Emp_Gol & 0x01) << 0) |
				((DeviceStateBusiness.ColBoardError & 0x01) << 1) |
				((DeviceStateBusiness.GOCError & 0x01) << 2) |
				((stLiftTable.Error_LIFTER & 0x01) << 3) |
				((stLiftTable.Error_TAKEMOTTOR & 0x01) << 4) |
				((stLiftTable.Error_FANGDAO & 0x01) << 5) |
				(0 << 6)|
				(0 << 7)
				;
		}
		else
		{
			temp = 0;
			temp = ((DeviceStateBusiness.GOCError & 0x01) << 2) |
				((DeviceStateBusiness.ColBoardError & 0x01) << 1) |
				((DeviceStateBusiness.Emp_Gol & 0x01) << 0)
			;
		}
	
		
		rst[index1++] = temp1;//备用
		rst[index1++] = temp;
		PC_DELAY_500MS;
		pcReply(STATUS_REQ,index1,rst);
	}
	
	
	
}

/*********************************************************************************************************
** Function name:     	pcFaultReq
** Descriptions:	    
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static void pcDebugReq(unsigned char len,unsigned char *data)
{
	unsigned char type,sendBuf[10] = {0},index = 0,index1 = 0; 
	type = data[index++];
	sendBuf[index1++] = type;

	if(type == 0)//获取
	{
		sendBuf[index1++] = mainTrace;
		sendBuf[index1++] = UserPara.billTrace;
		sendBuf[index1++] = UserPara.coinTrace;
		sendBuf[index1++] = UserPara.changelTrace;
		sendBuf[index1++] = UserPara.readerTrace;
		sendBuf[index1++] = UserPara.channelTrace;
		sendBuf[index1++] = UserPara.compressorTrace;
		sendBuf[index1++] = UserPara.ledTrace;
		sendBuf[index1++] = UserPara.selectTrace;
		sendBuf[index1++] = UserPara.PC;
		
	}
	else if(type == 1)//设置总Trace
	{		
		
		mainTrace = data[index++];
		sendBuf[index1++] = mainTrace;
		sendBuf[index1++] = UserPara.billTrace;
		sendBuf[index1++] = UserPara.coinTrace;
		sendBuf[index1++] = UserPara.changelTrace;
		sendBuf[index1++] = UserPara.readerTrace;
		sendBuf[index1++] = UserPara.channelTrace;
		sendBuf[index1++] = UserPara.compressorTrace;
		sendBuf[index1++] = UserPara.ledTrace;
		sendBuf[index1++] = UserPara.selectTrace;
		sendBuf[index1++] = UserPara.PC;
		
	}
	else if(type == 2)//设置纸币器
	{
		UserPara.billTrace = data[index++];
		sendBuf[index1++] = UserPara.billTrace;
		
	}
	else if(type == 3)
	{
		UserPara.coinTrace = data[index++];
		sendBuf[index1++] = UserPara.coinTrace;
	}
	else if(type == 4)
	{
		UserPara.changelTrace = data[index++];
		sendBuf[index1++] = UserPara.changelTrace;
		
	}
	else if(type == 5)
	{
		UserPara.readerTrace = data[index++];
		sendBuf[index1++] = UserPara.readerTrace;
	}
	else if(type == 6)
	{
		UserPara.channelTrace = data[index++];
		sendBuf[index1++] = UserPara.channelTrace;
	}
	else if(type == 7)
	{
		UserPara.compressorTrace = data[index++];
		sendBuf[index1++] = UserPara.compressorTrace;
	}
	else if(type == 8)
	{
		UserPara.ledTrace = data[index++];
		sendBuf[index1++] = UserPara.ledTrace;
	}
	else if(type == 9)
	{
		UserPara.selectTrace = data[index++];
		sendBuf[index1++] = UserPara.selectTrace;
	}
	else if(type == 10)
	{
		UserPara.PC = data[index++];
		sendBuf[index1++] = UserPara.PC;
	}
	else if(type == 0xF0)
	{
		UserPara.TraceFlag = mainTrace;
		WriteUserSystemPara(UserPara,0);
		UserPara.TraceFlag = 0;
		sendBuf[index1++] = 0x01;
	}
	else
	{
		print_warning(type);
		return;
	}
	
	PC_DELAY_500MS;	
	pcReply(DEBUG_RPT,index1,sendBuf) ;
	
}


/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
static unsigned char vmcPoll(unsigned char busy)
{

	unsigned char rlen,type;
	if(pcControlSend(VMC_PCM_POLL,1,&busy) != msg)
		return 1;
	if(busy)
	{
		print_warning(busy);
		return 1;
	}		
	type = pcUartMsg->recv_msg[3];//MT
	rlen = pcUartMsg->recv_len - head;
	print_log("cmd[%d]-len = %d\r\n",type,rlen);
	switch(type)
	{
		case TRADE_REQ:
			pcTradeReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case CLEAR_REQ:
			pcClearReq(rlen,pcUartMsg->recv_msg[head]);
			break;
		case SYSPARA_REQ:
			pcSysParaReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case HUODAO_REQ:
			pcHuodaoReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case FACTORRECOVER_REQ:
			pcFactorReq();
			break;
		case DATETIME_REQ:
			pcDateTimeReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case STATUS_REQ:
			pcStatusReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case DEBUG_REQ:
			pcDebugReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case FACTOR_SET_REQ:
			pcFactorSet();
			break;
		case ID_WRITE_REQ:
			pcIdSetReq(rlen,&pcUartMsg->recv_msg[head]);
			break;
		case ID_READ_REQ:
			pcIdReadReq();
			break;
		default:
			break;

	}
	return 1;
}



void SystemDatasBroadcast(void *pd)
{
	
	OS_STK_DATA task;
	
	pd = pd;
	while(1)
	{
		if(OSTaskStkChk(6,&task) == OS_NO_ERR) //uart2task
			print_log("Uart2Task used/free:%4d/%4d  usage:%%%d\r\n",task.OSUsed,task.OSFree,(task.OSUsed * 100)/(task.OSFree + task.OSUsed));
		OSTimeDly(100);
		
		if(OSTaskStkChk(12,&task) == OS_NO_ERR) //PCMangementtask
			print_log("PCManTask used/free:%4d/%4d  usage:%%%d\r\n",task.OSUsed,task.OSFree,(task.OSUsed * 100)/(task.OSFree + task.OSUsed));
		OSTimeDly(100);
		if(OSTaskStkChk(9,&task) == OS_NO_ERR) //uart1Task
			print_log("Uart1Task used/free:%4d/%4d  usage:%%%d\r\n",task.OSUsed,task.OSFree,(task.OSUsed * 100)/(task.OSFree + task.OSUsed));
		OSTimeDly(100);
		if(OSTaskStkChk(8,&task) == OS_NO_ERR) //13
			print_log("Uart3Task used/free:%4d/%4d  usage:%%%d\r\n",task.OSUsed,task.OSFree,(task.OSUsed * 100)/(task.OSFree + task.OSUsed));
		OSTimeDly(100);
		if(OSTaskStkChk(7,&task) == OS_NO_ERR)//13
			print_log("Main-Task used/free:%4d/%4d  usage:%%%d\r\n\r\n",task.OSUsed,task.OSFree,(task.OSUsed * 100)/(task.OSFree + task.OSUsed));
		
		OSTimeDly(3000/5);
	}
	

 
}

/*********************************************************************************************************
** Function name:     	sysBoardCastCheckTask
** Descriptions:	    监控任务
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void sysBoardCastCheckTask(void)
{
   #if 1
	//changed by yoc 2014.03.13
	OSTaskCreateExt(SystemDatasBroadcast,
					(void *)0,
					&SystemDatasBroadcastStk[STK_CHECK_TASK_SIZE - 1],
					STK_CHECK_TASK_PRIO,
					STK_CHECK_TASK_PRIO,
					&SystemDatasBroadcastStk[0],
					STK_CHECK_TASK_SIZE,
					(void *)0,
					OS_TASK_OPT_STK_CHK);
	//OSTaskCreate(SystemDatasBroadcast,(void *)0,&SystemDatasBroadcastStk[STK_CHECK_TASK_SIZE- 1],STK_CHECK_TASK_PRIO);
	OSTimeDly(10);
	#endif
}





/*********************************************************************************************************
** Function name:     	pcControlInit
** Descriptions:	    串口0任务初始化
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char pcControlInit()
{
	vmc_pcm_event = OSQCreate(&PCMSizeQ[0],PCM_MAX_SIZE);
	pcm_vmc_event = OSMboxCreate(NULL);

	//串口创建消息队列
	OSTaskCreateExt(enterPCControl,
					(void *)0,
					&pcControlTaskStk[PC_CONTROL_TASK_STK_SIZE - 1],
					PC_CONTROL_TASK_PRIO,
					PC_CONTROL_TASK_PRIO,
					&pcControlTaskStk[0],
					PC_CONTROL_TASK_STK_SIZE,
					(void *)0,
					OS_TASK_OPT_STK_CHK);
	print_log("Create taskPCManagement...\r\n");
	OSTimeDly(4);
	return 1;
	
}






/*********************************************************************************************************
** Function name:     	enterPCControl
** Descriptions:	    	PC控制任务
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void enterPCControl(void *pd)
{
	unsigned char errMBox;
	PCCONMSG *recvMsg;	
	pd = pd;
	
	pcUartMsg = &pcm_msg;//为了节省点内存又不影响可读性和移植性 		
	comErr = 12;
	mainTrace = UserPara.TraceFlag;	

	pcm_uartClear = ClrUart0Buff;
	pcm_uartGetCh = Uart0GetCh;
	pcm_uartNoEmpty = Uart0BuffIsNotEmpty;
	pcm_uartPutStr = Uart0PutStr;
	
	while(1)
	{
		if(comErr > 10)
		{
			UserPara.TraceFlag = mainTrace;//idsconnected PC Enable the uart0
			while(1)
			{	
				if(pcSeek())
				{
					comErr = 0;
					busyFlag = 0;
					mainTrace = UserPara.TraceFlag;
					if(cur_port == 0)
				    	UserPara.TraceFlag = 0;//connected PC disable the uart0						
					break;
				}
				print_log("pcm %d\r\n",++comErr);
				PC_DELAY_1S;
				//延时1秒
				
			}			
		}	
		recvMsg = OSQPend(vmc_pcm_event,500/5,&errMBox);
		if(errMBox == OS_NO_ERR)
		{
			switch(recvMsg->type)
			{
				//回应结果
				case MBOX_HUODAO_INIT:
					pcInitHuoDaoRpt(0x02,1);
					break;
				case MBOX_HUODAO_TEST:
					pcTestAisleRpt(recvMsg);
					break;
				case MBOX_HUODAO_OUTGOODS:
					pcHuodaoOutGoodsRpt(recvMsg);
					break;
				default:
					
					break;
			}
			
			PC_DELAY_POLL;
			busyFlag = 0;
		}

		vmcPoll(busyFlag);
		if(!Timer.pcm_handle_timeout)
			busyFlag = 0;

	}

	
}


/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/


void vmcPost(unsigned char type)
{
	vmc_pc_mail[pcm_index].type = type;
	OSQPost(vmc_pcm_event,&vmc_pc_mail[pcm_index]);
	pcm_index = (++pcm_index) % PCM_MAX_SIZE;
	OSTimeDly(4);
}

/*********************************************************************************************************
** Function name:     	vmcPoll
** Descriptions:	    轮询PC
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void pcPost(unsigned char type)
{
	pc_vmc_mail.type = type;
	OSMboxPost(pcm_vmc_event,&pc_vmc_mail);
	OSTimeDly(4);
}






/*********************************************************************************************************
** Function name:     	mainTaskPollPC
** Descriptions:	    主任务轮询PC长时间的指令
** input parameters:    state 1运行  2 故障  3维护
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void mainTaskPollPC(unsigned char state)
{

	INT8U err,rst;
	PCCONMSG *recvMsg;	
	recvMsg = OSMboxPend(pcm_vmc_event,2,&err);
	if(err == OS_NO_ERR)
	{
		switch(recvMsg->type)
		{
			case MBOX_HUODAO_INIT:
				ChannelResetinit();					
				break;
			case MBOX_HUODAO_TEST:
				print_log("recvMsg:%d %d %d",recvMsg->msg[0],recvMsg->msg[1],recvMsg->msg[2]);
				rst = ChannelAPIProcess(recvMsg->msg[2],CHANNEL_OUTGOODS,recvMsg->msg[1]);
				vmc_pc_mail[pcm_index].msg[0] = recvMsg->msg[0];
				vmc_pc_mail[pcm_index].msg[1] = recvMsg->msg[1];
				vmc_pc_mail[pcm_index].msg[2] = recvMsg->msg[2];
				vmc_pc_mail[pcm_index].msg[3] = (rst == 1) ? 0 : ((rst == 0 ) ? 4 :rst);
				break;
			case MBOX_HUODAO_OUTGOODS:
				if(state == 0)
					rst = ChannelAPIProcess(recvMsg->msg[2],CHANNEL_OUTGOODS,recvMsg->msg[1]);
				else
					rst = 0;
				vmc_pc_mail[pcm_index].msg[0] = recvMsg->msg[0];
				vmc_pc_mail[pcm_index].msg[1] = recvMsg->msg[1];
				vmc_pc_mail[pcm_index].msg[2] = recvMsg->msg[2];
				vmc_pc_mail[pcm_index].msg[3] = rst;
				
				break;
			default:
				
				break;
		}
		
		vmcPost(recvMsg->type);

		
	}


}


/**************************************************************************************************
*
*新协议目前止步于货道请求部分
************************************************************************************************/

