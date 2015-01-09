/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           BUSINESS.C
** Last modified Date:  2013-03-06
** Last Version:        No
** Descriptions:        维护流程头文件                    
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-06
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/
#ifndef __MAINTAIN_H 
#define __MAINTAIN_H

/******************************************维护函数*****************************************************/
void MaintainProcess(void *pvData);
void MaintainUserProcess(void *pvData);
//test demo
void TestBillValidator(void);
void TestCoinAcceptor(void);
void TestChanger(void);
void TestGoodsChannel(void);
void TestCashless(void);
void TestKeyBoard(void);
void TestSelectKey(void);
void TestATC(void);
void TestTuibi(void);
uint8_t AdminRPTWeihu(uint8_t adminType,uint8_t Column,uint8_t ColumnSum);
uint8_t TubeMoneyEnough(void);
uint8_t PCError(uint8_t PCType);



#endif
/**************************************End Of File*******************************************************/
