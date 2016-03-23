/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           LiftTableDriver.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        升降台的控制驱动                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-05
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"
extern volatile unsigned int LIFTTABLETIMER;
STLIFTTABLE stLiftTable;

unsigned char GetAckFromDevice[128]={0};
extern volatile unsigned int  EVBCONVERSATIONWAITACKTIMEOUT;	//EVB通讯接收ACK超时时间
extern volatile unsigned int  UART3RECVACKMSGTIMEOUT;			//接收ACK数据包超时时间




static unsigned char sendbuf[64] = {0};
//static unsigned char sendlen = 0;
static unsigned char recvbuf[64] = {0};
static unsigned char index = 0;
static unsigned char recvlen = 0;


//const static unsigned char I_SF = 0;
const static unsigned char I_LEN = 1;
//const static unsigned char I_ADDR = 2;
//const static unsigned char I_VER = 3;
const static unsigned char I_MT = 4;
const static unsigned char I_DATA = 5;


typedef struct{
	unsigned char addr;
	unsigned char cmd;
	unsigned char bin;
	unsigned char row;
	unsigned char column;
	unsigned char *recvbuf;
	unsigned char *sendbuf;
	unsigned char recvlen;
	unsigned char sendlen;
	unsigned char data[8];
	unsigned char datalen;

}ST_LIFT_MSG;

ST_LIFT_MSG liftMsg;




static void LIFT_clear(void)
{
	index = 0;recvlen = 0;
	memset(recvbuf,0,sizeof(recvbuf));
}


static unsigned char LIFT_recv(void){
	unsigned char ch;
	unsigned short crc;
	static unsigned char len = 0;
	
	if(Uart2BuffIsNotEmpty() != 1){
		OSTimeDly(5);
		return 0;
	}
	ch = Uart2GetCh();
	//TraceChannel("LIFT[%d]:%02x \r\n",index,ch);
	if(index == 0){
		if(ch == 0xC8){
			recvbuf[index++] = ch;
		}
		else{
			index = 0;
		}
	}
	else if(index == 1){
		len = ch;
		recvbuf[index++] = ch;
	}
	else{
		if(index >= (len + 1)){
			recvbuf[index++] = ch;
			crc  = CrcCheck(recvbuf,len); 
			recvlen = index;
			if(crc == INTEG16(recvbuf[len], recvbuf[len + 1])){
				return 1;
			}
			else{
				return 2;
			}
		}
		else{
			recvbuf[index++] = ch;
		}
	}
	recvlen = index;
	return 0;
}


unsigned char LIFT_send(ST_LIFT_MSG *msg)
{
	unsigned char i,res,in = 0;
	unsigned short crc = 0x0000;
	if(msg->bin == 1){
		SetUart2Evb2Mode();
	}
	else if(msg->bin == 2){
		SetUart2Evb1Mode();	
	}
	else{
		return 0;
	}
	SetUart2ParityMode(PARITY_DIS);
	in = 0;
	sendbuf[in++] = 0xC7;	
	sendbuf[in++] = 0x00;
	sendbuf[in++] = msg->addr;
	sendbuf[in++] = 0x00;
	sendbuf[in++] = msg->cmd;
	
	for(i = 0;i < msg->datalen;i++){
		sendbuf[in++] = msg->data[i];
	}
	sendbuf[I_LEN] = in;
	crc	= CrcCheck(sendbuf,in);
	sendbuf[in++] = HUINT16(crc);
	sendbuf[in++] = LUINT16(crc);

	ClrUart2Buff();
	Uart2PutStr(sendbuf,in);
	
	TraceChannel("Lift[send] ");
	for(i = 0;i < in;i++){TraceChannel("%02x ",sendbuf[i]);}
	TraceChannel("--------\r\n");
	LIFT_clear();
	EVBCONVERSATIONWAITACKTIMEOUT = 200;
	while(EVBCONVERSATIONWAITACKTIMEOUT){	// 1.5s内收到ACK,否则超时
		res = LIFT_recv();
		if(res == 1 || res == 2){
			TraceChannel("Lift[recv] ");
			for(i=0;i < recvlen;i++){TraceChannel("%02x ",recvbuf[i]);}	
			TraceChannel("--------\r\n");
			SetUart2MdbMode();
			msg->recvbuf = recvbuf;
			msg->recvlen = recvlen;
			return res;
		}
	}
	TraceChannel("Lift[timeout] ");
	for(i=0;i < recvlen;i++){TraceChannel("%02x ",recvbuf[i]);}	
	TraceChannel("XXXXXXXXXXXXXXXXXXXXX\r\n");
	SetUart2MdbMode();
	return 0;
}




/*********************************************************************************************************
** Function name:     	Uart3RecvEvbAckFromDevice
** Descriptions:	    EVB通讯，接收ACK包
** input parameters:    无
** output parameters:   无
** Returned value:      1：接收成功；0：接收失败
*********************************************************************************************************/
unsigned char RecvEvbAckFromDevice(void)
{
	unsigned char RecvData,AckIndex,temp;
	unsigned short Crcdata = 0x0000;

	if(Uart2BuffIsNotEmpty() == 1)
	{
		RecvData = Uart2GetCh();
		TraceChannel("EVB: %02x\r\n",RecvData);
		
		if((RecvData == 0xC8))
		{
			AckIndex = 0;
			GetAckFromDevice[AckIndex++] = RecvData;	
			UART3RECVACKMSGTIMEOUT = 500; 				//接收剩余字节超时时间，100ms
			while(UART3RECVACKMSGTIMEOUT)
			{
				if(Uart2BuffIsNotEmpty()==1)
				{
					temp = Uart2GetCh();
					TraceChannel("EVB: %02x\r\n",temp);
					GetAckFromDevice[AckIndex++] = temp;
					if(AckIndex == GetAckFromDevice[1]+2)	//处理6/8字节的ACK
					{
						Crcdata = CrcCheck(GetAckFromDevice,GetAckFromDevice[1]);
						if((GetAckFromDevice[AckIndex-2] == (unsigned char)(Crcdata>>8))&&((GetAckFromDevice[AckIndex-1] == (unsigned char)(Crcdata&0x00ff))))
						{
							return 1;
						}
						else
							break;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}


//出货指令
unsigned char LIFT_vmcVendingReq(unsigned char bin,unsigned char row,unsigned char column)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_VENDING_REQ;
	msg->data[0] = row;
	msg->data[1] = column;
	msg->datalen = 2;
	res = LIFT_send(msg);
	if(res == 1){
		return  msg->recvbuf[I_MT] == GCC_VENDING_ACK ?  1 : 2;
	}
	else{ 
		return res;
	}
}

//0x1F 通信故障 0x10 正常 0x11 整机忙 0x12故障
unsigned char LIFT_vmcStatusReq(unsigned char bin)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_STATUS_REQ;
	msg->datalen = 0;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_STATUS_ACK){
		stLiftTable.Error_OVERALLUINT = msg->recvbuf[5];
		if(msg->recvbuf[5] == 0x01){
			return 0x11; //忙
		}

		if(msg->recvbuf[6] == 0x00){
			return 0x10; //正常
		}
		else{
			return 0x12; //故障
		}
	}
	else{
		return 0x1F; //通信故障
	}
}


unsigned char LIFT_vmcReset(unsigned char bin)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_RESET_REQ;
	msg->datalen = 0;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_RESET_ACK){
		return 1;
	}
	else{
		return 0;
	}
}

//返回0:失败，1：成功，2：数据错误 3：无货 4：卡货 5：取货门未开启 6：货物未取走 7：未定义错误	0xff：通信失败
// 0 未接受 1成功 0x88正在出货
unsigned char LIFT_vmcVedingResult(unsigned char bin)
{
	unsigned char res,temp;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_VENDINGRESULT_REQ;
	msg->datalen = 0;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_VENDINGRESULT_ACK){
		if(msg->recvbuf[I_DATA] != 0x00){
			return 0x88;
		}
		if(msg->recvbuf[I_DATA + 1] == 0x00){
			return 1;
		}

		temp = msg->recvbuf[I_DATA + 2];
		if(temp == 0x00){
			return 1;
		}
		else if(temp == 0x01){
			return 2;
		}
		else if(temp == 0x02){
			return 3;
		}
		else if(temp == 0x03){
			return 4;
		}
		else if(temp == 0x04){
			return 5;
		}
		else if(temp == 0x05){
			return 6;
		}
		else if(temp == 0x06){
			return 7;
		}
		else if(temp == 0x07){
			return 8;
		}
		else{
			return 9;
		}
		//return 9;
	}
	else{
		return 0;
	}
}


unsigned char LIFT_vmcVendingResultByTime(unsigned char bin,unsigned int ms)
{
	unsigned char res;
	UART3RECVACKMSGTIMEOUT = ms / 5;
	while(UART3RECVACKMSGTIMEOUT){
		res = LIFT_vmcVedingResult(bin);
		if(res > 0){
			return res;
		}
		else{
			msleep(1000);
		}
	}
	return 0;
}


unsigned char LIFT_vmcOpenLight(unsigned char bin)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_OPENLIGHT_REQ;
	msg->datalen = 0;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_OPENLIGHT_ACK){
		return 1;
	}
	else{
		return 0;
	}


}

unsigned char LIFT_vmcCloseLight(unsigned char bin)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_CLOSELIGHT_REQ;
	msg->datalen = 0;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_CLOSELIGHT_ACK){
		return 1;
	}
	else{
		return 0;
	}
}


unsigned char LIFT_vmcChuchou(unsigned char bin,unsigned char flag)
{
	unsigned char res;
	ST_LIFT_MSG *msg = &liftMsg;
	msg->addr = 0x40;
	msg->bin = bin;
	msg->cmd = VMC_CHUCHOU_REQ;
	msg->data[0] = flag;
	msg->datalen = 1;
	
	res =  LIFT_send(msg);
	if(res == 1 && msg->recvbuf[I_MT] == GCC_CHUCHOU_ACK){
		return 1;
	}
	else{
		return 0;
	}
}


//返回0:失败，1：成功，2：数据错误 3：无货 4：卡货 5：取货门未开启 6：货物未取走 7：未定义错误	0xff：通信失败
unsigned char LIFT_vendout(unsigned char bin,unsigned char row,unsigned char column)
{
	unsigned char res,flow,err;
	LIFTTABLETIMER = 3000;flow = 0;
	while(LIFTTABLETIMER){
		res = LIFT_vmcStatusReq(bin);
		if(res == 0x10){
			flow = 1;
			break;
		}
		else{
			if(res != 0x1F){
				msleep(1000);
			}
			
		}
	}
	if(flow == 0){ //有问题
		return res;
	}
	
	//进行出货操作
	LIFTTABLETIMER = 6000;
	while(LIFTTABLETIMER){
		res = LIFT_vmcVendingReq(bin,row,column);
		if(res == 1){
			msleep(500);
			res = LIFT_vmcVendingResultByTime(bin,5000);
			if(res == 6){ //货物没取走
				msleep(2000);
				err = 6;
				continue;
			}
			else{
				flow = 1;break;
			}
		}
		else{
			res = LIFT_vmcVedingResult(bin);
			if(res == 0x88){
				flow = 1;break;
			}
			msleep(1000);
		}
	}
	
	if(flow == 0){return err == 6 ? 6 : 0;}
	
	LIFTTABLETIMER = 3000 + row * 200;
	while(LIFTTABLETIMER){
		res = LIFT_vmcVedingResult(bin);//检测出货结果
		if(res > 0 && res != 0x88){ //出货结果
			return res;
		}
		else{
			if(res > 0){
				msleep(1000);
			}
			
		}
	}
	return 0;
}




unsigned char LiftTableDriver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char Level,unsigned char *Result)
{
	unsigned char res = 0;
	switch(Cmd){
		case CHANNEL_OUTGOODS:
			res = LIFT_vendout(Binnum,Level,Add);
			break;
		case CHANNEL_CHECKSTATE:
			res = LIFT_vmcStatusReq(Binnum);
			break;
		case CHANNEL_CHECKOUTRESULT:
			res = LIFT_vmcVedingResult(Binnum);
			break;
		case MACHINE_RESET:
			res = LIFT_vmcReset(Binnum);
			break;
		case MACHINE_LIGHTOPEN:
			res = LIFT_vmcOpenLight(Binnum);
			break;
		case MACHINE_LIGHTCLOSE:
			res = LIFT_vmcCloseLight(Binnum);
			break;
		case MACHINE_OPENDOOR:
			res = 0;
			break;
		case MACHINE_CLOSEDOOR:
			res = 0;
			break;
		case MACHINE_INFORMATION:
			res = 0;
			break;
		case VMC_CHUCHOU_REQ:
			res = LIFT_vmcChuchou(Binnum,Level);
			break;
		default:break;	
	}
	return res;
	
}



