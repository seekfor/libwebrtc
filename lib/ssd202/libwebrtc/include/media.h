
#ifndef _MEDIA_H_
#define _MEDIA_H_

typedef enum
{
	SENDRECV,
	RECVONLY,
	SENDONLY,
}MediaDirection;


typedef struct
{
	MediaDirection audio;
	MediaDirection video;
}MediaAttr;


#define CODEC_H264	96
#define CODEC_H265	97
#define CODEC_VP8	98
#define CODEC_VP9	99
#define CODEC_OPUS	128
#define CODEC_PCMA	129
#define CODEC_PCMU	130
#define CODEC_AAC	131
#define CODEC_LPCM	132
#define CODEC_NONE	0

#define PACK_RTP	0
#define PACK_RED	1
#define PACK_ULPFEC	2


typedef int MediaCodec;

typedef struct
{
	MediaCodec audio;
	MediaCodec video;
	int numtracks;
}MediaStream;

#ifdef __cplusplus
extern "C"
{
#endif

	MediaStream* mediaCreate();
	void mediaAddTrack(MediaStream* media, MediaCodec codec);
	void mediaDestroy(MediaStream* media);

#ifdef __cplusplus
}
#endif

#endif
