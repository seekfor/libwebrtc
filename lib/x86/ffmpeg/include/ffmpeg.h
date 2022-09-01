#ifndef __FFMPEG_H__
#define __FFMPEG_H__


#ifdef __cplusplus
extern "C"
{
#endif

	void ffmpegInit();
	int mp4muxDestroy(void* hdl);
        int mp4muxProcess(void* hdl,int codec,unsigned int pts,char* buf,int size,int intra,int duration);
        void* mp4muxCreate(const char* movfile,int bps,int fps,int image_width,int image_height,int audio_bps,int audio_sample_rate,int audio_channels);
        int mp4muxAddSPS(void* hdl,const uint8_t* buf,uint32_t size);
        int mp4muxAddPPS(void* hdl,const uint8_t* buf,uint32_t size);
        void* mp4dmxCreate(char* filename,double* duration,unsigned int* vid_codec,unsigned int* aud_codec,unsigned int* sample_rate,unsigned int* channels);
        int mp4dmxGetAudioPacket(void* hdl,char* buf,double* pts,unsigned int* codec);
        int mp4dmxGetVideoPacket(void* hdl,char* buf,double* pts,unsigned int* codec,unsigned int* sync);
        int mp4dmxSeek(void*ff,double sec,uint64_t* vid_pts,uint64_t* aud_pts,uint64_t* video_idx,uint64_t* audio_idx);
        int mp4dmxReset(void* ff,unsigned int stc);
        int mp4dmxLocateVideo(void* hdl,int idx);
        int mp4dmxLocateAudio(void* hdl,int idx);
        int mp4dmxDestroy(void* hdl);

#ifdef __cplusplus
}
#endif



#endif

