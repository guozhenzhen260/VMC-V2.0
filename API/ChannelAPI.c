/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           ChannelAccepterAPI.C
** Last modified Date:  2013-03-05
** Last Version:         
** Descriptions:        货道控制API接口                     
**------------------------------------------------------------------------------------------------------
** Created by:          liya
** Created date:        2013-03-05
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"



//任务通信邮箱的对发邮箱句柄
extern OS_EVENT *g_CmdMsg;
extern OS_EVENT *g_CmdBackMsg;
//任务邮箱数据结构
extern MessagePack MsgPack;



//8*8货道物理地址表 
const unsigned char huodao_addr_8_8[8][10]={
	{57,58,59,60,61,62,63,64, 0, 0}, //  第一层 
	{49,50,51,52,53,54,55,56, 0, 0}, 
	{41,42,43,44,45,46,47,48, 0, 0},
	{33,34,35,36,37,38,39,40, 0, 0},
	{25,26,27,28,29,30,31,32, 0, 0},
	{17,18,19,20,21,22,23,24, 0, 0},
	{ 9,10,11,12,13,14,15,16, 0, 0},
	{ 1, 2, 3, 4, 5, 6, 7, 8, 0, 0}  //  第八层 
};


//8*10货道物理地址表
const unsigned char huodao_addr_8_10[8][10]={
	{71,72,73,74,75,76,77,78,79,80}, //  第一层 
	{61,62,63,64,65,66,67,68,69,70}, 
	{51,52,53,54,55,56,57,58,59,60},
	{41,42,43,44,45,46,47,48,49,50},
	{31,32,33,34,35,36,37,38,39,40},
	{21,22,23,24,25,26,27,28,29,30},
	{11,12,13,14,15,16,17,18,19,20},
	{ 1, 2, 3, 4, 5, 6, 7, 8, 9,10}   //  第八层 
};

//定义存储flash 页号 数组
const unsigned short cabinet1_page[4] = {
		HD_CABINET1_FLASH_1,
		HD_CABINET1_FLASH_2,
		HD_CABINET1_FLASH_3,
		HD_CABINET1_FLASH_4	
};
const unsigned short cabinet2_page[4] = {
		HD_CABINET2_FLASH_1,
		HD_CABINET2_FLASH_2,
		HD_CABINET2_FLASH_3,
		HD_CABINET2_FLASH_4	
};


//定义存储flash 镜像页号 数组
const unsigned short cabinet1_page_bak[4] = {
		HD_CABINET1_FLASH_1_BAK,
		HD_CABINET1_FLASH_2_BAK,
		HD_CABINET1_FLASH_3_BAK,
		HD_CABINET1_FLASH_4_BAK
};
const unsigned short cabinet2_page_bak[4] = {
		HD_CABINET2_FLASH_1_BAK,
		HD_CABINET2_FLASH_2_BAK,
		HD_CABINET2_FLASH_3_BAK,
		HD_CABINET2_FLASH_4_BAK
};






ST_HUODAO_KAOJI stHuodaoKaoji[2];


#pragma arm section zidata = "RAM2"

ST_LEVEL_HUODAO stHuodao[HUODAO_MAX_ROW];
ST_LEVEL_HUODAO stSubHuodao[HUODAO_MAX_ROW];


#pragma arm section zidata






//========================================================================
//第三代货道接口函数 hd -- huodao 所有函数均已hd开头
//by yoc  2014.04.01
//=============================================================================




/*****************************************************************************
** Function name:	hd_init	
** Descriptions:	      重新初始化货道参数														 			
** parameters:		cabinetNo  柜号 1 主柜 2 副柜					
** Returned value:	无
*******************************************************************************/
void hd_init(unsigned char cabinetNo)
{
	unsigned char i,j;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	unsigned char (*huodao_table)[8][10];
	
	if(cabinetNo == 1)
	{
		stHuodaoPtr = stHuodao;
		if(SystemPara.ChannelType == 1)//8 * 10
			huodao_table = (unsigned char (*)[8][10])huodao_addr_8_10;					
		else
			huodao_table = (unsigned char (*)[8][10])huodao_addr_8_8;
	}	
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
	{
		stHuodaoPtr = stSubHuodao;
		if(SystemPara.SubBinChannelType == 1)//8 * 10
			huodao_table = (unsigned char (*)[8][10])huodao_addr_8_10;						
		else
			huodao_table = (unsigned char (*)[8][10])huodao_addr_8_8;	
	}	
	else
	{
		hefanHuodaoInit(cabinetNo);
		return;
	}
	
	//初始化货道
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		stHuodaoPtr[i].openFlag = 1;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			//初始化物理货道编号
			if(j >= 9)
				stHuodaoPtr[i].huodao[j].physicNo = (i + 1) * 10;
			else
				stHuodaoPtr[i].huodao[j].physicNo = (i + 1) * 10 + j + 1;

			stHuodaoPtr[i].huodao[j].logicNo = stHuodaoPtr[i].huodao[j].physicNo;
			//初始化真实货道物理地址
			stHuodaoPtr[i].huodao[j].physicAddr = (*huodao_table)[i][j];
			//根据物理货道地址来决定是否开启此货道
			stHuodaoPtr[i].huodao[j].openFlag = (stHuodaoPtr[i].huodao[j].physicAddr) ? 1 : 0;
			stHuodaoPtr[i].huodao[j].id = 0;
			stHuodaoPtr[i].huodao[j].count = 10;
			stHuodaoPtr[i].huodao[j].maxCount = 10;
			stHuodaoPtr[i].huodao[j].sucCount = 0;
			stHuodaoPtr[i].huodao[j].state = 1;
			stHuodaoPtr[i].huodao[j].selectNo = 0;
			stHuodaoPtr[i].huodao[j].price = 100;
			
		}
	}
	
	print_hd("cabinet[%d]:InitOver..\r\n",cabinetNo);
	
}


/*****************************************************************************
** Function name:	hd_logic_init	
** Descriptions:	      重新初始化货道逻辑参数													 			
** parameters:		cabinetNo  柜号 1 主柜 2 副柜	
** Returned value:	1 成功 0 失败
*******************************************************************************/
unsigned char  hd_logic_init(unsigned char cabinetNo)
{
	unsigned char i,j,r = 0,c = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0,c = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(stHuodaoPtr[i].huodao[j].openFlag && stHuodaoPtr[i].openFlag && 
				stHuodaoPtr[i].huodao[j].physicAddr)
			{
				if(c >= 9)
					stHuodaoPtr[i].huodao[j].logicNo = (r + 1) * 10;	
				else
					stHuodaoPtr[i].huodao[j].logicNo = (r + 1) * 10 + 1 + c;
				c++;
			}
			else
				stHuodaoPtr[i].huodao[j].logicNo = 0;	
		}
		
		if(stHuodaoPtr[i].openFlag && c)
			r++;
		
	}

	return 1;
	
}


/*****************************************************************************
** Function name:hd_save_param_bak	
** Descriptions:	将货道数据数据保存到FLASH镜像中	
**				将整个货道结构体长度分成4份 分4页存储
**Input parameters:  cabinetNo 柜号
** Returned value:	1 成功  0 失败
*******************************************************************************/
unsigned char hd_save_param_bak(unsigned char cabinetNo)
{
	//分4页存储货道数据前三页存储长度为len_th,最后一页长度为len_lh
	unsigned short crc,len_th,len_lh;
	unsigned char buf[512]={0},*hdPtr,i;
	unsigned short *pageNo;
	print_hd("SaveChannel_Start..\r\n");

	if(cabinetNo == 1)
	{
		hdPtr = (unsigned char *)stHuodao;
		pageNo = (unsigned short *)cabinet1_page_bak;
	}
		
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
	{
		hdPtr = (unsigned char *)stSubHuodao;
		pageNo = (unsigned short *)cabinet2_page_bak;
	}	
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	len_th = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) / 4;
	len_lh = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) - len_th * 3;

	//存储前三页
	for(i = 0;i < 3;i++)
	{
		memset(buf,0,512);
		memcpy(buf,&hdPtr[len_th * i],len_th);
		crc = CrcCheck(buf,len_th);
		buf[len_th] = HUINT16(crc);
		buf[len_th + 1] = LUINT16(crc);	
		AT45DBWritePage(pageNo[i],buf);
	}	
	//存储第四页
	memset(buf,0,512);
	memcpy(buf,&hdPtr[len_th * 3],len_lh);
	crc = CrcCheck(buf,len_lh);
	buf[len_lh] = HUINT16(crc);
	buf[len_lh + 1] = LUINT16(crc);	
	AT45DBWritePage(pageNo[3],buf);

	print_hd("SaveChannel_OK..\r\n");	
	return 1;
	
	
}





/*****************************************************************************
** Function name:hd_save_param	
** Descriptions:	将货道数据数据保存到FLASH中	
**				将整个货道结构体长度分成4份 分4页存储
**Input parameters:  cabinetNo 柜号
** Returned value:	1 成功  0 失败
*******************************************************************************/
unsigned char hd_save_param(unsigned char cabinetNo)
{
	//分4页存储货道数据前三页存储长度为len_th,最后一页长度为len_lh
	unsigned short crc,len_th,len_lh;
	unsigned char buf[512]={0},*hdPtr,i;
	unsigned short *pageNo;
	print_hd("SaveChannel_Start..\r\n");

	if(cabinetNo == 1)
	{
		hdPtr = (unsigned char *)stHuodao;
		pageNo = (unsigned short *)cabinet1_page;
	}
		
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
	{
		hdPtr = (unsigned char *)stSubHuodao;
		pageNo = (unsigned short *)cabinet2_page;
	}	
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	len_th = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) / 4;
	len_lh = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) - len_th * 3;

	//存储前三页
	for(i = 0;i < 3;i++)
	{
		memset(buf,0,512);
		memcpy(buf,&hdPtr[len_th * i],len_th);
		crc = CrcCheck(buf,len_th);
		buf[len_th] = HUINT16(crc);
		buf[len_th + 1] = LUINT16(crc);	
		AT45DBWritePage(pageNo[i],buf);
	}	
	//存储第四页
	memset(buf,0,512);
	memcpy(buf,&hdPtr[len_th * 3],len_lh);
	crc = CrcCheck(buf,len_lh);
	buf[len_lh] = HUINT16(crc);
	buf[len_lh + 1] = LUINT16(crc);	
	AT45DBWritePage(pageNo[3],buf);

	print_hd("SaveChannel_OK..\r\n");	
	return 1;
	
	
}


/*****************************************************************************
** Function name:	hd_read_param	
** Descriptions:	从flash中读取货道参数															 			
** parameters:		无					
** Returned value:	无
** 
*******************************************************************************/
unsigned char hd_read_param(unsigned char cabinetNo)
{

	unsigned short crc,len_th,len_lh;
	unsigned char buf[512]={0},*hdPtr,i,err;
	unsigned short *pageNo;
	print_hd("SaveChannel_Start..\r\n");

	if(cabinetNo == 1)
	{
		hdPtr = (unsigned char *)stHuodao;
		pageNo = (unsigned short *)cabinet1_page;
	}
		
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
	{
		hdPtr = (unsigned char *)stSubHuodao;
		pageNo = (unsigned short *)cabinet2_page;
	}	
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	len_th = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) / 4;
	len_lh = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) - len_th * 3;


	//读取前三页
	for(i = 0;i < 3;i++)
	{
		memset(buf,0,512);
		err = AT45DBReadPage(pageNo[i],buf);
		if(err)
		{
			crc = CrcCheck(buf,len_th);
			if(crc == INTEG16(buf[len_th],buf[len_th + 1]))
			{
				memcpy(&hdPtr[len_th * i],buf,len_th);	//成功读取一页
				continue;
			}
		}
		//读取失败
		print_hd("read huodao faied! the pageNo is %d\r\n",pageNo[i]);
		return 0;		
	}	
	
	//读取第四页
	memset(buf,0,512);
	err = AT45DBReadPage(pageNo[3],buf);
	if(err)
	{
		crc = CrcCheck(buf,len_lh);
		if(crc == INTEG16(buf[len_lh],buf[len_lh + 1]))
		{
			memcpy(&hdPtr[len_th * 3],buf,len_lh);	//成功读取一页
			return 1;
		}
	}
	
	//读取失败
	print_hd("read huodao faied! the pageNo is %d\r\n",pageNo[3]);
	return 0;	
	
}



/*****************************************************************************
** Function name:	hd_read_param_bak	
** Descriptions:	从flash镜像中读取货道参数															 			
** parameters:		无					
** Returned value:	无
** 
*******************************************************************************/
unsigned char hd_read_param_bak(unsigned char cabinetNo)
{

	unsigned short crc,len_th,len_lh;
	unsigned char buf[512]={0},*hdPtr,i,err;
	unsigned short *pageNo;
	print_hd("SaveChannel_Start..\r\n");

	if(cabinetNo == 1)
	{
		hdPtr = (unsigned char *)stHuodao;
		pageNo = (unsigned short *)cabinet1_page_bak;
	}
		
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
	{
		hdPtr = (unsigned char *)stSubHuodao;
		pageNo = (unsigned short *)cabinet2_page_bak;
	}	
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	len_th = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) / 4;
	len_lh = (sizeof(ST_LEVEL_HUODAO) * HUODAO_MAX_ROW) - len_th * 3;


	//读取前三页
	for(i = 0;i < 3;i++)
	{
		memset(buf,0,512);
		err = AT45DBReadPage(pageNo[i],buf);
		if(err)
		{
			crc = CrcCheck(buf,len_th);
			if(crc == INTEG16(buf[len_th],buf[len_th + 1]))
			{
				memcpy(&hdPtr[len_th * i],buf,len_th);	//成功读取一页
				continue;
			}
		}
		//读取失败
		print_hd("read huodao faied! the pageNo is %d\r\n",pageNo[i]);
		return 0;		
	}	
	
	//读取第四页
	memset(buf,0,512);
	err = AT45DBReadPage(pageNo[3],buf);
	if(err)
	{
		crc = CrcCheck(buf,len_lh);
		if(crc == INTEG16(buf[len_lh],buf[len_lh + 1]))
		{
			memcpy(&hdPtr[len_th * 3],buf,len_lh);	//成功读取一页
			return 1;
		}
	}
	
	//读取失败
	print_hd("read huodao faied! the pageNo is %d\r\n",pageNo[3]);
	return 0;	
	
}



/*****************************************************************************
** Function name:	hd_get_index	
** Descriptions:		获取货道结构体索引 号														 			
** parameters:		type 1 通过逻辑货道号获取索引号
						2 通过物理货道号获取索引号
						3 选货按键号
						4 ID号
						5 物理地址号获取索引号
					rIndex 层索引号
					cIndex 货道索引号
					no  逻辑货道号/物理货道号/按键号/ID号
** Returned value:	1 成功  0 失败
** 
*******************************************************************************/
unsigned char hd_get_index(unsigned char cabinetNo,unsigned char no,unsigned char type,
		unsigned char *rIndex,unsigned char *cIndex)
{
	unsigned char i,j,temp;
	ST_LEVEL_HUODAO *stHuodaoPtr;

	*rIndex = 0;
	*cIndex = 0;
	
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}

	if(!no)
	{
		print_err(no);
		return 0;
	}

	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(type == 1)
				temp = stHuodaoPtr[i].huodao[j].logicNo;
			else if(type == 2)
				temp = stHuodaoPtr[i].huodao[j].physicNo;
			else if(type == 3)
				temp = stHuodaoPtr[i].huodao[j].selectNo;
			else if(type == 4)
				temp = stHuodaoPtr[i].huodao[j].id;
			else if(type == 5)
				temp = stHuodaoPtr[i].huodao[j].physicAddr;
			else
				return 0;

			if(temp == no)
			{
				*rIndex = i;
				*cIndex = j;
				return 1;
			}
				
		}
	}

	return 0;
	
}



/*****************************************************************************
** Function name:	hd_open_handle	
** Descriptions:	      货道开关函数
**                            层操作后将重新对整个货道进行逻辑排号；
					单货道操作后,重新对该层的货道进行逻辑排号
** parameters:		cabinetNo：箱柜编号
					no:     物理货道编号/层号 					
					type：1——某货道操作；2——某层操作
					flag:	1——开启，0——关闭														
** Returned value:	1 成功  0失败
*******************************************************************************/
unsigned char hd_open_handle(unsigned char cabinetNo,unsigned char no,unsigned char type,unsigned char flag)
{
	unsigned char i,j,err,temp;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	
	print_hd("no=%d type=%d flag=%d Bin=%d\r\n",no,type,flag,cabinetNo);
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}

	if(type == 2)//层操作
	{
		if(no < 1 || no > 8)
			return 0;	
		temp = (flag == 1) ? 1 : 0;
		if(stHuodaoPtr[no - 1].openFlag == temp)//此次操作作废即层开启又发来让其开启什么多不做直接返回1
			return 1;
		stHuodaoPtr[no - 1].openFlag = temp;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)//该层的所有货道执行相应的开关操作
		{
			if(!stHuodaoPtr[no - 1].huodao[j].physicAddr)
				stHuodaoPtr[no - 1].huodao[j].openFlag = 0;
			else
				stHuodaoPtr[no - 1].huodao[j].openFlag = temp;
		}
		hd_logic_init(cabinetNo);//重新排列逻辑号
		return 1;			
	}
	else//莫货道操作
	{
		err = hd_get_index(cabinetNo,no,2,&i,&j);
		if(err)
		{
			if(!stHuodaoPtr[i].openFlag )//层关闭了不允许操作货道开关
				return 0;
			if(!stHuodaoPtr[i].huodao[j].physicAddr)//物理地址不存在不允许操作
			{
				stHuodaoPtr[i].huodao[j].openFlag = 0;
				return 0;
			}
			temp = (flag == 1) ? 1 : 0;
			if(stHuodaoPtr[i].huodao[j].openFlag == temp)
				return 1;
			
			stHuodaoPtr[i].huodao[j].openFlag = temp;
			hd_logic_init(cabinetNo);//重新排列逻辑号
			return 1;	
			
			
		}
	}
	
	return 0;
	
}

/*****************************************************************************
** Function name:	hd_logicNo_by_selectNo	
** Descriptions:	      通过选货按键号获得货道逻辑号												 			
** parameters:		cabinetNo:箱柜编号		
					selectNo :选货按键号
** Returned value:	0:失败;  非0  返回货道逻辑号
*******************************************************************************/
unsigned char hd_logicNo_by_selectNo(unsigned char cabinetNo,unsigned char selectNo)
{
	unsigned char i,j,err;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	err = hd_get_index(cabinetNo,selectNo,3,&i,&j);
	if(err)
	{
		return stHuodaoPtr[i].huodao[j].logicNo;
	}
	return 0;

}


/*****************************************************************************
** Function name:	hd_logicNo_by_id	
** Descriptions:	      通过ID号获得货道逻辑号												 			
** parameters:		cabinetNo:箱柜编号		
					selectNo :选货按键号
** Returned value:	0:无此货到;  非0  返回货道逻辑号  0xFF 表示缺货
*******************************************************************************/
unsigned char hd_logicNo_by_id(unsigned char cabinetNo,unsigned char id)
{
	unsigned char i,j,count = 0,bestLogicNo = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;

	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			
			if(stHuodaoPtr[i].huodao[j].id != id)
			  	continue;
			
			if((stHuodaoPtr[i].huodao[j].count > count) && 
				(stHuodaoPtr[i].huodao[j].state == HUODAO_STATE_NORMAL))
			{		
				
				count =  stHuodaoPtr[i].huodao[j].count;
				bestLogicNo = stHuodaoPtr[i].huodao[j].logicNo;
			}
		}
	}
	
	return bestLogicNo ;

}

/*****************************************************************************
** Function name:	hd_logicNo_by_physic	
** Descriptions:	      通过物理货道号获得货道逻辑号												 			
** parameters:		cabinetNo:箱柜编号		
					physicNo :
** Returned value:	0:失败;  非0  返回货道逻辑号
*******************************************************************************/
unsigned char hd_logicNo_by_physic(unsigned char cabinetNo,unsigned char physicNo)
{
	unsigned char i,j,err;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	err = hd_get_index(cabinetNo,physicNo,2,&i,&j);
	if(err)
	{
		return stHuodaoPtr[i].huodao[j].logicNo;
	}
	return 0;

}


/*****************************************************************************
** Function name:	hd_physicNo_by_logicNo	
** Descriptions:	      通过逻辑货道号获得物理货道号											 			
** parameters:		cabinetNo:箱柜编号		
					selectNo :选货按键号
** Returned value:	0:失败;  非0  返回货道物理货道号
*******************************************************************************/
unsigned char hd_physicNo_by_logicNo(unsigned char cabinetNo,unsigned char logicNo)
{
	unsigned char i,j,err;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	err = hd_get_index(cabinetNo,logicNo,1,&i,&j);
	if(err)
	{
		return stHuodaoPtr[i].huodao[j].physicNo;
	}
	return 0;

}



/*****************************************************************************
** Function name:	hd_set_by_logic	
** Descriptions:	      通过逻辑货道号设置货道信息											 			
** parameters:		cabinetNo:箱柜编号		
					logicNo :逻辑货道号
** Returned value:	0:失败; 1成功
*******************************************************************************/
unsigned char hd_set_by_logic(unsigned char cabinetNo,unsigned char logicNo,unsigned char type,unsigned int value)
{
	unsigned char  i,j,err,flag = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	
	err = hd_get_index(cabinetNo,logicNo,1,&i,&j);
	if(err)
	{
		switch(type)
		{
			case HUODAO_TYPE_PRICE:
				stHuodaoPtr[i].huodao[j].price = value;
				flag = 1;
				break;
			case HUODAO_TYPE_COUNT:
				stHuodaoPtr[i].huodao[j].count= value;
				flag = 1;
				break;
			case HUODAO_TYPE_MAX_COUNT:
				stHuodaoPtr[i].huodao[j].maxCount= value;
				flag = 1;
				break;
			case HUODAO_TYPE_STATE:
				stHuodaoPtr[i].huodao[j].state= value;
				flag = 1;
				break;
			case HUODAO_TYPE_SELECTNO:
				stHuodaoPtr[i].huodao[j].selectNo= value;
				flag = 1;
				break;
			case HUODAO_TYPE_ID:
				stHuodaoPtr[i].huodao[j].id= value;
				flag = 1;
				break;
			case HUODAO_TYPE_SUC_COUNT:
				stHuodaoPtr[i].huodao[j].sucCount = value;
				flag = 1;
				break;
			case HUODAO_TYPE_OPEN:
				if(stHuodaoPtr[i].openFlag)
				{
					stHuodaoPtr[i].huodao[j].openFlag  = value;
					flag = 1;
				}					
				break;
			default:
				break;
		}
		
	}

	return flag;

}


/*****************************************************************************
** Function name:	hd_get_by_logic	
** Descriptions:	通过逻辑货道号读取货道参数值													 			
** parameters:		logicNo:逻辑地址
					type：1——货道单价
						  2——货道数量
						  3——货道状态l
						  4——货道最大存货量
						  5——选货按键值
						  6——成功交易次数
						  7——商品编号					
** Returned value:	0：无，其他相应值
*******************************************************************************/
unsigned int hd_get_by_logic(unsigned char cabinetNo,unsigned char logicNo,unsigned char type)
{
	unsigned char  i,j,err;
	unsigned int value = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
		return hefanGetParaVlaue(get_hefan_Index(cabinetNo),logicNo,type);
	
	err = hd_get_index(cabinetNo,logicNo,1,&i,&j);
	if(err)
	{
		switch(type)
		{
			case HUODAO_TYPE_PRICE:
				value = stHuodaoPtr[i].huodao[j].price;
				break;
			case HUODAO_TYPE_COUNT:
				value = stHuodaoPtr[i].huodao[j].count;
				break;
			case HUODAO_TYPE_MAX_COUNT:
				value = stHuodaoPtr[i].huodao[j].maxCount;
				break;
			case HUODAO_TYPE_STATE:
				value = stHuodaoPtr[i].huodao[j].state;
				break;
			case HUODAO_TYPE_SELECTNO:
				value = stHuodaoPtr[i].huodao[j].selectNo;
				break;
			case HUODAO_TYPE_ID:
				value = stHuodaoPtr[i].huodao[j].id;
				break;
			case HUODAO_TYPE_SUC_COUNT:
				value = stHuodaoPtr[i].huodao[j].sucCount;
				break;
			case HUODAO_TYPE_OPEN:
				value = (stHuodaoPtr[i].openFlag) ? (stHuodaoPtr[i].huodao[j].openFlag) : 0;
				break;
			default:
				break;
		}
		
	}
	return value;
	
}

/*****************************************************************************
** Function name:	hd_set_by_physic	
** Descriptions:	      通过物理货道号设置货道信息											 			
** parameters:		cabinetNo:箱柜编号		
					physicNo :物理货道号
** Returned value:	0:失败; 1成功
*******************************************************************************/
unsigned char hd_set_by_physic(unsigned char cabinetNo,unsigned char physicNo,unsigned char type,unsigned int value)
{
	unsigned char  i,j,err,flag = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return 0;
	}
	err = hd_get_index(cabinetNo,physicNo,2,&i,&j);
	if(err)
	{
		switch(type)
		{
			case HUODAO_TYPE_PRICE:
				stHuodaoPtr[i].huodao[j].price = value;
				flag = 1;
				break;
			case HUODAO_TYPE_COUNT:
				stHuodaoPtr[i].huodao[j].count= value;
				flag = 1;
				break;
			case HUODAO_TYPE_MAX_COUNT:
				stHuodaoPtr[i].huodao[j].maxCount= value;
				flag = 1;
				break;
			case HUODAO_TYPE_STATE:
				stHuodaoPtr[i].huodao[j].state= value;
				flag = 1;
				break;
			case HUODAO_TYPE_SELECTNO:
				stHuodaoPtr[i].huodao[j].selectNo= value;
				flag = 1;
				break;
			case HUODAO_TYPE_ID:
				stHuodaoPtr[i].huodao[j].id= value;
				flag = 1;
				break;
			case HUODAO_TYPE_SUC_COUNT:
				stHuodaoPtr[i].huodao[j].sucCount = value;
				flag = 1;
				break;
			case HUODAO_TYPE_OPEN:
				if(stHuodaoPtr[i].openFlag)
				{
					stHuodaoPtr[i].huodao[j].openFlag  = value;
					flag = 1;
				}					
				break;
			default:
				break;
		}
		
	}

	return flag;

}


/*****************************************************************************
** Function name:	hd_get_by_physic	
** Descriptions:	通过物理货道号读取货道参数值													 			
** parameters:		physicNo:物理货道号
					type：1——货道单价
						  2——货道数量
						  3——货道状态
						  4——货道最大存货量
						  5——选货按键值
						  6——成功交易次数
						  7——商品编号					
** Returned value:	0：无，其他相应值
*******************************************************************************/
unsigned int hd_get_by_physic(unsigned char cabinetNo,unsigned char physicNo,unsigned char type)
{
	unsigned char  i,j,err;
	unsigned int value = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
		return hefanGetParaVlaue(get_hefan_Index(cabinetNo),physicNo,type);
	
	err = hd_get_index(cabinetNo,physicNo,2,&i,&j);
	if(err)
	{
		switch(type)
		{
			case HUODAO_TYPE_PRICE:
				value = stHuodaoPtr[i].huodao[j].price;
				break;
			case HUODAO_TYPE_COUNT:
				value = stHuodaoPtr[i].huodao[j].count;
				break;
			case HUODAO_TYPE_MAX_COUNT:
				value = stHuodaoPtr[i].huodao[j].maxCount;
				break;
			case HUODAO_TYPE_STATE:
				value = stHuodaoPtr[i].huodao[j].state;
				break;
			case HUODAO_TYPE_SELECTNO:
				value = stHuodaoPtr[i].huodao[j].selectNo;
				break;
			case HUODAO_TYPE_ID:
				value = stHuodaoPtr[i].huodao[j].id;
				break;
			case HUODAO_TYPE_SUC_COUNT:
				value = stHuodaoPtr[i].huodao[j].sucCount;
				break;
			case HUODAO_TYPE_OPEN:
				value = stHuodaoPtr[i].huodao[j].openFlag;
				break;
			default:
				break;
		}
		
	}
	return value;
	
}



/*****************************************************************************
** Function name:	hd_check_ok	
** Descriptions:	      检测货道是否可服务														 			
** parameters:		logicNo:逻辑地址					
** Returned value:	0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用
*******************************************************************************/
unsigned char hd_check_ok(unsigned char cabinetNo,unsigned char logicNo)
{
	unsigned char  i,j,err;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
		return hefanHuodaoCheck(cabinetNo,logicNo);
	
	err = hd_get_index(cabinetNo,logicNo,1,&i,&j);
	if(!err)
		return 5;
	
	if((SystemPara.PcEnable)&&(stHuodaoPtr[i].huodao[j].id==0))
		return 6;
	if(stHuodaoPtr[i].huodao[j].price == 0)
		return 1;
	if(stHuodaoPtr[i].huodao[j].count== 0)
		return 4;
	if(stHuodaoPtr[i].huodao[j].state == 2)
		return 3;
	if(stHuodaoPtr[i].huodao[j].state== 3)
		return 4;
	if(stHuodaoPtr[i].huodao[j].state == 4)
		return 7;

	return 0;
		
}




/*****************************************************************************
** Function name:	hd_add_goods	
** Descriptions:	添加货道存货余量													 			
** parameters:		cabinetNo：1--主柜 2--副柜
					type：操作类型	
						1：单货道添货
						2：层添货逻辑层
						3：全部货道添货			
** Returned value:	0：失败；1：成功
*******************************************************************************/
unsigned char hd_add_goods(unsigned char cabinetNo,unsigned char no,unsigned type)
{
	unsigned char  i,j,err;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}

	if(type == 1)//单货道
	{
		err = hd_get_index(cabinetNo,no,1,&i,&j);
		if(err)
		{
			if(stHuodaoPtr[i].huodao[j].state ==  HUODAO_STATE_EMPTY
				|| stHuodaoPtr[i].huodao[j].state ==  HUODAO_STATE_NORMAL)
			{
				stHuodaoPtr[i].huodao[j].state =  HUODAO_STATE_NORMAL;
				stHuodaoPtr[i].huodao[j].count = stHuodaoPtr[i].huodao[j].maxCount;
				return 1;	
			}
		}
	}
	else if(type == 2)//层货道
	{
		if(no < 1 || no > HUODAO_MAX_ROW)
			return 0;
		if(!stHuodaoPtr[no - 1].openFlag)
			return 0;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(stHuodaoPtr[no - 1].huodao[j].state ==  HUODAO_STATE_EMPTY
				|| stHuodaoPtr[no - 1].huodao[j].state ==  HUODAO_STATE_NORMAL)
			{
				stHuodaoPtr[no - 1].huodao[j].state =  HUODAO_STATE_NORMAL;
				stHuodaoPtr[no - 1].huodao[j].count = stHuodaoPtr[no - 1].huodao[j].maxCount;
			}
		}
		return 1;
	}
	else if(type == 3)
	{
		for(i = 0;i < HUODAO_MAX_ROW;i++)
		{
			if(!stHuodaoPtr[i].openFlag)
				continue;
			for(j = 0;j < HUODAO_MAX_COLUMN;j++)
			{
				if(stHuodaoPtr[i].huodao[j].state ==  HUODAO_STATE_EMPTY
					|| stHuodaoPtr[i].huodao[j].state ==  HUODAO_STATE_NORMAL)
				{
					stHuodaoPtr[i].huodao[j].state =  HUODAO_STATE_NORMAL;
					stHuodaoPtr[i].huodao[j].count = stHuodaoPtr[i].huodao[j].maxCount;
				}
			}
		}

		return 1;
	}
	
	return 0;
}

/*****************************************************************************
** Function name:	hd_all_check	
** Descriptions:	      检测所有货道 故障													 			
** parameters:							
** Returned value:	0：正常；1全货道故障
*******************************************************************************/
unsigned char hd_all_check(void)
{
	unsigned char i,j;
	
	//先查看柜1
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		if(!stHuodao[i].openFlag)
			continue;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
				if(!stHuodao[i].huodao[j].openFlag)
					continue;
				if(stHuodao[i].huodao[j].count && stHuodao[i].huodao[j].price
					&&  stHuodao[i].huodao[j].state == HUODAO_STATE_NORMAL)
				{
					print_hd("The first ok hd is[%d][%d]\r\n",i,j);
					return 0;
				}
					
		}
	}

	//到此 表示柜1 全货道故障 若开启柜2 则查看柜2
	if(SystemPara.SubBinOpen)
	{
		for(i = 0;i < HUODAO_MAX_ROW;i++)
		{
			if(!stSubHuodao[i].openFlag)
				continue;
			for(j = 0;j < HUODAO_MAX_COLUMN;j++)
			{
				if(!stSubHuodao[i].huodao[j].openFlag)
					continue;
				if(stSubHuodao[i].huodao[j].count && stSubHuodao[i].huodao[j].price
					&&  stSubHuodao[i].huodao[j].state == HUODAO_STATE_NORMAL)
				{
					print_hd("The sub first ok hd is[%d][%d]\r\n",i,j);
					return 0;
				}
					
			}
		}
	}
	print_hd("hd all empty!\r\n");
	return 1;

			
}


/*****************************************************************************
** Function name:	hd_set_by_pc	
** Descriptions:	     PC设置货道信息
** parameters:		cabinetNo :柜号  1 设置ID ;  2:设置余量 ; 3 : 设置单价by ID  4:设置单价 by 货道 		
** Returned value:	1成功 0失败
*******************************************************************************/
unsigned char hd_set_by_pc(unsigned char cabinetNo,unsigned char len,void * data,unsigned char type)
{	
	unsigned char  i,j,err = 0,r,c,logicNo,index = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	struct GoodsMatrix (*stGoods)[];
	unsigned char *buf,value;
	unsigned int price;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}
	
	if((len != HUODAO_MAX_ROW * HUODAO_MAX_COLUMN) && (type != 3))
	{
		return 0;
	}
	
	if(type == 3) //设置货道 by ID
	{
		stGoods = (struct GoodsMatrix (*)[])data;
		for(i = 0;i < len;i++)
		{
			print_hd("id[%d] = %d,%d\r\n",i,(*stGoods)[i].GoodsType,(*stGoods)[i].Price);
			for(r = 0;r < HUODAO_MAX_ROW;r++)
			{
				for(c = 0;c < HUODAO_MAX_COLUMN;c++)
				{
					if(stHuodaoPtr[r].huodao[c].id == (*stGoods)[i].GoodsType && stHuodaoPtr[r].huodao[c].openFlag)
					{
						stHuodaoPtr[r].huodao[c].price = (*stGoods)[i].Price;
						print_hd("r= %d c= %d\r\n",r,c);
					}
				}
			}
		}
		return 1;
	}
	else if(type == 4)//设置货道 by 货道
	{
		stGoods = (struct GoodsMatrix (*)[])data;
		for(i = 0;i < HUODAO_MAX_ROW;i++)
		{
			for(j = 0;j < HUODAO_MAX_COLUMN;j++)
			{
				if(j >= 9 )
					logicNo = (i + 1) * 10;
				else
					logicNo = (i + 1) * 10 + (j + 1);
				price = (*stGoods)[index++].Price;
				if(hd_get_index(cabinetNo,logicNo,1,&r,&c))//货道存在
				{
					stHuodaoPtr[r].huodao[c].price = price;
				}		
			}
		}
	}
	else
	{
		//设置 ID  和余量
		buf = (unsigned char *)data;
		
		for(i = 0;i < HUODAO_MAX_ROW;i++)
		{
			for(j = 0;j < HUODAO_MAX_COLUMN;j++)
			{
				if(j >= 9 )
					logicNo = (i + 1) * 10;
				else
					logicNo = (i + 1) * 10 + (j + 1);
				
				value = buf[index++];
				//print_hd("type =%d value = %d\r\n",type,value);
				if(hd_get_index(cabinetNo,logicNo,1,&r,&c))//货道存在
				{
					if(type == 1)//更改ID 上位机固定发送80个字节数据
					{
						//上位机软关闭
						if(value == 0x00 || value == 0xFF)
						{
							stHuodaoPtr[r].huodao[c].id = 0;
							stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_PC_CLOSE;			
						}
						else//故障货道允许设置ID
						{
							stHuodaoPtr[r].huodao[c].id = value;
							if(stHuodaoPtr[r].huodao[c].state != HUODAO_STATE_FAULT && 
								stHuodaoPtr[r].huodao[c].state != HUODAO_STATE_NOT)
							{
								if(stHuodaoPtr[r].huodao[c].count)
									stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_NORMAL;
								else
									stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_EMPTY;
							}
						
						}
					}
					else if(type == 2)
					{	
						if((stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_EMPTY)||
							(stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_NORMAL))
						{
							stHuodaoPtr[r].huodao[c].count = (value & 0x3F);
							if(stHuodaoPtr[r].huodao[c].count)
								stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_NORMAL;
							else
								stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_EMPTY;
						}
					}
				}		
			}
		}
	}


	


	return err;
	
}



/*****************************************************************************
** Function name:	hd_get_next_physicNo	
** Descriptions:	      获取箱柜中该物理货道号的下一个存在的物理货道号										 			
** parameters:		cabinetNo :	
** Returned value:	1成功 0失败
*******************************************************************************/
unsigned char hd_get_next_physicNo(unsigned char cabinetNo,unsigned char physicNo)
{
#if 0
	unsigned char  row,col,i,j;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}

	row = physicNo / 10;//层号 1 - 8
	col = (physicNo % 10) ? (physicNo % 10) : 10;//列号 1- 10
	//如果是最后一层的最后一个货道则返回第一层第一个货道

	
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(stHuodaoPtr[i].huodao[j].physicAddr)
				  ;
		}
	}

#endif
	return 0;	
}


/*****************************************************************************
** Function name:	ChannelSendMBox	
**
** Descriptions:	给设备管理任务发送邮箱	
**					
**														 			
** parameters:		HandleType:操作类型
					ChkFlag:是否开启货到检测
					Binnum:箱柜编号
** Returned value:	0:成功;1:失败;
** 
*****************************************************************************/
unsigned char ChannelSendMBox(unsigned char HandleType,unsigned char ChkFlag,unsigned char ChannelNum,unsigned char Binnum)
{
	MessagePack *RecvBoxPack;
	uint8_t err;

	switch(HandleType)
	{
		case CHANNEL_OUTGOODS:
			MsgAccepterPack.ChnlHandleType = CHANNEL_OUTGOODS;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			//等待邮箱返回
			RecvBoxPack = OSMboxPend(g_ChannelBackMail,8000,&err);//8000 * 5 = 40s 
			if(err == OS_NO_ERR)
			{
				return RecvBoxPack->HandleResult;
			}
			break;
		case CHANNEL_CHECKSTATE:
			MsgAccepterPack.ChnlHandleType = CHANNEL_CHECKSTATE;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			//等待邮箱返回
			RecvBoxPack = OSMboxPend(g_ChannelBackMail,1000,&err);
			if(err == OS_NO_ERR)
			{
				return RecvBoxPack->HandleResult;
			}
			break;
		case CHANNEL_CHECKOUTRESULT:
			MsgAccepterPack.ChnlHandleType = CHANNEL_CHECKOUTRESULT;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			//等待邮箱返回
			RecvBoxPack = OSMboxPend(g_ChannelBackMail,1000,&err);
			if(err == OS_NO_ERR)
			{
				return RecvBoxPack->HandleResult;
			}
			break;
		default:
			break;
	}
	return 0xff;	
}

unsigned char TestColumnPost(unsigned char HandleType,unsigned char ChkFlag,unsigned char ChannelNum,unsigned char Binnum)
{
	switch(HandleType)
	{
		case CHANNEL_OUTGOODS:
			MsgAccepterPack.ChnlHandleType = CHANNEL_OUTGOODS;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			break;
		case CHANNEL_CHECKSTATE:
			MsgAccepterPack.ChnlHandleType = CHANNEL_CHECKSTATE;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			break;
		case CHANNEL_CHECKOUTRESULT:
			MsgAccepterPack.ChnlHandleType = CHANNEL_CHECKOUTRESULT;
			MsgAccepterPack.ChannelNum = ChannelNum;
			MsgAccepterPack.GOCFlag = ChkFlag;
			MsgAccepterPack.Binnum = Binnum;
			//发送邮箱给DevieTask让其操作Hopper做出币操作
			OSMboxPost(g_ChannelMail,&MsgAccepterPack);
			break;
		default:
			break;
	}
	
	return 0xff;
}



unsigned char TestColumnPend(unsigned char ChkFlag)
{
	MessagePack *RecvBoxPack;
	uint8_t err;

	//等待邮箱返回
	RecvBoxPack = OSMboxPend(g_ChannelBackMail,5,&err);
	if(err == OS_NO_ERR)
	{
		if(RecvBoxPack->HandleResult == 0x00)
		{
			return 0;	
		}
		else
		if(RecvBoxPack->HandleResult == 0xff)
		{
			return 5;
		}
		else
		if((ChkFlag==1)&&(RecvBoxPack->HandleResult&0x80))
			return 0;
		else
		if(RecvBoxPack->HandleResult&0x01)
		{
			//电机未转动
			if((ChkFlag==1)&&(RecvBoxPack->HandleResult&0x08))
				return 1;
			//出货确认开启。出货确认未检测到商品出货
			if((ChkFlag==1)&&(RecvBoxPack->HandleResult&0x20))
				return 4;	
			else
			//出货确认开启，电机未转到位，但出货确认检测到商品出货
			if((ChkFlag==1)&&(RecvBoxPack->HandleResult==0x11))
				return 0;
			else
			//出货确认未开启，电机未转到位
			if((ChkFlag==0)&&(RecvBoxPack->HandleResult&0x10))
				return 3;
			else
			//出货确认未开启，电机不能转动
			if((ChkFlag==0)&&(RecvBoxPack->HandleResult&0x08))
				return 1;
		}
		return 1;
	}
	else
		return 0xFF;
}



/*****************************************************************************
** Function name:	ChannelHandleProcess	
**
** Descriptions:	货道操作管理函数	
**					
**														 			
** parameters:		HandleType:操作类型
					ChkFlag:是否开启货到检测
 GOODS_SOLD_ERR          (1 << 0)   //bit0总故障位
 GOODS_SOLDOUT_BIT       (1 << 1)   //bit1电机故障
 MOTO_MISPLACE_BIT       (1 << 2)   //bit2电机在转之前就不在正确的位置上(也算电机故障)
 MOTO_NOTMOVE_BIT        (1 << 3)   //bit3电机不能转(也算电机故障)
 MOTO_NOTRETURN_BIT      (1 << 4)   //bit4电机没转到正确位置(也算电机故障)
 GOODS_NOTPASS_BIT       (1 << 5)   //bit5商品没出(出货确认没检测到)
 DRV_CMDERR_BIT          (1 << 6)   //bit6命令错误(只有发送命令和查询命令着两个命令，如果发了其他的命令就报错)
 DRV_GOCERR_BIT          (1 << 7)   //bit7出货检测模块状态(GOC故障)
 
** Returned value:	0:成功;1:故障;2:货道不存在;3:电机未到位;4:出货失败 5:通信故障
** 
*******************************************************************************/
unsigned char ChannelAPIHandle(unsigned char LogicNum,unsigned char HandleType,unsigned char GocType,unsigned char Binnum)
{
	unsigned char Rst=0;

	if(LogicNum == 0xff)
		return 2;
	Rst = ChannelSendMBox(HandleType,GocType,LogicNum,Binnum);
	TraceChannel("OutGoods_DeviceBack=%x bin = %d\r\n",Rst,Binnum);
	if(Rst == 0x00)
	{
		TraceChannel("\r\nOutGoods_OK");
		return 0;	
	}
	else if(Rst == 0xff)
	{
		TraceChannel("\r\nOutGoods_COMERR");
		return 5;
	}	
	//1.判断GOC是否故障bit7:  GOC故障->扣钱
	else if(Rst&DRV_GOCERR_BIT)
	{
		if(Rst&GOODS_SOLD_ERR)
		{
			//2.在判断电机：
			//没转到位等其他状况bit1,bit2,bit3,bit4->货道置故障  
			if( Rst & (GOODS_SOLDOUT_BIT|MOTO_MISPLACE_BIT|MOTO_NOTMOVE_BIT|MOTO_NOTRETURN_BIT)) 
			{
				TraceChannel("\r\nOutGoods_3");
				return 3;								
			}
			else
			{
				TraceChannel("\r\nOutGoods_4");
				return 4;	
			}
		}
		else
		{
			TraceChannel("\r\nOutGoods_0");
			return 0;
		}
	}
	else if(Rst&GOODS_SOLD_ERR)
	{
		/*
		//电机未转动
		if((GocType==1)&&(Rst&0x08))
			return 1;
		//出货确认开启。出货确认未检测到商品出货
		if((GocType==1)&&(Rst&0x20))
			return 4;	
		else
		//出货确认开启，电机未转到位，但出货确认检测到商品出货
		if((GocType==1)&&(Rst==0x11))
			return 0;
		else
		//出货确认未开启，电机未转到位
		if((GocType==0)&&(Rst&0x10))
			return 3;
		else
		//出货确认未开启，电机不能转动
		if((GocType==0)&&(Rst&0x08))
			return 1;
		*/
		/*********GOC打开的情况下********************/
		if(GocType==1)
		{			
			//1.电机未转动,不扣钱
			if(Rst&MOTO_NOTMOVE_BIT)
			{
				TraceChannel("\r\nOutGoods_1");
				return 1;
			}			
			//2.先判断GOC是否检测到bit5,  ==0说明出货确认检测到商品出货 			
			else if((Rst & GOODS_NOTPASS_BIT) == 0) 
			{				
				//有检测到->出货成功扣钱  
				/*************************************************************************/
				TraceChannel("\r\nOutGoods_0");
				return 0;		
			}
			else
			{ 
				   //没检测到->出货失败不扣钱
				   //3.在判断电机：
				   //没转到位等其他状况bit1,bit2,bit3,bit4->货道置故障	
				   if( Rst & (GOODS_SOLDOUT_BIT|MOTO_MISPLACE_BIT|MOTO_NOTMOVE_BIT|MOTO_NOTRETURN_BIT) ) 
				   {
						TraceChannel("\r\nOutGoods_3");
						return 3;
				   }
				   else
				   {
						TraceChannel("\r\nOutGoods_4");
						return 4;
				   }				   
			}
			
		}	
		/*********GOC关闭的情况下********************/
		else
		{
			//2.在判断电机：
		   //没转到位等其他状况bit1,bit2,bit3,bit4->货道置故障	
		   if( Rst & (GOODS_SOLDOUT_BIT|MOTO_MISPLACE_BIT|MOTO_NOTMOVE_BIT|MOTO_NOTRETURN_BIT)) 
		   {
				TraceChannel("\r\nOutGoods_3");
				return 3;								
		   }
		   else
		   {
				TraceChannel("\r\nOutGoods_4");
				return 4;	
		   }
		}
	}
	return 1;
}




//========================================================================
//第二代老货道接口函数
//整理 by yoc  2014.04.01
//=============================================================================


/*****************************************************************************
** Function name:	ChannelResetinit	
** Descriptions:	重新初始化货道参数														 			
** parameters:		无					
** Returned value:	无
*******************************************************************************/
void ChannelResetinit()
{
	hd_init(1);
	hd_init(2);
}


/*****************************************************************************
** Function name:	ChannelSaveParam	
**
** Descriptions:	将变更后的数据保存到FLASH中	
**					
**														 			
** parameters:		physicnum：保存索引
					1:逻辑结构数据保存
					2:前40个货道的物理结构数据保存
					3:后40个货道的逻辑结构数据保存
					4:备份数据保存
** Returned value:	无
** 
*******************************************************************************/
void ChannelSaveParam()
{
	hd_save_param(1);
	hd_save_param(2);
	ChannelGetFlashData();	
}


/*****************************************************************************
** Function name:	ChannelGetFlashData	
**
** Descriptions:	从flash中读取货道参数	
**					
**														 			
** parameters:		无
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelGetFlashData()
{
	hd_read_param(1);
	hd_read_param(2);
	read_hefan_info_by_flash();
}


/*****************************************************************************
** Function name:	ChannelExChannge	
**
** Descriptions:	货道开关操作后逻辑地址和物理地址之间的偏移交换	
**					
**														 			
** parameters:		LevelOrChnltype：1——某货道操作；2——某层操作
					Handletype:1——开启，2——关闭
					logicnum:逻辑货道编号
					Binnum：箱柜编号
					
** Returned value:	无
** 
*******************************************************************************/
void ChannelExChannge(unsigned char logicnum,unsigned char LevelOrChnltype,unsigned char Handletype,unsigned char Binnum)
{
	hd_open_handle(Binnum,logicnum,LevelOrChnltype,Handletype);
}




/*****************************************************************************
** Function name:	ChannelGetSelectColumn	
** Descriptions:	     	获取逻辑货道号											 			
** parameters:		Bin:箱柜编号
					type://1-选货按键 2-商品编号
					selectNo :选货按键号/id
** Returned value:	0:失败;  非0  返回货道逻辑
*******************************************************************************/

unsigned char ChannelGetSelectColumn(unsigned char Bin,unsigned char type,unsigned short SelectNum)
{
	if(type == 1)
		return hd_logicNo_by_selectNo(Bin,SelectNum);
	else
		return hd_logicNo_by_id(Bin,SelectNum);
}





/*****************************************************************************
** Function name:	ChannelGetGocState	
**
** Descriptions:	获得出货确认当前状态	
**					
**														 			
** parameters:		Binnum:箱柜编号
					
** Returned value:	0:正常;1:故障;
** 
*******************************************************************************/
unsigned char ChannelGetGocState(unsigned char Binnum)
{
	return 0;	
}




/*****************************************************************************
** Function name:	ChannelSetParam	
**
** Descriptions:	设置货道参数														 			
** parameters:		physicnum:物理地址
					HandleType:操作数据类型
					Value:新的数据
					Price:单价,成功交易次数					
** Returned value:	无
** 
*******************************************************************************/
unsigned char ChannelSetParam(unsigned char Logicnum,unsigned char Binnum,unsigned char HandleType,unsigned short Value,unsigned int Price)
{
	unsigned char type = HUODAO_TYPE_NOT;
	unsigned int tempValue = 0;
	switch(HandleType)
	{
		case CHANNELPRICE:
			type = HUODAO_TYPE_PRICE;
			tempValue = Price;
			break;
		case CHANNELCOUNT:
			type = HUODAO_TYPE_COUNT;
			tempValue = Value;
			break;
		case CHANNELSTATE:
			type = HUODAO_TYPE_STATE;
			tempValue = Value;
			break;
		case CHANNELMAXCOUNT:
			type = HUODAO_TYPE_MAX_COUNT;
			tempValue = Value;
			break;		
		case CHANNELSELECTNUM:
			type = HUODAO_TYPE_SELECTNO;
			tempValue = Value;
			break;
		case CHANNELSUCCESSCOUNT:
			type = HUODAO_TYPE_SUC_COUNT;
			tempValue = Price;
			break;
		case CHANNELGOODSNUM:
			type = HUODAO_TYPE_ID;
			tempValue = Value;
			break;
		default:
			type = HUODAO_TYPE_NOT;
			break;

	}

	return hd_set_by_logic(Binnum,Logicnum,type,tempValue);

}



/*****************************************************************************
** Function name:	ChannelGetPhysicNum	
** Descriptions:	通过输入的逻辑编号获得实际的物理编号													 			
** parameters:		Logicnum:物理地址					
** Returned value:	成功返回相应的物理编号，失败返回0xff
*******************************************************************************/
unsigned char ChannelGetPhysicNum(unsigned char Logicnum,unsigned char Binnum)
{
	unsigned char physicAddr = 0xFF,i,j,err;
	err = hd_get_index(Binnum,Logicnum,1,&i,&j);
	if(err)
	{
		if(Binnum == 1)
			physicAddr = stHuodao[i].huodao[j].physicAddr;
		else if(Binnum == 2)
			physicAddr = stSubHuodao[i].huodao[j].physicAddr;
	}
	return (physicAddr) ? physicAddr: 0xFF;
}


/*****************************************************************************
** Function name:	ChannelCheckIsOk	
** Descriptions:	检测货道是否可服务														 			
** parameters:		Logicnum:逻辑地址					
** Returned value:	0：正常；1：单价为0,3：货道故障;4:缺货；5：无此货道； 6商品ID为0 ；7PC置位不可用
*******************************************************************************/
unsigned int ChannelCheckIsOk(unsigned char Logicnum,unsigned char Binnum)
{
	return hd_check_ok(Binnum,Logicnum);	
}


/*****************************************************************************
** Function name:	ChannelGetParamValue	
** Descriptions:	读取货道参数值													 			
** parameters:		Logicnum:逻辑地址
					type：1——货道单价
						  2——货道数量
						  3——货道状态
						  4——货道最大存货量
						  5——选货按键值
						  6——成功交易次数
						  7——商品编号					
** Returned value:	0：无，其他相应值
*******************************************************************************/
unsigned int ChannelGetParamValue(unsigned char logicnum,unsigned char type,unsigned char Binnum)
{
	return hd_get_by_logic(Binnum,logicnum,type);
}


/*****************************************************************************
** Function name:	ChannelGetAddressLOWandRow	
** Descriptions:	通过传入的货道逻辑或物理编号得出其所在矩阵列表中的ROW和LOW													 			
** parameters:		Address:地址编号
					type：1——获得逻辑矩阵中的位置
						  2——获得物理矩阵中的位置
					Binnum:箱柜编号					
** Returned value:	0：无；
					1：成功索引到，Row横坐标，Low纵坐标
*******************************************************************************/
unsigned char ChannelGetAddressLOWandRow(unsigned char Type,unsigned char Binnum,
				unsigned char Address,unsigned char *Row,unsigned char *Low)
{	
	if(Type == 2)
		return hd_get_index(Binnum,Address,5,Row,Low);
	else if(Type == 1)
		return hd_get_index(Binnum,Address,1,Row,Low);
	return 0;
}


/*****************************************************************************
** Function name:	ChannelGetCloseCount	
** Descriptions:	根据不同的类型，算出层架的开关数量或者某层的关闭货道数量														 			
** parameters:		Level:某层
					type：1——某层的货道关闭数量
						  2——关闭的层数量
					Binnum:箱柜编号				
** Returned value:	0：无；
					1：成功索引到，Row横坐标，Low纵坐标
*******************************************************************************/
unsigned char ChannelGetCloseCount(unsigned char Binnum,unsigned char Type,unsigned char Level)
{
	
	return 0;
}






/*****************************************************************************
** Function name:	GetLevelOrColumnState	
** Descriptions:	获得某层的货道开关状态														 			
** parameters:		Num:  某层或者某物理货道
					Bin：1——主柜	 2——副柜
					Type：1——货道状态 2——层状态				
** Returned value:	0xff——失败1——开启0——关闭
*******************************************************************************/
unsigned char GetLevelOrColumnState(unsigned char Bin,unsigned char Type,unsigned char Num)
{
	unsigned char i,j,flag=0;
	//物理货道号
	if(Type==1)
	{
		flag = hd_get_index(Bin,Num,2,&i,&j);
		if(flag==0)
			return 0xFF;
		if(Bin==1)
			return stHuodao[i].huodao[j].openFlag;
		else if(Bin==2)
			return stSubHuodao[i].huodao[j].openFlag;
		else
			return 0xFF;
	}
	else if(Type==2)
	{
		if(Num < 1 || Num > HUODAO_MAX_ROW)
			return 0xFF;
		if(Bin==1)
			return stHuodao[Num - 1].openFlag;
		else if(Bin==2)
			return stSubHuodao[Num - 1].openFlag;
		else
			return 0xFF;
	}
	return 0xFF;	
}


/*****************************************************************************
** Function name:	GetColumnStateByPhysic	
** Descriptions:	      通过物理编号获得货道属性													 			
** parameters:		PhysicNum:货道物理编号
					Bin：1--主柜2--副柜
					Type：操作类型	
						1：单价
						2：储货数量
						3：状态
						4：最大储货数量
						5：成功交易次数
						6：选货按键
						7：商品编号				
** Returned value:	Type相应的值
*******************************************************************************/
unsigned int GetColumnStateByPhysic(unsigned char Bin,unsigned char PhysicNum,unsigned char Type)
{
	unsigned char tempType = HUODAO_TYPE_NOT;
	switch(Type)
	{
		case 1:
			tempType = HUODAO_TYPE_PRICE;
			break;
		case 2:
			tempType = HUODAO_TYPE_COUNT;
			break;
		case 3:
			tempType = HUODAO_TYPE_STATE;
			break;
		case 4:
			tempType = HUODAO_TYPE_MAX_COUNT;
			break;
		case 5:
			tempType = HUODAO_TYPE_SUC_COUNT;
			break;
		case 6:
			tempType = HUODAO_TYPE_SELECTNO;
			break;
		case 7:
			tempType = HUODAO_TYPE_ID;
			break;
		default:
			break;

	}
	
	return hd_get_by_physic(Bin,PhysicNum,tempType);
}


/*****************************************************************************
** Function name:	SetColumnParamByPhysic	
** Descriptions:	通过物理编号获得货道属性														 			
** parameters:		PhysicNum:货道物理编号
					Bin：1--主柜2--副柜
					Type：操作类型	
						1：单价
						2：储货数量
						3：状态
						4：最大储货数量
						5：成功交易次数
						6：选货按键
						7：商品编号				
** Returned value:	Type相应的值
*******************************************************************************/
unsigned int SetColumnParamByPhysic(unsigned char Bin,unsigned char PhysicNum,unsigned char Type,unsigned int Value)
{
	unsigned char tempType = HUODAO_TYPE_NOT;
	switch(Type)
	{
		case 1:
			tempType = HUODAO_TYPE_PRICE;
			break;
		case 2:
			tempType = HUODAO_TYPE_COUNT;
			break;
		case 3:
			tempType = HUODAO_TYPE_STATE;
			break;
		case 4:
			tempType = HUODAO_TYPE_MAX_COUNT;
			break;
		case 5:
			tempType = HUODAO_TYPE_SUC_COUNT;
			break;
		case 6:
			tempType = HUODAO_TYPE_SELECTNO;
			break;
		case 7:
			tempType = HUODAO_TYPE_ID;
			break;
		default:
			break;

	}
	return hd_set_by_physic(Bin,PhysicNum,tempType,Value);
	
}


/*****************************************************************************
** Function name:	AddColumnGoods	
** Descriptions:	添加货道存货余量													 			
** parameters:		
					Bin：1--主柜 2--副柜
					Type：操作类型	
						1：单货道添货
						2：层添货
						3：全部货道添货			
** Returned value:	0：失败；1：成功
*******************************************************************************/
unsigned char AddColumnGoods(unsigned char Bin,unsigned Type,unsigned char LogicNum)
{
	return hd_add_goods(Bin,LogicNum,Type);
}

/*****************************************************************************
** Function name:	FromPhysicGetLogicNum	
** Descriptions:	通过物理货道获得逻辑货道编号													 			
** parameters:		Bin：1——主柜2——副柜
					PhysicNum：物理货道编号			
** Returned value:	0：失败；其他：逻辑货道编号
*******************************************************************************/
unsigned char FromPhysicGetLogicNum(unsigned char Bin,unsigned char PhysicNum)
{
	return hd_logicNo_by_physic(Bin,PhysicNum);	
}


/*****************************************************************************
** Function name:	GetPhysicIndexByRowAndLow	
** Descriptions:	通过row和low索引地址，获得该逻辑货道映射物理货道的索引													 			
** parameters:		Bin：1——主柜2——副柜
					row：货道行索引
					low：货道列索引	
					type:1-索引得到逻辑货道编号
						 2-索引得到逻辑货道映射物理货道的索引值	
						 3-索引得到逻辑货道的状态
						 4-索引得到逻辑货道的储货数量	
** Returned value:	0：失败；其他：编号值
** 
*******************************************************************************/
unsigned char GetPhysicIndexByRowAndLow(unsigned char type,unsigned char Bin,unsigned char row,unsigned char low)
{
	ST_LEVEL_HUODAO *stHuodaoPtr;
	unsigned int value;
	if(Bin == 1)
		stHuodaoPtr = stHuodao;
	else if(Bin == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}
	switch(type)
	{
		case 1:
			value = stHuodaoPtr[row].huodao[low].logicNo;
			break;
		case 2:
			value = stHuodaoPtr[row].huodao[low].physicNo;
			break;
		case 3:
			value = stHuodaoPtr[row].huodao[low].state;
			break;
		case 4:
			value = stHuodaoPtr[row].huodao[low].count;
			break;
		case 5:
			value = stHuodaoPtr[row].huodao[low].openFlag;
			break;

		default:
			break;
	}
	
	return value;
	
}



//获得箱柜中可用的所有货道并统计编排
unsigned char ChannelGetAllOpenColumn(unsigned char Bin,unsigned char *IndexBuf)
{
	unsigned char  i,j,index = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(Bin == 1)
		stHuodaoPtr = stHuodao;
	else if(Bin == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		if(!stHuodaoPtr[i].openFlag)
			continue;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(stHuodaoPtr[i].huodao[j].openFlag && stHuodaoPtr[i].huodao[j].logicNo)
			{
				IndexBuf[index++] = stHuodaoPtr[i].huodao[j].logicNo;
			}
		}
	}
	return index;

	
}

/*****************************************************************************
** Function name:	pcSetHuodao													 			
** parameters:		setNums 设置货道个数 	stData 货道结构体数组指针	
** Returned value:	0：失败；1 成功
** 
*******************************************************************************/
unsigned char pcSetHuodao(unsigned char setNums,struct GoodsAttribute *stData)
{
	unsigned char i,cabinetNo,rst = 0;
	
	for(i = 0;i < setNums && setNums;i++)
	{
		cabinetNo = stData[i].Cabinet;
		if(cabinetNo < 3)
		{
			if(ChannelGetPhysicNum(stData[i].hd_no,cabinetNo) == 0xFF)//货道不存在
				continue;
				
			if(stData[i].hd_id)//软件开启
			{
				ChannelSetParam(stData[i].hd_no,cabinetNo,CHANNELGOODSNUM,stData[i].hd_id,0);		
				ChannelSetParam(stData[i].hd_no,cabinetNo,CHANNELCOUNT,stData[i].hd_count,0);
				ChannelSetParam(stData[i].hd_no,cabinetNo,CHANNELPRICE,0,stData[i].hd_Price);	
				if(ChannelGetParamValue(stData[i].hd_no,CHANNELSTATE,cabinetNo) == 3 ||
					ChannelGetParamValue(stData[i].hd_no,CHANNELSTATE,cabinetNo) == 4)
					
					ChannelSetParam(stData[i].hd_no,cabinetNo,CHANNELSTATE,1,0);		
			}
			else //软件关闭
			{		
				ChannelSetParam(stData[i].hd_no,cabinetNo,CHANNELSTATE,4,0);
			}

			rst = 1;
			
		}
		else//盒饭柜
		{
			cabinetNo = get_hefan_Index(cabinetNo);
			if(getHefanHuodaotState(cabinetNo,stData[i].hd_no) ==  0)//货道不存在
				continue;
			if(stData[i].hd_id)//软件开启
			{
				hefanSetParaVlaue(cabinetNo,stData[i].hd_no,stData[i].hd_id,HEFAN_HUODAO_ID);
				hefanSetParaVlaue(cabinetNo,stData[i].hd_no,stData[i].hd_Price,HEFAN_HUODAO_PRICE);
				if(hefanGetParaVlaue(cabinetNo,stData[i].hd_no,HEFAN_HUODAO_STATE) == HEFAN_HUODAO_CLOSE)
					hefanSetParaVlaue(cabinetNo,stData[i].hd_no,HEFAN_HUODAO_NORMAL,HEFAN_HUODAO_EMPTY);		
				if(stData[i].hd_count )
				{
					hefanSetParaVlaue(cabinetNo,stData[i].hd_no,HEFAN_HUODAO_NORMAL,HEFAN_HUODAO_STATE);	
				}
				else		
					hefanSetParaVlaue(cabinetNo,stData[i].hd_no,HEFAN_HUODAO_EMPTY,HEFAN_HUODAO_STATE);	
				
			}
			else //软件关闭
			{
				hefanSetParaVlaue(cabinetNo,stData[i].hd_no,HEFAN_HUODAO_CLOSE,HEFAN_HUODAO_STATE);	
						
			}
			
			rst = 1;
			
		}
	}

	return rst;
	
}


/*****************************************************************************
** Function name:	ChannelGetLevelNum													 			
** parameters:		
** Returned value:	0：失败；1 成功
** 
*******************************************************************************/

unsigned char ChannelGetLevelNum(unsigned char Binnum,unsigned char LogicNum)
{	
	unsigned char i,j;
	if(hd_get_index(Binnum,LogicNum,1,&i,&j))
		return i + 1;
	return 0;
		
}



/*****************************************************************************
** Function name:	nt_get_level_state	yoc
** Descriptions:														 			
** Returned value:	0:关闭;1:开启; 0xFF 不存在的层号
** 
*****************************************************************************/

unsigned char nt_get_level_state(unsigned char cabinetNo,unsigned char levelNo)
{	
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0xFF;
	}

	if(levelNo < 1 || levelNo > HUODAO_MAX_ROW)
		return 0xFF;

	return stHuodaoPtr[levelNo - 1].openFlag;
		
}

/*****************************************************************************
** Function name:	nt_get_huodoa_ON_OFF	yoc
** Descriptions:	   获取货道开关状态														 			
** Returned value:	0:关闭;1:开启; 0xFF 不存在的货道号
** 
*****************************************************************************/

unsigned char nt_get_huodoa_ON_OFF(unsigned char cabinetNo,unsigned char huodaoNo)
{	
	unsigned char  i,j;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0xFF;
	}
	if(hd_get_index(cabinetNo,huodaoNo,2,&i,&j))
	{
		return (stHuodaoPtr[i].openFlag) ? stHuodaoPtr[i].huodao[j].openFlag : 0;
	}
	return 0xFF;	
	
}

/*****************************************************************************
** Function name:	nt_huodao_exsit	yoc
** Descriptions:	   通过逻辑货道号检测货道是否存在														 			
** Returned value:	0:不能存在;1:存在;
** 
*****************************************************************************/
unsigned char nt_huodao_exsit(unsigned char cabinetNo,unsigned char logicNo)
{	
	unsigned char i,j;
	return hd_get_index(cabinetNo,logicNo,1,&i,&j);
	
}

/*****************************************************************************
** Function name:	nt_huodao_exsit_by_physic	yoc
** Descriptions:	   通过物理货道号检测货道是否存在														 			
** Returned value:	0:不能存在;1:存在;
** 
*****************************************************************************/
unsigned char nt_huodao_exsit_by_physic(unsigned char cabinetNo,unsigned char physicNo)
{	
	unsigned char row,col;
	row = physicNo / 10;
	col = (physicNo % 10) ? (physicNo % 10) : 10;
	if(cabinetNo == 1)
	{
		if(SystemPara.ChannelType == 1)//10道
			return (row >= 1 && row <= 8 && col >= 1 && col <= 10);
		else
			return (row >= 1 && row <= 8 && col >= 1 && col <= 8);			
	}
	else if(cabinetNo == 2)
	{
		if(SystemPara.SubBinChannelType == 1)//10道
			return (row >= 1 && row <= 8 && col >= 1 && col <= 10);
		else
			return (row >= 1 && row <= 8 && col >= 1 && col <= 8);
	}
	return 0;
	
}


/*****************************************************************************
** Function name:	special_cabinet_info													 			
** parameters:		
** Returned value:	返回非普通柜数量
** 
*******************************************************************************/

unsigned char special_cabinet_info(unsigned char *data)
{
	unsigned char i,cabinetNums = 0,index = 0;
	if(data == NULL)
	{
		print_log("data == NULL\r\n");
		return 0;
	}
	for(i = 0; i < 10;i ++)
	{

#if 0
			data[index++] = (i + 1);
			data[index++] = (i + 1) * 10;
			data[index++] =  0x10;
			cabinetNums++;
#else
		if(getHefanCabinetState(i + 1,HEFAN_EXIST))
		{
			data[index++] = (i + 1);
			data[index++] = (hefanGetParaVlaue(i + 1,0,HEFAN_CABINET_NUMS));
			data[index++] = (getHefanCabinetState(i + 1,HEFAN_HOT)) |
							(getHefanCabinetState(i + 1,HEFAN_HOT) << 1) |
							(getHefanCabinetState(i + 1,HEFAN_HOT) << 2);
			cabinetNums++;
		}
#endif
	}
	
	return cabinetNums;
	
}

/*****************************************************************************
** Function name:	ChannelGetPCChange	
** Descriptions:	根据接收PC机指令更改参数值														 			
** parameters:		type=1:更改商品编号
					type=2：更改商品单价
					type=3：更改商品存货数量 
					valuebuf：从PC机接收到的数据					
** Returned value:	无
** 
*******************************************************************************/
void ChannelGetPCChange(unsigned char cabinetNo,unsigned char type,unsigned char *valuebuf)
{
	unsigned char  i,j,index = 0,logicNo,r,c;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	unsigned int value;

	if(!valuebuf)
		return;
	
	cabinetNo = valuebuf[index++];//箱柜号
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		print_err(cabinetNo);
		return;
	}

	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++)
		{
			if(j >= 9)
				logicNo = (i + 1) * 10;
			else
				logicNo = (i + 1) * 10 + j + 1;
			
			value = valuebuf[index++];//提取数据包
			if(hd_get_index(cabinetNo,logicNo,1,&r,&c))//货道存在
			{
				if(type == 1)//更改ID 上位机固定发送80个字节数据
				{
					//上位机软关闭
					if(value == 0x00 || value == 0xFF)
					{
						stHuodaoPtr[r].huodao[c].id = 0;
						stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_PC_CLOSE;			
					}
					else//故障货道允许设置ID
					{
						stHuodaoPtr[r].huodao[c].id = value;
						if(stHuodaoPtr[r].huodao[c].state != HUODAO_STATE_FAULT && 
							stHuodaoPtr[r].huodao[c].state != HUODAO_STATE_NOT)
						{
							if(stHuodaoPtr[r].huodao[c].count)
								stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_NORMAL;
							else
								stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_EMPTY;
						}
					
					}
				}
				else if(type == 2)//更改商品单价
				{
					// 商品单价设置 暂时不用这个接口
					value = value;
				}
				else if(type == 3)
				{	
					if((stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_EMPTY)||
						(stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_NORMAL))
					{
						stHuodaoPtr[r].huodao[c].count = value;
						stHuodaoPtr[r].huodao[c].state = HUODAO_STATE_NORMAL;
					}
				}
			}
			
		}
	}
	

}


unsigned char hd_huodao_id_rpt_vp(unsigned char cabinetNo,unsigned char *buf)
{
	unsigned char i,j,index = 0,logicNo,r,c;
	
	cabinetNo = cabinetNo;
	
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++,index++)
		{
			if(j >= 9)
				logicNo = (i + 1) * 10;
			else
				logicNo = (i + 1) * 10 + j + 1;
			
			if(hd_get_index(1,logicNo,1,&r,&c))
				buf[index] = stHuodao[r].huodao[c].id;
			else
				buf[index] = 0x00;
			
		}
		
	}
	return 1;
}

/*****************************************************************************
** Function name:	hd_huodao_rpt_vp	
** Descriptions:		上报 HUODAO_RPT 							
** Returned value:	无
** 
*******************************************************************************/

unsigned char hd_huodao_rpt_vp(unsigned char cabinetNo,unsigned char *buf)
{
	unsigned char i,j,index = 0,logicNo,r,c;

	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
	{
		return 0;
	}

	
	for(i = 0;i < HUODAO_MAX_ROW;i++)
	{
		for(j = 0;j < HUODAO_MAX_COLUMN;j++,index++)
		{
			if(j >= 9)
				logicNo = (i + 1) * 10;
			else
				logicNo = (i + 1) * 10 + j + 1;
			buf[index] = 0;
			if(hd_get_index(cabinetNo,logicNo,1,&r,&c))
			{
				if(stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_PC_CLOSE)
				{
					buf[index] |= 0xC0;
				}
				else if(stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_NORMAL ||
					stHuodaoPtr[r].huodao[c].state == HUODAO_STATE_EMPTY)
				{
					buf[index] = 0;
				}
				else
					buf[index] |= 0x40;

				buf[index] |= ((stHuodaoPtr[r].huodao[c].count) & 0x3F);
			}
			else
				buf[index] = 0x80;
			
		}
		
	}
	return 1;

}

/*****************************************************************************
** Function name:	hd_id_by_logic	
** Descriptions:	通过逻辑货道获取该货道的ID号									 			
** parameters:		cabinetNo:箱柜编号		logic 逻辑货道
** Returned value:	返回ID号  失败返回0
*******************************************************************************/
unsigned char hd_id_by_logic(unsigned char cabinetNo,unsigned char logic)
{
	unsigned char i,j,id;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	//Trace("hd_id_by_logic:bin=%d,logic=%d\r\n",cabinetNo,logic);
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else{
		print_err(cabinetNo);
		return 0;
	}

	for(i = 0;i < HUODAO_MAX_ROW;i++){
		for(j = 0;j < HUODAO_MAX_COLUMN;j++){
			if(stHuodaoPtr[i].huodao[j].logicNo == logic){
				id = stHuodaoPtr[i].huodao[j].id;
				//Trace("stHuodaoPtr[%d][%d].id = %dr\n",i,j,id);
				return id;
			}		  	
		}
	}
	
	return 0 ;
	
}


/*****************************************************************************
** Function name:	hd_nums_by_id	
** Descriptions:	通过柜号获得货道总数									 			
** parameters:		cabinetNo:箱柜编号		
** Returned value:	返回货道总数
*******************************************************************************/
unsigned char hd_nums_by_id(unsigned char cabinetNo)
{
	unsigned char i,j,nums = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else{
		print_err(cabinetNo);
		return 0;
	}

	for(i = 0;i < HUODAO_MAX_ROW;i++){
		for(j = 0;j < HUODAO_MAX_COLUMN;j++){
			if((stHuodaoPtr[i].huodao[j].openFlag != 0) &&
				(stHuodaoPtr[i].huodao[j].id != 0)){
				nums++;
			}			
		}
	}

	return nums;
	
}

/*****************************************************************************
** Function name:	hd_state_by_id	
** Descriptions:	      通过ID号获得货道状态											 			
** parameters:		cabinetNo:箱柜编号		
					id :id号
** Returned value:	1:可用  0不可用
*******************************************************************************/
unsigned char hd_state_by_id(unsigned char cabinetNo,unsigned char id)
{
	unsigned char i,j;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else{
		print_err(cabinetNo);
		return 0;
	}

	for(i = 0;i < HUODAO_MAX_ROW;i++){
		for(j = 0;j < HUODAO_MAX_COLUMN;j++){
			if(stHuodaoPtr[i].huodao[j].id == id){
				if((stHuodaoPtr[i].huodao[j].state == 1) && 
					(stHuodaoPtr[i].huodao[j].count != 0)){
					return 1;
				}
			}			
		}
	}

	return 0;
}

/*****************************************************************************
** Function name:	hd_get_by_id	
** Descriptions:	通过ID号读取货道参数值													 			
** parameters:		logicNo:逻辑地址
					type：1——货道单价
						  2——货道数量
						  3——货道状态  货道状态：1:可用;2:故障;3:无货;4:PC置位不可用	
						  4——货道最大存货量
						  5——选货按键值
						  6——成功交易次数
						  7——商品编号					
** Returned value:	0：无，其他相应值
*******************************************************************************/
unsigned int hd_get_by_id(unsigned char cabinetNo,unsigned char id,unsigned char type)
{
	unsigned char  i,j,err;
	unsigned int value = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	if(cabinetNo == 1)
		stHuodaoPtr = stHuodao;
	else if(cabinetNo == 2 && SystemPara.SubBinOpen)
		stHuodaoPtr = stSubHuodao;
	else
		return 0;
	
	err = hd_get_index(cabinetNo,id,4,&i,&j);
	if(err)
	{
		switch(type)
		{
			case HUODAO_TYPE_PRICE:
				value = stHuodaoPtr[i].huodao[j].price;
				break;
			case HUODAO_TYPE_COUNT:
				value = stHuodaoPtr[i].huodao[j].count;
				break;
			case HUODAO_TYPE_MAX_COUNT:
				value = stHuodaoPtr[i].huodao[j].maxCount;
				break;
			case HUODAO_TYPE_STATE:
				value = stHuodaoPtr[i].huodao[j].state;
				break;
			case HUODAO_TYPE_SELECTNO:
				value = stHuodaoPtr[i].huodao[j].selectNo;
				break;
			case HUODAO_TYPE_ID:
				value = stHuodaoPtr[i].huodao[j].id;
				break;
			case HUODAO_TYPE_SUC_COUNT:
				value = stHuodaoPtr[i].huodao[j].sucCount;
				break;
			case HUODAO_TYPE_OPEN:
				value = (stHuodaoPtr[i].openFlag) ? (stHuodaoPtr[i].huodao[j].openFlag) : 0;
				break;
			default:
				break;
		}
		
	}
	return value;
	
}


/*****************************************************************************
** Function name:	hd_setNums_by_id	
** Descriptions:	      通过ID号设置货道余量											 			
** parameters:		cabinetNo:箱柜编号		
					id :ID号   value 余量值
** Returned value:	0:无此货到;  非0  返回货道逻辑号  0xFF 表示缺货
*******************************************************************************/
unsigned char hd_setNums_by_id(unsigned char cabinetNo,unsigned char id,unsigned char value)
{
	unsigned char i,j;
	ST_LEVEL_HUODAO *stHuodaoPtr;

	if(cabinetNo == 1){
		stHuodaoPtr = stHuodao;
	}	
	else if(cabinetNo == 2 && SystemPara.SubBinOpen){
		stHuodaoPtr = stSubHuodao;
	}	
	else{
		print_err(cabinetNo);
		return 0;
	}
	
	for(i = 0;i < HUODAO_MAX_ROW;i++){
		for(j = 0;j < HUODAO_MAX_COLUMN;j++){
			if(stHuodaoPtr[i].huodao[j].id == id){
				if(value == 0){
					stHuodaoPtr[i].huodao[j].count = 0;
					if(stHuodaoPtr[i].huodao[j].state != HUODAO_STATE_FAULT){
						stHuodaoPtr[i].huodao[j].state = HUODAO_STATE_EMPTY;
					}
				}
				else{
					stHuodaoPtr[i].huodao[j].count = value & 0x3F;
					if(stHuodaoPtr[i].huodao[j].state != HUODAO_STATE_FAULT){
						stHuodaoPtr[i].huodao[j].state = HUODAO_STATE_NORMAL;
					}
				}
			}
		}
	}
	
	return 1;
}


/*****************************************************************************
** Function name:	hd_ids_by_level	
** Descriptions:	通过层号获取该层的起始ID和结束ID											 			
** parameters:		cabinetNo:箱柜编号		
					level :层号   
					startId 起始ID stopid 结束ID
** Returned value:	0:失败; 1成功
*******************************************************************************/
unsigned char hd_ids_by_level(unsigned char cabinetNo,unsigned char level,unsigned char *startId,unsigned char *stopId)
{
	unsigned char i,j,temp = 0;
	ST_LEVEL_HUODAO *stHuodaoPtr;
	*startId = 0;
	*stopId = 0;
	if(cabinetNo == 1){
		stHuodaoPtr = stHuodao;
	}	
	else if(cabinetNo == 2 && SystemPara.SubBinOpen){
		stHuodaoPtr = stSubHuodao;
	}	
	else{
		print_err(cabinetNo);
		return 0;
	}
	
	for(i = 0;i < HUODAO_MAX_ROW;i++){
		if(stHuodaoPtr[i].openFlag != 1)
			continue;
		temp++;
		if(level != temp)
			continue;
		for(j = 0;j < HUODAO_MAX_COLUMN;j++){
			if(stHuodaoPtr[i].huodao[j].openFlag == 1 &&
				stHuodaoPtr[i].huodao[j].id != 0){
				if(*startId == 0){
					*startId = stHuodaoPtr[i].huodao[j].id;
				}
				else{
					*stopId = stHuodaoPtr[i].huodao[j].id;
				}
			}
		}
	}

	
	return (*startId != 0 && *stopId != 0);
}



/*--------------------------------------------------------------------------------
										修改记录
1.日期：2013.11.6 修改人：liya 
  内容：对文件中的函数做了清楚整理
--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------
										修改记录
1.日期：2014.04.01 修改人：yoc
  内容：设计新型货道结构体及 接口函数
--------------------------------------------------------------------------------*/





