#ifndef __G711_H__
#define __G711_H__

#ifdef __cplusplus
extern "C"
{
#endif

	int AlawDecode(unsigned char* in, int size,short int* out);
	int AlawEncode(short int* in, int size, unsigned char* out);

	int UlawDecode(unsigned char* in, int size, short int* out);
	int UlawEncode(short int* in, int size, unsigned char* out);

#ifdef __cplusplus
}
#endif


#endif

