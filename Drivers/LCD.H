/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           display.h
** Last modified Date:  2013-01-04
** Last Version:         
** Descriptions:        display驱动程序头文件                  
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-01-04
** Version:              
** Descriptions:        The original version       
********************************************************************************************************/
#ifndef	__LCD_H
#define	__LCD_H

void LCDInit(void);
void LCDBackLight(uint8_t ctrl);
void LCDClrScreen(void);
void LCDClrArea(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
void LCDSetCursor(uint8_t x,uint8_t y);
void LCDPutAscii(uint8_t x,uint8_t y,char *str,uint8_t Inverse);
void LCDPutChinese(uint8_t x,uint8_t y,char *str,uint8_t Inverse);
void LCDPutStr(uint8_t x,uint8_t y,char *str,uint8_t Inverse,uint8_t Language);
void LCDPrintf(uint8_t x,uint8_t y,uint8_t Inverse,uint8_t Language,char *format,...);
void LCDPrintf32(uint8_t x,uint8_t y,uint8_t Inverse,uint8_t Language,char *format,...);
void LCDNumberFontPrintf(uint8_t x,uint8_t y,uint8_t font,char *format,...);
void LCDSpecialPicPrintf(uint8_t x,uint8_t y,uint8_t mode,uint8_t font);
void LCDPutLOGOBmp(unsigned char x,unsigned char y,unsigned char mode);
void LCDClearLineDraw(uint8_t x,unsigned char row, uint8_t font);
void LCDPutBmp(uint8_t x,uint8_t y,const uint8_t W,const uint8_t H,const uint8_t *data,uint8_t Inverse);
void LCDDrawLine(unsigned char x,unsigned char y);
void LCDDrawRectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);

#endif   
/**************************************End Of File*******************************************************/
