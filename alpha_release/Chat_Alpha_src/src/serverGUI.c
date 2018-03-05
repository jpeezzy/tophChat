/* serverGUI.c
 * Author: Aung Thu
 * Date: 4 March 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <math.h>

const char *Program = "Toph Chat Server GUI";
int Shutdown = 0;

/*********** GUI functions **************/

static GtkTreeModel *fill_model_room(void)
{
    int i;
    GtkListStore *store;
    GtkTreeIter iter;
    
    char roomName[10];

    /* 2 colums, column 0 = string, column 1= string */
    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    
    /* Append a row and add in data */
    for(i = 0; i < 10; ++i)
    {
        sprintf(roomName, "Room %d", i);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 
                           0, roomName,
                           1, "Vacant",
                           -1);
    }                   
    
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_room(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    /* ----- Column #1 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Room List",
                                                renderer,
                                                "text", 0,
                                                NULL);

    /* ----- Column #2 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Status",
                                                renderer,
                                                "text", 1,
                                                NULL);

    model = fill_model_room();

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    /* The tree view has acquired its own reference to the model,
     * so we can drop ours. That way the model will be freed 
     * automatically when the tree view is destroyed */

    g_object_unref(model);

    return view; 
}

static GtkTreeModel *fill_model_user(void)
{
    int i;
    GtkListStore *store;
    GtkTreeIter iter;
    
    char username[10];

    /* 2 colums, column 0 = string, column 1= string */
    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    
    /* Append a row and add in data */
    for(i = 0; i < 15; ++i)
    {
        sprintf(username, "User %d", i);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 
                           0, username,
                           1, "Offline",
                           -1);
    }
                       
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_user(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    /* ----- Column #1 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "User List",
                                                renderer,
                                                "text", 0,
                                                NULL);

    /* ----- Column #2 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Status",
                                                renderer,
                                                "text", 1,
                                                NULL);

    model = fill_model_room();

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    /* The tree view has acquired its own reference to the model,
     * so we can drop ours. That way the model will be freed 
     * automatically when the tree view is destroyed */

    g_object_unref(model);

    return view; 
}

/* This callback quits the program */
static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    gtk_main_quit();
    return FALSE;
}

/* callback function for clicking shutdown button */
void ShutdownClicked(GtkWidget *Widget, gpointer Data)
{
#ifdef DEBUG
    printf("%s: Shutdown clicked callback starting...\n", Program);
#endif
    Shutdown = 1;
#ifdef DEBUG
    printf("%s: Shutdown clicked callback done.\n", Program);
#endif
} /* end of ShutdownClicked */

/* creates the server window */
GtkWidget *CreateWindow(int *argc,
                        char **argv[])
{
    GtkWidget *Window;
    GtkWidget *VBox, *HBox;
    GtkWidget *ClientList, *RoomList;
    GtkWidget *ShutdownButton;

    /* intitalize the GTK libraries */
    gtk_init(argc, argv);

    /* create the main, top level window */
    Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(Window), Program);   
    gtk_window_set_default_size(GTK_WINDOW(Window), 810, 500);
    gtk_container_set_border_width(GTK_CONTAINER(Window), 10);

    /* connect "delete-event" signal to main window */
    g_signal_connect(Window, "destroy",
                     G_CALLBACK(ShutdownClicked), NULL);

    /* overall vertical arrangement in the window */
    VBox = gtk_vbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(Window), VBox);    
    gtk_widget_show(VBox);    
 
    /* Horizontal box for Client List and Room List */
    HBox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_start((GtkBox *)VBox, HBox, TRUE, TRUE, 10);
    gtk_widget_show(HBox);    

    /* Client List */
    ClientList = view_model_user();
    gtk_box_pack_start((GtkBox *)HBox, ClientList, TRUE, TRUE, 0);
    gtk_widget_show(ClientList);
 
    /* Room List */
    RoomList = view_model_room();
    gtk_box_pack_start((GtkBox *)HBox, RoomList, TRUE, TRUE, 0);
    gtk_widget_show(RoomList);

    /* on the bottom, a button to shutdown the server and quit */
    ShutdownButton = gtk_button_new_with_label("Shutdown Server");
    gtk_box_pack_start((GtkBox *)VBox, ShutdownButton, FALSE, FALSE, 10);
    gtk_widget_show(ShutdownButton);    

    /* connect shutdown button with function terminating this server */
    g_signal_connect(ShutdownButton, "clicked", G_CALLBACK(ShutdownClicked), NULL);
    gtk_widget_show(Window);
    return(Window);
} /* end of CreateWindow */

/* render the window on screen */
void UpdateWindow(void)
{
    /* this server has it's own main loop for handling client connections;
     * as such, it can't have the usual GUI main loop (gtk_main);
     * instead, we call this UpdateWindow function on a regular basis
     */
    while(gtk_events_pending())
    {
        gtk_main_iteration();
    }
} /* end of UpdateWindow */

/*************** global functions ****************/

/* print error diagnostics and abort the program */
void FatalError(const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

int main(int argc, char* argv[])
{
    GtkWidget *Window; /* the server window */

#ifdef DEBUG
    printf("%s: Starting... \n", Program);
#endif
    
#ifdef DEBUG
    printf("%s: Creating the server window...\n", Program);
#endif
    Window = CreateWindow(&argc, &argv);
    if (!Window)
    {
        fprintf(stderr, "%s: cannot create GUI window\n", Program);
        exit(10);
    }
    
    /* server main loop */
    while(!Shutdown)
    {
        UpdateWindow();
    }
    return 0;
} /* end of main */

/* EOF serverGUI.c */
