/* Remote average server code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 *
 * Reformatted and loggin by Mark A. Sheldon.
 *
 */
#include <rpc/rpc.h>
#include "avg.h"
#include <stdio.h>


#define LOGFILENAME "/tmp/avg_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)
FILE *logfile;

static int sum_avg;

int *average_1(input_data *input, CLIENT *client)
{
        char *dp = input->input_data.input_data_val;
        u_int i;

	logfile = fopen(LOGFILENAME, "a");
	LOG("Average called with:  ");

        sum_avg = 0;
        for (i = 1; i <= input->input_data.input_data_len; i++) {
		LOG("%f ", *dp);
                /*sum_avg = sum_avg + *dp;*/
		printf("%c", *dp);
		dp++;
        }
        sum_avg = sum_avg / input->input_data.input_data_len;

	LOG("\nReturning:  %f\n", sum_avg);
	fclose(logfile);
        return &sum_avg;
}

int *average_1_svc(input_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return average_1(input, client);
}
