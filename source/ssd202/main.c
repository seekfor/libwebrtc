
#include <common.h>

#define ES_MAX_SIZE 256 * 1024
#define FILE_NAME "/mnt/nfs/media/smith4.mp4"

static volatile int quit = 0,run = 0;
static GThread* txthd = NULL,*rxthd = NULL;
static char* filename = FILE_NAME;

static void* main_recv_av_thread(void* data)
{
	janus_frame_t* frame;
	short int pcm[4096];
	short int* src;
	int i;
	while(run)
	{
		msleep(1);
		frame = janusVideoroomGetVideoFrame(data,1);
		if(frame)
		{
			sdkSendH264Stream(frame->buf,frame->size,frame->pts,0);
		}
		frame = janusVideoroomGetAudioFrame(data,1);
		if(frame)
		{
			if(frame->stereo == 0)
			{
				sdkSendPCMStream(frame->buf,frame->size,frame->pts);
			}
			else
			{
				src = (short int*)frame->buf;
				for(i = 0; i < frame->size / 4; i ++,src += 2)
				{
					pcm[i] = *src;
				}
				sdkSendPCMStream((char*)pcm,frame->size / 2,frame->pts);
			}
		}
	}
	return data;
}

static void* main_send_av_thread(void *data) 
{
	unsigned long timestamp = 0;
	void* mp4dmx;
	unsigned int vid_codec,aud_codec,sample_rate,channels,size,sync;
	double duration,pts;
	char es[ES_MAX_SIZE];
	char sps[1024];
	char pps[1024];
#if CFG_AUDIO_CODEC == CODEC_OPUS
	short int pcma[960] = {0};
#else
	short int pcma[160] = {0};
#endif
	char nal;
	int spsize = 0,ppsize = 0;
	int i;
	janus_frame_t frm;
	mp4dmx = mp4dmxCreate(filename,&duration,&vid_codec,&aud_codec,&sample_rate,&channels);

	while(run) 
	{
		size = mp4dmxGetVideoPacket(mp4dmx,es,&pts,&vid_codec,&sync);
		if(size <= 0)
		{
			mp4dmxReset(mp4dmx,0);
		}
		nal = es[4] & 0x1f;
		if(nal == 0x07) 
		{
			memcpy(sps,es,size);
			spsize = size;
			continue;
		}
		else if(nal == 0x08) 
    		{
			memcpy(pps,es,size);
			ppsize = size;
      			continue;
		}
    		else if(nal == 0x05) 
    		{
			frm.codec = CODEC_H264;
			frm.pts = timestamp;
			frm.size = spsize;
			frm.buf = sps;
			frm.sync = 1;
			janusVideoroomSendVideoFrame(data,0,&frm);
			frm.size = ppsize;
			frm.buf = pps;
			janusVideoroomSendVideoFrame(data,0,&frm);
    		}
		frm.codec = CODEC_H264;
		frm.size = size;
		frm.buf = es;
		frm.sync = sync;
		frm.pts = timestamp;
		janusVideoroomSendVideoFrame(data,0,&frm);
   		timestamp += 3600;
 
		for(i = 0; i < 2; i ++)
		{
			frm.codec = CODEC_LPCM;
			frm.size = sizeof(pcma);
			frm.buf = (char*)pcma;
			frm.sync = 1;
			janusVideoroomSendAudioFrame(data,0,&frm);
		}
		
		msleep(40);
	}
	mp4dmxDestroy(mp4dmx);
	return NULL;
}


static size_t libjanus_callback(int event,void* msg,size_t size,void* args)
{
	switch(event)
	{
		case JANUS_EVENT_HANGUP:
			break;
		case JANUS_EVENT_WEBRTCUP:
			break;
		case JANUS_EVENT_ONSTATE:
			break;
		case JANUS_EVENT_PLIREQ:
			break;
		case JANUS_EVENT_FIRREQ:
			break;
		case JANUS_EVENT_REMBREQ:
			break;
		default:
			break;
	}
	return 0;
}

int main(int argc, char *argv[]) 
{
	void* videoroom = NULL;
	if(argc > 2)
	{
		filename = argv[2];
	}
	ffmpegInit();
	sdkInit();
	webrtcInit(NULL,NULL);
	printf("Press 'L' or 'l' to join video room 1234,\r\nPress 'D' or 'd' to exit video room,\r\nand Press 'Q' or 'q' to quit!\r\n");

	while(!quit)
	{
		switch(getchar())
		{
			case 'q':
			case 'Q':
				if(videoroom)
				{
					run = 0;
					g_thread_join(rxthd);
					g_thread_join(txthd);
					janusVideoroomLeave(videoroom);
					videoroom = NULL;
				}
				quit = 1;
				break;
			case 'L':
			case 'l':
				if(videoroom)
				{
					break;
				}
				videoroom = janusVideoroomJoin("https://192.156.1.34:8089",(argc > 1)?argv[1]:"demo",1234,NULL);
				if(!videoroom)
				{
					break;
				}
				run = 1;
				txthd = g_thread_new("main_send_av_thread",main_send_av_thread,videoroom);
				rxthd = g_thread_new("main_recv_av_thread",main_recv_av_thread,videoroom);
				break;
			case 'D':
			case 'd':
				if(!videoroom)
				{
					break;
				}
				run = 0;
				g_thread_join(rxthd);
				g_thread_join(txthd);
				janusVideoroomLeave(videoroom);
				videoroom = NULL;
				break;
		}
		getchar();
	}
	webrtcUninit();
	sdkUninit();
	return 0;
}
