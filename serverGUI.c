/* serverGUI.c
 * Author: Aung Thu, 3/3/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <math.h>

#define BUFFSIZE 256

/*************** type definitions ******************/

typedef void (*ClientHandler) (int DataSocketFD);
typedef void (*TimeoutHandler) (void);

enum
{
    COL_NAME = 0,
    COL_AGE,
    NUM_COLS
};

/************** global variables *****************/

const char *Program = "Toph Chat Server"; /* program name for descriptive diagnostics */
int Shutdown = 0; /* keep running until Shutdown == 1 */
char ClockBuffer[26] = ""; /* current time in printable format */
GtkLabel *DigitalClock = NULL;

/************* GUI functions ********************/

static GtkTreeModel *create_and_fill_model_room(void)
{
    GtkListStore *store;
    GtkTreeIter iter;
    
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
    
    /* Append a row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 
                        COL_NAME, "Room 1",
                        COL_AGE, "Occupied",
                        -1);

    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Room 2",
                        COL_AGE, "Occupied",
                        -1);
    
    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Room 3",
                        COL_AGE, "Vacant",
                        -1);

    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Room 4",
                        COL_AGE, "Vacant",
                        -1);
    
    
    return GTK_TREE_MODEL (store);
}

static GtkWidget *create_view_and_model_room(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;
        
    view = gtk_tree_view_new();

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Room List",
                                                renderer,
                                                "text", COL_NAME,
                                                NULL);

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Status",
                                                renderer,
                                                "text", COL_AGE,
                                                NULL);
    
    model = create_and_fill_model_room();
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    
    /* The tree view has acquired its own reference to the model,
     * so we can drop ours. That way the model will be freed
     * automatically when the tree view is destroyed */

    g_object_unref(model);
    
    return view;
}

static GtkTreeModel *create_and_fill_model_client(void)
{
    GtkListStore *store;
    GtkTreeIter iter;
    
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
    
    /* Append a row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 
                        COL_NAME, "Username 1",
                        COL_AGE, "In Room",
                        -1);

    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Username 2",
                        COL_AGE, "Away",
                        -1);
    
    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Username 3",
                        COL_AGE, "Online",
                        -1);

    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Username 4",
                        COL_AGE, "Idle",
                        -1);
    
    
    return GTK_TREE_MODEL (store);
}

static GtkWidget *create_view_and_model_client(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;
        
    view = gtk_tree_view_new();

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Client List",
                                                renderer,
                                                "text", COL_NAME,
                                                NULL);

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "Status",
                                                renderer,
                                                "text", COL_AGE,
                                                NULL);
    
    model = create_and_fill_model_client();
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    
    /* The tree view has acquired its own reference to the model,
     * so we can drop ours. That way the model will be freed
     * automatically when the tree view is destroyed */

    g_object_unref(model);
    
    return view;
}

/* callback fucntion for clicking shutdown button */
void ShutdownClicked(GtkWidget *Widget,
                     gpointer Data)
{
#ifdef DEBUG
    printf("%s: ShutdownClicked callback starting...\n", Program);
#endif
    Shutdown = 1;
#ifdef DEBUG
    printf("%s: ShutdownClicked callback done.\n", Program);
#endif
} /* end of ShutdownClicked */

/* creates the server window */
GtkWidget *CreateWindow(int *argc,
                        char **argv[])
{
    GtkWidget *Window;
    GtkWidget *VBox, *HBox;
    GtkWidget *ClientList, *RoomList;
    GtkWidget *Frame, *Label;
    GtkWidget *ShutdownButton;

    /* intitalize the GTK libraries */
    gtk_init(argc, argv);

    /* create the main, top level window */
    Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(Window), Program);   
    gtk_window_set_default_size(GTK_WINDOW(Window), 810, 500);
    gtk_container_set_border_width(GTK_CONTAINER(Window), 10);

    /* overall vertical arrangement in the window */
    VBox = gtk_vbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(Window), VBox);
    
    /* Horizontal box for Client List and Room List */
    HBox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_start((GtkBox *)VBox, HBox, TRUE, TRUE, 10);
    
    /* Client List */
    ClientList = create_view_and_model_client();
    gtk_box_pack_start((GtkBox *)HBox, ClientList, TRUE, TRUE, 0);
 
    /* Room List */
    RoomList = create_view_and_model_room();
    gtk_box_pack_start((GtkBox *)HBox, RoomList, TRUE, TRUE, 0);
    
    /* on the top, a frame with the digital display of the time */
    Frame = gtk_frame_new("Time and Date");
    gtk_box_pack_start((GtkBox *)VBox, Frame, FALSE, FALSE, 10);
    Label = gtk_label_new("n/a");
    gtk_container_add(GTK_CONTAINER(Frame), Label);

    /* on the bottom, a button to shutdown the server and quit */
    ShutdownButton = gtk_button_new_with_label("Shutdown Server");
    gtk_box_pack_start((GtkBox *)VBox, ShutdownButton, FALSE, FALSE, 10);
    
    /* make sure that everything becomes visible */
    gtk_widget_show_all(Window);
    
    /* make clock widgets public */
    DigitalClock = (GtkLabel*)Label;

    /* connect window-close with function terminating this server */
    g_signal_connect(Window, "destroy", G_CALLBACK(ShutdownClicked), NULL);
    
    /* connect shutdown button with function terminating this server */
    g_signal_connect(ShutdownButton, "clicked", G_CALLBACK(ShutdownClicked), NULL);

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

/* update and display the current real time */
void DisplayCurrentTime(void) 
{
    time_t CurrentTime;
    char *TimeString;
    
    CurrentTime = time(NULL);
    TimeString = ctime(&CurrentTime);
    strncpy(ClockBuffer, TimeString, 25);
    ClockBuffer[24] = 0;
#ifdef DEBUG
    printf("\r%s: current time is %s", Program, ClockBuffer);
    fflush(stdout);
#endif
    if (DigitalClock) /* if digital clock is functional */
    {
        gtk_label_set_label(DigitalClock, ClockBuffer); /* update it */
    }   
} /* end of DisplayCurrentTime */

/****************** global functions *******************/

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

/* create a socket on this server */
int MakeServerSocket(uint16_t PortNo)
{
    int ServSocketFD;
    struct sockaddr_in ServSocketName;
    
    /* create the socket */
    ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {
        FatalError("Server socket creation failed");
    }
    /* bind the socket to this server */
    ServSocketName.sin_family = AF_INET;
    ServSocketName.sin_port = htons(PortNo);
    ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName, sizeof(ServSocketName)) < 0)
    {
        FatalError("Binding the server to a socket failed");
    }
    /* start listening to this socket */
    if (listen(ServSocketFD, 5) < 0) /* max 5 clients in backlog */
    {
        FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */

/* process a time request by a client */
void ProcessRequest(int DataSocketFD)
{
    int l, n;
    char RecvBuf[256]; /* message buffer for receiving a message */
    char SendBuf[256]; /* message buffer for sending a message */
    
    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0)
    {
        FatalError("Reading from data socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
    if (0 == strcmp(RecvBuf, "TIME"))
    {
        strncpy(SendBuf, "OK TIME: ", sizeof(SendBuf)-1);
        SendBuf[sizeof(SendBuf)-1] = 0;
        strncat(SendBuf, ClockBuffer, sizeof(SendBuf)-1-strlen(SendBuf));
    }
    else if (0 == strcmp(RecvBuf, "SHUTDOWN"))
    {
        Shutdown = 1;
        strncpy(SendBuf, "OK SHUTDOWN", sizeof(SendBuf)-1);
        SendBuf[sizeof(SendBuf)-1] = 0;
    }
    else
    {
        strncpy(SendBuf, "ERROR unknown command ", sizeof(SendBuf)-1);
        SendBuf[sizeof(SendBuf)-1] = 0;
        strncat(SendBuf, RecvBuf, sizeof(SendBuf)-1-strlen(SendBuf));
    }
    l = strlen(SendBuf);
#ifdef DEBUG
    printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
    n = write(DataSocketFD, SendBuf, l);
    if (n < 0)
    {
        FatalError("Writing to data socket failed");
    }
} /* end of ProcessRequest */

/* simple server main loop */
void ServerMainLoop(int ServSocketFD, /* server socket to wait on */
                    ClientHandler HandleClient, /* client handler to call */
                    TimeoutHandler HandleTimeout, /* timeout handler to call */
                    int Timeout) /* timeout in micro seconds */
{
    int DataSocketFD; /* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in ClientAddress; /* client address we connect with */
    fd_set ActiveFDs; /* socket file descriptors to select from */
    fd_set ReadFDs; /* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;
    
    FD_ZERO(&ActiveFDs); /* set of active sockets */     
    FD_SET(ServSocketFD, &ActiveFDs); /* server socket is active */
    while(!Shutdown)
    {
        UpdateWindow(); /*update the GUI window */
        ReadFDs = ActiveFDs;
        TimeVal.tv_sec = Timeout / 1000000;
        TimeVal.tv_usec = Timeout % 1000000;
        /* block until input arrives on active sockets or until timeout */
        res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
        if (res < 0)
        {
            FatalError("Wait for input or timeout (select) failed");
        }
        if (res == 0) /* timeout occured */
        {
            HandleTimeout();
        } 
        else /* some FDs have data ready to read */      
        {
            for(i = 0; i < FD_SETSIZE; i++)
            {
                if (FD_ISSET(i, &ReadFDs))
                {
                    /* connection request on server socket */
                    if (i == ServSocketFD)
                    {
                    #ifdef DEBUG
                        printf("%s: Accepting new client...\n", Program);
                    #endif
                        ClientLen = sizeof(ClientAddress);
                        DataSocketFD = accept(ServSocketFD, 
                                              (struct sockaddr*)&ClientAddress, 
                                              &ClientLen);
                        if (DataSocketFD < 0)
                        {
                            FatalError("Data socket creation (accept) failed");
                        }
                    #ifdef DEBUG
                        printf("%s: New client connected from %s:%hu.\n", 
                                Program, inet_ntoa(ClientAddress.sin.addr), 
                                ntohs(ClientAddress.sin_port));
                    #endif
                        FD_SET(DataSocketFD, &ActiveFDs);
                    }
                    /* active communication with a client */
                    else
                    {
                    #ifdef DEBUG
                        printf("%s: Dealing with client on FD%d...\n", Program ,i);
                    #endif
                        HandleClient(i);
                    #ifdef DEBUG
                        printf("%s: Closing client connection FD%d.\n", Program, i);
                    #endif 
                        close(i);
                        FD_CLR(i, &ActiveFDs);
                    }
                }
            }
        }
    }
} /* end of ServerMainLoop */

/**************** main function *******************************/

int main(int argc, char *argv[])
{
    int ServSocketFD; /* socket file descriptor for service */
    int PortNo; /* port number */
    GtkWidget *Window; /* the server window */

#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s port\n", Program);
        exit(10);
    }
    PortNo = atoi(argv[1]); /*get the port number */
    if (PortNo <= 2000)
    {
        fprintf(stderr, "%s: invalid port number %d, should be >2000\n", Program, PortNo);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Creating the server socket...\n", Program);
#endif
    ServSocketFD = MakeServerSocket(PortNo);

#ifdef DEBUG
    printf("%s: Creating the server window...\n", Program);
#endif
    Window = CreateWindow(&argc, &argv);
    if (!Window)
    {
        fprintf(stderr, "%s: cannot create GUI window\n", Program);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Providing current time at port %d...\n", Program, PortNo);
#endif
    ServerMainLoop(ServSocketFD, ProcessRequest, DisplayCurrentTime, 250000);
#ifdef DEBUG
    printf("\n%s: Shutting down. \n", Program);
#endif
    close(ServSocketFD);
    return 0;
} /* end of main */

/* EOF serverGUI.c */
    

