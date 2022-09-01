#ifndef RTP_DEPACKETIZER_H_
#define RTP_DEPACKETIZER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "media.h"
#include <rtp-packet.h>
#include <rtp-payload.h>

typedef int (*rtp_depacketizer_callback)(void* args,int video,char* buf,int size,unsigned int pts,int flags);

typedef struct
{
	struct rtp_payload_t handler;
	void* decoder;
	char codec[32];
	char buf[1024 * 256];


	rtp_depacketizer_callback callback;
	void* args;
}RtpDepacketizer;

#ifdef __cplusplus
extern "C"
{
#endif

	void* rtpdpkCreate(int codec,rtp_depacketizer_callback callback,void* args);
	void rtpdpkDestroy(void* handler);
	void rtpdpkProcess(void* handler, uint8_t *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif 
