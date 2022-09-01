1.mstar(ssd202)和x86(ubuntu64-18.04)平台下的libwebrtc示范,作者:seek_for@163.com,13416435784(libwebrtc暂不开源，有需要者请联系作者)
2.janus示范程序webrtc使用方法:./webrtc <display name> <mp4 file>
3.libwebrtc函数使用指南
  <1>janus高级API(应用代码，请参考main.c)
	(1) void* janusVideoroomJoin(char* url,char* display,int room,janus_event_callback cb);
		功能:
			加入janus服务器房间
		参数:
			url:janus服务器https域名，形如"https://meetting.voip.com:8089"
			display:本端显示名称
			room:房间号
			cb:回调，原型为"size_t janus_event_callback(int event,void* msg,size_t size,void* args);
		返回:
			成功返回一个有效的void*指针，否则返回NULL	
	(2) janus_frame_t* janusVideoroomGetVideoFrame(void* client,int idx);
		功能:
			获取房间中的某一路远端的视频数据
		参数:
			client: janusVideoroomJoin()返回的指针
			idx: 0 ~ 5,通道号,0--发送端,1 ~ 5--远端1 ~ 5
		返回:
			成功返回一个有效的janus_frame_t*指针，否则返回NULL
		备注:
			该返回返回的指针无需也不能释放
	(3) janus_frame_t* janusVideoroomGetAudioFrame(void* client,int idx);
		功能:
			获取房间中的某一路远端的音频数据
		参数:
			client: janusVideoroomJoin()返回的指针
			idx: 0 ~ 5,通道号,0--发送端,1 ~ 5--远端1 ~ 5
		返回:
			成功返回一个有效的janus_frame_t*指针，否则返回NULL
		备注:
			该返回返回的指针无需也不能释放
	(4) int janusVideoroomSendVideoFrame(void* client,int idx,janus_frame_t* frame);
		功能:
			向房间发送视频数据
		参数:
			client:janusVideoroomJoin()返回的指针
			idx:未使用，必须为0
			frame:视频裸数据
		返回：
			0--成功，其它--错误代码
	(5) int janusVideoroomSendAudioFrame(void* client,int idx,janus_frame_t* frame);
		功能:
			向房间发送音频数据
		参数:
			client:janusVideoroomJoin()返回的指针
			idx:未使用，必须为0
			frame:视频裸数据
		返回：
			0--成功，其它--错误代码
	(6) libqos_rtpkt_t* janusVideoroomGetVideoPacket(void* client,int idx);
		功能:
			获取远端房间的视频RTP数据包
		参数:
			client:janusVideoroomJoin()返回的指针
			idx:0 ~ 5,0--发送端(无意义),1 ~ 5--远端1 ~ 5
		返回:
			成功返回有效的libqos_rtpkt_t*指针，否则返回NULL
		备注:
			返回的指针必须用janusVideoroomFreePacket()释放或回收
	(7) libqos_rtpkt_t* janusVideoroomGetAudioPacket(void* client,int idx);
		功能:
			获取远端房间的音频RTP数据包
		参数:
			client:janusVideoroomJoin()返回的指针
			idx:0 ~ 5,0--发送端(无意义),1 ~ 5--远端1 ~ 5
		返回:
			成功返回有效的libqos_rtpkt_t*指针，否则返回NULL
		备注:
			返回的指针必须用janusVideoroomFreePacket()释放或回收
	(8) int janusVideoroomFreePacket(void* client,libqos_rtpkt_t* rtp);
		功能:
			释放或回收RTP缓冲
		参数:
			client:janusVideoroomJoin()返回的指针
			rtp:janusVideoroomGetVideoPacket()或janusVideoroomGetAudioPacket()返回的指针
		返回:
			0--成功，其它--错误代码
	(9) int janusVideoroomSendPacket(void* client,char*buf,int size,int timestamp,int flag);
		功能:
			向房间发送RTP数据包(已废除，请使用janusVideoroomSendVideoFrame()和janusVideoroomSendAudioFrame()代替)
		参数:
			client:janusVideoroomJoin()返回的指针
			buf:RTP包指针
			size:RTP数据大小
			timestamp:时间戳
			flag:未使用
		返回:
			0--成功，其它--错误代码
	(10) int janusVideoroomLeave(void* client);
		功能:
			推出janus房间
		参数:
			client:janusVideoroomJoin()返回的指针
		返回:
			0--成功，其它--错误代码
  <2>libwebrtc底层API
	(1) int webrtcInit(char* stun,char* turn);
		功能:
			webrtc初始化
		参数:
			stun:stun服务器域名，必须形如"stun://stun.voip.com:3478"
			turn:turn服务器域名，必须形如"turn://username:password@turn.voip.com:3478"
		返回:
			0--成功，其他--失败
		备注：
			在使用任何其他webrtc函数前必须首先调用webrtcInit()
	(2) int webrtcUninit();
		功能:
			webrtc反初始化
		参数:
			无
		返回:
			总是返回0
	(3) WEBRTC* webrtcCreate();
		功能:
			建立一个webrtc实例句柄
		参数:
			无
		返回:
			成功返回句柄，否则返回NULL
        (4) void webrtcDestroy(WEBRTC *pc);
		功能:
			销毁一个webrtc实例句柄
		参数:
			pc:webrtc句柄
		返回:
			无
        (4) int webrtcAddMediaAttr(WEBRTC* pc,MediaAttr attr);
		功能:
			向一个webrtc句柄增加媒体接收/发送属性
		参数:
			pc:webrtc句柄
			attr:媒体属性
		返回:
			0--成功,其他--错误代码

	(5) void webrtcAddStream(WEBRTC *pc,MediaStream *stream);
		功能：
			向一个webrtc句柄增加媒体信息
		参数:
			pc:webrtc句柄
			stream:MediaStream指针
		返回:
			无
		备注:
			可以采用以下代码向webrtc增加媒体信息(mediaCreate()默认建立sendonly的发送媒体，如果使用接收媒体，使用webrtcAddMediaAttr()修改为recvonly)
			MediaStream* media = mediaCreate();
			mediaAddCodec(media,CODEC_H264);
			mediaAddCodec(media,CODEC_OPUS);
			webrtcAddStream(pc,media);
			if(is_recv)
			{
				MediaAttr attr = {.video = RECVONLY, .audio = RECVONLY};
				webrtcAddMediaAttr(pc,attr);
			}
	(6) void webrtcSetRemoteDescription(WEBRTC* pc,char* sdp);
		功能:
			向一个webrtc句柄配置远端SDP信息
		参数:
			pc:webrtc句柄
			sdp:jsep型的sdp信息,回车换行必须是"\r\n"4个字符，即"\\r\\n"
		返回:
			无
			
        (7) int webrtcStart(WEBRTC* pc);
		功能:
			开始启动webrtc通讯逻辑
		参数:
			pc:webrtc句柄
		返回:
			0--成功，其他--错误代码
		备注:
			在调用webrtcStart()前，必须正确配置webrtc的回调函数指针，否则可能无法正确完成通讯
	(8) void webrtcOnCandidate(WEBRTC* pc,void (*onicecandidate),void* userdata);
		功能:
			设置webrtc的on_candidate_done()回调
		参数:
			pc:webrtc句柄
			onicecandidate:回调指针，原型为"void on_candidate_done(void* args,char* sdp)",其中args是userdata,sdp为nice完成后的本地完整sdp信息
			userdata:用户数据，会传递到回调函数的args参数
		返回:
			无
	(9) void webrtcOnStateChange(WEBRTC* pc,void (*oniceconnectionstatechange),void* userdata);
		功能:
			设置webrtc的on_state_change()回调
		参数:
			pc:webrtc句柄
			oniceconnectionstatechange:ice状态改变回调指针，原型为"void on_ice_state(void* args,IceConnectionState state)"
		返回:
			无
	(10) void webrtcOnConnected(WEBRTC* pc,void (*onconnected), void* userdata);
		功能:
			设置webrtc的on_connected()回调
		参数:
			pc:webrtc句柄
			onconnected:回调指针，原型为"void on_connected(void* args)"
			userdata:用户数据指针
		返回:
			无
		备注:
			当该回调函数被调用时，内部逻辑已经完成了握手，可以开始发送数据了
	(11) void webrtcOnRTP(WEBRTC* pc,void (*onrtp),void* userdata);
		功能:
			设置webrtc的on_rtp()回调
		参数:
			pc:webrtc句柄
			onrtp:回调指针，原型为"void on_rtp(void* args,uint8_t* pkt,size_t size)"
			userdata:用户数据指针
		返回:
			无
		备注:
			当从webrtc中收到rtp数据时会自动调用该函数
	(12) void webrtcOnRTCP(WEBRTC* pc,void (*onrtcp),void* userdata);
		功能:
			设置webrtc的on_rtcp()回调
		参数:
			pc:webrtc句柄
			onrtcp:回调指针，原型为"void on_rtcp(void* args,uint8_t* pkt,size_t size)"
			userdata:用户数据指针
		返回:
			无
		备注:
			当从webrtc中收到rtcp数据时会自动调用该函数
	(13) void webrtcOnQOS(WEBRTC* pc,int (*onqos)(void* args,int msg,char* buf,int size),void* userdata);
		功能:
			设置webrtc的on_qos()回调
		参数:
			pc:webrtc句柄
			onqos:回调指针,原型为"on_qos(void* args,int msg,char* buf,int size)"
			userdata:用户数据指针
		返回:
			无
		备注:
			当webrtc内部qos组件收到PLI/FIR/REMB等请求时自动调用该函数
	(14) int webrtcFeedQOS(WEBRTC* pc,uint8_t* buf,int bytes);
		功能:
			向webrtc的qos组件发送RTP数据包
		参数:
			pc:sendonly型的webrtc句柄
			buf:rtp数据包指针
			bytes:rtp数据长度
		返回:
			0--成功，其它--错误代码
	(15) libqos_rtpkt_t* webrtcGetVideoPacket(WEBRTC* pc);
		功能:
			返回接收到的视频RTP数据缓冲
		参数:
			pc:recvonly型的webrtc句柄
		返回:
			成功返回非空指针，否则返回NULL
		备注:
			返回的指针是一个链表，遍历方法如下:
			rtp = webrtcGetVideoPacket(pc);
			while(rtp)
			{
				app_send_to_rtp_decoder(rtp->buf,rtp->size);
				rtp = rtp->child;	
			}
			该函数返回的libqos_rtpkt_t*指针，必须用webrtcFreePacket()回收，否则会造成内存泄露	
	(16) libqos_rtpkt_t* webrtcGetAudioPacket(WEBRTC* pc);
		功能:
			返回接收到的音频RTP数据缓冲
		参数:
			pc:recvonly型的webrtc句柄
		返回:
			成功返回非空指针，否则返回NULL
		备注:
			返回的指针是一个链表，遍历方法如下:
			rtp = webrtcGetAudioPacket(pc);
			while(rtp)
			{
				app_send_to_rtp_decoder(rtp->buf,rtp->size);
				rtp = rtp->child;	
			}
			该函数返回的libqos_rtpkt_t*指针，必须用webrtcFreePacket()回收，否则会造成内存泄露	
		
	(17) int webrtcFreePacket(WEBRTC* pc,libqos_rtpkt_t* rtp);
		功能:
			回收rtp数据缓冲
		参数:
			pc:webrtc句柄
		返回:
			0--成功，其他--错误代码
		备注:
			必须用本函数销毁或回收webrtcGetVideoPacket()/webrtcGetAudioPacket()返回的指针

	(18) MediaStream* mediaCreate();
		功能:
			建立一个默认属性的MediaStream指针
		参数:
			无
		返回:
			成功返回MediaStream*指针，否则返回NULL
	(19) void mediaAddTrack(MediaStream* media, MediaCodec codec);
		功能:
			向MediaStream增加编码类型
		参数:
			media:MediaStream*指针
			codec:CODEC_H264/CODEC_PCMA/CODEC_PCMU/CODEC_OPUS,函数内部自动识别音频视频类型
		返回:
			无
	(20) void mediaDestroy(MediaStream* media);
		功能:
			销毁MediaStream*指针
		参数:
			media:MediaStream*指针
		返回:
			无
4.编译指南
	make clean
	make PLATFORM=ssd202
	make PLATFORM=x86

	
