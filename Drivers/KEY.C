/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           MDB.h
** Last modified Date:  2013-01-10
** Last Version:         
** Descriptions:        2*8键盘驱动                     
**------------------------------------------------------------------------------------------------------
** Created by:          sunway
** Created date:        2013-01-08
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/
#include "..\config.h"

#define	KEYINTERRUPUT	(1ul<<2)

#define KEYROW0	(1ul<<14)		//行0 P1.14
#define KEYROW1	(1ul<<15)		//行1 P1.15

#define	KEYCOL0	(1ul<<16)		//列0 P1.16
#define	KEYCOL1	(1ul<<17)		//列1 P1.17
#define	KEYCOL2	(1ul<<4)		//列2 P0.4
#define	KEYCOL3	(1ul<<5)		//列3 P0.5
#define	KEYCOL4	(1ul<<6)		//列4 P0.6
#define	KEYCOL5	(1ul<<7)		//列5 P0.7
#define	KEYCOL6	(1ul<<8)		//列6 P0.8
#define	KEYCOL7	(1ul<<9)		//列7 P0.9

#define MAINTAINKEY (1ul<<29)	//维护按键P4.29
#define PAYBACKKEY	(1ul<<22)	//退币按键P0.22
#define GAMEKEY		(1ul<<7)	//游戏按键P2.7
#define CLOSEHUMAN	(1ul<<3)	//人体接近感应器P2.3
#define GAMEKEYLED	(1ul<<27)	//游戏按键灯P1.27



#define KEY_QUEUE_SIZE 3
void *KeyValueQ[KEY_QUEUE_SIZE];
OS_EVENT *QKey;
volatile unsigned char PostValue,KeyBoardStatus;

void ScanDelayTime(unsigned int ms);
void SetKeyColOutput(void);
void SetKeyColInput(void);
void SetKeyRowOutput(void);
void SetKeyRowInput(void);
unsigned char ReadColValue(void);
unsigned char ReadRowValue(void);
unsigned char ScanKeyBoard(void);

const unsigned char KEY_ROW0_TABLE[8] = {'5','4','8','7','1','2','3','6'};
const unsigned char KEY_ROW1_TABLE[8] = {'>','C','E','D','9','.','0','<'};

typedef struct
{
	//按键值
	uint8_t   Keynum;                       //按键值
	
} KEYPack;
KEYPack KeyPack[KEY_QUEUE_SIZE];
uint8_t  g_Key_Queue = 0;


/*********************************************************************************************************
* Function Name:        InitKeyboard
* Description:          P2.12中断输入
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void InitKeyboard(void)
{
	QKey = OSQCreate(&KeyValueQ[0],KEY_QUEUE_SIZE);
	FIO4DIR &= (~MAINTAINKEY);	//维护按键GPIO口设置为输入P4.29
	FIO0DIR &= (~PAYBACKKEY);	//退币按键GPIO口设置为输入P0.22
	FIO2DIR &= (~GAMEKEY);		//游戏按键GPIO口设置为输入P2.7
	FIO2DIR &= (~CLOSEHUMAN);	//人体接近感应器口设置为输入P2.3
	SetKeyRowInput();
	SetKeyColOutput();
	zyIsrSet(NVIC_EINT2,(unsigned long)EINT2_IsrHandler,PRIO_TEN);
	EXTINT   = 0x00000007;		//清中断
	EXTMODE	 = KEYINTERRUPUT;	//边沿触发
	EXTPOLAR = 0x00000000;
	KeyBoardStatus = 1;
	SetKeyColOutput();
	SetKeyRowInput();
}
/*********************************************************************************************************
* Function Name:        ScanKeyBoard
* Description:          键盘扫描
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void EINT2_IsrHandler(void)
{
	//unsigned char err;
	//unsigned int i = 5000;
	//Trace("\r\n btnHandler");
	OSIntEnter();
	if(KeyBoardStatus == 1)
	{
		PostValue = ScanKeyBoard();
		//Trace("\r\n btnPost=%d",PostValue);
		if(PostValue != 0x00)
		{
			KeyPack[g_Key_Queue].Keynum = PostValue;
			//err = OSQPost(QKey,&KeyPack[g_Key_Queue]);
			//Trace("\r\n postkey1=%d,err=%d",KeyPack.Keynum,err);
			//if(err != OS_NO_ERR)
			//{
				//while(--i);
				OSQPost(QKey,&KeyPack[g_Key_Queue]);
				//Trace("\r\n btnPost=%d",KeyPack[g_Key_Queue].Keynum);
				//更新序列号
				if(g_Key_Queue<(KEY_QUEUE_SIZE-1))
					g_Key_Queue++;
				else
					g_Key_Queue=0;
				
			//}
		}
		SetKeyColOutput();
		SetKeyRowInput();
		EXTINT   = 0x00000007;
	}
    OSIntExit();	
}
/*********************************************************************************************************
* Function Name:        ReadKeyValue
* Description:          键盘API函数：从键盘消息队列中取出案件值并返回该按键值
* Input:                None
* Output:               None
* Return:               按键值
*********************************************************************************************************/
unsigned char ReadKeyValue(void)
{
	unsigned char err;
	//unsigned char *PendKey;
	KEYPack *KeyMsg;
	
	if(KeyBoardStatus == 0)
		return 0x00;
	KeyMsg = OSQPend(QKey,10,&err);
	if(err == OS_NO_ERR)
	{
		//Trace("\r\n btnPend=%d",KeyMsg->Keynum);
		return KeyMsg->Keynum;
	}
	else
		return 0x00;	
}
/*********************************************************************************************************
* Function Name:        ClearKey
* Description:          键盘API函数：清除键盘按键值
* Input:                None
* Output:               None
* Return:               按键值
*********************************************************************************************************/
void ClearKey(void)
{	
	OSQFlush(QKey);
}

/*********************************************************************************************************
* Function Name:        DisabaleKeyBoard
* Description:          P2.12中断输入
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void DisabaleKeyBoard(void)
{
	KeyBoardStatus = 0;
	PINSEL4 &= (~(0x03<<24));
	FIO2DIR &= (~(1ul<<12));
}
/*********************************************************************************************************
* Function Name:        InitKeyboard
* Description:          P2.12中断输入
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void EnableKeyBoard(void)
{
	PINSEL4 |= (P2_12_EINT2<<24);
	KeyBoardStatus = 1;	
}
/*********************************************************************************************************
* Function Name:        ScanDelayTime
* Description:          软件延时,n个ms
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void ScanDelayTime(unsigned int ms)
{
	unsigned long i,j;
	for(i=0;i<ms;i++)
	{
		for(j=0;j<16500;j++)
		{
			__nop();
		}	
	}
}
/*********************************************************************************************************
* Function Name:        SetKeyColOutput
* Description:          设置列为输出
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void SetKeyColOutput(void)
{
	FIO1DIR |= KEYCOL0; FIO1CLR |= KEYCOL0;    	//列0 P1.16
	FIO1DIR |= KEYCOL1;	FIO1CLR |= KEYCOL1;    	//列1 P1.17
	FIO0DIR |= KEYCOL2; FIO0CLR |= KEYCOL2;  	//列2 P0.4
	FIO0DIR |= KEYCOL3; FIO0CLR |= KEYCOL3; 	//列3 P0.5	
	FIO0DIR |= KEYCOL4; FIO0CLR |= KEYCOL4;    	//列4 P0.6
	FIO0DIR |= KEYCOL5; FIO0CLR |= KEYCOL5;    	//列5 P0.7
	FIO0DIR |= KEYCOL6; FIO0CLR |= KEYCOL6;    	//列6 P0.8
	FIO0DIR |= KEYCOL7; FIO0CLR |= KEYCOL7;	   	//列7 P0.9
}
/*********************************************************************************************************
* Function Name:        SetKeyColInput
* Description:          设置列为输入
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void SetKeyColInput(void)
{
	unsigned char i;
	FIO1DIR &= (~KEYCOL0); 
	FIO1DIR &= (~KEYCOL1); 
	FIO0DIR &= (~KEYCOL2); 
	FIO0DIR &= (~KEYCOL3); 
	FIO0DIR &= (~KEYCOL4); 
	FIO0DIR &= (~KEYCOL5); 
	FIO0DIR &= (~KEYCOL6); 
	FIO0DIR &= (~KEYCOL7);
	for(i=0;i<100;i++)
	{
		__nop();
	}
}
/*********************************************************************************************************
* Function Name:        SetKeyRowOutput
* Description:          设置行为输出
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void SetKeyRowOutput(void)
{
	FIO1DIR |= KEYROW0; FIO1CLR |= KEYROW0;
	FIO1DIR |= KEYROW1;	FIO1CLR |= KEYROW1;
}
/*********************************************************************************************************
* Function Name:        SetKeyRowInput
* Description:          设置行为输入
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void SetKeyRowInput(void)
{
	unsigned char i;
	FIO1DIR &= (~KEYROW0);
	FIO1DIR &= (~KEYROW1);
	for(i=0;i<100;i++)
	{
		__nop();
	}
}
/*********************************************************************************************************
* Function Name:        ReadColumVale
* Description:          读取列值
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
unsigned char ReadColValue(void)
{
	unsigned char ColValue;
	unsigned int col,col0,col1;
	col0 = (FIO1PIN & KEYCOL0) | (FIO1PIN & KEYCOL1);
	col0 = (col0>>16);
	col1 = (FIO0PIN & (0x3F<<4));
	col1 = (col1>>2);
	col  = col0|col1;
	ColValue = (unsigned char)col;
	return ColValue;	
}
/*********************************************************************************************************
* Function Name:        ReadRowumVale
* Description:          读取行值
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
unsigned char ReadRowValue(void)
{
	unsigned char RowValue;
	unsigned int Rda;
	Rda = (FIO1PIN & (0x00000003<<14));
	Rda = Rda>>14;
	RowValue = (unsigned char)Rda;
	return RowValue;
}
/*********************************************************************************************************
* Function Name:        ScanKeyBoard
* Description:          键盘扫描
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
unsigned char ScanKeyBoard(void)
{
	unsigned char i,comp = 0;
	unsigned char RowValue,ColValue;
	ScanDelayTime(5);
	//Trace("\r\n btnPost1");
	if(((FIO1PIN & KEYROW0) ==0)||((FIO1PIN & KEYROW1) == 0))
	{
		ScanDelayTime(8);
		//Trace("\r\n btnPost2");
		if(((FIO1PIN & KEYROW0) ==0)||((FIO1PIN & KEYROW1) == 0))
		{
			//Trace("\r\n btnPost3");
			SetKeyRowOutput();
			SetKeyColInput();
			ColValue = ReadColValue();
			ColValue = (~ColValue);
			SetKeyColOutput();
			SetKeyRowInput();
			RowValue = ReadRowValue();
			if((RowValue >= 0x03)||(RowValue == 0x00))
				return 0x00;
			else
			{
				if(RowValue == 0x01)
				{
					for(i=0;i<8;i++)
					{
						if(ColValue == (1<<i))
						{
							comp = 1;
							break;
						}
						else
							comp = 0;
					}
					if(comp == 1)
					{
						Buzzer();
						return KEY_ROW0_TABLE[i];
					}
					else
						return 0x00;
				}
				if(RowValue == 0x02)
				{
					for(i=0;i<8;i++)
					{
						if(ColValue == (1<<i))
						{
							comp = 1;
							break;
						}
						else
							comp = 0;
					}
					if(comp == 1)
					{
						Buzzer();
						return KEY_ROW1_TABLE[i];
					}
					else
						return 0x00;
				}		
			}	
		}
		else
			return 0x00;
	}
	return 0x00;
}
/*********************************************************************************************************
* Function Name:        ReadMaintainKeyValue
* Description:          读取维护按键
* Input:                None
* Output:               None
* Return:               1：无按键；0：按键按下
*********************************************************************************************************/
unsigned char ReadMaintainKeyValue(void)
{
	if((FIO4PIN & MAINTAINKEY) == 0)
	{
		OSTimeDly(6);//延时30ms
		if((FIO4PIN & MAINTAINKEY) == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;		
}
/*********************************************************************************************************
* Function Name:        ReadPayBackKeyValue
* Description:          读取退币按键
* Input:                None
* Output:               None
* Return:               1：按键按下；0：无按键
*********************************************************************************************************/
unsigned char ReadPayBackKeyValue(void)
{
	if((FIO0PIN & PAYBACKKEY) == 0)
	{
		OSTimeDly(6);//延时30ms
		if((FIO0PIN & PAYBACKKEY) == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;	
}
/*********************************************************************************************************
* Function Name:        PayBackKeyValue
* Description:          读取游戏按键
* Input:                None
* Output:               None
* Return:               0：无按键；1：按键按下
*********************************************************************************************************/
unsigned char ReadGameKeyValue(void)
{
	if((FIO2PIN & GAMEKEY) == 0)
	{
		OSTimeDly(6);//延时30ms
		if((FIO2PIN & GAMEKEY) == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
/*********************************************************************************************************
* Function Name:        ReadCloseHumanKeyValue
* Description:          读取接近感应器
* Input:                None
* Output:               None
* Return:               1：按键按下；0：无按键
*********************************************************************************************************/
unsigned char ReadCloseHumanKeyValue(void)
{
	if((FIO2PIN & CLOSEHUMAN) == 0)//低电平，代表有过来
	{
		OSTimeDly(6);//延时30ms
		if((FIO2PIN & CLOSEHUMAN) == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;	
}

/*********************************************************************************************************
** Function name:       gameLedControl
** Descriptions:        游戏按键灯开关控制
** input parameters:    flag 1 开灯 0 关灯
** output parameters:   无
** Returned value:      
*********************************************************************************************************/

void gameLedControl(unsigned char flag)
{
	FIO1DIR |= GAMEKEYLED;
	if(flag)
		FIO1SET |=  GAMEKEYLED;
	else
		FIO1CLR |= GAMEKEYLED;
	OSTimeDly(10);
}



/*********************************************************************************************************
** Function name:       GetKeyInValue
** Descriptions:        获得输入的按键键值
** input parameters:    无
** output parameters:   无
** Returned value:      有按键返回相应的键值，无按键返回0xff
*********************************************************************************************************/
uint8_t GetKeyInValue()
{
	uint8_t mkey;
	
	mkey = ReadKeyValue();
	if(mkey != 0x00)
	{
		switch(mkey)
		{
			case 0x31:
				return 0x01;
			case 0x32:
				return 0x02;
			case 0x33:
				return 0x03;
			case 0x34:
				return 0x04;
			case 0x35:
				return 0x05;
			case 0x36:
				return 0x06;
			case 0x37:
				return 0x07;
			case 0x38:
				return 0x08;
			case 0x39:
				return 0x09;
			case 0x30:
				return 0x00;
			case 0x44:
				return 0x0c;
			case 0x2e:
				return 0x0d;
			case 0x3c:
				return 0x0a;
			case 0x3e:
				return 0x0b;
			case 0x43://取消按键
				return 0x0e;
			case 0x45://确定按键
				return 0x0f;
			default:
				return 0xff;
		}
	}
	return 0xff;	
}
/*********************************************************************************************************
**				COL0	COL1	COL2	COL3	COL4	COL5	COL6	COL7
**			-----------------------------------------------------------------
** 	ROW0	|	6	  |	  3   |   2   |   1   |	  7	 |   8   |  4    |   5   |
**			-----------------------------------------------------------------
**	ROW1	|	<	  |	  0   |   .   |   9   |	 Clr | Enter |Cancel |   >   |
**			-----------------------------------------------------------------
*********************************************************************************************************/
/**************************************End Of File*******************************************************/
