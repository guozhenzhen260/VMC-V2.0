/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           MDB_UART2.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        建立在UART2上的MDB通讯协议函数头文件                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-03-05
** Version:             V2.0
** Descriptions:        The original version      
********************************************************************************************************/
#ifndef __MDB_H 
#define __MDB_H

void MdbBusHardwareReset(void);
uint8_t MdbConversation(uint8_t Dev,uint8_t *SeDat,uint8_t SenDatLen,uint8_t *RevDat,uint8_t *RevLen);	


#endif
/**************************************End Of File*******************************************************/
