/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           uart0.h
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        uart0驱动程序相关函数声明                      
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-04
** Version:             V0.1 
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#ifndef __UART1_H 
#define __UART1_H

void InitUart1(void);
void Uart1IsrHandler(void);
void Uart1PutChar(unsigned char ch);
void Uart1PutStr(unsigned char const *Str, unsigned int Len);
unsigned char Uart1BuffIsNotEmpty(void);
unsigned char Uart1GetCh(void);
void Uart1_ClrBuf(void);
uint8_t Uart1_Read(uint8_t *buf, uint8_t len);
 void SetUart1ParityMode(unsigned char mode) ;

#endif
/**************************************End Of File*******************************************************/
