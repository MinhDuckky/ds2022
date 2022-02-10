/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RPC_H_RPCGEN
#define _RPC_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct Request {
	char *file;
	char *action;
};
typedef struct Request Request;

#define HANDLE_FILE 0x77777777
#define PROG_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define response 1
extern  char ** response_1(Request *, CLIENT *);
extern  char ** response_1_svc(Request *, struct svc_req *);
extern int handle_file_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define response 1
extern  char ** response_1();
extern  char ** response_1_svc();
extern int handle_file_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Request (XDR *, Request*);

#else /* K&R C */
extern bool_t xdr_Request ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPC_H_RPCGEN */
