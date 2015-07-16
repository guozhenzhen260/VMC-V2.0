

#ifndef __TASKUART0DEVICE_H__
#define __TASKUART0DEVICE_H__


//调试宏
#define  PRINT_ENABLE         0
#if PRINT_ENABLE == 1
#define  print_log(...)       Trace(__VA_ARGS__)
#define  print_err(errno)        Trace("ERROR: In %s at Line %u - errno = %d\n", __FUNCTION__, __LINE__, errno)
#define  print_warning(no)		 Trace("Warning: In %s at Line %u - no = %d\n", __FUNCTION__, __LINE__, no)

#else 
#define  print_log(...)       do {} while(0)
#define  print_err(errno)      do {} while(0)
#define  print_warning(no)	  do {} while(0)

#endif

#define PRINT_HEFAN_ENABLE  0
#if PRINT_HEFAN_ENABLE == 1
#define  print_hefan(...)       Trace(__VA_ARGS__)

#else
#define  print_hefan(...)       do {} while(0)
#endif


//=====================创建堆栈监控任务=========================================================

#define STK_CHECK_TASK_SIZE     256
#define STK_CHECK_TASK_PRIO     18





//=============================================================================================

#define PC_CONTROL_TASK_STK_SIZE  1024
#define PC_CONTROL_TASK_PRIO       20



#define PC_DELAY_500MS			OSTimeDly(10)				
#define PC_DELAY_200MS			OSTimeDly(40)
#define PC_DELAY_50MS			OSTimeDly(10)
#define PC_DELAY_POLL			OSTimeDly(100)
#define PC_DELAY_1S			    OSTimeDly(200)


//PCM :pc control the vmc
#define PCM_MAX_SIZE		5//消息队列大小
#define UART0_SIZE			10
typedef struct _pcconmsg_st
{
	unsigned char type;
	unsigned char msg[20];

}PCCONMSG;

typedef struct _pcm_msg{
	//报文头
	//发送包
	unsigned char  send_msg[256];
	unsigned short send_len;
	//应答包
	unsigned char  recv_msg[256];
	unsigned short recv_len;
	
}PCM_MSG;


extern OS_EVENT *pcConMail;
extern OS_EVENT *pcConMailBack;
extern PCCONMSG vmc_pc_mail[PCM_MAX_SIZE];
extern PCCONMSG pc_vmc_mail;
extern OS_STK pcControlTaskStk[PC_CONTROL_TASK_STK_SIZE];
unsigned char pcControlInit(void);
void enterPCControl(void *pd);
void pcPost(unsigned char type);
void vmcPost(unsigned char type);
void pcTesttingAisle(PCCONMSG *mail);
void sysBoardCastCheckTask(void);
unsigned char pcShcHandle(unsigned char port);
void mainTaskPollPC(unsigned char state);
unsigned char pcEncodAmount(unsigned int amount);



#endif
