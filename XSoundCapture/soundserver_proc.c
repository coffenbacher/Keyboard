#include <rpc/rpc.h>
#include "soundserver.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>

static int result;

int *soundserver_1(input_data *input, CLIENT *client)
{
	FILE *fp;

	printf("server\n");
	/*
	dp = input->input_data.input_data_val;
        fp = fopen("screenshot.jpeg", "a");
        fwrite(dp, sizeof(*dp), 4096, fp);
	fclose(fp);
	*/
	result = 1;
        return &result;
}

int *soundserver_1_svc(input_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return soundserver_1(input, client);
}

int *control_1(control_data *input, CLIENT *client)
{
	int i;

	printf("control: i=%i\n", input->init);

	result = 1;
        return &result;
}

int *control_1_svc(control_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return control_1(input, client);
}
