#ifndef __SDK_H__
#define __SDK_H__

#include "mi_vdec.h"
#include "mi_vdec_datatype.h"
#include "mi_common.h"
#include "mi_common_datatype.h"
#include "mi_sys.h"
#include "mi_sys_datatype.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"
#include "mi_ai.h"
#include "mi_ao.h"

#define CFG_COLOR_BPP		32
#define	COLOR_TRANSPARENT	0x0000

#define COLOR_BLACK		0xff101010
#define COLOR_RED		0xffff0000
#define COLOR_GREEN		0xff00ff00
#define COLOR_BLUE		0xff0000ff
#define COLOR_YELLOW		0xffffff00
#define COLOR_WHITE		0xffffffff
#define COLOR_GRAY		0xff808080
#define COLOR_CANON		0xffaccfff


#define CFG_HW_VER		"2.1.0.0"
#define CFG_BOOT_VER		"1.0.3.0"
#define CFG_KERNEL_VER		"3.18.20.5"
#define CFG_APP_VER		"20.21.12.08"
#if CFG_AUDIO_CODEC == CODEC_OPUS
	#define CFG_AUDIO_FSIZE		960
#else
	#define CFG_AUDIO_FSIZE		160
#endif
#define CFG_AUDIO_SIZE		(CFG_AUDIO_FSIZE * 8)

#define CFG_AUDIO_PTSDELTA8K	(128.0f * (CFG_AUDIO_PT / 1024.0f))
#define CFG_AUDIO_PTSDELTA16K   (CFG_AUDIO_PTSDELTA8K / 2)
#define CFG_AUDIO_PTSDELTA32K   (CFG_AUDIO_PTSDELTA8K / 4)
#define CFG_AUDIO_PTSDELTA48K   (CFG_AUDIO_PTSDELTA8K / 6)


#define MISC_HDR_SPS		0
#define MISC_HDR_PPS		1
#define MISC_HDR_VPS		2

typedef struct
{
        int x;
        int y;
        int w;
        int h;
        int vpss;
        int chn;

        int attr;
}mci_t;


#ifdef __cplusplus
extern "C"
{
#endif
	int liveInit();
	int liveUninit();
	void* liveCreate(char* filename,void* prv,int* urp,char*username,char*realm,char*nonce,char*uri,char*response,double* duration,unsigned int* video_codec,unsigned int* video_width,unsigned int* video_height,unsigned int* video_fps,unsigned int* video_kbps,unsigned int* audio_codec,unsigned int* audio_samplerate,unsigned int* audio_channels,unsigned int* audio_kbps);
	int liveGetHeaderPacket(void* hdl,int data_type,char** buf);
	int liveGetAudioPacket(void* hdl,int idx,char** buf,double* pts);
	int liveGetVideoPacket(void* hdl,char** buf,double* pts,int* sync);
	int liveReset(void* hdl,double speed);
	int liveDestroy(void* hdl);

	int sdkInit();
	int sdkUninit();
	int sdkSetTime(int rtc);
	int sdkGetVolume(int in,int* vol);
	int sdkSetVolume(int in,int vol);
	int sdkGetHWSR();
	int sdkGetHWDelta();
	int sdkSetVO(int idx,int force,mci_t* mci);


	int sdkSendH264Stream(char* buf,int size,int pts,int sync);
	int sdkSendPCMStream(char* buf,int size,int pts);

	void* echoCreate(int sr,int pt,int length,int agcvol);
        int echoDestroy(void* hdl);
        int echoProcess(void* hdl,short int* ai,short int* ao,short int* out);
        int echoGetFrameSize(void* hdl);


#ifdef __cplusplus
}
#endif




#endif
