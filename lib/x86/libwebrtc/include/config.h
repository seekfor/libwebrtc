#ifndef __CONFIG_H__
#define __CONFIG_H__


#define CFG_NICE_BUG_FIXED

#define CFG_VIDEO_CODEC				CODEC_H264
#define CFG_VIDEO_PT				96
#define CFG_VIDEO_SSRC				0x11111111
#define CFG_VIDEO_PACK				PACK_RTP

#define CFG_AUDIO_CODEC				CODEC_OPUS
#if CFG_AUDIO_CODEC == CODEC_OPUS
	#define CFG_AUDIO_PT			111
#elif CFG_AUDIO_CODEC == CODEC_PCMA
	#define CFG_AUDIO_PT			8
#elif CFG_AUDIO_CODEC == CODEC_PCMU
	#define CFG_AUDIO_PT			0
#else
	#error Unsuport Audio Codec!
#endif
#define CFG_AUDIO_SSRC				0x22222222
#define CFG_RED_PT				121
#define CFG_AUDIO_PACK				PACK_RTP

#define CFG_MAX_VIDEO_SIZE			(256 * 1024)
#define CFG_MAX_AUDIO_SIZE			(8 * 1024)
#define CFG_MAX_FRAME				8
#define CFG_MAX_FREE_QUEUE			32

#endif

