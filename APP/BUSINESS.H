/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           BUSINESS.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        交易流程头文件                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __BUSINESS_H 
#define __BUSINESS_H

void BusinessProcess(void *pvData);
extern uint8_t StackMoneyInd(uint32_t PriceSale);
extern void CostMoneyInd(uint32_t PriceSale);
extern uint8_t CostReaderRPT(uint32_t PriceSale);
extern uint32_t GetAmountMoney(void);
extern void VendoutInd(uint16_t columnNo, uint32_t PriceSale,uint8_t Type);
extern void VendoutIndFail(uint16_t columnNo, uint32_t PriceSale,uint8_t Type,uint8_t status);
extern void ResetInd(void);
extern void TuiMoneyInd(void);
extern uint8_t VendoutSIMPLEInd(uint16_t columnNo);
extern void GetmoneySIMPLEInd(uint16_t payInMoney);
extern void PriceSIMPLEInd(uint16_t payInMoney);
extern void TxtSIMPLEInd(char *disp);
extern void BillCoinCtr(uint8_t billCtr,uint8_t coinCtr,uint8_t readerCtr);
extern void rstTime(void);
extern uint32_t GetReaderAmount(void);

#endif
/**************************************End Of File*******************************************************/
