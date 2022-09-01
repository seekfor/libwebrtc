PLATFORM ?= x86

ifeq ($(PLATFORM),ssd202)
	CC := arm-linux-gnueabihf-gcc
	STRIP := arm-linux-gnueabihf-strip
	INSTALL_PATH := ./lib/ssd202
	SRCS := $(wildcard ./source/ssd202/*.c)
	CFLAGS = -I../../project/release/include -I./include
	LDFLAGS = -L../../project/release/nvr/i2m/common/glibc/8.2.1/mi_libs/dynamic -lmi_common -lmi_ai -lmi_ao -lmi_vdec -lmi_sys -lmi_disp -lmi_panel
endif

ifeq ($(PLATFORM),x86)
	CC := gcc
	STRIP := strip
	INSTALL_PATH := ./lib/x86
	SRCS := $(wildcard ./source/x86/*.c)
	CFLAGS = -I./include
endif

CFLAGS += -g -O2 -I./ -I$(INSTALL_PATH)/libwebrtc/include
CFLAGS += -I$(INSTALL_PATH)/libnice/include/nice -I$(INSTALL_PATH)/libnice/include/glib-2.0 -I$(INSTALL_PATH)/libnice/lib/glib-2.0/include
ifeq ($(PLATFORM),x86)
        CFLAGS += -I$(INSTALL_PATH)/libnice/lib/x86_64-linux-gnu/glib-2.0/include/
endif
CFLAGS += -I$(INSTALL_PATH)/openssl/include
CFLAGS += -I$(INSTALL_PATH)/cJSON/include
CFLAGS += -I$(INSTALL_PATH)/libsrtp/include/srtp2
CFLAGS += -I$(INSTALL_PATH)/libcurl/include/curl
CFLAGS += -I$(INSTALL_PATH)/librtp/include
CFLAGS += -I$(INSTALL_PATH)/ffmpeg/include
CFLAGS += -I$(INSTALL_PATH)/libopus/include/opus
CFLAGS += -D_FILE_OFFSET_BITS=64


LDFLAGS += -lpthread -lm -ldl
LDFLAGS += -L$(INSTALL_PATH)/openssl/lib -L$(INSTALL_PATH)/libnice/lib -L$(INSTALL_PATH)/cJSON/lib -L$(INSTALL_PATH)/libsrtp/lib
LDFLAGS += -L$(INSTALL_PATH)/librtp/lib -L$(INSTALL_PATH)/ffmpeg/lib -L$(INSTALL_PATH)/libwebrtc/lib -L$(INSTALL_PATH)/libcurl/lib -L$(INSTALL_PATH)/cJSON/lib
LDFLAGS += -L$(INSTALL_PATH)/libopus/lib
LDFLAGS += -lz -lffi -lgmodule-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lnice
LDFLAGS += -lssl -lcrypto
#LDFLAGS += -lffmpeg -lwebrtc -lrtp -lcurl -lcjson -lsrtp2 -lopus -lpthread -lm -lc -ldl
LDFLAGS += -lpthread -lm -lc -ldl
ifeq ($(PLATFORM),x86)
	LDFLAGS += -lidn2
endif
STATIC_LIBS := $(INSTALL_PATH)/ffmpeg/lib/libffmpeg.a
STATIC_LIBS += $(INSTALL_PATH)/libwebrtc/lib/libwebrtc.a
STATIC_LIBS += $(INSTALL_PATH)/cJSON/lib/libcjson.a
STATIC_LIBS += $(INSTALL_PATH)/libsrtp/lib/libsrtp2.a
STATIC_LIBS += $(INSTALL_PATH)/librtp/lib/librtp.a
STATIC_LIBS += $(INSTALL_PATH)/libcurl/lib/libcurl.a
STATIC_LIBS += $(INSTALL_PATH)/libopus/lib/libopus.a



OBJS = $(patsubst %.c,%.o, $(SRCS))


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(OBJS)
	$(CC) $(CFLAGS) -o webrtc $(OBJS) $(STATIC_LIBS) $(LDFLAGS)
#	$(STRIP) webrtc
clean:
	rm -rf $(OBJS) webrtc

