#ifndef H264_PACKETIZER_H_
#define H264_PACKETIZER_H_

#include <stdint.h>
#include <rtp-packet.h>
#include <rtp-payload.h>
#include "peer.h"

typedef struct
{
	struct rtp_payload_t handler;
	void* encoder;
	WEBRTC* pc;
	void* janus;
	char buf[256 * 1024];
}RtpPacketizer;

typedef int (*rtp_packetizer_callback)(void* args,char* buf,int size);

#ifdef __cplusplus
extern "C"
{
#endif

	void* rtpkCreate(int codec,void* args);
	void rtpkDestroy(void* handler);
	void rtpkProcess(void* handler,uint8_t *buf,size_t size,unsigned long timestamp);

#ifdef __cplusplus
}
#endif



#endif
