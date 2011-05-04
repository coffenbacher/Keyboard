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

#define MAXAVGSIZE 1024

struct input_data {
	struct {
		u_int input_data_len;
		char *input_data_val;
	} input_data;
};
typedef struct input_data input_data;


#define AVERAGEPROG 22855
#define AVERAGEVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define AVERAGE 1
extern  double * average_1(input_data *, CLIENT *);
extern  double * average_1_svc(input_data *, struct svc_req *);
extern int averageprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define AVERAGE 1
extern  double * average_1();
extern  double * average_1_svc();
extern int averageprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_input_data (XDR *, input_data*);
extern  bool_t xdr_input_data (XDR *, input_data*);

#else /* K&R C */
extern bool_t xdr_input_data ();
extern bool_t xdr_input_data ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_AVG_H_RPCGEN */
