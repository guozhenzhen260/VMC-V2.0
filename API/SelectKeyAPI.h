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
#ifndef __SELECTKEYAPI_H 
#define __SELECTKEYAPI_H

void SelectInitAPI(void);
uint8_t GetSelectKeyAPI(void);
void FreeSelectionKeyAPI( uint8_t mode );
void FreeSelectionKeyValueAPI(void);
void SaleSelectionKeyAPI( uint32_t price );
uint8_t KeyValueAPI(uint8_t freeMode,uint8_t *keyMode);
void HuodaoSetSelectionKeyAPI( uint8_t keyID );
unsigned char ReadGameKeyValueAPI(void);
unsigned char SetTemperatureAPI( unsigned int temparater );


#endif
