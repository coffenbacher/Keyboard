/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "avg.h"

bool_t
xdr_input_data (XDR *xdrs, input_data *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->flag))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->input_data.input_data_val, (u_int *) &objp->input_data.input_data_len, 200,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}