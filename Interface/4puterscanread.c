#include <gtk/gtk.h>

#include "../Treatment/seg.h"
#include "../XOR/launch.h"

static GtkWindow *window;
static GtkSpinner *spinner;

static GtkImage *image_preview;
static GtkTextView *text_view;

static GtkButton *open_button;
static GtkButton *ocr_button;
static GtkButton *save_button;
static GtkButton *quit_button;

static GtkLabel *status_label;

static gchar *selected_file = NULL;

// ========================================================================
// OCR thread data
// ========================================================================

typedef struct
{
  gchar *file;
} OcrData;

// ========================================================================
// UI update after OCR success
// ========================================================================

static gboolean update_text_view(gpointer data)
{
  char *content = (char *) data;

  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_set_text(buffer, content, -1);

  free(content);

  gtk_spinner_stop(spinner);
  gtk_label_set_text(status_label, "OCR completed");
  gtk_widget_set_sensitive(GTK_WIDGET(save_button), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(ocr_button), TRUE);

  return FALSE;
}

// ========================================================================
// UI update after OCR failure
// ========================================================================

static gboolean ocr_failed(gpointer data)
{
  (void) data;

  gtk_spinner_stop(spinner);
  gtk_label_set_text(status_label, "Unable to read OCR output");
  gtk_widget_set_sensitive(GTK_WIDGET(ocr_button), TRUE);

  return FALSE;
}

// ========================================================================
// OCR worker thread
// ========================================================================

static gpointer ocr_thread(gpointer data)
{
  OcrData *d = (OcrData *) data;

  seg(d->file);
  launcher("../Treatment/.car");

  FILE *file = fopen("finalresult.txt", "r");

  if (!file)
  {
    g_idle_add(ocr_failed, NULL);

    g_free(d->file);
    g_free(d);

    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);

  char *content = malloc(size + 1);
  if (!content)
  {
    fclose(file);
    g_idle_add(ocr_failed, NULL);
  }

  else
  {
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);

    g_idle_add(update_text_view, content);
  }

  g_free(d->file);
  g_free(d);

  return NULL;
}

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
  gtk_widget_set_sensitive(GTK_WIDGET(ocr_button), FALSE);
  gtk_spinner_start(spinner);

  OcrData *d = g_malloc(sizeof(OcrData));
  d->file = g_strdup(selected_file);

  g_thread_new("ocr_thread", ocr_thread, d);
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

  window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  spinner = GTK_SPINNER(gtk_builder_get_object(builder, "spinner"));
  image_preview = GTK_IMAGE(gtk_builder_get_object(builder, "image_preview"));
  text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_view"));
  status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));
  open_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
  ocr_button = GTK_BUTTON(gtk_builder_get_object(builder, "ocr_button"));
  save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
  quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));

  g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), NULL);
  g_signal_connect(ocr_button, "clicked", G_CALLBACK(on_ocr_clicked), NULL);
  g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), NULL);
  g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_widget_show(GTK_WIDGET(spinner));
  gtk_main();

  g_free(selected_file);
  g_object_unref(builder);

  return 0;
}