
#ifndef __STREAM_DECODER_
#define __STREAM_DECODER_

#include "network_oper_define.h"


int GetPacketHeaderLength();

int ParsePacket(char * data, int len);

#endif


