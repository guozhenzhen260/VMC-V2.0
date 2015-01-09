/***************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           ZHIHUIPC.C
** Last modified Date:  2013-11-04
** Last Version:         
** Descriptions:        LCD驱动程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:        2013-11-04
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
*******************************************************************************************************/
#include "ZHIHUIPC.H"
#include "..\config.h"
//#include "VMC_GPRS_PC.h"
#include "..\APP\CHANNEL.h"
#include "..\App\Business.h"

volatile unsigned char zh_pc_err;

/***********************************************************************************************************
*Function name :   vmc_send_pc
*Descriptions:		主板机与PC机串口通信协议
*input parameters: type 指令类型；flag特征字; len数据包长度;data 数据包；  				
*output parameters:   无；
*Returned value: 0 成功发送回应NAC；1 成功并回应ACK或成功发送ACK或NAK；2 成功发送并回应数据包；
*				 3 回应错误或包格式问题,4 超时连接 
*************************************************************************************************************/
unsigned char vmc_send_pc(uint8_t type,uint8_t flag,
	uint8_t len,uint8_t *data)
{
	unsigned char i,err = ZH_R_TIMEOUT,temp,recvLen = 0,req = 1;
	unsigned char crc;

	memset(zhihui_msg.send_msg,0,SEND_SIZE); //清缓冲	
	
	zhihui_msg.send_msg[0] = type;	
	if(flag == ACK_T || flag == NAK_T)//发送ACK NAK包
	{
		zhihui_msg.send_msg[1] = flag;
		zhihui_msg.send_msg[2]	= type + flag;
		print_pc("send: %2x %2x %2x\r\n",
			zhihui_msg.send_msg[0],zhihui_msg.send_msg[1],zhihui_msg.send_msg[2]);
		Uart3PutStr(zhihui_msg.send_msg,3); //发串口
		return ZH_R_ACK;
	}
	else if(flag == ADDR_T)  //发送的是地址ADDR目前未用
	{
		zhihui_msg.send_msg[1] = 0x00;
		zhihui_msg.send_msg[2] = 0x00;
		zhihui_msg.send_msg[3] = 0xEF;	
	}
	else if(flag == CMD_T) //发送的是命令CMD
	{
		zhihui_msg.send_msg[1] = 0xEF;	 //协议特征字0xEF0xEE0xFE
		zhihui_msg.send_msg[2] = 0xEE;
		zhihui_msg.send_msg[3] = 0xFE;	
	}
	else if(flag == DATA_T)//发送的是数据DATA目前未用
	{
		zhihui_msg.send_msg[1] = 0x00;
		zhihui_msg.send_msg[2] = 0x00;
		zhihui_msg.send_msg[3] = 0xFE;	
	}	
	if(len == 0)
	{
		zhihui_msg.send_msg[1] = type;
		Uart3PutStr(zhihui_msg.send_msg,2); 
	}
	else
	{
		for(i = 0;i < len;i++)
		   zhihui_msg.send_msg[i+4] = data[i];
		crc = crc_check(zhihui_msg.send_msg,len+4);
		zhihui_msg.send_msg[len + 4] =	crc & 0xff;	
		//调试用
		if(type != 0x76)
		{
			print_pc("\r\nsend_%x ",type);
			for(i = 0;i < len + 5;i++)
		    	print_pc("%x ",zhihui_msg.send_msg[i]);
			print_pc("\r\n"); 
		}
		uart3_clr_buf();//串口清
		OSSchedLock();
		Uart3PutStr(zhihui_msg.send_msg,len+5); //发串口
		OSSchedUnlock();
	}	
	Timer.zhihui_recv_timeout = ACK_TIMEOUT_SIZE;
	while(Timer.zhihui_recv_timeout)	//接收ACK 超时则返回0
	{
		if(Uart3BuffIsNotEmpty() == 1)//串口非空
		{	
			temp = Uart3GetCh();
			crc = temp;
			if(temp != type)
			{
				print_err(2);			
				err =  ZH_R_ERR_U;
				uart3_clr_buf();
				continue;
			}
			temp = Uart3GetCh();//PC 指令值			
			crc += temp;
			switch(temp)
			{
				case 0x00:
					if(crc != Uart3GetCh())
					{
						print_err(2);
						err = ZH_R_ERR_U;
					}	
					else
						err =  ZH_R_ACK;
					req = 0;
					break;
				case 0x15:			
					if(crc != Uart3GetCh())
					{
						print_err(2);
						err =  ZH_R_ERR_U;
					}
					else
						err = ZH_R_NAK;
					req = 0;
					break;
				case 0x02://设置系统时间
					recvLen = ZH_L_DATETIME;				
					break;
				case 0x03://出货请求
					recvLen = ZH_L_TRADE;				
					break;
				case 0x04://4.3	设置料道信息
					recvLen = ZH_L_AISLEINFO;				
					break;
				case 0x05://4.4	设置料道数量
					recvLen = ZH_L_AISLECOUNT;				
					break;
				default:
					print_err(2);
					err = ZH_R_ERR_U;
					req = 0;
					break;
			}
			zhihui_msg.recv_msg[0] = temp;
			if(req == 0)
			{
				print_pc("recvErr1 = %d\r\n",err);
				return err;
			}
			//如果有请求接收余下的信息
			Timer.zhihui_recv_timeout = 50;//500msec超时
			i = 0;
			while(Timer.zhihui_recv_timeout && (i < (recvLen - 1)))
			{
				if(Uart3BuffIsNotEmpty())
				{
					temp = Uart3GetCh();
					crc += temp;
					zhihui_msg.recv_msg[1 + i] = temp;//串口接收PC机回应包
					i++;
				}		
			}
			if(!Timer.zhihui_recv_timeout)
			{
				print_err(2);//超时
				return ZH_R_RTIMEOUT;
			}
#if 0
			for(i = 1;i < recvLen;i++)
			{		
				temp = Uart3GetCh();
				crc += temp;
				zhihui_msg.recv_msg[i] = temp;//串口接收PC机回应包
				Timer.zhihui_recv_timeout = 10;
				while(!Uart3BuffIsNotEmpty())
				{
					if(!Timer.zhihui_recv_timeout)
					{
						print_err(2);
						return ZH_R_RTIMEOUT;
					}			
				}
			}
#endif
			if(crc == Uart3GetCh())
			{					
				err = ZH_R_MESG;	
			}
			else
			{
				print_err(2);
				err = ZH_R_ERR_U;
			}
			print_pc("PC--VMC:");
			for(i = 0;i < recvLen;i++)
			{
				print_pc("%x ",zhihui_msg.recv_msg[i]);
			}
			print_pc("\r\n");
			return err;	
			
		}
				
	}	
	print_pc("recvErr = %d\r\n",err);
	return err;
}




/********************************************************************************************************
** Function name:     	vmc_ack_rpt
** Descriptions:	    主控板发送ACK
** input parameters:   
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
void vmc_ack_rpt(unsigned char mode)
{
	unsigned char buf[3] = {0},crc;
	buf[0] = VMC_T_POLL;
	crc = buf[0];
	if(mode == ACK_T)
	{
		buf[1] = 0x00;		 
	}
	else if(mode == NAK_T)
	{
		buf[1] = 0x15;
		crc += 0x15;	
	}
	buf[2] = crc;
	Uart3PutStr(buf,3); 
}


/********************************************************************************************************
** Function name:     	vmc_date_set
** Descriptions:	    设置时间
** input parameters:    无
** output parameters:   无
** Returned value:      1成功  0设置时间失败
*********************************************************************************************************/

static unsigned char pc_date_set(unsigned char *r_data)
{

	unsigned char err;
	if(MACData.usr_trade == 1)
	{
		vmc_ack_rpt(NAK_T);	
		print_warning(MACData.usr_trade);
		return 0;
	}
	else
		vmc_ack_rpt(ACK_T);
	print_pc("PC_SYS_TIME_SET....\r\n");	
	RTCData.year = (unsigned short)BCD_to_HEC(r_data[0])*100+BCD_to_HEC(r_data[1]);
	RTCData.month = BCD_to_HEC(r_data[2]);
	RTCData.day = BCD_to_HEC(r_data[3]);
	RTCData.hour = BCD_to_HEC(r_data[4])
		;
	RTCData.minute = BCD_to_HEC(r_data[5]);
	RTCData.second = BCD_to_HEC(r_data[6]);
	err = RTCSetTime(&RTCData);
	print_pc("RTCSetTime(&RTCData) return %d",err);			
	return 1;	 
}

/********************************************************************************************************
** Function name:     	pc_aisle_set
** Descriptions:	   设置货道
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void pc_aisle_set(void)
{
	unsigned char i,logicNo	;
	
	if(MACData.usr_trade == 1)
	{
		vmc_ack_rpt(NAK_T);
		print_pc("vmc is busy can't recv the Pc CMD....\r\n");
		return;
	}
	else
		vmc_ack_rpt(ACK_T);
	for(i = 0; i < 48;i++)
	{
		
		logicNo = vmc_goods[i].logic_no;
		setAisleInfo(logicNo,AISLE_ID,i+1);	 	
	}
	
	ChannelSaveParam();//柜1
	
}


/********************************************************************************************************
** Function name:     	pc_cost_trade
** Descriptions:	    扣款出货请求 ;泡面柜和食品柜，支持饮料柜
** input parameters:    无
** output parameters:   无
** Returned value:     1成功 0 失败 
*********************************************************************************************************/
static unsigned char pc_cost_trade(unsigned char *r_data)
{
	unsigned char num,i,index = 0,sn[3],lastSn[3],tradeType,num_sn,
		cabinet,result = CHUHUO_OK,payMode;//PC发来的流水号
	unsigned int  price;	
	//饮料机或是食品机	
	cabinet  = r_data[index++];
	cabinet = cabinet;
	//解析出货类型 1货道号 2 商品序号
	tradeType = r_data[index++];
	//解析货道号/商品序号
	num_sn = r_data[index++];
	//解析售卖金额
	price = INTEG32(0,r_data[index++],r_data[index++],r_data[index++]);	
	//解析支付方式
	payMode = r_data[index++];
	//解析PC流水号
	sn[0]= r_data[index++];
	sn[1]= r_data[index++];
	sn[2]= r_data[index++];
	if(sn[0] == MACData.trade_run_num[0] &&
		sn[1] == MACData.trade_run_num[1] &&
		sn[2] == MACData.trade_run_num[2] )
	{
		vmc_ack_rpt(ACK_T);	
		print_warning(2);//防止重包
		return 0;
	}
	if(MACData.usr_trade == 1)
	{
		vmc_ack_rpt(NAK_T);
		print_warning(2);
		return 0;
	}		
	else
		vmc_ack_rpt(ACK_T);	
	
	//解析卡号 目前没用
	for(i = 0;i < 10;i++)
		MACData.trade_card_no[i] = r_data[index+i];
	index += 10;

	if(tradeType == 1)
	{	
		num = num_sn;
		if(vmc_check_no(num_sn) == 0)
		{	
			print_pc("GoodsNo is not right!\r\n");
			result = CHUHUO_NO_ERR;
		}
	}
	else if(tradeType == 2)
	{	
		num = vmc_sn_to_no(num_sn);
		if(num == 0)
		{	
			print_pc("GoodsId is not right!\r\n");
			result = CHUHUO_ID_ERR;
		}
	}
	else
	{
		print_pc("PC trade_req is not right!\r\n");
		result = CHUHUO_TYPE_ERR;
	}	
	if((payMode == TRADE_CASH || payMode == TRADE_ONE_CARD) &&
		price != getAisleInfo(vmc_goods[num - 1].logic_no,AISLE_PRICE))
	{
		print_warning(price);
		result = CHUHUO_PRICE_ERR;
	}
	if(Timer.zhihui_trade_time)
	{
		print_log("Timer.zhihui_trade_time > 0\r\n User select the aisle wait for 60sec\r\n");
		result = CHUHUO_BUTTON_ERR;
	}
	if(result != CHUHUO_OK)
	{
		lastSn[0] = MACData.trade_run_num[0];
		lastSn[1] = MACData.trade_run_num[1];
		lastSn[2] = MACData.trade_run_num[2];
		MACData.trade_run_num[0] = sn[0];
		MACData.trade_run_num[1] = sn[1];
		MACData.trade_run_num[2] = sn[2];
		vmc_trade_info(vmc_goods[num - 1].logic_no, payMode, price,result);
		OSTimeDly(300/5);
		MACData.trade_run_num[0] = lastSn[0];
		MACData.trade_run_num[1] = lastSn[1];
		MACData.trade_run_num[2] = lastSn[2];
		return 0;
	}
	MACData.trade_pay_mode = payMode;
	MACData.trade_run_num[0] = sn[0];
	MACData.trade_run_num[1] = sn[1];
	MACData.trade_run_num[2] = sn[2];
	MACData.usr_trade = 1;
	
	zh_vmc_mbox.cur_logic_no = vmc_goods[num - 1].logic_no;
	zh_vmc_mbox.cur_goods_price = (unsigned short)price;
	zh_vmc_mbox.pay_mode = MACData.trade_pay_mode;
	//zh_goods_update(0);
	print_pc("pc_cost_trade num=%d,price=%d,pay=%x,logic=%d\r\n",
		num,zh_vmc_mbox.cur_goods_price,
		zh_vmc_mbox.pay_mode,zh_vmc_mbox.cur_logic_no);

	zh_vmc_post(MBOX_ZH_VMC_TRADE);
	return 1;
}


/********************************************************************************************************
** Function name:     	pc_goods_set
** Descriptions:	    设置VMC货道属性
** input parameters:    无
** output parameters:   无
** Returned value:     1成功 0 失败 
*********************************************************************************************************/
static unsigned char pc_goods_set(unsigned char *r_data)
{
	unsigned char num,logicNo,index = 0,cabinet;
	unsigned short price,sn;
	if(MACData.usr_trade == 1)
	{
		vmc_ack_rpt(NAK_T);	
		print_warning(MACData.usr_trade);
		return 0;
	}
	else
		vmc_ack_rpt(ACK_T);
	
	cabinet = r_data[index++];//柜号
	cabinet = cabinet;
	num = r_data[index++];//1-48货道号
#if ID_BYTES == 1
	sn =  r_data[index++];
#else
	sn = INTEG16(r_data[index++],r_data[index++]);
#endif
	
	price = INTEG16(r_data[index++],r_data[index++]) * 10;		
	logicNo = vmc_goods[num - 1].logic_no;

	setAisleInfo(logicNo,AISLE_ID,sn);
	setAisleInfo(logicNo,AISLE_PRICE,price);

	print_pc("logicNo = %d,id=%x,price=%d\r\n",logicNo,sn,price);	
	ChannelSaveParam();//柜1
	//maintenance = 1;
	zh_all_update();
	vmc_goods_info(3);
	OSTimeDly(300/5);
	vmc_goods_price(3);
	OSTimeDly(300/5);
	return 1;
}
/********************************************************************************************************
** Function name:     	pc_clear_trade
** Descriptions:	  清交易记录
** input parameters:    无
** output parameters:   无
** Returned value:     1成功 0 失败 
*********************************************************************************************************/
static unsigned char pc_clear_trade(unsigned char type)
{
		if(MACData.usr_trade == 1)
		{
			vmc_ack_rpt(NAK_T);
			print_warning(MACData.usr_trade);
			return 0;
		}
		else
			vmc_ack_rpt(ACK_T);
		if(type == 0x01)//本批次
		{
			print_pc("PC batch clear............");					
			pc_trade_info_clear(1);			
		}
		else if(type == 0x02)//总交易清除
		{
			print_pc("PC sum clear............");					
			pc_trade_info_clear(2);			
		}
		return 1;
}

/********************************************************************************************************
** Function name:     	pc_ailse_num_set
** Descriptions:	   设置料到数量
** input parameters:    无
** output parameters:   无
** Returned value:     1成功 0 失败 
*********************************************************************************************************/
static unsigned char pc_ailse_num_set(unsigned char *acks)
{
	unsigned char num,add_nums,logicNo,cabinet;
	
	if(MACData.usr_trade == 1 || getAisleInfo(logicNo,AISLE_STATE) == 2)
	{
		vmc_ack_rpt(NAK_T);	
		print_warning(MACData.usr_trade);
		print_warning(getAisleInfo(logicNo,AISLE_STATE));
		return 0;
	}
	else
		vmc_ack_rpt(ACK_T);

	cabinet = acks[0];
	cabinet = cabinet;
	num = acks[1];
	add_nums = acks[2];	
	logicNo = vmc_goods[num -1].logic_no;

	setAisleInfo(logicNo,AISLE_COUNT,add_nums);
	setAisleInfo(logicNo,AISLE_MAXCOUNT,add_nums);
	setAisleInfo(logicNo,AISLE_STATE,1);//add by yoc 2013.11.05	

	
#if 0
	maxCount = getAisleInfo(logicNo,AISLE_MAXCOUNT);	
	add_nums = (add_nums > maxCount) ? maxCount: add_nums;	
	setAisleInfo(logicNo,AISLE_COUNT,add_nums);
	setAisleInfo(logicNo,AISLE_STATE,1);//add by yoc 2013.11.05	
#endif

	ChannelSaveParam();//柜1	
	vmc_aisle_run_info(2);
	OSTimeDly(250/5);
	return 1;

}


/********************************************************************************************************
** Function name:     	vmc_register_pc
** Descriptions:	    主控板发送签到指令
** input parameters:    无
** output parameters:   无
** Returned value:      1,签到成功  0签到失败
*********************************************************************************************************/
unsigned char vmc_register_pc(void)
{
	unsigned char index,err,data[VMC_L_REGISTER] = {0};
	unsigned int temp;
//	memset(data,0,DATA_BUF_SIZE); //清缓冲
	//主控板版本号2字节	十六进制	
	index = 0;
	data[index++] = HUINT16(MACData.machine_info.vmc_sn);
	data[index++] = LUINT16(MACData.machine_info.vmc_sn);

	temp = MACData.machine_info.mac_sn;//版本号例如 11040070 
	index+=4;
	data[index - 1] = HEC_to_BCD(temp % 100);
	temp /= 100;
	data[index - 2] = HEC_to_BCD(temp % 100);
	temp /= 100;
	data[index - 3] = HEC_to_BCD(temp % 100);
	temp /= 100;
	data[index - 4] = HEC_to_BCD(temp % 100);		

    //index = = 6
	data[index++] = HEC_to_BCD(RTCData.year/100);
	data[index++] = HEC_to_BCD(RTCData.year%100);
	data[index++] = HEC_to_BCD(RTCData.month);
	data[index++] = HEC_to_BCD(RTCData.day);
	data[index++] = HEC_to_BCD(RTCData.hour);
	data[index++] = HEC_to_BCD(RTCData.minute);
	data[index++] = HEC_to_BCD(RTCData.second);	
	
	data[index++] = MACData.machine_info.mac_t;//0X01饮料机0X11食品机0X21提货机固定 0x20饮料机一拖N模式
	data[index++] = (MACData.machine_info.mac_t == MAC_T_D)
		? 1 : 0;
	data[index++] = (MACData.machine_info.mac_t == MAC_T_F)
		? 1 : 0;
	data[index++] = 0;//resered
	data[index++] = (MACData.machine_info.mac_t == MAC_T_D)
		? MACData.machine_info.drink_size : 0;
	data[index++] = (MACData.machine_info.mac_t == MAC_T_F)
		? AISLE_SIZE: 0;

	data[index++] = 0;//resvered
	err = vmc_send_pc(VMC_T_REGISTER,CMD_T,VMC_L_REGISTER,data);//16
	
	if(err == ZH_R_ACK)
		return 1;
	else
		return 0;


}

/********************************************************************************************************
** Function name:     	vmc_poll_pc
** Descriptions:	    主控板发送POLL指令
** input parameters:    无
** output parameters:   无
** Returned value:     1成功 0 失败 
*********************************************************************************************************/
unsigned char vmc_poll_pc(void)
{
	unsigned char err,pc_type,index = 0,data[VMC_L_POLL] = {0};

	//memset(data,0,DATA_BUF_SIZE); //清缓冲
	data[index++] = HUINT16(MACData.all_amount);//硬币器和纸币器金额
	data[index++] = LUINT16(MACData.all_amount);
	data[index++] = HUINT16(MACData.bill_amount);//纸币器金额
	data[index++] = LUINT16(MACData.bill_amount);
	data[index++] = HUINT16(MACData.coin_amount);//硬币器金额
	data[index++] = LUINT16(MACData.coin_amount);


	if(!MACData.sel_num && MACData.rt_state)
	{
			
		data[index++] = 0x00 ; 
		data[index++] = 0x00;
#if ID_BYTES == 1
		data[index++] = 0x00;
#else
		data[index++] = 0x00; 
		data[index++] = 0x00; 
#endif		
		data[index++] = 0x00;
		data[index++] = 0x01;//人体接近

	}
	else
	{
			
		data[index++] =  (MACData.sel_num != 0) ? MACData.machine_info.c_no : 0 ; 
		data[index++] = MACData.sel_num; //检测用户选择的货道号
#if ID_BYTES == 1
		data[index++] = MACData.sel_id; //检测用户选择的商品序号
#else
		data[index++] = HUINT16(MACData.sel_id); 
		data[index++] = LUINT16(MACData.sel_id); 
#endif
		
		data[index++] = HUINT16(MACData.sel_amount/10);//商品价格
		data[index++] = LUINT16(MACData.sel_amount/10);

	}
	
	err = vmc_send_pc(VMC_T_POLL,CMD_T,VMC_L_POLL,data);
	if(err == ZH_R_MESG)
	{				
		pc_type	= zhihui_msg.recv_msg[ACK_OFF+0];	
		if(pc_type == PC_SYS_TIME_SET) //系统时间设置
		{				
			pc_date_set(&zhihui_msg.recv_msg[ACK_OFF + 1]); 
		}
		else if(pc_type ==  PC_DEDUCT_SHIP)//出货请求
		{	
			pc_cost_trade(&zhihui_msg.recv_msg[ACK_OFF + 1]);	
		}	
		else if(pc_type == PC_GOODS_SET) 
		{
			pc_goods_set(&zhihui_msg.recv_msg[ACK_OFF + 1]);			
		}
		else if(pc_type == PC_ADD_GOODS ) 
		{
			pc_ailse_num_set(&zhihui_msg.recv_msg[ACK_OFF + 1]);		
		}			
		else if(pc_type == PC_CLEAR_SET) //清零
		{
			pc_clear_trade(zhihui_msg.recv_msg[ACK_OFF+1]);				
		}	
		else if(pc_type == PC_AISLE_SET) //商品ID一次性配置
		{
			pc_aisle_set();
		}
		else	//PC发送请求错误
		{
			vmc_ack_rpt(NAK_T);
			zh_pc_err++;
			return 0;
		}
						
		if(zh_pc_err) zh_pc_err = 0;
		return 1;
	}
	else if(err == ZH_R_ACK)
	{
		if(zh_pc_err) zh_pc_err = 0;
		return 1;
	}	
	zh_pc_err++;
	return 0;
		
}



/********************************************************************************************************
** Function name:     	vmc_human_rpt
** Descriptions:	    上报PC机人体接近
** input parameters:    
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void vmc_human_rpt(unsigned char state)
{
	MACData.rt_state = state;
	//vmc_send_pc();
}


/********************************************************************************************************
** Function name:     	vmc_online_rpt
** Descriptions:	      在线模式上报信息
** input parameters:    
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void vmc_online_rpt(unsigned short tradeIndex)
{
	if(GetWeihuStatus() == 0)
		MACData.isWeihu = 0;
	else
		MACData.isWeihu = 1;
	MACData.write_index = tradeIndex;
	
}


/********************************************************************************************************
** Function name:     	vmc_weihu_rpt
** Descriptions:	  进入维护模式
** input parameters:    
** output parameters:   
** Returned value:      
*********************************************************************************************************/

void vmc_weihu_rpt(void)
{
	MACData.isWeihu = 1;
}



/********************************************************************************************************
** Function name:		vmc_normal_rpt
** Descriptions:		恢复正常模式
** input parameters:	
** output parameters:	
** Returned value:		
*********************************************************************************************************/
void vmc_normal_rpt(void)
{
	unsigned char err = 1;
	MACData.isWeihu = 0;	
	if(SystemPara.PcEnable == 1)
		vmc_report_pc(1);
	else if(SystemPara.PcEnable == 3)
	{
		print_pc("vmc_gprs_setup is finish!\r\n");
		//err = vmc_gprs_setup();
	}	
	if(err == 0)
		print_pc("vmc_normal_rpt register fail!!!\r\n");
}


/********************************************************************************************************
** Function name:		vmc_door_rpt
** Descriptions:		大门状态报告
** input parameters:	flag = 1 打开  0 关闭  
** output parameters:	
** Returned value:		
*********************************************************************************************************/
void vmc_door_rpt(unsigned char flag)
{
	unsigned char err = 1,buf[VMC_L_SUM] = {0},day;
	print_pc("vmc_door_rpt[%d]\r\n",flag);
	MACData.door_state = flag;
	if(flag)
	{
		
	}
	else
	{	
		if(vmc_batch_info_read(buf) != 1)
		{
			print_pc("vmc_door_rpt vmc_batch_info_read fail!!!\r\n");
			day = 0;
		}
		else
			day =  BCD_to_HEC(buf[72]);
		
		//hour = BCD_to_HEC(data[73]);
		if(RTCData.day > day)//自动清零
		{
			pc_trade_info_clear(1);
		}
		else
		{
			if(SystemPara.PcEnable == 1)
				vmc_report_pc(1);
			else if(SystemPara.PcEnable == 3)
			{
				//	err = vmc_gprs_setup();	
				print_pc("fun:vmc_door_rpt is not finish!\r\n");
			}
			
				
			if(err == 0)
			print_pc("vmc_door_rpt register fail!!!\r\n");
		}

	}	
}


/********************************************************************************************************
** Function name:     	vmc_trade_rpt
** Descriptions:	    主控板发送出货上报PC
** input parameters:    logic_no 逻辑号 十进制  pay_mode支付方式 
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
void vmc_trade_rpt(unsigned char logic_no,unsigned char pay_mode,
						unsigned short cost,unsigned char trade_result)
{

	if(!vmc_trade_info(logic_no,pay_mode,cost,trade_result))
	{
		print_warning(2);
	}
	if(!trade_result)
		Timer.zhihui_trade_time = 3;//出货后3秒内不允许再出货
	MACData.usr_trade = 0;  //允许PC出货请求	
	Timer.zhihui_trade_time = 0; 
}



/********************************************************************************************************
** Function name:     	vmc_button_rpt
** Descriptions:	    主控板上报选货信息到PC
** input parameters: pay_mode支付方式 logic_no逻辑号 十进制
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void vmc_button_rpt(unsigned char pay_mode,unsigned char logic_no)
{
	
	//MACData.usr_trade = 1;
	Timer.zhihui_button_timeout = 4;//用户按下货道时上报PC通知4秒
	Timer.zhihui_trade_time = 60;//有用户按键则60秒内拒绝PC出货
	MACData.trade_pay_mode = pay_mode;
	MACData.sel_num = vmc_logic_to_no(logic_no,vmc_goods);					
	MACData.sel_id = (unsigned short)getAisleInfo(logic_no,AISLE_ID);
	MACData.sel_amount = (unsigned short)getAisleInfo(logic_no,AISLE_PRICE);
}


/********************************************************************************************************
** Function name:     	vmc_trade_info
** Descriptions:	    主控板发送出货指令
** input parameters:    货道编号1-48
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_trade_info(unsigned char logic_no,unsigned char pay_mode,
					unsigned short cost,unsigned char trade_result)
{
	unsigned char i,err,num,index = 0,data[VMC_L_TRADE] = {0};
	unsigned short temp;
	num = vmc_logic_to_no(logic_no,vmc_goods);
	
	
	data[index++] = MACData.machine_info.c_no; 
	data[index++] = num;//货道编号
#if ID_BYTES == 1
	data[index++] = (unsigned char)getAisleInfo(logic_no,AISLE_ID);//商品序号
#else
	temp = (unsigned short)getAisleInfo(logic_no,AISLE_ID);//商品序号
	data[index++] = HUINT16(temp);
	data[index++] = LUINT16(temp);
#endif
	
	
	data[index++] = 0;
	temp = cost;//add by yoc 2013.10.31
	data[index++] = HUINT16(temp);
	data[index++] = LUINT16(temp);
	
	
	data[index++] = MACData.trade_run_no++;//序列号 0 - 255 主控板一次出货产生的 流水号	
	data[index++] = pay_mode; //支付方式 为PC机出货时提供	
	data[index++] = trade_result;//出货情况 0出成功货；1该货道无货或故障； 2：其他原因没有出货；3：PC机出货，信息不正确
	//data[7] - data[16]一卡通卡号
	//流水号 BCD码，PC机出货时提供的，6位流水号压缩。

	index += 10;//卡号忽略


	if(pay_mode == TRADE_CASH || pay_mode == TRADE_ONE_CARD)
	{
		data[index++] = 0;
		data[index++] = 0;		
		data[index++] = 0;		
	}
	else
	{
		data[index++] = MACData.trade_run_num[0];
		data[index++] = MACData.trade_run_num[1];		
		data[index++] = MACData.trade_run_num[2];
	}
	//商品销售总数量
	data[index++] = H1UINT32(LogPara.vpSuccessNumTotal);
	data[index++] = L0UINT32(LogPara.vpSuccessNumTotal);
	data[index++] = L1UINT32(LogPara.vpSuccessNumTotal); 
	//商品销售的总金额
	data[index++] = H0UINT32(LogPara.vpSuccessMoneyTotal);
	data[index++] = H1UINT32(LogPara.vpSuccessMoneyTotal);
	data[index++] = L0UINT32(LogPara.vpSuccessMoneyTotal);
	data[index++] = L1UINT32(LogPara.vpSuccessMoneyTotal);

	for(i = 0;i < 5;i++)
	{
		err = vmc_send_pc(VMC_T_TRADE,CMD_T,VMC_L_TRADE,data);
		if(err != 1)
			OSTimeDly(300/5);
		else
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;
		}			
	}
	zh_pc_err = 20;	
	return 0;
}

/********************************************************************************************************
** Function name:     	vmc_trade_over_rpt
** Descriptions:	    交易结束
** input parameters:   
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void vmc_trade_over_rpt(void)
{
	MACData.bill_amount = 0;
	MACData.coin_amount =0;	
	MACData.all_amount = 0;
	MACData.card_amount = 0;
	MACData.sel_num = 0x00;
	MACData.sel_id = 0x00;
  	MACData.sel_amount = 0x0000;
	MACData.trade_pay_mode = 0x00;
	MACData.change_amount = 0;
		
	MACData.usr_trade = 0;  //允许PC出货请求	
	Timer.zhihui_trade_time = 0; 
		
}


/********************************************************************************************************
** Function name:     	vmc_trade_over_rpt
** Descriptions:	    交易结束
** input parameters:   
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void vmc_money_rpt(unsigned char type,unsigned short recvAmount,unsigned short recvAllAmount)
{
	switch (type)
	{
		case ZH_BILL:
			if(MACData.escrow_flag)
			{						
				print_pc("MBOX_VMC_ZH_BILL_YaChao....\r\n");
				MACData.escrow_flag = 0;
			}
			else
			{		
				MACData.bill_amount += recvAmount;
			}
			MACData.trade_pay_mode = 0x01;
			MACData.all_amount = recvAllAmount;
			break;

		case ZH_COIN:
			MACData.coin_amount += recvAmount;					
			MACData.trade_pay_mode = 0x01;	
			MACData.all_amount = recvAllAmount;
			break;
		case ZH_ESCROWIN:
			MACData.escrow_flag = 1;
			MACData.bill_amount += recvAmount;
			MACData.trade_pay_mode = 0x01;
			MACData.all_amount = recvAllAmount;
			break;
		case ZH_ESCROWOUT:
			MACData.bill_amount -= recvAmount;
			MACData.all_amount = recvAllAmount;
			MACData.escrow_flag = 0;
			break;

		default:
			break;
	}

	
	
}


/********************************************************************************************************
** Function name:     	printTrade
** Descriptions:	  调试用
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void printTrade()
{
	print_pc("bill=%d,coin=%d,card=%d,num=%d,sn=%4d,price=%d,pay_mode=%d,change=%d,usr_trade=%d\r\n",
		MACData.bill_amount,MACData.coin_amount ,MACData.card_amount,
		MACData.sel_num,MACData.sel_id,MACData.sel_amount,MACData.trade_pay_mode,
		MACData.change_amount,MACData.usr_trade );	
}


/********************************************************************************************************
** Function name:     	vmc_sale_sum
** Descriptions:	    商品销售汇总信息
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_sale_sum(unsigned char rcx)
{
	unsigned char i,data[BATCH_FLASH_SIZE] = {0}; 
//	unsigned int temp;
	
	//memset(data,0,DATA_BUF_SIZE); //清缓冲
	if(vmc_batch_info_read(data) != 1)
	{
		print_pc("Total trade info read err Can't report to PC!\r\n");
		//return 1;
	}
	
	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_SUM,CMD_T,VMC_L_SUM,data) == 1)//old is 61
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}
	
	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_mac_run_info
** Descriptions:	    机器运行信息
** input parameters:    无
** output parameters:   无
** Returned value:       1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_mac_run_info(unsigned char rcx)
{
	unsigned char i,data[VMC_L_MAC_RUN]={0};

	data[0] = ((MACData.door_state & 0x01) << 1) |(MACData.vend_mac_state & 0x01);
	data[1] = MACData.vend_mac_state;
	data[2] = MACData.mdb_mac_state;
	data[3] = MACData.coin_mac_state;
	data[4] = HUINT16(MACData.coin_num1);
	data[5] = LUINT16(MACData.coin_num1);
	data[6] = HUINT16(MACData.coin_num2);
	data[7] = LUINT16(MACData.coin_num2);
	data[8] = HUINT16(MACData.bill_num);
	data[9] = LUINT16(MACData.bill_num);

	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_MAC_RUN,CMD_T,VMC_L_MAC_RUN,data) == 1)//
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}

	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_aisle_run_info
** Descriptions:	    料道运行信息
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_aisle_run_info(unsigned char rcx)
{
	unsigned char i,data[VMC_L_AISLE_RUN] = {0},index = 0,arr_no,arr_set,logicNo;
	//unsigned short temp;
	data[index++] = MACData.machine_info.c_no;
	//data[index++] = MACData.run_state;
	for(i = 0;i < AISLE_SIZE;i++)
	{
		arr_no = i / 8;
		arr_set = 0x01 << (i % 8);
		logicNo = vmc_goods[i].logic_no;
		if(getAisleInfo(logicNo,AISLE_STATE) == 1)//正常
			data[index + arr_no] &= ~arr_set;	
		else
			data[index + arr_no] |=  arr_set;
	}
	index += 6;
	
	for(i = 0;i < AISLE_SIZE;i++){
	   logicNo = vmc_goods[i].logic_no;
	   //更改为剩余量
	   data[i+7] = (unsigned char)getAisleInfo(logicNo,AISLE_COUNT);

	}
	
	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_AISLE_RUN,CMD_T,VMC_L_AISLE_RUN,data) == 1)//
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}
	
	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_config_info
** Descriptions:	    系统配置信息
** input parameters:    无
** output parameters:   无
** Returned value:       1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_config_info(unsigned char rcx)
{
	unsigned char i,data[VMC_L_CONFIG] = {0},index = 0;
//	memset(data,0,DATA_BUF_SIZE); //清缓冲
	data[index++] = MACData.machine_info.c_no;
	data[index++] = (MACData.sell_config << 1) | (0x01);
	data[index++] = MACData.light_state;
	data[index++] = HEC_to_BCD(MACData.lamp_time[0]);
	data[index++] = HEC_to_BCD(MACData.lamp_time[1]);
	data[index++] = HEC_to_BCD(MACData.lamp_time[2]);
	data[index++]	= HEC_to_BCD(MACData.lamp_time[3]);
	data[index++] = MACData.left_room_state;
	data[index++] = MACData.right_room_state;
	data[index++] = HEC_to_BCD(MACData.energy_save_time[0]);
	data[index++] = HEC_to_BCD(MACData.energy_save_time[1]);
	data[index++] = HEC_to_BCD(MACData.energy_save_time[2]);
	data[index++] = HEC_to_BCD(MACData.energy_save_time[3]);
	data[index++] = MACData.cool_temp[0];
	data[index++] = MACData.cool_temp[1];
	data[index++] = MACData.hot_temp[0];
	data[index++] = MACData.hot_temp[1];
	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_CONFIG,CMD_T,VMC_L_CONFIG,data) == 1)//
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}
	
	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_goods_info
** Descriptions:	    料道商品信息
** input parameters:    无
** output parameters:   无
** Returned value:       1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_goods_info(unsigned char rcx)
{
	unsigned char i,data[VMC_L_GOODS_SN]= {0};
	print_pc("goods_ID\r\n");
	data[0] = MACData.machine_info.c_no;
	for(i = 0; i < AISLE_SIZE;i++ )
	{
#if ID_BYTES == 1
		data[i+1] = getAisleInfo(vmc_goods[i].logic_no,AISLE_ID);
#else
		temp = (unsigned short)getAisleInfo(vmc_goods[i].logic_no,AISLE_ID);
		data[i*2 + 1] = HUINT16(temp);
		data[i*2 + 1 + 1] = LUINT16(temp);
#endif
	}	
	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_GOODS_SN,CMD_T,VMC_L_GOODS_SN,data) == 1)//
		{
			if(zh_pc_err) zh_pc_err = 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}	
	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_goods_price
** Descriptions:	    料道价格信息
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_goods_price(unsigned char rcx)
{			
	unsigned char i,data[VMC_L_GOODS_PRICE]= {0};
	unsigned short price;

	//memset(data,0,DATA_BUF_SIZE); //清缓冲
	print_pc("goods_pric is \r\n");

	data[0] = MACData.machine_info.c_no;
	for(i = 0; i < AISLE_SIZE;i++){
		//32位分为单位转换角单位
		price = getAisleInfo(vmc_goods[i].logic_no,AISLE_PRICE) / 10;
		data[i*2+ 1] = HUINT16(price);
		data[i*2+ 2] = LUINT16(price);
		
		
	}
	print_pc("\r\n"); 
	for(i = 0;i < rcx;i++)
	{
		if(vmc_send_pc(VMC_T_GOODS_PRICE,CMD_T,VMC_L_GOODS_PRICE,data) == 1)//old is 61
		{
			if(zh_pc_err) zh_pc_err= 0;
			return 1;	
		}
	   	OSTimeDly(300/5);
		zh_pc_err++;
	}
	
	return 0;

}

/********************************************************************************************************
** Function name:     	vmc_offline_report
** Descriptions:	    离线上报
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/

unsigned char vmc_offline_report(unsigned short index,unsigned char sub_index)
{
		return 1;		
}


/********************************************************************************************************
** Function name:     	vmc_trade_clear
** Descriptions:	   交易清零
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/

void vmc_trade_clear(void)
{	
	unsigned char err;
	if(MACData.isWeihu)//维护模式不用重新签到
		return;
	if(SystemPara.PcEnable == 1)
		vmc_report_pc(1);
	else if(SystemPara.PcEnable == 3)
	{
		print_pc("fun:vmc_trade_clear is finish!\r\n");
		//err = vmc_gprs_setup();
	}
	
	if(err == 0)
		print_pc("vmc_trade_clear register  fail!!!!\r\n");
	
}


/********************************************************************************************************
** Function name:     	vmc_setup_pc
** Descriptions:	    主控板与PC签到成功后附加发送机器总体信息；失败则根据主控板配置循环与PC签到
** input parameters:    无
** output parameters:   无
** Returned value:      1 成功 0 失败
*********************************************************************************************************/
unsigned char vmc_setup_pc(const unsigned char mode)
{	
	unsigned char i;
	do
	{
		for(i = 0;i < 5;i++)
		{		
			if(vmc_register_pc() == 1)
			{
				zh_pc_err = 0;		
				return 1;
			}
			OSTimeDly(1000/5);
		}
	}
	while(mode);
	zh_pc_err = 50;
	return 0;
}


/********************************************************************************************************
** Function name:     	vmc_report_pc
** Descriptions:	    主控板与PC发送机器总体信息；
** input parameters:    type 1表示需要签到
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void vmc_report_pc(unsigned char type)
{
	if(type == 1)
	{
		if(!vmc_setup_pc(0))
			return;
	}
	vmc_goods_info(5);
	OSTimeDly(400/5);
	vmc_goods_price(5);	
	OSTimeDly(400/5);
	vmc_config_info(5);
	OSTimeDly(400/5);
	vmc_sale_sum(5);
	OSTimeDly(400/5);
	vmc_aisle_run_info(5);	
	OSTimeDly(400/5);
	vmc_mac_run_info(5);
	OSTimeDly(400/5);
}

/********************************************************************************************************
** Function name:     	task3_zh_poll
** Descriptions:	  智慧轮询
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void task3_zh_poll(void)
{
	unsigned char err;
	MESSAGE_ZHIHUI *accepter_msg;
	static unsigned char zh_tick,zh_tick_flag;
	if(zh_pc_err > 10)//连续10次通讯失败则通知PC进入离线模式
	{
		print_pc("vmc_pc_err_num>10\r\n");
		//通知主控板进入离线模式
		zh_vmc_post(MBOX_ZH_VMC_OFFLINE);	
		vmc_setup_pc(1);//签到
		zh_vmc_post(MBOX_ZH_VMC_ONLINE);
		accepter_msg = OSQPend(g_PCMail,5000,&err);
		if(err == OS_NO_ERR)
		{
			if(accepter_msg->vmc_type == MBOX_VMC_ZH_OFFLINE_REPORT)
			{
				MACData.write_index = accepter_msg->trade_index;	
				vmc_report_pc(0);
			}
		}	
	}
	if(!MACData.usr_trade && !MACData.isWeihu && (zh_tick>=10))
	{	
	   //OSTimeDly(250/5);
	   switch (zh_tick_flag)
	   {
			case 1:
				zh_all_update();
				vmc_mac_run_info(2);  	//机器运行信息		
				break;
			case 2:
				vmc_aisle_run_info(2);   //机器故障信息	
			   	break;
			case 3:
				vmc_config_info(2);		
			   	break;
			case 4:
				vmc_goods_info(2);//料道商品信息	
			   	break;
			case 5:
				vmc_goods_price(2);//料道价格信息			
			   	break;
			default:
				zh_tick_flag = 0;
				zh_tick = 0;				
			    break;
	   }	
		zh_tick_flag ++;
		OSTimeDly(250/5);
	}
	if(!MACData.sel_num && !Timer.zhihui_button_timeout )//通知到时
	{
		MACData.sel_num =0;
		MACData.sel_id = 0;
		MACData.sel_amount = 0;
	}
	vmc_poll_pc(); //轮询PC机 空闲状态	
		
	zh_tick++;	
}






/*************************************End Of File******************************************************/





