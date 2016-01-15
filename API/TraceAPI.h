#ifndef _TRACEAPI_H

extern	void TraceBill(unsigned char *format,...);

extern  void TraceCoin(unsigned char *format,...);

extern	void TraceChange(unsigned char *format,...);

extern  void TraceReader(unsigned char *format,...);

extern	void TraceChannel(unsigned char *format,...);

extern  void TraceCompress(unsigned char *format,...);

extern  void TraceLed(unsigned char *format,...);

extern  void TraceSelection(unsigned char *format,...);

extern  void TracePC(unsigned char *format,...);

extern  void print_fs(unsigned char *format,...);

#endif
