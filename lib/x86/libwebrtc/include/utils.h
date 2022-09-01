#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

	int utlGetIPV4Addr(char *hostname, char *ipv4addr, size_t size);
	int utlIsValidIPAddr(char *ip_address);
	int utlParseURL(unsigned char* url,int defport,unsigned char* username,unsigned char* password,unsigned char* hostname,unsigned char* port,unsigned char* path,unsigned char* params);

#ifdef __cplusplus
}
#endif

#endif


