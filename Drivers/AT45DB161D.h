/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           AT45DB161D.H
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        AT45DB161D SPI Flash驱动程序头文件                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-03-04
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#ifndef __AT45DB161D_H 
#define __AT45DB161D_H

//=================================FLASH 页号规划=======================================================
#define ZHIHUI_BATCH_PAGE	3997	//存储 本批次交易汇总信息 
#define ZHIHUI_TRADE_PAGE   3999    //存储交易信息 包括 目前的索引号


#define HEFAN1_HUODAO_PAGE_TH   3995    //储存盒饭柜货道信息上半部页号
#define HEFAN1_HUODAO_PAGE_LH   3993   //储存盒饭柜货道信息下半部页号

#define HEFAN2_HUODAO_PAGE_TH   3991   
#define HEFAN2_HUODAO_PAGE_LH   3989 

#define HEFAN3_HUODAO_PAGE_TH   3987 
#define HEFAN3_HUODAO_PAGE_LH   3985 

#define HEFAN4_HUODAO_PAGE_TH   3983  
#define HEFAN4_HUODAO_PAGE_LH   3981

#define HEFAN5_HUODAO_PAGE_TH   3979  
#define HEFAN5_HUODAO_PAGE_LH   3977 

#define HEFAN6_HUODAO_PAGE_TH   3975 
#define HEFAN6_HUODAO_PAGE_LH   3973

#define HEFAN7_HUODAO_PAGE_TH   3971
#define HEFAN7_HUODAO_PAGE_LH   3969

#define HEFAN8_HUODAO_PAGE_TH   3967  
#define HEFAN8_HUODAO_PAGE_LH   3965

#define HEFAN9_HUODAO_PAGE_TH   3963   
#define HEFAN9_HUODAO_PAGE_LH   3961 

#define HEFAN10_HUODAO_PAGE_TH   3959  
#define HEFAN10_HUODAO_PAGE_LH   3957

#define PAGE_BIN1_HUODAO_KAOJI		 3955
#define PAGE_BIN2_HUODAO_KAOJI		 3953





void InitAT45DB(void);
unsigned char AT45DBReadStatus(void);
unsigned char AT45DBReadID(unsigned char *id);
void AT45DBReadNByte(uint16_t PageAddr,uint16_t start,uint16_t len,uint8_t *pbuf);
void AT45DBWriteNByte(uint8_t nbuf, uint16_t PageAddr, uint16_t start, uint16_t len, uint8_t *pbuf);
unsigned char AT45DBReadPage(uint16_t PageAddr, uint8_t *pbuf);
unsigned char AT45DBWritePage(uint16_t PageAddr,uint8_t *pbuf);

#endif
/**************************************End Of File*******************************************************/

