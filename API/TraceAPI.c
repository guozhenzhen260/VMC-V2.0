/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           TraceAPI
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        监控状态相关函数声明                     
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


/*********************************************************************************************************
** Function name:       TraceBill
** Descriptions:        纸币器调试信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceBill(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
	
	if( (UserPara.TraceFlag == 1)&&(UserPara.billTrace==1) )
	{		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}
	}
}

/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceCoin(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.coinTrace==1) )
	{		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}


/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceChange(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;

	if( (UserPara.TraceFlag == 1)&&(UserPara.changelTrace==1) )
	{		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}


/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceReader(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.readerTrace==1) )
	{		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}


/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceChannel(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.channelTrace==1) )
	{
		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}



/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceCompress(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.compressorTrace==1) )
	{
		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}


/*********************************************************************************************************
** Function name:       TraceCoin
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceLed(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.ledTrace==1) )
	{
		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}



/*********************************************************************************************************
** Function name:       TraceSelection
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TraceSelection(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.selectTrace==1) )
	{
		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}


/*********************************************************************************************************
** Function name:       TracePC
** Descriptions:        
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void TracePC(unsigned char *format,...)
{	
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
		
	if( (UserPara.TraceFlag == 1)&&(UserPara.PC==1) )
	{
		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}		
	}
}

/*********************************************************************************************************
** Function name:       print_fs
** Descriptions:        纸币找零器调试信息
** input parameters:    无
** output parameters:   无
** Returned value:      
*********************************************************************************************************/
void print_fs(unsigned char *format,...)
{
	va_list arg_ptr;
	unsigned char StringTemp[256];
	unsigned int i;
	unsigned int len;
	
	if( (UserPara.TraceFlag == 1)&&(UserPara.BillRecyclerTrace==1) )
	{		
		va_start(arg_ptr, format);
		len = vsprintf((char *)StringTemp,(const char *)format,arg_ptr);
		va_end(arg_ptr);
		for(i=0;i<len;i++)
		{
			Uart0PutChar(StringTemp[i]);
		}
	}
}


