#ifndef __JANUS_H__
#define __JANUS_H__

#include "glib.h"
#include "agent.h"
#include "cJSON.h"

#include "peer.h"
#include "qos.h"

#define CFG_MAX_JANUS_PEER			6

#define JANUS_STATE_INIT			0x00
#define JANUS_STATE_GOTOFFER			0x01
#define JANUS_STATE_GOTANSWER			0x04
#define JANUS_STATE_CONNECTED			0x08
#define JANUS_STATE_QUIT			0xff


#define JANUS_EVENT_ONSTATE			0
#define JANUS_EVENT_WEBRTCUP			1
#define JANUS_EVENT_HANGUP			2
#define JANUS_EVENT_DESTROY			3

#define JANUS_EVENT_PLIREQ			4
#define JANUS_EVENT_FIRREQ			5
#define JANUS_EVENT_REMBREQ			6


#define JANUS_PLUGIN_ECHO			0
#define JANUS_PLUGIN_STREAMMING			1
#define JANUS_PLUGIN_VIDEOCALL			2
#define JANUS_PLUGIN_SIP			3
#define JANUS_PLUGIN_VIDEOROOM			4
#define JANUS_PLUGIN_MVIDEOROOM			5
#define JANUS_PLUGIN_AUDIOBRIDGE		6
#define JANUS_PLUGIN_TEXTROOM			7
#define JANUS_PLUGIN_VOICEMAIL			8
#define JANUS_PLUGIN_VOD			9

typedef struct
{
	int codec;
	int sr;
	int stereo;
	int pts;
	int sync;
	int size;
	int flags;
	char* buf;
}janus_frame_t;


typedef size_t (*janus_event_callback)(int event,void* msg,size_t size,void* args);

#ifdef __cplusplus
extern "C"
{
#endif
	/*janus.c*/
	void msleep(int ms);
	int httpGET(void* curl,char* url,char* response);
	int httpPOST(char* url,char* content,int size,char* response);

	janus_frame_t* janusVideoroomGetVideoFrame(void* client,int idx);
	janus_frame_t* janusVideoroomGetAudioFrame(void* client,int idx);
	int janusVideoroomSendVideoFrame(void* client,int idx,janus_frame_t* frame);
	int janusVideoroomSendAudioFrame(void* client,int idx,janus_frame_t* frame);

	/*libvideoroom.c*/
	void* janusVideoroomJoin(char* url,char* display,int room,janus_event_callback cb);
	libqos_rtpkt_t* janusVideoroomGetVideoPacket(void* client,int idx);
	libqos_rtpkt_t* janusVideoroomGetAudioPacket(void* client,int idx);
	int janusVideoroomGetBitrate(void* client,int idx);
	int janusVideoroomFreePacket(void* client,libqos_rtpkt_t* rtp);
	int janusVideoroomSendPacket(void* client,char*buf,int size,int timestamp,int flag);
	int janusVideoroomLeave(void* janus);


#ifdef __cplusplus
}
#endif

#endif

