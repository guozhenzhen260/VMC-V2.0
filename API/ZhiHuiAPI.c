/***************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           ZhiHuiAPI.C
** Last modified Date:  2013-11-12
** Last Version:         
** Descriptions:        一鸣智慧PC 端接口                   
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:       2013-11-12
** Version:               V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
*******************************************************************************************************/


#include "..\config.h"
#include "..\App\Business.h"
#include "..\APP\CHANNEL.h"
//#include "ZhiHuiAPI.h"

#pragma arm section zidata = "RAM2"
MACDATA_ST MACData;
ZHIHHUI_MSG zhihui_msg;
GOOD_INFO vmc_goods[AISLE_SIZE];
#pragma arm section zidata

/********************************************************************************************************
** Function name:     	crc_check
** Descriptions:	    CRC和校验
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
********************************************************************************************************/
unsigned char crc_check(unsigned char *msg, unsigned short len) 
{
    unsigned short i;
    unsigned short crc = 0;
    for(i=0;i<len;i++) 
    {
		crc +=msg[i];
    }
    return crc;
}

/********************************************************************************************************
** Function name:     	zh_task3_post
** Descriptions:	     上报PC的消息队列
** input parameters:    type 上报消息类型
** output parameters:   无
** Returned value:      无
********************************************************************************************************/
void zh_task3_post(unsigned char type)
{  
	
	if((SystemPara.PcEnable != ZHIHUI_PC) && (SystemPara.PcEnable != GPRS_PC))
	{
		print_warning(4);
		return;
	}
	
	switch (type)
	{
		case MBOX_VMC_ZH_OFFLINE_REPORT://受到上线提示 通知上送当前状况	
			vmc_zh_mbox[ZhIndex].zh_status = 1;
			vmc_zh_mbox[ZhIndex].trade_index = LogPara.LogDetailPage;
			break;
		case MBOX_VMC_ZH_COIN://通知接收硬币	
		break;
		case MBOX_VMC_ZH_BILL://通知接收纸币
		
		break;
		case MBOX_VMC_ZH_ESCROWIN://通知暂存纸币
		
		break;
		case MBOX_VMC_ZH_ESCROWOUT://通知暂存纸币出币
		
		break;		
		case MBOX_VMC_ZH_BUTTON://通知用户选货
		
		break;
		case MBOX_VMC_ZH_TRADE://通知一次出货
		
		break;
		case MBOX_VMC_ZH_PAYOUT://通知一次交易完成
			vmc_zh_mbox[ZhIndex].usr_coming = 0;
						
		break;

		
		//case MBOX_VMC_RUN://VMC 恢复故障开始运行
		
		//break;
		//case MBOX_VMC_CLEAR://主控板清零
		
		//break;
		//case MBOx_VMC_MAINTENANCE_IN://主控板进入维护模式
		
		//break;
		//case MBOx_VMC_MAINTENANCE_OUT://主控板退出维护模式
		
		//break;
	
		default:
			
			break;		
	}	
	vmc_zh_mbox[ZhIndex].vmc_type  = type;
	OSQPost(g_PCMail,&vmc_zh_mbox[ZhIndex]);
	ZhIndex = (++ZhIndex) % MBOX_ST_LEN;	
	OSTimeDly(2);
}

/**************************add by yoc 2013.8.28************************************************************/
/********************************************************************************************************
** Function name:     	zh_task3_pend
** Descriptions:	     接收工控机邮箱入口
** input parameters:    payAllMoney 当前售货机接收的现金金额
** output parameters:   无
** Returned value:      无
********************************************************************************************************/
unsigned char zh_task3_pend(uint32_t payAllMoney)
{	
	
	MESSAGE_ZHIHUI *vmc_recv_msg;
	uint8_t resultdisp[8]={0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char err,result = 0;//0 正常，1单价为0，2余量为0，3货道故障，4缺货，5无此货道，6系统故障，7不够扣钱，8不能找零
	unsigned char recoin;
	unsigned short columnNo;
	if(((SystemPara.PcEnable==1)||(SystemPara.PcEnable==3)))
	{
		vmc_recv_msg = OSMboxPend(g_PCBackMail,500/5,&err);
		if(err == OS_NO_ERR)
		{
			switch(vmc_recv_msg->zh_type)
			{
				case MBOX_ZH_VMC_ONLINE:
					OSQAccept(g_PCMail);
					vmc_zh_mbox[ZhIndex].trade_index = LogPara.LogDetailPage;			
					OSTimeDly(3);					
					zh_task3_post(MBOX_VMC_ZH_OFFLINE_REPORT);
					break;

				case MBOX_ZH_VMC_OFFLINE:
					//vmc_msg_pc.trade_index = LogPara.LogDetailPage;
					//OSQAccept(g_PCMail);
					//OSTimeDly(3);									
					//zh_task3_post(MBOX_VMC_ZH_OFFLINE_REPORT);
				break;	
				case MBOX_ZH_VMC_TRADE:	
					columnNo = (unsigned short)vmc_recv_msg->cur_logic_no + 100;
					print_log("MBOX_ZH_VMC_TRADE 159line columnNo = %d\r\n",columnNo);
					// 1.如果是维护状态禁止出货
				#if 0
					if(ReadMaintainKeyValue())
					{
						print_pc("MiddZhiHui vendE9\r\n");
						result = 9;	
						goto result_end;
					}
				#endif
					
					//2.系统进入故障状态时，返回NAK_RPT  
					if(IsErrorState())
					{
						print_pc("MiddZhiHui vendE3\r\n");
						result = CHUHUO_SYS_FAULT;	
						goto result_end;
					}
					//3.货道无法出货时，返回NAK_RPT
					result = vmc_check_huodao(columnNo % 100,1);
					if(result != CHUHUO_OK)
					{
						print_pc("MiddZhiHuivend ChannelCheckIsOk %d\r\n",result);
						goto result_end;
					}
					if(vmc_recv_msg->pay_mode == TRADE_CASH ||
						vmc_recv_msg->pay_mode == TRADE_ONE_CARD)
					{
						//4.用户投币金额小于扣款金额时，返回NAK_RPT    
					    if( vmc_recv_msg->cur_goods_price > payAllMoney ) 
						{
							print_pc("MiddZhiHui vendE5\r\n ");
							result = CHUHUO_UNABLE_COST;
							goto result_end;
						}
						else
						{
							//5零钱是否够扣
							if(SystemPara.CoinChangerType == MDB_CHANGER)
							{
								if(MDBchange(resultdisp, &recoin, payAllMoney - (vmc_recv_msg->cur_goods_price )))//检查这些剩余的金额是否可以找零
									result = CHUHUO_OK;
								else
								{
									result = CHUHUO_UNABLE_PAYOUT;
									print_pc("MiddZhiHui vend MDBchange\r\n ");
									goto result_end;
								}
									
							}	
							else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
							{
								result = CHUHUO_OK;
							}
							//6压抄是否成功,是否可以支付
							if(StackMoneyInd(vmc_recv_msg->cur_goods_price)==1)
							{
								result = CHUHUO_OK;
							}
							else
							{
								result = CHUHUO_YACHAO_FAIL;
								print_pc("\r\n StackMoneyInd fail!\r\n");
							}	
						}	
					}
					else
					{
						//6如果又投入纸币 则禁止非现金出货方式
						if(payAllMoney)
						{
							print_pc("MiddZhiHui vend payAllMoney\r\n ");
							result = CHUHUO_UNABLE_PC;						
						}							
					}
result_end:
					//3如果可以出货
            	    if(result == CHUHUO_OK)
            	    {
            	    	print_pc("\r\result == CHUHUO_OK logic=%d pay=%x price= %d\r\n",
							columnNo,vmc_recv_msg->pay_mode,vmc_recv_msg->cur_goods_price);	
						//4出货
						VendoutInd(columnNo,vmc_recv_msg->cur_goods_price,vmc_recv_msg->pay_mode);
            	    }
					else
					{	
						print_pc("Trade--result = %d\r\n",result);	
						vmc_zh_mbox[ZhIndex].cur_trade_state = result;
						vmc_zh_mbox[ZhIndex].cur_logic_no = vmc_recv_msg->cur_logic_no;
						vmc_zh_mbox[ZhIndex].pay_mode = vmc_recv_msg->pay_mode;	
						vmc_zh_mbox[ZhIndex].cur_goods_price = vmc_recv_msg->cur_goods_price;
						vmc_zh_mbox[ZhIndex].trade_index =LogPara.LogDetailPage;
						zh_task3_post(MBOX_VMC_ZH_TRADE);
            	    }	
					break;
					
					case MBOX_ZH_VMC_CLEAR_OK:
					 result =  MBOX_ZH_VMC_CLEAR_OK;
					break;

					
			}
		}
		
	}
	return result;

}


/********************************************************************************************************
** Function name:     	zh_vmc_post
** Descriptions:	    工控机发送邮箱入口
** input parameters:    type 发送邮箱类型
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void zh_vmc_post(unsigned char type)
{
	zh_vmc_mbox.zh_type =type;
	OSMboxPost(g_PCBackMail,&zh_vmc_mbox);
	OSTimeDly(OS_TICKS_PER_SEC/100);
}
/********************************************************************************************************
** Function name:     pollHuman
** Descriptions:	    人体接近传感器轮询
** input parameters:    flag 若为真 则上报PC
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void pollHuman(unsigned char flag)
{
	//add by yoc newAisle
	static uint8_t human = 0;//人体感应标志 1表示有感应  0 表示无
	if(flag == human)
	{
		//没有变化不发消息
		return;
	}
	
	human = flag;
	//人体接近
	vmc_zh_mbox[ZhIndex].humanState = human;
	zh_task3_post(MBOX_VMC_ZH_HUMAN);
}

/********************************************************************************************************
** Function name:     pollDoorAPI
** Descriptions:	    机器大门轮询
** input parameters:    flag 若为真 则上报PC
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void pollDoorAPI(unsigned char flag)
{
	static unsigned char doorState = 0;//add by yoc 2013.10.31  
	if(flag)
	{
		doorState = 1;
		zh_task3_post(MBOX_VMC_ZH_DOOR_OPEN);
	}
		
	else
	{
		if(doorState)
		{
			zh_task3_post(MBOX_VMC_ZH_DOOR_CLOSE);
			doorState = 0;
		}		
	}
}

//***************智慧接口***********************************

/*********************************************************************************************************
** Function name:     getAisleInfo
** Descriptions:      获取货道信息
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned int getAisleInfo(unsigned char logicNo,unsigned char type)
{
	return ChannelGetParamValue(logicNo,type,1);
}

/*********************************************************************************************************
** Function name:     setAisleInfo
** Descriptions:       设置货道信息
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

unsigned char setAisleInfo(unsigned char logicNo,unsigned char type,const unsigned int value)
{
	unsigned char temp;
	switch(type)
	{
		case AISLE_COUNT:
			temp = CHANNELCOUNT;
		break;
		case AISLE_ID:
			temp = CHANNELGOODSNUM;
		break;
		case AISLE_MAXCOUNT:
			temp = CHANNELMAXCOUNT;
		break;
		case AISLE_PRICE:
			temp = CHANNELPRICE;
		break;

		case AISLE_SELCET:
			temp = CHANNELSELECTNUM;
		break;
		case AISLE_SELL_NUM:
			temp = CHANNELSUCCESSCOUNT;
		break;
		case AISLE_STATE:
			temp = CHANNELSTATE;
		break;
		
	}
	return	ChannelSetParam(logicNo,1,temp,(unsigned short)value,value);
}

/***********************************************************************************************************
*Function name :    vmc_check_no
*Descriptions:		检查货道号是否正常
*input parameters:  	
*output parameters:   无；
*Returned value: 	0 故障 非0 正常 
*************************************************************************************************************/
unsigned char vmc_check_no(unsigned char no)
{
	if(no == 0 || no > AISLE_SIZE)
	{
		print_warning(no);
		return 0;
	}
	if(vmc_goods[no - 1].logic_no == 0)
		return 0;
	return 1;
}


/***********************************************************************************************************
*Function name :    vmc_check_huodao
*Descriptions:		检查货道是否正常
*input parameters:  	
*output parameters:   无；
*Returned value: 	 
*************************************************************************************************************/
unsigned char vmc_check_huodao(unsigned char logicNo,unsigned char cabinetNo)
{
	unsigned char rst;
	rst = ChannelGetPhysicNum(logicNo,cabinetNo);
	if(rst == 0xFF || rst == 0x00)
	{
		print_warning(rst);
		return CHUHUO_NO_ERR;
	}
	rst = getAisleInfo(logicNo,AISLE_STATE);
	if(rst == 1)
	{
		rst = CHUHUO_OK;
	}
	else if(rst == 2)
	{
		print_warning(rst);
		return CHUHUO_AISLE_FAULT;
	}
	else if(rst == 3)
	{
		print_warning(rst);
		return CHUHUO_QUEHUO;
	}
	else
	{
		print_warning(rst);
		return CHUHUO_ERR;
	}
	rst = getAisleInfo(logicNo,AISLE_COUNT);
	if(rst == 0)
	{
		print_warning(rst);
		return CHUHUO_QUEHUO;
	}
	rst = getAisleInfo(logicNo,AISLE_PRICE);
	if(rst == 0)
	{
		print_warning(rst);
		return CHUHUO_PRICE_ERR;
	}
	
	return CHUHUO_OK;

	
	
}



/***********************************************************************************************************
*Function name :    vmc_logic_to_no
*Descriptions:		逻辑货道号转货道号 例如:十进制逻辑货到 11 转货道号为 1
*input parameters:  	
*output parameters:   无；
*Returned value: 	 0 表示失败  成功返回货道号1-80
*************************************************************************************************************/

unsigned char vmc_logic_to_no(unsigned char logic_no,GOOD_INFO *good_info)
{
	unsigned char i;
	if(logic_no == 0) 
		return 0;
	for(i = 0;i < AISLE_SIZE;i++)
	{
		if(good_info[i].logic_no  == logic_no)
		{
			return i+1;
		}		
	}
	return 0;
}

/********************************************************************************************************
** Function name:     pc_trade_info_clear
** Descriptions:	   清交易记录
** input parameters:    mode 1:本批次清除 2:清除总交易记录 3 :只清gprs
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

unsigned char pc_trade_info_clear(unsigned char mode)
{	
	if(mode == 1)//batch clear
	{	
		LogClearAPI();
		ChannelClearSuccesCount();
		gprs_clear_index();
	}
	else if(mode == 2)//sum clear
	{		
		memset((void *)&LogPara,0,sizeof(LogPara));
		WriteLogAPI();
		vmc_batch_info_write();
		gprs_clear_index();
		ChannelClearSuccesCount();		
	}
	else if(mode == 3)
	{
		gprs_clear_index();
	}
	else 
	{
		//vmc_send_pc(VMC_T_POLL,NAK_T,0,NULL);
		return 0;
	}
	//vmc_send_pc(VMC_T_POLL,ACK_T,0,NULL);
	zh_task3_post(MBOX_VMC_ZH_CLEAR);
	return 1;
}




/***********************************************************************************************************
*Function name :    vmc_sn_to_no
*Descriptions:		商品序号号转货道号 
*input parameters:  
*output parameters:   无；
*Returned value: 	 0 表示失败  成功返回货道号(如果对应多个货道号返回第一个可以出货的货道号)
*************************************************************************************************************/

unsigned char vmc_sn_to_no(unsigned char sn)
{
	unsigned char i,logicNo,state,flag = 0;
	
	if((sn == 0) || (sn == 0xFF) )
	{
		print_pc("sn = %x is not right goodsID!\r\n",sn);
		return 0;
	}
		
	//zh_goods_update(0);//更新货道状态

	for(i = 0;i < AISLE_SIZE;i++)
	{
		logicNo = vmc_goods[i].logic_no;
		if((getAisleInfo(logicNo,AISLE_ID) & 0xFFFF) == sn)			
		{	
			flag = 1;
			state = (unsigned char)getAisleInfo(logicNo,AISLE_STATE) & 0xFF;
			if(state == 1)
				return vmc_goods[i].num;
			else
				continue;
		}		
	}
	if(flag)
		return 0xEE;//表示序列号没错但是货道故障
	return 0;
	
}

	
/********************************************************************************************************
** Function name:     	mac_data_init
** Descriptions:	    初始化机器信息
** input parameters:    MACDATA_ST：机器信息
** output parameters:   无
** Returned value:      1成功 0 失败
*********************************************************************************************************/
unsigned char vmc_data_init(void)
{	

	memset((void *)&MACData,0,sizeof(MACDATA_ST)); //上电清零结构体		
	memset((void *)&vmc_goods,0,sizeof(GOOD_INFO) * AISLE_SIZE);

	MACData.machine_info.mac_sn = SystemPara.macSN;
	MACData.machine_info.vmc_sn = INTEG16(SystemPara.armSN[0],SystemPara.armSN[1]);
	MACData.machine_info.mac_t = SystemPara.vmcType;
	MACData.machine_info.aisle_sum = AISLE_SIZE;
	MACData.machine_info.column = FOOD_COLUMN;
	MACData.machine_info.row = FOOD_ROW;	
	MACData.machine_info.drink_size = (MACData.machine_info.mac_t == MAC_T_D) ? SystemPara.aisleSize : 0;
#if 0	
	MACData.machine_info.mac_sn = MAC_SN;	
	MACData.machine_info.vmc_sn = VMC_SN;
	MACData.machine_info.mac_t = MAC_T;
	MACData.machine_info.aisle_sum = AISLE_SUM;
	MACData.machine_info.column = FOOD_COLUMN;
	MACData.machine_info.row = FOOD_ROW;	
	MACData.machine_info.drink_size = (MACData.machine_info.mac_t == 0x01) ? DRINK_SIZE: 0;
#endif

	if(MACData.machine_info.mac_t == MAC_T_D)
		MACData.machine_info.c_no = 11;
	else if(MACData.machine_info.mac_t == MAC_T_F)
		MACData.machine_info.c_no = 9;
	else
		MACData.machine_info.c_no = 0; 


	zh_init();	
	//MACData.isWeihu = 0;
	//MACData.clear_state = 0;
	MACData.trade_run_no = 1;
	MACData.trade_gprs_rum_num = 1;

	return 1;
}

/********************************************************************************************************
** Function name:     	mac_data_clr
** Descriptions:	    机器信息清零
** input parameters:    MACDATA_ST：机器信息
** output parameters:   无
** Returned value:      1成功 0 失败
*********************************************************************************************************/
unsigned char vmc_data_clr(MACDATA_ST *mac_data)
{
	mac_data = mac_data;
//	MACData.clear_state = 1;//清零标志	
	return 1;
}
/***********************************************************************************************************
*Function name :    vmc_batch_info_write
*Descriptions:		汇总信息写入 flash
*input parameters:  	
*output parameters:   无；
*Returned value: 	 1表示成功 0 表示失败  
*************************************************************************************************************/

unsigned char vmc_batch_info_write(void)
{
	unsigned char crc,data[512] = {0};
//	unsigned short index = 0;
	unsigned int temp;

	//商品销售总数量不可复位
	temp = LogPara.vpSuccessNumTotal;
	data[0] = H1UINT32(temp);
	data[1] = L0UINT32(temp);
	data[2] = L1UINT32(temp);
	//商品销售的总金额 不可复
	temp = LogPara.vpSuccessMoneyTotal;
	data[3] = H0UINT32(temp);
	data[4] = H1UINT32(temp);
	data[5] = L0UINT32(temp);
	data[6] = L1UINT32(temp);
	//现金销售总数量不可复位 支付方式为01（现金）的汇总信息
	temp = LogPara.vpCashNumTotal;
	data[7] = H1UINT32(temp);
	data[8] = L0UINT32(temp);
	data[9] = L1UINT32(temp);
	//现金销售总金额不可复位
	temp = LogPara.vpCashMoneyTotal;
	data[10] = H0UINT32(temp);
	data[11] = H1UINT32(temp);
	data[12] = L0UINT32(temp);
	data[13] = L1UINT32(temp);
	//交通卡销售总数量不可复位 支付方式为02（交通卡，包括一卡通）的汇总信息
	temp = LogPara.vpOneCardNumTotal;
	data[14] = H1UINT32(temp);
	data[15] = L0UINT32(temp);
	data[16] = L1UINT32(temp);
	//交通卡销售总金额不可复位
	temp = LogPara.vpOneCardMoneyTotal;
	data[17] = H0UINT32(temp);
	data[18] = H1UINT32(temp);
	data[19] = L0UINT32(temp);
	data[20] = L1UINT32(temp);

	
	//银联卡销售总数量不可复位 支付方式为11（银联卡，包括PBOC）的汇总信息
	temp = LogPara.vpUCardNumTotal;
	data[21] = H1UINT32(temp);
	data[22] = L0UINT32(temp);
	data[23] = L1UINT32(temp);
    //银联卡销售总金额不可复位
	temp = LogPara.vpUCardMoneyTotal;
	data[24] = H0UINT32(temp);
	data[25] = H1UINT32(temp);
	data[26] = L0UINT32(temp);
	data[27] = L1UINT32(temp);
	//PC_1销售数量	 支付方式为21（提货，PC正常提货）的汇总信息
	temp = LogPara.vpOnlineNumTotal;
	data[28] = H1UINT32(temp);
	data[29] = L0UINT32(temp);
	data[30] = L1UINT32(temp);
	//PC_1销售金额
	temp = LogPara.vpOnlineMoneyTotal;
	data[31] = H0UINT32(temp);
	data[32] = H1UINT32(temp);
	data[33] = L0UINT32(temp);
	data[34] = L1UINT32(temp);
	//PC_2销售数量 支付方式为41（互联网提货，包括支付宝提货）的汇总信息
	temp = LogPara.vpPC2NumTotal;
	data[35] = H1UINT32(temp);
	data[36] = L0UINT32(temp);
	data[37] = L1UINT32(temp);
	//PC_2销售金额
	temp = LogPara.vpPC2MoneyTotal;
	data[38] = H0UINT32(temp);
	data[39] = H1UINT32(temp);
	data[40] = L0UINT32(temp);
	data[41] = L1UINT32(temp);
	//PC_3销售数量 其他PC支付方式的汇总信息
	temp = LogPara.vpPC3NumTotal;
	data[42] = H1UINT32(temp);
	data[43] = L0UINT32(temp);
	data[44] = L1UINT32(temp);
	//PC_3销售金额
	temp = LogPara.vpPC3MoneyTotal;
	data[45] = H0UINT32(temp);
	data[46] = H1UINT32(temp);
	data[47] = L0UINT32(temp);
	data[48] = L1UINT32(temp);
	
	//本批次商品销售数量 所有支付方式的汇总信息
	temp = LogPara.vpSuccessNum;
	data[49] = L0UINT32(temp);
	data[50] = L1UINT32(temp);
	//本批次商品销售金额 以分为单位
	temp = LogPara.vpSuccessMoney;
	data[51] = H1UINT32(temp);
	data[52] = L0UINT32(temp);
	data[53] = L1UINT32(temp);
	//本批次现金销售数量  支付方式为01的汇总信息
	temp = LogPara.vpCashNum;
	data[54] = L0UINT32(temp);
	data[55] = L1UINT32(temp);
	//本批次现金销售金额 以分为单位
	temp = LogPara.vpCashMoney;
	data[56] = H1UINT32(temp);
	data[57] = L0UINT32(temp);
	data[58] = L1UINT32(temp);
	//本批次卡销售数量	支付方式为02、11的汇总信息
	temp = LogPara.vpCardNum;
	data[59] = L0UINT32(temp);
	data[60] = L1UINT32(temp);
	//本批次卡销售金额 以分为单位
	temp = LogPara.vpCardMoney;
	data[61] = H1UINT32(temp);
	data[62] = L0UINT32(temp);
	data[63] = L1UINT32(temp);
	//本批次PC销售数量	支付方式为21、41、其他PC支付方式的汇总信息
	temp = LogPara.vpOnlineNum;
	data[64] = L0UINT32(temp);
	data[65] = L1UINT32(temp);
    //本批次PC销售金额 以分为单位
    temp = LogPara.vpOnlineMoney;
	data[66] = H1UINT32(temp);
	data[67] = L0UINT32(temp);
	data[68] = L1UINT32(temp);	
	
	//时间  data_to_14BCD 
	data[69] = HEC_to_BCD(RTCData.year/100);
	data[70] = HEC_to_BCD(RTCData.year%100);
	data[71] = HEC_to_BCD(RTCData.month);
	data[72] = HEC_to_BCD(RTCData.day);
	data[73] = HEC_to_BCD(RTCData.hour);
	data[74] = HEC_to_BCD(RTCData.minute);
	data[75] = HEC_to_BCD(RTCData.second);	
	//memcpy(buf,(uint8_t *)data,sizeof(BATCH_SALES_INFO));

	crc = crc_check(data,FLASH_WRITE_SIZE);
	data[FLASH_WRITE_SIZE] = crc;
	
	if(AT45DBWritePage(BATCH_FLASH_PAGE,data) != 1)//写flash
	{
		print_pc("batch_info write flash err\r\n");
		return 0;	
	}
	print_pc("batch_info write suc:\r\n");
	
	return 1;
	
}

/***********************************************************************************************************
*Function name :    vmc_batch_info_read
*Descriptions:		汇总信息读取
*input parameters:  	
*output parameters:   无；
*Returned value: 	 1表示成功 0 表示失败  
*************************************************************************************************************/

unsigned char vmc_batch_info_read(unsigned char *data)
{
	unsigned char crc,buf[512] = {0};
	
	AT45DBReadPage(BATCH_FLASH_PAGE,buf);
	
	crc = crc_check(buf,FLASH_WRITE_SIZE);

	if(buf[FLASH_WRITE_SIZE] != crc)
	{
		print_pc("batch_info read flash err\r\n");
		return 0;
	}
	memcpy((uint8_t *)data,buf,BATCH_FLASH_SIZE);

	return 1;
	//Trace("batch_info read suc:\r\n");
}

/***********************************************************************************************************
*Function name :    vmc_trade_info_read
*Descriptions:		
*input parameters:  	
*output parameters:   无；
*Returned value: 	 1表示成功 0 表示失败  
*************************************************************************************************************/
unsigned char vmc_trade_info_read(void)
{
	unsigned char crc,buf[512] = {0};
	AT45DBReadPage(TRADE_FLASH_PAGE,buf);
	crc = crc_check(buf,FLASH_WRITE_SIZE);
	print_pc("crc=%d buf[511]=%d\r\n",crc,buf[FLASH_WRITE_SIZE]);
	if(crc != buf[FLASH_WRITE_SIZE])
	{
		print_pc("gprs_flash read fail\r\n");
		return 0;
	}
	print_pc("gprs_flash read suc\r\n");
	buf[0] = buf[0];
	buf[1] = buf[1];
	MACData.read_index = INTEG16(buf[2],buf[3]);
	MACData.read_sub_index = buf[4];
	MACData.write_index = INTEG16(buf[5],buf[6]);

	return 1;
}

/***********************************************************************************************************
*Function name :    vmc_trade_info_write
*Descriptions:		flash读写
*input parameters:  	
*output parameters:   无；
*Returned value: 	 1表示成功 0 表示失败  
*************************************************************************************************************/
unsigned char vmc_trade_info_write(void)
{
	unsigned char err,crc,buf[512] = {0};

	buf[0] = 10;
	buf[1] = 0;
	buf[2] = HUINT16(MACData.read_index);
	buf[3] = LUINT16(MACData.read_index);
	buf[4] = MACData.read_sub_index;
	buf[5] = HUINT16(MACData.write_index);
	buf[6] = LUINT16(MACData.write_index);
	
	crc = crc_check(buf,FLASH_WRITE_SIZE);
	buf[FLASH_WRITE_SIZE] = crc;
	err = AT45DBWritePage(TRADE_FLASH_PAGE,buf);
	if(err != 1)
	{
		print_pc("gprs_flash write fail\r\n");
		return 0;
	}	
	return 1;
}


//===================更新程序=================================//

/*********************************************************************************************************
** Function name:       zh_fault_update
** Descriptions:       一鸣智慧故障更新
** input parameters:    MACDATA_ST 状态机构体
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void zh_fault_update(void)
{

	//售货机状态
	MACData.vend_mac_state = 0;
	if(IsErrorState() || MACData.isWeihu)//暂停营业--系统故障或者进入维护模式
	{
		MACData.vend_mac_state |= 0x01;		
	}		
	//纸币器状态
	MACData.mdb_mac_state = 0;
	if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)
	{
		if(DeviceStateBusiness.BillCommunicate == 1)
			MACData.mdb_mac_state |= 0x01;
		if(DeviceStateBusiness.Billmoto == 1)
			MACData.mdb_mac_state |= (0x01 << 1);
		if(DeviceStateBusiness.Billsensor == 1)
			MACData.mdb_mac_state |= (0x01 << 2);
		if(DeviceStateBusiness.Billromchk == 1)
			MACData.mdb_mac_state |= (0x01 << 3);
		if(DeviceStateBusiness.BillremoveCash == 1)
			MACData.mdb_mac_state |= (0x01 << 4); 
			
	}
	else if(SystemPara.BillValidatorType==OFF_BILLACCEPTER)
		MACData.mdb_mac_state |= 0x01;
		
	//硬币器状态
	MACData.coin_mac_state = 0;
	if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)
	{
		if(DeviceStateBusiness.CoinCommunicate == 1)
			MACData.coin_mac_state |= 0x01 << 0;

		if(DeviceStateBusiness.CoinCommunicate == 1)
			MACData.coin_mac_state |= 0x01 << 2;
		if(DeviceStateBusiness.Coinsensor == 1)
			MACData.coin_mac_state |= 0x01 << 3;
		if(DeviceStateBusiness.Coinromchk == 1)
			MACData.coin_mac_state |= 0x01 << 4;
		if(DeviceStateBusiness.Coinrouting == 1)
			MACData.coin_mac_state |= 0x01 << 5;
		if(DeviceStateBusiness.Cointubejam == 1)
			MACData.coin_mac_state |= 0x01 << 6;
			
	}
	else if(SystemPara.CoinAcceptorType==OFF_COINACCEPTER)
		MACData.coin_mac_state |= 0x01 << 0;
	

		
	
}

/********************************************************************************************************
** Function name:     zh_goods_update
** Descriptions:	  更新商品信息
** input parameters:    mode 1:初始化商品映射 2:更新商品状态
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void zh_goods_update(unsigned char mode)
{
	unsigned char i,index = 0;
	//aisle_size = AISLE_SIZE;
	if(mode == 1)
	{
		for(i = 0; i < 80;i++)
		{	
			//by yoc newAisle
			if(stHuodao[i/10].huodao[i%10].openFlag)//若非零则有此货道
			{
				
				vmc_goods[index].logic_no = stHuodao[i/10].huodao[i%10].logicNo;
				vmc_goods[index].num = index + 1;
				index++;
				if(index >= AISLE_SIZE)
					break;
			}		
		}
		//每个货道的商品信息
		print_pc("goods config....\r\n");
		for(i = 0;i < AISLE_SIZE;i++)
		{
			print_pc("[%d]:logic_no=%d,price=%d,state=%d,id=%x,sell=%d\r\n",
				i,vmc_goods[i].logic_no,getAisleInfo(vmc_goods[i].logic_no,AISLE_PRICE),
				getAisleInfo(vmc_goods[i].logic_no,AISLE_STATE),
				getAisleInfo(vmc_goods[i].logic_no,AISLE_ID),
				getAisleInfo(vmc_goods[i].logic_no,AISLE_SELL_NUM));			
		}
	}

	
}

/********************************************************************************************************
** Function name:     zh_init
** Descriptions:	  一鸣智慧 初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void zh_init(void)
{
	if(vmc_trade_info_read() == 1)
	{
		print_pc("init read trade_index flash suc");		
	}	
	zh_goods_update(1);
	MACData.sell_config = UserPara.TransEscape;//强制售卖 1表示投币 必须购买不退币
}


/********************************************************************************************************
** Function name:     zh_room_update
** Descriptions:	  售货机室内更新
** input parameters:    无
** output parameters:   无
** Returned value:      无
********************************************************************************************************/
	
void zh_room_update(void)

{
	MACData.light_state = NONSUPPORT;
	//日光灯时间
	MACData.lamp_time[0] = NONSUPPORT_BYBCD;
	MACData.lamp_time[1] = NONSUPPORT_BYBCD;
	MACData.lamp_time[2] = NONSUPPORT_BYBCD;
	MACData.lamp_time[3] = NONSUPPORT_BYBCD;
	//左室状态,右室状态
	MACData.left_room_state = NONSUPPORT;
	MACData.right_room_state = NONSUPPORT;
	//节能时间
	MACData.energy_save_time[0] = NONSUPPORT_BYBCD;
	MACData.energy_save_time[1] = NONSUPPORT_BYBCD;
	MACData.energy_save_time[2] = NONSUPPORT_BYBCD;
	MACData.energy_save_time[3] = NONSUPPORT_BYBCD;
	//制冷温度,加热温度
	MACData.cool_temp[0] = NONSUPPORT;
	MACData.cool_temp[1] = NONSUPPORT;
	MACData.hot_temp[0] = NONSUPPORT;
	MACData.hot_temp[1] = NONSUPPORT;


}


/********************************************************************************************************
** Function name:     zh_run_update
** Descriptions:	  售货机运行更新
** input parameters:    无
** output parameters:   无
** Returned value:      无
********************************************************************************************************/
		
void zh_run_update(void)
{
//	unsigned char i;
	MACData.run_state = 0;
	if(IsErrorState())//暂停营业
		MACData.run_state |= 0x01;
	if(0)//门开
		MACData.run_state |= 0x01 << 1;
	//硬币器检测到的硬币数量
	MACData.coin_num1 = 0;//5角
	MACData.coin_num2 = 0;//1元
	//纸币数量
	MACData.bill_num = 0;
	//每个货道的销售数量
	#if 0
	for(i = 0; i < AISLE_SIZE;i++ )
	{
	  // MACData.sales_status[i] = AccepterMsg->ColumnSuccessCount[MACData.goods[i].vmc_no-1];
	}
	#endif
	
}


/********************************************************************************************************
** Function name:     zh_all_update
** Descriptions:	 
** input parameters:    无
** output parameters:   无
** Returned value:      无
********************************************************************************************************/

void zh_all_update(void)
{
	//zh_goods_update(0);
	zh_fault_update();//货道状态信息更新
	zh_run_update();//数据更新	
	zh_room_update();	
}


/*********************************************************************************************************
** Function name:       vmcEndTrade
** Descriptions:        交易结束
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void vmcEndTrade(void)
{
	print_pc("vmcEndTrade\r\n");
	switch(SystemPara.PcEnable)
	{
		case ZHIHUI_PC:		//add by yoc zhihui	
			vmc_zh_mbox[ZhIndex].trade_index = LogPara.LogDetailPage;
			zh_task3_post(MBOX_VMC_ZH_TRADE_OVER);
			break;
		case GPRS_PC:		//add by yoc gprs
			vmc_zh_mbox[ZhIndex].trade_index = LogPara.LogDetailPage;
			zh_task3_post(MBOX_VMC_ZH_TRADE_OVER);
			break;
	}
	
}

/*********************************************************************************************************
** Function name:       gprs_clear_index
** Descriptions:        
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void gprs_clear_index(void)
{
	MACData.write_index = LogPara.LogDetailPage;
	MACData.read_index = MACData.write_index;
	MACData.read_sub_index = 0;
	vmc_trade_info_write();
}


/*********************************************************************************************************
** Function name:       vmc_enter_offline
** Descriptions:        
** input parameters:    
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

void vmc_enter_offline(void)
{
	vmc_trade_info_write();
}



