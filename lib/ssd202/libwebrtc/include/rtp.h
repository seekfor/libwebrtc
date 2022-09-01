#ifndef _RTP_H_
#define _RTP_H_

#include <stdint.h>
#include <endian.h>

typedef struct
{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t version:2;
	uint16_t padding:1;
	uint16_t extension:1;
	uint16_t csrccount:4;
	uint16_t markerbit:1;
	uint16_t type:7;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t csrccount:4;
	uint16_t extension:1;
	uint16_t padding:1;
	uint16_t version:2;
	uint16_t type:7;
	uint16_t markerbit:1;
#endif
	uint16_t seq_number;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t csrc[16];
}RtpHeader;

typedef struct
{
	int pt_vp8;
	int pt_h264;
	int pt_rtx;

	int pt_opus;
	int pt_pcma;
	int pt_pcmu;
}RtpMap;

#ifdef __cplusplus
extern "C"
{
#endif
	
	int rtpValidate(char *packet, size_t size);

#ifdef __cplusplus
}
#endif

#endif

