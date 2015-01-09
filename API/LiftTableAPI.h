#ifndef _LIFTTABLEAPI_H
#define _LIFTTABLEAPI_H


unsigned char LiftTableHandle(unsigned char HandleType,unsigned char ChannelNum,unsigned char LevelNum,unsigned char Binnum,unsigned char *BackMsg);

unsigned char TestLiftTablePost(unsigned char HandleType,unsigned char ChannelNum,unsigned char LevelNum,unsigned char Binnum);
unsigned char TestLiftTablePend(unsigned char *BackMsg);
unsigned char liftTableLight(unsigned char cabinet,unsigned char type);
unsigned char liftTableChuchou(unsigned char cabinet,unsigned char type);

void liftSetReady(unsigned char flag);

#endif
