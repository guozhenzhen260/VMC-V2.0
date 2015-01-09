/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           SelectKey.C
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        选货按键驱动程序相关函数声明                     
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
#include "xmt.h"



/*****************************************************************************
*****************************************************************************
***********************************XMT8008温控设备***************************
*****************************************************************************
*****************************************************************************/
//XMT温控;by gzz 20110105

unsigned char XMTMsgBuf[255];  //255

struct XMT_Mission sysXMTMission;//XMT温控;by gzz 20110105



void ZhkSerial1PutCh(uint8_t byte) 
{
	//Trace(" %02x",byte);
	OSTimeDly(2);
	Uart1PutChar(byte); 
}

void delay(INT16U ticks)
{

}


void DelayMs(INT16U ticks)
{

}

typedef unsigned char SERAIL1PARAM[6];
#define VERIFY_NULL 	0

SERAIL1PARAM  XMTSerialParam =
{
	8, VERIFY_NULL, 2, 0x00, 0x96, 0x00
};


//初始化串口
void XMTSerialInit( void )
{	
	
}

//send the message
unsigned char XMTBusTxMsg( void )
{   	
	
	//1.Send the message
    ZhkSerial1PutCh( sysXMTMission.Addr );
	//OSTimeDly(1);
	ZhkSerial1PutCh( sysXMTMission.Addr );
    //OSTimeDly(1);
    ZhkSerial1PutCh( sysXMTMission.cmd );
	//OSTimeDly(1);
	ZhkSerial1PutCh( sysXMTMission.cmdSet );
	//OSTimeDly(1);
    ZhkSerial1PutCh( sysXMTMission.cmdPar[0] );
	//OSTimeDly(1);
    ZhkSerial1PutCh( sysXMTMission.cmdPar[1] );
	//OSTimeDly(1);
    ZhkSerial1PutCh( sysXMTMission.crcEnd );
	//OSTimeDly(1);
    ZhkSerial1PutCh( sysXMTMission.cmdSet );
	//OSTimeDly(1);
	
    
    return 1;
}



//flag=1读，flag=0设定; temparater要设置的温度; cmdsetflag=1设置温度控制偏移精度,cmdsetflag=0设置温度
unsigned char XMTMsgPackSend( unsigned char flag,unsigned int temparater, unsigned char cmdsetflag)
{
	//Trace("\r\nXMTMsgPackSend");
    
    sysXMTMission.Addr = XMT_CTR_ADDR;
	//flag=1读温度，flag=0设定温度
    if( flag == 1 )
    {
     	sysXMTMission.cmd=XMT_CTR_READTEMP;
		sysXMTMission.cmdPar[0]=0x00;
		sysXMTMission.cmdPar[1]=0x00;
		sysXMTMission.crcEnd=(XMT_CTR_READTEMP+XMT_CTR_DEVADDR);	
    }
	else if( flag == 0 )
	{
	 	sysXMTMission.cmd=XMT_CTR_SETTEMP;	
		sysXMTMission.cmdPar[0]=temparater%256;
		sysXMTMission.cmdPar[1]=temparater/256;
		sysXMTMission.crcEnd=(XMT_CTR_SETTEMP+XMT_CTR_DEVADDR);	
	}

	
	if(cmdsetflag==0)
	{
    	sysXMTMission.cmdSet=XMT_CTR_CMDSETTMP;	
	}
	else if(cmdsetflag==1)
	{
    	sysXMTMission.cmdSet=XMT_CTR_CMDSETHY;	
	}	
    XMTBusTxMsg();
    DelayMs(10);
    return ITL_ERR_NULL;
}


//cmdsetflag=1读温度控制偏移精度,cmdsetflag=0读温度
unsigned char XMTBusUnpack( unsigned char cmdsetflag )
{
    unsigned char i=0;	

	
	//Trace("\r\nXMTBusUnpack");

	sysXMTMission.rectemp=0;
	memset( sysXMTMission.recBuf, 0x00, sizeof(sysXMTMission.recBuf) );

	sysXMTMission.rectemp = Uart1_Read(sysXMTMission.recBuf,30);


	//Trace(" sysXMTMission.rectemp = %02d -",sysXMTMission.rectemp);
	//for(i = 0; i < 15; i++)
	//	Trace(" %02x",sysXMTMission.recBuf[i]);

	if(sysXMTMission.rectemp>0)
	{
		i=0;
		//如果本次的温度检测太大，表明数据是失效的，不予承认
		if( (sysXMTMission.recBuf[i+1]*256+sysXMTMission.recBuf[i])>2000)
		{
			//Trace(" \r\nArecPVTemp = %02d,recSVTemp = %02d",sysXMTMission.recPVTemp,sysXMTMission.recSVTemp);
			return 1;
		}
		sysXMTMission.recPV[0]=sysXMTMission.recBuf[i];
		sysXMTMission.recPV[1]=sysXMTMission.recBuf[i+1];
		sysXMTMission.recPVTemp=sysXMTMission.recPV[1]*256+sysXMTMission.recPV[0];
		sysXMTMission.recSV[0]=sysXMTMission.recBuf[i+2];
		sysXMTMission.recSV[1]=sysXMTMission.recBuf[i+3];
		sysXMTMission.recSVTemp=sysXMTMission.recSV[1]*256+sysXMTMission.recSV[0];	
		if(cmdsetflag==1)
		{
			sysXMTMission.recHY[0]=sysXMTMission.recBuf[i+6];
			sysXMTMission.recHY[1]=sysXMTMission.recBuf[i+7];
			sysXMTMission.recHYTemp=sysXMTMission.recHY[1]*256+sysXMTMission.recHY[0];
		}		
		//Trace(" \r\nBrecPVTemp = %02d,recSVTemp = %02d",sysXMTMission.recPVTemp,sysXMTMission.recSVTemp);
		return 1;
	}  
	else
	{
		//如果原先有温度，多执行几次，都是没温度才予以清除原先温度
		if(( sysXMTMission.recPVTemp>0 )&&( sysXMTMission.recPVTemp<1000 )&&(sysXMTMission.IsValid<5))
		{
			//Trace(" \r\nCrecPVTemp = %02d,recSVTemp = %02d",sysXMTMission.recPVTemp,sysXMTMission.recSVTemp);
			sysXMTMission.IsValid++;
			return 1;
		}		
		sysXMTMission.IsValid=0;
		sysXMTMission.recPV[0]=0;
		sysXMTMission.recPV[1]=0;
		sysXMTMission.recPVTemp=sysXMTMission.recPV[1]*256+sysXMTMission.recPV[0];
		sysXMTMission.recSV[0]=0;
		sysXMTMission.recSV[1]=0;
		sysXMTMission.recSVTemp=sysXMTMission.recSV[1]*256+sysXMTMission.recSV[0];	    
		if(cmdsetflag==1)
		{
			sysXMTMission.recHY[0]=0;
			sysXMTMission.recHY[1]=0;
			sysXMTMission.recHYTemp=sysXMTMission.recHY[1]*256+sysXMTMission.recHY[0];
		}
		//Trace(" \r\nDrecPVTemp = %02d,recSVTemp = %02d",sysXMTMission.recPVTemp,sysXMTMission.recSVTemp);
	}

	
	return 0;
			
}



unsigned char XMTMission_GetTemperature( void )
{    
    unsigned char retry = 3; 	
	unsigned char flag = 0;
  
	//1.
    XMTSerialInit();
	delay( 200 );
	//2.
    retry = ITL_COM_RETRY;
TAB_XMT_GETTEMP_RETRY:	
	flag = XMTMsgPackSend( XMT_CTR_GETTEMPOPT, 0,0);

	if(flag == ITL_ERR_NULL)
		Trace(" \r\n=");
	if( flag != ITL_ERR_NULL )
    {
		return ITL_ERR_PAR;
	}

	//OSTimeDly(20);//by gzz 20120505
	
	//flag = XMTBusUnpack(0);

	//if(flag)
	//	Trace(" =");
	sysXMTMission.msTimer = ITL_TIME_OUT;
	while( sysXMTMission.msTimer )
	{
		if( XMTBusUnpack(0) )
		{ 
			goto TAB_XMT_GETTEMP_COM_OK;
		}
	}
	if( retry-- )
	{
		goto TAB_XMT_GETTEMP_RETRY;
	}
	else
	{
        return ITL_ERR_COM;
	}
TAB_XMT_GETTEMP_COM_OK:	
    return ITL_ERR_NULL;

}


//cmdsetflag=1设置温度控制偏移精度,cmdsetflag=0设置温度
unsigned char XMTMission_SetTemperature( unsigned int temparater,unsigned char cmdsetflag )
{    
    unsigned char retry = 0;	
	unsigned char flag = 0;
  	Trace("\r\n option=TEMP=");
	//1.
    //XMTSerialInit();
	delay( 200 );
	//2.
    retry = ITL_COM_RETRY;
TAB_XMT_SETTEMP_RETRY:		
	flag = XMTMsgPackSend( XMT_CTR_SETTEMPOPT, temparater, cmdsetflag);

	//if(flag == ITL_ERR_NULL)
	//	Trace(" =");

	//OSTimeDly(20);//by gzz 20120505
	
	if( flag != ITL_ERR_NULL )
	{
		return ITL_ERR_PAR;
	}
	//flag = XMTBusUnpack(cmdsetflag);

	//if(flag)
	//	Trace(" =");
	sysXMTMission.msTimer = ITL_TIME_OUT;
	while( sysXMTMission.msTimer )
	{
		if( XMTBusUnpack(cmdsetflag) )
		{ 
			goto TAB_XMT_SETTEMP_COM_OK;
		}
	}
	if( retry-- )
	{
		goto TAB_XMT_SETTEMP_RETRY;
	}
	else
	{
        return ITL_ERR_COM;
	}
	
TAB_XMT_SETTEMP_COM_OK: 
    return ITL_ERR_NULL;

}

//设置温度API,返回值1设置失败,0设置正确
unsigned char XMTMission_SetTemperatureS( unsigned int temparater )
{
	uint8_t TempCtr = 1;
	TempCtr=XMTMission_SetTemperature(temparater,0);
	if(TempCtr==ITL_ERR_NULL)
	{									
		Trace("\r\n +++SetTemp over");
		OSTimeDly(OS_TICKS_PER_SEC/10);
		XMTMission_SetTemperature(20,1);
		OSTimeDly(OS_TICKS_PER_SEC/10);
		Trace("\r\n +++GetTemp");
		XMTMission_GetTemperature();
		OSTimeDly(OS_TICKS_PER_SEC/10);
		Trace("\r\n +++GetTemp over");					
	}
	return TempCtr;
}

