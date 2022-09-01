#ifndef _PEER_H_
#define _PEER_H_

typedef enum
{
	DISCONNECTED,
	GATHERING,
	CONNECTING,
	CONNECTED,
	READY,
	FAILED,
}IceConnectionState;

typedef struct
{
	void* qos;
	NiceAgent *nice;
	gboolean controlling;
	guint stream_id;
	guint component_id;
	GMainLoop *gloop;
	GThread *gthread;
	gboolean mdns_enabled;
	uint32_t audio_ssrc, video_ssrc;
	int video_codec;
	int audio_codec;

	DTLS *dtls;
	SDP *sdp;
	MediaAttr media_attr;
	MediaStream *media_stream;
	RtpMap rtp_map;

	void (*oncandidate)(void* userdata,char *sdp);
	void (*onstatechange)(void* userdata,IceConnectionState state);
	void (*onrtp)(void* userdata,uint8_t *packet,size_t bytes);
	void (*onrtcp)(void* userdata,uint8_t *packet, size_t bytes);
	void (*onconnected)(void* userdata);
	int  (*onqos)(void* userdata,int msg,char* buf,int size);

	void *oncandidate_userdata;
	void *onstatechange_userdata;
	void *onrtp_userdata;
	void *onrtcp_userdata;
	void *onconnected_userdata;
	void *onqos_userdata;

}WEBRTC;


#ifdef __cplusplus
extern "C"
{
#endif
	int webrtcInit(char* stun,char* turn);
	int webrtcUninit();

	WEBRTC* webrtcCreate();
	void webrtcDestroy(WEBRTC *pc);
	void webrtcAddStream(WEBRTC *pc,MediaStream *stream);
	void webrtcEnableMDNS(WEBRTC* pc,gboolean b_enabled);
	void webrtcSetRemoteDescription(WEBRTC* pc,char* sdp);
	int webrtcAddMediaAttr(WEBRTC* pc,MediaAttr attr);
	int webrtcStart(WEBRTC* pc);

	void webrtcOnCandidate(WEBRTC* pc,void (*onicecandidate),void* userdata);
	void webrtcOnStateChange(WEBRTC* pc,void (*oniceconnectionstatechange),void* userdata);
	void webrtcOnRTP(WEBRTC* pc,void (*onrtp),void* userdata);
	void webrtcOnRTCP(WEBRTC* pc,void (*onrtcp),void* userdata);
	void webrtcOnQOS(WEBRTC* pc,int (*onqos)(void* userdata,int msg,char* buf,int size),void* userdata);
	void webrtcOnConnected(WEBRTC* pc,void (*onconnected), void* userdata);

	uint32_t webrtcGetSSRC(WEBRTC* pc,char* type);
	int webrtcGetRTPMAP(WEBRTC* pc,MediaCodec codec);

	int webrtcFeedQOS(WEBRTC* pc,uint8_t* buf,int bytes);
	libqos_rtpkt_t* webrtcGetVideoPacket(WEBRTC* pc);
	libqos_rtpkt_t* webrtcGetAudioPacket(WEBRTC* pc);
	int webrtcFreePacket(WEBRTC* pc,libqos_rtpkt_t* rtp);

#ifdef __cplusplus
}
#endif

#endif
