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


uint8_t  decimalPlacesCR=1;//基本单位1角,2分,0元
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
			if( (VPMsgBufCR[i+len] != sum/256)||(VPMsgBufCR[i+len+1] != sum%256)	)
			{
				continue;
		    }
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
    
    uint8_t i=0,j=0,k=0,index=0,tempcan=0;
	uint8_t issnup=0;//1时不升级sn,0时需要升级sn
	uint16_t tempMoney;
	uint8_t tempSend=0;
	uint32_t tradeMoney=0;
	uint8_t cabinet=0,cabno=0;
	

    GoodsSNCR=sysVPMissionCR.receive.sn;
    
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
		case VP_HUODAO_RPT:
            {
                sysVPMissionCR.send.msgType = VP_HUODAO_RPT;
				if(SystemPara.EasiveEnable == 1)
				{
                	sysVPMissionCR.send.datLen  = COLUMN_NUM_SET+1;    
				}
				else
				{
                	sysVPMissionCR.send.datLen  = 1+1;    
				}	
				//sysVPMissionCR.send.msg[0]  = 0;
                //
            }
            break;	
		case VP_PAYIN_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_PAYIN_RPT;
                sysVPMissionCR.send.datLen = 5;      //3,5
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.payInDev;
				TracePC("\r\n Drv Uboxbill=%d,all=%ld",sysVPMissionCR.payInMoney,sysVPMissionCR.payAllMoney); 				
				tempMoney = MoneySend_CR(sysVPMissionCR.payInMoney);
				sysVPMissionCR.send.msg[1] = tempMoney/256;
				sysVPMissionCR.send.msg[2] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[3] = tempMoney/256;
                sysVPMissionCR.send.msg[4] = tempMoney%256;
			}
			break;
		case VP_PAYOUT_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_PAYOUT_RPT;
				sysVPMissionCR.send.datLen = 6;   
                sysVPMissionCR.send.msg[0] = sysVPMissionCR.payoutDev;
				tempMoney = MoneySend_CR(sysVPMissionCR.payoutMoney);
				sysVPMissionCR.send.msg[1] = tempMoney/256;
				sysVPMissionCR.send.msg[2] = tempMoney%256; 				
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721				
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[3] = tempMoney/256;
                sysVPMissionCR.send.msg[4] = tempMoney%256;
				sysVPMissionCR.send.msg[5] = sysVPMissionCR.type;				
			}
			break;
        case VP_COST_RPT://扣款操作;by gzz 20110823
		    {
			    sysVPMissionCR.send.msgType = VP_COST_RPT;
				sysVPMissionCR.send.datLen = 6;   
                sysVPMissionCR.send.msg[0] = 0;
				tempMoney = MoneySend_CR(sysVPMissionCR.costMoney);
				sysVPMissionCR.send.msg[1] = tempMoney/256;
				sysVPMissionCR.send.msg[2] = tempMoney%256;                 
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721				
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[3] = tempMoney/256;
                sysVPMissionCR.send.msg[4] = tempMoney%256;
				sysVPMissionCR.send.msg[5] = sysVPMissionCR.type;				
			}
			break;	
		case VP_BUTTON_RPT:		    
			sysVPMissionCR.send.msgType = VP_BUTTON_RPT;
			if((sysVPMissionCR.type==VP_BUT_GOODS)||(sysVPMissionCR.type==VP_BUT_NUMBER))
			{
				sysVPMissionCR.send.datLen = 3;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
				sysVPMissionCR.send.msg[1] = sysVPMissionCR.device;
				sysVPMissionCR.send.msg[2] = sysVPMissionCR.Column;
			}
			else if(sysVPMissionCR.type==VP_BUT_GAME)
			{
				sysVPMissionCR.send.datLen = 2;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
				sysVPMissionCR.send.msg[1] = 0;
			}	
			else if(sysVPMissionCR.type==VP_BUT_RETURN)
			{
				sysVPMissionCR.send.datLen = 2;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
				sysVPMissionCR.send.msg[1] = 0;
			}	
			break;	
		case VP_VENDOUT_RPT:
		    {
				sysVPMissionCR.send.msgType = VP_VENDOUT_RPT;
                sysVPMissionCR.send.datLen = 9;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.device;
				sysVPMissionCR.send.msg[1] = sysVPMissionCR.status;
				sysVPMissionCR.send.msg[2] = sysVPMissionCR.Column;
				sysVPMissionCR.send.msg[3] = sysVPMissionCR.type;
				tempMoney = MoneySend_CR(sysVPMissionCR.costMoney);
				sysVPMissionCR.send.msg[4] = tempMoney/256;
				sysVPMissionCR.send.msg[5] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721				
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[6] = tempMoney/256;
                sysVPMissionCR.send.msg[7] = tempMoney%256;
				sysVPMissionCR.send.msg[8] = sysVPMissionCR.ColumnSum;
			}
			break;
		case VP_STATUS_RPT:
		    {
			    sysVPMissionCR.send.msgType = VP_STATUS_RPT;
                //sysVPMissionCR.send.datLen  = 15;
				sysVPMissionCR.send.datLen  = 16;

				tempSend |= sysVPMissionCR.vmc_st;
				tempSend |= sysVPMissionCR.cc_st<<2;
				tempSend |= sysVPMissionCR.bv_st<<4;
				tempSend |= sysVPMissionCR.check_st<<6;				
				sysVPMissionCR.send.msg[0]  = tempSend;
				tempSend = 0xff;
				sysVPMissionCR.send.msg[1]  = tempSend;
				
				tempMoney = MoneySend_CR(sysVPMissionCR.change);
                sysVPMissionCR.send.msg[2] = tempMoney/256;
                sysVPMissionCR.send.msg[3] = tempMoney%256;
				
				sysVPMissionCR.send.msg[4]  = sysVPMissionCR.recyclerSum[0];
				sysVPMissionCR.send.msg[5]  = sysVPMissionCR.recyclerSum[1];
				sysVPMissionCR.send.msg[6]  = sysVPMissionCR.recyclerSum[2];
				sysVPMissionCR.send.msg[7]  = sysVPMissionCR.recyclerSum[3];
				sysVPMissionCR.send.msg[8]  = sysVPMissionCR.recyclerSum[4];
				sysVPMissionCR.send.msg[9]  = sysVPMissionCR.recyclerSum[5];
				
				sysVPMissionCR.send.msg[10] = sysVPMissionCR.coinSum[0];
				sysVPMissionCR.send.msg[11] = sysVPMissionCR.coinSum[1];
				sysVPMissionCR.send.msg[12] = sysVPMissionCR.coinSum[2];
				sysVPMissionCR.send.msg[13] = sysVPMissionCR.coinSum[3];
				sysVPMissionCR.send.msg[14] = sysVPMissionCR.coinSum[4];	
				sysVPMissionCR.send.msg[15] = sysVPMissionCR.coinSum[5];		
			}
			break;
		case VP_ACTION_RPT:		    
			sysVPMissionCR.send.msgType = VP_ACTION_RPT;	
			if( (sysVPMissionCR.action==VP_ACT_HEART)||(sysVPMissionCR.action==VP_ACT_ONLINE)||(sysVPMissionCR.action==VP_ACT_PCON) )
			{
				sysVPMissionCR.send.datLen = 1;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.action;
				decimalPlacesCR = SystemPara.DecimalNum;
				TracePC("\r\n Drv PCAction=%d",sysVPMissionCR.action); 
			}			
			else if(sysVPMissionCR.action==VP_ACT_ADMIN)
			{
				sysVPMissionCR.send.datLen = 2;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.action;
				sysVPMissionCR.send.msg[1] = sysVPMissionCR.value;
			}	
			else if(sysVPMissionCR.action==VP_ACT_CHUHUO)
			{
				sysVPMissionCR.send.datLen = 8;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.action;
				sysVPMissionCR.send.msg[1] = sysVPMissionCR.second;
				sysVPMissionCR.send.msg[2] = sysVPMissionCR.Column;
				sysVPMissionCR.send.msg[3] = sysVPMissionCR.type;
				tempMoney = MoneySend_CR(sysVPMissionCR.costMoney);
				sysVPMissionCR.send.msg[4] = tempMoney/256;
				sysVPMissionCR.send.msg[5] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[6] = tempMoney/256;
                sysVPMissionCR.send.msg[7] = tempMoney%256;
			}	
			else if(sysVPMissionCR.action==VP_ACT_PAYOUT)
			{
				sysVPMissionCR.send.datLen = 7;
				sysVPMissionCR.send.msg[0] = sysVPMissionCR.action;
				if(SystemPara.EasiveEnable == 1)
				{
					sysVPMissionCR.send.msg[1] = sysVPMissionCR.second;
				}
				else
				{
					sysVPMissionCR.send.msg[1] = 240;
				}	
				tempMoney = MoneySend_CR(sysVPMissionCR.costMoney);
				sysVPMissionCR.send.msg[2] = tempMoney/256;
				sysVPMissionCR.send.msg[3] = tempMoney%256;
				//PAYIN_RPT协议中末尾添加两个字节
                //Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
				//by gzz 20110721
				tempMoney = MoneySend_CR(sysVPMissionCR.payAllMoney);
                sysVPMissionCR.send.msg[4] = tempMoney/256;
                sysVPMissionCR.send.msg[5] = tempMoney%256;
				sysVPMissionCR.send.msg[6] = sysVPMissionCR.type;
				//TracePC("\r\n Drv ActPayout=%d,%d,%d,%d,%d,%ld,%ld",AccepterUboxMsg->action,AccepterUboxMsg->value,AccepterUboxMsg->second,AccepterUboxMsg->Column,AccepterUboxMsg->Type,AccepterUboxMsg->costMoney,AccepterUboxMsg->payAllMoney); 
			}				
			break; 
		case VP_ADMIN_RPT:
		    {
			    sysVPMissionCR.send.msgType =  VP_ADMIN_RPT;

				if( sysVPMissionCR.type == VP_ADMIN_GOODSADDCOL )
				{
					sysVPMissionCR.send.datLen = 3;
					sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
					sysVPMissionCR.send.msg[1] = sysVPMissionCR.Column;
					sysVPMissionCR.send.msg[2] = sysVPMissionCR.ColumnSum;
				}
                else if( sysVPMissionCR.type == VP_ADMIN_GOODSADDTRAY ) //Add the tray's goods
				{
				    sysVPMissionCR.send.datLen = 2;
					sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
					sysVPMissionCR.send.msg[1] = sysVPMissionCR.Column;
				}
				else  //Add all columns' goods, chage, get bill
				{
				    sysVPMissionCR.send.datLen = 1;
					sysVPMissionCR.send.msg[0] = sysVPMissionCR.type;
				}
				
			}
			break;    	
		
		case VP_INFO_RPT://120419 by cq TotalSell 
			{
                sysVPMissionCR.send.msgType = VP_INFO_RPT;  
				switch(sysVPMissionCR.type)
				{
					case VP_INFO_TOTALVALUE:
						sysVPMissionCR.send.datLen  = 3;
						sysVPMissionCR.send.msg[0]  = VP_INFO_TOTALVALUE;
						//Total_Value：表示当前交易投币后，或者出货后，屏幕上显示的金额数 
						//by gzz 20110721						
						tempMoney = MoneySend_CR(GetAmountMoney());
						//tempMoney = sysVPMissionCR.payAllMoney;
		                sysVPMissionCR.send.msg[1] = tempMoney/256;
		                sysVPMissionCR.send.msg[2] = tempMoney%256;
						break;
					case VP_INFO_CLOCK:
						sysVPMissionCR.send.datLen  = 9;
						sysVPMissionCR.send.msg[0]  = VP_INFO_CLOCK;												
		                sysVPMissionCR.send.msg[1] = RTCData.year/256;
		                sysVPMissionCR.send.msg[2] = RTCData.year%256;
						sysVPMissionCR.send.msg[3] = RTCData.month;
						sysVPMissionCR.send.msg[4] = RTCData.day;
						sysVPMissionCR.send.msg[5] = RTCData.hour;
						sysVPMissionCR.send.msg[6] = RTCData.minute;
						sysVPMissionCR.send.msg[7] = RTCData.second;
						sysVPMissionCR.send.msg[8] = RTCData.week;
						break;
					case VP_INFO_CHUHUO:
						sysVPMissionCR.send.datLen  = 53;
						sysVPMissionCR.send.msg[0]  = VP_INFO_CHUHUO;
						if(SystemPara.EasiveEnable == 1)
						{
							//全部出货数量
							sysVPMissionCR.send.msg[1]  = LogPara.vpSuccessNumTotal/256/256/256;       
			                sysVPMissionCR.send.msg[2]  = LogPara.vpSuccessNumTotal/256/256%256;       
			                sysVPMissionCR.send.msg[3]  = LogPara.vpSuccessNumTotal/256%256;       
			                sysVPMissionCR.send.msg[4]  = LogPara.vpSuccessNumTotal%256; 
							//全部出货金额						
							//tradeMoney = MoneySendInfo_CR(TradeCounter.vpSuccessMoney); 
							tradeMoney = LogPara.vpSuccessMoneyTotal; 
							sysVPMissionCR.send.msg[5]  = tradeMoney/256/256/256;		 
							sysVPMissionCR.send.msg[6]  = tradeMoney/256/256%256;		 
							sysVPMissionCR.send.msg[7]  = tradeMoney/256%256;		 
							sysVPMissionCR.send.msg[8]  = tradeMoney%256;	
							
							//tradeMoney = MoneySendInfo_CR(TradeCounter.vpSuccessMoney); 
							//sysVPMissionCR.send.msg[5]  = tradeMoney/256/256/256;		 
							//sysVPMissionCR.send.msg[6]  = tradeMoney/256/256%256;		 
							//sysVPMissionCR.send.msg[7]  = tradeMoney/256%256;		 
							//sysVPMissionCR.send.msg[8]  = tradeMoney%256;
							//len = sprintf( str, "%ld,%ld,%u,%u", TradeCounter.vpSuccessMoney, tradeMoney,sysVPMissionCR.send.msg[7],sysVPMissionCR.send.msg[8] );
							//DisplayStr( 0, 0, 1, str, len );
							//len = sprintf( str, "%u,%u", tradeMoney/256%256, tradeMoney%256 );
							//DisplayStr( 0, 1, 1, str, len );
							//WaitForWork( 5000, NULL );

							//现金出货数量
							sysVPMissionCR.send.msg[9]  = LogPara.vpCashNumTotal/256/256/256;		 
							sysVPMissionCR.send.msg[10]  = LogPara.vpCashNumTotal/256/256%256;		 
							sysVPMissionCR.send.msg[11]  = LogPara.vpCashNumTotal/256%256;		 
							sysVPMissionCR.send.msg[12]  = LogPara.vpCashNumTotal%256; 
							//现金出货金额	
							//tradeMoney = MoneySendInfo_CR(TradeCounter.vpCashMoney); 
							tradeMoney = LogPara.vpCashMoneyTotal; 
							sysVPMissionCR.send.msg[13]  = tradeMoney/256/256/256; 	 
							sysVPMissionCR.send.msg[14]  = tradeMoney/256/256%256; 	 
							sysVPMissionCR.send.msg[15]  = tradeMoney/256%256; 	 
							sysVPMissionCR.send.msg[16]  = tradeMoney%256; 
							/*
							//游戏出货数量 		
							sysVPMissionCR.send.msg[17]  = TradeCounter.vpGameNum/256/256/256;		 
							sysVPMissionCR.send.msg[18]  = TradeCounter.vpGameNum/256/256%256;		 
							sysVPMissionCR.send.msg[19]  = TradeCounter.vpGameNum/256%256;		 
							sysVPMissionCR.send.msg[20]  = TradeCounter.vpGameNum%256; 
							//游戏出货金额
							tradeMoney = 0; 
							sysVPMissionCR.send.msg[21]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[22]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[23]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[24]  = tradeMoney; 
							//刷卡出货数量
							sysVPMissionCR.send.msg[25]  = TradeCounter.vpCardNum/256/256/256;		 
							sysVPMissionCR.send.msg[26]  = TradeCounter.vpCardNum/256/256%256;		 
							sysVPMissionCR.send.msg[27]  = TradeCounter.vpCardNum/256%256;		 
							sysVPMissionCR.send.msg[28]  = TradeCounter.vpCardNum%256; 
							//刷卡出货金额
							tradeMoney =0; 
							sysVPMissionCR.send.msg[29]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[30]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[31]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[32]  = tradeMoney;
							//在线出货数量
							sysVPMissionCR.send.msg[33]  = TradeCounter.vpOnlineNum/256/256/256;		 
							sysVPMissionCR.send.msg[34]  = TradeCounter.vpOnlineNum/256/256%256;		 
							sysVPMissionCR.send.msg[35]  = TradeCounter.vpOnlineNum/256%256;		 
							sysVPMissionCR.send.msg[36]  = TradeCounter.vpOnlineNum%256; 
							//在线出货金额
							tradeMoney = 0; 
							sysVPMissionCR.send.msg[37]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[38]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[39]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[40]  = tradeMoney;
							*/
							//游戏出货数量 		
							sysVPMissionCR.send.msg[17]  = LogPara.vpGameNumTotal/256/256/256;		 
							sysVPMissionCR.send.msg[18]  = LogPara.vpGameNumTotal/256/256%256;					 
							sysVPMissionCR.send.msg[19]  = LogPara.vpGameNumTotal/256%256;			 
							sysVPMissionCR.send.msg[20]  = LogPara.vpGameNumTotal%256; 	
							//游戏出货金额
							tradeMoney = 0; 
							sysVPMissionCR.send.msg[21]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[22]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[23]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[24]  = tradeMoney; 
							//刷卡出货数量
							sysVPMissionCR.send.msg[25]  = LogPara.vpCardNumTotal/256/256/256;		 
							sysVPMissionCR.send.msg[26]  = LogPara.vpCardNumTotal/256/256%256;		 
							sysVPMissionCR.send.msg[27]  = LogPara.vpCardNumTotal/256%256;	 
							sysVPMissionCR.send.msg[28]  = LogPara.vpCardNumTotal%256; 
							//刷卡出货金额
							tradeMoney =0; 
							sysVPMissionCR.send.msg[29]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[30]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[31]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[32]  = tradeMoney;
							//在线出货数量
							sysVPMissionCR.send.msg[33]  = LogPara.vpOnlineNumTotal/256/256/256;	 
							sysVPMissionCR.send.msg[34]  = LogPara.vpOnlineNumTotal/256/256%256;		 
							sysVPMissionCR.send.msg[35]  = LogPara.vpOnlineNumTotal/256%256;	 
							sysVPMissionCR.send.msg[36]  = LogPara.vpOnlineNumTotal%256; 
							//在线出货金额
							tradeMoney = 0; 
							sysVPMissionCR.send.msg[37]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[38]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[39]  = tradeMoney; 	 
							sysVPMissionCR.send.msg[40]  = tradeMoney;
							//硬币投入
							//tradeMoney = MoneySendInfo_CR(TradeCounter.CoinSum1y+TradeCounter.CoinSum5j); 
							tradeMoney = LogPara.CoinsIncomeTotal; 
							sysVPMissionCR.send.msg[41]  = tradeMoney/256/256/256; 	 
							sysVPMissionCR.send.msg[42]  = tradeMoney/256/256%256; 	 
							sysVPMissionCR.send.msg[43]  = tradeMoney/256%256; 	 
							sysVPMissionCR.send.msg[44]  = tradeMoney%256;
							//纸币投入
							//tradeMoney = MoneySendInfo_CR(TradeCounter.CashSum); 
							tradeMoney = LogPara.NoteIncomeTotal; 
							sysVPMissionCR.send.msg[45]  = tradeMoney/256/256/256; 	 
							sysVPMissionCR.send.msg[46]  = tradeMoney/256/256%256; 	 
							sysVPMissionCR.send.msg[47]  = tradeMoney/256%256; 	 
							sysVPMissionCR.send.msg[48]  = tradeMoney%256;
							//硬币出币
							//tradeMoney = MoneySendInfo_CR(TradeCounter.Hopper2Sum+TradeCounter.Hopper1Sum); 
							tradeMoney = LogPara.TotalChangeTotal; 
							sysVPMissionCR.send.msg[49]  = tradeMoney/256/256/256; 	 
							sysVPMissionCR.send.msg[50]  = tradeMoney/256/256%256; 	 
							sysVPMissionCR.send.msg[51]  = tradeMoney/256%256; 	 
							sysVPMissionCR.send.msg[52]  = tradeMoney%256;
						}
						else
						{
							for( i=0; i<52; i++)
							{
								//sysVPMissionCR.send.msg[i+2] = sysVPMissionCR.selItem[i];
								sysVPMissionCR.send.msg[i+1] = 0;
							}
						}
						break;
					case VP_INFO_HUODAO:	
						if(SystemPara.EasiveEnable == 1)
						{
							sysVPMissionCR.send.datLen  = COLUMN_NUM_SET + 2;
							sysVPMissionCR.send.msg[0]  = VP_INFO_HUODAO;
							sysVPMissionCR.send.msg[1]  = 1; 
							hd_huodao_id_rpt_vp(1,&sysVPMissionCR.send.msg[2]);
						}
						else
						{
							sysVPMissionCR.send.datLen  = 48 + 2;
							sysVPMissionCR.send.msg[0]  = VP_INFO_HUODAO;
							sysVPMissionCR.send.msg[1]  = 1; 
							for( i=0; i<48; i++)
							{
								//sysVPMissionCR.send.msg[i+2] = sysVPMissionCR.selItem[i];
								sysVPMissionCR.send.msg[i+2] = 0;
							}
						}
						break;
					case VP_INFO_POSITION:
						sysVPMissionCR.send.datLen  = 18 + 2;
						sysVPMissionCR.send.msg[0]  = VP_INFO_POSITION;
						sysVPMissionCR.send.msg[1]  = 0; 
						for( i=0; i<18; i++)
						{
							//sysVPMissionCR.send.msg[i+2] = sysVPMissionCR.selItem[i];
							sysVPMissionCR.send.msg[i+2] = 0;
						}
						break;					
					case VP_INFO_VMC:
						sysVPMissionCR.send.datLen  = 4;
						sysVPMissionCR.send.msg[0]  = VP_INFO_VMC;
						sysVPMissionCR.send.msg[1]  = 0; 
						sysVPMissionCR.send.msg[2]  = 0; 
						sysVPMissionCR.send.msg[3]  = COLUMN_NUM_SET + 1;
						break;
					case VP_INFO_VER:
						sysVPMissionCR.send.datLen  = 32;
						sysVPMissionCR.send.msg[0]  = VP_INFO_VER;
						// unpeng-zonghe-2.00(2012-11-07)
						sysVPMissionCR.send.msg[1]  = 'J'; 	 
						sysVPMissionCR.send.msg[2]  = 'u'; 	 
						sysVPMissionCR.send.msg[3]  = 'n'; 	 
						sysVPMissionCR.send.msg[4]  = 'p';
						sysVPMissionCR.send.msg[5]  = 'e'; 	 
						sysVPMissionCR.send.msg[6]  = 'n'; 	 
						sysVPMissionCR.send.msg[7]  = 'g'; 	 
						sysVPMissionCR.send.msg[8]  = '-';
						sysVPMissionCR.send.msg[9]  = 'z'; 	 
						sysVPMissionCR.send.msg[10]  = 'o'; 	 
						sysVPMissionCR.send.msg[11]  = 'n';
						sysVPMissionCR.send.msg[12]  = 'g'; 	 
						sysVPMissionCR.send.msg[13]  = 'h'; 	 
						sysVPMissionCR.send.msg[14]  = 'e'; 	 
						sysVPMissionCR.send.msg[15]  = '-';
						sysVPMissionCR.send.msg[16]  = '2'; 	 
						sysVPMissionCR.send.msg[17]  = '.'; 	 
						sysVPMissionCR.send.msg[18]  = '0';
						sysVPMissionCR.send.msg[19]  = '3';
						sysVPMissionCR.send.msg[20]  = '('; 	 
						sysVPMissionCR.send.msg[21]  = '2'; 	 
						sysVPMissionCR.send.msg[22]  = '0'; 	 
						sysVPMissionCR.send.msg[23]  = '1';
						sysVPMissionCR.send.msg[24]  = '4'; 	 
						sysVPMissionCR.send.msg[25]  = '-'; 	 
						sysVPMissionCR.send.msg[26]  = '0';
						sysVPMissionCR.send.msg[27]  = '4'; 	 
						sysVPMissionCR.send.msg[28]  = '-'; 	 
						sysVPMissionCR.send.msg[29]  = '0'; 	 
						sysVPMissionCR.send.msg[30]  = '3';
						sysVPMissionCR.send.msg[31]  = ')';
						break;
					case VP_INFO_HARD:
						sysVPMissionCR.send.datLen  = 18;
						sysVPMissionCR.send.msg[0]  = VP_INFO_HARD;						
						sysVPMissionCR.send.msg[1]  = stMacSn.id[0]; 	 
						sysVPMissionCR.send.msg[2]  = stMacSn.id[1];	 
						sysVPMissionCR.send.msg[3]  = stMacSn.id[2];	 
						sysVPMissionCR.send.msg[4]  = stMacSn.id[3];
						sysVPMissionCR.send.msg[5]  = stMacSn.id[4];	 
						sysVPMissionCR.send.msg[6]  = stMacSn.id[5];	 
						sysVPMissionCR.send.msg[7]  = stMacSn.id[6]; 	
						sysVPMissionCR.send.msg[8]  = stMacSn.id[7];	 
						sysVPMissionCR.send.msg[9]  = stMacSn.id[8];	 
						sysVPMissionCR.send.msg[10]  = stMacSn.id[9];
						sysVPMissionCR.send.msg[11]  = stMacSn.id[10];	 
						sysVPMissionCR.send.msg[12]  = stMacSn.id[11];	 
						sysVPMissionCR.send.msg[13]  = stMacSn.id[12]; 
						sysVPMissionCR.send.msg[14]  = stMacSn.id[13];	 
						sysVPMissionCR.send.msg[15]  = stMacSn.id[14];	 
						sysVPMissionCR.send.msg[16]  = stMacSn.id[15];
						sysVPMissionCR.send.msg[17]  = stMacSn.id[16];
						
						
						break;
					case VP_INFO_BILL:						
						sysVPMissionCR.send.datLen  = 19+33;
						sysVPMissionCR.send.msg[0]  = VP_INFO_BILL;
						sysVPMissionCR.send.msg[1]  = stDevValue.BillLevel;
						sysVPMissionCR.send.msg[2]  = stDevValue.BillCode/256;	 
						sysVPMissionCR.send.msg[3]  = stDevValue.BillCode%256; 	 
						sysVPMissionCR.send.msg[4]  = stDevValue.BillScale/256; 	
						sysVPMissionCR.send.msg[5]  = stDevValue.BillScale%256; 		 
						sysVPMissionCR.send.msg[6]  = stDevValue.BillDecimal; 	 
						sysVPMissionCR.send.msg[7]  = stDevValue.BillStkCapacity/256;  
						sysVPMissionCR.send.msg[8]  = stDevValue.BillStkCapacity%256; 
						sysVPMissionCR.send.msg[9]  = stDevValue.BillSecurity%256;  	 
						sysVPMissionCR.send.msg[10]  = stDevValue.BillEscrowFun; 	 
						sysVPMissionCR.send.msg[11]  = stDevValue.BillValue[0]/stDevValue.BillScale;
						sysVPMissionCR.send.msg[12]  = stDevValue.BillValue[1]/stDevValue.BillScale; 	 
						sysVPMissionCR.send.msg[13]  = stDevValue.BillValue[2]/stDevValue.BillScale; 	 
						sysVPMissionCR.send.msg[14]  = stDevValue.BillValue[3]/stDevValue.BillScale; 	 
						sysVPMissionCR.send.msg[15]  = stDevValue.BillValue[4]/stDevValue.BillScale;
						sysVPMissionCR.send.msg[16]  = stDevValue.BillValue[5]/stDevValue.BillScale; 	
						sysVPMissionCR.send.msg[17]  = stDevValue.BillValue[6]/stDevValue.BillScale;
						sysVPMissionCR.send.msg[18]  = stDevValue.BillValue[7]/stDevValue.BillScale;
						for(i=0;i<33;i++)
						{
							sysVPMissionCR.send.msg[19+i]  = stDevValue.billIDENTITYBuf[i];
						}
						TracePC("\r\nDrvUbill=%d,%ld,%ld,%ld,%ld,%ld,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.BillLevel,stDevValue.BillCode,stDevValue.BillScale,stDevValue.BillDecimal,stDevValue.BillStkCapacity,
							stDevValue.BillSecurity,stDevValue.BillEscrowFun,stDevValue.BillValue[0],stDevValue.BillValue[1],stDevValue.BillValue[2],stDevValue.BillValue[3],
							stDevValue.BillValue[4],stDevValue.BillValue[5],stDevValue.BillValue[6],stDevValue.BillValue[7]);
						break;
					case VP_INFO_COIN:						
						sysVPMissionCR.send.datLen  = 18+33;
						sysVPMissionCR.send.msg[0]  = VP_INFO_COIN;
						sysVPMissionCR.send.msg[1]  = stDevValue.CoinLevel;
						sysVPMissionCR.send.msg[2]  = stDevValue.CoinCode/256;	 
						sysVPMissionCR.send.msg[3]  = stDevValue.CoinCode%256; 	 
						sysVPMissionCR.send.msg[4]  = stDevValue.CoinScale; 	
						sysVPMissionCR.send.msg[5]  = SendCoinDem_CR(); 		 
						sysVPMissionCR.send.msg[6]  = stDevValue.CoinValue[0]/stDevValue.CoinScale; 	 
						sysVPMissionCR.send.msg[7]  = stDevValue.CoinValue[1]/stDevValue.CoinScale;  
						sysVPMissionCR.send.msg[8]  = stDevValue.CoinValue[2]/stDevValue.CoinScale; 
						sysVPMissionCR.send.msg[9]  = stDevValue.CoinValue[3]/stDevValue.CoinScale;  	 
						sysVPMissionCR.send.msg[10]  = stDevValue.CoinValue[4]/stDevValue.CoinScale; 	 
						sysVPMissionCR.send.msg[11]  = stDevValue.CoinValue[5]/stDevValue.CoinScale;
						sysVPMissionCR.send.msg[12]  = stDevValue.CoinValue[0]/stDevValue.CoinScale; 	 
						sysVPMissionCR.send.msg[13]  = stDevValue.CoinValue[1]/stDevValue.CoinScale; 	 
						sysVPMissionCR.send.msg[14]  = stDevValue.CoinValue[2]/stDevValue.CoinScale; 	 
						sysVPMissionCR.send.msg[15]  = stDevValue.CoinValue[3]/stDevValue.CoinScale;
						sysVPMissionCR.send.msg[16]  = stDevValue.CoinValue[4]/stDevValue.CoinScale; 	
						sysVPMissionCR.send.msg[17]  = stDevValue.CoinValue[5]/stDevValue.CoinScale;
						for(i=0;i<33;i++)
						{
							sysVPMissionCR.send.msg[18+i]  = stDevValue.coinIDENTITYBuf[i];
						}						
						TracePC("\r\nDrvUcoin=%d,%ld,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",stDevValue.CoinLevel,stDevValue.CoinCode,stDevValue.CoinScale,stDevValue.CoinDecimal,stDevValue.CoinValue[0],
							stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3],stDevValue.CoinValue[4],stDevValue.CoinValue[5],
							stDevValue.CoinValue[0],stDevValue.CoinValue[1],stDevValue.CoinValue[2],stDevValue.CoinValue[3],stDevValue.CoinValue[4],stDevValue.CoinValue[5]);
						break;
					case VP_INFO_COMP:
						sysVPMissionCR.send.datLen  = 1;
						sysVPMissionCR.send.msg[0]  = VP_INFO_COMP;
						break;
					case VP_INFO_ERR:
						{	
							i = 0;				
							sysVPMissionCR.send.msg[i++] = VP_INFO_ERR;

							if(SystemPara.EasiveEnable == 1)
							{
								//找零器状态
								//MDB找零器
								if(SystemPara.CoinChangerType==MDB_CHANGER)
								{
									if(DeviceStateBusiness.CoinCommunicate)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa2;
									}
									else if(DeviceStateBusiness.Coinsensor)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa3;
									}
									else if(DeviceStateBusiness.Cointubejam)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa4;
									}
									else if(DeviceStateBusiness.Coinromchk)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa5;
									}
									else if(DeviceStateBusiness.Coinrouting)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa6;
									}							
									else if(DeviceStateBusiness.Coinjam)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa7;
									}
									else if(DeviceStateBusiness.CoinremoveTube)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa8;
									}
									else if(DeviceStateBusiness.Coindisable)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa9;
									}
									else if(DeviceStateBusiness.CoinunknowError)
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xaa;
									}
									else
									{
										sysVPMissionCR.send.msg[i++] = 0x08;
										sysVPMissionCR.send.msg[i++] = 0xa1;
									}	
									
								}
								//Hopper找零器
								else if(SystemPara.CoinChangerType == HOPPER_CHANGER)
								{
									if(SystemPara.HopperValue[0]==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x13;
										sysVPMissionCR.send.msg[i++] = 0x88;
									}
									else
									{
										if(DeviceStateBusiness.Hopper1State==0)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x89;
										}
										if(DeviceStateBusiness.Hopper1State==1)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x8a;
										}
										if(DeviceStateBusiness.Hopper1State==2)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x8b;
										}
									}
									if(SystemPara.HopperValue[1]==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x13;
										sysVPMissionCR.send.msg[i++] = 0x8c;
									}
									else
									{
										if(DeviceStateBusiness.Hopper2State==0)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x8d;
										}
										if(DeviceStateBusiness.Hopper2State==1)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x8e;
										}
										if(DeviceStateBusiness.Hopper2State==2)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x8f;
										}
									}
									if(SystemPara.HopperValue[2]==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x13;
										sysVPMissionCR.send.msg[i++] = 0x90;
									}
									else
									{
										if(DeviceStateBusiness.Hopper3State==0)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x91;
										}
										if(DeviceStateBusiness.Hopper3State==1)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x92;
										}
										if(DeviceStateBusiness.Hopper3State==2)
										{
											sysVPMissionCR.send.msg[i++] = 0x13;
											sysVPMissionCR.send.msg[i++] = 0x93;
										}
									}
								}
								else
								{
									sysVPMissionCR.send.msg[i++] = 0x08;
									sysVPMissionCR.send.msg[i++] = 0xa0;
								}
															
								//5.硬币器模块	
								if(SystemPara.CoinAcceptorType==MDB_COINACCEPTER)				
								{															
									if(DeviceStateBusiness.CoinCommunicate)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa2;
									}
									else if(DeviceStateBusiness.Coinsensor)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa3;
									}
									else if(DeviceStateBusiness.Cointubejam)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa4;
									}
									else if(DeviceStateBusiness.Coinromchk)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa5;
									}
									else if(DeviceStateBusiness.Coinrouting)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa6;
									}							
									else if(DeviceStateBusiness.Coinjam)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa7;
									}
									else if(DeviceStateBusiness.CoinremoveTube)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa8;
									}
									else if(DeviceStateBusiness.Coindisable)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa9;
									}
									else if(DeviceStateBusiness.CoinunknowError)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xaa;
									}
									else if(GetBillCoinStatus(2)==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa0;
									}
									else 
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa1;
									}
								}
								//脉冲硬币器
								else if((SystemPara.CoinAcceptorType == PARALLEL_COINACCEPTER)||(SystemPara.CoinAcceptorType == SERIAL_COINACCEPTER))
								{
									if(GetBillCoinStatus(2)==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa0;
									}
									else 
									{
										sysVPMissionCR.send.msg[i++] = 0x0f;
										sysVPMissionCR.send.msg[i++] = 0xa1;
									}
								}
								else 
								{
									sysVPMissionCR.send.msg[i++] = 0x0f;
									sysVPMissionCR.send.msg[i++] = 0xa0;
								}
															
								//7.纸币器模块
								if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)					
								{
									if(DeviceStateBusiness.BillCommunicate)
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x72;
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
									else if(DeviceStateBusiness.Billdisable)
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x79;
									}
									else if(TubeMoneyEnough())
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x7a;
									}
									else if(GetBillCoinStatus(1)==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x70;
									}
									else
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x71;
									}	
								}
								else
								{
									sysVPMissionCR.send.msg[i++] = 0x17;
									sysVPMissionCR.send.msg[i++] = 0x70;
								}
							}
							else
							{								
								//7.纸币器模块
								if(SystemPara.BillValidatorType==MDB_BILLACCEPTER)					
								{
									if(DeviceStateBusiness.Billmoto)
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x73;
									}
									else if(DeviceStateBusiness.Billsensor)
									{
										sysVPMissionCR.send.msg[i++] = 0x17;
										sysVPMissionCR.send.msg[i++] = 0x74;
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
								
							}
							
							sysVPMissionCR.send.datLen = i;   
						}
						break;
					case VP_INFO_HEFANGGUI:
						sysVPMissionCR.send.datLen  = 4;
						sysVPMissionCR.send.msg[0]  = VP_INFO_HEFANGGUI;
						sysVPMissionCR.send.msg[1]  = sysVPMissionCR.Control_Bin;
						sysVPMissionCR.send.msg[2]  = sysVPMissionCR.Control_Huodao;
						sysVPMissionCR.send.msg[3]  = sysVPMissionCR.value;
						break;
					case VP_INFO_CABINET:
						i = 0;
						sysVPMissionCR.send.msg[i++]  = VP_INFO_CABINET;
						sysVPMissionCR.send.msg[i++]  = 1;
						for(cabinet = 0,cabno=0;cabinet<sysVPMissionCR.cabinetNums;cabinet++)
						{
							sysVPMissionCR.send.msg[i++]  = sysVPMissionCR.cabinetdata[cabno++];
							sysVPMissionCR.send.msg[i++]  = sysVPMissionCR.cabinetdata[cabno++];
							sysVPMissionCR.send.msg[i++]  = sysVPMissionCR.cabinetdata[cabno++];
						}
						sysVPMissionCR.send.datLen = i;
						break;
					case VP_INFO_CABERR:
						TracePC("\r\nDrvCabErr=%d",sysVPMissionCR.Control_Bin);
						i = 0;
						sysVPMissionCR.send.msg[i++]  = VP_INFO_CABERR;
						//1柜设备状态
						if(sysVPMissionCR.Control_Bin==1)
						{
							//1.货道类型
							if((SystemPara.GeziDeviceType==1)||(SystemPara.Channel==0))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd0;
							}
							else if(SystemPara.Channel==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd1;
							}
							else if(SystemPara.Channel==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd2;
							}
							else if(SystemPara.Channel==3)
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd3;
							}
							else if(SystemPara.Channel==4)
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd4;
							}
							
							if(SystemPara.GeziDeviceType==0)
							{
								//1.GOC故障模块
								if(SystemPara.GOCIsOpen == 0)
								{
									sysVPMissionCR.send.msg[i++] = 0x03;
									sysVPMissionCR.send.msg[i++] = 0xe8;									
									
								}
								else if(SystemPara.GOCIsOpen == 1)
								{
									if(DeviceStateBusiness.GOCError==0)//1柜
									{
										sysVPMissionCR.send.msg[i++] = 0x03;
										sysVPMissionCR.send.msg[i++] = 0xe9;
									}
									else
									{
										sysVPMissionCR.send.msg[i++] = 0x03;
										sysVPMissionCR.send.msg[i++] = 0xea;
									}									
								}
																
								//2.弹簧货道模块
								//1柜
								if(SystemPara.Channel==1)
								{
									if(DeviceStateBusiness.ColBoardError==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x07;
										sysVPMissionCR.send.msg[i++] = 0xd1;
									}
									else
									{
										sysVPMissionCR.send.msg[i++] = 0x07;
										sysVPMissionCR.send.msg[i++] = 0xd2;
									}	
								}
															

								//3.升降机模块
								//1柜
								if((SystemPara.Channel==2)||(SystemPara.Channel==3)||(SystemPara.Channel==4))							
								{
									if((DeviceStateBusiness.Error_FMD==0)&&(DeviceStateBusiness.Error_FUM==0)&&(DeviceStateBusiness.Error_GOC==0)&&(DeviceStateBusiness.Error_FLD==0)&&(DeviceStateBusiness.Error_FOD==0)
										&&(DeviceStateBusiness.Error_UDS==0)&&(DeviceStateBusiness.Error_GCD==0)&&(DeviceStateBusiness.Error_SOVB==0)&&(DeviceStateBusiness.Error_SOMD1==0)&&(DeviceStateBusiness.Error_SOEC==0)
										&&(DeviceStateBusiness.Error_SFHG==0)&&(DeviceStateBusiness.Error_SOFP==0)&&(DeviceStateBusiness.Error_SOMD2==0))
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xb9;
									}
									if(DeviceStateBusiness.Error_FMD)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xba;
									}
									if(DeviceStateBusiness.Error_FUM)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xbb;
									}				
									if(DeviceStateBusiness.Error_GOC)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xbc;
									}
									if(DeviceStateBusiness.Error_FLD)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xbd;
									}
									if(DeviceStateBusiness.Error_FOD)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xbe;
									}
									if(DeviceStateBusiness.Error_UDS)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xbf;
									}
									if(DeviceStateBusiness.Error_GCD)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc0;
									}			
									if(DeviceStateBusiness.Error_SOVB)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc1;
									}
									if(DeviceStateBusiness.Error_SOMD1)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc2;
									}
									if(DeviceStateBusiness.Error_SOEC)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc3;
									}
									if(DeviceStateBusiness.Error_SFHG)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc4;
									}								
									if(DeviceStateBusiness.Error_SOFP)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc5;
									}				
									if(DeviceStateBusiness.Error_SOMD2)
									{
										sysVPMissionCR.send.msg[i++] = 0x0b;
										sysVPMissionCR.send.msg[i++] = 0xc6;
									}
								}
																
								//4.货道全不可用
								if(DeviceStateBusiness.Emp_Gol)
								{
									sysVPMissionCR.send.msg[i++] = 0x07;
									sysVPMissionCR.send.msg[i++] = 0xd6;
								}
							}
																			
							//8.							
							//1柜加热模块故障
							if(acdc_status_API(1,3)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x58;
							}	
							else if(acdc_status_API(1,3)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x59;
								//读取1柜温度
								sysVPMissionCR.send.msg[i++] = 0xff;
								sysVPMissionCR.send.msg[i++] = 0x01;
								sysVPMissionCR.send.msg[i++] = sysXMTMission.recPVTemp/256;
								sysVPMissionCR.send.msg[i++] = sysXMTMission.recPVTemp%256;
								
							}
							else if(acdc_status_API(1,3)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5a;
							}	
							
							//1柜制冷模块故障
							if(acdc_status_API(1,2)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5e;
							}	
							else if(acdc_status_API(1,2)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5f;
								//读取1柜温度
								sysVPMissionCR.send.msg[i++] = 0xff;
								sysVPMissionCR.send.msg[i++] = 0x03;
								sysVPMissionCR.send.msg[i++] = 0xfe;
								sysVPMissionCR.send.msg[i++] = 0xfe;
							}
							else if(acdc_status_API(1,2)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x60;
							}	
							
							//1柜照明块故障
							if(acdc_status_API(1,1)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x64;
							}	
							else if(acdc_status_API(1,1)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x65;
							}
							else if(acdc_status_API(1,1)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x66;
							}
							
							//1柜制除臭块故障
							if(acdc_status_API(1,4)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6a;
							}	
							else if(acdc_status_API(1,4)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6b;
							}
							else if(acdc_status_API(1,4)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6c;
							}
							
						
						}
						//2柜设备状态
						else if(sysVPMissionCR.Control_Bin==2)
						{
							//1.货道类型
							if((SystemPara.GeziDeviceType==1)||(SystemPara.SubChannel==0)||(SystemPara.SubBinOpen==0))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd0;
							}
							else if((SystemPara.SubBinOpen==1)&&(SystemPara.SubChannel==1))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd1;
							}
							else if((SystemPara.SubBinOpen==1)&&(SystemPara.SubChannel==2))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd2;
							}
							else if((SystemPara.SubBinOpen==1)&&(SystemPara.SubChannel==3))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd3;
							}
							else if((SystemPara.SubBinOpen==1)&&(SystemPara.SubChannel==4))
							{
								sysVPMissionCR.send.msg[i++] = 0x06;
								sysVPMissionCR.send.msg[i++] = 0xd4;
							}
							
							if(SystemPara.GeziDeviceType==0)
							{
								//1.GOC故障模块
								if(SystemPara.GOCIsOpen == 0)
								{
									if(SystemPara.SubBinOpen==1)
									{
										sysVPMissionCR.send.msg[i++] = 0x03;
										sysVPMissionCR.send.msg[i++] = 0xe8;
									}
									
								}
								else if(SystemPara.GOCIsOpen == 1)
								{									
									if(SystemPara.SubBinOpen==1)
									{
										if(DeviceStateBusiness.GOCErrorSub==0)//2柜
										{
											sysVPMissionCR.send.msg[i++] = 0x03;
											sysVPMissionCR.send.msg[i++] = 0xe9;
										}
										else
										{
											sysVPMissionCR.send.msg[i++] = 0x03;
											sysVPMissionCR.send.msg[i++] = 0xea;
										}
									}
								}
								
								//2.弹簧货道模块								
								//2柜
								if((SystemPara.SubBinOpen==1)&&(SystemPara.SubChannel==1))							
								{
									if(DeviceStateBusiness.ColBoardErrorSub==0)
									{
										sysVPMissionCR.send.msg[i++] = 0x07;
										sysVPMissionCR.send.msg[i++] = 0xd1;
									}
									else
									{
										sysVPMissionCR.send.msg[i++] = 0x07;
										sysVPMissionCR.send.msg[i++] = 0xd2;
									}	
								}
								

								//3.升降机模块
								//2柜
								if((SystemPara.SubBinOpen==1)&&((SystemPara.SubChannel==2)||(SystemPara.SubChannel==3)||(SystemPara.SubChannel==4)))	
								{
									sysVPMissionCR.send.msg[i++] = 0x0b;
									sysVPMissionCR.send.msg[i++] = 0xb9;
								}
								
								
								//4.货道全不可用
								if(DeviceStateBusiness.Emp_Gol)
								{
									sysVPMissionCR.send.msg[i++] = 0x07;
									sysVPMissionCR.send.msg[i++] = 0xd6;
								}
							}
																			
							//8.		
							//2柜加热模块故障
							if(acdc_status_API(2,3)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x58;
							}	
							else if(acdc_status_API(2,3)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x59;
								//读取2柜温度
								sysVPMissionCR.send.msg[i++] = 0xff;
								sysVPMissionCR.send.msg[i++] = 0x01;
								sysVPMissionCR.send.msg[i++] = 0xfe;
								sysVPMissionCR.send.msg[i++] = 0xfe;
							}
							else if(acdc_status_API(2,3)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5a;
							}								
							//2柜制冷模块故障
							if(acdc_status_API(2,2)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5e;
							}	
							else if(acdc_status_API(2,2)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x5f;
								//读取1柜温度
								sysVPMissionCR.send.msg[i++] = 0xff;
								sysVPMissionCR.send.msg[i++] = 0x03;
								sysVPMissionCR.send.msg[i++] = 0xfe;
								sysVPMissionCR.send.msg[i++] = 0xfe;
							}
							else if(acdc_status_API(2,2)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x60;
							}								
							//2柜照明块故障
							if(acdc_status_API(2,1)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x64;
							}	
							else if(acdc_status_API(2,1)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x65;
							}
							else if(acdc_status_API(2,1)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x66;
							}							
							//2柜制除臭块故障
							if(acdc_status_API(2,4)==0)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6a;
							}	
							else if(acdc_status_API(2,4)==1)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6b;
							}
							else if(acdc_status_API(2,4)==2)
							{
								sysVPMissionCR.send.msg[i++] = 0x1b;
								sysVPMissionCR.send.msg[i++] = 0x6c;
							}
						
						}
						
						sysVPMissionCR.send.datLen = i; 
						break;
				}
				/*
                sysVPMissionCR.send.datLen  = 9;
                sysVPMissionCR.send.msg[0]  = 6; 
				
                sysVPMissionCR.send.msg[1]  = TradeCounter.TotalSellNum/256/256/256;       
                sysVPMissionCR.send.msg[2]  = TradeCounter.TotalSellNum/256/256%256;       
                sysVPMissionCR.send.msg[3]  = TradeCounter.TotalSellNum/256%256;       
                sysVPMissionCR.send.msg[4]  = TradeCounter.TotalSellNum%256; 

				sysVPMissionCR.send.msg[5]  = TradeCounter.TotalSellMoeny/256/256/256;		 
				sysVPMissionCR.send.msg[6]  = TradeCounter.TotalSellMoeny/256/256%256;		 
				sysVPMissionCR.send.msg[7]  = TradeCounter.TotalSellMoeny/256%256;		 
				sysVPMissionCR.send.msg[8]  = TradeCounter.TotalSellMoeny%256;	
				*/

			}
			break;			
			case VP_OFFLINEDATA_RPT:
            {
				if(sysVPMissionCR.type==0)
				{
	                sysVPMissionCR.send.msgType = VP_OFFLINEDATA_RPT;  	                
	                sysVPMissionCR.send.msg[index++]  = sysVPMissionCR.type; 
					if(ReadLogDetailAPI(LogPara.offDetailPage))
					{
						for(i = 0,j = 0;i < 8;i++,j += 3)
						{
							if(LogParaDetail.ColumnNo[j] == 0) break;
							sysVPMissionCR.send.msg[index++] = LogParaDetail.ColumnNo[j] - 'A';
							sysVPMissionCR.send.msg[index++] = (INT8U)(LogParaDetail.ColumnNo[j+1] - '0') * 10 + (INT8U)(LogParaDetail.ColumnNo[j+2] - '0');
							sysVPMissionCR.send.msg[index++] = LogParaDetail.GoodsNo[i];
							sysVPMissionCR.send.msg[index++] = HUINT16(LogParaDetail.PriceNo[i]);
							sysVPMissionCR.send.msg[index++] = LUINT16(LogParaDetail.PriceNo[i]);
							
							k = LogParaDetail.PayMode[i];						
							if(k == 0)
								k = 0;
							else if(k == 1)
								k = 5;
							else if((k == 5) || ((k >= 101) && (k <= 255)))//刷卡出货
								k = 7;
							else if(((k >= 2) && (k <= 4)) || ((k >= 6) && (k <= 100)))//在线出货
								k = 6;												
							sysVPMissionCR.send.msg[index++] = k;
							sysVPMissionCR.send.msg[index++] = LogParaDetail.TransSucc[i];
							sysVPMissionCR.send.msg[index++] = LogParaDetail.run_no[i];						
						
						}
					}
					sysVPMissionCR.send.datLen  = index;
					
				}	
				else if(sysVPMissionCR.type==1)
				{
	                sysVPMissionCR.send.msgType = VP_OFFLINEDATA_RPT;  
	                sysVPMissionCR.send.datLen  = 5;
	                sysVPMissionCR.send.msg[0]  = sysVPMissionCR.type;    
					
					tempMoney = MoneySend_CR(LogPara.offLineMoney);				
	                sysVPMissionCR.send.msg[1]  = tempMoney/256;
					sysVPMissionCR.send.msg[2]  = tempMoney%256;
					sysVPMissionCR.send.msg[3]  = LogPara.offLineNum/256;
					sysVPMissionCR.send.msg[4]  = LogPara.offLineNum%256;
					LogPara.offLineMoney = 0;
					LogPara.offLineNum = 0;	
				}
				
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
	}
	else
	{
		sysVPMissionCR.send.sn=GoodsSNCR;
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
	flag = VPMsgPackSend_CR( VP_VMC_SETUP,1);
	//DisplayStr( 0, 0, 1, "2", 1 );  
	//WaitForWork(2000,NULL);
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}

	while( retry )
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
	
	return VP_ERR_NULL;
}
#if 0

/*********************************************************************************************************
** Function name:     	VP_CMD_HuodaoPar
** Descriptions:	    设置货道商品ID信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_HuodaoPar( void )
{	
	if(SystemPara.EasiveEnable == 1)
	{
		//uint8_t i;
		if( sysVPMissionCR.receive.datLen == COLUMN_NUM_SET+1 )
		{
	        if(SystemPara.EasiveEnable == 1)
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		        {
			    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
		        }
			}
			else
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
			    {
					VPMsgPackSend_CR( VP_ACK_RPT, 0 );
				}
			}
			TracePC("\r\n DrvHuodaoIDSet changed by yoc");
			hd_set_by_pc(sysVPMissionCR.receive.msg[0],COLUMN_NUM_SET,(void *)&sysVPMissionCR.receive.msg[1],1);
			//ChannelGetPCChange(1,1,sysVPMissionCR.receive.msg);
			//for( i=0; i<COLUMN_NUM_SET; i++ )
			//{
			//	Trace("\r\n gID=%d",sysVPMissionCR.receive.msg[i+1]);
			//}
			//-------------------------------------------------
			//1...       48  49...        96
			//colNum_1...48  colGoods_1...48
		/*	for( i=0; i<COLUMN_NUM_SET; i++ )
			{
				if( (sysVPMissionCR.receive.msg[i+1]==0xff)||(sysVPMissionCR.receive.msg[i+1]==0x0) )
	            {
	                GoodsWaySetVal[i].GoodsCurrentSum = 0;
	                GoodsWaySetVal[i].GoodsType = sysVPMissionCR.receive.msg[i+1];
					GoodsWaySetVal[i].Price = 0;
	            }
	            else
	            {
					//GoodsWaySetVal[i].GoodsCurrentSum = sysVPMissionCR.receive.msg[i];
					GoodsWaySetVal[i].GoodsType = sysVPMissionCR.receive.msg[i+1];
	            }
			}*/
			//sysVPMissionCR.sysStatus |= VPM_STA_COLUMNPAR;
			
			
		}
		else
		{
			VPMsgPackSend_CR( VP_NAK_RPT, 0);  
			return VP_ERR_PAR;
		}
	}
	else
	{
		if(SystemPara.EasiveEnable == 1)
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
	        {
		    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
	        }
		}
		else
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		    {
				VPMsgPackSend_CR( VP_ACK_RPT, 0 );
			}
		}
	}
	
    return VP_ERR_NULL;	
}	


/*********************************************************************************************************
** Function name:     	VP_CMD_HuodaoNo
** Descriptions:	    设置货道商品存货数量信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_HuodaoNo( void )
{
    unsigned char i = 0;

	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.datLen == COLUMN_NUM_SET+1 )
		{
	        if(SystemPara.EasiveEnable == 1)
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		        {
			    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
		        }
			}
			else
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
			    {
					VPMsgPackSend_CR( VP_ACK_RPT, 0 );
				}
			}	
			TracePC("\r\n DrvHuodaoNOSet");
			//-------------------------------------------------
			//1...       48  49...        96
			//colNum_1...48  colGoods_1...48		
			for( i=0; i<COLUMN_NUM_SET; i++)
			{
				//if( (GoodsWaySetVal[i].GoodsType!=0xff)&&(GoodsWaySetVal[i].GoodsType!=0x0) )
	            {
					sysVPMissionCR.receive.msg[i+1] = (sysVPMissionCR.receive.msg[i+1]>63)?63:(sysVPMissionCR.receive.msg[i+1]&0x3f);				
				}
			}	
			//changed by yoc
			hd_set_by_pc(sysVPMissionCR.receive.msg[0],COLUMN_NUM_SET,(void *)&sysVPMissionCR.receive.msg[1],2);	
			//ChannelGetPCChange(1,3,sysVPMissionCR.receive.msg);
		}
		else
		{
			VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			return VP_ERR_PAR;
		}
	}
	else
	{
		if(SystemPara.EasiveEnable == 1)
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
	        {
		    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
	        }
		}
		else
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		    {
				VPMsgPackSend_CR( VP_ACK_RPT, 0 );
			}
		}
	}
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VP_CMD_GoodsPar
** Descriptions:	    设置货道商品单价信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_GoodsPar( void )
{
	unsigned char i = 0;
	unsigned char j = 0;
	//static u_int  tempMoney = 0;	
	unsigned char Binunm = 0;
	
	
	//if( sysVPMissionCR.receive.datLen == (GOODSTYPEMAX+GOODSTYPEMAX*2)	)
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.datLen <= (COLUMN_NUM_SET*2+2) )
		{	
			if(SystemPara.EasiveEnable == 1)
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		        {
			    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
		        }
			}
			else
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
			    {
					VPMsgPackSend_CR( VP_ACK_RPT, 0 );
				}
			}	
			Binunm = sysVPMissionCR.receive.msg[0];
			TracePC("\r\n DrvPriceSet");
			//----------------------------------
			//	1...			  n
			//SP1_Price...Sn_Price
			GoodsRev = (sysVPMissionCR.receive.datLen - 2) / 2;	
			if(sysVPMissionCR.receive.msg[1]==0)//根据ID设置商品单价
			{
				for( i=0,  j=2; i<GoodsRev; i++, j+=2 )
				{
					sysGoodsMatrix[i].GoodsType = i+1;			
					if((sysVPMissionCR.receive.msg[j]==0xff)&&(sysVPMissionCR.receive.msg[j+1]==0xff))
					{
						sysGoodsMatrix[i].Price  = 0;
					}
					else
					{				
						sysGoodsMatrix[i].Price  = MoneyRec_CR(sysVPMissionCR.receive.msg[j],sysVPMissionCR.receive.msg[j+1]);				
					}			
					//Trace("\r\n id=%d,[%ld]",sysGoodsMatrix[i].GoodsType,sysGoodsMatrix[i].Price);
				}	
				//changed by yoc
				hd_set_by_pc(Binunm,GoodsRev,(void *)sysGoodsMatrix,3);//根据ID设置商品单价 
				#if 0
				for(j=0;j<GoodsRev;j++)
				{
					hd_set_price_by_id(Binunm,sysGoodsMatrix[j].GoodsType,sysGoodsMatrix[j].Price);
				}
				#endif
				//sysVPMissionCR.sysStatus |= VPM_STA_GOODSPAR; 
			}
			else 
			if(sysVPMissionCR.receive.msg[1]==1)//根据货道设置商品单价
			{
				for( i=0,  j=2; i<GoodsRev; i++, j+=2 )
				{
					sysGoodsMatrix[i].GoodsType = i+1;			
					if((sysVPMissionCR.receive.msg[j]==0xff)&&(sysVPMissionCR.receive.msg[j+1]==0xff))
					{
						sysGoodsMatrix[i].Price  = 0;
					}
					else
					{				
						sysGoodsMatrix[i].Price  = MoneyRec_CR(sysVPMissionCR.receive.msg[j],sysVPMissionCR.receive.msg[j+1]);				
					}			
				}
				//changed by yoc
				hd_set_by_pc(Binunm,GoodsRev,(void *)sysGoodsMatrix,4);
#if 0
				j=0;
				for(i=0;i<8;i++)
				{
					for(n=0;n<10;n++)
					{	
						if(n >= 9 )
							logicNo = (i + 1) * 10;
						else
							logicNo = (i + 1) * 10 + n + 1;
						if(ChannelGetPhysicNum(logicNo,Binunm) == 0xff)
						{
							j++;
							continue;
						}	
						else
						{
							TracePC("logicNo = %d ,price[%d].Price = %d\r\n",logicNo,j,sysGoodsMatrix[j].Price);
							ChannelSetParam(logicNo,Binunm,CHANNELPRICE,0,sysGoodsMatrix[j++].Price);	
						}
							
					}
				}	
				//sysVPMissionCR.sysStatus |= VPM_STA_GOODSPAR; 
#endif

			}
			
		}
		else
		{
			VPMsgPackSend_CR( VP_NAK_RPT, 0);  
			return VP_ERR_PAR;
		}
	}
	else
	{
		if(SystemPara.EasiveEnable == 1)
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
	        {
		    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
	        }
		}
		else
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		    {
				VPMsgPackSend_CR( VP_ACK_RPT, 0 );
			}
		}
	}
	return VP_ERR_NULL; 
}


/*********************************************************************************************************
** Function name:     	VP_CMD_Position
** Descriptions:	    设置各个展示位
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_Position( void )
{
    //if( sysVPMissionCR.receive.datLen == (GOODSTYPEMAX+GOODSTYPEMAX*2)  )
    if(SystemPara.EasiveEnable == 1)
    {
	    if( sysVPMissionCR.receive.datLen == 18+1)
		{
	        if(SystemPara.EasiveEnable == 1)
			{
				if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		        {
			    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
		        }
			}
			else
			{
			    if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		        {
			    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
		        }
			}
			TracePC("\r\n DrvPosIDSet");
			/*
			for( i=0; i<GOODSTYPEMAX; i++)
			{
				//sysVPMissionCR.selItem[i] = sysVPMissionCR.receive.msg[i+1];
				SystemParameter.selItem[i] = sysVPMissionCR.receive.msg[i+1];
			}
			*/
		}
		else
		{
		    VPMsgPackSend_CR( VP_NAK_RPT, 0 );	 //1, 0
			return VP_ERR_PAR;
		}
    }
	else
	{
		if(SystemPara.EasiveEnable == 1)
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
	        {
		    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
	        }
		}
		else
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		    {
				VPMsgPackSend_CR( VP_ACK_RPT, 0 );
			}
		}
	}
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VPMission_HuodaoSta_RPT
** Descriptions:	    上报各个货道状态
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_ColumnSta_RPT( void )
{
	unsigned char bin=0;
	unsigned char i = 0;
	unsigned char flag = 0; 


	if(SystemPara.EasiveEnable == 1)
	{
		bin = sysVPMissionCR.receive.msg[0];

		//2.send huodao data
		//retry = VP_COM_RETRY;
		//--------------------------------------------
		for(i=0; i<COLUMN_NUM_SET+1; i++)
		{
			sysVPMissionCR.send.msg[i] = 0;
		}
		sysVPMissionCR.send.msg[0] = bin;
		hd_huodao_rpt_vp(bin,&sysVPMissionCR.send.msg[1]);
		flag = VPMsgPackSend_CR( VP_HUODAO_RPT, 0);   //Not need ACK
		if( flag != VP_ERR_NULL )
		{
			return VP_ERR_PAR;
		}	
		/*
		while( retry )
		{
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if( VPBusFrameUnPack_CR() )
				{			
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			LogPara.offLineFlag = 1;	
			return VP_ERR_COM;
		}
		*/

		switch( sysVPMissionCR.receive.msgType )
		{
			default:
				break;
		}
	}	
	else
	{
		sysVPMissionCR.send.msg[0] = bin;
		sysVPMissionCR.send.msg[1] = bin;
		flag = VPMsgPackSend_CR( VP_HUODAO_RPT, 0);   //Not need ACK
		if( flag != VP_ERR_NULL )
		{
			return VP_ERR_PAR;
		}
	}
	
		
	
	
	return VP_ERR_NULL;

}

/*********************************************************************************************************
** Function name:     	VPMission_HuodaoSta_RPT
** Descriptions:	    上报各个货道状态
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_GetColumnSta( void )
{
	/*
	//1.Check the data	
    if( sysVPMissionCR.receive.datLen == 0  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0  );	 
		//return VP_ERR_PAR;
	}
	if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK  )
	{
		VPMsgPackSend_CR( VP_ACK_RPT, 0  );
	}
	*/
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	OSTimeDly(OS_TICKS_PER_SEC / 10);
	
	//2.
    VPMission_ColumnSta_RPT();	
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VPMission_Status_RPT
** Descriptions:	    上报机器状态
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Status_RPT( uint8_t check_st,uint8_t bv_st,uint8_t cc_st,uint8_t vmc_st,uint16_t change,uint8_t tem_st,uint16_t *recyclerSum,uint16_t *coinSum )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

	
    retry = VP_COM_RETRY;
	
	
	if(SystemPara.EasiveEnable == 1)
	{
		sysVPMissionCR.check_st = check_st;
		sysVPMissionCR.bv_st = bv_st;
		sysVPMissionCR.cc_st = cc_st;
		sysVPMissionCR.vmc_st = vmc_st;
		sysVPMissionCR.change = change;
		sysVPMissionCR.tem_st = tem_st;
		sysVPMissionCR.recyclerSum[0] = recyclerSum[0];
		sysVPMissionCR.recyclerSum[1] = recyclerSum[1];
		sysVPMissionCR.recyclerSum[2] = recyclerSum[2];
		sysVPMissionCR.recyclerSum[3] = recyclerSum[3];
		sysVPMissionCR.recyclerSum[4] = recyclerSum[4];
		sysVPMissionCR.recyclerSum[5] = recyclerSum[5];
		sysVPMissionCR.coinSum[0] = coinSum[0];
		sysVPMissionCR.coinSum[1] = coinSum[1];
		sysVPMissionCR.coinSum[2] = coinSum[2];
		sysVPMissionCR.coinSum[3] = coinSum[3];
		sysVPMissionCR.coinSum[4] = coinSum[4];
		sysVPMissionCR.coinSum[5] = coinSum[5];
	}
	else
	{
		sysVPMissionCR.check_st = 0;
		sysVPMissionCR.bv_st = bv_st;
		sysVPMissionCR.cc_st = 0; 
		sysVPMissionCR.vmc_st = vmc_st;
		if(change>0)
			sysVPMissionCR.change = 50000;
		else
			sysVPMissionCR.change = 0;
		sysVPMissionCR.tem_st = tem_st;
		sysVPMissionCR.recyclerSum[0] = 0xfe;
		sysVPMissionCR.recyclerSum[1] = 0xfe;
		sysVPMissionCR.recyclerSum[2] = 0xfe;
		sysVPMissionCR.recyclerSum[3] = 0xfe;
		sysVPMissionCR.recyclerSum[4] = 0xfd;
		sysVPMissionCR.recyclerSum[5] = SystemPara.SaleTime;
		if(sysVPMissionCR.change>0)
			sysVPMissionCR.coinSum[0] = 50;
		else
			sysVPMissionCR.coinSum[0] = 0;
		sysVPMissionCR.coinSum[1] = 0;
		sysVPMissionCR.coinSum[2] = 0;
		sysVPMissionCR.coinSum[3] = 0;
		sysVPMissionCR.coinSum[4] = 0;
		sysVPMissionCR.coinSum[5] = 0;
	}
	if(SystemPara.EasiveEnable == 1)
	{
		//==============================================================
		flag = VPMsgPackSend_CR( VP_STATUS_RPT, 1);  //1-0, not need ACK
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
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			if(LogPara.offLineFlag == 0)
			{
				LogPara.offLineFlag = 1;
				LogPara.offDetailPage = LogPara.LogDetailPage;
			}
	        return VP_ERR_COM;
		}

	    switch( sysVPMissionCR.receive.msgType )
		{
			default:
			    break;
		}
	}
	else
	{
		//==============================================================
		flag = VPMsgPackSend_CR( VP_STATUS_RPT, 0);  //1-0, not need ACK
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
	}
	//------------------------------------------------------
    /*//1.硬币器状态cc_st
	//if( DeviceStatus.CoinAccepter != 0 )
	if(sysVPMissionCR.billCoinEnable == 0)
	{
        sysVPMissionCR.STA_CoinA = 1;
    }	
    else if( sysMDBMission.coinDeviceStatus != 0 )
	{
        sysVPMissionCR.STA_CoinA = 2;
    }
    //币管没关紧，上报故障;by gzz 20110902
	else if( sysMDBMission.tubeRemoved == 1 )
	{
		sysVPMissionCR.STA_CoinA = 2;
	}
	else if(sysMDBMission.coinIsEnable == 0)
	{
        sysVPMissionCR.STA_CoinA = 1;
    }	
	else
	{
		sysVPMissionCR.STA_CoinA = 0;
	}

    //2.机器中当前可用于找零的5角硬币量cnt5j
    //if( DeviceStatus.ChangeUnit1 == 0 )
    if( ( sysMDBMission.coinDeviceStatus == 0x00 )&&(sysMDBMission.coin5jNum > 0) )
    	sysVPMissionCR.STA_ChangeNum1 = sysMDBMission.coin5jNum;   //VP_STA_CHANGE_ENOUGH;
    else
	    sysVPMissionCR.STA_ChangeNum1 = 0;

    //3.机器中当前可用于找零的1元硬币量cnt1y
    //if( DeviceStatus.ChangeUnit2 == 0 )
    if( ( sysMDBMission.coinDeviceStatus == 0x00 )&&(sysMDBMission.coin1yNum > 0) )
    	sysVPMissionCR.STA_ChangeNum2 = sysMDBMission.coin1yNum;   //VP_STA_CHANGE_ENOUGH;
    else
	    sysVPMissionCR.STA_ChangeNum2 = 0;

    //4.纸币器状态bv_st
	//1. 
	if( SystemParameter.BillNo == 1 )
	{
	    //if( DeviceStatus.BillAccepter == 0 )
	    if(sysVPMissionCR.billCoinEnable == 0)
		{
	        sysVPMissionCR.STA_BillA = 1;
	    }	
		else if(sysMDBMission.billCashBuf == 1)//钞箱移除报故障;by gzz 20121224
	    {
			if( sysITLMission.billHoldingFlag == 1 )
			{
				CurrentMoney = CurrentPayed + sysITLMission.billHoldingValue;
			}
			else
			{
				CurrentMoney = CurrentPayed;
			}
			if( (CurrentMoney == 0)&&( sysITLMission.billHoldingFlag == 0 ) )
			{
				sysVPMissionCR.STA_BillA = 2;	
			}
			else
			{
				sysVPMissionCR.STA_BillA = 0;	
			}				
		}		
	    else if( sysMDBMission.billDeviceStatus == 0 )
		{
			if(sysMDBMission.billIsEnable == 0)
			{
				sysVPMissionCR.STA_BillA = 1;			
			}
			else
			{
				sysVPMissionCR.STA_BillA = 0;
			}
            
			sysVPMissionCR.STA_BillCount = 0;//by gzz 20110905
		}
        //1)如果发现此次纸币器和硬币器同时故障，则全部上报
        //2)pc轮询时，如果只有纸币器故障，给予累加，只有连续故障次数大于5次时，才上报
		//by gzz 20110905
		else if(( sysMDBMission.coinDeviceStatus != 0 )||( sysVPMissionCR.STA_BillCount >= 1 ))
		{
			sysVPMissionCR.STA_BillA = 2;			
		}		
		else//by gzz 20110905 
		{
		    sysVPMissionCR.STA_BillA = 0;
			sysVPMissionCR.STA_BillCount ++;
		}
	}
	else
	{
		sysVPMissionCR.STA_BillA = 3;
	}

    //5.找零设备状态ch1_st,ch2_st
	//2.
    if( SystemParameter.HopperCoinPrice1 > 0 )
	{
		//if( DeviceStatus.ChangeUnit1 == 1 )
        if( sysMDBMission.coinDeviceStatus != 0 )
		{
			sysVPMissionCR.STA_Changer1 = 2;
		}
		//币管没关紧，上报故障;by gzz 20110901
		else if( sysMDBMission.tubeRemoved == 1 )
		{
			sysVPMissionCR.STA_Changer1 = 2;
		}
		else
		{
			sysVPMissionCR.STA_Changer1 = 0;
		}
	}
	else
	{
		sysVPMissionCR.STA_Changer1 = 3;
	}
    //
	if( SystemParameter.HopperCoinPrice2 > 0 )
	{
	    
		//if( DeviceStatus.ChangeUnit2 == 1 )
        if( sysMDBMission.coinDeviceStatus != 0 )
		{
			sysVPMissionCR.STA_Changer2 = 2;
		}
		//币管没关紧，上报故障;by gzz 20110901
		else if( sysMDBMission.tubeRemoved == 1 )
		{
			sysVPMissionCR.STA_Changer2 = 2;
		}
		else
		{
			sysVPMissionCR.STA_Changer2 = 0;
		}

	}
	else
	{
		sysVPMissionCR.STA_Changer2 = 3;
	}
    
    //6.主控器状态vmc_st
	//只有机器因故障而不能工作后，才上报故障信息;by gzz 20110902
	//if( HardWareErr == 0 )
	if(( HardWareErr & 0x0010 )||( HardWareErr & 0x0080 ))
    {
	    sysVPMissionCR.STA_VMC = 1;
	}
	//else if(( HardWareErr & 0x0020 )||( HardWareErr & 0x0040 )||( HardWareErr & 0x0100 )
	//	   ||( HardWareErr & 0x0200 )||( HardWareErr & 0x1000 )||( HardWareErr & 0x2000 ))
	else if( HardWareErr != 0)
    {
	    sysVPMissionCR.STA_VMC = 2;
	}
	else if(sysVPMissionCR.adminType == 1)
	{
		sysVPMissionCR.STA_VMC = 3;
	}
    else
	{
		sysVPMissionCR.STA_VMC = 0;
	}
	
	
	
    //7.选货1,选货2,选货3状态
	//sysVPMissionCR.STA_Tep1    = 0x7f;
    //sysVPMissionCR.STA_Tep2    = 0x7f;
	if( DeviceStatus.Selection[0] == 0 )
	{
		sysVPMissionCR.STA_Tep1 = 0;
	}
	else
	{
		sysVPMissionCR.STA_Tep1 = 2;	
	}
    //
    if( DeviceStatus.Selection[1] == 0 )
	{
		sysVPMissionCR.STA_Tep2 = 0;
	}
	else
	{
		sysVPMissionCR.STA_Tep2 = 2;	
	}
    
	//sysVPMissionCR.STA_Bank   = 3;
	if( DeviceStatus.Selection[2] == 0 )
	{
		sysVPMissionCR.STA_Tep3 = 0;
	}
	else
	{
		sysVPMissionCR.STA_Tep3 = 2;	
	}
    

    

	//8.出货检测
	//sysVPMissionCR.STA_ICCard    = 3;
	if( SystemParameter.GOCOn == 1 )
	{
        //只有确认出货检测故障，才发送故障信息给pc端;by cq 20110815
        / *	            
        if( DeviceStatus.GOCStatus & 0x01 )
		{
			sysVPMissionCR.STA_ICCard = 2;
		}
		else 
		{
			sysVPMissionCR.STA_ICCard = 0;
		} 
        * /
        if( DeviceStatus.GOCStatus == 0 )
        {
        	sysVPMissionCR.STA_ICCard = 0;
            sysVPMissionCR.STA_ICCardCount = 0;//by gzz 20111012
        }
        else if(( DeviceStatus.GOCStatus & 0x01 )&&( sysVPMissionCR.STA_ICCardCount > 5 ))
        {
			sysVPMissionCR.STA_ICCard = 2;
		}
        else 
		{
			sysVPMissionCR.STA_ICCard = 0;
            sysVPMissionCR.STA_ICCardCount ++;
		} 
	}
	else
	{
		sysVPMissionCR.STA_ICCard = 1;
	}	
	//9.货仓1状态
	if(sysVPMissionCR.ACDCCompressorCtr == 1)
		sysVPMissionCR.ESTA_Tem1 = 1;
	else
		sysVPMissionCR.ESTA_Tem1 = 0;
	//10.货仓1温度tem1
	if( SystemParameter.ACDCModule == 1 )
	{
        if( DeviceStatus.ACDCModule == 0x00 )
		{
			sysVPMissionCR.E_Tem1 = 0xfd;//0xfd表示温度无意义;by gzz 20110721
		}
		else
		{
			sysVPMissionCR.E_Tem1 = 0xff;//故障
		}
	}
	else
	{  
		sysVPMissionCR.E_Tem1 = 0xfe;//不存在此货仓
	}
	//11.货仓2,3,4状态
	sysVPMissionCR.ESTA_Tem2 = 3;//不存在此货仓
	//12，货仓2,3,4温度tem2,表示
	sysVPMissionCR.E_Tem2 = 0xfe;//不存在此货仓

    //13交易时间
	sysVPMissionCR.Payout_Time = SystemParameter.tradeTime;

	//==============================================================
//TAB_VPM_STATUS_RETRY:
    flag = VPMsgPackSend_CR( VP_STATUS_RPT, 0, 1);  //1-0, not need ACK
    if( flag != VP_ERR_NULL )
    {
		return VP_ERR_PAR;
	}
	*/
	/*
	sysVPMissionCR.msTimer2 = VP_TIME_OUT;
	while( sysVPMissionCR.msTimer2 )
	{
		if( VPBusFrameUnPack_CR() )
		{
		    sysVPMissionCR.comErrNum = 0;
			goto TAB_VPM_STATUS_COM_OK;
		}
	}
	if( retry-- )
	{
		goto TAB_VPM_STATUS_RETRY;
	}
	else
	{
		sysVPMissionCR.comErrNum++;
		//? 
        return VP_ERR_COM;
	}
TAB_VPM_STATUS_COM_OK:
    switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	*/
	return VP_ERR_NULL;

}


/*********************************************************************************************************
** Function name:     	VP_CMD_GetStatus
** Descriptions:	    需要上报状态信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_GetStatus( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;

	//1.Check the data
    if( sysVPMissionCR.receive.datLen != 0  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0 );	 
		return VP_ERR_PAR;
	}
	//发送邮箱给vmc
	//MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_STATUSIND;				
	//OSMboxPost(g_Ubox_PCTOVMCMail,&MsgUboxPack[g_Ubox_Index]);	
	//UpdateIndex();
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
	TracePC("\r\n Drv Payout_Ind ACK"); 
	//VPMsgPackSend_CR( VP_ACK_RPT, 0  );
	OSTimeDly(OS_TICKS_PER_SEC/10);
	StatusRPTAPI();
	/*
	//取得返回值
	AccepterUboxMsg = OSMboxPend(g_Ubox_PCTOVMCBackCMail,OS_TICKS_PER_SEC*10,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv Payout_Ind ACK"); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Payout_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}
	}	
	*/
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VPMission_Info_RPT
** Descriptions:	    上报Info信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Info_RPT( unsigned char type, unsigned int payAllMoney,uint8_t check_st,uint8_t bv_st,uint8_t cc_st,uint8_t Control_Huodao,uint8_t value,uint8_t Control_device,uint8_t  cabinetNums,uint8_t *cabinetdata )
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
	sysVPMissionCR.payAllMoney = payAllMoney;
	sysVPMissionCR.check_st = check_st;
	sysVPMissionCR.bv_st = bv_st;
	sysVPMissionCR.cc_st = cc_st;
	sysVPMissionCR.Control_Bin = Control_device;
	sysVPMissionCR.Control_Huodao = Control_Huodao;
	sysVPMissionCR.value = value;
	sysVPMissionCR.cabinetNums = cabinetNums;
	memcpy(sysVPMissionCR.cabinetdata,(uint8_t *)cabinetdata,30);
	if(SystemPara.EasiveEnable == 1)
	{
		//===========================================
		flag = VPMsgPackSend_CR( VP_INFO_RPT, 1);
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
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			if(LogPara.offLineFlag == 0)
			{
				LogPara.offLineFlag = 1;
				LogPara.offDetailPage = LogPara.LogDetailPage;
			}
	        return VP_ERR_COM;
		}
	    switch( sysVPMissionCR.receive.msgType )
		{
			default:
			    break;
		}
	}
	else
	{
		//===========================================
		flag = VPMsgPackSend_CR( VP_INFO_RPT, 0);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
	}
	return VP_ERR_NULL;
}


/*********************************************************************************************************
** Function name:     	VP_CMD_GetInfo
** Descriptions:	    需要上报Info信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_GetInfo( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;

	//1.Check the data
    if( sysVPMissionCR.receive.datLen != 1  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0 );	 
		return VP_ERR_PAR;
	}
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
	switch(sysVPMissionCR.receive.msg[0])
	{
		case 24:
		case 25:
			//发送邮箱给vmc
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_INFOIND;	
			MsgUboxPack[g_Ubox_Index].Type = sysVPMissionCR.receive.msg[0];
			OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/10);
			/*
			//取得返回值
			AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				switch(AccepterUboxMsg->PCCmd)
				{
					case MBOX_VMCTOPC_ACK:
						TracePC("\r\n Drv Info_Ind ACK"); 
						VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
						break;
					case MBOX_VMCTOPC_NAK:
						TracePC("\r\n Drv Info_Ind NAK"); 
						VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
						break;	
				}
			}*/	
			break;
		default:
			//TracePC("\r\n Drv Info_Ind ACK"); 
			//VPMsgPackSend_CR( VP_ACK_RPT, 0  );
			//OSTimeDly(OS_TICKS_PER_SEC/10);
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_INFORPT;		
			MsgUboxPack[g_Ubox_Index].Type = sysVPMissionCR.receive.msg[0];
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);	
			break;
	}
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VP_CMD_GetInfoExp
** Descriptions:	    (扩展)需要上报Info信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_GetInfoExp( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;

	//1.Check the data
    if( sysVPMissionCR.receive.datLen > 2  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0 );	 
		return VP_ERR_PAR;
	}
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
	switch(sysVPMissionCR.receive.msg[0])
	{
		case 24:
		case 25:
			//发送邮箱给vmc
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_INFOIND;	
			MsgUboxPack[g_Ubox_Index].Type = sysVPMissionCR.receive.msg[0];
			MsgUboxPack[g_Ubox_Index].Control_device = sysVPMissionCR.receive.msg[1];
			OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/10);
			/*//取得返回值
			AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*10,&ComStatus);
			if(ComStatus == OS_NO_ERR)
			{
				switch(AccepterUboxMsg->PCCmd)
				{
					case MBOX_VMCTOPC_ACK:
						TracePC("\r\n Drv Info_Ind ACK"); 
						VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
						break;
					case MBOX_VMCTOPC_NAK:
						TracePC("\r\n Drv Info_Ind NAK"); 
						VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
						break;	
				}
			}*/		
			break;
		case 26:
			//TracePC("\r\n Drv Info_Ind ACK"); 
			//VPMsgPackSend_CR( VP_ACK_RPT, 0  );
			//OSTimeDly(OS_TICKS_PER_SEC/10);
			MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_VMCTOPC_INFORPT;		
			MsgUboxPack[g_Ubox_Index].Type = sysVPMissionCR.receive.msg[0];
			MsgUboxPack[g_Ubox_Index].Control_device=sysVPMissionCR.receive.msg[1];
			OSQPost(g_Ubox_VMCTOPCQ,&MsgUboxPack[g_Ubox_Index]);
			UpdateIndex();
			OSTimeDly(OS_TICKS_PER_SEC/100);	
			break;
	}
	
    return VP_ERR_NULL;	
}


/*********************************************************************************************************
** Function name:     	VP_CMD_SetHuodao
** Descriptions:	    设置单独的货道信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_CMD_SetHuodao( void )
{	 
	unsigned char i = 0,j = 0,GoodsRev = 0;
	//uint8_t i;
	if( sysVPMissionCR.receive.datLen <= 80 )
	{
		//Trace("\r\n getaction4=%d",( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK ));
		if(SystemPara.EasiveEnable == 1)
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
			{
				VPMsgPackSend_CR( VP_ACK_RPT, 0);
			}
		}
		else
		{
			if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
			{
				VPMsgPackSend_CR( VP_ACK_RPT, 0 );
			}
		}
		TracePC("\r\n DrvHuodaoAttrSet");
		GoodsRev = sysVPMissionCR.receive.datLen/8;
		for( i=0,  j=0; i<GoodsRev; i++, j+=8 )
		{
			sysGoodsAttr[i].Cabinet = sysVPMissionCR.receive.msg[j];
			sysGoodsAttr[i].hd_no = sysVPMissionCR.receive.msg[j+1];
			sysGoodsAttr[i].hd_id = sysVPMissionCR.receive.msg[j+2];
			sysGoodsAttr[i].hd_count = sysVPMissionCR.receive.msg[j+3];
			if((sysVPMissionCR.receive.msg[j+4]==0xff)&&(sysVPMissionCR.receive.msg[j+5]==0xff))
			{
				sysGoodsAttr[i].hd_Price  = 0;
			}
			else
			{				
				sysGoodsAttr[i].hd_Price  = MoneyRec_CR(sysVPMissionCR.receive.msg[j+4],sysVPMissionCR.receive.msg[j+5]);				
			}
		}
		for( i=0; i<GoodsRev; i++ )
		{
			TracePC("\r\n DrvHuodaoAttr=%d,%d,%d,%d,%d",sysGoodsAttr[i].Cabinet,sysGoodsAttr[i].hd_no,sysGoodsAttr[i].hd_id,sysGoodsAttr[i].hd_count,sysGoodsAttr[i].hd_Price);
		}
		pcSetHuodao(GoodsRev,sysGoodsAttr);
		//-------------------------------------------------
		//1...		 48  49...		  96
		//colNum_1...48  colGoods_1...48
	/*	for( i=0; i<COLUMN_NUM_SET; i++ )
		{
			if( (sysVPMissionCR.receive.msg[i+1]==0xff)||(sysVPMissionCR.receive.msg[i+1]==0x0) )
			{
				GoodsWaySetVal[i].GoodsCurrentSum = 0;
				GoodsWaySetVal[i].GoodsType = sysVPMissionCR.receive.msg[i+1];
				GoodsWaySetVal[i].Price = 0;
			}
			else
			{
				//GoodsWaySetVal[i].GoodsCurrentSum = sysVPMissionCR.receive.msg[i];
				GoodsWaySetVal[i].GoodsType = sysVPMissionCR.receive.msg[i+1];
			}
		}*/
		//sysVPMissionCR.sysStatus |= VPM_STA_COLUMNPAR;
		
		
	}
	else
	{
		VPMsgPackSend_CR( VP_NAK_RPT, 0);	
		return VP_ERR_PAR;
	}
	
	return VP_ERR_NULL; 
}






/*********************************************************************************************************
** Function name:     	VPMission_OfflineData_RPT
** Descriptions:	    上报离线交易信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_OfflineData_RPT( void )
{
	unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;


	if(LogPara.offLineFlag == 1)
	{
		return VP_ERR_NULL;
	}

	TracePC("\r\n Drv Offline=%ld,%ld",LogPara.offDetailPage,LogPara.LogDetailPage); 
	//上报逐条交易记录
	while(LogPara.offDetailPage < LogPara.LogDetailPage)
	{		
		TracePC("\r\n Drv OfflineDe=%ld",LogPara.offDetailPage);
		retry = VP_COM_RETRY;
		sysVPMissionCR.type = 0;
		flag = VPMsgPackSend_CR( VP_OFFLINEDATA_RPT, 1);
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
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			LogPara.offLineFlag = 1;
			//LogPara.offDetailPage = LogPara.LogDetailPage;
			return VP_ERR_COM;
		}			
		else
		{
			LogPara.offDetailPage++;
		}
		OSTimeDly(2);		
	}
	OSTimeDly(10);

	TracePC("\r\n Drv OfflineAll=%ld",LogPara.offDetailPage);
	//上报总交易记录
	retry = VP_COM_RETRY;
	sysVPMissionCR.type = 1;
	flag = VPMsgPackSend_CR( VP_OFFLINEDATA_RPT, 1);
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
				if(LogPara.offLineFlag == 1)
				{
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}
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
		LogPara.offLineFlag = 1;
		//LogPara.offDetailPage = LogPara.LogDetailPage;
		return VP_ERR_COM;
	}

	switch( sysVPMissionCR.receive.msgType )
	{
		default:
			break;
	}	
	
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VP_Payout_Ind
** Descriptions:	    PC指示VMC出币命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Payout_Ind( void )
{	 	
	//MessageUboxPCPack *AccepterUboxMsg;
//	unsigned char ComStatus;

    //1.Check the data
    if( sysVPMissionCR.receive.datLen != 4  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0  );	 
		return VP_ERR_PAR;
	}
	//2.ACK
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
        {
	    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
        }
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	    {
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	    
	sysVPMissionCR.changeMoney = MoneyRec_CR(sysVPMissionCR.receive.msg[1],sysVPMissionCR.receive.msg[2]);
	sysVPMissionCR.type = sysVPMissionCR.receive.msg[3];
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_PAYOUTIND;				
	MsgUboxPack[g_Ubox_Index].changeMoney = sysVPMissionCR.changeMoney;
	MsgUboxPack[g_Ubox_Index].Type        = sysVPMissionCR.type;
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv Payout_Ind"); 
	/*
	//取得返回值
	AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*10,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv Payout_Ind ACK"); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Payout_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}
	}
	*/
	/*//if( (sysVPMissionCR.changeDev != 0) || (sysVPMissionCR.changeMoney < SystemParameter.HopperCoinPrice1 ) )
    if( (sysVPMissionCR.changeDev != 0) || (sysVPMissionCR.changeMoney > sysMDBMission.coinAllValue ) || (sysMDBMission.coinDeviceStatus!=0) )
	{
    	VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}
	//扣钱后不能找零
	tempPrice = sysVPMissionCR.changeMoney/10;
	if(!(changeCNY2(resultdisp,tempPrice)))
	{
    	VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}
	

	//2.ACK
	if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	{
    	VPMsgPackSend_CR( VP_ACK_RPT, 0, 0 );
	}

    //3.Set command flag
	sysVPMissionCR.changeCmd = 1;
	*/
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VP_Cost_Ind
** Descriptions:	    PC指示VMC扣款命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Cost_Ind( void )
{
//	MessageUboxPCPack *AccepterUboxMsg;
//	unsigned char ComStatus;
	//u_char xdata len = 0;
	//u_char xdata str[20];
//	unsigned char ComReturn = 0;

    //1.Check the data
    if( sysVPMissionCR.receive.datLen != 4  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0 );	 
		return VP_ERR_PAR;
	}
	//2.ACK
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
        {
	    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
        }
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	    {
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
		
    sysVPMissionCR.costMoney = MoneyRec_CR(sysVPMissionCR.receive.msg[1],sysVPMissionCR.receive.msg[2]);
	sysVPMissionCR.type = sysVPMissionCR.receive.msg[3];
	
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_COSTIND;				
	MsgUboxPack[g_Ubox_Index].costMoney = sysVPMissionCR.costMoney;
	MsgUboxPack[g_Ubox_Index].Type        = sysVPMissionCR.type;
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv Cost_Ind"); 
	/*
	//取得返回值
	AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*5,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv Cost_Ind ACK"); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Cost_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}
	}	
	*/
	
	/*
	//2.得到当前投入的金额
	if( sysITLMission.billHoldingFlag == 1 )
	{
		CurrentMoney = CurrentPayed + sysITLMission.billHoldingValue;
	}
	else
	{
		CurrentMoney = CurrentPayed;
	}
	//3.用户投币金额小于扣款金额时，返回NAK_RPT    
    if( (sysVPMissionCR.costDev != 0) || (sysVPMissionCR.costMoney > CurrentMoney ) )
	{
		//len = sprintf( str, "1=%lu,%lu", sysVPMissionCR.costMoney,CurrentMoney );
		//DisplayStr( 0, 0, 1, str, strlen(str) );  
		//WaitForWork(2000,NULL);
    	VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}
	//扣钱后不能找零
	if(!(IsCanChange(sysVPMissionCR.costMoney)))
	{
		//DisplayStr( 0, 0, 1, "2", 1 );  
		//WaitForWork(2000,NULL);
    	VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}
	sysVPMissionCR.costType = sysVPMissionCR.receive.msg[3];

	//2.ACK
	if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	{
    	VPMsgPackSend_CR( VP_ACK_RPT, 0, 0 );
	}
	

    //3.Set command flag
	sysVPMissionCR.costCmd = 1;
	*/
    return VP_ERR_NULL;	
}



/*********************************************************************************************************
** Function name:     	VP_Vendout_Ind
** Descriptions:	    PC指示VMC出货命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Vendout_Ind( void )
{
	unsigned char method = 0;
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
    
	//1.Check the data
    if( sysVPMissionCR.receive.datLen != 6  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0  );	 
		return VP_ERR_PAR;
	}
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
	
/*	if(sysVPMissionCR.ErrFlag2 == 1)
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}*/	
    
	method = sysVPMissionCR.receive.msg[1];
	if((method==0)||(method>2))
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0  );	 
		return VP_ERR_PAR;
	}	
	sysVPMissionCR.device = sysVPMissionCR.receive.msg[0];
	sysVPMissionCR.method = method;	
	sysVPMissionCR.Column = sysVPMissionCR.receive.msg[2];
    sysVPMissionCR.type  = sysVPMissionCR.receive.msg[3];
    sysVPMissionCR.costMoney  = MoneyRec_CR(sysVPMissionCR.receive.msg[4], sysVPMissionCR.receive.msg[5]); 
	//发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_VENDOUTIND;	
	MsgUboxPack[g_Ubox_Index].device = sysVPMissionCR.device;
	MsgUboxPack[g_Ubox_Index].method = sysVPMissionCR.method;
	MsgUboxPack[g_Ubox_Index].Column = sysVPMissionCR.Column;
    MsgUboxPack[g_Ubox_Index].Type  = sysVPMissionCR.type;
    MsgUboxPack[g_Ubox_Index].costMoney  = sysVPMissionCR.costMoney; 
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeSet(0);
	TracePC("\r\n Drv %dVendout_Ind",OSTimeGet()); 
	OSTimeDly(OS_TICKS_PER_SEC/10);	
	/*
	//取得返回值
	AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*5,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv %dVendout_Ind ACK",OSTimeGet()); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Vendout_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}
	}	
	*/
	/*

	if(method == 1)
	{
		for( i=0; i<sysVPMissionCR.GoodsRev; i++ )
		{
			if( sysVPMissionCR.receive.msg[2] == sysGoodsMatrix[i].GoodsType )
			{
		    	sel = i;
			    break;
			}
		}

	    if( sel == 0xffff )
		{
	    	//VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	
			//return VP_ERR_PAR;
			//The goods code isn't in the goods window, find in the column matrix
			for( i=0; i<COLUMN_NUM_SET; i++  )
			{
				if( GoodsWaySetVal[i].GoodsType == sysVPMissionCR.receive.msg[2] )
	     	    {
					if( (GoodsWaySetVal[i].GoodsCurrentSum >= 1) && ( GoodsWaySetVal[i].WayState == 0x01 ) )
					{
						col = i;
	                    break;
					}
				}
			}
			if( i >= COLUMN_NUM_SET )
			{
				VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	
			    return VP_ERR_PAR;
			}
	        sysVPMissionCR.goodsType2 = GOODSTYPEMAX;

		}
	    else
		{
			if( ( sysGoodsMatrix[sel].ColumnNum == 0 )||( sysGoodsMatrix[sel].NextColumn == GOODS_MATRIX_NONE ) )
			{
				VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
				return VP_ERR_PAR;		
			}
		    else
			{
				col = sysGoodsMatrix[sel].NextColumn;
				sysVPMissionCR.goodsType2 = sel;
			}    
		}
	}
	else if(method == 2)
	{
		col = sysVPMissionCR.receive.msg[2]-1;
		
		if( (!(GoodsWaySetVal[col].WayState & 0x01))||( GoodsWaySetVal[col].WayState & 0x0A )||( GoodsWaySetVal[col].GoodsCurrentSum == 0 ) )
		{
			VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
			return VP_ERR_PAR;			
		}
	}

    //3.Check the column's status
    
	//4.ACK
    sysVPMissionCR.vendGoods = sysVPMissionCR.receive.msg[2];
    sysVPMissionCR.vendType  = sysVPMissionCR.receive.msg[3];
    sysVPMissionCR.vendCost  = MoneyRec_CR(sysVPMissionCR.receive.msg[4], sysVPMissionCR.receive.msg[5]);
    sysVPMissionCR.vendColumn = col;
	if(( sysVPMissionCR.vendType > 0 )&&(sysVPMissionCR.vendCost > 0))
	{
		//DisplayStr( 0, 0, 1, "1", 1 ); 
		//WaitForWork(5000,NULL);
		VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;			
	}
	if(( sysVPMissionCR.vendType == 0 )&&(sysVPMissionCR.vendCost == 0))
	{
		//DisplayStr( 0, 0, 1, "2", 1 ); 
		//WaitForWork(5000,NULL);
		VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;			
	}
    
    //---------------------------------------------------
    //2011.7.19 changed for 
    moneyBuf = CurrentPayed;
    if( sysITLMission.billHoldingFlag == 1 )
    {
        moneyBuf += sysITLMission.billHoldingValue;
    }
    //if( sysVPMissionCR.vendCost > CurrentPayed )
    if( sysVPMissionCR.vendCost > moneyBuf )
	{
		//DisplayStr( 0, 0, 1, "3", 1 ); 
		//WaitForWork(5000,NULL);
		VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;			
	}
	//扣钱后不能找零
	if(!(IsCanChange(sysVPMissionCR.vendCost)))
	{
		//DisplayStr( 0, 0, 1, "4", 1 ); 
		//WaitForWork(5000,NULL);
    	VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;
	}
    //
    if( ( sysITLMission.billHoldingFlag == 1 )&&(sysVPMissionCR.vendCost > CurrentPayed) )
    {
        StackTheBillAPI();
        dspUpdate = 1;			
    }
    if( sysVPMissionCR.vendCost > CurrentPayed )
    {
    	//DisplayStr( 0, 0, 1, "5", 1 ); 
		//WaitForWork(5000,NULL);
		VPMsgPackSend_CR( VP_NAK_RPT, 0, 0  );	 
		return VP_ERR_PAR;	    
	}
	if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	{
    	VPMsgPackSend_CR( VP_ACK_RPT, 0, 0 );
	}
	if( dspUpdate == 1 )
	{
		  //payin report
		  if( sysVPMissionCR.payInCoinFlag == 1 )
		  {
		      VPMission_Payin_RPT( VP_DEV_COIN, sysVPMissionCR.payInCoinMoney );
		      sysVPMissionCR.payInCoinFlag = 0;
		      sysVPMissionCR.payInCoinMoney = 0;
						        
		  }
		  if( sysVPMissionCR.payInBillFlag == 1 )
		  {
		      VPMission_Payin_RPT( VP_DEV_BILL, sysVPMissionCR.payInBillMoney );
		      sysVPMissionCR.payInBillFlag = 0;
		      sysVPMissionCR.payInBillMoney = 0;      
		  }
		  //
		  if( sysVPMissionCR.escrowOutFlag == 1 )
		  {
		      VPMission_Payin_RPT( VP_DEV_ESCROWOUT, sysVPMissionCR.escrowMoney );
		      sysVPMissionCR.escrowOutFlag = 0;
		      sysVPMissionCR.escrowMoney = 0;     
		  }
		  DisplayCurrentMoney( CurrentPayed );
	}
    //5.Vendout
    //OutGoods(1);
	sysVPMissionCR.vendCmd = 1;
	//6.Vendout_RPT
    //VPMission_Vendout_RPT( sysVPMissionCR.vendSta, sysVPMissionCR.vendColumn, sysVPMissionCR.vendType, sysVPMissionCR.vendCost );
    */
    return VP_ERR_NULL;	
}

/*********************************************************************************************************
** Function name:     	VP_Reset_Ind
** Descriptions:	    PC指示VMC重新复位命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Reset_Ind( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
    
	//1.Check the data
    if( sysVPMissionCR.receive.datLen != 1  )
	{
	    VPMsgPackSend_CR( VP_NAK_RPT, 0  );	 
		return VP_ERR_PAR;
	}
	//2.ACK	
	/**/
	if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0);
		}
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
		{
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
    //发送邮箱给vmc
	MsgUboxPack[g_Ubox_Index].PCCmd = MBOX_PCTOVMC_RESETIND;				
	OSQPost(g_Ubox_PCTOVMCQ,&MsgUboxPack[g_Ubox_Index]);	
	UpdateIndex();
	OSTimeDly(OS_TICKS_PER_SEC/10);
	TracePC("\r\n Drv Reset_Ind"); 
	/*
	//取得返回值
	AccepterUboxMsg = OSQPend(g_Ubox_PCTOVMCBackQ,OS_TICKS_PER_SEC*10,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv Reset_Ind ACK"); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Reset_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}
	}	
	*/
    return VP_ERR_NULL;	
}



/*********************************************************************************************************
** Function name:     	VP_Control_Ind
** Descriptions:	    PC指示VMC控制设备的命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VP_Control_Ind( void )
{
	//MessageUboxPCPack *AccepterUboxMsg;
	//unsigned char ComStatus;
	
	//1.Check the data
    //2.ACK	
    /**/
    if(SystemPara.EasiveEnable == 1)
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROEASIV_ACK )
        {
	    	VPMsgPackSend_CR( VP_ACK_RPT, 0);
        }
	}
	else
	{
		if( sysVPMissionCR.receive.verFlag & VP_PROTOCOL_ACK )
	    {
			VPMsgPackSend_CR( VP_ACK_RPT, 0 );
		}
	}
	
	//发送邮箱给vmc
    MsgUboxPack[g_Ubox_Index].Type  = sysVPMissionCR.receive.msg[0];
	//sysVPMissionCR.ctrValue = sysVPMissionCR.receive.msg[1];
    switch(MsgUboxPack[g_Ubox_Index].Type)
    {    
    	case VP_CONTROL_BILLCOIN:
			MsgUboxPack[g_Ubox_Index].value  = sysVPMissionCR.receive.msg[1];
			TracePC("\r\n Drv CtrBillcoin"); 
			break;
		case VP_CONTROL_CABINETDEV:
			MsgUboxPack[g_Ubox_Index].Control_device = sysVPMissionCR.receive.msg[1];
			MsgUboxPack[g_Ubox_Index].Control_type  = sysVPMissionCR.receive.msg[2];
			MsgUboxPack[g_Ubox_Index].wenduvalue    = INTEG16(sysVPMissionCR.receive.msg[3],sysVPMissionCR.receive.msg[4]);
			TracePC("\r\n Drv CtrCabinet"); 
			break;	
    	case VP_CONTROL_PAYOUT:
			TracePC("\r\n Drv Ctrpayout"); 
			break;
		case VP_CONTROL_GAMELED:
			MsgUboxPack[g_Ubox_Index].value  = sysVPMissionCR.receive.msg[1];
			TracePC("\r\n Drv Ctrgame=%d",MsgUboxPack[g_Ubox_Index].value);
			break;
		case VP_CONTROL_CLOCK:
			//MsgUboxPack.value  = sysVPMissionCR.receive.msg[1];
			MsgUboxPack[g_Ubox_Index].RTCyear = sysVPMissionCR.receive.msg[1]*256 + sysVPMissionCR.receive.msg[2];
			MsgUboxPack[g_Ubox_Index].RTCmonth = sysVPMissionCR.receive.msg[3];
			MsgUboxPack[g_Ubox_Index].RTCday = sysVPMissionCR.receive.msg[4];
			MsgUboxPack[g_Ubox_Index].RTChour = sysVPMissionCR.receive.msg[5];
			MsgUboxPack[g_Ubox_Index].RTCmin = sysVPMissionCR.receive.msg[6];
			MsgUboxPack[g_Ubox_Index].RTCsecond = sysVPMissionCR.receive.msg[7];
			MsgUboxPack[g_Ubox_Index].RTCweek = sysVPMissionCR.receive.msg[8];
			TracePC("\r\n Drv Ctrclock");
			break;	
		case VP_CONTROL_SCALFACTOR:
			MsgUboxPack[g_Ubox_Index].value  = sysVPMissionCR.receive.msg[1];
			TracePC("\r\n Drv Ctrscale");
			break;
		case VP_CONTROL_INITOK:
			TracePC("\r\n Drv InitOK"); 
			break;
		case VP_CONTROL_HEFANGUI:
			MsgUboxPack[g_Ubox_Index].Control_device = sysVPMissionCR.receive.msg[1];
			MsgUboxPack[g_Ubox_Index].Control_type  = sysVPMissionCR.receive.msg[2];
			MsgUboxPack[g_Ubox_Index].value  = sysVPMissionCR.receive.msg[3];
			TracePC("\r\n Drv HefangGui"); 
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
	//TracePC("\r\n Drv Control_Ind ACK"); 
	//VPMsgPackSend_CR( VP_ACK_RPT, 0  );
	/*
	//取得返回值
	AccepterUboxMsg = OSMboxPend(g_Ubox_PCTOVMCBackCMail,OS_TICKS_PER_SEC*10,&ComStatus);
	if(ComStatus == OS_NO_ERR)
	{		
		switch(AccepterUboxMsg->PCCmd)
		{
			case MBOX_VMCTOPC_ACK:
				TracePC("\r\n Drv Control_Ind ACK"); 
				VPMsgPackSend_CR( VP_ACK_RPT, 0  );	
				break;
			case MBOX_VMCTOPC_NAK:
				TracePC("\r\n Drv Control_Ind NAK"); 
				VPMsgPackSend_CR( VP_NAK_RPT, 0  );	
				break;	
		}		
	}	    
	*/
    return VP_ERR_NULL;	
}



/*********************************************************************************************************
** Function name:     	VPMission_Poll
** Descriptions:	    轮询PC机
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Poll( uint8_t *isInit )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;

	if(LogPara.offLineFlag == 1)
	{		
		if(Timer.offLinePCTimer)
		{	
			return VP_ERR_NULL;
		}
		Timer.offLinePCTimer = 5;
	}		

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
				TracePC("\r\n Drv PollLine=%d",LogPara.offLineFlag); 
			    if(LogPara.offLineFlag == 1)
				{
					TracePC("\r\n Drv PollLineOK"); 
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}	
				TracePC("\r\n Drv rec=ok"); 
				recRes = 1;
				if(SystemPara.EasiveEnable == 0)
				{
					if(globalSys.pcInitFlag == 0)
						globalSys.pcInitFlag = 1;
				}
				break;
			}			
		}
		if(Timer.PCRecTimer==0)
		{
			if(LogPara.offLineFlag == 1)//离线退出
				retry=0;
			else
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
					TracePC("\r\n Drv PollLine2=%d",LogPara.offLineFlag); 
				    if(LogPara.offLineFlag == 1)
					{
						TracePC("\r\n Drv PollLine2OK"); 
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}	
					TracePC("\r\n Drv rec2=ok"); 
					recRes = 1;
					if(SystemPara.EasiveEnable == 0)
					{
						if(globalSys.pcInitFlag == 0)
							globalSys.pcInitFlag = 1;
					}
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
		if(LogPara.offLineFlag == 0)
		{
			LogPara.offLineFlag = 1;
			LogPara.offDetailPage = LogPara.LogDetailPage;
		}
        return VP_ERR_COM;
	}

	//DisplayStr( 0, 0, 1, "System 2", 10 );  
	//WaitForWork(2000,NULL);
	//TracePC("\r\n Drv rec=result"); 
    switch( sysVPMissionCR.receive.msgType )
	{
		case VP_GET_SETUP_IND: 
			VPMission_Setup_RPT_CR();
			*isInit = 1;			
			break;
 	    
		case VP_HOUDAO_IND:
		    VP_CMD_HuodaoPar();
		    break;
		case VP_HUODAO_SET_IND: 
			VP_CMD_HuodaoNo();			
			break;	
		case VP_POSITION_IND:
		    VP_CMD_Position();			
			break;	
		case VP_SALEPRICE_IND:
			VP_CMD_GoodsPar();	
			break;	
		case VP_GET_HUODAO:
			VP_CMD_GetColumnSta();            
            break;	
		case VP_VENDOUT_IND:
		    VP_Vendout_Ind();
		    break;
		case VP_RESET_IND:
		    VP_Reset_Ind();
		    break;
		case VP_CONTROL_IND:
		    VP_Control_Ind();
		    break;
		case VP_GETINFO_IND://120419 by cq TotalSell 			
			VP_CMD_GetInfo();
			break;
		case VP_GET_STATUS:
		    VP_CMD_GetStatus();
		    break;		
		case VP_COST_IND://添加扣款函数;by gzz 20110823
		    VP_Cost_Ind();
		    break;	
		case VP_PAYOUT_IND:
		    VP_Payout_Ind();
		    break;   	
		case VP_GET_OFFLINEDATA_IND:
			VPMission_OfflineData_RPT();
			break;	
		case VP_GETINFO_INDEXP:
			VP_CMD_GetInfoExp();
			break;		
		default:
		    break;
	}
	
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMission_Payin_RPT
** Descriptions:	    上报投币信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币,5读卡器
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Payin_RPT(uint8_t dev,uint16_t payInMoney,uint32_t payAllMoney)
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
		case 5:
			sysVPMissionCR.payInDev = VP_DEV_READER;
			break;	
			
	}    
	sysVPMissionCR.payInMoney = payInMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
	if(SystemPara.EasiveEnable == 1)
	{
		//===========================================
		flag = VPMsgPackSend_CR( VP_PAYIN_RPT, 1);
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
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			if(LogPara.offLineFlag == 0)
			{
				LogPara.offLineFlag = 1;
				LogPara.offDetailPage = LogPara.LogDetailPage;
			}
	        return VP_ERR_COM;
		}

	    switch( sysVPMissionCR.receive.msgType )
		{
			default:
			    break;
		}
	}
	else
	{
		//===========================================
		flag = VPMsgPackSend_CR( VP_PAYIN_RPT, 0);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
	}
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMission_Payout_RPT
** Descriptions:	    上报找零信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Payout_RPT( uint8_t payoutDev,unsigned char Type, unsigned int payoutMoney, unsigned int payAllMoney )
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
    sysVPMissionCR.payoutDev = payoutDev;
    sysVPMissionCR.type = Type;
	sysVPMissionCR.payoutMoney = payoutMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
	//===========================================	
	while( retry )
	{
		flag = VPMsgPackSend_CR( VP_PAYOUT_RPT, 1);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
		Timer.PCRecTimer = VP_TIME_OUT;
		while( Timer.PCRecTimer )
		{
			if( VPBusFrameUnPack_CR() )
			{
				if(LogPara.offLineFlag == 1)
				{
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}
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
		if(LogPara.offLineFlag == 0)
		{
			LogPara.offLineFlag = 1;
			LogPara.offDetailPage = LogPara.LogDetailPage;
		}
        return VP_ERR_COM;
	}
	switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	return VP_ERR_NULL;
}



/*********************************************************************************************************
** Function name:     	VPMission_Cost_RPT
** Descriptions:	    上报扣款信息
** input parameters:    dev: 1投入硬币,2投入纸币,3暂存纸币进入,4暂存纸币出币
                        moneyIn投入的金额,以分为单位
                        moneyAll总的投入的金额,以分为单位 
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Cost_RPT( unsigned char Type, uint32_t costMoney, unsigned int payAllMoney )
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
    sysVPMissionCR.type = Type;
	sysVPMissionCR.costMoney = costMoney;
	sysVPMissionCR.payAllMoney = payAllMoney;
	//===========================================
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
				if(LogPara.offLineFlag == 1)
				{
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}
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
		if(LogPara.offLineFlag == 0)
		{
			LogPara.offLineFlag = 1;
			LogPara.offDetailPage = LogPara.LogDetailPage;
		}
        return VP_ERR_COM;
	}
    switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	return VP_ERR_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMission_Button_RPT
** Descriptions:	    上报按键信息
** input parameters:    type=1小键盘选择货道,4退币,0游戏按键
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMission_Button_RPT( unsigned char type, unsigned char value,uint8_t device )
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
    sysVPMissionCR.type   = type;
	sysVPMissionCR.Column  = value;
	sysVPMissionCR.device  = device;
    //if(type==VP_BUT_GAME)//游戏按键蜂鸣器响;by gzz 20110721
    //{   
    //	Buzzer();
    //}
    if(SystemPara.EasiveEnable == 1)
	{
		//===========================================
	    //1-0: button message, not need ACK
		flag = VPMsgPackSend_CR( VP_BUTTON_RPT, 1);   
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
					if(LogPara.offLineFlag == 1)
					{
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
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
			if(LogPara.offLineFlag == 0)
			{
				LogPara.offLineFlag = 1;
				LogPara.offDetailPage = LogPara.LogDetailPage;
			}
	        return VP_ERR_COM;
		}
		switch( sysVPMissionCR.receive.msgType )
		{
			default:
			    break;
		}
    }
	else
	{
		//1-0: button message, not need ACK
		flag = VPMsgPackSend_CR( VP_BUTTON_RPT, 0);   
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
	}
	return VP_ERR_NULL;

}



/*********************************************************************************************************
** Function name:       VPMission_Vendout_RPT
** Descriptions:        向PC上报出货信息
** input parameters:    status=0出货成功,2，出货失败
                        column实际出货货道  
                        type出货类型
                        cost商品的花费
                        payAllMoney出货完成后，用户投币余额
                        columnLeft本货道剩余商品个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMission_Vendout_RPT( unsigned char status, unsigned char device,unsigned char column, unsigned char type, unsigned int cost, unsigned int payAllMoney, unsigned char columnLeft )
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
	
	
	if(status == 2)
	{
		changeFailedBeep();
		
		//离线时不上报
		if(LogPara.offLineFlag == 0)
		{
			VPMission_ColumnSta_RPT();
		}
	}

	
	//离线时不上报
	if(LogPara.offLineFlag == 1)
	{
		return VP_ERR_NULL;
	}
    retry = VP_COM_RETRY;
    //-------------------------------------------
    sysVPMissionCR.status = status;
	sysVPMissionCR.device = device;
	sysVPMissionCR.Column = column;
	sysVPMissionCR.type = type;
	sysVPMissionCR.costMoney = cost;
	sysVPMissionCR.payAllMoney = payAllMoney;
	sysVPMissionCR.ColumnSum = columnLeft;
	
	//===========================================	
	while( retry )
	{
		flag = VPMsgPackSend_CR( VP_VENDOUT_RPT, 1);
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
		Timer.PCRecTimer = VP_TIME_OUT;
		while( Timer.PCRecTimer )
		{
			if( VPBusFrameUnPack_CR() )
			{
				if(LogPara.offLineFlag == 1)
				{
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}
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
		if(LogPara.offLineFlag == 0)
		{
			LogPara.offLineFlag = 1;
			LogPara.offDetailPage = LogPara.LogDetailPage;
		}
        return VP_ERR_COM;
	}
	switch( sysVPMissionCR.receive.msgType )
	{
		default:
		    break;
	}
	return VP_ERR_NULL;
}



/*********************************************************************************************************
** Function name:       VPMission_Act_RPT
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
unsigned char VPMission_Act_RPT( unsigned char action, uint8_t value,uint8_t second,uint8_t column,uint8_t type,uint32_t cost,uint32_t payAllMoney)
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
    
	//离线不处理
	//if(LogPara.offLineFlag == 1)
	//{		
	//	return VP_ERR_NULL;
	//}
	
    retry = VP_COM_RETRY;
	//-------------------------------------------
    sysVPMissionCR.action   = action;
	sysVPMissionCR.value  = value; 
	sysVPMissionCR.second  = second;
	sysVPMissionCR.Column = column;
	sysVPMissionCR.type = type;
	sysVPMissionCR.costMoney = cost;
	sysVPMissionCR.payAllMoney = payAllMoney;	
	//===========================================
	if(SystemPara.EasiveEnable == 1)
	{
	    flag = VPMsgPackSend_CR( VP_ACTION_RPT, 1);   
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
					TracePC("\r\n Drv ActLine=%d",LogPara.offLineFlag); 
					if(LogPara.offLineFlag == 1)
					{
						TracePC("\r\n Drv ActLineOK"); 
						LogPara.offLineFlag = 0;					
						VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
					}
					recRes = 1;
					//Trace("\r\n getaction1");
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
			if(LogPara.offLineFlag == 0)
			{
				LogPara.offLineFlag = 1;
				LogPara.offDetailPage = LogPara.LogDetailPage;
			}
	        return VP_ERR_COM;
		}
		
		//Trace("\r\n getaction2=%d",sysVPMissionCR.receive.msgType);
		switch( sysVPMissionCR.receive.msgType )
		{		
			case VP_GET_SETUP_IND: 
				VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
				break;
			case VP_HOUDAO_IND:
			    VP_CMD_HuodaoPar(); //1,0
			    break;
			case VP_HUODAO_SET_IND: 
				VP_CMD_HuodaoNo();			
				break;
			case VP_POSITION_IND:
			    VP_CMD_Position();  //1,0	
				break;	
			case VP_SALEPRICE_IND:
				VP_CMD_GoodsPar();	  //1,0
				break;	
			case VP_GET_HUODAO:
				VP_CMD_GetColumnSta();  //1,0        
	            break;	
			case VP_VENDOUT_IND:
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;
			case VP_RESET_IND:
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;
			case VP_CONTROL_IND:
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;
			case VP_GETINFO_IND://120419 by cq TotalSell 			
				VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
				break;
			case VP_GET_STATUS:
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;		
			case VP_COST_IND://添加扣款函数;by gzz 20110823
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;	
			case VP_PAYOUT_IND:
			    VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
			    break;   	
			case VP_GET_OFFLINEDATA_IND:
				VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
				break;	
			case VP_GETINFO_INDEXP:
				VPMsgPackSend_CR( VP_NAK_RPT, 0 );  //1,0
				break;		
			case VP_SET_HUODAO: 
				//Trace("\r\n getaction3");
				VP_CMD_SetHuodao();
				break;
			default:
			    break;
		}
	}
	else
	{
	    flag = VPMsgPackSend_CR( VP_ACTION_RPT, 0);   
	    if( flag != VP_ERR_NULL )
	    {
			return VP_ERR_PAR;
		}
	}
	return VP_ERR_NULL;
}




/*********************************************************************************************************
** Function name:       VPMission_Act_RPT
** Descriptions:        向PC上报Action信息
** input parameters:    type
                        
						#define VP_ADMIN_GOODSADDALL  1
						#define VP_ADMIN_GOODSADDCOL  2
						#define VP_ADMIN_CHANGEADD    3
						#define VP_ADMIN_GOODSCOLUMN  4
						#define VP_ADMIN_GOODSADDTRAY 8
						#define VP_ADMIN_GETBILL      9
						#define VP_ADMIN_GOODSBUHUO  10
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMission_Admin_RPT( unsigned char type,uint8_t Column,uint8_t ColumnSum)
{
    unsigned char retry = 0;
	unsigned char recRes=0;
	unsigned char flag = 0;
	//u_char xdata str[20];
	//u_char xdata len = 0;

	if(LogPara.offLineFlag == 1)
	{
		return VP_ERR_NULL;
	}
	
	//
    retry = VP_COM_RETRY;
	//-------------------------------------------
	sysVPMissionCR.type = type;
	sysVPMissionCR.Column = Column;
	sysVPMissionCR.ColumnSum = ColumnSum;
	
	//===========================================
	flag = VPMsgPackSend_CR( VP_ADMIN_RPT, 1);
	//DisplayStr( 0, 0, 1, "01", 2 );  
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
				if(LogPara.offLineFlag == 1)
				{
					LogPara.offLineFlag = 0;					
					VPMission_Act_RPT(VP_ACT_ONLINE,0,0,0,0,0,0);
				}
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
		if(LogPara.offLineFlag == 0)
		{
			LogPara.offLineFlag = 1;
			LogPara.offDetailPage = LogPara.LogDetailPage;
		}
		TracePC("\r\n Drv UboxAdminFail");
        return VP_ERR_COM;
	}
	
    switch( sysVPMissionCR.receive.msgType )
	{   		
        case VP_NAK:
			changeFailedBeep();
			TracePC("\r\n Drv UboxAdminFail");
            return VP_ERR_PAR;	         	
		case VP_ACK:
			TracePC("\r\n Drv UboxAdminSuc");
		    return VP_ERR_NULL;		    
		case VP_HOUDAO_IND:
			VP_CMD_HuodaoPar();
		    break;
		case VP_HUODAO_SET_IND: 
			VP_CMD_HuodaoNo();			
			break;		
		case VP_POSITION_IND:
			VP_CMD_Position();			
			break;
		case VP_SALEPRICE_IND:
			VP_CMD_GoodsPar();	
			break;	
		default:
		    break;
	}	
	TracePC("\r\n Drv UboxAdminSuc");
	return VP_ERR_NULL;

}
#endif


