#ifndef _LFMDBREADER_H

#define RTV_JUST_RESET					0x00
#define RTV_READER_CONFIG_INFO			0x01
#define RTV_DISPLAY_REQUEST				0x02
#define RTV_BEGIN_SESSION				0x03
#define RTV_SESSION_CANCEL_REQUEST		0x04
#define RTV_VEND_APPROVED				0x05
#define RTV_VEND_DENIED					0x06
#define RTV_END_SESSION					0x07
#define RTV_CANCELLED					0x08
#define RTV_PERIPHERAL_ID				0x09
#define RTV_MALFUNCTION_ERROR			0x0A
#define RTV_CMD_OUT_OF_SEQUENCE			0x0B


extern void LFReaderDevProcess(uint32_t *RecvMoney,uint8_t cmdOpt,uint16_t  ReaderPrice, uint8_t *readerStatus);

extern uint8_t LFReaderDevInit(void);

extern void LFReaderDevDisable(void);

extern void LFReaderDevEnable(void);

extern void LFReaderSessionVendRequest(uint16_t ItemPrice);

extern void LFReaderSessionVendSuccess(void);

extern void LFReaderSessionVendFail(void);


#endif
