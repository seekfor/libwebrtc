#ifndef _NICE_H_
#define _NICE_H_

#include <agent.h>
#include <openssl/ssl.h>

typedef struct
{
	NiceAgent* agent;
	guint stream_id;
	guint component_id;
}NiceAgentBio;

#ifdef __cplusplus
extern "C"
{
#endif

	BIO* nice_agent_bio_new(NiceAgent *nice_agent, guint stream_id, guint component_id);

#ifdef __cplusplus
}
#endif

#endif

