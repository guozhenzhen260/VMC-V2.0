/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           key.h
** Last modified Date:  2013-01-08
** Last Version:         
** Descriptions:        2*8键盘驱动头文件                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-10
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/
#ifndef __KEY_H 
#define __KEY_H

//用户键盘
void InitKeyboard(void);
void EINT2_IsrHandler(void);//中断服务函数，应用不得调用
unsigned char ReadKeyValue(void);
void ClearKey(void);
void DisabaleKeyBoard(void);
void EnableKeyBoard(void);
//键盘测试函数,维护模式中键盘自检调用
void TestKeyBoard(void);
//其他单独按键
unsigned char ReadMaintainKeyValue(void);
unsigned char ReadPayBackKeyValue(void);
unsigned char ReadGameKeyValue(void);
unsigned char ReadCloseHumanKeyValue(void);
void gameLedControl(unsigned char flag);
unsigned char GetKeyInValue(void);

 #endif
/**************************************End Of File*******************************************************/
