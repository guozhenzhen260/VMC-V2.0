
#include "colApi.h"
#include "..\config.h"


#define RESET			0x00
#define SWITCH			0x01
#define CTRL			0x02
#define COLUMN			0x03
#define POLL			0x04
#define STATUS			0x05

#if 1


static unsigned char col_addr[4] = {0x80,0x88,0xE0,0xE8};



#pragma arm section zidata = "RAM2" rwdata = "RAM2"
static volatile unsigned char recvbuf[36] = {0};
static volatile unsigned char recvlen = 0;
ST_BIN stBin;
static COL_CTRL ctrl;
#pragma arm section


/*********************************************************************************************************
** Function name:     	msleep
** Descriptions:	    毫秒睡眠函数  比较
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/

void msleep(unsigned int msec)
{
	unsigned int temp;
	temp = (msec > 10) ? msec/5 : 2;
	OSTimeDly(temp);
}


/*********************************************************************************************************
** Function name:       col_send
** Descriptions:        货道指令发送
** input parameters:    dev:设备号
**						*data:发送缓冲区 len:发送长度
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
static unsigned char col_send(unsigned char dev,unsigned char *data,unsigned char len)
{
	unsigned char res;
	memset((void *)recvbuf,0,sizeof(recvbuf));
	res = MdbConversation(dev,data,len,(unsigned char *)recvbuf,(unsigned char *)&recvlen);
	return (res == 1) ?  1 : (res == 0 ? 0 : 2);
}




/*********************************************************************************************************
** Function name:       col_reset
** Descriptions:        货道复位
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_reset(unsigned char dev)
{
	unsigned char res;
	res = col_send(dev + RESET,NULL,0);
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_switch
** Descriptions:        货道出货指令
** input parameters:    dev 机型  column货道号
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_switch(unsigned char dev,unsigned char column)
{
	unsigned char res,buf[2] = {0x00};
	buf[0] = column;
	res = col_send(dev + SWITCH,buf,2);
	return (res == 1);
}


/*********************************************************************************************************
** Function name:       col_ctrl
** Descriptions:        货道控制指令
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_ctrl(unsigned char dev,COL_CTRL *col_ctrl)
{
	unsigned char res,buf[4] = {0x00};
	
	if(col_ctrl->cool == 1){
		buf[0] |= COL_BIT_COOL;
		buf[1] = col_ctrl->coolTemp;
	}
	
	if(col_ctrl->light == 1){
		buf[0] |= COL_BIT_LIGHT;
	}
	if(col_ctrl->hot == 1){
		buf[0] |= COL_BIT_HOT;
		buf[2] = col_ctrl->hotTemp;
	}
	
	res = col_send(dev + CTRL,buf,4);
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_column
** Descriptions:        货道获取货道
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/

unsigned char col_column(unsigned char dev,ST_BIN *bin)
{
	unsigned char res,i,temp,j,index = 0;
	res = col_send(dev + COLUMN,NULL,0);
	if(res == 1){
		for(i = 0;i < 8;i++){
			temp = recvbuf[7 - i];
			for(j = 0;j < 8;j++){
				if(temp & (0x01 << j)){ //售完
					bin->col[index].empty = 1;
				}
				else{
					bin->col[index].empty = 1;
				}
				index++;
			}
		}
		
		bin->sum = recvbuf[8];
		bin->sensorFault = recvbuf[9];
		bin->coolTemp = recvbuf[10];
		bin->hotTemp = recvbuf[11];
	}
	
	
	return (res == 1);
}



/*********************************************************************************************************
** Function name:       col_poll
** Descriptions:        货道轮询
** input parameters:    dev 机型   
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_poll(unsigned char dev,unsigned char *s)
{
	unsigned char res;
	res = col_send(dev + POLL,NULL,0);
	if(res == 1){
		*s = recvbuf[0];
	}
	else{
		*s = COL_POLL_IDLE;
	}
	
	return (res == 1);
}

/*********************************************************************************************************
** Function name:       col_status
** Descriptions:        货道获取货道
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      0:通信超时  1接受正确  2 数据错误
*********************************************************************************************************/
unsigned char col_status(unsigned char dev,ST_BIN *bin)
{
	unsigned char res,index = 0,temp;
	res = col_send(dev + STATUS,NULL,0);
	if(res == 1){
		bin->mcb_ver = recvbuf[index++];
		bin->svc_ver = recvbuf[index++];
		bin->sum = recvbuf[index++];
		temp = recvbuf[index++];
		temp = recvbuf[index++];
		
		temp = recvbuf[index++];
		bin->iscool = temp & 0x01;
		bin->ishot = (temp >> 1) & 0x01;
		bin->isemptyCtrl = (temp >> 2) & 0x01;
	}
	
	
	return (res == 1);

}




/*********************************************************************************************************
** Function name:       colReset
** Descriptions:        货道初始化
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      1:成功  0 失败
*********************************************************************************************************/
unsigned char colReset(unsigned char dev)
{
	unsigned char res,s;

	Trace("colReset:dev=%d\r\n",dev);
	res = col_reset(dev);
	if(res != 1){
		return 0;
	}
	
	Timer.mdb_col_timer = 5;
	while(Timer.mdb_col_timer){
		msleep(100);
		col_poll(dev,&s);
		Trace("colReset:s=%d\r\n",s);
		if(s == COL_POLL_BUSY){
			continue;
		}
		else if(s == COL_POLL_JUSTRESET){
			msleep(100);
			col_status(dev,&stBin);
			return 1;
		}
		else if(s == COL_POLL_ERR){
			break;
		}
		else{
			break;
		}	
	}
	return 0;
	
}


/*********************************************************************************************************
** Function name:       colTrade
** Descriptions:        货道出货
** input parameters:    dev 机型 
**						
** output parameters:   
** Returned value:      1:成功  0 失败
*********************************************************************************************************/
unsigned char colTrade(unsigned char dev,unsigned char col)
{
	uint8 res,s;
	res = col_switch(dev,col);
	if(res != 1){
		return 0;
	}
	Timer.mdb_col_timer = 3;
	while(Timer.mdb_col_timer){
		msleep(50);
		col_poll(dev,&s);
		if(s == COL_POLL_BUSY){
			continue;
		}
		else if(s == COL_POLL_SUC){
			return 1;
		}
		else if(s == COL_POLL_FAIL){
			return 0;
		}
		else{
			break;
		}
	}
	return 0;
}


unsigned char colCtrl(unsigned char dev,COL_CTRL *ctrl)
{
	uint8 res,s;

	res = col_ctrl(dev,ctrl);
	if(res == 1){
		Timer.mdb_col_timer = 3;
		while(Timer.mdb_col_timer){
			msleep(50);
			res = col_poll(dev,&s);
			if(res == 1){
				if(s == COL_POLL_BUSY){
					continue;
				}
				else if(s == COL_POLL_IDLE){
					return 1;
				}
				else if(s == COL_POLL_ERR){
					return 0;
				}
				else {
					return 0;
				}
			}
		}
	}
	return 0;
	
}


unsigned char colTestBin(uint8 dev)
{
	COL_CTRL ctrl;
	unsigned char s,i,res;
	for(i = 0;i < 3;i++){
		res = colReset(dev);
		if(res == 1){
			break;
		}
		else{
			msleep(500);
		}	
	}
	
	if(res != 1){
		return 0;
	}

	for(s = 0;s < stBin.sum;s++){
		colTrade(dev,s + 1);
	}
	
	for(s = 0;s < 3;s++){
		ctrl.light = 0;
		colCtrl(dev,&ctrl);
		msleep(300);
		ctrl.light = 1;
		colCtrl(dev,&ctrl);
		msleep(300);
	}
	
	return 1;
}


/*********************************************************************************************************
** Function name:       COL_driver
** Descriptions:        MDB货道控制接口
** input parameters:    Binnum 柜号 Cmd
**						
** output parameters:   
** Returned value:      1成功  0失败
*********************************************************************************************************/

unsigned char COL_driver(unsigned char Binnum,unsigned char Cmd,unsigned char Add,unsigned char *Result)
{
	unsigned char mdb_addr,col,res = 0;
	
	if(Result != NULL){
		Result[1] = 8;
	}
	Trace("COL_driver:bin=%d,cmd=%d,add=%d\r\n",
		Binnum,Cmd,Add);
	col = Add;
	mdb_addr = col_addr[Binnum];
	switch(Cmd){
		case HEFANGUI_KAIMEN:
			res = colTrade(mdb_addr,col);
			break;
		case HEFANGUI_CHAXUN:
			res = colReset(mdb_addr);
			if(res == 1){
				Result[1] = 16;
				Result[6] = stBin.sum;
				stBin.islight 	= 1;
				Result[8] = (stBin.islight << 2) | (stBin.iscool << 1) | stBin.ishot;
			}
			break;
		case HEFANGUI_JIAREKAI:
			break;
		case HEFANGUI_JIAREGUAN:break;
		case HEFANGUI_ZHILENGKAI:break;
		case HEFANGUI_ZHILENGGUAN:break;
		case HEFANGUI_ZHAOMINGKAI:
			ctrl.light = 1;
			res = colCtrl(mdb_addr,&ctrl);
			break;
		case HEFANGUI_ZHAOMINGGUAN:
			ctrl.light = 0;
			res = colCtrl(mdb_addr,&ctrl);
			break;
		default:break;
	}
	
	return res;
}


#endif
