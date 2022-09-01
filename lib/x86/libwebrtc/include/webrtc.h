#ifndef __WEBRTC_H__
#define __WEBRTC_H__


#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <termios.h>
//#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <linux/sockios.h>
#include <dirent.h>
//#include <linux/if.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <memory.h>
#include <string.h>
#include <sys/statfs.h>



#include <gio/gnetworking.h>
#include <agent.h>
#include <glib.h>

#include "media.h"
#include "config.h"
#include "curl.h"
#include "qos.h"
#include "dtls.h"
#include "nice.h"
#include "rtp.h"
#include "rtcp.h"
#include "utils.h"
#include "sdp.h"
#include "peer.h"
#include "packetizer.h"
#include "depacketizer.h"
#include "janus.h"

#include "opus.h"
#include "g711.h"

#endif

