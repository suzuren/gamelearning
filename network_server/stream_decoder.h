
#ifndef __STREAM_DECODER_
#define __STREAM_DECODER_




int GetPacketHeaderLength();

int ParsePacket(char * data, int len);

#endif


