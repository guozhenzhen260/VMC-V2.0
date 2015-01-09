/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           SELFCHECK_MENU.C
** Last modified Date:  2013-03-04
** Last Version:         
** Descriptions:        设备自检主菜单                  
**------------------------------------------------------------------------------------------------------
** Created by:          gzz 
** Created date:        2013-03-04
** Version:              
** Descriptions:        The original version       
********************************************************************************************************/

/***********************************设备自检菜单按键选择************************************************/
typedef struct
{
	unsigned char KeyCurrent;						//当前菜单索引号
	unsigned char ExtSelfCheck;						//退出设备自检菜单并返回维护主菜单
	unsigned char PageNumb;							//菜单页索引
	void (*TestOperate)();							//当前状态应该执行的功能操作
} SELFCHECKMENU;									//维护主菜单结构体

SELFCHECKMENU SelfCheckMenu;
/**************************************设备自检主菜单****************************************************/
typedef struct
{
	char *ModeCapion[MAXLANGUAGETYPE];				//自检模式提示
	char *EnterPin[MAXLANGUAGETYPE];				//提示输入密码
	char *PinError[MAXLANGUAGETYPE];				//密码错误提示
	char *ReturnCapion[MAXLANGUAGETYPE];			//返回提示
	char *PageDown[MAXLANGUAGETYPE];					//上页提示
	char *PageUp[MAXLANGUAGETYPE];				//下页提示
	//以下为密码验证后的菜单项
	char *BillValidatorTest[MAXLANGUAGETYPE];		
	char *CoinAcceptorTest[MAXLANGUAGETYPE];
	char *CoinChangerTest[MAXLANGUAGETYPE];
	char *GoodsChannelTest[MAXLANGUAGETYPE];
	char *CashlessDeviceTest[MAXLANGUAGETYPE];

	char *KeyBoardTest[MAXLANGUAGETYPE];
	char *SelectKeyTest[MAXLANGUAGETYPE];
	char *AtcTest[MAXLANGUAGETYPE];
	char *TuibiTest[MAXLANGUAGETYPE];
} SELFCHECKMENULIST;

const SELFCHECKMENULIST SelfCheckMenuList = 
{
	{
		"设备自检",
		"SELFCHECK",
		"SELFCHECK",
		"SELFCHECK",
		"SELFCHECK"
	},
	{
		"请输入密码：",
		"Enter Password:"
	},
	{
		"密码错误！！",
		"Password Error!!"
	},
	{
		"请按“取消”返回",
		"Press<Cancel>return"
	},
	{
		"“下一页”",
		"<Next page>",
		"<Next page>",
		"<DIGER SAYFA>",
		"<Next page>"
	},
	{
		"“上一页”",
		"<Pre page>",
		"<Pre page>",
		"<ONCEKI SAYFA>",
		"<Pre page>"
	},
	//一下通过密码验证后
	{
		"① 纸币器测试",
		"1.BillValidator",
		"1.BillValidator",
		"1.BillValidator",
		"1.BillValidator"
	},
	{
		"② 硬币器测试",
		"2.CoinAcceptor",
		"2.CoinAcceptor",
		"2.CoinAcceptor",
		"2.CoinAcceptor"
	},
	{
		"③ 找零器测试",
		"3.Changer",
		"3.Changer",
		"3.Changer",
		"3.Changer"
	},
	{
		"④ 货道出货测试",
		"4.GoodsChannel",
		"4.GoodsChannel",
		"4.GoodsChannel",
		"4.GoodsChannel"
	},
	{
		"⑤ 读卡器测试",
		"5.Cashless",
		"5.Cashless",
		"5.Cashless",
		"5.Cashless"
	},
	{
		"⑥ 按键测试",
		"6.KeyBoard",
		"6.KeyBoard",
		"6.KeyBoard",
		"6.KeyBoard"
	},
	{
		"⑦ 选货按键测试",
		"7.SelectKey",
		"7.SelectKey"	,
		"7.SelectKey"	,
		"7.SelectKey"	
	},
	{
		"⑧ 温控器测试",
		"8.ATC",
		"8.ATC",
		"8.ATC",
		"8.ATC"
	},
	{
		"⑨ 退币按键测试",
		"9.Other Test",
		"9.Other Test",
		"9.Other Test",
		"9.Other Test"
	}
};


/**************************************设备自检提示语****************************************************/
typedef struct
{
	char *Balance[MAXLANGUAGETYPE];	
	char *Change[MAXLANGUAGETYPE];	
} SELFCHECKTEXT;

const SELFCHECKTEXT SelfCheckText = 
{
	{
		"金额:",
		"Amount:"	,
		"Баланс:",
		"para girin:",
		"SALDO:"
	},
	{
		"找零:",
		"Change:"	,
		"Change:",
		"Change:",
		"Change:"
	}
};
/**************************************End Of File*******************************************************/

