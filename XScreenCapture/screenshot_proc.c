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
#include <unistd.h>

FILE *fp;

static double sum_screenshot;
static int result;

int *screenshot_1(input_data *input, CLIENT *client)
{
        gchar *dp = input->input_data.input_data_val;
	
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
	unlink("/home/charles/Keyboard/XScreenCapture/screenshot.jpeg");
	result = 1;
        return &result;
}

int *refreshdisplay_1_svc(void *tmp, struct svc_req *svc)
{
	/* INSERT NEW SIGNAL HERE */
	/* image = gtk_image_new_from_file ("screenshot.jpeg"); */
	/*gtk_container_add(GTK_CONTAINER(window), image);	
	gtk_widget_show_all(window);
	gtk_main();
	
	gtk_widget_queue_draw(window);*/
	kill(12036, SIGINT);
	sleep(0.1);
	result = 1;
	return &result;
}

int *initdisplay_1_svc(void *tmp, struct svc_req *svc)
{
       result = 1;	
       return &result;
}
