/***************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           ZHIHUIPC.H
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        LCD驱动程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-04
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
*******************************************************************************************************/



#ifndef __ZHIHUI_PC_H__
#define __ZHIHUI_PC_H__



extern volatile unsigned char zh_pc_err;

/***********************************************************
* 函数声明
************************************************************/
void vmc_com_pc(void);
unsigned char vmc_shipment_pc(unsigned char num);
void vmc_trade_rpt(unsigned char logic_no,unsigned char pay_mode,
					unsigned short cost,unsigned char trade_result);
void vmc_trade_over_rpt(void);
void vmc_human_rpt(unsigned char state);
void vmc_weihu_rpt(void);
void vmc_normal_rpt(void);
void vmc_door_rpt(unsigned char flag);
void vmc_online_rpt(unsigned short tradeIndex);
void vmc_money_rpt(unsigned char type,unsigned short recvAmount,unsigned short recvAllAmount);
void vmc_button_rpt(unsigned char pay_mode,unsigned char logic_no);



unsigned char vmc_setup_pc(const unsigned char mode);
void vmc_report_pc(unsigned char type);
unsigned char vmc_register_pc(void);
unsigned char vmc_poll_pc(void);
unsigned char vmc_aisle_run_info(unsigned char rcx);
unsigned char vmc_mac_run_info(unsigned char rcx);
unsigned char vmc_config_info(unsigned char rcx);
unsigned char vmc_goods_info(unsigned char rcx);
unsigned char vmc_goods_price(unsigned char rcx);
unsigned char vmc_sale_sum(unsigned char rcx);
unsigned char vmc_trade_info(unsigned char logic_no,unsigned char pay_mode,
					unsigned short cost,unsigned char trade_result);

unsigned char vmc_data_init(void);
unsigned char vmc_send_pc(unsigned char type,unsigned char flag,unsigned char len,unsigned char *data);
unsigned char vmc_offline_ship(unsigned char mode);
void vmc_ack_rpt(unsigned char mode);
void vmc_trade_update(unsigned char num,unsigned char pay_mode,unsigned short price);
void vmc_trade_clear(void);
void printTrade(void);
void gprs_clear_index(void);
void task3_zh_poll(void);


#endif

