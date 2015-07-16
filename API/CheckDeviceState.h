#ifndef _CHECKDEVSTATE_H

extern	void CheckDeviceState(void);

extern  void SetScaleError(uint8_t errorValue);

extern  uint8_t GetScaleError(void);

extern  void SetWeihuStatus(uint8_t enable);

extern uint8_t GetWeihuStatus(void);

extern  uint8_t IsErrorState(void);

extern uint8_t HopperIsEmpty(void);

extern uint8_t ChangerIsErr(void);

extern uint8_t BillIsErr(void);

extern void SetBillCoinStatus(uint8_t type,uint8_t enable);

extern uint8_t GetBillCoinStatus(uint8_t type);

extern uint8_t ErrorStatus(uint8_t type);

extern uint8_t SIMPLEErrorStatus(uint8_t type);
#endif

