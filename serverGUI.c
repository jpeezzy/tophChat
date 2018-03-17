/* serverGUI.c
 * Author: Aung Thu
 * Date: 4 March 2018
 */
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <math.h>

#include "constants.h"
#include "protocol_const.h"
#include "tcpPacket.h"
#include "server_back_end.h"
#include "utils.h"
#include "protocol.h"
#include "fifo.h"
#include "tophChatUsers.h"


const char *Program = "Toph Chat Server";
int server_Shutdown = 0;

/*********** GUI functions **************/

static GtkTreeModel *store_model_room(serverRoomList *allRoom)
{
    assert(allRoom);
    int i;
    GtkListStore *store;
    GtkTreeIter iter;
    
    char roomName[10];

    /* 4 columns */
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT);
    
    /* Append a row and add in data */
    for(i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        sprintf(roomName, "Room %d", i);
        gtk_list_store_append(store, &iter);
        
        /* if the room is occupied */
        if (allRoom->roomList[i].isOccupied != ROOM_NOT_OCCUPIED)
        {
            gtk_list_store_set(store, &iter, 
                               0, roomName,
                               1, allRoom->roomList[i].adminID, 
                               2, "Occupied",
                               3, allRoom->roomList[i].peopleNum,
                               -1);
        }
        else
        {
            gtk_list_store_set(store, &iter, 
                               0, roomName,
                               1, allRoom->roomList[i].adminID, 
                               2, "Vacant",
                               3, allRoom->roomList[i].peopleNum,
                               -1);

        }
    }                   
    
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_room(serverRoomList *allRoom)
{
    assert(allRoom);
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
                                                "ID",
                                                renderer,
                                                "text", 1,
                                                NULL);
    /* ----- Column #3 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Status",
                                                renderer,
                                                "text", 2,
                                                NULL);

    /* ----- Column #4 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "No. of Users",
                                                renderer,
                                                "text", 3,
                                                NULL);


    model = store_model_room(allRoom);

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    /* The tree view has acquired its own reference to the model,
     * so we can drop ours. That way the model will be freed 
     * automatically when the tree view is destroyed */

    g_object_unref(model);

    return view; 
}

static GtkTreeModel *store_model_user(TINFO *userDatabase)
{
    assert(allOnline);
    int i;
    GtkListStore *store;
    GtkTreeIter iter;
    
    /* initialize columns with data types */
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);
    
    /* Append a row and add in data */
    for(i = 0; i < userDatabase->numOfUsers; ++i)
    {
        gtk_list_store_append(store, &iter);
        
        /* if user is available */
        if (userDatabase->Users[i]->socket >= 0)
        {
            gtk_list_store_set(store, &iter, 
                               0, userDatabase->Users[i]->userName,
                               1, "Online",
                               2, userDatabase->Users[i]->socket,
                               3, userDatabase->Users[i]->friendCount,
                              -1);
        }
        else
        {
            gtk_list_store_set(store, &iter, 
                               0, userDatabase->Users[i]->userName,
                               1, "Offline",
                               2, userDatabase->Users[i]->socket,
                               3, userDatabase->Users[i]->friendCount,
                              -1);
        }
    }
                       
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_user(TINFO *userDatabase)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    /* ----- Column #1 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Username",
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

    /* ----- Column #3 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Socket No.",
                                                renderer,
                                                "text", 2,
                                                NULL);

    /* ----- Column #4 ----- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "No. of Friends",
                                                renderer,
                                                "text", 3,
                                                NULL);

    model = store_model_user(userDatabase);

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
    server_Shutdown = 1;
#ifdef DEBUG
    printf("%s: Shutdown clicked callback done.\n", Program);
#endif
} /* end of ShutdownClicked */

/* creates the server window */
GtkWidget *CreateWindow(int *argc,
                        char **argv[],
                        serverRoomList *allRoom,
                        onlineUserList *allOnline)
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
    gtk_box_pack_start(GTK_BOX(VBox), HBox, TRUE, TRUE, 10);
    gtk_widget_show(HBox);    

    /* Client List */
    ClientList = view_model_user(allOnline);
    gtk_box_pack_start(GTK_BOX(HBox), ClientList, TRUE, TRUE, 0);
    gtk_widget_show(ClientList);
 
    /* Room List */
    RoomList = view_model_room(allRoom);
    gtk_box_pack_start(GTK_BOX(HBox), RoomList, TRUE, TRUE, 0);
    gtk_widget_show(RoomList);

    /* on the bottom, a button to shutdown the server and quit */
    ShutdownButton = gtk_button_new_with_label("Shutdown Server");
    gtk_box_pack_start(GTK_BOX(VBox), ShutdownButton, FALSE, FALSE, 10);
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
    int incomingSocket = -1;
    struct sockaddr addrDummy;
    socklen_t socklenDummy;
    struct timeval timeout;
    timeout.tv_sec = 2;
    fd_set setListener;
    FD_ZERO(&setListener);

    int socketListener = listenSocketInit();
    assert(socketListener >= 0);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";
    FD_SET(socketListener, &setListener);
    int j = 0;
    int res;

    TINFO *dataBase = createTINFO();
    serverRoomList *roomList = serverRoomSetCreate();
    struct messageServerRoom *testRoom = serverRoomCreate(roomList);
    onlineUserList *userList = serverCreateOnlineList();

    char *userName[] = {"USER", "ADMIN"};
    addUser(userName[0], userName[0], 213123, dataBase);
    addUser(userName[1], userName[1], 213123, dataBase);

    TUSER *userProfile0 = findUserByName(userName[0], dataBase);
    TUSER *userProfile1 = findUserByName(userName[1], dataBase);

    addFriend(userName[0], userProfile1, dataBase);
    
    GtkWidget *Window; /* the server window */

#ifdef DEBUG
    printf("%s: Starting... \n", Program);
#endif
    
#ifdef DEBUG
    printf("%s: Creating the server window...\n", Program);
#endif
    Window = CreateWindow(&argc, &argv, roomList, userList);
    if (!Window)
    {
        fprintf(stderr, "%s: cannot create GUI window\n", Program);
        exit(10);
    }
    
    /* server main loop */
    while(!server_Shutdown)
    {
       // UpdateWindow();
        FD_ZERO(&setListener);
        FD_SET(socketListener, &setListener);
        if (j < 2)
        {
            res = select(socketListener + 1, &setListener, NULL, NULL, &timeout);
            if (res > 0)
            {
                if (((incomingSocket = accept(socketListener, &addrDummy, &socklenDummy)) >= 0))
                {
#ifdef DEBUG
                    printf("\nconnection received the socket is %d\n", incomingSocket);
#endif
                    ++(userList->totalOnlineUser);
                    if (serverAddOnlineUser(userName[j], userList, incomingSocket, dataBase) == NULL)
                    {
                        perror("\ncan't add user to server\n");
                    }
                    ++j;
                }
                else
                {
#ifdef DEBUG
                    printf("\nfailed to acceept socket\n");
#endif
                }
            }
        }
        if (j == 2)
        {
            // #ifdef DEBUG
            // printf("\nreceived both\n");
            // fflush(stdout);
            // #endif
            if (triagePacket(userList, roomList, dataBase) == 2)
            {
#ifdef DEBUG
                printf("\naccessing message\n");
#endif
                readBuffer(testRoom->inMessage, packet);
                getMessageBody(packet, message);
                printf("received message: %s\n", message);
                serverRoomSpreadMessage(testRoom, dataBase);
            }
        }
        UpdateWindow();
    }
    return 0;
} /* end of main */

/* EOF serverGUI.c */
