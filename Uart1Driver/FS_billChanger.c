#include "..\config.h"
#include "FS_billChanger.h"

//#define PRINT_FS_ENABLE  1
//#if PRINT_FS_ENABLE == 1
//#define  print_fs(...)       Trace(__VA_ARGS__)
//#else
//#define  print_fs(...)       do {} while(0)
//#endif

#define FS_TYPE_REQUEST			1
#define FS_TYPE_ACK				2
#define FS_TYPE_NAK				3

// 1 5 10 20 50 100  元
#define FS_BILL_1	0
#define FS_BILL_5	1
#define FS_BILL_10	2
#define FS_BILL_20	3
#define FS_BILL_50	4
#define FS_BILL_100	5





//{Length-H Length-L Hex}
const unsigned char fsBillInfo[][3] = {
		{0x8C,0x78,0x0C},
		{0x91,0x7D,0x0C},
		{0x96,0x82,0x0C},
		{0x9B,0x87,0x0C},
		{0xA0,0x8C,0x0C},
		{0xA5,0x91,0x0C}
}; 

//偶校验码表
const unsigned char fs_code_hex[] = {
	0x30,0xB1,0xB2,0x33,0xB4,0x35,0x36,0xB7,0xB8,0x39
};

const unsigned char STX = 0x02;
const unsigned char ETX = 0x03;
const unsigned char ENQ = 0x05;
const unsigned char ACK = 0x06;
const unsigned char NAK = 0x15;
const unsigned char DLE = 0x10;
const unsigned char FS = 0x1C;


#define FS_STATUS_IDLE			0		//空闲状态 等待接收上位机命令
#define FS_STATUS_SEND_READY	1	
#define FS_STATUS_SEND			2	
#define FS_STATUS_SEND_REQ		3
#define FS_STATUS_RECV_DATA		4		//接收数据状态
#define FS_STATUS_RECV_ENQ		7		//接收ENQ
#define FS_STATUS_ERR			8
#define FS_STATUS_TIMEOUT		0xEE

static volatile unsigned char flowStatus = FS_STATUS_IDLE; // 0空闲  1 准备握手  2 准备发送数据 3准备接收结果
static unsigned char retry = 0;

#if 0
const unsigned char fs_testdata[] = {0x10,0x02,0x00,0x08,0x60,0x01,0xFF,0x00,0x00,0x01,0x00,0x1C,0x10,0x03,0xC8,0x72};
#endif

#define FS_REQ_IDLE		0
#define FS_REQ_STATUS	1
#define FS_REQ_RESET	2
#define FS_REQ_DISPENSE	3



static volatile unsigned char request;
static volatile unsigned char userRequest = 0;

typedef struct{
	uint8 buf[512];
	uint16 len;
	uint8 *data;
	uint16 dataLen;
}FS_MSG;


#define FS_BOX_SUM			4  //最大4个仓
#define FS_DISPENSE_MAX		9 //最大拒绝纸币数
#define FS_DISPENSE_RETRY	1 //找零重试次数



#define FS_STATUS_QUEBI		(0x00000001IL << 0)   // 缺币


#define FS_BOX_BILL_LOW			(0x01 << 0) // 钞箱币不足
#define FS_BOX_REMOVE		(0x01 << 1) //钞箱被移走
#define FS_BOX_FALSE_PLACE	(0x01 << 2) //钞箱在错误位置
#define FS_BOX_ENABLE		(0x01 << 7)  //启用该通道


typedef struct{
	uint8 no;  //仓 号 1- 4
	uint8 status;
	uint8 empty; //纸币空标志
	uint8 w;  	//宽度
	uint8 l; 	// 长度
	uint8 h; 	//厚度 
	uint8 changed;
	uint8 num; 		//通道配币数
	uint8 dispense; //实际通道找币数
	uint32 ch; //仓号面值
	
}FS_BOX;  //FS钞箱 结构体

typedef struct{
	uint32 amount; //找零金额 分为单位
	uint32 remain;
	uint32 changed;
}FS_BILL_REQ;


#define FS_NORMAL		0x00000000UL
#define FS_RESET_ERR	(0x01UL << 30)  //初始化失败
#define FS_COM_ERR		(0x01UL << 31)  //通信故障


typedef struct{
	uint16 errCode; //故障码 0为正常
	uint16 errAddr; //故障地址
	uint16 fw;
	uint8 errRegister[3];
	uint8 sensorRegister[6];
	uint8 hexSensor; //厚度传感器状态值
	uint32 status;  // 0正常  非0 
	FS_BOX box[FS_BOX_SUM];
	FS_BOX *boxsort[FS_BOX_SUM]; //经过排序后的 仓 指针
	FS_BILL_REQ req;
}FS_BILL;




#pragma arm section zidata = "RAM2"
FS_MSG fsRecvMsg;
FS_MSG fsSendMsg;
FS_BILL fsBill;
FS_STATUS fsStatus;
#pragma arm section zidata

volatile unsigned int fs_timer = 0;
volatile unsigned int fs_reqTimer = 0;
volatile unsigned int fs_taskTimer = 0;



/*********************************************************************************************************
** Function name:     	msleep
** Descriptions:	    毫秒睡眠函数  比较
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/

static void msleep(unsigned int msec)
{
	unsigned int temp;
	temp = (msec > 10) ? msec/5 : 2;
	OSTimeDly(temp);
}


static uint8 FS_getFlow(void)
{
	return flowStatus;
}

static void FS_setFlow(uint8 flow)
{
	flowStatus = (flowStatus == FS_STATUS_TIMEOUT) ? FS_STATUS_IDLE : flow;
}




static uint8 FS_getValueByCodeHex(uint8 hex)
{
	uint8 i;
	for(i = 0;i < sizeof(fs_code_hex);i++){
		if(fs_code_hex[i] == hex){
			return i;
		}
	}
	return 0xFF;
}

static uint8 FS_getValueByCodeHex2(uint8 hexh,uint8 hexl)
{
	uint8 temp1,temp2;
	temp1 = FS_getValueByCodeHex(hexh);
	temp2 = FS_getValueByCodeHex(hexl);

	if(temp1 != 0xFF && temp2 != 0xFF){
		return temp1 * 10 + temp2;
	}
	else{
		return 0xFF;
	}
}


static uint32 FS_getDataTimeout(void)
{
	uint8 i;
	uint32 timeout;
	if(request == FS_REQ_STATUS){
		timeout = 3000;
	}
	else if(request == FS_REQ_RESET){
		timeout = 10000;
	}
	else if(request == FS_REQ_DISPENSE){
		timeout = 8000;
		for(i = 0;i < FS_BOX_SUM;i++){
			timeout += fsBill.box[i].num * 3000;
		}
	}
	else{
		timeout = 3000;
	}
	print_fs("\r\n===========================================\r\n");
	print_fs("\r\n=============Data set timeout = %d=====\r\n",timeout);
	print_fs("\r\n===========================================\r\n");
	return timeout;
}

static void FS_setTimer(unsigned int ms)
{
	fs_timer = ms / 10;
}

static unsigned char FS_isTimeout(void)
{
	return (fs_timer == 0) ? 1 : 0;
}

static void FS_setTaskTimer(unsigned int ms)
{
	fs_taskTimer = ms / 10;
}

static unsigned char FS_isTaskTimeout(void)
{
	return (fs_taskTimer == 0) ? 1 : 0;
}



static void FS_setReqTimer(unsigned int ms)
{
	fs_reqTimer = ms / 10;
	if(ms == 0){ //重置
		retry = 0;
	}
}

// 1 超时 0不超时  2 超时上限
static unsigned char FS_getReqTimer(void)
{
	if(fs_reqTimer == 0){
		if(retry++ > 3){
			retry = 0;
			return 2;
		}
		else{
			return 1;
		}
	}
	else{
		return 0;
	}
	
}



static uint8 *FS_getBillInfoIndex(uint32 value)
{
	uint8 *data = NULL;
	switch(value){
		case 100:
			data = (uint8 *)fsBillInfo[FS_BILL_1];
			break;
		case 500:
			data = (uint8 *)fsBillInfo[FS_BILL_5];
			break;
		case 1000:
			data = (uint8 *)fsBillInfo[FS_BILL_10];
			break;
		case 2000:
			data = (uint8 *)fsBillInfo[FS_BILL_20];
			break;
		case 5000:
			data = (uint8 *)fsBillInfo[FS_BILL_50];
			break;
		case 10000:
			data = (uint8 *)fsBillInfo[FS_BILL_100];
			break;
		default:break;
	}

	return data;
}


static uint8 FS_billChannelSort(void)
{
	uint8 i,j;
	uint32 value = 0;
	FS_BOX *box = NULL,*p;
	for(j = 0;j < FS_BOX_SUM;j++){
		value = 0;
		for(i = 0;i < FS_BOX_SUM;i++){
			p = &fsBill.box[i];
			if(!(p->status & FS_BOX_ENABLE) && value < p->ch){
				value = p->ch;box = p;
			}
		}
		if(value > 0){
			box->status |= FS_BOX_ENABLE;
			fsBill.boxsort[j] = box;
		}
		else{
			fsBill.boxsort[j] = NULL;
		}
		
	}

	print_fs("\r\n-------------FS_billChannelSort--------\r\n");
	for(i = 0;i < FS_BOX_SUM;i++){
		if(fsBill.boxsort[i]){
			print_fs("[%d] = %d\r\n",fsBill.boxsort[i]->no,fsBill.boxsort[i]->ch);
		}
		print_fs("fsBill.box[%d].status=%x\r\n",i,fsBill.box[i].status);
	}
	
	print_fs("\r\n----------------------------------------\r\n");

	return 1;	
}


static void FS_msgInit(void)
{
	memset((void *)&fsRecvMsg,0,sizeof(fsRecvMsg));
	memset((void *)&fsSendMsg,0,sizeof(fsSendMsg));
	fsRecvMsg.data = &fsRecvMsg.buf[4];
	fsSendMsg.data = &fsSendMsg.buf[4];
}

static void FS_devInit(void)
{
	uint8 i;
	FS_BOX *box;
	memset((void *)&fsBill,0,sizeof(fsBill));

	fsBill.status = FS_COM_ERR;
	for(i = 0;i < FS_BOX_SUM;i++){
		box = &fsBill.box[i];
		box->no = i + 1;
		box->status = 0;
		box->empty = 0;
	}


	for(i = 0;i < FS_BOX_SUM;i++){
		fsBill.box[i].ch = 0;//SystemPara.RecyclerValue[i];
		print_fs("\r\n-fsBill.box[i].ch=%d---------\r\n",i,fsBill.box[i].ch);
	}


	fsBill.box[0].ch = SystemPara.RecyclerMoney;
	
      print_fs("\r\n-fsBill.box[0].ch=%d---------\r\n",fsBill.box[0].ch);
	  
	print_fs("\r\n--------------FS_devInit------------------------\r\n");
	FS_billChannelSort();
}

void FS_init(void)
{
	SetUart1ParityMode(PARITY_EVEN);
	FS_msgInit();
	FS_devInit();
}


static uint16 FS_crc(uint8 *buf,uint16 len)
{
	uint8 data;
	uint16 i,dat;
    uint16 crc = 0;
    while(len--){
        data = *buf++;
        for(i = 0;i < 8;i++){
            dat = (uint16)data;
            dat ^= crc;
            crc >>= 1;
            if(dat & 0x0001){
                crc ^= 0x8408;
            }
            data >>= 1;
        }
    }
    return crc;
}

static void FS_clearUart(void)
{
	Uart1_ClrBuf();
}

static void FS_sendUart(unsigned char *buf,unsigned int len)
{
	unsigned int i;
	//OSIntEnter();
	OSSchedLock();
	Uart1PutStr((const unsigned char *)buf,len);
	OSSchedUnlock();
	//OSIntExit();
	print_fs("PC-->FS:");
	for(i = 0;i < len;i++){
		print_fs("%02x ",buf[i]);
	}
	print_fs("\r\nFS-->PC:");
	
}

static unsigned char FS_sendCmd(unsigned char type)
{
	unsigned char buf[2] = {0};
	switch(type){
		case FS_TYPE_REQUEST:
			buf[0] = DLE;
			buf[1] = ENQ;
			break;
		case FS_TYPE_ACK:
			buf[0] = DLE;
			buf[1] = ACK;
			break;
		case FS_TYPE_NAK:
			buf[0] = DLE;
			buf[1] = NAK;
			break;
		default:break;
	}
	FS_sendUart(buf,2);
	return 1;
}






static unsigned char FS_getChar(unsigned char *ch)
{
	if(Uart1BuffIsNotEmpty()){
		*ch = Uart1GetCh();
		print_fs("%02x ",*ch);
		return 1;
	}	
	else{
		return 0;
	}
}



static unsigned char FS_recvCmd(void)
{
	static unsigned char ch;
	static unsigned char in = 0;
	static unsigned char buf[2] = {0};
	if(FS_getChar(&ch) == 1){
		buf[in++] = ch;
		if(in >= 2){
			in = 0;
			print_fs("\r\n-----------cmd:%02x %02x-------------------\r\n\r\n",buf[0],buf[1]);
			if(buf[0] == DLE){
				return buf[1];
			}
		}
	}
	return 0;
}

#if 0
static unsigned char FS_recvByCmd(uint8 cmd,uint32 timeout)
{
	uint8 temp;
	FS_setTimer(timeout);
	while(!(FS_isTimeout())){
		temp = FS_recvCmd();
		if(temp == cmd){
			return 1;
		}
		else if(temp == NAK){
			return 2;
		}
		msleep(10);
	}

	return 0;
	
}
#endif



//driver级别，主窗口
unsigned char FS_poll(void)
{
	static unsigned char ch;
	static unsigned int in = 0;
	unsigned char *buf = (unsigned char *)fsRecvMsg.buf;
	static unsigned int textLen = 0;
	static unsigned short crc;

	switch(flowStatus){
		case FS_STATUS_IDLE:
			if(FS_getChar(&ch) == 1){
				print_fs("ch = %x ******************************\r\n",ch);
			}
			break;
		case FS_STATUS_SEND_REQ:
			if(request != FS_REQ_IDLE){ //上位机有请求命令
				print_fs("FS-----request == %d\r\n\r\n",request);
				FS_clearUart();
				memset(fsRecvMsg.buf,0,sizeof(fsRecvMsg.buf));
				FS_sendCmd(FS_TYPE_REQUEST);
				FS_setFlow(FS_STATUS_SEND_READY);
				//FS_setReqTimer(2000);
			}
			break;
		case FS_STATUS_SEND_READY:
			if(FS_recvCmd() == ACK){ 
				FS_sendUart(fsSendMsg.buf,fsSendMsg.len); //发送命令 还没实现
				FS_setFlow(FS_STATUS_SEND);
			}
			#if 0
			else{
				ch == FS_getReqTimer();
				if(ch == 1){
					FS_setFlow(FS_STATUS_SEND_REQ);
					msleep(1000);
				}
				else if(ch == 2){ //超时太多
					FS_setFlow(FS_STATUS_ERR);
				}
			}
			#endif
			break;
		case FS_STATUS_SEND:
			if(FS_recvCmd() == ACK){ 
				FS_setFlow(FS_STATUS_RECV_ENQ);
			}
			break;
		case FS_STATUS_RECV_ENQ:
			if(FS_recvCmd() == ENQ){ 
				FS_sendCmd(FS_TYPE_ACK);
				FS_setFlow(FS_STATUS_RECV_DATA);
				in = 0;
			}
			break;
		case FS_STATUS_RECV_DATA:
			if(FS_getChar(&ch) == 1){
				buf[in++] = ch;
				if(in == 1){ //DLE
					textLen = 0;
				}
				else if(in == 2){ //STX
					if(buf[0] != DLE || buf[1] != STX){ //不是 STX重新接收
						in = 0;
					}
				}
				else if(in == 3){
					textLen = ch;
				}
				else if(in == 4){
					textLen = textLen * 256 + ch;
					//print_fs("LEN=%x .....[%x %x %x %x]\r\n",textLen,buf[0],buf[1],buf[2],buf[3]);
				}
				else {
					if(in >= (textLen + 8)){ //接收数据完成
						crc = FS_crc(&buf[2],in - 4);
						print_fs("--------Crc[%x %x] vs FS_crc[%x %x]------\r\n",buf[in - 2],buf[in - 1],HUINT16(crc),LUINT16(crc));
						if(buf[in - 2] == LUINT16(crc) && buf[in - 1] == HUINT16(crc)){
							//如果校验成功 则需要回应ACK 一旦回应ACK FS56不再重复发送
							FS_sendCmd(FS_TYPE_ACK);
							FS_setFlow(FS_STATUS_IDLE);
							print_fs("\r\n=====================Over====================================\r\n\r\n\r\n");
						}
						in = 0;
					}
				}
			}
			break;
		case FS_STATUS_ERR:break;
		default:FS_setFlow(FS_STATUS_IDLE);break;
	}

	return 1;
}





uint8 FS_package(FS_MSG *msg)
{
	uint8 *buf = msg->buf;
	uint16 len = 0,crc;
	len = msg->dataLen;
	buf[0] = DLE;
	buf[1] = STX;
	buf[2] = HUINT16(len);
	buf[3] = LUINT16(len);
	
	len += 4;
	buf[len++] = DLE;
	buf[len++] = ETX;
	crc = FS_crc(&buf[2],len - 2);
	buf[len++] = LUINT16(crc);
	buf[len++] = HUINT16(crc);
	msg->len = len;
	return 1;
}





static uint8 FS_sendRequest(FS_MSG *msg,uint8 type)
{
	FS_setTimer(10000);
	while(!FS_isTimeout()){
		if(FS_getFlow() == FS_STATUS_IDLE){
			FS_package(msg);
			request = type;
			FS_setReqTimer(0);
			FS_setFlow(FS_STATUS_SEND_REQ);
			return 1;
		}
		else{
			msleep(20);
		}
	}
	FS_setFlow(FS_STATUS_TIMEOUT);
	return 0;
}




static FS_MSG *FS_waitResult(void)
{
	uint8 flow;
	FS_setTimer(FS_getDataTimeout());
	while(!FS_isTimeout()){
		flow = FS_getFlow();
		if(flow == FS_STATUS_IDLE){ //处理完成
			return &fsRecvMsg;
		}
		else if(flow == FS_STATUS_ERR){
			FS_setFlow(FS_STATUS_TIMEOUT);
			return NULL;
		}
		msleep(100);
	}
	FS_setFlow(FS_STATUS_TIMEOUT);
	return NULL; //处理超时
}

static FS_MSG *FS_userSendRequest(FS_MSG *msg,uint8 type)
{
	FS_MSG *rpt = NULL;
	#if 0
	if(fsBill.status != FS_NORMAL){
		print_fs("\r\n----------------------------------------\r\n");
		print_fs("\r\nFS_userSendRequest status=%x != FS_NORMAL\r\n",fsBill.status);
		print_fs("\r\n----------------------------------------\r\n");
		return NULL;
	}
	#endif
	
	userRequest = 1;
	if(FS_sendRequest(msg,type) == 0){
		print_fs("\r\n----------------------------------------\r\n");
		print_fs("\r\nFS_sendRequest Fail type=%d-------------\r\n",type);
		print_fs("\r\n----------------------------------------\r\n");
		rpt = NULL;
	}
	else{
		rpt = FS_waitResult();
		if(rpt == NULL){
			print_fs("\r\n----------------------------------------\r\n");
			print_fs("\r\nFS_waitResult Timeout type=%d------------\r\n",type);
			print_fs("\r\n----------------------------------------\r\n");
		}
	}
	userRequest = 0;
	return rpt;
}




static unsigned char FS_readStatusPackage(FS_MSG *msg)
{
	uint8 *data = msg->data;
	uint16 in = 0;
	data[in++] = 0x00;
	data[in++] = 0x01;
	data[in++] = 0x1C;
	msg->dataLen = in;
	return 1;
}

static unsigned char FS_readStatusRpt(FS_MSG *recv)
{
	uint8 *data;
	uint8 i,temp;
	uint16 in = 0;
	FS_BOX *box;

	
	if(recv->data[0] != 0xE0 || 
		recv->data[1] != 0x01 ||
		recv->data[2] != 0x24){
		print_fs("\r\nFS_REQ_STATUS:-----------bad recv---------------\r\n");
		return 0;
	}

	data = recv->data; in = 3;
	fsBill.errCode = INTEG16(data[in + 0],data[in + 1]); in += 2;
	fsBill.errAddr= INTEG16(data[in + 0],data[in + 1]); in += 2;
	fsBill.fw = INTEG16(data[in + 0],data[in + 1]); in += 2;
	for(i = 0;i < 3;i++){
		fsBill.errRegister[i] = data[in++];
	}

	for(i = 0;i < 6;i++){
		fsBill.sensorRegister[i] = data[in++];
	}

	for(i = 0;i < 4;i++){
		temp = data[in++];
		box = &fsBill.box[i];
		//box->status = 0;
		if(temp & (0x01 << 4)){
			box->status |= FS_BOX_FALSE_PLACE;
		}
		else{
			box->status &= ~FS_BOX_FALSE_PLACE;
		}
		if(temp & (0x01 << 5)){
			box->status |= FS_BOX_REMOVE;
		}
		else{
			box->status &= ~FS_BOX_REMOVE;
		}
		
		if(temp & (0x01 << 7)){
			box->status |= FS_BOX_BILL_LOW;
		}
		else{
			box->status &= ~FS_BOX_BILL_LOW;
		}
	}

	fsBill.hexSensor = data[in++];
	for(i = 0;i < 4;i++){
		box = &fsBill.box[i];
		box->l = data[in++];
		box->w = data[in++];
	}

	for(i = 0;i < 4;i++){
		box = &fsBill.box[i];
		box->h = data[in++];
	}
	for(i = 0;i < 4;i++){
		box = &fsBill.box[i];
		box->changed = data[in++];
	}
	
	print_fs("errCode=%x errAddr=%x fw =%x\r\n",fsBill.errCode,fsBill.errAddr,fsBill.fw);
	print_fs("errRegister =%x %x %x\r\n",fsBill.errRegister[0],fsBill.errRegister[1],fsBill.errRegister[2]);
	for(i = 0;i < 4;i++){
		box = &fsBill.box[i];
		print_fs("\r\nBox[%d]---------------\r\n",i);
		print_fs("w=%x l=%x h=%x status=%x",box->w,box->l,box->h,box->status);
		print_fs("\r\n----------------------\r\n");
	}

	fsBill.status &= ~FS_COM_ERR;
	fsBill.status |= fsBill.errCode & 0xFFFF;

	return 1;

	
}




//读取设备状态
unsigned char FS_readStatus(void)
{
	FS_MSG *recv;
	unsigned char res;
	print_fs("\r\n--------FS_readStatus--------\r\n");
	FS_readStatusPackage(&fsSendMsg);	
	recv = FS_userSendRequest(&fsSendMsg,FS_REQ_STATUS);
	if(recv == NULL){
		print_fs("\r\n--------FS_readStatus Timeout--------\r\n");
		return 0;
	}	
	res = FS_readStatusRpt(recv);
	print_fs("\r\n--------FS_readStatus Over--------\r\n");
	return res;
}


static uint8 FS_billResetReqPackage(void)
{
	uint8 *data = fsSendMsg.data;
	uint8 *billInfo = NULL;
	uint8 i;
	uint16 in = 0;
	
	data[in++] = 0x60;
	data[in++] = 0x02;
	data[in++] = 0x0D;
	data[in++] = 0x00;
	for(i = 0;i < 4;i++){
		billInfo = FS_getBillInfoIndex(fsBill.box[i].ch);
		if(billInfo == NULL || 
			(fsBill.box[i].status & FS_BOX_FALSE_PLACE) ||
			(fsBill.box[i].status & FS_BOX_REMOVE)){
			data[in++] = 0x00;
			data[in++] = 0x00;
		}
		else{
			data[in++] = billInfo[0];
			data[in++] = billInfo[1];
		}
	}

	for(i = 0;i < 4;i++){
		billInfo = FS_getBillInfoIndex(fsBill.box[i].ch);
		if(billInfo == NULL || 
			(fsBill.box[i].status & FS_BOX_FALSE_PLACE) ||
			(fsBill.box[i].status & FS_BOX_REMOVE)){
			data[in++] = 0x00;
		}
		else{
			data[in++] = billInfo[2];
		}
	}
	data[in++] = 0x1C;
	fsSendMsg.dataLen = in;
	return 1;

	
}

//币仓 初始化
uint8 FS_billReset(void)
{
	FS_MSG *recv;
	print_fs("\r\n------------FS_billReset---------------\r\n");
	FS_billResetReqPackage();
	recv = FS_userSendRequest(&fsSendMsg,FS_REQ_RESET);
	if(recv != NULL){
		if(recv->data[0] == 0xE0 && recv->data[1] == 0x02 &&
			recv->data[2] == 0x34){
			return 1;
		}
		else{
			if(recv->data[3] == 0xFC && recv->data[4] == 0x00){ //故障码fc 00 需要先发送出币指令
				FS_dispense(fsBill.box[0].ch);
			}
			
		}
	}
	return 0;
}





uint8 FS_billDispense(FS_BILL *bill)
{
	uint8 *data = fsSendMsg.data;
	uint8 i,temp;
	uint16 in = 0;
	FS_MSG *recv;
	FS_BOX *box;
	data[in++] = 0x60;
	data[in++] = 0x03;
	data[in++] = 0x15;
	data[in++] = 0xE4;
	for(i = 0;i < 4;i++){
		temp = bill->box[i].num;
		data[in++] = fs_code_hex[temp / 10];
		data[in++] = fs_code_hex[temp % 10];
	}


	for(i = 0;i < 4;i++){
		temp = (bill->box[i].num > 0) ? FS_DISPENSE_MAX : 0;
		data[in++] = fs_code_hex[temp / 10];
		data[in++] = fs_code_hex[temp % 10];
	}

	for(i = 0;i < 4;i++){
		temp = (bill->box[i].num > 0) ? FS_DISPENSE_RETRY : 0;
		data[in++] = temp;
	}

	data[in++] = 0x1C;
	fsSendMsg.dataLen = in;
	recv = FS_userSendRequest(&fsSendMsg,FS_REQ_DISPENSE);
	for(i = 0;i < FS_BOX_SUM;i++){
		bill->box[i].dispense = 0;
	}
	if(recv == NULL){
		return 0;
	}	

	if( recv->data[1] != 0x03 ||
		recv->data[2] != 0x99){
		print_fs("\r\nFS_REQ_DISPENSE:-----------bad recv---------------\r\n");
		return 0;
	}

	data = recv->data; in = 39;
	for(i = 0;i < 4;i++){ //实际找币数
		temp = FS_getValueByCodeHex2(data[in + 0],data[in + 1]);in += 2;
		box = &bill->box[i];
		box->dispense = (temp == 0xFF) ? 0 : temp;
		if(box->num > 0 && box->num > box->dispense){
			box->empty = 1;
		}
		print_fs("\r\n----------dispense[%d] = %d----------------------\r\n",i,bill->box[i].dispense);
	}
	return 1;
}


static uint32 FS_distribution(uint32 payAmount)
{
	uint8 i;
	FS_BOX *box;
	for(i = 0;i < FS_BOX_SUM;i++){
		box = fsBill.boxsort[i];
		if(box != NULL && box->ch > 0 && box->empty == 0){
			if(!(box->status & (FS_BOX_FALSE_PLACE | FS_BOX_REMOVE))){
				box->num = payAmount / box->ch; //配币
				payAmount = payAmount - box->num * box->ch;
				print_fs("\r\nDistribution  box[%d] num= %d\r\n",box->no,box->num);
			}
			
			if(payAmount == 0){ //配币完成
				return 0;
			}
		}	
	}
	return payAmount;
}



static void FS_dispenseReset(void)
{
	FS_distribution(0);
	FS_billDispense(&fsBill);
}

//返回已找零金额
uint32 FS_dispense(uint32 payAmount)
{
	uint8 i,k;
	uint32 temp32 = 0;
	FS_BILL_REQ *req;
	FS_BOX *box;
	req = &fsBill.req;

	req->amount = payAmount;
	req->changed = 0;
	req->remain = payAmount;

	for(i = 0;i < FS_BOX_SUM;i++){
		fsBill.box[i].empty = 0;
	}
	for(k = 0;k < 2;k++){
		temp32 = FS_distribution(req->remain);
		if(temp32 == req->remain){ //配币不成功 直接退出
			return req->changed;
		}
		else{
			FS_billDispense(&fsBill);
			for(i = 0;i < FS_BOX_SUM;i++){
				req->changed += fsBill.box[i].dispense * fsBill.box[i].ch;
			}
		}
		if(req->changed >= req->amount){ //找零完成
			break;
		}
		else{
			req->remain = req->amount - req->changed;
		}
		
	}
	return req->changed;
	
}


// 0 正常  1初始化失败   0xFF通信失败  0xEE其他故障
FS_STATUS *FS_getStatus(void)
{
	FS_BOX *box;
	FS_STATUS *s = &fsStatus;
	uint8 i;

	memset((void *)&fsStatus,0,sizeof(fsStatus));
	if(fsBill.status != 0){
		if(fsBill.status & FS_COM_ERR){
			s->status = 2;
		}
		else if(fsBill.status & FS_RESET_ERR){
			s->status = 1;
		}
		else{
			s->status = 0xFF;
			s->errCode = fsBill.status & 0xFFFF;
		}
	}
	else{
		s->status = 0;
	}

	for(i = 0;i < FS_BOX_SUM;i++){
		box = &fsBill.box[i];
		print_fs("[%d] box->ch=%d,box->status=%x\r\n",i,box->ch,box->status);
		if(box->ch > 0 && (box->status & FS_BOX_ENABLE)){
			if(box->status & FS_BOX_BILL_LOW){ //缺币
				s->box[i] = 1;
			}
			else if(box->status & FS_BOX_FALSE_PLACE || box->status & FS_BOX_REMOVE){
				//钞箱移走
				s->box[i] = 2;
			}
			else{
				s->box[i] = 0;
			}
		}
		else{
			s->box[i] = 3;
		}
		
	}


	return s;
	
	
}

//api级别更新找零器状态
void FS_mainTask(void)
{
	static uint8 err = 0;
	uint8 res;
	uint32 payAmount;
	if(SystemPara.BillRecyclerType != FS_BILLRECYCLER)
	{
		return;
	}
	if((fsBill.status & FS_COM_ERR) || (fsBill.status & FS_RESET_ERR))
	{
		print_fs("\r\n---fsBill.status = %x--\r\n",fsBill.status);
		if(FS_isTaskTimeout()){
			FS_readStatus();
			if(!(fsBill.status & FS_COM_ERR)){
				if(FS_billReset() == 1){
					fsBill.status &= ~FS_RESET_ERR;
					err = 0;
				}
				else{
					//FS_dispenseReset();
					fsBill.status |= FS_RESET_ERR;
				}
			}
			FS_setTaskTimer(10000);
		}
		
	}
	else
	{
		if(FS_isTaskTimeout())
		{
			res = FS_readStatus();
			err = (res == 0) ? err + 1: 0;
			if(err > 5){
				fsBill.status |= FS_COM_ERR | FS_RESET_ERR;
			}
			FS_setTaskTimer(5000);
		}
		#if 0
		print_fs("\r\n---fsBill.status = %x--\r\n",fsBill.status);
		print_fs("\r\n------------------Start  dispense----------------------------\r\n");
		payAmount = FS_dispense(1000);
		if(payAmount <  1000){
			print_fs("\r\n----------------------------------------------\r\n");
			print_fs("\r\nDispense failed\r\n");
			print_fs("\r\n----------------------------------------------\r\n");
			print_fs("\r\n----------------------------------------------\r\n");
		}
		#endif
	}	
}


