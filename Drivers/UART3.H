/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           Uart3.h
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        Uart3驱动程序相关函数声明                      
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
#ifndef __UART3_H 
#define __UART3_H

void InitUart3(void);
void Uart3IsrHandler(void);
void Uart3PutChar(unsigned char ch);
void uart3_clr_buf(void);
void Uart3PutStr(unsigned char const *Str, unsigned int Len);
unsigned char Uart3BuffIsNotEmpty(void);
unsigned char Uart3GetCh(void);
unsigned char Uart3Read(unsigned char *buf, unsigned char len); 

#endif
/**************************************End Of File*******************************************************/
