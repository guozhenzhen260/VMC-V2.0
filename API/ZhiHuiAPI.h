
#ifndef __ZHIHUIAPI_H__
#define __ZHIHUIAPI_H__


#define TRADE_PAGE_MAX			3990				//交易记录在flash中最大页数


#define print_pc(...)		TracePC(__VA_ARGS__)

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



//=============================================================================
//获取货道定义
//=============================================================================
#define AISLE_PRICE		1
#define AISLE_COUNT		2
#define AISLE_STATE	  	3
#define AISLE_MAXCOUNT	4
#define AISLE_SELCET	5
#define AISLE_SELL_NUM	6
#define AISLE_ID	    7


//=============================================================================
//ZHIHUIPC
//=============================================================================

//#define ZH_DEBUG //调试用正式版要取消
#define RECV_SIZE			     256	    //主控板POLL指令后 PC回应包size
#define SEND_SIZE		         256     //  发送串口数据size 
#define VMC_ONLINE				 0x01
#define VMC_OFFLINE				 0x02


//=============================================================================
//机器配置信息
//=============================================================================
#define MAC_T_F				 	 0x11	//食品机
#define MAC_T_G				 	 0x21//其它
#define MAC_T_D				 	 0x01//饮料机
#define MAC_T_YT				 0x20 //一拖多

#define AISLE_SIZE			     48//默认80货道
#define FOOD_ROW    		 	 (uint8_t)6     	//货道排数
#define FOOD_COLUMN 		 	 (uint8_t)8     	//货道列数

#define ID_BYTES			         1  //商品ID用ID_BYTES 字节


#define MBOX_ST_LEN				 0x08 //与智慧有关的邮箱通信结构体长度
#if 0
#define MAC_SN					 (uint32_t)11040070  //机器型号
#define VMC_SN					 (uint16_t)0x1212	 //主控板版本号
#define MAC_T					 (uint8_t)MAC_T_D    //机器类型 0X01饮料机，0X11食品机，0X21
#define AISLE_SUM				 48//货道总数 8*3 
#define DRINK_SIZE				 48//饮料机最大货道数
#endif


//=============================================================================
//售货机投钱方式
//=============================================================================

#define ZH_BILL					1
#define ZH_COIN				    2
#define ZH_CARD					3
#define ZH_ESCROWIN				4
#define ZH_ESCROWOUT			5


//=============================================================================
// ACK信号及错误码
//=============================================================================

#define ZH_R_NAK						 0x00 
#define ZH_R_ACK						 0x01
#define ZH_R_MESG					 	 0x02
#define ZH_R_ERR_CRC					 0x03	//crc校验错误 接收包不正确
#define ZH_R_TIMEOUT					 0x04	//超时
#define ZH_R_ERR_U				 		 0x05
#define ZH_R_RTIMEOUT					 0x06   //余下字节接收超时



#define ACK_TIMEOUT_SIZE		 15    //接收ACK超时时间

//=============================================================================
// 特征字
//=============================================================================

#define ADDR_T					 0xEF	
#define CMD_T					 0xEE
#define DATA_T					 0xFE
#define ACK_T				     0x00	
#define NAK_T			         0x15

#define ACK_PC					 0x01
#define NAK_PC				     0x15

//=============================================================================
//type 协议包类型 及包长
//=============================================================================

#define PC_SHIPMENT_REQUEST 	 0x01   //出货请求
#define PC_SYS_TIME_SET			 0x02   //系统时间设置
#define PC_DEDUCT_SHIP			 0x03   //扣款出货请求(泡面柜和食品柜，支持饮料柜)
#define PC_GOODS_SET		     0x04	//设置VMC系统商品序号 单价
#define PC_ADD_GOODS 			 0x05   //设置VMC系统加货料道
#define PC_PRICE_SET			 0x06	//设置VMC系统商品价格
#define PC_CLEAR_SET			 0x70  //设置清零
#define PC_AISLE_SET			 0x71  //设置货道
#define ACK_OFF				     0     	//返回协议包的数据偏移


#define VMC_T_POLL        			 0x76	//轮询主机
#define VMC_T_REGISTER 	   			 0x78	//签到
#define VMC_T_TRADE   			 	 0x7C	//出货
#define VMC_T_AISLE_RUN 			 0x79	//料道运行信息
#define VMC_T_MAC_RUN   			 0x7D	//机器运行信息
#define VMC_T_CONFIG		 		 0x7A	//系统配置信息
#define VMC_T_GOODS_SN	 			 0x7E	//料道商品信息
#define VMC_T_GOODS_PRICE 		     0x7F   //料道价格信息
#define VMC_T_SUM 				     0x7B   //商品销售汇总信息

#define VMC_L_POLL        			 (10 + ID_BYTES)	//轮询主机
#define VMC_L_REGISTER 	   			 20	//签到
#define VMC_L_TRADE   			 	 (28 + ID_BYTES)	//出货
#define VMC_L_AISLE_RUN 			 (AISLE_SIZE + 7)  //55 - 48    	//料道运行信息
#define VMC_L_MAC_RUN   			 10	 	//机器运行信息
#define VMC_L_CONFIG		 		 17	//系统配置信息
#define VMC_L_GOODS_SN	 			 (AISLE_SIZE * ID_BYTES + 1) //49	//料道商品信息
#define VMC_L_GOODS_PRICE 		     (AISLE_SIZE + AISLE_SIZE + 1) //97//97  -48    //料道价格信息
#define VMC_L_SUM 				     76   //商品销售汇总信息



#define ZH_L_TRADE					 21 //邋PC出货请求数据长度
#define ZH_L_DATETIME				 8
#define ZH_L_AISLEINFO				 8
#define ZH_L_AISLECOUNT				 6


//出货结果
#define CHUHUO_OK  	        0x00   		//出货成功
#define CHUHUO_PRICE_ERR 	0x01		//单价为零或者PC以现金扣款方式出货时 发送的扣款金额与商品单价不符
#define CHUHUO_QUEHUO		0x02		//缺货
#define CHUHUO_AISLE_FAULT  	0x03	//货道故障
#define CHUHUO_AISLE_NOEXIST	0x04 	//无此货道
#define CHUHUO_SYS_FAULT	0x05		//系统故障
#define CHUHUO_UNABLE_COST	0x06		//金额不足，不够扣钱
#define CHUHUO_UNABLE_PAYOUT	0x07	//不够找零
#define CHUHUO_UNABLE_PC	0x08		//已投入现金 禁止非现金(TRADE_CASH || TRADE_ONECARD)方式出货 
#define CHUHUO_YACHAO_FAIL	0x09		//压钞失败
#define CHUHUO_CABINET_ERR	0x10		//柜号错误
#define CHUHUO_TYPE_ERR		0x11		//出货类型不是货道号或者ID号
#define CHUHUO_ID_ERR		0x12		//商品的ID号错误
#define CHUHUO_NO_ERR		0x13		//货道号错误
#define CHUHUO_BUTTON_ERR	0x14		//有用户按键选货则60秒内拒绝PC出货
#define CHUHUO_ERR		0x99		//未知错误


//=============================================================================
//购物方式
//=============================================================================

#define TRADE_CASH				 0x01 //现金支付
#define TRADE_ONE_CARD			 0x02 //一卡通
#define TRADE_UNION_CARD 	 	 0x11//银联卡
#define TRADE_PC_NORMAL			 0x21//PC 正常出货
#define TRADE_PC_ALIPAY			 0x41//支付宝

#define BATCH_FLASH_PAGE		3997
#define TRADE_FLASH_PAGE		3999
#define BATCH_FLASH_SIZE		76

#define FLASH_WRITE_SIZE	    510 //定义写flash 一页 最大字节数

#define TRADE_PAGE_SUM			(TRADE_PAGE_MAX + 1)//交易记录最大笔数
#define OFFLINE_REPORT_GPRS		0
#define OFFLINE_REPORT_PC		1


//=============================================================================
//配置信息
//=============================================================================
#define NONSUPPORT				 0x99 //不支持
#define NONSUPPORT_BYBCD		 99 //不支持


//=============================================================================
//宏函数及表达式
//=============================================================================

#define SN_NO(n)		(((n)/16-1)*COLUMN+((n)&0x0f))//逻辑编号转1-48货道号				
#define ARR_NO(n)			    (((n)-1)/8)     //根据货到编号查找数组下标号
#define ARR_BIT(n)				(((n)-1)%8)		//根据货道编号查找数组元素位号bit0-7
#define BIT_MASK(n)				(0x01<<ARR_BIT(n))//货道掩码

#define  HEC_to_ASC(n) ((n) + 48)//限数字
#define  ASC_to_HEC(n) ((n) - 48)
#define HUINT16(n)				(unsigned char)(((n) >> 8) & 0xff)//16位数值高8位
#define LUINT16(n)				(unsigned char)((n) & 0xff)//16位数值低8位	
#define H0UINT32(n)				(unsigned char)(((n) >> 24) & 0xff)//32位数值第一个高8位
#define H1UINT32(n)				(unsigned char)(((n) >> 16) & 0xff)//32位数值第二个高8位
#define L0UINT32(n)				(unsigned char)(((n) >> 8) & 0xff)//32位数值第一个低8位
#define L1UINT32(n)				(unsigned char)((n) & 0xff)//32位数值第二个低8位
#define yoffset_of(type,member) ({(INT32U & ((type*)(0)->member));})
//32位处理器ptr结构体中member地址，type结构体类型member结构体成员
#define ycontain_of(ptr,type,member) ({	\
		(type*)((INT32U*)ptr - yoffset_of(type,member)); \
		})
	//两个unsigned char 数据整合成unsigned short型数据
#define INTEG16(h,l)			(unsigned short)(((unsigned short)(h) << 8) | ((l) & 0xff))
	//四个unsigned char 数据整合成unsigned int型数据
#define INTEG32(h0,l0,h1,l1)	(unsigned int)(((unsigned int)(h0) << 24) | \
									((unsigned int)(l0) << 16) | \
									((unsigned int)(h1) << 8) | \
									((l1) & 0xff))



//=============================================================================
//协议包结构体声明
//=============================================================================

typedef struct _zhihui_msg{
	//报文头
	unsigned short len;//报文长度 不包含长度本身，报文头 - 4 + 报文体 4位 整形
	unsigned int terminal;//8位 整形
	unsigned char  version;//2位 整形
	//unsigned char  reserve[6];//6位保留 字符型
	unsigned char  direction; //方向1位 字符型
	unsigned char  contrl; // 控制码 2位 整形
	//发送包
	unsigned char  send_msg[SEND_SIZE];
	unsigned short send_len;
	//应答包
	unsigned char  recv_msg[RECV_SIZE];
	unsigned short recv_len;
	
	unsigned char  ship_cache_len;
	unsigned char  recv_err_num;	
}ZHIHHUI_MSG;


//=============================================================================
//商品信息结构体声明
//=============================================================================

typedef struct _good_info{
	unsigned char	num;//商品所在货道号1-24
	unsigned char	logic_no;//商品逻辑编号 十进制

}GOOD_INFO;	

//=============================================================================
//售货机信息结构体声明
//=============================================================================

typedef struct _machine_info{
	unsigned char row;
	unsigned char column;
	unsigned char aisle_sum;//实际货道数
	unsigned char mac_t;//机器类型
	unsigned int mac_sn; //机器编号
	//unsigned char mac_sn[4];
	unsigned int vmc_sn;//主控版本号
	unsigned char  drink_size;	
	unsigned char c_no;//货柜号
}MACHINE_INFO;


//=============================================================================
//整机信息结构体声明
//=============================================================================

typedef struct _data_st{

	//机器配置信息
	MACHINE_INFO machine_info;
	//机器状态	
	unsigned char	vend_mac_state;//售货机状态 Bit7读卡器无应答;Bit6红外模块故障;Bit5右室温度异常;Bit4左室温度异常;
						   //Bit3右室温度传感器故障;Bit2左室温度传感器故障;Bit1系统时钟不正常（警告）;Bit0驱动板无应答(暂停营业);
	unsigned char	mdb_mac_state;//纸币器状态Bit7保留;Bit6纸币堵塞（纸币器口）;Bit5纸币器钱箱满（钱箱）;Bit4纸币器钱箱被取走（钱箱;
    					  //Bit3纸币器ROM校验错Bit2;纸币器传感器故障;Bit1纸币器驱动马达故障;Bit0纸币器无应答;
	unsigned char	coin_mac_state;//硬币器状态Bit7硬币器1元缺币（钱币）;Bit6硬币器5角缺币（钱币）;Bit5硬币支出堵塞（硬币口）;
	//Bit4硬币接收堵塞（硬币口）;Bit3硬币器ROM校验错;Bit2硬币器传感器故障;Bit1硬币器工作电压低（警告）;Bit0硬币器无应答
	unsigned char	light_state;//照明状态0X00：自动；0X01：常闭；0X02：常亮	   
	unsigned char	lamp_time[4];//日光灯时间1-2：开灯时间HHMM压缩3-4：停止时间HHMM压缩
	unsigned char	left_room_state;//左室状态
	unsigned char	right_room_state;//右室状态
	unsigned char	energy_save_time[4];//节能时间
	unsigned char	cool_temp[2];//制冷温度 分左右室 bit7表示符号
	unsigned char	hot_temp[2];   //加热温度 分左右室 bit7表示符号
	unsigned char	run_state; //运行状态Bit1:0门关；1门开Bit0:0营业；1暂停营业
	//客户与机器交互信息
	unsigned short	bill_num;//纸币数量
	unsigned short	coin_num1;//硬币0.5元个数
	unsigned short	coin_num2;//硬币1元个数
	unsigned short	coin_amount;//硬币接收金额
	unsigned short	bill_amount;//纸币接收金
	unsigned short  card_amount;
	unsigned short  all_amount;
	unsigned short	change_amount;//找零
	unsigned char	sel_num;//客户选择货道号
	unsigned short	sel_id; //客户选择商品序号
	unsigned short	sel_amount;//客户选择商品价格	
	unsigned char   escrow_flag;
	//PC   出货信息
	unsigned char	trade_run_no;//主控板产生的流水号 0-255
	unsigned char	trade_run_num[3];//PC产生的六位BCD码流水号
	unsigned char	trade_gprs_rum_num;
	unsigned char	trade_card_no[10];//10位为20位卡号编码压缩，卡号不足20位前补0。
	unsigned char	trade_state;//出货情况 
	unsigned char	trade_pay_mode;//支付方式
	unsigned short	trade_price;
	unsigned char	trade_time[7];//出货时间
	unsigned char   trade_num;
	unsigned char	trade_sn;
	
	unsigned char	sell_config;  //售卖配置
	unsigned char	usr_trade;// 1表示用户正在与售货机 会话	  0空闲		
	unsigned char	driver_state;//驱动故障 暂停营业
	unsigned char	online;
	//unsigned char	err_report_pc;//主板机与PC错误链接数
	unsigned char   isWeihu;//进入维护模式
	//unsigned char   clear_state;//清零标志 1清零
	unsigned char   door_state;//门状态标志 1 表示由开
	unsigned char   rt_state; //人体感应 1表示有人靠近 0 表示无

	//gprs
	unsigned short	read_index;
	unsigned char	read_sub_index;
	unsigned short	write_index;
	
}MACDATA_ST;

//=============================================================================
//关于智慧PC的邮箱结构体
//=============================================================================


//=============================================================================
//变量声明
//=============================================================================
extern MACDATA_ST MACData;
extern ZHIHHUI_MSG zhihui_msg;
extern GOOD_INFO vmc_goods[AISLE_SIZE];

//=============================================================================
//函数声明
//=============================================================================
unsigned char crc_check(unsigned char *msg, unsigned short len);
extern void zh_task3_post(unsigned char type);
extern unsigned char zh_task3_pend(uint32_t payAllMoney);
extern void zh_vmc_post(unsigned char type);
extern void vmcEndTrade(void);
void pollHuman(unsigned char flag);
void pollDoorAPI(unsigned char flag);

//智慧接口
void zh_goodsID_set(void *pc_msg);

void zh_fault_update(void);
void zh_goods_update(unsigned char mode);
void zh_room_update(void);
void zh_run_update(void);
void zh_all_update(void);
void zh_init(void);
extern void vmcEndTrade(void);


unsigned int getAisleInfo(unsigned char logicNo,unsigned char type);
unsigned char setAisleInfo(unsigned char logicNo,unsigned char type,const unsigned int value);
unsigned char vmc_check_huodao(unsigned char logicNo,unsigned char cabinetNo);
unsigned char vmc_check_no(unsigned char no);
unsigned char vmc_batch_info_read(unsigned char *data);
unsigned char vmc_batch_info_write(void);
unsigned char vmc_trade_info_read(void);
unsigned char vmc_trade_info_write(void);

unsigned char vmc_logic_to_no(unsigned char logic_no,GOOD_INFO *good_info);
unsigned char vmc_sn_to_no(unsigned char sn);

unsigned char pc_trade_info_clear(unsigned char mode);

unsigned char vmc_data_clr(MACDATA_ST *mac_data);

void gprs_clear_index(void);








#endif
