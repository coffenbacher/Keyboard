#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
/*int main (int argc, char *argv[])
{
    GdkWindow *window_root;
    GdkPixbuf *screenshot;
    gint width, height;
    gtk_init(&argc, &argv);
    window_root = gdk_get_default_root_window ();
    
    gdk_drawable_get_size(window_root, &width, &height);
    screenshot = gdk_pixbuf_get_from_drawable (NULL, window_root, NULL,
                                               0, 0, 0, 0,
                                               width, height);
                                               
    gdk_pixbuf_save (screenshot, "screenshot.jpg", "jpeg", NULL,
                     "quality", "100", NULL);

   return 0;
}*/

GdkPixbuf * get_screenshot(){
    GdkPixbuf *screenshot;
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
    FILE *fp;
    fp = fopen("test.png", "a");
    fwrite(buf, sizeof(buf), count, fp);
    fclose(fp);
    return 1;
}
int main(int argc, char *argv[]) {
    int i = 0;
    GdkPixbuf *screenshot;
    gtk_init(&argc, &argv);
    for (i=0; i<100; i++)
        screenshot = get_screenshot();
    /*gdk_pixbuf_save_to_callback(screenshot, save_func, NULL, "png", NULL, NULL);*/
    /*for (i=0; i<100; i++) 
        gdk_pixbuf_save(screenshot, "screenshot.jpg", "jpeg", NULL, "quality", "20", NULL);*/
    return 1;
}
