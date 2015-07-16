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
#include "UBProtocol_SIMPLE.h"
#include "..\APP\CHANNEL.h"


//uint8_t  decimalPlaces=1;//基本单位1角,2分,0元
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
sysVPMissionSIMPLE                             0x2007c000   Data         560  ubprotocol.o(RAM2)
sysGoodsMatrix                           0x2007c230   Data         496  ubprotocol.o(RAM2)

********************************/
#pragma arm section zidata = "RAM2"

unsigned char SIMPLEg_msg[255]={0};
unsigned char SIMPLEVPMsgBuf[255]={0};

struct VP_MissionSIMPLE sysVPMissionSIMPLE;
unsigned char hdstart=1,hdend=1;//开始遍历的货单号，结束的货道号

#pragma arm section zidata

/*********************************************************************************************************
** Function name:     	VPSIMPLE_Sethdquery
** Descriptions:	    设置需要查询存货数量的货道
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void VPSIMPLE_Sethdquery(uint8_t num)
{
	hdstart=num;
	hdend=num;
}

/*********************************************************************************************************
** Function name:     	SIMPLESIMPLEVPSerialInit
** Descriptions:	    初始化
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void SIMPLESIMPLEVPSerialInit()
{
	uart3_clr_buf();
	memset( &sysVPMissionSIMPLE.send, 0, sizeof(sysVPMissionSIMPLE.send) );
	memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );
}




/*********************************************************************************************************
** Function name:     	crc_check
** Descriptions:	    CRC校验和
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
unsigned char SIMPLESIMPLEVPBusTxMsg( void )
{
	unsigned char  i   = 0;
//	//unsigned char  data len = 0;
//	//unsigned char  data sum = 0;
//	//unsigned char xdata DataString[255];
//
	
//	//1.caculate the CHK
    memset(SIMPLEg_msg,0x00,sizeof(SIMPLEg_msg));	

	SIMPLEg_msg[0] = sysVPMissionSIMPLE.send.sf;
	SIMPLEg_msg[1] = sysVPMissionSIMPLE.send.msgType;
	SIMPLEg_msg[2] = sysVPMissionSIMPLE.send.datLen;
	for( i = 0; i < sysVPMissionSIMPLE.send.datLen; i++ )
	{
		SIMPLEg_msg[3+i] += sysVPMissionSIMPLE.send.msg[i]; 
	}
	
	sysVPMissionSIMPLE.send.chk = CrcCheck(SIMPLEg_msg,sysVPMissionSIMPLE.send.len);
	//2.send the message
	Uart3PutChar( sysVPMissionSIMPLE.send.sf );
	Uart3PutChar( sysVPMissionSIMPLE.send.msgType );
	Uart3PutChar( sysVPMissionSIMPLE.send.datLen );
    for( i=0; i<sysVPMissionSIMPLE.send.datLen; i++  )
	{
		Uart3PutChar( sysVPMissionSIMPLE.send.msg[i] );
	}
	Uart3PutChar( sysVPMissionSIMPLE.send.chk/256 );
    Uart3PutChar( sysVPMissionSIMPLE.send.chk%256 );
	//Trace("\r\n send = %02x,%02x,%02x,%02x,%02x,%02x",sysVPMissionSIMPLE.send.sf,sysVPMissionSIMPLE.send.len,sysVPMissionSIMPLE.send.verFlag,sysVPMissionSIMPLE.send.msgType,sysVPMissionSIMPLE.send.sn,sysVPMissionSIMPLE.send.datLen); 

	return 1;
}


/*********************************************************************************************************
** Function name:     	changeFailedBeep
** Descriptions:	    失败的蜂鸣器叫声
** input parameters:    
** output parameters:   无
** Returned value:      CRC检验结果
*********************************************************************************************************/
void changeSIMPLEFailedBeep()
{
	Buzzer();
	OSTimeDly(OS_TICKS_PER_SEC/5);
	Buzzer();
}



/*********************************************************************************************************
** Function name:     	SIMPLESIMPLEVPBusFrameUnPack
** Descriptions:	    
** input parameters:    msg需要检验的数据;len数据长度
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char SIMPLESIMPLEVPBusFrameUnPack( void )
{
	uint8_t datalen=0,dataindex=0,isft=0;
	unsigned char i=0, k=0, m=0;
	unsigned char len = 0;
	unsigned int  sum = 0;
	unsigned char reclen=0;
	


	/*while(Uart3BuffIsNotEmpty() == 1)
	{		
		for( i=0; i<sizeof( SIMPLEVPMsgBuf )-1; i++ )
		{
			SIMPLEVPMsgBuf[i] = SIMPLEVPMsgBuf[i+1];
		}
		SIMPLEVPMsgBuf[sizeof(SIMPLEVPMsgBuf)-1] = Uart3GetCh();	
		reclen=1;
	}*/
	/*
	Timer.PCRecTimer = VP_TIME_OUT;
	while( Timer.PCRecTimer )
	{
		if(Uart3BuffIsNotEmpty())
		{
			//1.接收数据
			ch = Uart3GetCh();	
			SIMPLEVPMsgBuf[dataindex++]=ch;
			//2.寻找到包头
			if(dataindex == 1)
			{
                if(ch != VPSIMPLE_SF)
				{
					dataindex = 0;
				}    
            }
			//3.寻找到字节长度
            else if(dataindex == 3)
			{
                datalen = ch;
            }
			else if(dataindex >= (len + 5))
			{
				reclen=1;
				break;
			}
		}
	}
	if(reclen)
	{
		TracePC("\r\n Drv <<dat2=");
		for(i=0;i<datalen+5;i++)
			TracePC(" [%02x]",SIMPLEVPMsgBuf[i]);
	}*/	
		
	if(Uart3BuffIsNotEmpty())
	{
		OSTimeDly(20);//等待一段时间，等数据下发完毕
		
		//1.寻找包头	
		while(Uart3BuffIsNotEmpty() == 1)
		{
			if( Uart3GetCh() != VPSIMPLE_SF ) 
			{
				continue;
			}
			else
			{
				SIMPLEVPMsgBuf[i++]=VPSIMPLE_SF;
				SIMPLEVPMsgBuf[i++]=Uart3GetCh();//MT
				datalen=Uart3GetCh();//datalength
				SIMPLEVPMsgBuf[i++]=datalen;
				isft=1;
				break;
			}
		}
		//2.得到包的data和chk
		if(isft)
		{
			for(dataindex=0;dataindex<datalen+2;dataindex++)
			{
				SIMPLEVPMsgBuf[i++]=Uart3GetCh();
				reclen=1;
			}
			TracePC("\r\n Drv <<dat=");
			for(i=0;i<datalen+5;i++)
				TracePC(" [%02x]",SIMPLEVPMsgBuf[i]);
		}
	}
	
		
	
	if(reclen)
	{		
		for( i=0; i<=sizeof(SIMPLEVPMsgBuf); i++ )
		{  	
			//Check the SF
			if( SIMPLEVPMsgBuf[i] != VPSIMPLE_SF ) 
			{
				continue;
			}
			//Check the len
			len = SIMPLEVPMsgBuf[i+2]+3;
			//TracePC("\r\n Drv rec=%d",len); 
			//Check the CHK
			sum = 0;
			sum = CrcCheck(SIMPLEVPMsgBuf + i, len);
			if( (SIMPLEVPMsgBuf[i+len] != sum/256)||(SIMPLEVPMsgBuf[i+len+1] != sum%256)	)
			{
				break;
		    }
			//TracePC("\r\n Drv rec=chk"); 
			OSTimeDly(20);
			//Check the message type
	        if( !((SIMPLEVPMsgBuf[i+1]>=VP_MT_MIN_RECEIVESIMPLE) && (SIMPLEVPMsgBuf[i+1]<=VP_MT_MAX_RECEIVESIMPLE)) )   
	            continue;  
			//TracePC("\r\n Drv rec=type"); 
			OSTimeDly(20);
			//Save the message
			sysVPMissionSIMPLE.receive.sf      = SIMPLEVPMsgBuf[i];
			sysVPMissionSIMPLE.receive.msgType = SIMPLEVPMsgBuf[i+1];
			sysVPMissionSIMPLE.receive.datLen  = SIMPLEVPMsgBuf[i+2];
			sysVPMissionSIMPLE.receive.len	   = sysVPMissionSIMPLE.receive.datLen+5;
	 	    

	        
			TracePC("\r\n Drv <<sf=%02x,mt=[%02x],len=%02x,data=",sysVPMissionSIMPLE.receive.sf,sysVPMissionSIMPLE.receive.msgType,sysVPMissionSIMPLE.receive.datLen); 
			OSTimeDly(20);
			for( m=0,k=i+3; k<i+3+sysVPMissionSIMPLE.receive.datLen; m++,k++)
			{			
				sysVPMissionSIMPLE.receive.msg[m] = SIMPLEVPMsgBuf[k];	
				//TracePC("\r\n Drv k=%d,m=%d,k<%d,data=[%02x]",k,m,(3+sysVPMissionSIMPLE.receive.datLen),sysVPMissionSIMPLE.receive.msg[m]);
				TracePC(" [%02x]",sysVPMissionSIMPLE.receive.msg[m]);
			}
			//TracePC("\r\n Drv rec2=%02x,%02x,%02x,%02x,%02x",sysVPMissionSIMPLE.receive.msg[0],sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2],sysVPMissionSIMPLE.receive.msg[3],sysVPMissionSIMPLE.receive.msg[4]); 
			OSTimeDly(20);
			//TracePC("\r\n Drv chk=%02x,%02x",SIMPLEVPMsgBuf[k],SIMPLEVPMsgBuf[k+1]);
			sysVPMissionSIMPLE.receive.chk = sysVPMissionSIMPLE.receive.msg[k]*256 + sysVPMissionSIMPLE.receive.msg[k+1];
			memset( SIMPLEVPMsgBuf, 0, sizeof(SIMPLEVPMsgBuf) );
			//TracePC("\r\n Drv rec3=%d",sysVPMissionSIMPLE.receive.chk); //OSTimeDly(20);
			return 1;
		}	
	}
	//memset( SIMPLEVPMsgBuf, 0, sizeof(SIMPLEVPMsgBuf) );
	return 0;
}

/*********************************************************************************************************
** Function name:     	VPMsgPackSend
** Descriptions:	    组装报文发送数据
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char SIMPLESIMPLEVPMsgPackSend( unsigned char msgType )
{
//    
    uint8_t i=0,j=0,hdnum=0,ceng=0,hdstate=0,huodaost=0;     
//	uint16_t tempMoney;
//	uint8_t tempSend=0;
//	uint32_t tradeMoney=0;
//	uint8_t cabinet=0,cabno=0;
//	//u_char xdata len = 0;
//
	if( msgType>VP_MT_MAX_SENDSIMPLE )
		return VP_ERRSIMPLE_PAR;

//
	switch( msgType )
	{
		case SIMPLE_INFO_RPT:
		    {				
				i = 0;
				sysVPMissionSIMPLE.send.msgType = SIMPLE_INFO_RPT;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.receive.msg[0]; 
				switch(sysVPMissionSIMPLE.receive.msg[0])
				{
					//获取售货机属性报告
					case 0:
						TracePC("\r\n Drv info=0machine");
						if(SystemPara.GOCIsOpen == 1)
						{
							sysVPMissionSIMPLE.send.msg[i++] = 0x12; //flag1
						}
						else
						{
							sysVPMissionSIMPLE.send.msg[i++] = 0x10; //flag1
						}
						sysVPMissionSIMPLE.send.msg[i++] = 0;    //flag2
						sysVPMissionSIMPLE.send.msg[i++] = hd_nums_by_id(1);    //货道个数
						sysVPMissionSIMPLE.send.msg[i++] = 0;    //展示位个数
						sysVPMissionSIMPLE.send.msg[i++] = 51;    //软件版本字符串长度
						sysVPMissionSIMPLE.send.msg[i++]  = 'e';
						sysVPMissionSIMPLE.send.msg[i++]  = 'a';
						sysVPMissionSIMPLE.send.msg[i++]  = 's';
						sysVPMissionSIMPLE.send.msg[i++]  = 'i';
						sysVPMissionSIMPLE.send.msg[i++]  = 'v';
						sysVPMissionSIMPLE.send.msg[i++]  = 'e';
						sysVPMissionSIMPLE.send.msg[i++]  = 'n';
						sysVPMissionSIMPLE.send.msg[i++]  = 'd';
						sysVPMissionSIMPLE.send.msg[i++]  = '_';
						sysVPMissionSIMPLE.send.msg[i++]  = '8';
						sysVPMissionSIMPLE.send.msg[i++]  = 'X';
						sysVPMissionSIMPLE.send.msg[i++]  = '6';
						sysVPMissionSIMPLE.send.msg[i++]  = ':';
						sysVPMissionSIMPLE.send.msg[i++]  = 'T';
						sysVPMissionSIMPLE.send.msg[i++]  = 'a';
						sysVPMissionSIMPLE.send.msg[i++]  = 'n';
						sysVPMissionSIMPLE.send.msg[i++]  = 'H';
						sysVPMissionSIMPLE.send.msg[i++]  = 'u';
						sysVPMissionSIMPLE.send.msg[i++]  = 'a';
						sysVPMissionSIMPLE.send.msg[i++]  = 'n';
						sysVPMissionSIMPLE.send.msg[i++]  = 'g';
						sysVPMissionSIMPLE.send.msg[i++]  = ':';
						sysVPMissionSIMPLE.send.msg[i++]  = 'T';
						sysVPMissionSIMPLE.send.msg[i++]  = 'o';
						sysVPMissionSIMPLE.send.msg[i++]  = 'u';
						sysVPMissionSIMPLE.send.msg[i++]  = 'M';
						sysVPMissionSIMPLE.send.msg[i++]  = 'i';
						sysVPMissionSIMPLE.send.msg[i++]  = 'n';
						sysVPMissionSIMPLE.send.msg[i++]  = 'g';
						sysVPMissionSIMPLE.send.msg[i++]  = '_';
						sysVPMissionSIMPLE.send.msg[i++]  = 'Z';
						sysVPMissionSIMPLE.send.msg[i++]  = 'h';
						sysVPMissionSIMPLE.send.msg[i++]  = 'i';
						sysVPMissionSIMPLE.send.msg[i++]  = 'L';
						sysVPMissionSIMPLE.send.msg[i++]  = 'e';
						sysVPMissionSIMPLE.send.msg[i++]  = 'n';
						sysVPMissionSIMPLE.send.msg[i++]  = 'g';
						sysVPMissionSIMPLE.send.msg[i++]  = '_';
						sysVPMissionSIMPLE.send.msg[i++]  = 'A';
						sysVPMissionSIMPLE.send.msg[i++]  = 'p';
						sysVPMissionSIMPLE.send.msg[i++]  = 'r';
						sysVPMissionSIMPLE.send.msg[i++]  = 'i';
						sysVPMissionSIMPLE.send.msg[i++]  = 'l';
						sysVPMissionSIMPLE.send.msg[i++]  = ' ';
						sysVPMissionSIMPLE.send.msg[i++]  = '1';
						sysVPMissionSIMPLE.send.msg[i++]  = '0';
						sysVPMissionSIMPLE.send.msg[i++]  = ' ';
						sysVPMissionSIMPLE.send.msg[i++]  = '2';
						sysVPMissionSIMPLE.send.msg[i++]  = '0';
						sysVPMissionSIMPLE.send.msg[i++]  = '1';
						sysVPMissionSIMPLE.send.msg[i++]  = '5';
						break;
					case 1:
						TracePC("\r\n Drv info=1huoemp");
						hdnum=0;
						if(SIMPLEErrorStatus(1)||SIMPLEErrorStatus(2))
							hdnum|=0x04;
						if(SIMPLEErrorStatus(3))
						{
							hdnum|=0x02;
						}						
						sysVPMissionSIMPLE.send.msg[i++]  = hdnum;//flag	
						hdnum=hd_nums_by_id(1);    //货道个数
						if(hdnum%8==0)
							ceng  = hdnum/8;//后面跟的channel的字节数	
						else
							ceng  = hdnum/8+1;//后面跟的channel的字节数	
						sysVPMissionSIMPLE.send.msg[i++]  = ceng;//后面跟的channel的字节数	

						for(j=0;j<ceng;j++)
						{
							hdstate=0;
							if((j*8+1)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+1)))
								{
									hdstate|=0x01;
								}
							}
							if((j*8+2)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+2)))
								{
									hdstate|=0x02;
								}
							}
							if((j*8+3)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+3)))
								{
									hdstate|=0x04;
								}
							}
							if((j*8+4)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+4)))
								{
									hdstate|=0x08;
								}
							}
							if((j*8+5)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+5)))
								{
									hdstate|=0x10;
								}
							}
							if((j*8+6)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+6)))
								{
									hdstate|=0x20;
								}
							}
							if((j*8+7)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+7)))
								{
									hdstate|=0x40;
								}
							}
							if((j*8+8)<=hdnum)
							{
								if(hd_state_by_id(1,(j*8+8)))
								{
									hdstate|=0x80;
								}
							}
							sysVPMissionSIMPLE.send.msg[i++]  = hdstate;//货道售空信息
						}						
						break;
					case 2:
						TracePC("\r\n Drv info=2err");
						sysVPMissionSIMPLE.send.msg[i++] = 0x00; //errorcount
						break;
					case 3:
						TracePC("\r\n Drv info=3time");
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.year/256;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.year%256;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.month;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.day;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.hour;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.minute;
						sysVPMissionSIMPLE.send.msg[i++] = RTCData.second;
						break;
					case 4:
						TracePC("\r\n Drv info=4huoerr");
						hdnum=hd_nums_by_id(1);    //货道个数
						sysVPMissionSIMPLE.send.msg[i++] = hdnum;    //货道个数
						for(j=1;j<=hdnum;j++)
						{
							hdstate=0;
							huodaost=hd_get_by_id(1,j,3);
							if(huodaost==2)//故障
							{
								hdstate=0x02;								
							}
							else if(huodaost==4)//pc禁用
							{
								hdstate=0x01;								
							}
							sysVPMissionSIMPLE.send.msg[i++] = hdstate;							
						}
						break;
				}
				sysVPMissionSIMPLE.send.datLen = i; 
			}
			break;
		case SIMPLE_BUTTON_RPT:	
			{
				i = 0;
				sysVPMissionSIMPLE.send.msgType = SIMPLE_BUTTON_RPT;
				sysVPMissionSIMPLE.send.msg[i++] = 1;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.btnchannel_id;
				sysVPMissionSIMPLE.send.datLen = i; 
			}	
			break;	
		case SIMPLE_VEND_RPT:
		    {
				i = 0;
				sysVPMissionSIMPLE.send.msgType = SIMPLE_VEND_RPT;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.channel_id;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.channel_result;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.SN;
				sysVPMissionSIMPLE.send.datLen = i; 
			}
			break;
		case SIMPLE_ADMIN_RPT:
		    {
				i = 0;
			    sysVPMissionSIMPLE.send.msgType =  SIMPLE_ADMIN_RPT;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.admintype;	
				//1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
				switch(sysVPMissionSIMPLE.admintype)
				{
					case 1:
						TracePC("\r\n Drv Admin=allhuo");
						sysVPMissionSIMPLE.send.msg[i++] = 0;
						sysVPMissionSIMPLE.send.msg[i++] = 0;						
						break;
					case 2:
						TracePC("\r\n Drv Admin=cenghuo");
						sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.admincolumn;
						sysVPMissionSIMPLE.send.msg[i++] = 0;						
						break;
					case 3:
						TracePC("\r\n Drv Admin=simplehuo");
						sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.admincolumn;
						sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.admincolumnsum;						
						break;
					case 4:						
					case 5:	
						TracePC("\r\n Drv Admin=huoorpayout");
						sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.admincolumnsum;	
						sysVPMissionSIMPLE.send.msg[i++] = 0;	
						break;		
				}
				sysVPMissionSIMPLE.send.datLen = i; 
				
			}
			break; 
		case SIMPLE_GET_ADMIN:
			{
				i = 0;
			    sysVPMissionSIMPLE.send.msgType =  SIMPLE_GET_ADMIN;
				sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.getadmintype;	
				//3请求从某一个货道编号的商品剩余量,4获取当前最长出货时间,5获取当前自动退币时间
				switch(sysVPMissionSIMPLE.getadmintype)
				{
					case 3:
						sysVPMissionSIMPLE.send.msg[i++] = sysVPMissionSIMPLE.getadmincolumn;										
						break;
					case 4:
						sysVPMissionSIMPLE.send.msg[i++] = 0;										
						break;
					case 5:
						sysVPMissionSIMPLE.send.msg[i++] = 0;										
						break;	
				}
				sysVPMissionSIMPLE.send.datLen = i; 
			}
			break;
		default: break;

	}
	sysVPMissionSIMPLE.send.sf  = VPSIMPLE_SF;	 
    sysVPMissionSIMPLE.send.len = sysVPMissionSIMPLE.send.datLen + 3;		   	
	
    SIMPLESIMPLEVPBusTxMsg();    
	return VP_ERRSIMPLE_NULL;
}

/*********************************************************************************************************
** Function name:     	VPMissionSIMPLE_Setup_RPT
** Descriptions:	    启动时上报配置信息
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Setup_RPT( void )
{
	SIMPLESIMPLEVPMsgPackSend( SIMPLE_INFO_RPT);
	return VP_ERRSIMPLE_NULL;
}

/*********************************************************************************************************
** Function name:     	VP_Vendout_Ind
** Descriptions:	    PC指示VMC出货命令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPSIMPLE_Vendout_Ind( void )
{
	//MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	//unsigned char ComStatus;


	//SN相同时，不重新出货
	if((sysVPMissionSIMPLE.SN  == sysVPMissionSIMPLE.receive.msg[1])&&(sysVPMissionSIMPLE.SN  > 0))
	{
		TracePC("\r\n Drv vendind=error");
		VPMissionSIMPLE_Vendout_RPT(sysVPMissionSIMPLE.channel_result);
	}
	else
	{
		//1.得到出货货道	
		sysVPMissionSIMPLE.channel_id = sysVPMissionSIMPLE.receive.msg[0];	
	    sysVPMissionSIMPLE.SN  = sysVPMissionSIMPLE.receive.msg[1];
		TracePC("\r\n Drv vendind=%d,%d",sysVPMissionSIMPLE.channel_id,sysVPMissionSIMPLE.SN); 
		
		//发送邮箱给vmc
		MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEPCTOVMC_VENDIND;	
		MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_id = sysVPMissionSIMPLE.channel_id;
		OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
		UpdateSIMPLEIndex();
		OSTimeSet(0);
		TracePC("\r\n Drv %dVendout_Ind",OSTimeGet()); 
	}
    return VP_ERRSIMPLE_NULL;	
}



/*********************************************************************************************************
** Function name:     	VPSIMPLE_Display_Ind
** Descriptions:	    PC指示VMC显示金额或者时间的指令
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPSIMPLE_Display_Ind( void )
{
	//MessageSIMPLEUboxPCPack *AccepterSIMPLEUboxMsg;
	unsigned char distype=0;	
	unsigned char bai,shi,ge,mao;
	//1.得到显示类型
	distype=(sysVPMissionSIMPLE.receive.msg[0]>>5)&0x07;
	sysVPMissionSIMPLE.distype=distype;		
	TracePC("\r\n Drv distype=%d",sysVPMissionSIMPLE.distype);
	//0用户余额，1时间，2文本，3商品单价 
	switch(sysVPMissionSIMPLE.distype)
	{
		case 0:
		case 3:	
			bai=(sysVPMissionSIMPLE.receive.msg[1]>0x30)?(sysVPMissionSIMPLE.receive.msg[1]-0x30):0;
			shi=(sysVPMissionSIMPLE.receive.msg[2]>0x30)?(sysVPMissionSIMPLE.receive.msg[2]-0x30):0;
			ge=(sysVPMissionSIMPLE.receive.msg[3]>0x30)?(sysVPMissionSIMPLE.receive.msg[3]-0x30):0;
			mao=(sysVPMissionSIMPLE.receive.msg[5]>0x30)?(sysVPMissionSIMPLE.receive.msg[5]-0x30):0;
			sysVPMissionSIMPLE.payInMoney=bai*10000+shi*1000+ge*100+mao*10;
			TracePC("\r\n Drv dismoney=%ld",sysVPMissionSIMPLE.payInMoney);
			//发送邮箱给vmc
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEPCTOVMC_DISPLAYIND;	
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].distype = sysVPMissionSIMPLE.distype;
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].payInMoney = sysVPMissionSIMPLE.payInMoney;
			OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
			UpdateSIMPLEIndex();
			break;
		case 1:
			RTCData.year=2000+sysVPMissionSIMPLE.receive.msg[1];
			RTCData.month=sysVPMissionSIMPLE.receive.msg[2];
			RTCData.day=sysVPMissionSIMPLE.receive.msg[3];
			RTCData.hour=sysVPMissionSIMPLE.receive.msg[4];
			RTCData.minute=sysVPMissionSIMPLE.receive.msg[5];	
			TracePC("\r\n Drv distime=%ld-%d-%d %d:%d",RTCData.year,RTCData.month,RTCData.day,RTCData.hour,RTCData.minute);
			RTCSetTime(&RTCData);
			break;
		case 2:
			TracePC("\r\n Drv disptxt=%c,%c,%c,%c,%c",sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2],
				sysVPMissionSIMPLE.receive.msg[3],sysVPMissionSIMPLE.receive.msg[4],sysVPMissionSIMPLE.receive.msg[5]);
			//发送邮箱给vmc
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEPCTOVMC_DISPLAYIND;	
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].distype = sysVPMissionSIMPLE.distype;
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].disp[0] = sysVPMissionSIMPLE.receive.msg[1];
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].disp[1] = sysVPMissionSIMPLE.receive.msg[2];
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].disp[2] = sysVPMissionSIMPLE.receive.msg[3];
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].disp[3] = sysVPMissionSIMPLE.receive.msg[4];
			MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].disp[4] = sysVPMissionSIMPLE.receive.msg[5];
			OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
			UpdateSIMPLEIndex();			
			break;
	}
	
	//发送邮箱给vmc
	//MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEPCTOVMC_VENDIND;	
	//MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].channel_id = sysVPMissionSIMPLE.channel_id;
	//OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
	//UpdateSIMPLEIndex();
    return VP_ERRSIMPLE_NULL;	
}



/*********************************************************************************************************
** Function name:     	VPMissionSIMPLE_Poll
** Descriptions:	    轮询PC机
** input parameters:    
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMissionSIMPLESIMPLE_Poll()
{
    unsigned char recRes=VP_ERRSIMPLE_COM;
		
	//TracePC("\r\n Drv Pollsystem1>>"); 
	if( SIMPLESIMPLEVPBusFrameUnPack() )
	{ 	
		//TracePC("\r\n Drv rec<<<<<<<<<ok"); 
		recRes = VP_ERRSIMPLE_NULL;	 		
	}			

    switch( sysVPMissionSIMPLE.receive.msgType )
	{
		case SIMPLE_GET_INFO: 
			VPMissionSIMPLE_Setup_RPT();			
			break;
		case SIMPLE_VEND_IND:
		    VPSIMPLE_Vendout_Ind();
		    break;
		case SIMPLE_DISPLAY_IND: 
			VPSIMPLE_Display_Ind();			
			break;	
		case SIMPLE_RESULT_IND:
			VPSIMPLE_Result_Ind();
			break;
		case SIMPLE_ADMIN_IND:
			VPSIMPLE_Admin_Ind();
			break;	
	}
	memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );
	return recRes;
}

/*********************************************************************************************************
** Function name:     	VPMissionSIMPLE_Button_RPT
** Descriptions:	    上报按键信息
** input parameters:    type=1小键盘选择货道,4退币,0游戏按键
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Button_RPT( uint8_t btnchannel_id )
{
    //unsigned char flag = 0; 	
    //-------------------------------------------
    sysVPMissionSIMPLE.btnchannel_id = btnchannel_id;
	SIMPLESIMPLEVPMsgPackSend( SIMPLE_BUTTON_RPT);
	return VP_ERRSIMPLE_NULL;
}



/*********************************************************************************************************
** Function name:       VPMissionSIMPLE_Vendout_RPT
** Descriptions:        向PC上报出货信息
** input parameters:    channel_result=0出货成功,1，出货失败                        
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Vendout_RPT( uint8_t channel_result )
{					     	
    //-------------------------------------------
    sysVPMissionSIMPLE.channel_result = channel_result;
	//蜂鸣器叫两声
	if(channel_result==1)
		changeSIMPLEFailedBeep();
	SIMPLESIMPLEVPMsgPackSend( SIMPLE_VEND_RPT);
	return VP_ERRSIMPLE_NULL;
}

/*********************************************************************************************************
** Function name:       VPMissionSIMPLE_Get_Admin
** Descriptions:        向PC请求参数信息
** input parameters:    type
                        
						
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Get_Admin( unsigned char admintype,unsigned char hdstart)
{
	unsigned char recRes=0;
	unsigned char recAck=VP_ERRSIMPLE_PAR;	
	//-------------------------------------------
	sysVPMissionSIMPLE.getadmintype = admintype;
	
	//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
	//3请求从某一个货道编号的商品剩余量,4获取当前最长出货时间,5获取当前自动退币时间
	switch(sysVPMissionSIMPLE.getadmintype)
	{
		case 3:		
			sysVPMissionSIMPLE.getadmincolumn = hdstart;
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_GET_ADMIN);		
			Timer.PCRecTimer = VP_TIMSIMPLEE_OUT;
			while( Timer.PCRecTimer )
			{
				if( SIMPLESIMPLEVPBusFrameUnPack() )
				{ 	
					TracePC("\r\n Drv rec=ok"); 
					recRes = 1;	
					break;
				}		
			}
			if( recRes== 0 )
			{

				TracePC("\r\n Drv UboxGetAdminTimeFail");
		        recAck= VP_ERRSIMPLE_COM;
			}
			else
			{
				if( sysVPMissionSIMPLE.receive.msgType==SIMPLE_ADMIN_IND )
				{						
					TracePC("\r\n Drv UboxGetAdminSuchd=%d,sum=%d",sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2]);
					hd_setNums_by_id(1,sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2]);
					recAck= VP_ERRSIMPLE_NULL;
				}
				
			}
			//memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );			
			break;
		case 4:		
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_GET_ADMIN);		
			Timer.PCRecTimer = VP_TIMSIMPLEE_OUT;
			while( Timer.PCRecTimer )
			{
				if( SIMPLESIMPLEVPBusFrameUnPack() )
				{ 	
					TracePC("\r\n Drv rec=ok"); 
					recRes = 1;	
					break;
				}		
			}
			if( recRes== 0 )
			{

				TracePC("\r\n Drv UboxGetAdminTimeFail");
		        recAck= VP_ERRSIMPLE_COM;
			}
			else
			{
				if( sysVPMissionSIMPLE.receive.msgType==SIMPLE_ADMIN_IND )
				{						
					TracePC("\r\n Drv UboxGetAdminSuchd=%d",sysVPMissionSIMPLE.receive.msg[1]);
					SystemPara.ColumnTime=sysVPMissionSIMPLE.receive.msg[1];
					recAck= VP_ERRSIMPLE_NULL;
				}
				
			}
			//memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );			
			break;	
		case 5:		
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_GET_ADMIN);		
			Timer.PCRecTimer = VP_TIMSIMPLEE_OUT;
			while( Timer.PCRecTimer )
			{
				if( SIMPLESIMPLEVPBusFrameUnPack() )
				{ 	
					TracePC("\r\n Drv rec=ok"); 
					recRes = 1;	
					break;
				}		
			}
			if( recRes== 0 )
			{

				TracePC("\r\n Drv UboxGetAdminTimeFail");
		        recAck= VP_ERRSIMPLE_COM;
			}
			else
			{
				if( sysVPMissionSIMPLE.receive.msgType==SIMPLE_ADMIN_IND )
				{						
					TracePC("\r\n Drv UboxGetAdminSuchd=%d",sysVPMissionSIMPLE.receive.msg[1]);
					SystemPara.SaleTime=sysVPMissionSIMPLE.receive.msg[1];
					recAck= VP_ERRSIMPLE_NULL;
				}
				
			}
			//memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );			
			break;		
	}
	//发送邮箱给vmc
	MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
	MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
	OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
	UpdateSIMPLEIndex();
	memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );
	return recAck;

}

/*********************************************************************************************************
** Function name:       VPMissionSIMPLE_Set_Admin
** Descriptions:        向PC请求参数信息
** input parameters:    admintype=3货道存货数量，4出货时间，5退币时间
                        
						
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Get_Admin2( unsigned char admintype)
{
	unsigned char recRes=0;
	unsigned char recAck=VP_ERRSIMPLE_PAR;
	unsigned char hdnum=0,i;
	//-------------------------------------------
	sysVPMissionSIMPLE.getadmintype = admintype;
	//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
	//3请求从某一个货道编号的商品剩余量,4设置当前最长出货时间,5设置当前自动退币时间
	switch(sysVPMissionSIMPLE.getadmintype)
	{
		case 3:			
			TracePC("\r\n Drv colget=%d",hdstart);
			sysVPMissionSIMPLE.getadmincolumn = hdstart;
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_GET_ADMIN);
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if(VPMissionSIMPLESIMPLE_Poll()==VP_ERRSIMPLE_NULL)
				{
					recRes = 1;	
					break;
				}
				OSTimeDly(4);
			}
			if(recRes==0)
			{ 	
				TracePC("\r\n Drv UboxAdminTimeFail");
				recAck= VP_ERRSIMPLE_COM;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);					
				UpdateSIMPLEIndex();
			}
			break;
	}
	return recAck;

}

/*********************************************************************************************************
** Function name:       VPSIMPLE_Admin_Ind
** Descriptions:        向VMC保存PC返回的Admin信息
** input parameters:    admintype:admintype=3货道存货数量，4出货时间，5退币时间
                        
						
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void VPSIMPLE_Admin_Ind()
{
	unsigned char recAck=VP_ERRSIMPLE_NULL;
	switch(sysVPMissionSIMPLE.receive.msg[0])
	{
		case 3:
			TracePC("\r\n Drv UboxAdmin_Indcol=%d,sum=%d",sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2]);
			hd_setNums_by_id(1,sysVPMissionSIMPLE.receive.msg[1],sysVPMissionSIMPLE.receive.msg[2]);
			memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );
			hdstart++;
			if(hdstart<=hdend)
			{
				VPMissionSIMPLE_Get_Admin2(3);
			}
			else
			{
				hdstart=1;
				hdend=1;
				//协议成功，当货道数据完全同步完成之后，再发送邮箱给vmc表示完成了
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
				UpdateSIMPLEIndex();
			}
			break;
	}
}


/*********************************************************************************************************
** Function name:       VPMissionSIMPLE_Admin_RPT
** Descriptions:        向PC上报Admin信息
** input parameters:    admintype:1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
                        
						
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPMissionSIMPLE_Admin_RPT( unsigned char admintype,uint8_t admincolumn,uint8_t admincolumnsum)
{
	unsigned char recRes=0;
	unsigned char recAck=VP_ERRSIMPLE_PAR;
	unsigned char hdstart=0, hdend=0;
	//-------------------------------------------
	sysVPMissionSIMPLE.admintype = admintype;
	//vmc上报给PC他的admin的类型1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
	switch(sysVPMissionSIMPLE.admintype)
	{
		case 1:
			sysVPMissionSIMPLE.admincolumn = admincolumn;
			sysVPMissionSIMPLE.admincolumnsum = admincolumnsum;
			//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_ADMIN_RPT);
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if(VPMissionSIMPLESIMPLE_Poll()==VP_ERRSIMPLE_NULL)
				{
					recRes = 1;	
					break;
				}
				OSTimeDly(4);
			}
			if(recRes==0)
			{ 	
				TracePC("\r\n Drv UboxAdminTimeFail");
				recAck= VP_ERRSIMPLE_COM;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);					
				UpdateSIMPLEIndex();
			}				
			break;
		case 2:
			sysVPMissionSIMPLE.admincolumn = admincolumn;
			sysVPMissionSIMPLE.admincolumnsum = admincolumnsum;
			//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_ADMIN_RPT);
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if(VPMissionSIMPLESIMPLE_Poll()==VP_ERRSIMPLE_NULL)
				{
					recRes = 1;	
					break;
				}
				OSTimeDly(4);
			}
			if(recRes==0)
			{ 	
				TracePC("\r\n Drv UboxAdminTimeFail");
				recAck= VP_ERRSIMPLE_COM;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);					
				UpdateSIMPLEIndex();
			}			
			break;	
		case 3:
			sysVPMissionSIMPLE.admincolumn = admincolumn;
			sysVPMissionSIMPLE.admincolumnsum = admincolumnsum;
			//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_ADMIN_RPT);
			Timer.PCRecTimer = VP_TIME_OUT;
			while( Timer.PCRecTimer )
			{
				if(VPMissionSIMPLESIMPLE_Poll()==VP_ERRSIMPLE_NULL)
				{
					recRes = 1;	
					break;
				}
				OSTimeDly(4);
			}
			if(recRes==0)
			{ 	
				TracePC("\r\n Drv UboxAdminTimeFail");
				recAck= VP_ERRSIMPLE_COM;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);					
				UpdateSIMPLEIndex();
			}

			/*
			Timer.PCRecTimer = VP_TIMSIMPLEE_OUT;
			while( Timer.PCRecTimer )
			{
				if( SIMPLESIMPLEVPBusFrameUnPack() )
				{ 	
					TracePC("\r\n Drv rec=ok"); 					
					recRes = 1;	
					break;
				}		
			}
			if( recRes== 0 )
			{

				TracePC("\r\n Drv UboxAdminTimeFail");
		        recAck= VP_ERRSIMPLE_COM;
			}
			else
			{
				if( sysVPMissionSIMPLE.receive.msgType==SIMPLE_RESULT_IND )
				{
					if(sysVPMissionSIMPLE.receive.msg[0]>0)
					{
						TracePC("\r\n Drv UboxAdminAckFail");
						 recAck= VP_ERRSIMPLE_PAR;
					}
					else
					{				
						TracePC("\r\n Drv UboxAdminSuc");						
						recAck= VP_ERRSIMPLE_NULL;
					}
				}
				
			}	*/
			break;	
		case 4:
		case 5:	
			sysVPMissionSIMPLE.admincolumn = admincolumn;
			sysVPMissionSIMPLE.admincolumnsum = admincolumnsum;
			//TracePC("\r\n Drv Admin=%d,%d,%d",sysVPMissionSIMPLE.admintype,sysVPMissionSIMPLE.admincolumn,sysVPMissionSIMPLE.admincolumnsum);	
			SIMPLESIMPLEVPMsgPackSend( SIMPLE_ADMIN_RPT);



			Timer.PCRecTimer = VP_TIMSIMPLEE_OUT;
			while( Timer.PCRecTimer )
			{
				if( SIMPLESIMPLEVPBusFrameUnPack() )
				{ 	
					TracePC("\r\n Drv rec=ok"); 					
					recRes = 1;	
					break;
				}		
			}
			if( recRes== 0 )
			{

				TracePC("\r\n Drv UboxAdminTimeFail");
		        recAck= VP_ERRSIMPLE_COM;
			}
			else
			{
				if( sysVPMissionSIMPLE.receive.msgType==SIMPLE_RESULT_IND )
				{
					if(sysVPMissionSIMPLE.receive.msg[0]>0)
					{
						TracePC("\r\n Drv UboxAdminAckFail");
						 recAck= VP_ERRSIMPLE_PAR;
					}
					else
					{				
						TracePC("\r\n Drv UboxAdminSuc");						
						recAck= VP_ERRSIMPLE_NULL;
					}
				}
				
			}	
			break;	
	}
	//发送邮箱给vmc
	/*MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
	MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
	OSQPost(g_SIMPLEUbox_PCTOVMCQ,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
	UpdateSIMPLEIndex();
	memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );*/
	
	return recAck;

}


/*********************************************************************************************************
** Function name:       VPSIMPLE_Result_Ind
** Descriptions:        向PC上报Admin信息后，返回的结果
** input parameters:    admintype:1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
                        
						
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
unsigned char VPSIMPLE_Result_Ind()
{
	unsigned char recAck=VP_ERRSIMPLE_PAR;
	//vmc上报给PC他的admin的类型1加满全部货道，2加满层架货道，3单货道补货，4最长出货时间，5自动退币时间
	switch(sysVPMissionSIMPLE.admintype)
	{
		case 1:
			if(sysVPMissionSIMPLE.receive.msg[0]>0)
			{
				TracePC("\r\n Drv UboxAdminAllAckFail");
				 recAck= VP_ERRSIMPLE_PAR;
			}
			else
			{				
				TracePC("\r\n Drv UboxAdminAllSuc");
				hdstart=1;//设置起始货道号
				hdend=hd_nums_by_id(1);//设置结束货道号
				VPMissionSIMPLE_Get_Admin2(3);
				recAck= VP_ERRSIMPLE_NULL;
			}
			break;
		case 2:
			if(sysVPMissionSIMPLE.receive.msg[0]>0)
			{
				TracePC("\r\n Drv UboxAdminCengAckFail");
				 recAck= VP_ERRSIMPLE_PAR;
			}
			else
			{				
				TracePC("\r\n Drv UboxAdminCengSuc");
				hd_ids_by_level(1,sysVPMissionSIMPLE.admincolumn,&hdstart,&hdend);
				VPMissionSIMPLE_Get_Admin2(3);
				recAck= VP_ERRSIMPLE_NULL;
			}
			break;
		case 3:
			if(sysVPMissionSIMPLE.receive.msg[0]>0)
			{
				TracePC("\r\n Drv UboxAdminSimAckFail");
				 recAck= VP_ERRSIMPLE_PAR;
			}
			else
			{				
				TracePC("\r\n Drv UboxAdminSimSuc");
				//hd_ids_by_level(1,sysVPMissionSIMPLE.admincolumn,&hdstart,&hdend);
				//VPMissionSIMPLE_Get_Admin2(3);				
				recAck= VP_ERRSIMPLE_NULL;
				//发送邮箱给vmc
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
				MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
				OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
				UpdateSIMPLEIndex();
			}
			break;	
	}
	//协议失败，直接上报。协议成功，还需要把各个货道的值得到，再上报
	if(recAck> VP_ERRSIMPLE_NULL)
	{
		//发送邮箱给vmc
		MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].PCTOVMCCmd = MBOX_SIMPLEVMCTOPC_RESULTIND;	
		MsgSIMPLEUboxPack[g_SIMPLEUbox_Index].adminresult = recAck;
		OSMboxPost(g_SIMPLEUbox_VMCTOPCBackCMail,&MsgSIMPLEUboxPack[g_SIMPLEUbox_Index]);	
		UpdateSIMPLEIndex();
	}
	memset( &sysVPMissionSIMPLE.receive, 0, sizeof(sysVPMissionSIMPLE.receive) );
	return recAck;
}




