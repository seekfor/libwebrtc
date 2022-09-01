#ifndef _SDP_H_
#define _SDP_H_

#include <stdlib.h>
#include <stdint.h>
#include "rtp.h"
#include "media.h"

#define SDP_MAX_SIZE 10240

typedef struct
{
	size_t size;
	char content[SDP_MAX_SIZE];
}SDP;

#ifdef __cplusplus
extern "C"
{
#endif

	SDP* sdpCreate(void);
	void sdpDestroy(SDP* sdp);
	int sdpAppend(SDP* sdp, const char *attribute, ...);
	int sdpAppendNew(SDP* sdp, const char *attribute, ...);
	int sdpUpdateMDNS(char *candidate_src, char *candidate_dst, size_t size);
	char* sdpGetContent(SDP* sdp);
	void sdpAddCodec(SDP* sdp, MediaCodec codec,MediaDirection direction, const char *ufrag, const char *password, const char *fingerprint, int mid);
	uint32_t sdpFindSSRC(const char *type, const char *sdp);
	RtpMap sdpParseRTPMAP(const char *sdp);

#ifdef __cplusplus
}
#endif

#endif
