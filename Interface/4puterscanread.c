#include <gtk/gtk.h>
#include "../XOR/launch.h"
#include "../Treatment/seg.h"

GtkWindow *window;
GtkLabel *title;
GtkLabel *input;
GtkPaned *paned;
GtkGrid *grid;
GtkFileChooserButton *open;
GtkButton *launch;
GtkImage *logo;
GtkWidget *quit;

void on_open(GtkFileChooserButton *chooser, GtkWidget *l)
{
    gtk_widget_set_sensitive(GTK_WIDGET(l), TRUE);
}

void on_launch(GtkButton *button, GtkFileChooserButton *c)
{
    seg(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(c)));
    launcher("../Treatment/.car");
}

int main(int argc, char *argv[])
{
    // Initializes GTK.
    gtk_init(NULL, NULL);

    // Constructs a GtkBuilder instance.
    GtkBuilder* builder = gtk_builder_new ();

    // Loads the UI description.
    // (Exits if an error occurs.)
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "4puterscanread.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.4puterscanread"));
    title = GTK_LABEL(gtk_builder_get_object(builder, "title"));
    input = GTK_LABEL(gtk_builder_get_object(builder, "in_label"));
    paned = GTK_PANED(gtk_builder_get_object(builder, "window_panel"));
    grid = GTK_GRID(gtk_builder_get_object(builder, "buttons_grid"));
    open = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "open_button"));
    launch = GTK_BUTTON(gtk_builder_get_object(builder, "launch_button"));
    logo = GTK_IMAGE(gtk_builder_get_object(builder, "logo_ocr"));
    quit = GTK_WIDGET(gtk_builder_get_object(builder, "quit"));

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(open, "file-set", G_CALLBACK(on_open), launch);
    g_signal_connect(launch, "clicked", G_CALLBACK(on_launch), open);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
