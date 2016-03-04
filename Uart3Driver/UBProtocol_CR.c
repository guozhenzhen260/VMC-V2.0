/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           UBProtocol.C
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        友宝驱动程序相关函数声明                     
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
#include "UBProtocol_CR.h"
#include "..\APP\CHANNEL.h"


uint8_t  decimalPlacesCR=0;//基本单位1角,2分,0元
/******************************
//将定义的结构体存放到RAM2的方法:
第一步:
#pragma arm section zidata = "RAM2"
xxxx
#pragma arm section zidata
表明这一段包括的所有东西都存放到RAM2中，其中RAM2名字是自己定义的
接下去的东西还是放回到原来的RAM中


第二步:编译器的管理工具->Linker->.\Firmware\SRAM_SCT.sct->Edit 打开文件
ROM_LOAD 0x00000000
{
    VECTOR 0x00000000//原先定义，表示RO编译到地址0x00000000中
    {
        vector_table.o (+RO)
    }
    ROM_EXEC +0
    {
        *.o (+RO)
    }

    SRAM 0x10000000//原先定义，表示RW,ZI编译到地址0x10000000中
    {
        * (+RW,+ZI)
    }

    //我们自己新增的定义，表示UBProtocol.o的文件编译到地址0x2007c000中,
    //芯片有两块16k的，0x2007c000的16k用来做网络
    //0x20080000的16k用来做usb
    //这里我们把他放到网络的RAM中
    //由于在第一步有定义，因此只有一小块放到RAM2中
    //RW_RAM2是我们自己起的名字,(RAM2)是第一步起的名字
	RW_RAM2 0x2007c000 0x00004000
	{
		*.o (RAM2)
	}

    HEAP +0 UNINIT
    {
        rt_sys_s.o (Heap)
    }
}

第三步:编译器的管理工具->Target
IRAM1:0x10000000     0x8000
IRAM1:0x2007C000     0x8000
打勾

已经完成了
第四步:看包括的这两个结构体是否放到RAM2中
.Map文件中可以看到
sysVPMissionCR                             0x2007c000   Data         560  ubprotocol.o(RAM2)
sysGoodsMatrix                           0x2007c230   Data         496  ubprotocol.o(RAM2)

********************************/
#pragma arm section zidata = "RAM2"

unsigned char g_msgCR[255]={0};
unsigned char VPMsgBufCR[255]={0};

struct VP_MissionCR sysVPMissionCR;

#pragma arm section zidata

uint8_t GoodsSNCR;//ack与nak的sn序列号
uint8_t issnup=0;//1时不升级sn,0时需要升级sn


/*********************************************************************************************************
** Function name:     	PackSNUpdate_CR
** Descriptions:	    更新SN
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void PackSNUpdate_CR()
{
	if(sysVPMissionCR.send.sn >= 255)
		sysVPMissionCR.send.sn = 0;
	else
		sysVPMissionCR.send.sn++;
}


/*********************************************************************************************************
** Function name:     	PackSNUpdate_CR
** Descriptions:	    更新SN
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void VPSerialInit_CR()
{
	uart3_clr_buf();
	memset( &sysVPMissionCR.send, 0, sizeof(sysVPMissionCR.send) );
	memset( &sysVPMissionCR.receive, 0, sizeof(sysVPMissionCR.receive) );
}


/*********************************************************************************************************
** Function name:     	calc_crc69
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned short  calc_crc69(unsigned char *msg, unsigned short len) 
{
    unsigned short i, j;
    unsigned short crc = 0;
    unsigned short current = 0;
    for(i=0;i<len;i++) 
    {
        current = msg[i] << 8;
        for(j=0;j<8;j++) 
        {
            if((short)(crc^current)<0)
                crc = (crc<<1)^0x1221;
            else 
                crc <<= 1; 
            current <<= 1; 
        }
    }
    return crc;
}

/*********************************************************************************************************
** Function name:     	crc_check
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned char VPBusTxMsg_CR( void )
{
	unsigned char  i   = 0;
	//unsigned char  data len = 0;
	//unsigned char  data sum = 0;
	//unsigned char xdata DataString[255];
	
	//1.caculate the CHK
	//sysVPMissionCR.datLen = sysVPMissionCR.len - 5;
      memset(g_msgCR,0x00,sizeof(g_msgCR));
	
	g_msgCR[0] = sysVPMissionCR.send.sf;
	g_msgCR[1] = sysVPMissionCR.send.len;
	g_msgCR[2] = sysVPMissionCR.send.verFlag;
	g_msgCR[3] = sysVPMissionCR.send.msgType;
	g_msgCR[4] = sysVPMissionCR.send.sn;
	for( i = 0; i < sysVPMissionCR.send.datLen; i++ )
	{
		g_msgCR[5+i] += sysVPMissionCR.send.msg[i]; 
	}
	
	sysVPMissionCR.send.chk = calc_crc69(g_msgCR,sysVPMissionCR.send.len);
	//2.send the message
	Uart3PutChar( sysVPMissionCR.send.sf );
	Uart3PutChar( sysVPMissionCR.send.len );
	Uart3PutChar( sysVPMissionCR.send.verFlag );
	Uart3PutChar( sysVPMissionCR.send.msgType );
	Uart3PutChar( sysVPMissionCR.send.sn );
	TracePC("\r\n Drvsend >> %02x,%02x,%02x,%02x,%02x",sysVPMissionCR.send.sf,sysVPMissionCR.send.len,sysVPMissionCR.send.verFlag,sysVPMissionCR.send.msgType,sysVPMissionCR.send.sn); 
    for( i=0; i<sysVPMissionCR.send.datLen; i++  )
	{
		Uart3PutChar( sysVPMissionCR.send.msg[i] );
		TracePC(",%02x",sysVPMissionCR.send.msg[i]);
	}
	Uart3PutChar( sysVPMissionCR.send.chk/256 );
    Uart3PutChar( sysVPMissionCR.send.chk%256 );
	TracePC(",%02x,%02x",sysVPMissionCR.send.chk/256,sysVPMissionCR.send.chk%256); 
	
	return 1;
}




/***********************************************************
vmc内部处理都是以分为单位，但是在上传给pc时需要转换

decimalPlacesCR=1以角为单位
	例如，需要上传给pc的是200分,
	200*10=2000
	2000/100=20角，即上传20角
	
decimalPlacesCR=2以分为单位
	例如，需要上传给pc的是200分,
	200*100=20000
	20000/100=200分，即上传200分	

decimalPlacesCR=0以元为单位
	例如，需要上传给pc的是200分,
	200=200
	200/100=2元，即上传2元	
***************************************************************/
uint32_t   MoneySend_CR(uint32_t sendMoney)
{
	uint32_t tempMoney;
	
	//公式2: 上传ScaledValue = ActualValue元/(10-decimalPlacesCR次方)
	if(decimalPlacesCR==1)
	{
		//tempMoney = sendMoney*10;
		//tempMoney = tempMoney/100;
		tempMoney = sendMoney/10;
	}
	else if(decimalPlacesCR==2)
	{
		tempMoney = sendMoney;
	}
	else if(decimalPlacesCR==0)
	{
		//tempMoney = sendMoney;
		//tempMoney = tempMoney/100;
		tempMoney = sendMoney/100;
	}
	//例如:600分=6元
	return tempMoney;
}
uint32_t   MoneySendInfo_CR(uint32_t sendMoney)
{
	uint32_t tempMoney;
	
	//公式2: 上传ScaledValue = ActualValue元/(10-decimalPlacesCR次方)
	if(decimalPlacesCR==1)
	{
		//tempMoney = sendMoney*10;
		//tempMoney = tempMoney/100;
		tempMoney = sendMoney/10;
	}
	else if(decimalPlacesCR==2)
	{
		tempMoney = sendMoney;
	}
	else if(decimalPlacesCR==0)
	{
		//tempMoney = sendMoney;
		//tempMoney = tempMoney/100;
		tempMoney = sendMoney/100;
	}
	//例如:600分=6元
	return tempMoney;
}

//通过小数点位数，得到需要上传给pc机多少的值
uint8_t SendCoinDem_CR()
{
    uint8_t DecimalNum;
	switch(SystemPara.DecimalNum) 
	{
		case 2://以分为单位
		 	  DecimalNum  = 1;  
			  break;
	    case 1://以角为单位
		  	  DecimalNum  = 10;  
			  break;
			  
		case 0://以元为单位
			  DecimalNum  = 100;  
			  break;
	}
	return DecimalNum;
}

/***********************************************************
vmc内部处理都是以分为单位，所以接收pc传下来的数据时需要转换

decimalPlacesCR=1以角为单位
	例如，pc下传是20角,
	20*100=2000
	2000/10=200分，即接收200分
	
decimalPlacesCR=2以分为单位
	例如，pc下传是200分,
	200*100=20000
	20000/100=200分，即接收200分	

decimalPlacesCR=0以元为单位
	例如，pc下传是2元,
	2*100=200
	200=200分，即接收200分
***************************************************************/
uint32_t   MoneyRec_CR(uint8_t recMoneyH,uint8_t recMoneyL)
{
	uint32_t tempMoney;
	
	tempMoney = recMoneyH*256+recMoneyL;
	//公式1:  ActualValue元 =下载ScaledValue*(10-decimalPlacesCR次方)
	if(decimalPlacesCR==1)
	{
		//tempMoney = tempMoney*100;
		//tempMoney = tempMoney/10;
		tempMoney = tempMoney*10;
	}
	else if(decimalPlacesCR==2)
	{
		tempMoney = tempMoney;
	}
	else if(decimalPlacesCR==0)
	{
		tempMoney = tempMoney*100;
		//tempMoney = tempMoney;
	}
	//例如:6元=600分	
	return tempMoney;
}




unsigned char Uart3pcGetChWhile_CR()
{
	uint8_t ch=0;
	while(Uart3BuffIsNotEmpty() == 0);	
	//1.接收数据
	ch = Uart3GetCh();	
	TracePC("[%02x]",ch);
	return ch;
}


/*********************************************************************************************************
** Function name:     	VPBusFrameUnPack_CR
** Descriptions:	    
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPBusFrameUnPack_CR( void )
{
	uint8_t datalen=0,dataindex=0,isft=0;
	unsigned char i=0, k=0, m=0;
	unsigned char len = 0;
	unsigned int  sum = 0;
	unsigned char reclen=0;
	

	/*
	while(Uart3BuffIsNotEmpty() == 1)
	{		
		for( i=0; i<sizeof( VPMsgBufCR )-1; i++ )
		{
			VPMsgBufCR[i] = VPMsgBufCR[i+1];
		}
		VPMsgBufCR[sizeof(VPMsgBufCR)-1] = Uart3GetCh();	
		reclen=1;
	}
	*/	
	
	//TracePC("\r\n Drv <<dat1");
	if(Uart3BuffIsNotEmpty())
	{
		OSTimeDly(20);//等待一段时间，等数据下发完毕
		//TracePC("\r\n Drv <<dat2");
		//1.寻找包头	
		while(Uart3BuffIsNotEmpty() == 1)
		{
			//TracePC("\r\n Drv <<dat3");
			if( Uart3GetCh() != VP_SF ) 
			{
				//TracePC("\r\n Drv <<dat4");
				continue;
			}
			else
			{
				TracePC("\r\n Drv <<dat=");
				VPMsgBufCR[i++]=VP_SF;
				TracePC("[%02x]",VPMsgBufCR[i-1]);	
				datalen=Uart3pcGetChWhile_CR();//length
				VPMsgBufCR[i++]=datalen;
				datalen-=5;
				VPMsgBufCR[i++]=Uart3pcGetChWhile_CR();//ver
				VPMsgBufCR[i++]=Uart3pcGetChWhile_CR();//MT	
				VPMsgBufCR[i++]=Uart3pcGetChWhile_CR();//SN	
				isft=1;
				break;
			}
		}
		//2.得到包的data和chk
		if(isft)
		{
			for(dataindex=0;dataindex<datalen+2;dataindex++)
			{
				VPMsgBufCR[i++]=Uart3pcGetChWhile_CR();
				reclen=1;
			}			
		}
	}
	
		
		
	
	if(reclen)
	{
		for( i=0; i<=sizeof(VPMsgBufCR)-7; i++ )
		{			
			
			//TracePC("\r\n Drv i=%d",i);
		    //Check the SF
			if( VPMsgBufCR[i] != VP_SF ) 
				continue;
			//TracePC("\r\n Drv rec=sf"); 
			//TracePC( "\n Bus Return Str = ");
			//for( j = 0; j < 32; j++ )
			//	Trace( " %02x", VPMsgBufCR[i+j] );
			//Check the len
			len = VPMsgBufCR[i+1];
			if( !(len >=5) ) 
			    continue;
					
			if( i+len+2 > sizeof( VPMsgBufCR ) ) 
			    break;	
			//TracePC("\r\n Drv rec=len"); 
			//Check the CHK
			sum = 0;
			sum = calc_crc69(VPMsgBufCR + i, len);
		//	if( (VPMsgBufCR[i+len] != sum/256)||(VPMsgBufCR[i+len+1] != sum%256)	)
		//	{
		//		continue;
		//   }
			//TracePC("\r\n Drv rec=chk"); 
			//OSTimeDly(20);
			//Check the message type
	        if( !((VPMsgBufCR[i+3]>=VP_MT_MIN_RECEIVE) && (VPMsgBufCR[i+3]<=VP_MT_MAX_RECEIVE)) )   
	            continue;  
			//TracePC("\r\n Drv rec=type"); 
			//OSTimeDly(20);
			//Save the message
			sysVPMissionCR.receive.sf      = VPMsgBufCR[i];
			sysVPMissionCR.receive.len     = VPMsgBufCR[i+1];
	        sysVPMissionCR.receive.verFlag = VPMsgBufCR[i+2];
	 	    sysVPMissionCR.receive.msgType = VPMsgBufCR[i+3];
			sysVPMissionCR.receive.sn      = VPMsgBufCR[i+4];
	        sysVPMissionCR.receive.datLen  = sysVPMissionCR.receive.len - 5;
			TracePC("\r\n Drv <<sf=%02x,len=%02x,mt=[%02x],data=",sysVPMissionCR.receive.sf,sysVPMissionCR.receive.len,sysVPMissionCR.receive.msgType); 
			OSTimeDly(20);
			for( m=0,k=i+5; k<i+5+sysVPMissionCR.receive.datLen; m++,k++)
			{			
				sysVPMissionCR.receive.msg[m] = VPMsgBufCR[k];	
				//TracePC("\r\n Drv k=%d,m=%d,k<%d,msg=%d",k,m,(5+sysVPMissionCR.receive.datLen),sysVPMissionCR.receive.msg[m]);
				TracePC(" [%02x]",sysVPMissionCR.receive.msg[m]);
			}
			//TracePC("\r\n Drv rec2=%02x,%02x,%02x,%02x,%02x",sysVPMissionCR.receive.msg[0],sysVPMissionCR.receive.msg[1],sysVPMissionCR.receive.msg[2],sysVPMissionCR.receive.msg[3],sysVPMissionCR.receive.msg[4]); 
			OSTimeDly(20);
			sysVPMissionCR.receive.chk = sysVPMissionCR.receive.msg[k]*256 + sysVPMissionCR.receive.msg[k+1];
			memset( VPMsgBufCR, 0, sizeof(VPMsgBufCR) );
			//TracePC("\r\n Drv rec3=%02x,%02x,%02x,%02x,%02x",sysVPMissionCR.receive.sf,sysVPMissionCR.receive.len,sysVPMissionCR.receive.verFlag,sysVPMissionCR.receive.msgType,sysVPMissionCR.receive.sn); //OSTimeDly(20);
			//TracePC(",%02x,%02x",sysVPMissionCR.receive.msg[k],sysVPMissionCR.receive.msg[k+1]);
			return 1;			
		}	
	}
	return 0;
}

/*********************************************************************************************************
** Function name:     	VPMsgPackSend_CR
** Descriptions:	    组装报文发送数据
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMsgPackSend_CR( unsigned char msgType, unsigned char flag )
{
    
      uint8_t i=0,j=0,k=0,index=0,tempcan=0,dex=0,dexind=0,strdex[20] = {0};	
	uint16_t tempMoney;
	uint8_t tempSend=0;
	uint32_t tradeMoney=0;
	uint8_t cabinet=0,cabno=0;
	

    
    
	if( msgType>VP_MT_MAX_SEND )
		return VP_ERR_PAR;
    if(flag>1)
	    return VP_ERR_PAR;

	switch( msgType )
	{
		case VP_TEXT_MSG:
			{
			}
			break;
		case VP_ACK_RPT:
            {
				sysVPMissionCR.send.msgType = VP_ACK_RPT;
                sysVPMissionCR.send.datLen = 0;
				issnup=1;
			}
			break;
		case VP_NAK_RPT:
		    {
				sysVPMissionCR.send.msgType = VP_NAK_RPT;
                sysVPMissionCR.send.datLen = 0;
				issnup=1;
			}
			break;
		case VP_POLL:
		    {
			    sysVPMissionCR.send.msgType = VP_POLL;
                sysVPMissionCR.send.datLen = 0;
			}
			break;
		case VP_VMC_SETUP:
		    {
			    sysVPMissionCR.send.msgType = VP_VMC_SETUP;                
                	    i = 0;				
				sysVPMissionCR.send.msg[i++]  = 0;
				sysVPMissionCR.send.msg[i++]  = 0;
				sysVPMissionCR.send.msg[i++]  = 0;
				sysVPMissionCR.send.msg[i++]  = 0;
				sysVPMissionCR.send.msg[i++]  = 0;
				sysVPMissionCR.send.msg[i++]  = 0;
				
				sysVPMissionCR.send.datLen = i;      
				
			}
			break;
		case VP_STATUS_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_STATUS_RPT;
                	    i=0;

				tempSend |= sysVPMissionCR.vmc_st;
				tempSend |= sysVPMissionCR.cc_st<<2;
				tempSend |= sysVPMissionCR.check_st<<4;
				tempSend |= sysVPMissionCR.bv_st<<6;				
				sysVPMissionCR.send.msg[i++]  = tempSend;
				
				tempMoney = MoneySend_CR(sysVPMissionCR.change);
	                sysVPMissionCR.send.msg[i++] = tempMoney/256;
	                sysVPMissionCR.send.msg[i++] = tempMoney%256;
				
				tempMoney = MoneySend_CR(sysVPMissionCR.recychange);
				if(tempMoney>0)
				{
					if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)//富士用ffff代替
					{
			                sysVPMissionCR.send.msg[i++] = 0xff;
			                sysVPMissionCR.send.msg[i++] = 0xff;	
					}
					else
					{
			                sysVPMissionCR.send.msg[i++] = tempMoney/256;
			                sysVPMissionCR.send.msg[i++] = tempMoney%256;	
					}
				}
				else
				{
		                sysVPMissionCR.send.msg[i++] = 0;
		                sysVPMissionCR.send.msg[i++] = 0;	
				}
				//用户余额
				tempMoney = MoneySend_CR(GetAmountMoney());
	                   sysVPMissionCR.send.msg[i++] = tempMoney/256;
	                   sysVPMissionCR.send.msg[i++] = tempMoney%256;
			      //暂存余额
				tempMoney = MoneySend_CR(GetHoldMoney());
	                   sysVPMissionCR.send.msg[i++] = tempMoney/256;
	                   sysVPMissionCR.send.msg[i++] = tempMoney%256;
				sysVPMissionCR.send.datLen  = i;
			}
			break;
		case VP_INFO_RPT://120419 by cq TotalSell 
			{
                		sysVPMissionCR.send.msgType = VP_INFO_RPT; 
				i=0;		
				sysVPMissionCR.send.msg[i++]  = sysVPMissionCR.type;				
				switch(sysVPMissionCR.type)
				{
					case VP_INFO_ERR:
						//找零器状态
						//MDB找零器
						if(SystemPara.CoinChangerType==MDB_CHANGER)
						{
							if(DeviceStateBusiness.CoinCommunicate)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbf;
							}
							else if(DeviceStateBusiness.Coinsensor)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbc;
							}
							else if(DeviceStateBusiness.Cointubejam)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbe;
							}
							else if(DeviceStateBusiness.Coinromchk)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbd;
							}							
							else if(DeviceStateBusiness.CoinunknowError)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbf;
							}								
							
						}
						//Hopper找零器
						else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
						{
							if(SystemPara.HopperValue[0]==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x0b;
								sysVPMissionCR.send.msg[i++] = 0xbf;
							}
							else
							{	
								if(DeviceStateBusiness.Hopper1State==2)
								{
									sysVPMissionCR.send.msg[i++] = 0x0b;
									sysVPMissionCR.send.msg[i++] = 0xbe;
								}
							}							
						}						
													
						//5.硬币器模块	
						if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)				
						{															
							if(DeviceStateBusiness.Coinsensor)
							{
								sysVPMissionCR.send.msg[i++] = 0x13;
								sysVPMissionCR.send.msg[i++] = 0x8b;
							}
							else if(DeviceStateBusiness.Coinromchk)
							{
								sysVPMissionCR.send.msg[i++] = 0x13;
								sysVPMissionCR.send.msg[i++] = 0x8e;
							}
							else if(DeviceStateBusiness.Coinrouting)
							{
								sysVPMissionCR.send.msg[i++] = 0x13;
								sysVPMissionCR.send.msg[i++] = 0x8f;
							}							
							else if(DeviceStateBusiness.Coinjam)
							{
								sysVPMissionCR.send.msg[i++] = 0x13;
								sysVPMissionCR.send.msg[i++] = 0x8d;
							}
							else if(DeviceStateBusiness.CoinremoveTube)
							{
								sysVPMissionCR.send.msg[i++] = 0x13;
								sysVPMissionCR.send.msg[i++] = 0x8c;
							}							
						}
						//脉冲硬币器
						else if((SystemPara.CoinAcceptorType == PARALLEL_COINACCEPTER)||(SystemPara.CoinAcceptorType == SERIAL_COINACCEPTER))
						{							
						}
						
													
						//7.纸币器模块
						if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)					
						{
							if(DeviceStateBusiness.BillCommunicate)
							{
								sysVPMissionCR.send.msg[i++] = 0x0f;
								sysVPMissionCR.send.msg[i++] = 0xa7;
							}
							else if(DeviceStateBusiness.Billmoto)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x73;
							}
							else if(DeviceStateBusiness.Billsensor)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x74;
							}
							else if(DeviceStateBusiness.Billromchk)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x75;
							}
							else if(DeviceStateBusiness.Billjam)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x76;
							}
							else if(DeviceStateBusiness.BillremoveCash)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x77;
							}
							else if(DeviceStateBusiness.BillcashErr)
							{
								sysVPMissionCR.send.msg[i++] = 0x17;
								sysVPMissionCR.send.msg[i++] = 0x78;
							}								
						}	
						//纸币找零模块
						if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
						{
							if(DeviceStateBusiness.recyErr)
							{
								sysVPMissionCR.send.msg[i++] = 0x0f;
								sysVPMissionCR.send.msg[i++] = 0xa6;
							}
						}
						else if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)
						{
						}
						break;
					case VP_INFO_TOTALVALUE:
						//Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
						//by gzz 20110721						
						tempMoney = MoneySend_CR(GetAmountMoney());
						//tempMoney = sysVPMissionCR.payAllMoney;
				                sysVPMissionCR.send.msg[i++] = tempMoney/256;
				                sysVPMissionCR.send.msg[i++] = tempMoney%256;
						break;	
					case VP_INFO_VER:
						//easivend-duligui-Jun 14 2009 
						sysVPMissionCR.send.msg[i++]  = 'e'; 	 
						sysVPMissionCR.send.msg[i++]  = 'a'; 	 
						sysVPMissionCR.send.msg[i++]  = 's'; 	 
						sysVPMissionCR.send.msg[i++]  = 'i'; 	
						sysVPMissionCR.send.msg[i++]  = 'v'; 
						sysVPMissionCR.send.msg[i++]  = 'e'; 	 
						sysVPMissionCR.send.msg[i++]  = 'n'; 	 
						sysVPMissionCR.send.msg[i++]  = 'd';
						sysVPMissionCR.send.msg[i++]  = '-'; 	 
						sysVPMissionCR.send.msg[i++]  = 'd'; 	 
						sysVPMissionCR.send.msg[i++]  = 'u';
						sysVPMissionCR.send.msg[i++]  = 'l'; 	 
						sysVPMissionCR.send.msg[i++]  = 'i'; 	 
						sysVPMissionCR.send.msg[i++]  = 'g'; 	 
						sysVPMissionCR.send.msg[i++]  = 'u';
						sysVPMissionCR.send.msg[i++]  = 'i'; 	 
						sysVPMissionCR.send.msg[i++]  = '-'; 	 
						sysVPMissionCR.send.msg[i++]  = 'j';
						sysVPMissionCR.send.msg[i++]  = 'u';
						sysVPMissionCR.send.msg[i++]  = 'n'; 	 
						sysVPMissionCR.send.msg[i++]  = ' '; 
						dexind=sprintf(strdex,"%s",__DATE__);
						for(dex=0;dex<dexind;dex++)
							sysVPMissionCR.send.msg[i++]  = strdex[dex]; 
						TracePC("\r\n Drv Uboxdata=%s",__DATE__ );
						break;
					case VP_INFO_HARD:						
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[0]; 	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[1];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[2];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[3];
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[4];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[5];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[6]; 	
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[7];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[8];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[9];
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[10];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[11];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[12]; 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[13];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[14];	 
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[15];
						sysVPMissionCR.send.msg[i++]  = stMacSn.id[16];			
						
						break;	
					case VP_INFO_BILL:
						if(SystemPara.BillValidatorType>OFF_BILLACCEPTER)					
						{
							for(dex=0;dex<29;dex++)
								sysVPMissionCR.send.msg[i++]  = stDevValue.billIDENTITYBuf[dex];  
						}
						break;
					case VP_INFO_COIN:
						if(SystemPara.CoinAcceptorType>OFF_COINACCEPTER)					
						{
							sysVPMissionCR.send.msg[i++]  = 'e'; 	 
							sysVPMissionCR.send.msg[i++]  = 'a'; 	 
							sysVPMissionCR.send.msg[i++]  = 's'; 	 
							sysVPMissionCR.send.msg[i++]  = 'i';
							sysVPMissionCR.send.msg[i++]  = 'e'; 	
							sysVPMissionCR.send.msg[i++]  = 'v'; 
							sysVPMissionCR.send.msg[i++]  = 'e'; 	 
							sysVPMissionCR.send.msg[i++]  = 'n'; 	 
							sysVPMissionCR.send.msg[i++]  = 'd';
							sysVPMissionCR.send.msg[i++]  = '-';
							sysVPMissionCR.send.msg[i++]  = 'c'; 	 
							sysVPMissionCR.send.msg[i++]  = 'o'; 	 
							sysVPMissionCR.send.msg[i++]  = 'i'; 	 
							sysVPMissionCR.send.msg[i++]  = 'n';
						}
						break;
					case VP_INFO_COINOUT:
						if(SystemPara.CoinChangerType>OFF_CHANGER)					
						{
							sysVPMissionCR.send.msg[i++]  = 'e'; 	 
							sysVPMissionCR.send.msg[i++]  = 'a'; 	 
							sysVPMissionCR.send.msg[i++]  = 's'; 	 
							sysVPMissionCR.send.msg[i++]  = 'i';
							sysVPMissionCR.send.msg[i++]  = 'e'; 	
							sysVPMissionCR.send.msg[i++]  = 'v'; 
							sysVPMissionCR.send.msg[i++]  = 'e'; 	 
							sysVPMissionCR.send.msg[i++]  = 'n'; 	 
							sysVPMissionCR.send.msg[i++]  = 'd';
							sysVPMissionCR.send.msg[i++]  = '-'; 
							sysVPMissionCR.send.msg[i++]  = 'c'; 	 
							sysVPMissionCR.send.msg[i++]  = 'o'; 	 
							sysVPMissionCR.send.msg[i++]  = 'i'; 	 
							sysVPMissionCR.send.msg[i++]  = 'n';
							sysVPMissionCR.send.msg[i++]  = 'o'; 	 
							sysVPMissionCR.send.msg[i++]  = 'u'; 	 
							sysVPMissionCR.send.msg[i++]  = 't';
						}
						break;
					case VP_INFO_BILLOUT:
						if(SystemPara.BillRecyclerType>OFF_BILLRECYCLER)					
						{
							if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
							{
								for(dex=0;dex<29;dex++)
									sysVPMissionCR.send.msg[i++]  = stDevValue.billIDENTITYBuf[dex];  
							}
							else if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)
							{
								sysVPMissionCR.send.msg[i++]  = 'F'; 	 
								sysVPMissionCR.send.msg[i++]  = 'S'; 
								sysVPMissionCR.send.msg[i++]  = '_'; 
								sysVPMissionCR.send.msg[i++]  = 'b'; 	 
								sysVPMissionCR.send.msg[i++]  = 'i'; 	 
								sysVPMissionCR.send.msg[i++]  = 'l'; 	 
								sysVPMissionCR.send.msg[i++]  = 'l';
							}
						}
						break;	
				}	
				sysVPMissionCR.send.datLen  = i;
				break;
			}
		case VP_PAYIN_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_PAYIN_RPT;
			    i=0;
				sysVPMissionCR.send.msg[i++] = sysVPMissionCR.payInDev;
				TracePC("\r\n Drv Uboxbill=%d,all=%ld",sysVPMissionCR.payInMoney,sysVPMissionCR.payAllMoney); 				
				tempMoney = MoneySend_CR(sysVPMissionCR.payInMoney);
				sysVPMissionCR.send.msg[i++] = tempMoney/256;
				sysVPMissionCR.send.msg[i++] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
			    //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
			    sysVPMissionCR.send.msg[i++] = tempMoney/256;
			    sysVPMissionCR.send.msg[i++] = tempMoney%256;
			    sysVPMissionCR.send.datLen = i;      //3,5
			}
			break;
		case VP_PAYOUT_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_PAYOUT_RPT;
				i=0;					 
                sysVPMissionCR.send.msg[i++] = sysVPMissionCR.type;
				tempMoney = MoneySend_CR(sysVPMissionCR.payoutMoney);
				sysVPMissionCR.send.msg[i++] = tempMoney/256;
				sysVPMissionCR.send.msg[i++] = tempMoney%256; 
				tempMoney = MoneySend_CR(sysVPMissionCR.payremainMoney);
                sysVPMissionCR.send.msg[i++] = tempMoney/256;
                sysVPMissionCR.send.msg[i++] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721				
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[i++] = tempMoney/256;
                sysVPMissionCR.send.msg[i++] = tempMoney%256;
				sysVPMissionCR.send.datLen = i;  
			}
			break;	
		case VP_COST_RPT://扣款操作;by gzz 20110823
		    {
			    sysVPMissionCR.send.msgType = VP_COST_RPT;
				i=0;				
               	      tempMoney = MoneySend_CR(sysVPMissionCR.costMoney);
				sysVPMissionCR.send.msg[i++] = tempMoney/256;
				sysVPMissionCR.send.msg[i++] = tempMoney%256;                 
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721				
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
	                sysVPMissionCR.send.msg[i++] = tempMoney/256;
	                sysVPMissionCR.send.msg[i++] = tempMoney%256;
				sysVPMissionCR.send.datLen = i;   
			}
			break;	
		case VP_ACTION_RPT:	
			{
				sysVPMissionCR.send.msgType = VP_ACTION_RPT;	
				i=0;		
				sysVPMissionCR.send.msg[i++]  = sysVPMissionCR.action;	
				sysVPMissionCR.send.datLen  = i;
			}				
			break; 
		case VP_BUTTON_RPT:	
			{
				sysVPMissionCR.send.msgType = VP_BUTTON_RPT;	
				i=0;		
				sysVPMissionCR.send.msg[i++]  = 0;	
				sysVPMissionCR.send.datLen  = i;
			}				
			break; 	
		case VP_RESET_RPT:
			{
				sysVPMissionCR.send.msgType = VP_RESET_RPT;	
				i=0;		
				sysVPMissionCR.send.datLen  = i;
			}
			break;
			
		default: break;
	}
	//	
	sysVPMissionCR.send.sf  = VP_SF;
	
	//
    sysVPMissionCR.send.len = sysVPMissionCR.send.datLen + 5;
	//
    sysVPMissionCR.send.verFlag = VP_PROTOCOL_VER;
	if( flag == 1 )
	{
	    sysVPMissionCR.send.verFlag += VP_PROTOCOL_ACK;
	}
	else
	{
		sysVPMissionCR.send.verFlag += VP_PROTOCOL_NAK;
	}
	
	
	if(issnup==0)
	{
		//更新SN流水号			
		PackSNUpdate_CR();
		GoodsSNCR=sysVPMissionCR.send.sn;
	}
	else
	{
		sysVPMissionCR.send.sn=GoodsSNCR;
		issnup=0;
	}
	
	
    VPBusTxMsg_CR();    
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMission_Setup_RPT_CR
** Descriptions:	    启动时上报配置信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Setup_RPT_CR( void )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

    retry = VP_COM_RETRY;   
	flag = VPMsgPackSend_CR( VP_VMC_SETUP,0);
	//DisplayStr( 0, 0, 1, "2", 1 );  
	//WaitForWork(2000,NULL);
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}

	/*while( retry )
	{
		Timer.PCRecTimer = VP_TIME_OUT;
		while( Timer.PCRecTimer )
		{
			//DisplayStr( 0, 0, 1, "3", 1 );  
			//WaitForWork(1000,NULL);
			if( VPBusFrameUnPack_CR() )
			{
				//DisplayStr( 0, 0, 1, "4", 1 );  
				//WaitForWork(2000,NULL);
			    //sysVPMissionCR.comErrNum = 0;
				recRes = 1;
				break;				
			}
		}
		if(Timer.PCRecTimer==0)
		{
			retry--;
			//TracePC("\r\n Drv failretry=%d",retry); 
		}	
		if(recRes)
		{
			break;
		}
	}	
	if( retry== 0 )
	{
		//sysVPMissionCR.comErrNum++;
		//
        return VP_ERR_COM;
	}
    switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	*/
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMission_Status_RPT_CR
** Descriptions:	    上报机器状态
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Status_RPT_CR()
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
	uint32_t change;
	
    retry = VP_COM_RETRY;	
	
	sysVPMissionCR.bv_st = ErrorStatus(2);
	sysVPMissionCR.check_st = ErrorStatus(2);
	sysVPMissionCR.cc_st = ErrorStatus(3);
	sysVPMissionCR.vmc_st = ErrorStatus(1);
	if(SystemPara.CoinChangerType == MDB_CHANGER)
	{
		change = stDevValue.CoinValue[0]*stDevValue.CoinNum[0] + stDevValue.CoinValue[1]*stDevValue.CoinNum[1] + stDevValue.CoinValue[2]*stDevValue.CoinNum[2]
				+stDevValue.CoinValue[3]*stDevValue.CoinNum[3] + stDevValue.CoinValue[4]*stDevValue.CoinNum[4] + stDevValue.CoinValue[5]*stDevValue.CoinNum[5] 
				+stDevValue.CoinValue[6]*stDevValue.CoinNum[6] + stDevValue.CoinValue[7]*stDevValue.CoinNum[7]
				+stDevValue.CoinValue[8]*stDevValue.CoinNum[8] + stDevValue.CoinValue[9]*stDevValue.CoinNum[9] + stDevValue.CoinValue[10]*stDevValue.CoinNum[10]
				+stDevValue.CoinValue[11]*stDevValue.CoinNum[11] + stDevValue.CoinValue[12]*stDevValue.CoinNum[12] + stDevValue.CoinValue[13]*stDevValue.CoinNum[13]
				+stDevValue.CoinValue[14]*stDevValue.CoinNum[14] + stDevValue.CoinValue[15]*stDevValue.CoinNum[15]; 
	}
	else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
	{
		change =( !HopperIsEmpty() )? SystemPara.MaxValue:0;
		if(change < SystemPara.BillEnableValue)
		{
			change  = 0;
		}
		else
		{
			change  = 6553500;
		}
		//MsgUboxPack[g_Ubox_Index].change  =( !HopperIsEmpty() )? SystemPara.MaxValue:0;				
	}
	//无硬币器
	else if(SystemPara.CoinChangerType == OFF_CHANGER)
	{	
		change =0;
	}
	sysVPMissionCR.change=change;
	
	//添加纸币循环器时，可找硬币金额
	//if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
	//{
	//	MsgUboxPack[g_Ubox_Index].change += stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
	//			+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
	//			+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
	//}
	//添加纸币循环器时，纸币的存币数量
	if(SystemPara.BillRecyclerType==MDB_BILLRECYCLER)
	{
		change = stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
				+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
				+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
	}
	else if(SystemPara.BillRecyclerType==FS_BILLRECYCLER)
	{
		change = stDevValue.RecyclerValue[0]*stDevValue.RecyclerNum[0] + stDevValue.RecyclerValue[1]*stDevValue.RecyclerNum[1] + stDevValue.RecyclerValue[2]*stDevValue.RecyclerNum[2]
				+stDevValue.RecyclerValue[3]*stDevValue.RecyclerNum[3] + stDevValue.RecyclerValue[4]*stDevValue.RecyclerNum[4] + stDevValue.RecyclerValue[5]*stDevValue.RecyclerNum[5]
				+stDevValue.RecyclerValue[6]*stDevValue.RecyclerNum[6];
	}
	else
	{
		change =0;
	}
	sysVPMissionCR.recychange = change;
	
	//==============================================================
	flag = VPMsgPackSend_CR( VP_STATUS_RPT, 0);  //1-0, not need ACK
      if( flag != VP_ERR_NULL )
      {
		return VP_ERR_PAR;
	}	
	return VP_ERR_NULL;

}


/*********************************************************************************************************
** Function name:     	VPMission_Info_RPT_CR
** Descriptions:	    上报Info信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Info_RPT_CR(uint8_t type)
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

	if(LogPara.offLineFlag == 1)
	{
		return VP_ERR_NULL;
	}
	//
    retry = VP_COM_RETRY;
	//-------------------------------------------
    sysVPMissionCR.type = type;		
	//===========================================
	flag = VPMsgPackSend_CR( VP_INFO_RPT, 0);
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}
	
	return VP_ERR_NULL;
}


/*********************************************************************************************************
** Function name:     	VP_Control_Ind_CR
** Descriptions:	    PC指示VMC控制设备的命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Control_Ind_CR( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
		
	//发送邮箱给vmc
    MsgUboxPack[g_Ubox_Index].Type  = sysVPMissionCR.receive.msg[0];
	//sysVPMissionCR.ctrValue = sysVPMissionCR.receive.msg[1];
    switch(MsgUboxPack[g_Ubox_Index].Type)
    {    
    	case VP_CONTROL_BILLCOIN:
		MsgUboxPack[g_Ubox_Index].value  = sysVPMissionCR.receive.msg[1];
		TracePC("\r\n Drv CtrBillcoin"); 
		break;		
    }
	//TracePC("\r\n Drv Ctr=%d,%d",MsgUboxPack.Type,sysVPMissionCR.receive.msg[1]); 
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_CONTROLIND;				
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeSet(0);
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv %dControl_Ind",OSTimeGet()); 
	
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:       VPMission_Act_RPT_CR
** Descriptions:        向PC上报Action信息
** input parameters:    status=0出货成功,2，出货失败
                        column实际出货货道  
                        type出货类型
                        cost商品的花费
                        payAllMoney出货完成后，用户投币余额
                        columnLeft本货道剩余商品个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMission_Act_RPT_CR( unsigned char action)
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
    
	
	
    retry = VP_COM_RETRY;
	//-------------------------------------------
      sysVPMissionCR.action   = action;	
	//===========================================
		
    flag = VPMsgPackSend_CR( VP_ACTION_RPT, 0);   
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}	
	return VP_ERR_NULL;
}


/*********************************************************************************************************
** Function name:     	VPMission_Button_RPT_CR
** Descriptions:	    上报按键信息
** input parameters:    type=1小键盘选择货道,4退币,0游戏按键
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Button_RPT_CR()
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

	if(LogPara.offLineFlag == 1)
	{
		//if(type==VP_BUT_GAME)//游戏按键蜂鸣器响;by gzz 20110721
	    //{   
	    	//Buzzer();
	    //}
		return VP_ERR_NULL;
	}
	//
    retry = VP_COM_RETRY;
    //-------------------------------------------  
	//1-0: button message, not need ACK
    flag = VPMsgPackSend_CR( VP_BUTTON_RPT, 0);   
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}	
	return VP_ERR_NULL;

}


/*********************************************************************************************************
** Function name:     	VPMission_Payin_RPT
** Descriptions:	    上报投币信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Payin_RPT_CR(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney)
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

	//
    retry = VP_COM_RETRY;
	//-------------------------------------------
	switch(dev)
	{
		case 1:
			sysVPMissionCR.payInDev = VP_DEV_COIN;
			break;
		case 2:
			sysVPMissionCR.payInDev = VP_DEV_BILL;
			break;
		case 3:
			sysVPMissionCR.payInDev = VP_DEV_ESCROWIN;
			break;
		case 4:
			sysVPMissionCR.payInDev = VP_DEV_ESCROWOUT;
			break;				
			
	}    
	sysVPMissionCR.payInMoney = payInMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
		
	//===========================================
	flag = VPMsgPackSend_CR( VP_PAYIN_RPT, 0);
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}	
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VP_Control_Ind_CR
** Descriptions:	    PC指示VMC控制设备的命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Payin_Ind_CR( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
		
	//发送邮箱给vmc
    MsgUboxPack[g_Ubox_Index].Type  = sysVPMissionCR.receive.msg[0];
	//sysVPMissionCR.ctrValue = sysVPMissionCR.receive.msg[1];
    
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_PAYININD;				
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeSet(0);
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv %dPayin_Ind",OSTimeGet()); 
	
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VPMission_Cost_RPT_CR
** Descriptions:	    上报扣款信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Cost_RPT_CR( unsigned char Type, uint32_t costMoney, unsigned int payAllMoney )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
	unsigned char ressend=1;

	 
	//-------------------------------------------
    sysVPMissionCR.type = Type;
	sysVPMissionCR.costMoney = costMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
	//===========================================
	while(ressend)
	{
	    retry = VP_COM_RETRY;
	    flag = VPMsgPackSend_CR( VP_COST_RPT, 1);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
		while( retry )
		{
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if( VPBusFrameUnPack_CR() )
				{
					recRes = 1;
					break;
				}
			}
			if(Timer.PCRecTimer==0)
			{
				retry--;
				//TracePC("\r\n Drv failretry=%d",retry); 
			}	
			if(recRes)
			{
				break;
			}
			
		}
		if( retry== 0 )
		{
			OSTimeDly(10);	
			issnup=1;
		}
		else
		{
			ressend=0;			
		}
	}
    switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VP_Cost_Ind_CR
** Descriptions:	    PC指示VMC扣款命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Cost_Ind_CR( void )
{
//	MessageUboxPCPack *AccepterUboxMsg;
//	unsigned char ComStatus;
	//u_char xdata len = 0;
	//u_char xdata str[20];
//	unsigned char ComReturn = 0;
		
    sysVPMissionCR.costMoney = MoneyRec_CR(sysVPMissionCR.receive.msg[0],sysVPMissionCR.receive.msg[1]);
		
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_COSTIND;				
	MsgUboxPack[g_Ubox_Index].costMoney = sysVPMissionCR.costMoney;	
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv Cost_Ind"); 
	
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VP_Payout_Ind_CR
** Descriptions:	    PC指示VMC出币命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Payout_Ind_CR( void )
{	 	
	//MessageUboxPCPack *AccepterUboxMsg;
//	unsigned char ComStatus;
   	sysVPMissionCR.type = sysVPMissionCR.receive.msg[0];    
	sysVPMissionCR.changeMoney = MoneyRec_CR(sysVPMissionCR.receive.msg[1],sysVPMissionCR.receive.msg[2]);
	
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_PAYOUTIND;				
	MsgUboxPack[g_Ubox_Index].changeMoney = sysVPMissionCR.changeMoney;
	MsgUboxPack[g_Ubox_Index].Type        = sysVPMissionCR.type;
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv Payout_Ind"); 
	
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VPMission_Payout_RPT_CR
** Descriptions:	    上报找零信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Payout_RPT_CR( uint8_t payoutDev,unsigned char Type, unsigned int payoutMoney,  unsigned int payremainMoney, unsigned int payAllMoney )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
	unsigned char ressend=1;

	
    //-------------------------------------------
    sysVPMissionCR.payoutDev = payoutDev;
    sysVPMissionCR.type = Type;
	sysVPMissionCR.payoutMoney = payoutMoney;
	sysVPMissionCR.payremainMoney=payremainMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
	//===========================================	
	while(ressend)
	{
		retry = VP_COM_RETRY;
		flag = VPMsgPackSend_CR( VP_PAYOUT_RPT, 1);
		 if( flag != VP_ERR_NULL )
	      {
			return VP_ERR_PAR;
		}
		while( retry )
		{
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if( VPBusFrameUnPack_CR() )
				{
					recRes = 1;
					break;
				}
			}
			if(Timer.PCRecTimer==0)
			{
				retry--;
				//TracePC("\r\n Drv failretry=%d",retry); 
			}	
			if(recRes)
			{
				break;
			}
			
		}
		if( retry== 0 )
		{
			OSTimeDly(10);	
			issnup=1;
		}
		else
		{
			ressend=0;			
		}
	}
      switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	return VP_ERR_NULL;
}



/*********************************************************************************************************
** Function name:     	VP_Reset_Ind_CR
** Descriptions:	    PC指示VMC重新复位命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Reset_Ind_CR( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
       //暂存退币
	if(GetHoldMoney())
	{
		if(ReturnBillDevMoneyInAPI())
		{
			VPMission_Payin_RPT_CR(4,GetHoldMoney(),GetAmountMoney()-GetHoldMoney());//上报PC端
			ClearHoldMoney();			
		}
		TracePC("\r\n AppHoldRet%d",OSTimeGet());
		OSTimeDly(OS_TICKS_PER_SEC/2);
	}
	VP_Reset_Rpt_CR();
	MdbBusHardwareReset();
      OSTimeDly(OS_TICKS_PER_SEC*2);
	zyReset(ZY_HARD_RESET);
    //发送邮箱给vmc
	//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_RESETIND;				
	//OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	//UpdateIndex();
	//OSTimeDly(OS_TICKS_PER_SEC/10);
	//TracePC("\r\n Drv Reset_Ind"); 	
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VP_Reset_Rpt_CR
** Descriptions:	    VMC上报重新复位命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Reset_Rpt_CR( void )
{
	unsigned char flag = 0;
	
	flag = VPMsgPackSend_CR( VP_RESET_RPT, 0);
	 if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}
	
	return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VPMission_Poll_CR
** Descriptions:	    轮询PC机
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Poll_CR()
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

		

	retry = VP_COM_RETRY;	

	while( retry )
	{
		flag = VPMsgPackSend_CR( VP_POLL, 1);
		//DisplayStr( 0, 0, 1, "2", 1 );  
		//WaitForWork(2000,NULL);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
		Timer.PCRecTimer = VP_TIME_OUT;
		while( Timer.PCRecTimer )
		{
			//DisplayStr( 0, 0, 1, "System 1", 10 );  
			//WaitForWork(2000,NULL);
			if( VPBusFrameUnPack_CR() )
			{
				TracePC("\r\n Drv rec=ok"); 
				recRes = 1;				
				break;
			}			
		}
		if(Timer.PCRecTimer==0)
		{			
			retry--;
			TracePC("\r\n Drv failretry=%d",retry); 
		}	
		if(recRes)
		{
			//如果发现接收有延后，可以超前一步再接收一次数据	
			if(sysVPMissionCR.receive.msgType==VP_ACK)
			{
				if( VPBusFrameUnPack_CR() )
				{
					TracePC("\r\n Drv rec2=ok"); 
					recRes = 1;					
				}
			}
			break;//接收到数据退出
		}
	}
	//TracePC("\r\n Drv retry=%d",retry); 
	if( retry== 0 )
	{
		TracePC("\r\n Drv rec=fail"); 
		OSTimeDly(10);	
        return VP_ERR_COM;
	}

	//DisplayStr( 0, 0, 1, "System 2", 10 );  
	//WaitForWork(2000,NULL);
	//TracePC("\r\n Drv rec=result"); 
    switch( sysVPMissionCR.receive.msgType )
	{
		case VP_GET_SETUP_IND: 
			VPMission_Setup_RPT_CR();						
			break;
 	      case VP_GET_STATUS:
		    VPMission_Status_RPT_CR();
		    break;	
		case VP_GETINFO_IND:			
			VPMission_Info_RPT_CR(sysVPMissionCR.receive.msg[0]);
			break;	
		case VP_CONTROL_IND:
		    VP_Control_Ind_CR();
		    break;
		case VP_PAYIN_IND:
			VP_Payin_Ind_CR();
			break;
		case VP_PAYOUT_IND:
		    VP_Payout_Ind_CR();
		    break; 	
		case VP_COST_IND://添加扣款函数;by gzz 20110823
		    VP_Cost_Ind_CR();
		    break;		
		case VP_RESET_IND:
		    VP_Reset_Ind_CR();
		    break;		
		default:
		    break;
	}
	
	return VP_ERR_NULL;
}




