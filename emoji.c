/* Author: Aung Thu 
 * Date: 10 Mar 2018
 * Emoji functions for GUI */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "emoji.h"

#define EMOJI_NAME_LEN 8
#define NUM_OF_EMOJIS 3

EMOJI_STRUCT* create_emoji_struct(char* filename, GtkTextView *view)
{
    EMOJI_STRUCT* emoji_struct = NULL;
    emoji_struct = malloc(sizeof(EMOJI_STRUCT));
    if(emoji_struct == NULL)
    {
        perror("Out of memory aborting...");
        exit(10);
    }
    emoji_struct->filename = filename;
    emoji_struct->view = view;
    return emoji_struct;
}

void delete_emoji_struct(EMOJI_STRUCT *emoji_struct)
{
    assert(emoji_struct);
    free(emoji_struct);
}

static gboolean is_colon (gunichar ch, gpointer user_data)
{
    if (ch == ':')
    {
        return TRUE;
    }    
    return FALSE;
}

static gboolean emoji_button_press_callback (GtkWidget *button,
                                             gpointer user_data)
{   
    assert(user_data);
    GtkTextBuffer *buffer;
    GtkTextMark *selection_bound;
    GtkTextView *view;
    EMOJI_STRUCT *emoji_struct = (EMOJI_STRUCT*) user_data;
    
    char emoji_name[10] = ":";
    strncat(emoji_name, emoji_struct->filename, 8);
    strcat(emoji_name, ":");
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(emoji_struct->view));
    /* checks if text is selected */
    if (gtk_text_buffer_get_has_selection(buffer) == TRUE)
    {
        gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);   
    }
    gtk_text_buffer_insert_at_cursor(buffer, emoji_name, 10);

    return TRUE;
}

static GtkWidget* createEmojiButton(char* filename)
{
    GtkImage *image;
    GtkWidget *emojiButton;
    
    image = gtk_image_new_from_file(filename);   
    emojiButton = gtk_button_new();
       
    gtk_button_set_image(GTK_BUTTON(emojiButton), image);
    gtk_button_set_relief(GTK_BUTTON(emojiButton), GTK_RELIEF_NONE);
    return emojiButton;
}

static void emoji_popup_callback(GtkWidget *button,
                                 GtkWidget *menu)
{
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1, 
                   gtk_get_current_event_time());
}

GtkWidget* emoji_popup(GtkTextView *view)
{
    GtkWidget *emoji_popup_button, *emoji_popup_menu;
    GtkImage *thumbnail;    
    GtkWidget *emoji1, *emoji2, *emoji3;
    GtkWidget *emoji1_item, *emoji2_item, *emoji3_item;
    EMOJI_STRUCT *emoji_struct;
    char* filename;

    emoji_popup_button = gtk_button_new();
    thumbnail = gtk_image_new_from_file("thumbnail.gif");

    gtk_button_set_image(emoji_popup_button, thumbnail);
    gtk_button_set_relief(GTK_BUTTON(emoji_popup_button), GTK_RELIEF_NONE);
    
    filename = "ditto___.gif";
    emoji1 = createEmojiButton(filename);
    emoji1_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji1_item), emoji1);
 
    gtk_widget_show(emoji1);
    gtk_widget_show(emoji1_item);
   
      emoji_struct = create_emoji_struct(filename, view);
    g_signal_connect(emoji1_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer *)emoji_struct);    

    filename = "lol_____.gif";
    emoji2 = createEmojiButton(filename);
    emoji2_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji2_item), emoji2);
    
    gtk_widget_show(emoji2);
    gtk_widget_show(emoji2_item);
   
    emoji_struct = create_emoji_struct(filename, view);
    g_signal_connect(emoji2_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer *)emoji_struct);    

    emoji_popup_menu = gtk_menu_new();
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji1_item, 0, 1, 0, 1);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji2_item, 1, 2, 0, 1);

    gtk_widget_show(emoji_popup_menu);
    g_signal_connect(emoji_popup_button, "clicked", G_CALLBACK(emoji_popup_callback), emoji_popup_menu);

    return emoji_popup_button;      
}

void insert_emoji (GtkTextView *view, GtkTextMark *start)
{
    GtkTextIter start_tag, end_tag;
    gboolean found;
    gunichar colon;
    char *emoji_name;
    GtkImage *emoji;    
    GtkTextChildAnchor *anchor;
    GtkTextBuffer *buffer;
            
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_get_iter_at_mark(buffer, &start_tag, start);
    gtk_text_iter_backward_char(&start_tag);
    found = gtk_text_iter_forward_find_char (&start_tag,
                                             (GtkTextCharPredicate) is_colon, 
                                             NULL, NULL); 

    while (found != FALSE)
    {
        end_tag = start_tag;
        gtk_text_iter_forward_chars(&end_tag, EMOJI_NAME_LEN + 1);
        if (gtk_text_iter_is_start(&end_tag) == TRUE)
        {
            break;
        }
        else
        {
            colon = gtk_text_iter_get_char(&end_tag);              
            if (colon == ':')
            {
                gtk_text_iter_forward_char(&start_tag);
                emoji_name = gtk_text_buffer_get_text (buffer, &start_tag,
                                                       &end_tag, FALSE);               
                strcat(emoji_name, ".gif");
                /* if file exists */
                if( access( emoji_name, F_OK ) != -1 )
                {
                    gtk_text_iter_backward_char(&start_tag);
                    gtk_text_iter_forward_char(&end_tag);
                    gtk_text_buffer_delete (buffer, &start_tag, &end_tag); 
                    emoji = gtk_image_new_from_file(emoji_name);
                    anchor = gtk_text_buffer_create_child_anchor(buffer, &start_tag);
                    gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(view),
                                                      GTK_WIDGET(emoji),
                                                      anchor);
                    gtk_widget_show(GTK_WIDGET(emoji));
                    gtk_text_iter_backward_char(&start_tag); 
                    found = gtk_text_iter_forward_find_char (&start_tag,
                                                             (GtkTextCharPredicate) is_colon, 
                                                             NULL, NULL); 
                }
                /* file doesn't exist */
                else
                {
                    gtk_text_iter_backward_char(&start_tag);
                    found = gtk_text_iter_forward_find_char (&start_tag,
                                                             (GtkTextCharPredicate) is_colon, 
                                                             NULL, NULL);
                }
            }
            else
            {
                found = gtk_text_iter_forward_find_char (&start_tag,
                                                         (GtkTextCharPredicate) is_colon, 
                                                         NULL, NULL);
            }       
        }
    }       
}

