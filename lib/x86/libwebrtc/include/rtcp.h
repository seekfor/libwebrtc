#ifndef _RTCP_H_
#define _RTCP_H_

#include <stdint.h>
#include <endian.h>

typedef enum
{
	RTCP_FIR = 192,
	RTCP_SR = 200,
	RTCP_RR = 201,
	RTCP_SDES = 202,
	RTCP_BYE = 203,
	RTCP_APP = 204,
	RTCP_RTPFB = 205,
	RTCP_PSFB = 206,
	RTCP_XR = 207,
}RtcpType;

typedef struct
{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t version:2;
	uint16_t padding:1;
	uint16_t rc:5;
	uint16_t type:8;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t rc:5;
	uint16_t padding:1;
	uint16_t version:2;
	uint16_t type:8;
#endif
	uint16_t length:16;
}RtcpHeader;

#ifdef __cplusplus
extern "C"
{
#endif

	int rtcpValidate(char* packet,size_t size);

#ifdef __cplusplus
}
#endif

#endif
