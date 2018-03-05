#include <gtk/gtk.h>
#include "GTK.h"
#include <assert.h>

gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("You exited out the window! \n");
    g_print("See ya later, %s!\n", (gchar *) data);
    gtk_main_quit();    /* leaves event loop */
    return FALSE;       /* return false to destroy window */
}


void click(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("You've clicked the button! \n");
}

GtkWidget *CreateBox(GtkWidget *window, GtkWidget *button) 
{
    GtkWidget *box;

    box = gtk_hbox_new(TRUE, 0);

    button = gtk_button_new_with_label("hello");

    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), box);
    
    return box;
}

void EnterKey(GtkWidget *entry, GtkWidget *messageScreen)
{
    GtkTextIter iter;
    guint16 check = 0;
    check = gtk_entry_get_text_length(GTK_ENTRY(entry));    /* checking length of text input */

    if (check != 0) /* only run this if there is a text input */
    {  
        GtkTextBuffer *buffer;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (messageScreen)); /* gets the buffer for the current screen */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);  /* get mark at the end */
       
        gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);      /* insert new lines */
        
        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);  /* get mark at end again */
 
        gtk_text_buffer_insert(buffer, &iter, gtk_entry_get_text(GTK_ENTRY(entry)), -1);   /* inserts user text */
 
        gtk_entry_set_text(GTK_ENTRY(entry), "");  /* replaces textBox with empty text again */
    }
}
void SendButton(GtkWidget *widget, GtkWidget *widgets[])   
{
    GtkTextIter iter;
    guint16 check = 0;
    check = gtk_entry_get_text_length(GTK_ENTRY(widgets[2]));    /* checking length of text input */

    if (check != 0) /* only run this if there is a text input */
    {    
        GtkTextBuffer *buffer;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgets[1])); /* gets the buffer for the current screen */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);  /* get mark at the end */
       
        gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);      /* insert new lines */
        
        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);  /* get mark at end again */
    
        gtk_text_buffer_insert(buffer, &iter, gtk_entry_get_text(GTK_ENTRY(widgets[2])), -1);   /* inserts user text */
 
        gtk_entry_set_text(GTK_ENTRY(widgets[2]), "");  /* replaces textBox with empty text again */
    }
}

void OptionsPopup(GtkWidget *button, GtkWidget *options[])
{
    gtk_menu_popup(GTK_MENU (options[0]), NULL, NULL, NULL, NULL, 1, gtk_get_current_event_time()); /* option menu pops up */
}

void MessageUser(GtkWidget *widget, GtkWidget *tabCreation[])
{
    int success = 0;

    success = gtk_notebook_append_page(GTK_NOTEBOOK (tabCreation[0]), tabCreation[1], tabCreation[2]);  /* creating new notebook */

    for (int i = 0; i < 3; i++) 
    {
        gtk_widget_show(tabCreation[i]);    /* showing all the newly created widgets */
    }
    gtk_notebook_next_page(GTK_NOTEBOOK(tabCreation[0]));        
}

