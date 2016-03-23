/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           UBProtocol.h
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
********************************************************************************************************/



#ifndef __UBPROTOCOL_CR_H__
#define __UBPROTOCOL_CR_H__

//==========
//协议包信息
//==========
#define CR_ERR_NULL     0
#define CR_ERR_PAR      1   //mission parameter error
#define CR_ERR_COM      2   //communication error

#define CR_SF             0xAA
#define CR_PROTOCOL_VER   0x0//协议版本
#define CR_PROTOCOL_ACK   0x80
#define CR_PROTOCOL_NAK   0x00

//==========
//VMC-->PC
//==========
#define CR_ACK_RPT       1
#define CR_NAK_RPT       2
#define CR_POLL          3
#define CR_VMC_SETUP     4
#define CR_STATUS_RPT   5
#define CR_INFO_RPT      6
#define CR_PAYIN_RPT     8
#define CR_PAYOUT_RPT    9
#define CR_COST_RPT      10//扣款后，返回扣款执行结果;by gzz 20110823
#define CR_ACTION_RPT    11   //V2
#define CR_BUTTON_RPT    12
#define CR_RESET_RPT       13

//==========
//PC-->VMC
//==========
#define CR_ACK             1
#define CR_GET_SETUP_IND   4
#define CR_GET_STATUS      5
#define CR_GETINFO_IND     6
#define CR_CONTROL_IND     7
#define CR_PAYIN_IND     8
#define CR_PAYOUT_IND      9    //V2
#define CR_COST_IND        10//执行扣款命令;by gzz 20110823
#define CR_RESET_IND       13

#define CR_MT_MIN_RECEIVE  CR_ACK
#define CR_MT_MAX_RECEIVE  CR_RESET_IND

//================
//应用层的协议常量
//================
#define CR_DEV_COIN         0
#define CR_DEV_BILL         1
#define CR_DEV_ESCROWIN        2
#define CR_DEV_ESCROWOUT       3
#define CR_CONTROL_BILLCOIN   0

#define CR_INFO_ERR           0
#define CR_INFO_TOTALVALUE    1
#define CR_INFO_VER           2
#define CR_INFO_HARD          3
#define CR_INFO_BILL          4
#define CR_INFO_COIN          5
#define CR_INFO_COINOUT    6
#define CR_INFO_BILLOUT    7
#define CR_TIME_OUT 150


#define VPM_LEN_MAX   250
struct VMC_PC_MSGCR
{
    uint8_t   sf;                //fixed with 0xe7
	uint8_t   len;               //5~250, msg+5
    uint8_t   datLen;            //
	uint8_t   verFlag;           //0x01( none response ); 0x81( need response )
    uint8_t   sn;                //0~255
	uint8_t   msgType;           //
	uint8_t   msg[VPM_LEN_MAX];  //0~250
	uint16_t    chk;
};


struct VP_MissionCR
{
    struct  VMC_PC_MSG  send;
	struct  VMC_PC_MSG  receive;
	uint8_t  payInDev;
	uint16_t payInMoney;
	uint32_t payAllMoney;
	uint16_t payoutMoney;
	uint16_t payremainMoney;
	uint32_t changeMoney;
	uint32_t costMoney;
	uint8_t  payoutDev;
	uint8_t  type;
	uint8_t Column;
	uint8_t status;                     
	uint8_t ColumnSum;    
	uint8_t method; 
	uint8_t device; 
	uint8_t action;
	//向PC机下发设备状态
	uint8_t bv_st;                      //纸币器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t check_st;                   //纸币找零器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t cc_st;                      //硬币器状态（包含找零功能）：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t vmc_st;                     //硬币找零器状态（代表整个售货机）：0=正常，1= 正常货道商品全部售空，或非售卖时间，2=故障，3=维护模式
	uint32_t change;                    //硬币找零量总金额
	uint32_t recychange;                     //纸币找零量总金额	
};
//extern struct VP_Mission sysVPMission;


//unsigned char VPBusTxMsg( void );
extern void VPSerialInit_CR(void);
extern unsigned char VPMission_Poll_CR( void);
extern unsigned char VPMission_Payin_RPT_CR(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney);
extern unsigned char VPMission_Payout_RPT_CR( uint8_t payoutDev,unsigned char Type, unsigned int payoutMoney,  unsigned int payremainMoney, unsigned int payAllMoney );
extern unsigned char VPMission_Cost_RPT_CR( unsigned char Type, uint32_t costMoney, unsigned int payAllMoney );
extern unsigned char VPMission_Button_RPT_CR( void );
extern unsigned char VPMission_Status_RPT_CR(void);
extern unsigned char VPMission_Act_RPT_CR( unsigned char action);
extern unsigned char VPMission_Info_RPT_CR( uint8_t type );
extern unsigned char VP_Reset_Rpt_CR( void );
extern unsigned char VPMission_Setup_RPT_CR( void );
#endif

