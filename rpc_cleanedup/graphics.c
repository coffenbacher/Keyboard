/* Software Systems Final Project
   graphics.c
   Graphics Code

   Authors: Charles Offenbacher, Poorva Singal, and Jon Reed
   Last updated: 5/4/2011

   Notes:
   This file loads image data from screenshot.jpeg and displays it in a
   fullscreen window. When it receives a SIGIO signal, it updates the image.

   Charlie wrote this file.

   Credits:
   The majority of this file came from the GTK+ documentation. The signalling
   came from the GTK+ tutorial at:
   http://www.gtk.org/tutorial1.2/gtk_tut-2.html

   We also referred a to the man pages.
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static GtkWidget *window;
static GtkWidget *image;

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return (FALSE);
}

void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
	exit(1);
}

/* Update image */
void sigio_handler(int sig)
{
	gtk_container_remove(GTK_CONTAINER(window), image);
	image = gtk_image_new_from_file("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);
	gtk_widget_show_all(window);
}

int main()
{
	(void) signal(SIGIO, sigio_handler);

	gtk_init(NULL, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	image = gtk_image_new_from_file("screenshot.jpeg");
	gtk_container_add(GTK_CONTAINER(window), image);
	gtk_widget_show_all(window);
	gtk_window_fullscreen(GTK_WINDOW(window));

	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(
			delete_event), NULL);

	gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(
			destroy), NULL);

	gtk_main();
	exit(1);
}
