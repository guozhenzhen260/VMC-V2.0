/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           AT45DB161D.c
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        SPI Flash驱动程序                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-03-04
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

#define PAGESIZE 512

void SPIInit(void);
void SPIHightSpeed(unsigned char on);
void SPIDeviceSelect(unsigned char selectchip);
unsigned char SPIReadAndWriteCh(unsigned char Ch);
unsigned char AT45DBWaitOK(void);
unsigned char AT45DBCompare(uint8_t nbuf, uint16_t PageAddr);
void AT45DBWriteBuffer(uint8_t nbuf,uint16_t start,uint16_t len,uint8_t *pbuf);

/*********************************************************************************************************
** Function name:     	InitAT45DB
** Descriptions:	    初始化AT45DB161D
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void InitAT45DB(void) 
{
	SPIInit();
	SPIHightSpeed(1);
}
/*********************************************************************************************************
** Function name:     	AT45DBReadID
** Descriptions:	    读取AT45DB161D的ID
** input parameters:    无
** output parameters:   无
** Returned value:      返回状态
*********************************************************************************************************/
unsigned char AT45DBReadID(unsigned char *id) 
{
	unsigned char i;
	unsigned char status = 0x00;
	SPIDeviceSelect(1); 
	SPIReadAndWriteCh(0x9F);
	for(i=0;i<4;i++)
	{
		*id++ = SPIReadAndWriteCh(0xFF);
	}
	SPIDeviceSelect(0);
	return status;
}
/*********************************************************************************************************
** Function name:     	AT45DBReadStatus
** Descriptions:	    读取AT45DB161D状态
** input parameters:    无
** output parameters:   无
** Returned value:      返回状态
*********************************************************************************************************/
unsigned char AT45DBReadStatus(void) 
{
	unsigned char status = 0x00;
	SPIDeviceSelect(1); 
	SPIReadAndWriteCh(0xD7);
	status = SPIReadAndWriteCh(0xFF);
	SPIDeviceSelect(0);
	return status;
}
/*********************************************************************************************************
** Function name:     	AT45DBWaitOK
** Descriptions:	    读取AT45DB161D状态，看芯片是否处理完毕，回到空闲
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char AT45DBWaitOK(void)
{
	unsigned char status;
	while(1) 
	{
		status = AT45DBReadStatus();
		if(status & 0x80) 
		{
			if(status == 0xFF) 
			{
				return 1;
			} 
			return 0;
		}
	}
}
/*********************************************************************************************************
** Function name:     	AT45DBReadNByte
** Descriptions:	    读取AT45DB161D的数据
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void AT45DBReadNByte(uint16_t PageAddr,uint16_t start,uint16_t len,uint8_t *pbuf) 
{
	uint16_t i;     
	AT45DBWaitOK();	
	SPIDeviceSelect(1);  
	SPIReadAndWriteCh(0x0B); 
	SPIReadAndWriteCh((uint8_t)(PageAddr >> 6));				   
	SPIReadAndWriteCh((uint8_t)((PageAddr << 2)|(start >> 8))); 
	SPIReadAndWriteCh((uint8_t)start);
	SPIReadAndWriteCh(0xFF);
	for(i=0; i<len; i++) 
	{
		pbuf[i] = SPIReadAndWriteCh(0xFF); 
	}
	SPIDeviceSelect(0);
}
/*********************************************************************************************************
** Function name:     	AT45DBWriteBuffer
** Descriptions:	    写入buffer寄存器中
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void AT45DBWriteBuffer(uint8_t nbuf,uint16_t start,uint16_t len,uint8_t *pbuf) 
{
	uint16_t i;      
	AT45DBWaitOK();
	SPIDeviceSelect(1);  
	switch(nbuf) 
	{
		case 1 : SPIReadAndWriteCh(0x84); break;
		case 2 : SPIReadAndWriteCh(0x87); break;
		default: break;
	}
	SPIReadAndWriteCh(0x00);
	SPIReadAndWriteCh((uint8_t)(start >> 8));
	SPIReadAndWriteCh((uint8_t)start);
	for(i=0; i<PAGESIZE; i++) 
	{
		SPIReadAndWriteCh(pbuf[i]); 
	}
	SPIDeviceSelect(0);
}
/*********************************************************************************************************
** Function name:     	AT45DBWriteNByte
** Descriptions:	    写入真实flash中
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void AT45DBWriteNByte(uint8_t nbuf, uint16_t PageAddr, uint16_t start, uint16_t len, uint8_t *pbuf) 
{
   AT45DBWriteBuffer(nbuf,start,len,pbuf);//写入buffer寄存器中
   AT45DBWaitOK();//等到写完成
   //从buffer写入到真正的flash中
   SPIDeviceSelect(1);
   switch(nbuf) 
   {
      case 1 : SPIReadAndWriteCh(0x83); break; //Buffer1 to Main page Program
	  case 2 : SPIReadAndWriteCh(0x86); break; //Buffer2 to Main page Program
	  default: break;
   } 
   SPIReadAndWriteCh((uint8_t)(PageAddr >> 6));   
   SPIReadAndWriteCh((uint8_t)(PageAddr << 2));
   SPIReadAndWriteCh(0x00); 
   SPIDeviceSelect(0);
}
/*********************************************************************************************************
** Function name:     	AT45DBWriteBuffer
** Descriptions:	    将buffer寄存器中得数据，与flash中得数据比较，是否一样
** input parameters:    无
** output parameters:   无
** Returned value:      如果一样，返回0，如果不一样，返回1
*********************************************************************************************************/
unsigned char AT45DBCompare(uint8_t nbuf, uint16_t PageAddr) 
{
	AT45DBWaitOK();
	SPIDeviceSelect(1);
	switch(nbuf) 
	{
		case 1 : SPIReadAndWriteCh(0x60); break;
		case 2 : SPIReadAndWriteCh(0x61); break;		
		default: break;
	} 
	SPIReadAndWriteCh((uint8_t)(PageAddr >> 6));   
	SPIReadAndWriteCh((uint8_t)(PageAddr << 2));
	SPIReadAndWriteCh(0xFF); 
	SPIDeviceSelect(0);
	AT45DBWaitOK();
	return(AT45DBReadStatus() & 0x40);
}
/*********************************************************************************************************
** Function name:     	AT45DBReadPage
** Descriptions:	    页读入缓冲区
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char AT45DBReadPage(uint16_t PageAddr, uint8_t *pbuf) 
{
	if(PageAddr < 4096)
	{
		AT45DBReadNByte(PageAddr,0,PAGESIZE,pbuf);
		return 1;
	}
	else
		return 0;
}
/*********************************************************************************************************
** Function name:     	AT45DBWritePage
** Descriptions:	    页写入，先写入第二缓冲区后送入主存储区页范围(0~4095)
** input parameters:    无
** output parameters:   无
** Returned value:      1写入成功，0写入失败
*********************************************************************************************************/
unsigned char AT45DBWritePage(uint16_t PageAddr,uint8_t *pbuf) 
{
	if(PageAddr >= 4096)
		return 0;
	AT45DBWriteNByte(2,PageAddr,0,PAGESIZE,pbuf);//先写入flash中
	AT45DBWriteBuffer(2,0,PAGESIZE,pbuf);//再写入buffer寄存器中
	//将buffer寄存器中得数据，与flash中得数据比较，是否一样
	//如果一样，返回0，如果不一样，返回1
	if(AT45DBCompare(2,PageAddr))
	{	 
		//如果不一样，再重新写一次
		AT45DBWriteNByte(2,PageAddr,0,PAGESIZE,pbuf);
		AT45DBWriteBuffer(2,0,PAGESIZE,pbuf);
		if(AT45DBCompare(2,PageAddr)) 
			return 0;
		else 
			return 1;
	} 
	else 
		return 1;	
}
/*****************************************SPI总线驱动程序************************************************/

/*********************************************************************************************************
** Function name:     	SPIInit
** Descriptions:	    SPI总线初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SPIInit(void) 
{
	PCONP    |=  (1 << 21);			// Enable power to SPI block  
  	FIO0DIR  |=  (1<<21);         	// P0.21 is output 
  	FIO0SET  |=  (1<<21);         	// set P0.21 high (SSEL inactiv)

  	FIO1DIR  |=  (1<<18);         	// P1.18 is output
  	FIO1SET  |=  (1<<18);         	// set P1.18 high (SSEL inactiv)

	PINSEL0 &= ~(3UL<<30);          // P0.15 cleared
	PINSEL0 |=  (2UL<<30);          // P0.15 SCK0
	PINSEL1 &= ~((3<<2) | (3<<4));  // P0.17, P0.18 cleared
	PINSEL1 |=  ((2<<2) | (2<<4));  // P0.17 MISO0, P0.18 MOSI0 

	SSP0CPSR  = 240;           		// 96MHz / 240 = 400kBit ,maximum of 18MHz is possible   
	SSP0CR0	  = 0x00C7;
	SSP0CR1   = 0x0002;  
}
/*********************************************************************************************************
** Function name:     	SPIHightSpeed
** Descriptions:	    设置spi的速度
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SPIHightSpeed(unsigned char on) 
{
	//Set a SPI clock speed to desired value
	if(on == 1) 
		SSP0CPSR =  12; //96MHz / 12   = 8MBit 
	else
		SSP0CPSR = 240; //96MHz / 240 = 400kBit
}
/*********************************************************************************************************
** Function name:     	SPIDeviceSelect
** Descriptions:	    SPI总线设备片选
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SPIDeviceSelect(unsigned char selectchip) 
{
	
	switch(selectchip)
	{
		case 1:	 FIO0CLR  |=  (1<<21);//选中AT45DB161D
				 FIO1SET  |=  (1<<18);
				 break;	
		case 2:	 FIO1CLR  |=  (1<<18);//选中字库芯片
			     FIO0SET  |=  (1<<21);
				 break;
		default: FIO0SET  |=  (1<<21);//禁能全部SPI设备
				 FIO1SET  |=  (1<<18);
				 break;
	}
}
/*********************************************************************************************************
** Function name:     	SPIReadAndWriteCh
** Descriptions:	    总线字节读写
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char SPIReadAndWriteCh(unsigned char Ch) 
{
	SSP0DR = Ch;
	while(SSP0SR & 0x10);	//Wait for transfer to finish
	return(SSP0DR);          	//Return received value
}

/**************************AT45DB161D芯片介绍及实际应用说明**********************************************/
/*
一，AT45DB161D芯片介绍
	1.1 AT45DB161D为SPI接口的Nor型Flash
	1.2 4096页，每页512/528字节，总容量为2M字节
	1.3 可按扇区、快、页擦除
二，实际应用
	2.1 设备参数保存在最后一页即4090-4095页
	    其中主保存页4090-4092页;  镜像保存页4093-4095页,这个是用作出厂配置恢复使用
	2.2 用户交易相关的配置参数保存在4070~4089页之间
	    其中主保存页4070-4079页;  镜像保存页4080-4089页,这个是用作出厂配置恢复使用
	2.3 交易总记录保存在4040~4069页之间
	2.3 货道参数保存在4000~4039页之间  
	    货道主柜			  flash   镜像页号
		HD_CABINET1_FLASH_1    4000  4020
	    HD_CABINET1_FLASH_2    4002  4022
		HD_CABINET1_FLASH_3    4004  4024
		HD_CABINET1_FLASH_4    4006  4026
        货道副柜
		HD_CABINET2_FLASH_1    4008  4028
		HD_CABINET2_FLASH_2    4010  4030
		HD_CABINET2_FLASH_3    4012  4032
		HD_CABINET2_FLASH_4    4014  4034


		
	2.4 单笔交易记录保存在0~3999页之间，循环记录
*/
/**************************************End Of File*******************************************************/

