#ifndef _BILLACCEPTERAPI_H


extern void BillDevInitAPI(void);

extern void BillDevDisableAPI(void);

extern void BillDevEnableAPI(void);

extern void BillDevResetAPI(void);

extern uint32_t GetBillDevMoneyInAPI(void);

extern uint32_t GetBillDevMoneyUnStackInAPI(void);

extern uint8_t StackedBillDevMoneyInAPI(void);

extern uint8_t ReturnBillDevMoneyInAPI(void);

extern unsigned char BillRecyclerPayoutNumExpanseAPI(unsigned int RecyPayoutMoney,unsigned char RecyPayoutNum);

extern unsigned char BillRecyclerPayoutValueExpanseAPI(unsigned int RecyPayoutMoney,uint32_t  *RecyPayoutMoneyBack);

#endif
