#include <common.h>

//#define CFG_ST7701_480X480
#ifdef CFG_ST7701_480X480
	#define LCD_WIDTH	480
	#define LCD_HEIGHT	480
	#include "st7701_480x480.h"
#else
	#define LCD_WIDTH	1024
	#define LCD_HEIGHT	600
	#include "SAT070CP50_1024x600.h"
#endif

//#define CFG_ECHO_SUPPORT

static void* mutex = NULL;
static int quit = 0;

#define SNDPCM_IDX_LOCAL		0
#define SNDPCM_IDX_REMOTE		1

typedef struct
{
        short int left[CFG_AUDIO_SIZE];
        short int right[CFG_AUDIO_SIZE];
        int pts[128];
        int wr;
}pcm_pool_t;


static int ainthd;
#ifndef CFG_ECHO_SUPPORT
	static int aouthd;
#endif

static pcm_pool_t pcmpool[2];

static int pcmpool_putbuf(int idx,int pts,short int* l,short int* r,int pt)
{
        int wridx;
        int wr;
        pcm_pool_t* pool = &pcmpool[idx];
        wr = pool->wr;
        wridx = wr / CFG_AUDIO_FSIZE;

        if(l)
        {
                memcpy(&pool->left[wr],l,pt * 2);
        }
        if(r)
        {
                memcpy(&pool->right[wr],r,pt * 2);
        }
        else if(l)
        {
                memcpy(&pool->right[wr],l,pt * 2);
        }
        pool->pts[wridx] = pts;
        pool->wr = (wr + pt) % CFG_AUDIO_SIZE;
        return pool->wr;
}

static int pcmpool_getbuf(int idx,int* rd,int* pts,short int* l,short int* r,int pt,unsigned int* skip,int peek)
{
        unsigned int rb = 1;
        int len;
        int t = *rd;
        int rdidx;
        pcm_pool_t* pool = &pcmpool[idx];

        if(t < 0)
        {
                *rd = t = pool->wr;
        }
        if(skip)
        {
                rb = *skip;
        }

        rdidx = t / CFG_AUDIO_FSIZE;

        if(t <= pool->wr)
        {
                len = pool->wr - t;
                if(len < (pt * rb))
                {
                        if(skip && (rb == 1))
                        {
                                *skip = 4;
                        }
                        return 0;
                }
                if(l)
                {
                        memcpy(l,&pool->left[t],pt * 2);
                }
                if(r)
                {
                        memcpy(r,&pool->right[t],pt * 2);
                }

        }
        else
        {
                len = CFG_AUDIO_SIZE - t + pool->wr;
                if(len < (pt * rb))
                {
                        if(skip && (rb == 1))
                        {
                                *skip = 4;
                        }
                        return 0;
                }
                len = CFG_AUDIO_SIZE - t;
                if(len >= pt)
                {
                        if(l)
                        {
                                memcpy(l,&pool->left[t],pt * 2);
                        }
                        if(r)
                        {
                                memcpy(r,&pool->right[t],pt * 2);
                        }
                }
                else
                {
                        if(l)
                        {
                                memcpy(l,&pool->left[t],len * 2);
                                memcpy(l + len,pool->left,(pt - len) * 2);
                        }
                        if(r)
                        {
                                memcpy(r,&pool->right[t],len * 2);
                                memcpy(r + len,pool->right,(pt - len) * 2);
                        }
                }
        }
        if(pts)
        {
                *pts = pool->pts[rdidx] + (t - rdidx * CFG_AUDIO_FSIZE) * sdkGetHWDelta() / 1000.0f;
        }
        if(peek)
        {
                *rd = (t + pt) % CFG_AUDIO_SIZE;
        }
        if(skip)
        {
                *skip = 1;
        }
        return pt;
}

int sdkSetVO(int idx,int force,mci_t* mci)
{
	int i;
	int chn;
        MI_DISP_InputPortAttr_t stInputPortAttr;
	MI_VDEC_OutputPortAttr_t stOutputPortAttr;
	if(idx != 0)
	{
		return -1;
	}
	for(i = 0; i < 8; i ++,mci ++)
	{
		if(!mci->w || !mci->h)
		{
			break;
		}
		chn = mci->vpss;
		memset(&stOutputPortAttr, 0, sizeof(MI_VDEC_OutputPortAttr_t));
		stOutputPortAttr.u16Width  = mci->w;
		stOutputPortAttr.u16Height = mci->h;
		MI_VDEC_SetOutputPortAttr(chn, &stOutputPortAttr);
	
		memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));
		stInputPortAttr.u16SrcWidth = mci->w;
		stInputPortAttr.u16SrcHeight = mci->h;
		stInputPortAttr.stDispWin.u16X = mci->x;
		stInputPortAttr.stDispWin.u16Y = mci->y;
		stInputPortAttr.stDispWin.u16Width = mci->w;
		stInputPortAttr.stDispWin.u16Height = mci->h;
		MI_DISP_SetInputPortAttr(0,i,&stInputPortAttr);
		
	}
	return 0;
}

int vdecInit()
{
	MI_VDEC_ChnAttr_t stVdecChnAttr;
	MI_VDEC_OutputPortAttr_t stOutputPortAttr;
        MI_SYS_ChnPort_t  stChnPort;
        MI_DISP_PubAttr_t stDispPubAttr;
        MI_DISP_InputPortAttr_t stInputPortAttr;
        MI_SYS_ChnPort_t stSrcChnPort;
	MI_SYS_ChnPort_t stDstChnPort;

	MI_SYS_Init();
		
	memset(&stVdecChnAttr,0,sizeof(stVdecChnAttr));
	stVdecChnAttr.eCodecType   = E_MI_VDEC_CODEC_TYPE_H264;
	stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 3;
	stVdecChnAttr.eVideoMode   = E_MI_VDEC_VIDEO_MODE_STREAM;
	stVdecChnAttr.u32BufSize   = 1920 * 1080;
	stVdecChnAttr.u32PicWidth  = 1920;
	stVdecChnAttr.u32PicHeight = 1080;
	stVdecChnAttr.eDpbBufMode  = E_MI_VDEC_DPB_MODE_INPLACE_ONE_BUF;
	stVdecChnAttr.u32Priority  = 255;

	MI_VDEC_CreateChn(0, &stVdecChnAttr);
	MI_VDEC_StartChn(0);

	memset(&stOutputPortAttr,0,sizeof(MI_VDEC_OutputPortAttr_t));
	stOutputPortAttr.u16Width  = LCD_WIDTH;
	stOutputPortAttr.u16Height = LCD_HEIGHT;
	MI_VDEC_SetOutputPortAttr(0,&stOutputPortAttr);

	memset(&stChnPort,0,sizeof(MI_SYS_ChnPort_t));
	stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
	stChnPort.u32DevId  = 0;
	stChnPort.u32ChnId  = 0;
	stChnPort.u32PortId = 0;
	MI_SYS_SetChnOutputPortDepth(&stChnPort,0,3);


        stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
	stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
	stDispPubAttr.u32BgColor = 0x10801080;

        stDispPubAttr.stSyncInfo.u16Vact = stPanelParam.u16Height;
        stDispPubAttr.stSyncInfo.u16Vbb = stPanelParam.u16VSyncBackPorch;
        stDispPubAttr.stSyncInfo.u16Vfb = stPanelParam.u16VTotal - (stPanelParam.u16VSyncWidth + stPanelParam.u16Height + stPanelParam.u16VSyncBackPorch);
        stDispPubAttr.stSyncInfo.u16Hact = stPanelParam.u16Width;
        stDispPubAttr.stSyncInfo.u16Hbb = stPanelParam.u16HSyncBackPorch;
        stDispPubAttr.stSyncInfo.u16Hfb = stPanelParam.u16HTotal - (stPanelParam.u16HSyncWidth + stPanelParam.u16Width + stPanelParam.u16HSyncBackPorch);
        stDispPubAttr.stSyncInfo.u16Bvact = 0;
        stDispPubAttr.stSyncInfo.u16Bvbb = 0;
        stDispPubAttr.stSyncInfo.u16Bvfb = 0;
        stDispPubAttr.stSyncInfo.u16Hpw = stPanelParam.u16HSyncWidth;
        stDispPubAttr.stSyncInfo.u16Vpw = stPanelParam.u16VSyncWidth;
        stDispPubAttr.stSyncInfo.u32FrameRate = stPanelParam.u16DCLK * 1000000 / (stPanelParam.u16HTotal * stPanelParam.u16VTotal);
        stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;
        stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;


        MI_DISP_SetPubAttr(0,&stDispPubAttr);
        MI_DISP_Enable(0);
        MI_DISP_BindVideoLayer(0,0);
	MI_DISP_EnableVideoLayer(0);


	memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));
	stInputPortAttr.u16SrcWidth = LCD_WIDTH;
	stInputPortAttr.u16SrcHeight = LCD_HEIGHT;
	stInputPortAttr.stDispWin.u16X = 0;
	stInputPortAttr.stDispWin.u16Y = 0;
	stInputPortAttr.stDispWin.u16Width = LCD_WIDTH;
	stInputPortAttr.stDispWin.u16Height = LCD_HEIGHT;
	MI_DISP_SetInputPortAttr(0,0,&stInputPortAttr);
	MI_DISP_EnableInputPort(0,0);
	MI_DISP_SetInputPortSyncMode(0,0,E_MI_DISP_SYNC_MODE_FREE_RUN);

       	MI_PANEL_Init(E_MI_PNL_LINK_TTL);
       	MI_PANEL_SetPanelParam(&stPanelParam);

	stSrcChnPort.eModId = E_MI_MODULE_ID_VDEC;
	stSrcChnPort.u32DevId = 0;
	stSrcChnPort.u32ChnId = 0;
	stSrcChnPort.u32PortId = 0;

	stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
	stDstChnPort.u32DevId = 0;
	stDstChnPort.u32ChnId = 0;
	stDstChnPort.u32PortId = 0;
	MI_SYS_BindChnPort(&stSrcChnPort,&stDstChnPort,30,30);
        return 0;
}

#ifndef CFG_ECHO_SUPPORT
static void* live_aout_thread(void* args)
{
	int rd = -1;
	unsigned int skip = 4;
	int pts;
	short  int l[CFG_AUDIO_FSIZE];
	short int r[CFG_AUDIO_FSIZE];
	MI_AUDIO_Frame_t stAoChFrame;
	memset(&stAoChFrame,0,sizeof(stAoChFrame));
	stAoChFrame.u32Len = CFG_AUDIO_FSIZE * 2;
	stAoChFrame.apVirAddr[0] = l;
	stAoChFrame.apVirAddr[1] = r;
	while(!quit)
	{
		msleep(10);
		memset(l,0,sizeof(l));
		memset(r,0,sizeof(r));
		if(pcmpool_getbuf(SNDPCM_IDX_REMOTE,&rd,&pts,l,r,CFG_AUDIO_FSIZE,&skip,1) > 0)
		{
			MI_AO_SendFrame(0,0,&stAoChFrame,-1);
		}
	}
	return args;
}
#endif


#if 0
static void* live_aenc_thread(void* args)
{
	int rd = -1;
	int pts;
	unsigned int skip = 4;
	char pcma[1024];
	short int l[160];
	short int r[160];
	int size;
	int sr = 8000;

	while(!quit)
	{
		msleep(10);
		if(0 == pcmpool_getbuf(SNDPCM_IDX_LOCAL,&rd,&pts,l,r,160,&skip,1))
		{
			continue;
		}
		size = AlawEncode(l,160,pcma);
		livePutAudioStream(pcma,size,pts);
	}
	return args;
}
#endif

static void* live_ain_thread(void* args)
{
	int ret;
	int dev = (int)args;
	MI_AUDIO_Frame_t stAiChFrame;
	short int stereo[CFG_AUDIO_FSIZE * 2];
	unsigned char adts[4096];

	short int l[CFG_AUDIO_FSIZE];
	short int r[CFG_AUDIO_FSIZE];
	int size;
	int i;
#ifdef CFG_ECHO_SUPPORT
	int sr;
        int rd = -1;
        unsigned int skip = 4;
        int pts;
        short int mic[CFG_AUDIO_FSIZE];
        short int ref[CFG_AUDIO_FSIZE];
        short int out[CFG_AUDIO_FSIZE];
        void* echo;
        MI_AUDIO_Frame_t stAoChFrame;
	memset(&stAoChFrame,0,sizeof(stAoChFrame));
        stAoChFrame.u32Len = CFG_AUDIO_FSIZE * 2;
        stAoChFrame.apVirAddr[0] = ref;
        stAoChFrame.apVirAddr[1] = ref;
#if CFG_AUDIO_CODEC == CODEC_OPUS
	sr = 48000;
#else
	sr = 8000;
#endif
        memset(mic,0,sizeof(mic));
        memset(ref,0,sizeof(ref));
        echo = echoCreate(sr,CFG_AUDIO_FSIZE,100,2);
#endif

	while(!quit)
	{
#ifdef CFG_ECHO_SUPPORT
		memset(ref,0,sizeof(ref));
		pcmpool_getbuf(SNDPCM_IDX_REMOTE,&rd,&pts,ref,NULL,CFG_AUDIO_FSIZE,&skip,1);
		for(i = 0; i < CFG_AUDIO_FSIZE; i ++)
		{
			ref[i] = ref[i] / 2;
		}
		MI_AO_SendFrame(dev,0,&stAoChFrame,-1);
#endif		
		ret = MI_AI_GetFrame(dev,0,&stAiChFrame,NULL, -1);
		if(ret == 0)
		{
			int pt = CFG_AUDIO_FSIZE;
			memcpy(l,stAiChFrame.apVirAddr[0],CFG_AUDIO_FSIZE * 2);
			memcpy(r,stAiChFrame.apVirAddr[0],CFG_AUDIO_FSIZE * 2);
#ifdef CFG_ECHO_SUPPORT
			if(aa->aec)
			{
				memcpy(mic,l,CFG_AUDIO_FSIZE * 2);
				pt = echoProcess(echo,mic,ref,out);
				if(pt)
				{
                        		memcpy(l,out,CFG_AUDIO_FSIZE * 2);
                        		memcpy(r,out,CFG_AUDIO_FSIZE * 2);
				}
			}
#endif
			if(pt)
			{
				pcmpool_putbuf(SNDPCM_IDX_LOCAL,stAiChFrame.u64TimeStamp / 1000ll,l,r,CFG_AUDIO_FSIZE);
			}
			MI_AI_ReleaseFrame(dev, 0, &stAiChFrame,NULL);

    		}
	}
#ifdef CFG_ECHO_SUPPORT
	echoDestroy(echo);
#endif	
	return args;
}

static int procfile_write(char* pt,char* value)
{
	int fd = open(pt,O_WRONLY);
	if(fd < 0)
	{
		return -1;
	}
	write(fd,value,strlen(value));
	close(fd);
	return 0;
	
}

int aoInit()
{
	MI_S32 vol;
	MI_S32 ret;
	MI_AUDIO_Attr_t stAttr;
	MI_AUDIO_Attr_t stGetAttr;
	MI_AUDIO_DEV AoDevId = 0;
	MI_AO_CHN AoChn = 0;
	MI_SYS_ChnPort_t stAoChnOutputPort;
	
	procfile_write("/sys/class/gpio/export","12");
	procfile_write("/sys/class/gpio/gpio12/direction","low");

	stAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
#if CFG_AUDIO_CODEC == CODEC_OPUS
	stAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_48000;
#else
	stAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_8000;
#endif
	stAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
	stAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
	stAttr.u32PtNumPerFrm = CFG_AUDIO_FSIZE;
	stAttr.u32ChnCnt = 1;

	ret = MI_AO_SetPubAttr(AoDevId,&stAttr);
	if(MI_SUCCESS != ret)
	{
		return ret;
	}

	ret = MI_AO_GetPubAttr(AoDevId,&stGetAttr);

	if(MI_SUCCESS != ret)
	{
		return ret;
	}
	
	ret = MI_AO_Enable(AoDevId);
	if(MI_SUCCESS != ret)
	{
		return ret;
	}

	ret = MI_AO_EnableChn(AoDevId, AoChn);


	stAoChnOutputPort.eModId = E_MI_MODULE_ID_AO;
	stAoChnOutputPort.u32DevId = AoDevId;
	stAoChnOutputPort.u32ChnId = AoChn;
	stAoChnOutputPort.u32PortId = 0;
	MI_SYS_SetChnOutputPortDepth(&stAoChnOutputPort,1,8);

	if(MI_SUCCESS != ret)
	{
		return ret;
	}
	vol = vol = -60 + 100 * 90 / 100; 
	MI_AO_SetVolume(AoDevId,vol);
	return 0;
}

int aiInit()
{
	int vol;
	MI_S32 ret;
	MI_AUDIO_Attr_t stAttr;
	MI_AUDIO_DEV AiDevId = 0;
	MI_AI_CHN AiChn = 0;
	MI_SYS_ChnPort_t stChnPort;
	MI_S32 s32Fd;
	fd_set readFdSet;
	MI_SYS_ChnPort_t stAiChnOutputPort;

	stAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
#if CFG_AUDIO_CODEC == CODEC_OPUS
	stAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_48000;
#else
	stAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_8000;
#endif
	stAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
	stAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_SLAVE;
	stAttr.u32PtNumPerFrm = CFG_AUDIO_FSIZE;
	stAttr.u32ChnCnt = 1;
	ret = MI_AI_SetPubAttr(AiDevId, &stAttr);
	if (MI_SUCCESS != ret)
	{
    		return ret;
	}

	ret = MI_AI_GetPubAttr(AiDevId, &stAttr);
	if (MI_SUCCESS != ret)
	{
		return ret;
	}
	ret = MI_AI_Enable(AiDevId);
	if (MI_SUCCESS != ret)
	{
		return ret;
	}
	ret = MI_AI_EnableChn(AiDevId, AiChn);

	if (MI_SUCCESS != ret)
	{
		return ret;
	}

	vol = 21;
	MI_AI_SetVqeVolume(AiDevId,AiChn,vol);

	stAiChnOutputPort.eModId = E_MI_MODULE_ID_AI;
	stAiChnOutputPort.u32DevId = AiDevId;
	stAiChnOutputPort.u32ChnId = AiChn;
	stAiChnOutputPort.u32PortId = 0;
	MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort, 1, 8);
	return 0;
}

int sdkSendH264Stream(char* buf,int size,int pts,int sync)
{
	MI_VDEC_VideoStream_t stVdecStream;

	stVdecStream.pu8Addr = buf;
	stVdecStream.u32Len = size;
	stVdecStream.u64PTS = 0;
	stVdecStream.bEndOfFrame  = 1;
	stVdecStream.bEndOfStream = 0;
	return MI_VDEC_SendStream(0,&stVdecStream,-1);
}

int sdkSendPCMStream(char* buf,int size,int pts)
{
	pcmpool_putbuf(SNDPCM_IDX_REMOTE,pts,(short int*)buf,NULL,size / 2);
	return 0;
}


int liveInit()
{
	vdecInit();
	aiInit();
	aoInit();
	ainthd = libqosCreateThread(100,live_ain_thread,NULL,128000);
#ifndef CFG_ECHO_SUPPORT
	aouthd = libqosCreateThread(100,live_aout_thread,NULL,128000);
#endif
	return 0;
}


int liveUninit()
{
	quit = 1;
	libqosJoinThread(ainthd);
#ifndef CFG_ECHO_SUPPORT
	libqosJoinThread(aouthd);
#endif
	return 0;
}

int sdkInit()
{
	mutex = libqosCreateSemaphore("SDK-MTX-VENC",1);
	liveInit();
	return 0;
}

int sdkUninit()
{
	liveUninit();
	return 0;
}

int sdkGetHWSR()
{
#if CFG_AUDIO_CODEC == CODEC_OPUS
	return 48000;
#else
	return 8000;
#endif
}

int sdkGetHWDelta()
{
#if CFG_AUDIO_CODEC == CODEC_OPUS
	return CFG_AUDIO_PTSDELTA48K;
#else
	return CFG_AUDIO_PTSDELTA8K;
#endif
}

int sdkGetVolume(int in,int* vol)
{
	return 0;
}









