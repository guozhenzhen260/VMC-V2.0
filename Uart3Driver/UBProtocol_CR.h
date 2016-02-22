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
#define VP_ERR_NULL     0
#define VP_ERR_PAR      1   //mission parameter error
#define VP_ERR_COM      2   //communication error

#define VP_SF             0xAA
#define VP_PROEASIV_SF    0xE5//骏鹏协议
#define VP_PROTOCOL_VER   0x0//协议版本
#define VP_PROEASIV_VER   0x40//骏鹏协议
#define VP_PROTOCOL_ACK   0x80
#define VP_PROEASIV_ACK   0x01//骏鹏协议
#define VP_PROTOCOL_NAK   0x00

//==========
//VMC-->PC
//==========
#define VP_ACK_RPT       1
#define VP_NAK_RPT       2
#define VP_POLL          3
#define VP_VMC_SETUP     4
#define VP_STARTUP_RPT   5
#define VP_PAYIN_RPT     6
#define VP_PAYOUT_RPT    7
#define VP_VENDOUT_RPT   8
#define VP_REQUEST       9
#define VP_ADMIN_RPT     10
#define VP_ACTION_RPT    11   //V2
#define VP_BUTTON_RPT    12
#define VP_STATUS_RPT    13
#define VP_HUODAO_RPT    14
#define VP_CARD_RPT      15
#define VP_COST_RPT      16//扣款后，返回扣款执行结果;by gzz 20110823
//#define VP_DEBT_RPT      17//返回欠条结果;by gzz 20110825
#define VP_INFO_RPT      17//120419 by cq TotalSell 
#define VP_VENDOUT_REQ   18
#define VP_OFFLINEDATA_RPT   19

#define VP_STARTUP_RPT_1   0x22 //120419 by cq TotalSell


#define VP_MT_MIN_SEND  VP_TEXT_MSG
//#define VP_MT_MAX_SEND  VP_DEBT_RPT
#define VP_MT_MAX_SEND  VP_OFFLINEDATA_RPT//120419 by cq TotalSell 
//#define VP_MT_MAX_SEND  VP_STARTUP_RPT_1//120419 by cq TotalSell 

//==========
//PC-->VMC
//==========
#define VP_ACK             1
#define VP_NAK             2
#define VP_GET_SETUP_IND   4
#define VP_GET_STATUS      5
#define VP_VENDOUT_IND     131
#define VP_RESET_IND       132
#define VP_CONTROL_IND     133
#define VP_HOUDAO_IND      135
#define VP_POSITION_IND    136
#define VP_PAYOUT_IND      137    //V2
#define VP_GET_HUODAO	   138
#define VP_COST_IND        139//执行扣款命令;by gzz 20110823
#define VP_GETINFO_IND     140//120419 by cq TotalSell 
#define VP_SALETIME_IND    141
#define VP_SALEPRICE_IND   142
#define VP_HUODAO_SET_IND  143
#define VP_GET_OFFLINEDATA_IND   145
#define VP_GETINFO_INDEXP  146
#define VP_SET_HUODAO      147



#define VP_MT_MIN_RECEIVE  VP_ACK
#define VP_MT_MAX_RECEIVE  VP_SET_HUODAO

//================
//应用层的协议常量
//================
#define VP_MAC_SET_H        0
#define VP_MAC_SET_L        1
#define COLUMN_NUM_SET      80
#define VP_DEV_COIN         0
#define VP_DEV_BILL         1
#define VP_DEV_ESCROWIN        100
#define VP_DEV_ESCROWOUT       101
#define VP_DEV_READER         2
#define VP_BUT_GAME           0
#define VP_BUT_NUMBER         1
#define VP_BUT_GOODS          2
#define VP_BUT_RETURN         4

#define VP_ACT_HEART          0
#define VP_ACT_CHUHUO         1
#define VP_ACT_PAYOUT         2
#define VP_ACT_ADMIN          5
#define VP_ACT_ONLINE         6
#define VP_ACT_PCON           7


#define VP_CONTROL_BILLCOIN   2
#define VP_CONTROL_CABINETDEV 3
#define VP_CONTROL_PAYOUT     6
#define VP_CONTROL_GAMELED    16
#define VP_CONTROL_CLOCK      17
#define VP_CONTROL_SCALFACTOR 18
#define VP_CONTROL_INITOK     19
#define VP_CONTROL_HEFANGUI   21


#define VP_ADMIN_GOODSADDALL  1
#define VP_ADMIN_GOODSADDCOL  2
#define VP_ADMIN_CHANGEADD    3
#define VP_ADMIN_GOODSCOLUMN  4
#define VP_ADMIN_GOODSADDTRAY 8
#define VP_ADMIN_GETBILL      9
#define VP_ADMIN_GOODSBUHUO  10

#define VP_INFO_TOTALVALUE    3
#define VP_INFO_CLOCK         5
#define VP_INFO_CHUHUO        6
#define VP_INFO_HUODAO        10
#define VP_INFO_POSITION      11
#define VP_INFO_SALEPRICE     12
#define VP_INFO_VMC           13
#define VP_INFO_VER           14
#define VP_INFO_HARD          15
#define VP_INFO_BILL          16
#define VP_INFO_COIN          17
#define VP_INFO_COMP          18
#define VP_INFO_ERR           23
#define VP_INFO_HEFANGGUI     24
#define VP_INFO_CABINET       25
#define VP_INFO_CABERR        26


#define VP_TIME_OUT 150
#define VP_COM_RETRY 3



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
	uint32_t changeMoney;
	uint32_t costMoney;
	uint8_t  payoutDev;
	uint8_t  type;
	uint8_t Column;
	uint8_t status;                     
	uint8_t ColumnSum;    
	uint8_t method; 
	uint8_t device; 
	
	//向PC机下发设备状态
	uint8_t check_st;                   //找零器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t bv_st;                      //纸币器状态：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t cc_st;                      //硬币器状态（包含找零功能）：0=正常，1=被软件临时禁用，2=故障，3=设备不存在
	uint8_t vmc_st;                     //VMC状态（代表整个售货机）：0=正常，1= 正常货道商品全部售空，或非售卖时间，2=故障，3=维护模式
	uint16_t change;                    //找零量总金额
	uint8_t tem_st;                     //货仓状态设置值，共支持4个货仓
	uint16_t coinSum[6];                //每种通道硬币的存币数量
	uint16_t recyclerSum[6];            //每种通道纸币的存币数量

	uint8_t action;
	uint8_t value;
	uint8_t second; 
	uint8_t  Control_Bin;                  //箱柜号 
	uint8_t  Control_Huodao;              //箱柜的货道号 
	uint8_t  cabinetNums;                 //目前非普通柜的数量 
	uint8_t  cabinetdata[30];             //所有非普通柜状态
};
//extern struct VP_Mission sysVPMission;


//unsigned char VPBusTxMsg( void );
extern void VPSerialInit_CR(void);
extern unsigned char VPMission_Poll( uint8_t *isInit );
extern unsigned char VPMission_Payin_RPT(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney);
extern unsigned char VPMission_Payout_RPT( uint8_t payoutDev,unsigned char Type, unsigned int payoutMoney, unsigned int payAllMoney );
extern unsigned char VPMission_Cost_RPT( unsigned char Type, uint32_t costMoney, unsigned int payAllMoney );
extern unsigned char VPMission_Button_RPT( unsigned char type, unsigned char value,uint8_t device );
extern unsigned char VPMission_Vendout_RPT( unsigned char status, unsigned char device,unsigned char column, unsigned char type, unsigned int cost, unsigned int payAllMoney, unsigned char columnLeft );
extern unsigned char VPMission_Status_RPT( uint8_t check_st,uint8_t bv_st,uint8_t cc_st,uint8_t vmc_st,uint16_t change,uint8_t tem_st,uint16_t *recyclerSum,uint16_t *coinSum );
extern unsigned char VPMission_Act_RPT( unsigned char action, uint8_t value,uint8_t second,uint8_t column,uint8_t type,uint32_t cost,uint32_t payAllMoney);
extern unsigned char VPMission_Admin_RPT( unsigned char type,uint8_t Column,uint8_t ColumnSum);
extern unsigned char VPMission_Info_RPT( unsigned char type, unsigned int payAllMoney,uint8_t check_st,uint8_t bv_st,uint8_t cc_st,uint8_t Control_Huodao,uint8_t value,uint8_t Control_device,uint8_t  cabinetNums,uint8_t *cabinetdata );
#endif

