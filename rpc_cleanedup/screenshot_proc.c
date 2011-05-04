/* Software Systems Final Project
   screenshot_proc.c
   Screenshot Server Code

   Authors: Charles Offenbacher, Poorva Singal, and Jon Reed
   Last updated: 5/4/2011

   Notes:
   This file receives image data and stores it in screenshot.jpeg until
   the image file is completely written. It forks and launches the graphics
   executable and signals it when a new image should be loaded.

   Charlie wrote this file.

   This file depends on the system call "pidof", which is bundled with
   Ubuntu 10.10, the target system. Behavior on other platforms may be
   unstable.

   Credits:
   The basic structure for how the RPC is set up was taken from:
   Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html

   We used it essentially just for the structure but modified all of its
   methods.

   The rest of the file consists of common programming patterns including
   fork-exec and kill (signalling), both available on Wikipedia:
   http://en.wikipedia.org/wiki/Fork-exec
   http://en.wikipedia.org/wiki/Signal_(computing)
   http://en.wikipedia.org/wiki/Kill_(command)

   We also referred a to the man pages.
 */



#include <rpc/rpc.h>
#include "screenshot.h"
#include <stdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/prctl.h>

static int result; /* global - only used for responses which are ignored */

/* Writes a received block to file and then flushes it */
int *screenshot_1(input_data *input, CLIENT *client)
{
	FILE *fp;
	gchar *dp;
	int blocksize = 4096;

	dp = input->input_data.input_data_val;
	fp = fopen("screenshot.jpeg", "a");
	if(fp == NULL) {
	        fprintf(stderr, "failed to open screenshot file");
	        return EXIT_FAILURE;
	}
	fwrite(dp, sizeof(*dp), blocksize, fp);
	fclose(fp);
	result = 1;
	return &result;
}

/* RPC service - save input*/
int *screenshot_1_svc(input_data *input, struct svc_req *svc)
{
	CLIENT *client;
	return screenshot_1(input, client);
}

/* RPC service - clears screenshot file*/
int *deleteimage_1_svc(void *tmp, struct svc_req *svc)
{
	unlink("screenshot.jpeg");
	result = 1;
	return &result;
}

/* Fork-exec off graphics process to launch viewing window */
int exec_graphics()
{
	pid_t forkpid;
	if ((forkpid = fork()) < 0)
		fprintf(stderr, "Can't create fork.");
	else if (forkpid == 0) {
		execv("graphics", NULL);
		exit(1);
	} else {
		return forkpid;
	}
}

/* Signal graphics process that a new image is ready */
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

/* DEPRECATED - left for improvements in the future */
int *initdisplay_1_svc(void *tmp, struct svc_req *svc)
{
	result = 1;
	return &result;
}
