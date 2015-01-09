

#ifndef   ___VMC_GPRS_PC_H__
#define   ___VMC_GPRS_PC_H__

#include "ZHIHUIPC.h"


//程序配置
#define  GPRS_CON_FLASH	  1		//利用Flash 作为数据上报缓冲 若定义为0 则使用内存
#define  GPRS_FALSH_OFF	  3900 //	
//控制码
#define  G_REGISTER  	  51    //签到请求
#define  G_POLL		      52	//
#define  G_FUALT		  61	//
#define  G_RUN 			  62	//
#define  G_CONFIG		  63	//系统配置信息
#define  G_AISLE		  64	//货道配置信息	
#define  G_GOODS		  71	//商品销售信息
#define  G_TOTAL		  72	//商品销售汇总信息

//各包长度
#define  L_REGISTER  	  35    //签到请求长度下同
#define  L_POLL		      0		//
#define  L_FAULT		  49	//
#define  L_RUN 			  68	//
#define  L_CONFIG		  42	//系统配置信息
#define  L_AISLE		  168	//货道配置信息	
#define  L_GOODS		  87	//商品销售信息
#define  L_TOTAL		  160	//商品销售汇总信息 old 146
//回应码	  	
#define	 GPRS_A_ERR		  0	    
#define  GPRS_A_SUCCEE	  1		//成功 “00”
#define  GPRS_A_TIMEOUT	  2		//超时
#define  GPRS_A_TERMINAL  3		//签到是终端编号不存在
#define  GPRS_A_FAIL 	  4     //回应码不是“00”
#define  GPRS_A_GEN_ERR	  5		//通用失败 返回“99”
#define  GPRS_A_RESET	  6



#define  REQ	         'R'
#define  ASK		     'A'

#define  TIMES	 		   3 
#define  GPRS_HEAD	   	  37	
#define  GPRS_H_INIT   	  1
#define  GPRS_H_NOINIT    0

#define  GPRS_F_WRTIE	  0 //falsh写
#define  GPRS_F_READ	  1//flash读
#define  GPRS_F_FAULT	  3940
#define  GPRS_F_RUN	  	  3941
#define  GPRS_F_AISLE	  3942
#define  GPRS_F_CONFIG	  3943
#define  GPRS_F_TRADE	  3944
#define  GPRS_F_POLL	  3945


#define  GPRS_T_FAULT	  '1'
#define  GPRS_T_RUN	  	  '2'
#define  GPRS_T_AISLE	  '3'
#define  GPRS_T_CONFIG	  '4'
#define  GPRS_T_TRADE	  '5'
#define  GPRS_T_POLL	  '6'

typedef struct _trade_cache_node
{
	//unsigned char	node[360];
	unsigned char	len;
	unsigned char	type;
	unsigned char buf_empty;
	unsigned char recv_data;
	unsigned char	poll_req;
	unsigned char fault_req;
	unsigned char aisle_req;
	unsigned char run_req;
	unsigned char config_req;
	unsigned char com_err_num;
	
}TRADE_CACHE_NODE;
extern volatile unsigned char vmc_gprs_pc_state;
extern TRADE_CACHE_NODE trade_cache_head;

unsigned char vmc_gprs_recv(unsigned char *recv_msg,unsigned short *len,unsigned char times);
unsigned char vmc_gprs_register(unsigned char mode);
unsigned char vmc_gprs_poll(unsigned char mode);
unsigned char vmc_gprs_fualt(unsigned char mode);
unsigned char vmc_gprs_run(unsigned char mode);
unsigned char vmc_gprs_config(unsigned char mode);
unsigned char vmc_gprs_aisle(unsigned char mode);
unsigned char vmc_gprs_goods(unsigned char num,unsigned char mode);
unsigned char vmc_gprs_total(void);
unsigned char vmc_gprs_setup(void);
char ship_cache_empty(void);
unsigned char ship_cache_send(unsigned char len);
unsigned char ship_cache_poll(void);
unsigned char trade_cache_init(void);
void vmc_poll_req(void);
unsigned char vmc_gprs_trade_rpt(unsigned short index);


void task3_gprs_poll(void);

#endif
