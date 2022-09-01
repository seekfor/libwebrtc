#ifndef _DTLS_H_
#define _DTLS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <srtp.h> 

typedef struct
{
	SSL *ssl;
	SSL_CTX *ssl_ctx;
	X509 *certificate;
	EVP_PKEY *private_key;
	BIO *read_bio;
	BIO *write_bio;

	srtp_policy_t remote_policy;
	srtp_policy_t local_policy;
	srtp_t srtp_in;
	srtp_t srtp_out;

	char fingerprint[160];
	gboolean handshake_done;
	gboolean srtp_init_done;
}DTLS;

#ifdef __cplusplus
extern "C"
{
#endif

	DTLS* dtlsCreate(BIO* bio);
	void dtlsDestroy(DTLS *dtls);
	int dtlsValidate(char *buf);
	int dtlsIncommingMsg(DTLS *dtls, char *buf, int len);
	void dtlsDoHandshake(DTLS *dtls);
	void dtlsEncryptRTP(DTLS *dtls,uint8_t *buf,int* size);
	void dtlsDecryptRTP(DTLS *dtls,uint8_t *buf,int* size);
	void dtlsDecryptRTCP(DTLS *dtls,uint8_t *buf,int* size);
	void dtlsEncryptRTCP(DTLS *dtls,uint8_t *buf,int* size);
	char* dtlsGetFingerprint(DTLS *dtls);
	int dtlsIsSRTPInitialized(DTLS *dtls);

#ifdef __cplusplus
}
#endif

#endif
