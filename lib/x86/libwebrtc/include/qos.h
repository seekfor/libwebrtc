#ifndef __QOS_H__
#define __QOS_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>

/*NACK period in ms*/
#define CFG_NACK_PERIOD			10
/*max NACK count*/
#define CFG_NACK_TIMEOUT		8
/*max POOL*/
#define CFG_MAX_POOL			65536
/*extra bytes for libsrtp*/
#define CFG_EXTRA_BYTES			16
/*max frames in queue*/
#ifndef CFG_MAX_FRAME
	#define CFG_MAX_FRAME		8
#endif
#ifndef CFG_MAX_FREE_QUEUE
	#define CFG_MAX_FREE_QUEUE	32
#endif
/*QOS direction*/
#define LIBQOS_RX_MODE			0
#define LIBQOS_TX_MODE			1


/*RTCP types*/
#define LIBQOS_RTCP_SR			200
#define LIBQOS_RTCP_RR			201
#define LIBQOS_RTCP_SDES		202
#define LIBQOS_RTCP_RTPFB		205
#define LIBQOS_RTCP_PSFB		206

/*send rtcp packet*/
#define LIBQOS_MESSAGE_SENDRTCP		0x00
/*send rtp packet*/
#define LIBQOS_MESSAGE_SENDRTP		0x01
/*a PLI request received*/
#define LIBQOS_MESSAGE_PLIREQ		0x02
/*a FIR request received*/
#define LIBQOS_MESSAGE_FIRREQ		0x03
/*a REMB requst received*/
#define LIBQOS_MESSAGE_REMBREQ		0x04
/*callbacks*/
typedef int (*libqos_callback)(void* args,int msg,char* buf,int size);

typedef struct
{
	uint32_t lastntp;
	uint32_t nowntp;
	uint32_t pktscount;
	uint32_t bytescount;
	uint32_t realbps;
	uint32_t calcbps;
}libqos_remb_t;


typedef struct
{
	uint32_t ssrc;
	uint64_t ntp;
	uint32_t pts;
	uint32_t pktscount;
	uint32_t bytescount;	
}libqos_sr_t;

typedef struct
{
	uint32_t ssrc;
	uint8_t  lost;
	uint32_t alllosts; 
	uint32_t seqmsb;
	uint32_t jitter;
	uint32_t lsr;
	uint32_t dlsr;

	uint32_t seq;
	uint64_t pktscount;
	uint64_t bytescount;

	uint32_t pktslost;
	uint32_t pktstotal;
}libqos_rr_t;

typedef struct
{
	char cname[256];
}libqos_sdes_t;


typedef struct
{
	libqos_sr_t sr[2];
	libqos_rr_t rr[2];
	libqos_remb_t remb;
}libqos_rtcp_t;

typedef struct
{
	uint64_t ntpnow;
	uint64_t rtcnow;

	uint32_t srtime;
	uint32_t rrtime;
	uint32_t firtime;
	uint32_t plitime;
	uint32_t rembtime;
}libqos_ntp_t;

typedef struct _libqos_rtpkt_t_
{
	uint32_t idx;
	uint32_t nacks;
	uint32_t utc;

	uint32_t seq;
	uint32_t mark;
	uint32_t pt;
	uint32_t ssrc;
	uint32_t pts;
	int32_t size;
	int8_t buf[1500];

	struct _libqos_rtpkt_t_* next;
	struct _libqos_rtpkt_t_* child;
}libqos_rtpkt_t;


typedef struct
{
	uint32_t stcfirst;
	uint32_t stcnow;
	uint32_t stc;
	uint32_t len;
}libqos_jb_t;


typedef struct
{
	uint32_t mode;
	uint32_t audio_pt;
	uint32_t audio_clock;
	uint32_t audio_ssrc;
	uint32_t video_pt;
	uint32_t video_clock;
	uint32_t video_ssrc;
	uint32_t remote_options;

	uint32_t local_video_ssrc;
	uint32_t local_video_pt;
	uint32_t local_audio_ssrc;
	uint32_t local_audio_pt;
	uint32_t local_options;
	
	libqos_rtcp_t rtcp;
	libqos_ntp_t ntp;

	libqos_callback callback;
	void* args;

	libqos_rtpkt_t* freed_rtp;
	libqos_rtpkt_t* decode_rtp[2];
	libqos_rtpkt_t* incoming_rtp[2][CFG_MAX_POOL];

	libqos_jb_t jb[2];

	int32_t last_seq[2];
	int32_t start[2];
	int32_t stop[2];

	void* mutex;

}libqos_t;

#ifdef __cplusplus
extern "C"
{
#endif
	void* libqosCreate(int mode,int audio_pt,int audio_ssrc,int video_pt,int video_ssrc,libqos_callback callback,void* args);
	int libqosSetLocalOptions(void* handler,int video_ssrc,int audio_ssrc,int options);
	int libqosSetRemoteOptions(void* handler,int video_clock,int audio_clock,int options);
	int libqosFeedRTCP(void* qos,unsigned char* buf,int size);
	int libqosFeedRTP(void* qos,unsigned char* buf,int size);
	libqos_rtpkt_t* libqosGetVideoRTP(void* qos);
	libqos_rtpkt_t* libqosGetAudioRTP(void* qos);
	int libqosFreeRTP(void* qos,libqos_rtpkt_t* rtp);
	int libqosDestroy(void* qos);


	void* libqosCreateSemaphore(char* name,int level);
	int libqosWaitSemaphore(void* sem);
	int libqosPostSemaphore(void* sem);
	int libqosDestroySemaphore(void* sem);


	int libqosGetTickCount();
	int libqosCreateThread(int prior,void* (*route)(void* args),void* args,int stack);
	int libqosJoinThread(int tid);

#ifdef __cplusplus
}
#endif

#endif
