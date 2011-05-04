/* Software Systems Final Project
 rscreenshot.c
 RScreenshot Client Code

 Authors: Charles Offenbacher, Poorva Singal, and Jon Reed
 Last updated: 5/4/2011

 Notes:
 This file was developed by Charles to deliver screenshots to a remote
 RPC server. He developed and adapted this file from RPC and GTK+
 screenshotting documentation and resources.

 Credits:
 The basic structure for how the RPC is set up was taken from:
 Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html

 The get_screenshot() function was abstracted from code available at
 http://www.gtkforums.com/viewtopic.php?t=6771 and expanded via the
 documentation.

 The documentation available at http://developer.gnome.org/ was helpful
 in writing proper functions for both GDK and GTK+.
 --
 */

#include "screenshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <errno.h>

 /* must be global because of fixed save_func callback interface */
static CLIENT *clnt_screenshot;

/* Captures a screenshot and returns pixbuf of screen */
GdkPixbuf *get_screenshot(GdkPixbuf *screenshot)
{
	GdkWindow *root_window;
	gint x_orig, y_orig;
	gint width, height;
	root_window = gdk_get_default_root_window();
	gdk_drawable_get_size(root_window, &width, &height);
	gdk_window_get_origin(root_window, &x_orig, &y_orig);

	screenshot = gdk_pixbuf_get_from_drawable(NULL, root_window, NULL,
			x_orig, y_orig, 0, 0, width, height);
	return screenshot;
}

/* Writes image blocks to RPC stream */
gboolean save_func(const gchar *buf, gsize count, GError **err, gpointer data)
{
	char *host;
	input_data screenshot_1_arg;
	gint *result;
	gchar *input_val;
	int blocksize = 4096;

	input_val = (gchar *) malloc(blocksize * sizeof(gchar));
	if (input_val == NULL)
		exit(ENOMEM);

	/* marshall the data into opaque memory */
	screenshot_1_arg.input_data.input_data_val = input_val;
	memcpy(screenshot_1_arg.input_data.input_data_val, buf, blocksize);
	screenshot_1_arg.input_data.input_data_len = blocksize;

	result = (int *) (screenshot_1(&screenshot_1_arg, clnt_screenshot));
	if (result == NULL) {
		clnt_perror(clnt_screenshot, "localhost");
		exit(1);
	}

	free(input_val);
	return TRUE;
}

/* RPC call to trigger a graphics update on the remote computer */
void update_remote_display(CLIENT *clnt_refresh, CLIENT *clnt_delete)
{
	int *result;
	GdkPixbuf *screenshot;
	result = (int *) deleteimage_1(NULL, clnt_delete);
	screenshot = get_screenshot(screenshot);
	gdk_pixbuf_save_to_callback(screenshot, save_func, NULL, "jpeg", NULL,
			"quality", "20", NULL);
	result = refreshdisplay_1(NULL, clnt_refresh);
	fprintf(stderr, "Taking screenshot\n");
}

/* Create RPC clients. Using multiple clients for easy packet debugging */
void create_clients(char *host, CLIENT **clnt_init, CLIENT **clnt_delete,
		CLIENT **clnt_refresh)
{
	*clnt_init = clnt_create(host, INITDISPLAYPROG, INITDISPLAYVERS, "udp");
	*clnt_delete = clnt_create(host, DELETEIMAGEPROG, DELETEIMAGEVERS,
			"udp");
	clnt_screenshot = clnt_create(host, SCREENSHOTPROG, SCREENSHOTVERS,
			"udp");
	*clnt_refresh = clnt_create(host, REFRESHDISPLAYPROG,
			REFRESHDISPLAYVERS, "udp");
	if (*clnt_init == NULL || *clnt_delete == NULL || clnt_screenshot
			== NULL || *clnt_refresh == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}

}

/* Close all clients */
void close_clients(CLIENT **clnt_init, CLIENT **clnt_delete,
		CLIENT **clnt_refresh)
{
	clnt_destroy(*clnt_init);
	clnt_destroy(*clnt_delete);
	clnt_destroy(*clnt_refresh);
	clnt_destroy(clnt_screenshot);
}

/* Main loop that takes and sends screenshots until SIGINT killed */
void screenshotprog_1(char *host, double framerate)
{
	input_data screenshot_1_arg;
	CLIENT *clnt_delete, *clnt_refresh, *clnt_init;

	create_clients(host, &clnt_init, &clnt_delete, &clnt_refresh);

	gtk_init(NULL, NULL);
	initdisplay_1(NULL, clnt_init);

	while (1) {
		update_remote_display(clnt_refresh, clnt_delete);
		sleep(1.0 / framerate);
	}

	close_clients(&clnt_init, &clnt_delete, &clnt_refresh);
}

/* Simple initialization, set framerate */
main(int argc, char* argv[])
{
	double framerate = 1.0;
	screenshotprog_1(argv[1], framerate);
}
