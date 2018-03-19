/* Author: Aung Thu 
 * Date: 10 Mar 2018
 * Emoji functions for GUI */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "emoji.h"

#define EMOJI_NAME_LEN 8

EMOJI_STRUCT* create_emoji_struct(char* filename, GtkEntry *entry)
{
    EMOJI_STRUCT* emoji_struct = NULL;
    emoji_struct = malloc(sizeof(EMOJI_STRUCT));
    if(emoji_struct == NULL)
    {
        perror("Out of memory aborting...");
        exit(10);
    }
    strncpy(emoji_struct->filename, filename, 10);
    emoji_struct->filename[10] = '\0';
    emoji_struct->entry = entry;
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
    GtkEntry *textBox;
    EMOJI_STRUCT *emoji_struct = (EMOJI_STRUCT*) user_data;
    
    char emoji_name[10] = ":";
    strncat(emoji_name, emoji_struct->filename, 8);
    strcat(emoji_name, ":");
    buffer = gtk_entry_get_buffer(GTK_ENTRY(emoji_struct->entry));
    /* checks if text is selected */
    /* if (gtk_text_buffer_get_has_selection(buffer) == TRUE)
    {
        gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);   
    } */
    gtk_entry_buffer_insert_text(buffer, "cursor-position", emoji_name, 10);

    return TRUE;
}

static GtkWidget* createEmojiButton(char* filename)
{
    GtkImage *image;
    GtkWidget *emojiButton;
    
    image = GTK_IMAGE(gtk_image_new_from_file(filename));   
    emojiButton = gtk_button_new();
       
    gtk_button_set_image(GTK_BUTTON(emojiButton), GTK_WIDGET(image));
    gtk_button_set_relief(GTK_BUTTON(emojiButton), GTK_RELIEF_NONE);
    return emojiButton;
}

static void emoji_popup_callback(GtkWidget *button,
                                 GtkWidget *menu)
{
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1, 
                   gtk_get_current_event_time());
}

GtkWidget* emoji_popup(GtkEntry *textBox)
{
    GtkWidget *emoji_popup_button, *emoji_popup_menu;
    GtkImage *thumbnail;    
    GtkWidget *emoji1, *emoji2, *emoji3;
    GtkWidget *emoji4, *emoji5, *emoji6;
    GtkWidget *emoji7, *emoji8, *emoji9;

    GtkWidget *emoji1_item, *emoji2_item, *emoji3_item;
    GtkWidget *emoji4_item, *emoji5_item, *emoji6_item;
    GtkWidget *emoji7_item, *emoji8_item, *emoji9_item;

    EMOJI_STRUCT *emoji_struct;

    emoji_popup_button = gtk_button_new();
    thumbnail = GTK_IMAGE(gtk_image_new_from_file("thumbnail.gif"));

    gtk_button_set_image(GTK_BUTTON(emoji_popup_button), GTK_WIDGET(thumbnail));
    gtk_button_set_relief(GTK_BUTTON(emoji_popup_button), GTK_RELIEF_NONE);
    
    emoji_struct = create_emoji_struct("PogChamp.gif", textBox);
    emoji1 = createEmojiButton("PogChamp.gif");
    emoji1_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji1_item), emoji1);
 
    gtk_widget_show(emoji1);
    gtk_widget_show(emoji1_item);
   
    g_signal_connect(emoji1_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    
    
    emoji_struct = create_emoji_struct("OMEGALUL.gif", textBox);
    emoji2 = createEmojiButton("OMEGALUL.gif");
    emoji2_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji2_item), emoji2);
    
    gtk_widget_show(emoji2);
    gtk_widget_show(emoji2_item);
   
    g_signal_connect(emoji2_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    
    
    emoji_struct = create_emoji_struct("Kappa___.gif", textBox);
    emoji3 = createEmojiButton("Kappa___.gif");
    emoji3_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji3_item), emoji3);
    
    gtk_widget_show(emoji3);
    gtk_widget_show(emoji3_item);
   
    g_signal_connect(emoji3_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    

    emoji_struct = create_emoji_struct("KapPride.gif", textBox);
    emoji4 = createEmojiButton("KapPride.gif");
    emoji4_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji4_item), emoji4);
    
    gtk_widget_show(emoji4);
    gtk_widget_show(emoji4_item);
   
    g_signal_connect(emoji4_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    

    emoji_struct = create_emoji_struct("pepShrug.gif", textBox);
    emoji5 = createEmojiButton("pepShrug.gif");
    emoji5_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji5_item), emoji5);
    
    gtk_widget_show(emoji5);
    gtk_widget_show(emoji5_item);
   
    g_signal_connect(emoji5_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    

    emoji_struct = create_emoji_struct("rareDan_.gif", textBox);
    emoji6 = createEmojiButton("rareDan_.gif");
    emoji6_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji6_item), emoji6);
    
    gtk_widget_show(emoji6);
    gtk_widget_show(emoji6_item);
   
    g_signal_connect(emoji6_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    
    
    emoji_struct = create_emoji_struct("monkaS__.gif", textBox);
    emoji7 = createEmojiButton("monkaS__.gif");
    emoji7_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji7_item), emoji7);
    
    gtk_widget_show(emoji7);
    gtk_widget_show(emoji7_item);
   
    g_signal_connect(emoji7_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    
    
    emoji_struct = create_emoji_struct("POGGERS_.gif", textBox);
    emoji8 = createEmojiButton("POGGERS_.gif");
    emoji8_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji8_item), emoji8);
    
    gtk_widget_show(emoji8);
    gtk_widget_show(emoji8_item);
   
    g_signal_connect(emoji8_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    

    emoji_struct = create_emoji_struct("ClapClap.gif", textBox);
    emoji9 = createEmojiButton("ClapClap.gif");
    emoji9_item = gtk_menu_item_new();
    gtk_container_add(GTK_CONTAINER(emoji9_item), emoji9);
    
    gtk_widget_show(emoji9);
    gtk_widget_show(emoji9_item);
   
    g_signal_connect(emoji9_item, "activate",
                     G_CALLBACK (emoji_button_press_callback), 
                     (gpointer)emoji_struct);    

    emoji_popup_menu = gtk_menu_new();
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji1_item, 0, 1, 0, 1);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji2_item, 1, 2, 0, 1);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji3_item, 2, 3, 0, 1);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji4_item, 0, 1, 1, 2);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji5_item, 1, 2, 1, 2);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji6_item, 2, 3, 1, 2);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji7_item, 0, 1, 2, 3);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji8_item, 1, 2, 2, 3);
    gtk_menu_attach(GTK_MENU(emoji_popup_menu), emoji9_item, 2, 3, 2, 3);

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
                    emoji = GTK_IMAGE(gtk_image_new_from_file(emoji_name));
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
