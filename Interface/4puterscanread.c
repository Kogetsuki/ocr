#include <gtk/gtk.h>

#include "../Treatment/seg.h"
#include "../XOR/launch.h"

static GtkWindow *window;

static GtkImage    *image_preview;
static GtkTextView *text_view;

static GtkButton *open_button;
static GtkButton *ocr_button;
static GtkButton *save_button;
static GtkButton *quit_button;

static GtkLabel *status_label;

static gchar *selected_file = NULL;

// ========================================================================
// Open image dialog
// ========================================================================

void on_open_clicked()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Select image",
      window,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Open",
      GTK_RESPONSE_ACCEPT,
      NULL
  );

  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "../Treatment/tests");

  GtkFileFilter *filter = gtk_file_filter_new();

  gtk_file_filter_add_pixbuf_formats(filter);
  gtk_file_filter_set_name(filter, "Images");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    g_free(selected_file);

    selected_file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    gtk_image_set_from_file(image_preview, selected_file);
    gtk_widget_set_sensitive(GTK_WIDGET(ocr_button), TRUE);
    gtk_label_set_text(status_label, "Image loaded");
  }

  gtk_widget_destroy(dialog);
}

// ========================================================================
// OCR
// ========================================================================
void on_ocr_clicked()
{
  if (!selected_file)
    return;

  gtk_label_set_text(status_label, "Running OCR...");

  // Existing pipeline
  seg(selected_file);
  launcher("../Treatment/.car");

  // Read OCR result
  FILE *file = fopen("finalresult.txt", "r");
  if (!file)
  {
    gtk_label_set_text(status_label, "Unable to read OCR output");
    return;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  char *content = malloc(size + 1);
  fread(content, 1, size, file);
  content[size] = '\0';
  fclose(file);

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_set_text(buffer, content, -1);

  free(content);

  gtk_widget_set_sensitive(GTK_WIDGET(save_button), TRUE);
  gtk_label_set_text(status_label, "OCR completed");
}

// ========================================================================
// Save output
// ========================================================================

void on_save_clicked()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Save Text",
      window,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Save",
      GTK_RESPONSE_ACCEPT,
      NULL
  );

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);

    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_bounds(buffer, &start, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    FILE *f = fopen(filename, "w");
    if (f)
    {
      fputs(text, f);
      fclose(f);
    }

    g_free(text);
    g_free(filename);

    gtk_label_set_text(status_label, "File saved");
  }

  gtk_widget_destroy(dialog);
}

// ========================================================================
// Main
// ========================================================================

int main(void)
{
  gtk_init(NULL, NULL);

  GtkBuilder *builder = gtk_builder_new_from_file("4puterscanread.glade");

  window        = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  image_preview = GTK_IMAGE(gtk_builder_get_object(builder, "image_preview"));
  text_view     = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_view"));
  status_label  = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));
  open_button   = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
  ocr_button    = GTK_BUTTON(gtk_builder_get_object(builder, "ocr_button"));
  save_button   = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
  quit_button   = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));

  g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), NULL);
  g_signal_connect(ocr_button, "clicked", G_CALLBACK(on_ocr_clicked), NULL);
  g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), NULL);
  g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_main();

  g_free(selected_file);

  return 0;
}