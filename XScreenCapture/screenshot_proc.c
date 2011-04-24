/* Remote screenshot server code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 *
 * Reformatted and loggin by Mark A. Sheldon.
 *
 */
#include <rpc/rpc.h>
#include "screenshot.h"
#include <stdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define LOGFILENAME "/tmp/screenshot_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)

FILE *logfile;
FILE *fp;

static double sum_screenshot;
static int result;
int *screenshot_1(input_data *input, CLIENT *client)
{
	logfile = fopen(LOGFILENAME, "a");
        LOG("starting write\n");
        LOG("writing: \n\n %s", input->input_data.input_data_val);
        gchar *dp = input->input_data.input_data_val;
	
        fp = fopen("test_client.jpeg", "a");
        fwrite(dp, sizeof(*dp), 4096, fp);
	fclose(fp);
	LOG("ending write\n");
	fclose(logfile);
	result = 1;
        return &result;
}

int *screenshot_1_svc(input_data *input, struct svc_req *svc)
{
        CLIENT *client;
        return screenshot_1(input, client);
}

