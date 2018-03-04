/******************************************
 * Programmer: Jason Duong                *
 * Affiliated with: UCI                   *
 * Date Created: 2/28/18                  *
 * Last modification: 3/3/2018, 10:40 PM  *
 * ****************************************/

#include <stdio.h>
#include <gtk/gtk.h>

#include "GTK.h"


int main(int argc, char *argv[])
{

    /************************************/
    /*****   INITIALIZATION   **********/
    /**********************************/
    GtkWidget *window;  /* declaring a window */

    char name[50];
    
    g_print("Enter your name: ");
    scanf("%50s", &name); 

    gtk_init(&argc, &argv); /* initializing GTK environment */

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);   /* initialize window to be on top */

    gtk_window_set_title(GTK_WINDOW(window), "Toph Chat Alpha Version");  /* make title for window */ 
    gtk_container_set_border_width(GTK_CONTAINER(window), 0); /* sets the width from the inside */

    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);

    g_signal_connect(window, "delete-event", G_CALLBACK(CloseWindow), (gpointer) name);  /* deletes window */

    /**************************************************************************************************/ 

    /*******************************/
    /*****      VARIABLES     *****/
    /*****************************/

    /* Notebook Variables */
    GtkWidget *tabs; 
    tabs = gtk_notebook_new();  /* initializing tabs */

    GtkWidget *tabLabel;
    tabLabel = gtk_label_new("Chat 1"); /* initializing label */

    GtkWidget *tabLabel2;
    tabLabel2 = gtk_label_new("Chat 2");
    int success = 0;

 
    /* messageScreen Variables */    
    GtkWidget *messageScreen;           /* screen to display messages */
    messageScreen = gtk_text_view_new_with_buffer(NULL);    
    gtk_widget_set_size_request(messageScreen, 500, 300);   /* setting size */
    gtk_text_view_set_editable(GTK_TEXT_VIEW (messageScreen), FALSE);    /* turning of editability */
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW (messageScreen), FALSE); /* turn off cursor */

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (messageScreen)); 
    gtk_text_buffer_set_text(buffer, "Welcome to the chat room. Say hello!", -1); /* Welcome message */
    
    GtkWidget *messageScreen2;
    messageScreen2 = gtk_text_view_new_with_buffer(NULL);    
    gtk_widget_set_size_request(messageScreen2, 500, 300);   /* setting size */
    gtk_text_view_set_editable(GTK_TEXT_VIEW (messageScreen2), FALSE);    /* turning of editability */
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW (messageScreen2), FALSE); /* turn off cursor */

    GtkTextBuffer *buffer2;
    buffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW (messageScreen2)); 
    gtk_text_buffer_set_text(buffer2, "Welcome to the chat room. Say hello!", -1); /* Welcome message */
 

 
    /* textBox Variables */
    GtkWidget *textBox;
    textBox = gtk_entry_new();
    gtk_widget_set_size_request(textBox, 500, 50);    

    /* Alignment */
    GtkWidget *alignment;
    gtk_alignment_new(0,0,1,1);    

    /* Buttons */
    GtkWidget *sendButton;
    sendButton = gtk_button_new_with_label("SEND");
    gtk_widget_set_size_request(sendButton, 100, 50);
   
    GtkWidget *friends[10];
    for (int i = 0; i < 10; i++)
    {
        friends[i] = gtk_button_new_with_label("[Friend]");
        gtk_button_set_relief(GTK_BUTTON (friends[i]), GTK_RELIEF_NONE);
        gtk_widget_set_size_request(friends[i], 100, 30);
        gtk_widget_show(friends[i]);
    }
     
    /* Fixed Placements */
    GtkWidget *fixed;
    fixed = gtk_fixed_new();    /* creating "fixed" with no adjustments */
    
    /* Vertical Boxes */
    GtkWidget *vBox;
    vBox = gtk_vbox_new(FALSE, 0);  /* (same size?, spacing) */
    
    GtkWidget *friendsList;
    friendsList = gtk_vbox_new(TRUE, 2);
    
    /* Horizontal Boxes */
    GtkWidget *hBox;
    hBox = gtk_hbox_new(FALSE, 0);      

    GtkWidget *hBox2;
    hBox2 = gtk_hbox_new(FALSE, 20);

    /* Vertical Separator */
    GtkWidget *vLine;
    vLine = gtk_vseparator_new();
    
    GtkWidget *vLine2;
    vLine2 = gtk_vseparator_new();

    /* Frame */
    GtkWidget *frame;
    frame = gtk_frame_new("TOPH Chat"); /* add frame around */
    gtk_frame_set_label_align(GTK_FRAME (frame), .5, .5);   /* put label in center */   
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN); /* setting shadow type */

    GtkWidget *friendListFrame;
    friendListFrame = gtk_frame_new("");
    gtk_frame_set_label_align(GTK_FRAME (friendListFrame), .5, .5);
    gtk_container_set_border_width(GTK_CONTAINER(friendListFrame), 0);

    /* Option Menu */
    GtkWidget *options;
    options = gtk_menu_new();   /* Options menu */
    gtk_menu_set_title(GTK_MENU(options), "Options");
    
    GtkWidget *messageButton;   /* message button */
    messageButton = gtk_menu_item_new_with_label("Message");    
    
    GtkWidget *blockButton;     /* block button */
    blockButton = gtk_menu_item_new_with_label("Block");

    gtk_menu_attach(GTK_MENU (options), messageButton, 0,1,0,1);
    gtk_menu_attach(GTK_MENU (options), blockButton, 0,1,1,2);


    /* Expander */
    GtkWidget *expander;
    expander = gtk_expander_new("Friends List");

    /************************************************************************************************/

    /************************************/
    /****    MAIN PROGRAM    ***********/
    /**********************************/
 
    success = gtk_notebook_append_page(GTK_NOTEBOOK (tabs), messageScreen, tabLabel);  /* creating new notebook tab*/

    gtk_box_pack_start(GTK_BOX (hBox), textBox, FALSE, TRUE, FALSE);     /* HBOX textBox */
    gtk_box_pack_end(GTK_BOX (hBox), sendButton, FALSE, TRUE, FALSE);    /* HBOX textBox, sendButton */

    gtk_box_pack_start(GTK_BOX (vBox), tabs, TRUE, TRUE, FALSE);        /* VBOX tabs */
    gtk_box_pack_start(GTK_BOX (vBox), hBox, FALSE, TRUE, FALSE);        /* VBOX tabs, HBOX */

    gtk_box_pack_start(GTK_BOX (friendsList), friends[0], TRUE, TRUE, FALSE);   /* building friends list */
    for (int i = 1; i < 9; i++)
    {
        gtk_box_pack_end(GTK_BOX (friendsList), friends[i], TRUE, TRUE, FALSE);
    }
    gtk_container_add(GTK_CONTAINER (friendListFrame), friendsList); /* framing friends list */
    
    gtk_container_add(GTK_CONTAINER(expander), friendListFrame); /* putting friendsList into an expanding box */
    
    gtk_box_pack_start(GTK_BOX(hBox2), vBox, FALSE, FALSE, FALSE);                /* HBOX vBox */
    gtk_box_pack_end(GTK_BOX(hBox2), vLine, FALSE, TRUE, FALSE);                 /* HBOX vBox, vLine */    
    gtk_box_pack_start(GTK_BOX(hBox2), expander, FALSE, TRUE, FALSE);     /* HBOX vBox, vLine, friendslist */

    
    /* final packing */
    gtk_container_add(GTK_CONTAINER(frame), hBox2);

    gtk_fixed_put(GTK_FIXED (fixed), frame, 0, 0); /* placing container in top left corner */
    
    gtk_container_add(GTK_CONTAINER(window), fixed); /* adding container to window */



    /***** Widget Arrays *******/
    GtkWidget *messageWidgetArray[3];
    messageWidgetArray[1] = messageScreen;
    messageWidgetArray[2] = textBox;    

    GtkWidget *optionsArray[3];
    optionsArray[0] = options;
    optionsArray[1] = tabs;

    GtkWidget *tabsArray[3];
    tabsArray[0] = tabs;
    tabsArray[1] = messageScreen2;
    tabsArray[2] = tabLabel2;

    /**** SIGNALS ********/
    g_signal_connect(textBox, "activate", G_CALLBACK(EnterKey), messageScreen);             /* to send message with enter key */
    g_signal_connect(sendButton, "clicked", G_CALLBACK(SendButton),  messageWidgetArray);   /* to send message with "Send" button */
    g_signal_connect(messageButton, "activate", G_CALLBACK(MessageUser), tabsArray);        /* messaging a new user */
 
        /* option clicks for all of friends on list */
    g_signal_connect(friends[0], "clicked", G_CALLBACK(OptionsPopup), optionsArray); /* opens up options popup */ 
    g_signal_connect(friends[1], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[2], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[3], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[4], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[5], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[6], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[7], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[8], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  
    g_signal_connect(friends[9], "clicked", G_CALLBACK(OptionsPopup), optionsArray);  


    /****** SHOWING WIDGETS ******/
    gtk_widget_show(expander);
    gtk_widget_show(blockButton);
    gtk_widget_show(messageButton);
    gtk_widget_show(options);
    gtk_widget_show(friendListFrame);
    gtk_widget_show(frame);
    gtk_widget_show(vLine2);
    gtk_widget_show(friendsList);
    gtk_widget_show(vLine);
    gtk_widget_show(hBox2);
    gtk_widget_show(sendButton); 
    gtk_widget_show(messageScreen);   
    gtk_widget_show(textBox);
    gtk_widget_show(tabLabel);
    gtk_widget_show(tabs);
    gtk_widget_show(fixed);
    gtk_widget_show(vBox);
    gtk_widget_show(hBox);
    gtk_widget_show(window);

    gtk_main(); /* main event loop */
    
    return 0;
} 
    


 
 
 
