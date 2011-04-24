#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static GtkWidget *window;
static GtkWidget *image;

void sigint_handler(int sig)
{
	fprintf(stderr, "Oof!\n");

	gtk_container_remove(GTK_CONTAINER(window), image);
	image = gtk_image_new_from_file ("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);	
	gtk_widget_show_all(window);
}

int main() {
	(void) signal(SIGINT, sigint_handler);

	gtk_init(NULL, NULL);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	image = gtk_image_new_from_file ("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);	
	gtk_widget_show_all(window);
	gtk_main();
/*        pid = fork();
        if (pid == 0) {
	}*/
}
