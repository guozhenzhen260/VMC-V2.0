/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           SelectKeyAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        选货按键中间程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Created date:        2013-03-04
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"
#include "SelectKeyAPI.h"


/*********************************************************************************************************
** Function name:     	SelectionKeyMBox
** Descriptions:	    将实际按键值传给邮箱
** input parameters:   						 
						mode=1,交易状态,mode=0,空闲状态
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void SelectionKeyMBox( uint8_t mode )
{
	
	MsgKEYPack.sel1ErrLed = selectKey.sel1ErrLed;
    MsgKEYPack.sel1ReadyLed = selectKey.sel1ReadyLed;
	MsgKEYPack.sel2ErrLed = selectKey.sel2ErrLed;
    MsgKEYPack.sel2ReadyLed = selectKey.sel2ReadyLed;
	MsgKEYPack.sel3ErrLed = selectKey.sel3ErrLed;
    MsgKEYPack.sel3ReadyLed = selectKey.sel3ReadyLed;
	MsgKEYPack.sel4ErrLed = selectKey.sel4ErrLed;
   	MsgKEYPack.sel4ReadyLed = selectKey.sel4ReadyLed;
	MsgKEYPack.sel5ErrLed = selectKey.sel5ErrLed;
    MsgKEYPack.sel5ReadyLed = selectKey.sel5ReadyLed;
	MsgKEYPack.sel6ErrLed = selectKey.sel6ErrLed;
    MsgKEYPack.sel6ReadyLed = selectKey.sel6ReadyLed;
	MsgKEYPack.selectMode = mode;
}

/*********************************************************************************************************
** Function name:     	FreeSelectionKey
** Descriptions:	    空闲状态控制灯
** input parameters:   						 
						mode=1,打开所有灯,mode=0,关闭所有灯,
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void FreeSelectionKey( uint8_t mode )
{
	if(mode == 1)
	{
		selectKey.sel1ErrLed = 0xff;
	    selectKey.sel1ReadyLed = 0xff;
		selectKey.sel2ErrLed = 0xff;
	    selectKey.sel2ReadyLed = 0xff;
		selectKey.sel3ErrLed = 0xff;
	    selectKey.sel3ReadyLed = 0xff;
		selectKey.sel4ErrLed = 0xff;
	    selectKey.sel4ReadyLed = 0xff;
		selectKey.sel5ErrLed = 0xff;
	    selectKey.sel5ReadyLed = 0xff;
		selectKey.sel6ErrLed = 0xff;
	    selectKey.sel6ReadyLed = 0xff;		
	}	
	else if(mode == 0)
	{
		selectKey.sel1ErrLed = 0;
	    selectKey.sel1ReadyLed = 0;
		selectKey.sel2ErrLed = 0;
	    selectKey.sel2ReadyLed = 0;
		selectKey.sel3ErrLed = 0;
	    selectKey.sel3ReadyLed = 0;
		selectKey.sel4ErrLed = 0;
	    selectKey.sel4ReadyLed = 0;
		selectKey.sel5ErrLed = 0;
	    selectKey.sel5ReadyLed = 0;
		selectKey.sel6ErrLed = 0;
	    selectKey.sel6ReadyLed = 0;
	}
	//将值传给邮箱
	SelectionKeyMBox(0);
}


/*********************************************************************************************************
** Function name:     	FreeSelectionKey
** Descriptions:	    空闲状态控制灯
** input parameters:   						 
						mode=1,打开所有灯,mode=0,关闭所有灯,
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void FreeSelectionKeyAPI( uint8_t mode )
{
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:
			FreeSelectionKey(mode);
			MsgKEYPack.KeyCmd = MBOX_SELECTLIGHT;			
			OSMboxPost(g_KEYMail,&MsgKEYPack);
			break;	
		default:break;	
	}
}


/*********************************************************************************************************
** Function name:     	FreeSelectionKeyValueAPI
** Descriptions:	    空闲状态控制打勾的灯和打叉的灯
** input parameters:   						 
						
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void FreeSelectionKeyValueAPI()
{
	uint8_t i;
	uint8_t columnNo=0;
	
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:
			selectKey.sel1ErrLed = 0;
		    selectKey.sel1ReadyLed = 0;
			selectKey.sel2ErrLed = 0;
		    selectKey.sel2ReadyLed = 0;
			selectKey.sel3ErrLed = 0;
		    selectKey.sel3ReadyLed = 0;
			selectKey.sel4ErrLed = 0;
		    selectKey.sel4ReadyLed = 0;
			selectKey.sel5ErrLed = 0;
		    selectKey.sel5ReadyLed = 0;
			selectKey.sel6ErrLed = 0;
		    selectKey.sel6ReadyLed = 0;

			for( i=11; i<=16; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel1ReadyLed |= 1<<(i%10-1);		        	
		        }	
				else
				{      
		        	selectKey.sel1ErrLed |= 1<<(i%10-1);		        	
		        }	
		    }
			for( i=21; i<=26; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel2ReadyLed |= 1<<(i%10-1);		        	
		        }	
				else
				{      
		        	selectKey.sel2ErrLed |= 1<<(i%10-1);		        	
		        }
		    }
			for( i=31; i<=36; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel3ReadyLed |= 1<<(i%10-1);		        	
		        }
				else
				{      
		        	selectKey.sel3ErrLed |= 1<<(i%10-1);		        	
		        }
		    }
			for( i=41; i<=46; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel4ReadyLed |= 1<<(i%10-1);		        	
		        }	
				else
				{      
		        	selectKey.sel4ErrLed |= 1<<(i%10-1);		        	
		        }
		    }
			for( i=51; i<=56; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel5ReadyLed |= 1<<(i%10-1);		        	
		        }	
				else
				{      
		        	selectKey.sel5ErrLed |= 1<<(i%10-1);		        	
		        }
		    }
			for( i=61; i<=66; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	selectKey.sel6ReadyLed |= 1<<(i%10-1);		        	
		        }	
				else
				{      
		        	selectKey.sel6ErrLed |= 1<<(i%10-1);		        	
		        }
		    }
			//将值传给邮箱
			SelectionKeyMBox(0);
			
			MsgKEYPack.KeyCmd = MBOX_SELECTLIGHT;			
			OSMboxPost(g_KEYMail,&MsgKEYPack);
			break;	
		default:break;	
	}
}


/*********************************************************************************************************
** Function name:     	SaleSelectionKeyAPI
** Descriptions:	    交易状态控制灯
** input parameters:   						 
						mode=1,打开所有灯,mode=0,关闭所有灯,
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void SaleSelectionKeyAPI( uint32_t price )
{
	uint8_t i;
	uint8_t columnNo=0;
	
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:
			selectKey.sel1ErrLed = 0;
		    selectKey.sel1ReadyLed = 0;
			selectKey.sel2ErrLed = 0;
		    selectKey.sel2ReadyLed = 0;
			selectKey.sel3ErrLed = 0;
		    selectKey.sel3ReadyLed = 0;
			selectKey.sel4ErrLed = 0;
		    selectKey.sel4ReadyLed = 0;
			selectKey.sel5ErrLed = 0;
		    selectKey.sel5ReadyLed = 0;
			selectKey.sel6ErrLed = 0;
		    selectKey.sel6ReadyLed = 0;

			for( i=11; i<=16; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel1ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			for( i=21; i<=26; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel2ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			for( i=31; i<=36; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel3ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			for( i=41; i<=46; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel4ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			for( i=51; i<=56; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel5ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			for( i=61; i<=66; i++ )
		    {   
		    	columnNo = ChannelGetSelectColumn(1,1,i);
		    	if(columnNo)
		        {      
		        	if(price >= ChannelGetParamValue(columnNo,1,1))
		        		selectKey.sel6ReadyLed |= 1<<(i%10-1);		        	
		        }		          
		    }
			//将值传给邮箱
			SelectionKeyMBox(1);
			
			MsgKEYPack.KeyCmd = MBOX_SELECTLIGHT;			
			OSMboxPost(g_KEYMail,&MsgKEYPack);
			break;
		default:break;	
	}
}



/*********************************************************************************************************
** Function name:     	HuodaoSetSelectionKeyAPI
** Descriptions:	    货道配置时控制灯
** input parameters:   						 
						keyID展示位编号
** output parameters:   无
** Returned value: 
*********************************************************************************************************/
void HuodaoSetSelectionKeyAPI( uint8_t keyID )
{
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:
			selectKey.sel1ErrLed = 0;
		    selectKey.sel1ReadyLed = 0;
			selectKey.sel2ErrLed = 0;
		    selectKey.sel2ReadyLed = 0;
			selectKey.sel3ErrLed = 0;
		    selectKey.sel3ReadyLed = 0;
			selectKey.sel4ErrLed = 0;
		    selectKey.sel4ReadyLed = 0;
			selectKey.sel5ErrLed = 0;
		    selectKey.sel5ReadyLed = 0;
			selectKey.sel6ErrLed = 0;
		    selectKey.sel6ReadyLed = 0;
			switch(keyID/10)
			{
				case 1:
					selectKey.sel1ReadyLed |= 1<<(keyID%10-1);							
					break;
				case 2:
					selectKey.sel2ReadyLed |= 1<<(keyID%10-1);	
					break;
				case 3:
					selectKey.sel3ReadyLed |= 1<<(keyID%10-1);	
					break;
				case 4:
					selectKey.sel4ReadyLed |= 1<<(keyID%10-1);	
					break;
				case 5:
					selectKey.sel5ReadyLed |= 1<<(keyID%10-1);	
				case 6:
					selectKey.sel6ReadyLed |= 1<<(keyID%10-1);		
					break;
				default:break;	
			}
			//将值传给邮箱
			SelectionKeyMBox(1);
			TraceSelection("\r\n MiddLed=%d,%x,%x,%x,%x,%x,%x",MsgKEYPack.selectMode,MsgKEYPack.sel1ReadyLed,MsgKEYPack.sel2ReadyLed,MsgKEYPack.sel3ReadyLed,MsgKEYPack.sel4ReadyLed,MsgKEYPack.sel5ReadyLed,MsgKEYPack.sel6ReadyLed);
			MsgKEYPack.KeyCmd = MBOX_SELECTLIGHT;			
			OSMboxPost(g_KEYMail,&MsgKEYPack);
			break;	
		default:break;	
	}
}



/*********************************************************************************************************
** Function name:       GetSelectKeyAPI
** Descriptions:        得到按键值
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      0无按键,其他为按键值
*********************************************************************************************************/
uint8_t GetSelectKeyAPI()
{
	MessageKEYPackBack *AccepterMsg;
	unsigned char ComStatus;
	uint8_t key = 0;
	
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:			
			//OSTimeDly(OS_TICKS_PER_SEC/2);
			AccepterMsg = OSMboxPend(g_KEYBackMail,10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				//Trace("\r\n MiddKeykey1=%d,%d",AccepterMsg->KeyBackCmd,AccepterMsg->selectInput);
				if((AccepterMsg->KeyBackCmd)==MBOX_SELECTVALUE)
				{	
					TraceSelection("\r\n MiddKeykey2=%d",AccepterMsg->selectInput);
					key = AccepterMsg->selectInput;	
				}
			}
			break;	
		default:break;	
	}
	
	
	return key;
}


/*********************************************************************************************************
** Function name:       SelectInitAPI
** Descriptions:        启动选货按键板
** input parameters:    pvData: 没有使用
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void SelectInitAPI()
{
	switch(SystemPara.UserSelectKeyBoard)
	{
		case SELECT_KEY:
			TraceSelection("\r\n MiddInitKey");
			FreeSelectionKey(1);
			
			MsgKEYPack.KeyCmd = MBOX_SELECTKEYINIT;			
			OSMboxPost(g_KEYMail,&MsgKEYPack);
			OSTimeDly(OS_TICKS_PER_SEC);
			break;	
		default:break;	
	}
	return;
}



/*********************************************************************************************************
** Function name:       KeyValueAPI
** Descriptions:        得到按键值
** input parameters:    
                        freeMode=0在一般模式，按取消键无效
                                =1在货道输入模式，按取消键有效
** output parameters:   keyMode=1,表明这个按键是选货按钮按下的,
                               =2,表明这个按键是小键盘按下的,
                               =0表明没有按键按下
** Returned value:      0无按键,其他为按键值
*********************************************************************************************************/
uint8_t KeyValueAPI(uint8_t freeMode,uint8_t *keyMode)
{	
	uint8_t key = 0,keyValue = 0;
	*keyMode = 0;
	//开启pc时,按键无效
	//if(SystemPara.PcEnable==UBOX_PC)
	//	return 0;
	
	if(SystemPara.UserSelectKeyBoard==SELECT_KEY)
	{
		key = GetSelectKeyAPI();
		if(key)
		{
			*keyMode = 1;
		}
	}

	if(key==0)
	{
		keyValue = ReadKeyValue();
		if(freeMode==0)
		{
			
			if(
				(keyValue == '1')||(keyValue == '2')||(keyValue == '3')||(keyValue == '4')
				||(keyValue == '5')||(keyValue == '6')||(keyValue == '7')
				||(keyValue == '8')||(keyValue == '9')||(keyValue == '0')||(keyValue == '.')||(keyValue == 'D')				
			  )
			{
				key = keyValue;
			} 
		}
		else
		{
			if(
					(keyValue == 'C')||(keyValue == '1')||(keyValue == '2')||(keyValue == '3')||(keyValue == '4')
					||(keyValue == '5')||(keyValue == '6')||(keyValue == '7')
					||(keyValue == '8')||(keyValue == '9')||(keyValue == '0')||(keyValue == '.')||(keyValue == 'D')	
					||(keyValue == 'E')
				  )
			{
				key = keyValue;
			}
		}
		if(key)
		{
			*keyMode = 2;
		}
		//Trace("\r\n btn=%d,%d",keyValue,key);
	}
	if(key)
	{
		TraceSelection("\r\n MiddKeykey3=%d,keymode=%d",key,*keyMode);
	}
		
	
	return key;
}

//查询游戏按键按下,1按下,0没按下
unsigned char ReadGameKeyValueAPI()
{
    //按下
	if(ReadGameKeyValue())
	{
		Buzzer();
		ButtonRPTAPI(0,0,0);//上报pc按键信息
		return 1;
	}
	else
		return 0;
}

/*********************************************************************************************************
** Function name:       SetTemperatureAPI
** Descriptions:        设置温度
** input parameters:    
                        temparater温度
** output parameters:   
** Returned value:      返回值0设置失败,1设置正确
*********************************************************************************************************/
unsigned char SetTemperatureAPI( unsigned int temparater )
{
	MessageXMTPack *AccepterMsg;
	unsigned char ComStatus;
	uint8_t key = 0;
	
	switch(SystemPara.XMTTemp)
	{
		case TEMPERATURE:
			MsgXMTPack.KeyCmd = MBOX_XMTSETTEMP;	
			MsgXMTPack.temparater = temparater;
			//Trace("\r\n g_XMTMail send=\r\n");
			OSMboxPost(g_XMTMail,&MsgXMTPack);
			//得到设置温度的结果
			AccepterMsg = OSMboxPend(g_XMTBackMail,200,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				//Trace("\r\n MiddKeykey1=%d,%d",AccepterMsg->KeyBackCmd,AccepterMsg->selectInput);
				if((AccepterMsg->KeyBackCmd)==MBOX_XMTTEMPOK)
				{	
					key = 1;	
				}
				else if((AccepterMsg->KeyBackCmd)==MBOX_XMTTEMPFAIL)
				{	
					key = 0;	
				}
			}
			break;
		default:break;	
	}
	return key;
}

