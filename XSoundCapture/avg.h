/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _AVG_H_RPCGEN
#define _AVG_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MAXBUFFERSIZE 1024

struct input_data {
	struct {
		u_int input_data_len;
		char *input_data_val;
	} input_data;
};
typedef struct input_data input_data;


struct control_data {
	int flag;
};
typedef struct control_data control_data;


#define SERVERPROG 22855
#define SERVERVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define SERVER 1
extern  int * server_1(input_data *, CLIENT *);
extern  int * server_1_svc(input_data *, struct svc_req *);
extern int serverprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SERVER 1
extern  int * server_1();
extern  int * server_1_svc();
extern int serverprog_1_freeresult ();
#endif /* K&R C */

#define CONTROLPROG 22856
#define CONTROLVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define CONTROL 1
extern  int * control_1(control_data *, CLIENT *);
extern  int * control_1_svc(control_data *, struct svc_req *);
extern int controlprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define CONTROL 1
extern  int * control_1();
extern  int * control_1_svc();
extern int controlprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_input_data (XDR *, input_data*);
extern  bool_t xdr_input_data (XDR *, input_data*);
extern  bool_t xdr_control_data (XDR *, control_data*);
extern  bool_t xdr_control_data (XDR *, control_data*);

#else /* K&R C */
extern bool_t xdr_input_data ();
extern bool_t xdr_input_data ();
extern bool_t xdr_control_data ();
extern bool_t xdr_control_data ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_AVG_H_RPCGEN */