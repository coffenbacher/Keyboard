/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "avg.h"
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>


#define BUFSIZE 1024

void
averageprog_1( char* host, int argc, char *argv[])
{
        CLIENT *clnt;
	int *result_1;
        double  f;
	char *dp;
        char *endptr;
        input_data  average_1_arg;
     
	static const pa_sample_spec ss = {
        	.format = PA_SAMPLE_S16LE,
        	.rate = 44100,
        	.channels = 2
	};
	int ret = 1;
	int error;
	int i,j;
	pa_simple *s_read = NULL;
	uint8_t buf[BUFSIZE];


	if (!(s_read = pa_simple_new(NULL, "read_server", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        	fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
	}

	if (pa_simple_read(s_read, buf, sizeof(buf), &error) < 0) {
		printf("pa_simple_read() failed: %s\n", pa_strerror(error));
	}

        average_1_arg.input_data.input_data_val =
                (char*) malloc(BUFSIZE*sizeof(char));
        dp = average_1_arg.input_data.input_data_val;
        average_1_arg.input_data.input_data_len = BUFSIZE;
        for ( i = 0; i <= BUFSIZE; i++) {
                *dp = 0;/*(char)buf[BUFSIZE];*/
                printf("value   = %c\n", *dp);
                dp++;
        }
        clnt = clnt_create(host, AVERAGEPROG, AVERAGEVERS, "udp");
        if (clnt == NULL) {
                clnt_pcreateerror(host);
                exit(1);
        }
        result_1 = average_1(&average_1_arg, clnt);
        if (result_1 == NULL) {
                clnt_perror(clnt, "call failed:");
        }
        clnt_destroy( clnt );
        printf("average = %i\n", *result_1);
}


main( int argc, char* argv[] )
{
        char *host;

        if (argc < 3) {
                printf("usage: %s server_host value ...\n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXAVGSIZE + 2) {
                printf("Two many input values\n");
                exit(2);
        }
        host = argv[1];
        averageprog_1(host, argc, argv);
}