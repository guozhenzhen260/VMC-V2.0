/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           CHANGERACCEPTER
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        找零器驱动程序相关函数声明                     
**------------------------------------------------------------------------------------------------------
** Created by:          gzz
** Version:             V2.0
** Descriptions:        The original version
**------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
********************************************************************************************************/
#include "..\config.h"	
#include "MDBChanger.h"



//当前纸币器设备的类型
//uint8_t NowChangerDev = 0;
//纸币器通道面值
uint32_t nChangervalue[8]={0};
uint8_t NowChangerDevState = 0;

//接收纸币器通信应答数据包定时参数
//extern unsigned int MDBCONVERSATIONWAITACKTIMEOUT;





/*********************************************************************************************************
** Function name:       ChangeGetTubes
** Descriptions:        MDB硬币器余币量
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void ChangeGetTubes(void)
{
	uint8_t ChangerRdBuff[36],ChangerRdLen,ComStatus,j;
	memset(ChangerRdBuff,0,sizeof(ChangerRdBuff));	
	for(j = 0; j < 16; j++) 
	{
		 stDevValue.CoinNum[j]=0;
		 //TraceChange("\r\nqqDrvChangebuf[%d] = %d,%d", j,ChangerRdBuff[j+2],stDevValue.CoinNum[j]);
    }
	//if(NowChangerDev==2)
	{		
	   	ComStatus = MdbConversation(0x0A,NULL,0x00,&ChangerRdBuff[0],&ChangerRdLen);
		if(ComStatus == 1)
		{
			 for(j = 0; j < ChangerRdLen-2; j++) 
			 {
				 stDevValue.CoinNum[j] =  ChangerRdBuff[2+j];				 
				 //TraceChange("\r\n**DrvChangebuf[%d] = %d,%d", j, ChangerRdBuff[j+2],stDevValue.CoinNum[j]);
	         }
			 if(SystemPara.CoinChangerType == MDB_CHANGER)
			 {
				 TraceChange("%dDrvChangebuf=",ChangerRdLen);
				 for(j = 0; j < ChangerRdLen; j++) 
				 {
					 TraceChange("[%d]", stDevValue.CoinNum[j]);
		         }
				  TraceChange("\r\n");
			}
		}
	}
}

/*********************************************************************************************************
** Function name:       ChangePayoutProcessLevel3
** Descriptions:        level3找零操作
** input parameters:    PayMoney——需要找出的硬币金额
** output parameters:   
                        PayoutNum--各通道实际出币数量						
** Returned value:      有硬币找出返回1，无返回0
*********************************************************************************************************/
unsigned char ChangePayoutProcessLevel3(uint32_t PayMoney,unsigned char PayoutNum[16])
{
	unsigned char CoinRdBuff[36],CoinRdLen,ComStatus,VMCValue[2]={0},VMCPoll[1]={0};
	uint32_t coinscale,dispenseValue;
	uint8_t  i;
	uint8_t tempdispenseValue;
	
	memset(CoinRdBuff,0,sizeof(CoinRdBuff));
	//NowChangerDev = SystemPara.CoinChangerType;
	//Trace("6\r\n");	
	
	ChangeGetTubes();
	OSTimeDly(OS_TICKS_PER_SEC / 10);
	//stDevValue.CoinLevel = 2;
	/***************************************
	** level3级别，直接发送找零金额即可
	***************************************/
	//if(stDevValue.CoinLevel >= 3)
	{
		coinscale = stDevValue.CoinScale;
		dispenseValue = PayMoney / coinscale;//发送找零基准数量			
		TraceChange("\r\nDrvChangescale = %d,%d", coinscale,dispenseValue);
		while(dispenseValue>0)
		{
			tempdispenseValue=(dispenseValue>200)?200:dispenseValue;
			dispenseValue-=tempdispenseValue;
			TraceChange("\r\nDrvChangedispense = %d,%d", dispenseValue,tempdispenseValue);
			VMCValue[0] = 0x02;
			VMCValue[1] = tempdispenseValue;
			//1发送找币指令
			ComStatus = MdbConversation(0x0F,VMCValue,2,&CoinRdBuff[0],&CoinRdLen);
			if(ComStatus == 1)
			{
				Timer.PayoutTimer = tempdispenseValue/2 + 20;
				while(Timer.PayoutTimer)
				{					
					//2发送扩展poll指令，检测找币是否完成
					VMCPoll[0] = 0x04;
					ComStatus = MdbConversation(0x0F,VMCPoll,1,&CoinRdBuff[0],&CoinRdLen);
					TraceChange("\r\nDrvChangesend = %d,%d,%d,%d\r\n",ComStatus,CoinRdBuff[0],CoinRdBuff[1],CoinRdLen);
					//找零进行时，CoinRdLen=1 找零完成后，CoinRdLen = 0
					if( CoinRdLen == 0 )
					{
						memset(CoinRdBuff,0,sizeof(CoinRdBuff));
						//CoinRdLen = 0;
						//3发送扩展指令，检测本次找币各通道找多少枚
						VMCPoll[0] = 0x03;
						ComStatus = MdbConversation(0x0F,VMCPoll,1,&CoinRdBuff[0],&CoinRdLen);
						TraceChange("\r\nDrvChange s = %d,%d,%d,%d\r\n",ComStatus,CoinRdBuff[0],CoinRdBuff[1],CoinRdLen);
						if( CoinRdLen > 0 )
						{
							TraceChange("\r\nDrvChangeOut=%d,%d,%d,%d",CoinRdBuff[0],CoinRdBuff[1],CoinRdBuff[2],CoinRdBuff[3]);
							for(i = 0;i < CoinRdLen;i++)
							{
								PayoutNum[i] += CoinRdBuff[i];
							}
							for(i = 0; i < 16; i++) 
							{
								 TraceChange("\r\n%dDrvPayoutNum[%d] = %d", CoinRdLen,i,PayoutNum[i]);
							}
							
							
							break;
						}
					}
					OSTimeDly(OS_TICKS_PER_SEC / 100);
				}
			}	
			else
			{
				TraceChange("\r\nDrvChangesendFail");
			}
		}
	}		
	return 0;
}


/*********************************************************************************************************
** Function name:       ChangePayoutProcessLevel2
** Descriptions:        level2找零操作
** input parameters:    PayType——出币通道
**                      PayNum ——出币数量
** output parameters:   PayoutNum--各通道实际出币数量							
** Returned value:      有硬币找出返回1，无返回0
*********************************************************************************************************/
unsigned char ChangePayoutProcessLevel2(unsigned char PayType,unsigned char PayNum, unsigned char PayoutNum[8])
{
	unsigned char ComStatus,VMCPoll[1]={0},CoinRdBuff[36],CoinRdLen;
	
	//5出币
	VMCPoll[0]  =  PayType&0x0f;
	VMCPoll[0] |= ((PayNum<<4)&0xf0);
	ComStatus = MdbConversation(0x0D,VMCPoll,1,&CoinRdBuff[0],&CoinRdLen);
	//6上传通道的出币枚数
	if(ComStatus == 1)
	{
		PayoutNum[PayType] = PayNum;	
		TraceChange("\r\nDrvChange nu=%d",PayoutNum[PayType]);
	}	
	return 0;
}





