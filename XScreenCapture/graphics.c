#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static GtkWidget *window;
static GtkWidget *image;

void sigio_handler(int sig)
{
	printf("Updating buffer...");

	gtk_container_remove(GTK_CONTAINER(window), image);
	image = gtk_image_new_from_file ("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);	
	gtk_widget_show_all(window);
}

int main() {
	(void) signal(SIGIO, sigio_handler);

	gtk_init(NULL, NULL);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	image = gtk_image_new_from_file ("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);	
	gtk_widget_show_all(window);
	gtk_window_fullscreen(window);
	gtk_main();
/*        pid = fork();
        if (pid == 0) {
	}*/
}
