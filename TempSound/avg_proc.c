/* Remote average server code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 *
 * Reformatted and loggin by Mark A. Sheldon.
 *
 */
#include <rpc/rpc.h>
#include "avg.h"
#include <stdio.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define BUFSIZE 1024

#define LOGFILENAME "/tmp/avg_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)
FILE *logfile;

static int sum_avg;
static pa_simple *s_write = NULL;


int startWriteServer()
{
    /* The Sample format to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    int ret = 1;
    int error;

    /* Create a new playback stream */
    if (!(s_write = pa_simple_new(NULL, "write_server", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    }

    return ret;
}

int stopWriteServer()
{
    if (s_write)
        pa_simple_free(s_write);

    return 0;
}

int *server_1(input_data *input, CLIENT *client)
{
 	u_int i;
	int error;
        uint8_t buf[BUFSIZE];
        ssize_t r;
	char *dp = input->input_data.input_data_val;
 
	printf("server: buffer=");
        for (i = 1; i <= 1024; i++) {
		printf("%c ", *dp);
                dp++;
        }
	printf("\n");
	
	/*
	dp = input->input_data.input_data_val;

	printf("writing\n");
        if (pa_simple_write(s_write, dp, (size_t) 1024, &error) < 0) {
            printf(": pa_simple_write() failed: %s\n", pa_strerror(error));
        }
	*/
	sum_avg = 1;
        return &sum_avg;
}

int *server_1_svc(input_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return server_1(input, client);
}

int *control_1(control_data *input, CLIENT *client)
{
	printf("control: flag=%i\n", input->flag);
	
	if (input->flag == 1) {
		startWriteServer();
	} else if (input->flag == 2) {
		
	} else if (input->flag == 3) {
		stopWriteServer();
	}
	sum_avg = 2;
        return &sum_avg;
}

int *control_1_svc(control_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return control_1(input, client);
}
