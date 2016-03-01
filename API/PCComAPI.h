/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           PCComAPI.H
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
#ifndef __PCCOMAPI_H 
#define __PCCOMAPI_H

extern uint32_t getchangeMoneyInd(void);
extern void setchangeMoneyInd(uint32_t changeMoney);
extern uint32_t getpayAllMoneyInd(void);
extern uint8_t getTypeInd(void);
extern void PCInitAPI(void);
extern void PayinRPTAPI(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney);
extern void PayoutRPTAPI(uint8_t payoutDev,uint8_t payoutType,uint16_t payoutMoney,uint16_t payoutRemain,uint32_t payAllMoney);
extern void ButtonRPTAPI(uint8_t type,unsigned char Logicnum,unsigned char binnum);
extern void VendoutRPTAPI( unsigned char status, unsigned char Binnum,unsigned char column, unsigned char type, uint32_t cost,uint32_t payAllMoney, unsigned char columnLeft );
extern void ActionRPTAPI(uint8_t action,uint8_t value,uint8_t second,uint8_t column,uint8_t type,uint32_t  cost,uint32_t payAllMoney);
extern uint8_t AdminRPTAPI(uint8_t adminType,uint8_t Column,uint8_t ColumnSum);
extern uint8_t AdminRPTSIMPLEAPI(uint8_t adminType,uint8_t Column,uint8_t ColumnSum);
extern uint8_t GetAdminSIMPLEAPI(uint8_t adminType,uint8_t Column);
extern void BillStatusRPTAPI(void);
extern void StatusRPTAPI(void);
extern void ButtonSIMPLERPTAPI(uint8_t channel_id);
extern void PollAPI(uint32_t payAllMoney);
extern void PCHuamanCloseAPI(void);
extern void UpdateIndex(void);
extern void UpdateSIMPLEIndex(void);
extern void WaitForPCInit(void);
#endif
