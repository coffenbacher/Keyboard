#include "screenshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <errno.h>

static CLIENT *clnt_screenshot;

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

gboolean save_func(const gchar *buf, gsize count, GError **err, gpointer data){
        char *host;
        input_data screenshot_1_arg;
 	gint *result;
	gchar *input_val;
	
	input_val = (gchar *)malloc(4096 * sizeof(gchar));
	if (input_val == NULL) exit(ENOMEM); 
        
	screenshot_1_arg.input_data.input_data_val = input_val;
	memcpy(screenshot_1_arg.input_data.input_data_val, buf, 4096);
        screenshot_1_arg.input_data.input_data_len = 4096;

        result = (int *) (screenshot_1(&screenshot_1_arg, clnt_screenshot));
	if (result == NULL) {
		clnt_perror(clnt_screenshot, "localhost");
		exit(1);
	}
	
	free(input_val);	
	return TRUE;
}

void update_remote_display(CLIENT *clnt_refresh, CLIENT *clnt_delete){
        int *result;
        GdkPixbuf *screenshot;
	result = (int *)deleteimage_1(NULL, clnt_delete);
	screenshot = get_screenshot(screenshot);
	gdk_pixbuf_save_to_callback(screenshot, save_func, NULL, "jpeg", 
						NULL, "quality", "20", NULL);
        result = refreshdisplay_1(NULL, clnt_refresh);
	fprintf(stderr, "Taking screenshot\n");
}

void create_clients(char *host, CLIENT **clnt_init, CLIENT **clnt_delete, 
				CLIENT **clnt_refresh)
{
	*clnt_init = clnt_create(host, INITDISPLAYPROG, INITDISPLAYVERS, "udp");	
        *clnt_delete = clnt_create(host, DELETEIMAGEPROG, DELETEIMAGEVERS, "udp");
	clnt_screenshot = clnt_create(host, SCREENSHOTPROG, SCREENSHOTVERS, "udp");
	*clnt_refresh = clnt_create(host, REFRESHDISPLAYPROG, REFRESHDISPLAYVERS, "udp");	
        if (*clnt_init == NULL || *clnt_delete == NULL || clnt_screenshot == NULL 
	   || *clnt_refresh == NULL) {
                clnt_pcreateerror(host);
                exit(1);
        }

}

void close_clients(CLIENT **clnt_init, CLIENT **clnt_delete, CLIENT **clnt_refresh)
{
	clnt_destroy(*clnt_init);
	clnt_destroy(*clnt_delete);
	clnt_destroy(*clnt_refresh);
	clnt_destroy(clnt_screenshot);	
}

void screenshotprog_1(char *host, double framerate)
{
        input_data screenshot_1_arg;
        CLIENT *clnt_delete, *clnt_refresh, *clnt_init;	

	create_clients(host, &clnt_init, &clnt_delete, &clnt_refresh);

	gtk_init(NULL, NULL);
	initdisplay_1(NULL, clnt_init);
	
        //TODO: NEED TO IMPLEMENT QUITTING SOMEHOW ?? 
        while (1){
		update_remote_display(clnt_refresh, clnt_delete);
		sleep(1.0 / framerate);
	}

	close_clients(&clnt_init, &clnt_delete, &clnt_refresh); 
}


main( int argc, char* argv[] )
{
        screenshotprog_1(argv[1], 1.0);
}
