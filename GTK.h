#ifndef GTK
#define GTK
#include <gtk/gtk.h>
#include "tcpGUI.h"
#include "fifo.h"
#include "constants.h"
#include "protocol_const.h"

typedef struct MessageStruct MESSAGE_STRUCT;

/* message struct to hold server and GUI input stuff */
struct MessageStruct
{
    GtkWidget *widget;
    serverConnection *server;
    struct allRoom *Allroom;
    fifo *outputFIFO;
    inboxQueue *inbox;
};

gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data);        /* Exits out of the window and the program */
gboolean LoginExit(GtkWidget *widget, GdkEvent *event, gpointer data); /* Exits out of the window and the program */
//void click(GtkWidget *widget, GdkEvent *event, gpointer data);
//GtkWidget *CreateBox(GtkWidget *window, GtkWidget *button);
void EnterKey(GtkWidget *entry, gpointer messageStruct);                                                                                      /* when the user presses enter in the textBox */
void SendButton(GtkWidget *widget, GtkWidget *vBox);                                                                                   /* when user presses send button */
void OptionsPopup(GtkWidget *button, GtkWidget *options[]);                                                                            /* option box pops up when friend is clicked on */
void MessageUser(GtkWidget *widget, GtkWidget *tabCreation[]);                                                                         /* when "message" button pressed. creates new tab for the friend" */
void BlockUser(GtkWidget *widget, GtkWidget *tabCreation[]);                                                                           /* blocks the user selected */
void SetMessageScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen);                        /* setting up message screens */
void SetWelcomeScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen, GtkTextBuffer *buffer); /* setting up welcome screens */
void Login(GtkWidget *widget, GtkWidget *vBox[]);                                                                                      /* logs the user into the hat application */
int CheckNotebook(GtkWidget *tabs, GtkWidget *tabLabel);                                                                               /* checks through the notebook's tabs for matching tab names */
gboolean HideCharacters(GtkWidget *widget, GdkEvent *event, GtkWidget *button);                                                        /* turns on character hiding for password typing */
gboolean Overwrite(GtkWidget *username, GtkWidget *entry);                                                                             /* overwrites the entry buffer in username box */
void ShowCharacters(GtkWidget *button, GtkWidget *vBox);                                                                               /* shows password characters if button is toggled */
void ClearForm(GtkWidget *button, GtkWidget *vBox);                                                                                    /* clears the forms */
void CreateAccount(GtkWidget *button, GtkWidget *screen);                                                                              /* create a new account */
void AcceptMessage(GtkWidget *button, GtkWidget *window);

#endif
