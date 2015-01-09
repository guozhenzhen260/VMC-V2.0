#ifndef _CHANGERACCEPTERAPI_H

extern uint8_t MDBchange(unsigned char* results,unsigned char* recoin, int money);

extern uint8_t ChangerDevPayoutAPI(uint32_t money,uint32_t *debtMoney);

extern void ChangerDevInitAPI(void);	

extern void TestHopperHandle(unsigned char HopperNum,unsigned char OutCount);

#endif
