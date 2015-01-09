/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           SelectKey.h
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



#ifndef _SELECTKEY_H

typedef struct	
{
	//选货按键;by gzz 20121025
	uint8_t  sel1ReadyLed;               //按键板1的√灯
	uint8_t  sel1ErrLed;                 //按键板1的×灯
	uint8_t  sel2ReadyLed;               //按键板2的√灯
	uint8_t  sel2ErrLed;                 //按键板2的×灯
	uint8_t  sel3ReadyLed;               //按键板3的√灯
	uint8_t  sel3ErrLed;                 //按键板3的×灯
	uint8_t  sel4ReadyLed;               //按键板4的√灯
	uint8_t  sel4ErrLed;                 //按键板4的×灯
	uint8_t  sel5ReadyLed;               //按键板5的√灯
	uint8_t  sel5ErrLed;                 //按键板5的×灯
	uint8_t  sel6ReadyLed;               //按键板6的√灯
	uint8_t  sel6ErrLed;                 //按键板6的×灯
	uint8_t  selectMode;                 //模式,0空闲状态,1交易状态
}SELECTKEY;
extern SELECTKEY selKey;

uint8_t GetSelectKey(void);
void SelectKey_InitProc(void);
void UpdateSelectionLed(MessageKEYPack *AccepterMsg);


#define _SELECTKEY_H

#endif
