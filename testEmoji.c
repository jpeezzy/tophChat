#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <assert.h>

GtkWidget *view;

/*
static gboolean button_press_callback (GtkWidget *event_box,
                                       GdkEventButton *event,
                                       char* filename)
{
    GtkTextMark *cursor_pos;
    GtkTextIter iter;
    GtkImage *image;
    GtkTextChildAnchor *anchor;
    GtkTextBuffer *buffer;
   
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    cursor_pos = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, cursor_pos);
    image = gtk_image_new_from_file(filename);
    anchor = gtk_text_buffer_create_child_anchor(buffer, &iter);
    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(view), GTK_WIDGET(image), anchor);
    gtk_widget_show(GTK_WIDGET(image));
    
    return TRUE;
}
*/

static gboolean button_press_callback (GtkWidget *event_box,
                                       GdkEventButton *event,
                                       char* filename)
{   
    GtkTextBuffer *buffer;
    GtkTextMark *selection_bound;

    char emoji_name[10] = ":";
    strncat(emoji_name, filename, 8);
    strcat(emoji_name, ":");
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    /* checks if text is selected */
    if (gtk_text_buffer_get_has_selection(buffer) == TRUE)
    {
        gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);   
    }
    gtk_text_buffer_insert_at_cursor(buffer, emoji_name, 10);

    return TRUE;
}


GtkWidget* createClickableImage(char* filename)
{
    GtkImage *image;
    GtkWidget *emojiButton;
    
    image = gtk_image_new_from_file(filename);   
    emojiButton = gtk_button_new();
    
    gtk_button_set_image(emojiButton, image);
    gtk_button_set_relief(GTK_BUTTON(emojiButton), GTK_RELIEF_NONE);
    g_signal_connect(G_OBJECT(emojiButton), "button_press_event",
                     G_CALLBACK (button_press_callback), filename);
    return emojiButton;
}

int main( int argc, char *argv[] )
{
    GtkWidget *window;
    GtkWidget *VBox, *Label, *HBox;
    GtkWidget *emoji1, *emoji2, *emoji3;    
    GtkWidget *scrolled_window;    
    GtkTextBuffer *buffer;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Emoji Test");
    
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 240);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    g_signal_connect(G_OBJECT(window), "destroy", 
                     G_CALLBACK(gtk_main_quit), NULL);
    
    VBox = gtk_vbox_new(TRUE, 10);    
    gtk_container_add(GTK_CONTAINER(window), VBox);
    
    Label = gtk_label_new("Insert images in GtkTextView.\n Click on the Emojis to insert the simleys");
    gtk_box_pack_start(GTK_BOX(VBox), Label, FALSE, FALSE, 10);
    
    view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), TRUE);    

    HBox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(VBox), HBox, FALSE, FALSE, 0);
       
    emoji1 = createClickableImage("smile___.gif");
    emoji2 = createClickableImage("lol_____.gif");
    emoji3 = createClickableImage("cool____.gif");
    gtk_box_pack_start(GTK_BOX(HBox), emoji1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(HBox), emoji2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(HBox), emoji3, FALSE, FALSE, 0);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(VBox), scrolled_window, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), view);

   
    gtk_widget_show_all(window);
    gtk_main();   
    return 0;
}
