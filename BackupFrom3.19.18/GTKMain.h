#ifndef _GTKMAIN_H_
#define _GTKMAIN_H_

#include <gtk/gtk.h>

#include "fifo.h"
#include "constants.h"
#include "protocol_const.h"
#include "tcpGUI.h"
#include "utils.h"
#include "protocol.h"
#include "emoji.h"
#include "userInboxParsing.h"

typedef struct MessageStruct MESSAGE_STRUCT;
/* message struct to hold server and GUI input stuff */
struct MessageStruct
{
    GtkWidget *widget;
    GtkWidget *window;
    serverConnection *server;
    struct allRoom *Allroom;
    fifo *outputFIFO;
    inboxQueue *inbox;
    char username[MAX_USER_NAME];
    char targetName[MAX_USER_NAME];
    char message[MESS_LIMIT];
    int roomNumber;
};

MESSAGE_STRUCT *CreateMessageStruct(GtkWidget *widget, GtkWidget *window, serverConnection *server, struct allRoom *Allroom, fifo *outputFIFO, inboxQueue *inbox, char *username, char *message);
gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data);                                                               /* Exits out of the window and the program */
gboolean LoginExit(GtkWidget *widget, GdkEvent *event, gpointer data);                                                                 /* Exits out of the window and the program */
void EnterKey(GtkWidget *entry, gpointer messageStruct);                                                                               /* when the user presses enter in the textBox */
void SendButton(GtkWidget *widget, gpointer messageStruct);                                                                            /* when user presses send button */
void OptionsPopup(GtkWidget *button, GtkWidget *options[]);                                                                            /* option box pops up when friend is clicked on */
void MessageUser(GtkWidget *widget, GtkWidget *tabCreation[]);                                                                         /* when "message" button pressed. creates new tab for the friend" */
void BlockUser(GtkWidget *widget, GtkWidget *tabCreation[]);                                                                           /* blocks the user selected */
void SetMessageScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen);                        /* setting up message screens */
void SetWelcomeScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen, GtkTextBuffer *buffer); /* setting up welcome screens */
void Login(GtkWidget *widget, gpointer messageStructArray[]);                                                                          /* logs the user into the hat application */
int CheckNotebook(GtkWidget *tabs, GtkWidget *tabLabel);                                                                               /* checks through the notebook's tabs for matching tab names */
gboolean HideCharacters(GtkWidget *widget, GdkEvent *event, GtkWidget *button);                                                        /* turns on character hiding for password typing */
gboolean Overwrite(GtkWidget *username, GtkWidget *entry);                                                                             /* overwrites the entry buffer in username box */
void ShowCharacters(GtkWidget *button, GtkWidget *vBox);                                                                               /* shows password characters if button is toggled */
void ClearForm(GtkWidget *button, GtkWidget *vBox);                                                                                    /* clears the forms */
void CreateAccount(GtkWidget *button, GtkWidget *screen);                                                                              /* create a new account */
void AcceptMessage(GtkWidget *button, GtkWidget *window);
void Hide(GtkWidget *widget, GdkEvent *event, gpointer data);
static GtkTreeModel *store_online_friends(char **friendList);
static GtkWidget *view_model_online_friends(void);
static void update_online_friend_model(GtkWidget *view, char **friendList);
static GtkWidget *create_online_friend_list(MESSAGE_STRUCT *messageStruct);
static GtkTreeModel *store_offline_friends(char **friendList);
static GtkWidget *view_model_offline_friends(void);
static void update_offline_friend_model(GtkWidget *view, char **friendList);
static GtkWidget *create_offline_friend_list(MESSAGE_STRUCT *messageStruct);
void friend_list_clicked(GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void friend_invite_clicked(GtkButton *button, MESSAGE_STRUCT *messageStruct);
void inviteClicked(GtkButton *button, MESSAGE_STRUCT *messageStruct);
void popupExit(GtkButton *button, GtkWidget *user_data);
void CreateNewTab(GtkButton *button, MESSAGE_STRUCT *messageStruct);
void CreateNewAccount(GtkWidget *button, MESSAGE_STRUCT *messageStruct);
void Popup(char* message, char noti_type, int noti_ID, int roomnum, char *targetName, MESSAGE_STRUCT *messageStruct);
void AcceptFriendRequest(GtkWidget *button, MESSAGE_STRUCT *messageStruct);
void RejectFriendRequest(GtkWidget *button, MESSAGE_STRUCT *messageStruct);
void AcceptRoom(GtkWidget *button, MESSAGE_STRUCT *messageStruct);
void RejectRoom(GtkWidget *button, MESSAGE_STRUCT *messageStruct);
void popup_with_label(char *label);

#endif
