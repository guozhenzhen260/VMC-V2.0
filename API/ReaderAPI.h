#ifndef _READERACCEPTERAPI_H


extern void ReaderDevInitAPI(void);

extern void ReaderDevDisableAPI(void);

extern void ReaderDevEnableAPI(void);

extern uint32_t GetReaderDevMoneyInAPI(uint8_t *readerType);

extern uint8_t ReaderDevVendoutTestAPI(uint16_t money,uint8_t vendrst);

extern uint8_t ReaderDevVendoutRPTAPI(uint16_t money);

extern void ReaderDevVendoutResultAPI(uint8_t result);

extern void testReaderDevVendoutAPI(uint16_t money);

extern void ReaderDevCashSaleAPI(uint16_t money);


#endif
