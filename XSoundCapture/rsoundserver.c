#include "soundserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static CLIENT *clnt_server;
static CLIENT *clnt_control;

void update_remote_display(CLIENT *clnt_refresh, CLIENT *clnt_delete){
        int *result;

        result = 0;
	fprintf(stderr, "Taking soundserver\n");
}

void create_clients(char *host)
{
        clnt_server = clnt_create(host, SOUNDSERVERPROG, SOUNDSERVERVERS, "udp");
	clnt_control = clnt_create(host, CONTROLPROG, CONTROLVERS, "udp");	

        if (clnt_server == NULL || clnt_control) {
                clnt_pcreateerror(host);
                exit(1);
        }
}

void close_clients()
{
	clnt_destroy(clnt_server);
	clnt_destroy(clnt_control);	
}

void soundserverprog_1(char *host, double framerate)
{
        input_data soundserver_1_arg;
	control_data control_1_arg;

	create_clients(host);

	control_1_arg.init = 1;
	control_1(&control_1_arg, clnt_control);
	soundserver_1(NULL, clnt_server);	

	sleep(10);

	/*
        while (1){
		update_remote_display(clnt_refresh, clnt_delete);
		sleep(1.0 / framerate);
	}
	*/
	close_clients(); 
}


main( int argc, char* argv[] )
{
        soundserverprog_1(argv[1], 1.0);
}
