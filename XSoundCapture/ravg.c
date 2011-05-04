/* Software Systems Final Project
 *  ravg.c
 *  Remtop Client Code (Sound portion)
 *  
 *  Authors: Jon Reed
 *  Last updated: 5/4/2011
 *
 *  This code is still in developement and as such has not been refactored or
 *  commented for clarity.  A large portion of it is taken from the RPC example
 *  provided by Mark in class. The other portions of the code have been created
 *  from the inspiration of the Simple API for PulseAudio. This can be found at:
 * 	http://0pointer.de/lennart/projects/pulseaudio/doxygen/
 *  and sample clients found here:
	http://0pointer.de/lennart/projects/pulseaudio/doxygen/pacat-simple_8c-example.html
	http://0pointer.de/lennart/projects/pulseaudio/doxygen/parec-simple_8c-example.html
 * 
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

pa_simple *s_read = NULL;

void
serverprog_1( char* host, int argc, char *argv[])
{
        CLIENT *clnt_control;
        CLIENT *clnt_server;
        control_data  control_1_arg;
        input_data  server_1_arg;
        int  *result_1, *result_2;
	char buffer[1024];
	char *dp, f;
        char *endptr;
	int i;
	uint8_t buf[BUFSIZE];
	int error;


        clnt_control = clnt_create(host, CONTROLPROG, CONTROLVERS, "udp");
        if (clnt_control == NULL) {
                clnt_pcreateerror(host);
                exit(1);
        }

        control_1_arg.flag = 1;
        result_1 = control_1(&control_1_arg, clnt_control);
	if (result_1 == NULL) {
                clnt_perror(clnt_control, "control call failed:");
        } else {
	        printf("result_1 = %i\n", *result_1);
	}




	server_1_arg.input_data.input_data_val = (char*) malloc(1024*sizeof(char));
        dp = server_1_arg.input_data.input_data_val;
        for ( i = 0; i < 100; i++) {
                *dp = (char)i;
                dp++;
        }
        clnt_server = clnt_create(host, SERVERPROG, SERVERVERS, "udp");
        if (clnt_server == NULL) {
                clnt_pcreateerror(host);
                exit(1);
        }
        result_2 = server_1(&server_1_arg, clnt_server);
        if (result_2 == NULL) {
                clnt_perror(clnt_server, "call failed:");
        }
        clnt_destroy( clnt_server );



/*
	startReadServer();
	clnt_server = clnt_create(host, SERVERPROG, SERVERVERS, "udp");
	if (clnt_server == NULL) {
		clnt_pcreateerror(host);
		printf("clnt_server error\n");
		exit(1);
	}
        server_1_arg.input_data.input_data_val = (char*) malloc(1024*sizeof(char));

	dp = server_1_arg.input_data.input_data_val;
        for ( i = 0; i < 1024; i++) {
                *dp = (char)i;
                dp++;
        }
        result_2 = server_1(&server_1_arg, clnt_server);
        if (result_2 == NULL) {
               	printf("result2  error\n");
		clnt_perror(clnt_server, "server call failed:");
	} else {
        	printf("result_2 = %i\n", *result_2);
	}
*/
/*
	for (i = 0; i < 1000; i++) {
        	printf("i=%i\n", i);

		if (pa_simple_read(s_read, buf, sizeof(buf), &error) < 0) {
			printf("pa_simple_read() failed: %s\n", pa_strerror(error));
		}
		dp = server_1_arg.input_data.input_data_val;
		for ( i = 0; i < 1024; i++) {
			f = 0;
			*dp = f;
			dp++;
		}
		printf("before\n");
		result_2 = server_1(&server_1_arg, clnt_server);
		printf("after\n");
		if (result_2 == NULL) {
			printf("result2  error");
			clnt_perror(clnt_server, "server call failed:");
		}				
		printf("result_2 = %i\n", *result_2);
	}
*/	
	control_1_arg.flag = 3;
        result_1 = control_1(&control_1_arg, clnt_control);
	if (result_1 == NULL) {
                clnt_perror(clnt_control, "control call failed:");
        }
        printf("result_1 = %i\n", *result_1);

        clnt_destroy( clnt_server );
        clnt_destroy( clnt_control );
	stopReadServer();
	
        /*printf("result_2 = %i\n", *result_2);*/
}

int startReadServer()
{
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    int ret = 1;
    int error;
    int i,j;

    /* Create the recording stream */
    if (!(s_read = pa_simple_new(NULL, "read_server", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
finish:
    return ret;
}

int stopReadServer()
{
    if (s_read)
        pa_simple_free(s_read);
}

main( int argc, char* argv[] )
{
        char *host;

        if (argc < 3) {
                printf("usage: %s server_host value ...\n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXBUFFERSIZE + 2) {
                printf("Two many input values\n");
                exit(2);
        }
        host = argv[1];
        serverprog_1(host, argc, argv);
}
