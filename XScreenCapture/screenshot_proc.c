#include <rpc/rpc.h>
#include "screenshot.h"
#include <stdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/prctl.h>

static int result;

int *screenshot_1(input_data *input, CLIENT *client)
{
	FILE *fp;
        gchar *dp;

	dp = input->input_data.input_data_val;
        fp = fopen("screenshot.jpeg", "a");
        fwrite(dp, sizeof(*dp), 4096, fp);
	fclose(fp);
	result = 1;
        return &result;
}

int *screenshot_1_svc(input_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return screenshot_1(input, client);
}

int *deleteimage_1_svc(void *tmp, struct svc_req *svc)
{
	unlink("screenshot.jpeg");
	result = 1;
        return &result;
}

int exec_graphics()
{
	pid_t forkpid;
	if ( (forkpid = fork()) < 0 )
       	        fprintf(stderr, "Can't create fork.");
        else if (forkpid == 0){
               	execv("graphics", NULL);
		exit(1);
	}
	else {
		return forkpid;
	}
}

int *refreshdisplay_1_svc(void *tmp, struct svc_req *svc)
{
        FILE *pid_grep;
        pid_t forkpid;
	int pid;
	pid_grep = popen("pidof graphics", "r");
	if (fscanf(pid_grep, "%d", &pid) == -1) {
	        pid = exec_graphics();
		result = 0;
		return &result;
	}
	kill(pid, SIGIO);
	result = 1;
	return &result;
	
}

int *initdisplay_1_svc(void *tmp, struct svc_req *svc)
{
/* PUT INITIALIZATION AND FORKING CODE HERE */
       result = 1;	
       return &result;
}
