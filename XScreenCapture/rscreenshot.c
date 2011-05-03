/* Remote screenshot client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "screenshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

static CLIENT *clnt;

GdkPixbuf *get_screenshot(GdkPixbuf *screenshot){
    GdkWindow *root_window;
    gint x_orig, y_orig;
    gint width, height;
    root_window = gdk_get_default_root_window ();
    gdk_drawable_get_size (root_window, &width, &height);      
    gdk_window_get_origin (root_window, &x_orig, &y_orig);

    screenshot = gdk_pixbuf_get_from_drawable (NULL, root_window, NULL,
                                           x_orig, y_orig, 0, 0, width, height);
    return screenshot;
}

gboolean save_func(const gchar *buf, gsize count, GError **error, gpointer data){	
        char *host;
        input_data  screenshot_1_arg;
 	gboolean result;

        screenshot_1_arg.input_data.input_data_val = (gchar *) malloc(4096*sizeof(gchar));
	memcpy(screenshot_1_arg.input_data.input_data_val, buf, 4096);
        screenshot_1_arg.input_data.input_data_len = 4096;

        result = screenshot_1(&screenshot_1_arg, clnt);
	if (result == NULL) {
	/*
	* An error occurred while calling the server.
	* Print error message and die.
	*/
		clnt_perror(clnt, "localhost");
		exit(1);
	}
    return TRUE;
}

void screenshotprog_1( char* host, int argc, char *argv[])
{
        double  *result_1, *dp, f;
        char *endptr;
        int result, i;
        GdkPixbuf *screenshot;
        input_data  screenshot_1_arg;
        CLIENT *clnt2, *clnt3, *clnt4;	


	gtk_init(NULL, NULL);

	clnt = clnt_create(host, SCREENSHOTPROG, SCREENSHOTVERS, "udp");
        clnt2 = clnt_create(host, DELETEIMAGEPROG, DELETEIMAGEVERS, "udp");
	clnt3 = clnt_create(host, REFRESHDISPLAYPROG, REFRESHDISPLAYVERS, "udp");	
	clnt4 = clnt_create(host, INITDISPLAYPROG, INITDISPLAYVERS, "udp");	
	initdisplay_1(NULL, clnt4);

	while (1){
	        result = deleteimage_1(NULL, clnt2);
		screenshot = get_screenshot(screenshot);
	    	gdk_pixbuf_save_to_callback(screenshot, save_func, NULL, "jpeg", NULL, "quality", "20", NULL);
        	result = refreshdisplay_1(NULL, clnt3);
		sleep(1.0);
		fprintf(stderr, "Taking screenshot\n");
	}

}


main( int argc, char* argv[] )
{
        char *host;
/*
        if (argc < 3) {
                printf("usage: %s server_host value ...\n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXAVGSIZE + 2) {
                printf("Two many input values\n");
                exit(2);
        }*/
        host = argv[1];
        screenshotprog_1(host, argc, argv);
}
