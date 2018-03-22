/******************************************
 * Programmer: Jason Duong                *
 * Affiliated with: UCI                   *
 * Date Created: 2/28/18                  *
 * Last modification: 3/11/2018, 3:51 AM  *
 * ****************************************/

#include <stdio.h>
#include <gtk/gtk.h>

#include "fifo.h"
#include "constants.h"
#include "protocol_const.h"
#include "tcpGUI.h"
#include "utils.h"
#include "protocol.h"
#include "emoji.h"
#include "userInboxParsing.h"
#include "GTKMain.h"

int client_shutdown = 0;
int isLogin = 0;

void Popup(char* message, char noti_type, int noti_ID, int roomnum, char *targetName, MESSAGE_STRUCT *messageStruct)
{
    
    GtkWidget *messagePopupScreen;
    messagePopupScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(messagePopupScreen), "New Notification!"); /* sets title of window */
    gtk_widget_set_size_request(messagePopupScreen, 270, 100);
    gtk_container_set_border_width(GTK_CONTAINER(messagePopupScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(messagePopupScreen), FALSE);

    /* labels */
    GtkWidget *newMessage;
    newMessage = gtk_label_new(message);
    
    /* buttons */
    GtkWidget *reject;
    reject = gtk_button_new_with_label("Reject");
    gtk_widget_set_size_request(reject, 70, 30);

    GtkWidget *accept;
    accept = gtk_button_new_with_label("Accept");
    gtk_widget_set_size_request(accept, 70, 30);

    /* hbox */
    GtkWidget *newMessageHBox;
    newMessageHBox = gtk_hbox_new(TRUE, 0);

    /* vbox */
    GtkWidget *newMessageVBox;
    newMessageVBox = gtk_vbox_new(FALSE, 0);

    /*** final packing ***/
    gtk_box_pack_start(GTK_BOX(newMessageHBox), reject, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(newMessageHBox), accept, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(newMessageVBox), newMessage, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(newMessageVBox), newMessageHBox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(messagePopupScreen), newMessageVBox);

    messageStruct->widget = messagePopupScreen;
    messageStruct->roomNumber = roomnum;
    if(strcmp("user1", messageStruct->username)==0)
    {
        strncpy(messageStruct->targetName, "user2", sizeof(char) * MAX_USER_NAME);
        messageStruct->targetName[MAX_USER_NAME] = '\0';
    }
    else 
    {
        strncpy(messageStruct->targetName, "user1", sizeof(char) * MAX_USER_NAME);
        messageStruct->targetName[MAX_USER_NAME] = '\0';
    }

    if (noti_type == FRIENDID && noti_ID==FREQUEST)
    {
        g_signal_connect(reject, "clicked", G_CALLBACK(RejectFriendRequest), messageStruct);
        g_signal_connect(accept, "clicked", G_CALLBACK(AcceptFriendRequest), messageStruct);
    }
    else if (noti_type == ROID && noti_ID==ROREQUEST)
    {
        g_signal_connect(reject, "clicked", G_CALLBACK(RejectRoom), messageStruct);
        g_signal_connect(accept, "clicked", G_CALLBACK(AcceptRoom), messageStruct);
    }

    /* SIGNALS */

    /* Show widgets */
    gtk_widget_show(newMessageVBox);
    gtk_widget_show(newMessageHBox);
    gtk_widget_show(reject);
    gtk_widget_show(accept);
    gtk_widget_show(newMessage);
    gtk_widget_show(messagePopupScreen);
}

void AcceptFriendRequest(GtkWidget *button, MESSAGE_STRUCT *messageStruct)
{
    acceptFriendInvite(messageStruct->username, messageStruct->targetName, messageStruct->outputFIFO);
    gtk_widget_destroy(messageStruct->widget);
}

void RejectFriendRequest(GtkWidget *button, MESSAGE_STRUCT *messageStruct)
{
    denyFriendInvite(messageStruct->username, messageStruct->targetName, messageStruct->outputFIFO);
    gtk_widget_destroy(messageStruct->widget);
}
void AcceptRoom(GtkWidget *button, MESSAGE_STRUCT *messageStruct)
{
    joinInvitedRoom(messageStruct->Allroom, messageStruct->roomNumber, messageStruct->username, messageStruct->outputFIFO);
    gtk_widget_destroy(messageStruct->widget);
}
void RejectRoom(GtkWidget *button, MESSAGE_STRUCT *messageStruct)
{
    denyInvitedRoom(messageStruct->roomNumber, messageStruct->username, messageStruct->targetName, messageStruct->outputFIFO);
    gtk_widget_destroy(messageStruct->widget);
}

MESSAGE_STRUCT *CreateMessageStruct(GtkWidget *widget, GtkWidget *window, serverConnection *server, struct allRoom *Allroom, fifo *outputFIFO, inboxQueue *inbox, char *username, char *message)
{
    MESSAGE_STRUCT *messageStruct = NULL;
    messageStruct = malloc(sizeof(MESSAGE_STRUCT));
    if (messageStruct == NULL)
    {
        perror("Out of memory...");
        exit(10);
    }

    messageStruct->widget = widget;
    messageStruct->window = window;
    messageStruct->server = server;
    messageStruct->Allroom = Allroom;
    messageStruct->outputFIFO = outputFIFO;
    messageStruct->inbox = inbox;
    return messageStruct;
}

gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("You exited out the window! \n");
    client_shutdown = 1; /* leaves event loop */
    return FALSE;        /* return false to destroy window */
}

gboolean LoginExit(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("\nSee you later! \n");
    client_shutdown = 1;
    return FALSE;
}

void Login(GtkWidget *widget, gpointer messageStructArray[])
{
    GtkWidget *vBox[3];

    MESSAGE_STRUCT *messageStruct1;
    MESSAGE_STRUCT *messageStruct2;

    messageStruct1 = (MESSAGE_STRUCT *)messageStructArray[0];
    messageStruct2 = (MESSAGE_STRUCT *)messageStructArray[1];

    printf("this is your username: %s \n", messageStruct1->username);

    vBox[0] = messageStruct2->widget;
    vBox[1] = messageStruct2->window;
    vBox[2] = messageStruct1->window;

    GList *vBoxList;
    GtkWidget *usernameEntry;
    GtkWidget *passwordEntry;

    const gchar *username;
    const gchar *password;

    vBoxList = gtk_container_get_children(GTK_CONTAINER(vBox[0]));                               /* getting list of vbox children */
    usernameEntry = gtk_container_get_children(GTK_CONTAINER(vBoxList->data))->next->data;       /* assigning username */
    passwordEntry = gtk_container_get_children(GTK_CONTAINER(vBoxList->next->data))->next->data; /* assigning password */

    username = gtk_entry_get_text(GTK_ENTRY(usernameEntry));
    password = gtk_entry_get_text(GTK_ENTRY(passwordEntry));

    printf("here is username %s \n", username);
    printf("here is password %s \n", password);

    //    strcpy(messageStruct1->username, username);
    strncpy(messageStruct1->username, username, sizeof(char)* MAX_USER_NAME);
    messageStruct1->username[MAX_USER_NAME-1] = '\0';

    strncpy(messageStruct2->username, username, sizeof(char)* MAX_USER_NAME);
    messageStruct2->username[MAX_USER_NAME-1] = '\0';

    gtk_widget_hide(vBox[1]);
    gtk_widget_show(vBox[2]);

    // server connection
    // TODO: change public key

    userLogin((char *)username, (char *)password, messageStruct1->server);

    isLogin = 1;
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

void CreateAccount(GtkWidget *button, GtkWidget *screen)
{

    gtk_widget_show(screen);
}

gboolean HideCharacters(GtkWidget *widget, GdkEvent *event, GtkWidget *button)
{
    GtkEntryBuffer *buffer;
    const gchar *bufferText;
    int test = 0;
    int test2 = 0;
    gboolean active;

    assert(button);
    active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

    buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
    bufferText = gtk_entry_get_text(GTK_ENTRY(widget));

    test = strcmp(bufferText, "Password is case-sensitive.");
    test2 = strcmp(bufferText, "Re-type password to confirm.");

    if (test == 0 || test2 == 0)
    {
        gtk_entry_set_text(GTK_ENTRY(widget), ""); /* deletes the text only if it's the starting text */
    }

    if (active == FALSE)
    {
        gtk_entry_set_visibility(GTK_ENTRY(widget), FALSE); /* hides password input if toggle isn't on */
        gtk_entry_set_invisible_char(GTK_ENTRY(widget), '*');
    }

    return FALSE;
}

gboolean Overwrite(GtkWidget *username, GtkWidget *entry)
{
    GtkEntryBuffer *buffer;
    const gchar *bufferText;
    int test = 0;

    buffer = gtk_entry_get_buffer(GTK_ENTRY(username));
    bufferText = gtk_entry_get_text(GTK_ENTRY(username));

    test = strcmp(bufferText, "Choose a name you'd like to go by.");

    if (test == 0)
    {
        gtk_entry_set_text(GTK_ENTRY(username), ""); /* deletes the text only if it's the starting text */
    }

    return FALSE;
}

void ShowCharacters(GtkWidget *button, GtkWidget *vBox)
{
    GList *list;
    GtkWidget *passConfirmBox;
    GtkWidget *passBox;
    GtkWidget *passConfirmEntry;
    GtkWidget *passEntry;

    list = gtk_container_get_children(GTK_CONTAINER(vBox)); /* gets children of vBox */

    passConfirmBox = list->next->next->data; /* extracting widgets from the list */
    passBox = list->next->data;

    passConfirmEntry = gtk_container_get_children(GTK_CONTAINER(passConfirmBox))->next->data; /* gets username entry */
    passEntry = gtk_container_get_children(GTK_CONTAINER(passBox))->next->data;               /* gets password entry */

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {
        gtk_entry_set_visibility(GTK_ENTRY(passConfirmEntry), TRUE); /* turns on visibility for the passwords */
        gtk_entry_set_visibility(GTK_ENTRY(passEntry), TRUE);
    }
    else
    {
        gtk_entry_set_visibility(GTK_ENTRY(passConfirmEntry), FALSE); /* hides password input */
        gtk_entry_set_invisible_char(GTK_ENTRY(passConfirmEntry), '*');

        gtk_entry_set_visibility(GTK_ENTRY(passEntry), FALSE); /* hides password input */
        gtk_entry_set_invisible_char(GTK_ENTRY(passEntry), '*');
    }

    //    g_signal_connect(passConfirmEntry, "key-press-event", G_CALLBACK(ResetShowToggle), button);
    //    g_signal_connect(passEntry, "key-press-event", G_CALLBACK(ResetShowToggle), button);
}

void ClearForm(GtkWidget *button, GtkWidget *vBox)
{
    GList *list;
    GtkWidget *passConfirmBox;
    GtkWidget *passBox;
    GtkWidget *passConfirmEntry;
    GtkWidget *passEntry;
    GtkWidget *userBox;
    GtkWidget *userEntry;

    list = gtk_container_get_children(GTK_CONTAINER(vBox)); /* gets children of vBox */

    passConfirmBox = list->next->next->data; /* extracting widgets from the list */
    passBox = list->next->data;
    userBox = list->data;

    passConfirmEntry = gtk_container_get_children(GTK_CONTAINER(passConfirmBox))->next->data; /* gets username entry */
    passEntry = gtk_container_get_children(GTK_CONTAINER(passBox))->next->data;               /* gets password entry */
    userEntry = gtk_container_get_children(GTK_CONTAINER(userBox))->next->data;               /* gets username entry */

    gtk_entry_set_visibility(GTK_ENTRY(passConfirmEntry), TRUE); /* turns on visibility for the passwords */
    gtk_entry_set_visibility(GTK_ENTRY(passEntry), TRUE);

    gtk_entry_set_text(GTK_ENTRY(userEntry), "Choose a name you'd like to go by.");
    gtk_entry_set_text(GTK_ENTRY(passEntry), "Password is case-sensitive.");
    gtk_entry_set_text(GTK_ENTRY(passConfirmEntry), "Re-type password to confirm.");
}

void EnterKey(GtkWidget *entry, gpointer messageStruct)
{
    GtkWidget *button;
    button = gtk_button_new_with_label("test");
    SendButton(button, messageStruct);
}

void SendButton(GtkWidget *widget, gpointer messageStruct)
{
    GtkWidget *vBox;
    GtkTextIter iter;
    GtkWidget *tabs;
    GtkWidget *messageScreen;
    GtkWidget *scrolledWindow;
    GList *child;
    int currentPage = 0;
    GList *list;
    GList *list2;
    MESSAGE_STRUCT *messageData;
    messageData = (MESSAGE_STRUCT *)messageStruct;
    vBox = messageData->widget;
    const gchar *actualMessage;

    guint16 check = 0;

    /* list->data = tabs, list->next->data = hbox*/
    list = gtk_container_get_children(GTK_CONTAINER(vBox));              /* list of vBox: tabs, hBox */
    list2 = gtk_container_get_children(GTK_CONTAINER(list->next->data)); /* list of hBox: textBox, sendButton */

    tabs = list->data;
    currentPage = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));             /* getting current page */
    scrolledWindow = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), currentPage); /* getting scrolled view */
    child = gtk_container_get_children(GTK_CONTAINER(scrolledWindow));           /* child->data is messageScreen */

  /*  char *roomName;
    int roomNum;
    GtkWidget *tabLabel;

    tabLabel = gtk_notebook_get_menu_label(GTK_NOTEBOOK(scrolledWindow), child);
    roomName = gtk_label_get_text(GTK_LABEL(tabLabel));
    sscanf(roomName, "Room %d", &roomNum);
    */
    messageScreen = child->data;

    check = gtk_entry_get_text_length(GTK_ENTRY(list2->data));  /* checking length of text input */
    actualMessage = gtk_entry_get_text(GTK_ENTRY(list2->data)); /* saving actual text */

    if (check != 0) /* only run this if there is a text input */
    {
        GtkTextBuffer *buffer;
        GtkTextMark *start;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen)); /* gets the buffer for the current screen */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at the end */

        gtk_text_buffer_insert(buffer, &iter, "\n\n", -1); /* insert new lines */

        gtk_text_buffer_insert(buffer, &iter, messageData->username, -1); /* adds "username: " */

        gtk_text_buffer_insert(buffer, &iter, ": ", 2); /* adds ": " */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at end again */

        /* get mark for emoji display at the point before user text is inserted */
        start = gtk_text_buffer_create_mark(buffer, NULL, &iter, TRUE);

        gtk_text_buffer_insert(buffer, &iter, gtk_entry_get_text(GTK_ENTRY(list2->data)), -1); /* inserts user text */

        sendMessage(&(messageData->Allroom->roomList[0]), messageData->outputFIFO, messageData->username, (char *)actualMessage); /* send message to fifo */

        /* display emoji on messageScreen */
        insert_emoji(GTK_TEXT_VIEW(messageScreen), start);

        gtk_entry_set_text(GTK_ENTRY(list2->data), ""); /* replaces textBox with empty text again */
    }
}

void OptionsPopup(GtkWidget *button, GtkWidget *options[])
{
    /*    GtkWidget *optionsArray0[7];
    options[0] = options;
    options[1] = messageButton;
    options[2] = tabs;
    options[3] = scrollWindow0;
    options[4] = messageScreen0;
    options[5] = tabLabel0;
    options[6] = blockButton;
*/

    gtk_menu_popup(GTK_MENU(options[0]), NULL, NULL, NULL, NULL, 1, gtk_get_current_event_time()); /* option menu pops up */

    g_signal_connect(options[1], "activate", G_CALLBACK(MessageUser), options); /* messaging a new user */
    g_signal_connect(options[6], "activate", G_CALLBACK(BlockUser), options);   /* blocking user */
}

void MessageUser(GtkWidget *widget, GtkWidget *tabCreation[])
{
    int check = 0;

    check = CheckNotebook(tabCreation[2], tabCreation[5]);
    if (check == 0)
    {
        SetMessageScreen(tabCreation[2], tabCreation[5], tabCreation[3], tabCreation[4]);
    }

    gtk_menu_item_deselect(GTK_MENU_ITEM(widget));
}

void BlockUser(GtkWidget *widget, GtkWidget *tabCreation[])
{
    int check = 0;

    check = CheckNotebook(tabCreation[2], tabCreation[5]); /* checking if tab already exists */
    if (check > 1)
    {
        printf("[Delete the tab] \n");
        printf("[TO BE IMPLEMENTED] \n");
    }
    else if (check == 0)
    {
        printf("You blocked the user from contacting you. \n");
        printf("[TO BE IMPLEMENTED]\n");
    }
}

void AcceptMessage(GtkWidget *button, GtkWidget *window)
{
    gtk_widget_destroy(window);
}

void SetWelcomeScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen, GtkTextBuffer *buffer)
{
    int success = 0;
    gtk_widget_set_size_request(messageScreen, 500, 300);                  /* setting size */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messageScreen), FALSE);       /* turning of editability */
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(messageScreen), FALSE); /* turn off cursor */

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen));
    gtk_text_buffer_set_text(buffer,
                             "Hello! Welcome to Toph Chat! \n\nCurrent Version: Beta\n\nTo get started:\n  1. Click on your \"Friends List\" on the right.\n  2. Click on a friend you want to chat with.\n  3. Select \"Message\". \n  4. Get started chatting!\n", -1); /* Welcome message */

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); /* settings for scroll bars */
    gtk_container_add(GTK_CONTAINER(scrollWindow), messageScreen);                                              /* putting message screen into a scrolled window */

    success = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), scrollWindow, tabLabel); /* creating new notebook tab*/
}

int CheckNotebook(GtkWidget *tabs, GtkWidget *tabLabel)
{
    GtkWidget *currentTab;
    GtkWidget *child;
    gint totalTabs = 0;

    int count = 0;

    totalTabs = gtk_notebook_get_n_pages(GTK_NOTEBOOK(tabs)); /* get total number of tabs open */
    for (int i = 0; i < totalTabs; i++)
    {
        child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), i); /* get child of notebook */
        currentTab = gtk_notebook_get_tab_label(GTK_NOTEBOOK(tabs), child);

        if (currentTab == tabLabel)
        {
            count++;
        }
    }

    return count;
}

void SetMessageScreen(GtkWidget *tabs, GtkWidget *tabLabel, GtkWidget *scrollWindow, GtkWidget *messageScreen) /* setting up message screens */
{
    int success = 0;
    GtkTextBuffer *buffer;

    assert(messageScreen);

    gtk_widget_set_size_request(messageScreen, 500, 300);                  /* setting size */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(messageScreen), FALSE);       /* turning of editability */
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(messageScreen), FALSE); /* turn off cursor */

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen));
    gtk_text_buffer_set_text(buffer, "Welcome to the chat room. Say hello!", -1); /* Welcome message */

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); /* settings for scroll bars */
    gtk_container_add(GTK_CONTAINER(scrollWindow), messageScreen);                                              /* putting message screen into a scrolled window */

    success = gtk_notebook_append_page(GTK_NOTEBOOK(tabs), scrollWindow, tabLabel); /* creating new notebook tab*/

    gtk_widget_show(tabLabel);
    gtk_widget_show(scrollWindow);
    gtk_widget_show(messageScreen);
}

void CreateNewAccount(GtkWidget *button, MESSAGE_STRUCT *messageStruct)
{
    char *username;
    char *password;
    GList *child;

    GtkWidget *userEntry;
    GtkWidget *passEntry;

    child = gtk_container_get_children(GTK_CONTAINER(messageStruct->widget));
    userEntry = gtk_container_get_children(GTK_CONTAINER(child->data))->next->data;
    passEntry = gtk_container_get_children(GTK_CONTAINER(child->next->data))->next->data;

    username = gtk_entry_get_text(GTK_ENTRY(userEntry));
    password = gtk_entry_get_text(GTK_ENTRY(passEntry));
    printf("username = %s passowrd = %s\n", username, password);
    userSignUp(username, password, messageStruct->server);

    gtk_widget_hide(messageStruct->window);
}

void CreateNewTab(GtkButton *button, MESSAGE_STRUCT *messageStruct)
{
    int success = 0;
    GtkTextBuffer *buffer;
    GtkWidget *tabLabel;
    GtkWidget *scrollWindow;
    GtkWidget *messageScreen;
    int roomNum;
    char roomName[9];    
    roomNum = requestRoom(messageStruct->Allroom, messageStruct->outputFIFO, messageStruct->username);
    if (roomNum < 0)
    {
        char failMsg[80] = "Failed to create Room.\nYou have exceed the max room limit.";
        popup_with_label(failMsg);
    }
    else
    {
        joinCreatedRoom(messageStruct->Allroom, roomNum); 
        sprintf(roomName, "Room %d", roomNum);
        tabLabel = gtk_label_new(roomName);      
        
        messageScreen = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
        scrollWindow = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */

        gtk_widget_set_size_request(messageScreen, 500, 300);                  /* setting size */
        gtk_text_view_set_editable(GTK_TEXT_VIEW(messageScreen), FALSE);       /* turning of editability */
        gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(messageScreen), FALSE); /* turn off cursor */

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen));
        gtk_text_buffer_set_text(buffer, "Welcome to the chat room. Say hello!", -1); /* Welcome message */

        gtk_widget_set_size_request(scrollWindow, 200, 600);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS); /* settings for scroll bars */
        gtk_container_add(GTK_CONTAINER(scrollWindow), messageScreen);                                              /* putting message screen into a scrolled window */

        success = gtk_notebook_append_page(GTK_NOTEBOOK(messageStruct->widget), scrollWindow, tabLabel); /* creating new notebook tab*/

        gtk_widget_show(tabLabel);
        gtk_widget_show(scrollWindow);
        gtk_widget_show(messageScreen);
    }
}

void Hide(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_widget_hide(widget);
}

static GtkTreeModel *store_online_friends(char **friendList)
{
    assert(friendList);
    int i;
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);

    for (i = 0; i < MAX_USER_FRIEND; i++)
    {
        if (friendList[i] == NULL)
        {
            continue;
        }
        else
        {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, friendList[i],
                               -1);
        }
    }
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_online_friends(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1, "Online Friend List",
                                                renderer, "text", 0,
                                                NULL);
    model = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
    return view;
}

static void update_online_friend_model(GtkWidget *view, char **friendList)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    model = store_online_friends(friendList);
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
}

static GtkWidget *create_online_friend_list(MESSAGE_STRUCT *messageStruct)
{
    GtkWidget *friendList_widget, *scrolledWindow;

    friendList_widget = view_model_online_friends();
    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), friendList_widget);
    g_signal_connect(friendList_widget, "row-activated", G_CALLBACK(friend_list_clicked), messageStruct);
    gtk_widget_show(friendList_widget);
    return scrolledWindow;
}

static GtkTreeModel *store_offline_friends(char **friendList)
{
    assert(friendList);
    int i;
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);

    for (i = 0; i < MAX_USER_FRIEND; i++)
    {
        if (friendList[i] == NULL)
        {
            continue;
        }
        else
        {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, friendList[i],
                               -1);
        }
    }
    return GTK_TREE_MODEL(store);
}

static GtkWidget *view_model_offline_friends(void)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;

    view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1, "Offline Friend List",
                                                renderer, "text", 0,
                                                NULL);
    model = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
    return view;
}

static void update_offline_friend_model(GtkWidget *view, char **friendList)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    model = store_offline_friends(friendList);
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
}

static GtkWidget *create_offline_friend_list(MESSAGE_STRUCT *messageStruct)
{
    GtkWidget *friendList_widget, *scrolledWindow;

    friendList_widget = view_model_offline_friends();
    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), friendList_widget);
    gtk_widget_show(friendList_widget);
    return scrolledWindow;
}

void friend_list_clicked(GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
    GtkWidget *child;
    GtkWidget *tabLabel;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *roomName;
    int roomNum;
    char *friendName;

    int currentTab;
    MESSAGE_STRUCT *messageStruct;
    messageStruct = (MESSAGE_STRUCT *)user_data;
    currentTab = gtk_notebook_get_current_page(GTK_NOTEBOOK(messageStruct->widget));
    child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(messageStruct->widget), currentTab);
    
    tabLabel = gtk_notebook_get_menu_label(GTK_NOTEBOOK(messageStruct->widget), child);
    roomName = gtk_label_get_text(GTK_LABEL(tabLabel));
    sscanf(roomName, "Room %d", &roomNum);

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get_value(model, &iter, 0, &friendName);

    sendRoomInvite(roomNum, messageStruct->username, friendName, messageStruct->outputFIFO);
}

void friend_invite_clicked(GtkButton *button, MESSAGE_STRUCT *messageStruct)
{
    GtkWidget *friendInviteScreen;
    friendInviteScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(friendInviteScreen), "Friend Invitation"); /* sets title of window */
    gtk_widget_set_size_request(friendInviteScreen, 430, 200);
    gtk_container_set_border_width(GTK_CONTAINER(friendInviteScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(friendInviteScreen), FALSE);

    /* frame */
    GtkWidget *inviteFrame;
    inviteFrame = gtk_frame_new("Toph Chat");

    /* Labels */
    GtkWidget *usernameLabel;
    usernameLabel = gtk_label_new("Friend's Username: ");

    /* VBox */
    GtkWidget *inviteVBox;
    inviteVBox = gtk_vbox_new(FALSE, 10); /* vertical box for the entries */

    /* HBox */
    GtkWidget *inviteHBox;
    inviteHBox = gtk_hbox_new(FALSE, 0); /* horizontal box for the quit and invite */

    GtkWidget *usernameBox;
    usernameBox = gtk_hbox_new(FALSE, 5);

    /* buttons */
    GtkWidget *quitButton;
    quitButton = gtk_button_new_with_label("Quit");
    gtk_widget_set_size_request(quitButton, 70, 30);

    GtkWidget *inviteButton;
    inviteButton = gtk_button_new_with_label("Invite");
    gtk_widget_set_size_request(inviteButton, 100, 30);

    /* entries */
    GtkWidget *username;
    username = gtk_entry_new();

    /* fixed */
    GtkWidget *inviteFixed;
    inviteFixed = gtk_fixed_new();

    GtkWidget *warningLabel;
    warningLabel = gtk_label_new("**DO NOT INPUT FORWARD SLASH '/' IN USERNAME**");

    /***** packing *****/
    /* usernameBox*/
    gtk_box_pack_start(GTK_BOX(usernameBox), usernameLabel, FALSE, FALSE, 10);
    gtk_box_pack_end(GTK_BOX(usernameBox), username, FALSE, FALSE, 10);

    /* invite HBox */
    gtk_box_pack_start(GTK_BOX(inviteHBox), quitButton, TRUE, FALSE, 30);
    gtk_box_pack_start(GTK_BOX(inviteHBox), inviteButton, TRUE, FALSE, 30);

    /* vBox */
    gtk_box_pack_start(GTK_BOX(inviteVBox), usernameBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inviteVBox), inviteHBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inviteVBox), warningLabel, FALSE, FALSE, 0);

    gtk_fixed_put(GTK_FIXED(inviteFixed), inviteVBox, 20, 40);

    gtk_container_add(GTK_CONTAINER(inviteFrame), inviteFixed);

    gtk_container_add(GTK_CONTAINER(friendInviteScreen), inviteFrame);

    g_signal_connect(quitButton, "clicked", G_CALLBACK(popupExit), friendInviteScreen);
    g_signal_connect(friendInviteScreen, "delete-event", G_CALLBACK(popupExit), friendInviteScreen);

    messageStruct->widget = username;
    messageStruct->window = friendInviteScreen;
    g_signal_connect(inviteButton, "clicked", G_CALLBACK(inviteClicked), messageStruct);

    /* show all the widgets */
    gtk_widget_show(inviteFixed);
    gtk_widget_show(inviteFrame);
    gtk_widget_show(inviteVBox);
    gtk_widget_show(usernameLabel);
    gtk_widget_show(usernameBox);
    gtk_widget_show(inviteHBox);
    gtk_widget_show(warningLabel);
    gtk_widget_show(quitButton);
    gtk_widget_show(inviteButton);
    gtk_widget_show(username);
    gtk_widget_show(friendInviteScreen);
}

void inviteClicked(GtkButton *button, MESSAGE_STRUCT *messageStruct)
{
    char *friendName=malloc(sizeof(char)*MAX_USER_NAME);
    strncpy(friendName, gtk_entry_get_text(GTK_ENTRY(messageStruct->widget)), sizeof(char)*MAX_USER_NAME);
    friendName[MAX_USER_NAME-1]='\0';
    printf("target friend = %s", friendName);
    sendFriendInvite(messageStruct->username, friendName, messageStruct->outputFIFO);
    gtk_widget_destroy(GTK_WIDGET(messageStruct->window));
}

void popupExit(GtkButton *button, GtkWidget* user_data)
{
    gtk_widget_destroy(user_data);
}

void popup_with_label(char *label)
{
    GtkWidget *messagePopupScreen;
    messagePopupScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(messagePopupScreen), "New Notification!"); /* sets title of window */
    gtk_widget_set_size_request(messagePopupScreen, 270, 100);
    gtk_container_set_border_width(GTK_CONTAINER(messagePopupScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(messagePopupScreen), FALSE);

    /* labels */
    GtkWidget *newMessage;
    newMessage = gtk_label_new(label);
    
    GtkWidget *okButton;
    okButton = gtk_button_new_with_label("OK");
    gtk_widget_set_size_request(okButton, 70, 30);
    
    GtkWidget *popupVBox;
    popupVBox = gtk_vbox_new(FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(popupVBox), newMessage, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(popupVBox), okButton, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(messagePopupScreen), popupVBox);
        
    g_signal_connect(messagePopupScreen, "delete-event", G_CALLBACK(popupExit), messagePopupScreen);
    g_signal_connect(okButton, "clicked", G_CALLBACK(popupExit), messagePopupScreen);
    
    gtk_widget_show(newMessage);
    gtk_widget_show(okButton);
    gtk_widget_show(popupVBox);
    gtk_widget_show(messagePopupScreen);
}
/* render the window on screen */
void UpdateWindow(void)
{
    /* this server has it's own main loop for handling client connections;
     * as such, it can't have the usual GUI main loop (gtk_main);
     * instead, we call this UpdateWindow function on a regular basis
     */
    while (gtk_events_pending())
    {
        gtk_main_iteration();
    }
} /* end of UpdateWindow */

int main(int argc, char *argv[])
{

    /************************************/
    /*****   INITIALIZATION   **********/
    /**********************************/
    GtkWidget *window; /* declaring a window */

    gtk_init(&argc, &argv); /* initializing GTK environment */

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); /* initialize window to be on top */

    gtk_window_set_title(GTK_WINDOW(window), "Toph Chat Alpha Version"); /* make title for window */
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);            /* sets the width from the inside */

    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    /**************************************************************************************************/
    /**********************************************/
    /***********     LOGIN SCREEN    *************/
    /********************************************/
    /* window */
    GtkWidget *loginScreen;
    loginScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(loginScreen), "Toph Chat Login/Registration"); /* sets title of window */
    gtk_widget_set_size_request(loginScreen, 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(loginScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(loginScreen), FALSE);

    /* frame */
    GtkWidget *loginFrame;
    loginFrame = gtk_frame_new("Toph Chat");

    /* Labels */
    GtkWidget *usernameLabel;
    usernameLabel = gtk_label_new("Username: ");

    GtkWidget *passwordLabel;
    passwordLabel = gtk_label_new("Password: ");

    GtkWidget *warningLabel;
    warningLabel = gtk_label_new("DO NOT INPUT '/' IN USERNAME");

    GtkWidget *createAccountLabel;
    createAccountLabel = gtk_label_new("Don't have an account? Create a new one");

    //GtkWidget *forgotAccountLabel;
    //forgotAccountLabel = gtk_label_new("Forgot account? Recover it");

    /* VBox */
    GtkWidget *loginVBox;
    loginVBox = gtk_vbox_new(FALSE, 10); /* vertical box for the entries */

    GtkWidget *accountVBox;
    accountVBox = gtk_vbox_new(FALSE, 0);

    /* HBox */
    GtkWidget *loginHBox;
    loginHBox = gtk_hbox_new(FALSE, 0); /* horizontal box for the quit and login */

    GtkWidget *usernameBox, *passwordBox;
    usernameBox = gtk_hbox_new(FALSE, 5);
    passwordBox = gtk_hbox_new(FALSE, 5);

    GtkWidget *createAccountBox;
    createAccountBox = gtk_hbox_new(FALSE, 0);

    //GtkWidget *forgotAccountBox;
    //forgotAccountBox = gtk_hbox_new(FALSE, 0);

    /* buttons */
    GtkWidget *quitButton;
    quitButton = gtk_button_new_with_label("Quit");
    gtk_widget_set_size_request(quitButton, 70, 30);

    GtkWidget *loginButton;
    loginButton = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(loginButton, 100, 30);

    GtkWidget *createAccount;
    createAccount = gtk_button_new_with_label("here!");
    gtk_button_set_relief(GTK_BUTTON(createAccount), GTK_RELIEF_NONE);

    //GtkWidget *forgotAccount;
    //forgotAccount = gtk_button_new_with_label("here!");
    //gtk_button_set_relief(GTK_BUTTON(forgotAccount), GTK_RELIEF_NONE);

    /* entries */
    GtkWidget *username;
    username = gtk_entry_new();

    GtkWidget *password;
    password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(password), '*');
    gtk_entry_set_has_frame(GTK_ENTRY(password), TRUE);

    /* fixed */
    GtkWidget *loginFixed;
    loginFixed = gtk_fixed_new();

    /***** packing *****/
    /* usernameBox*/
    gtk_box_pack_start(GTK_BOX(usernameBox), usernameLabel, FALSE, FALSE, 10);
    gtk_box_pack_end(GTK_BOX(usernameBox), username, FALSE, FALSE, 10);

    /* passwordBox*/
    gtk_box_pack_start(GTK_BOX(passwordBox), passwordLabel, FALSE, FALSE, 10);
    gtk_box_pack_end(GTK_BOX(passwordBox), password, FALSE, FALSE, 10);

    /* loginHBox */
    gtk_box_pack_start(GTK_BOX(loginHBox), quitButton, TRUE, FALSE, 30);
    gtk_box_pack_start(GTK_BOX(loginHBox), loginButton, TRUE, FALSE, 30);

    /* create account */
    gtk_box_pack_start(GTK_BOX(createAccountBox), createAccountLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(createAccountBox), createAccount, FALSE, FALSE, 0);

    /* forgot account */
    //gtk_box_pack_start(GTK_BOX(forgotAccountBox), forgotAccountLabel, FALSE, FALSE, 0);
    //gtk_box_pack_start(GTK_BOX(forgotAccountBox), forgotAccount, FALSE, FALSE, 0);

    /* account box */
    gtk_box_pack_start(GTK_BOX(accountVBox), warningLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountVBox), createAccountBox, FALSE, FALSE, 0);
    //gtk_box_pack_start(GTK_BOX(accountVBox), forgotAccountBox, FALSE, FALSE, 0);

    /* vBox */
    gtk_box_pack_start(GTK_BOX(loginVBox), usernameBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(loginVBox), passwordBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(loginVBox), loginHBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(loginVBox), accountVBox, FALSE, FALSE, 20);

    gtk_fixed_put(GTK_FIXED(loginFixed), loginVBox, 20, 40);

    gtk_container_add(GTK_CONTAINER(loginFrame), loginFixed);

    gtk_container_add(GTK_CONTAINER(loginScreen), loginFrame);

    /* show all the widgets */
    gtk_widget_show(accountVBox);
    //gtk_widget_show(forgotAccount);
    //gtk_widget_show(forgotAccountBox);
    //gtk_widget_show(forgotAccountLabel);
    gtk_widget_show(createAccount);
    gtk_widget_show(warningLabel);
    gtk_widget_show(createAccountBox);
    gtk_widget_show(createAccountLabel);
    gtk_widget_show(loginFixed);
    gtk_widget_show(loginFrame);
    gtk_widget_show(loginVBox);
    gtk_widget_show(usernameLabel);
    gtk_widget_show(passwordLabel);
    gtk_widget_show(usernameBox);
    gtk_widget_show(passwordBox);
    gtk_widget_show(loginHBox);
    gtk_widget_show(quitButton);
    gtk_widget_show(loginButton);
    gtk_widget_show(username);
    gtk_widget_show(password);
    gtk_widget_show(loginScreen);

    /***************************************************************************************************/
    /********************************/
    /*****   ACCOUNT CREATION ******/
    /******************************/

    /* Account Creation Window */
    GtkWidget *accountCreationScreen;
    accountCreationScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(accountCreationScreen), "Account Registration"); /* sets title of window */
    gtk_widget_set_size_request(accountCreationScreen, 600, 300);
    gtk_container_set_border_width(GTK_CONTAINER(accountCreationScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(accountCreationScreen), FALSE);

    /* frame */
    GtkWidget *accountFrame;
    accountFrame = gtk_frame_new("Account Registration");
    gtk_frame_set_label_align(GTK_FRAME(accountFrame), .5, .5);

    /* fixed */
    GtkWidget *accountFixed;
    accountFixed = gtk_fixed_new();

    GtkWidget *checkBoxFixed;
    checkBoxFixed = gtk_fixed_new();

    /* labels */
    GtkWidget *newUsernameLabel;
    newUsernameLabel = gtk_label_new("             Username:");

    GtkWidget *newPasswordLabel;
    newPasswordLabel = gtk_label_new("              Password:");

    GtkWidget *newPasswordConfirmLabel;
    newPasswordConfirmLabel = gtk_label_new("Confirm Password:");

    /* buttons */
    GtkWidget *clearForm;
    clearForm = gtk_button_new_with_label("Clear Form");

    GtkWidget *createAccountButton;
    createAccountButton = gtk_button_new_with_label("Create");

    /* checkbox */
    GtkWidget *showPasswordCheckBox;
    showPasswordCheckBox = gtk_check_button_new_with_label("Show Password");

    /* Entries */
    GtkWidget *newUsername;
    newUsername = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(newUsername), "Choose a name you'd like to go by.");
    gtk_widget_set_size_request(newUsername, 300, 25);
    gtk_entry_set_has_frame(GTK_ENTRY(newUsername), TRUE);

    GtkWidget *newPassword;
    newPassword = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(newPassword), "Password is case-sensitive.");
    gtk_widget_set_size_request(newPassword, 300, 25);
    gtk_entry_set_has_frame(GTK_ENTRY(newPassword), TRUE);

    GtkWidget *newPasswordConfirm;
    newPasswordConfirm = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(newPasswordConfirm), "Re-type password to confirm.");
    gtk_widget_set_size_request(newPassword, 300, 25);
    gtk_entry_set_has_frame(GTK_ENTRY(newPasswordConfirm), TRUE);

    GtkWidget *warningAccountLabel;
    warningAccountLabel = gtk_label_new("DO NOT INPUT '/' IN USERNAME");

    /* HBox */
    GtkWidget *newUserBox;
    newUserBox = gtk_hbox_new(FALSE, 0);

    GtkWidget *newPassBox;
    newPassBox = gtk_hbox_new(FALSE, 0);

    GtkWidget *newPassConfirmBox;
    newPassConfirmBox = gtk_hbox_new(FALSE, 0);

    GtkWidget *accountButtonBox;
    accountButtonBox = gtk_hbox_new(FALSE, 0);

    /* VBox */
    GtkWidget *accountCreationVBox;
    accountCreationVBox = gtk_vbox_new(FALSE, 0);

    /*** Packing ***/
    gtk_box_pack_start(GTK_BOX(newUserBox), newUsernameLabel, FALSE, FALSE, 0); /* username box */
    gtk_box_pack_start(GTK_BOX(newUserBox), newUsername, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(newPassBox), newPasswordLabel, FALSE, FALSE, 0); /* password box */
    gtk_box_pack_start(GTK_BOX(newPassBox), newPassword, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(newPassConfirmBox), newPasswordConfirmLabel, FALSE, FALSE, 0); /* password confirmation box */
    gtk_box_pack_start(GTK_BOX(newPassConfirmBox), newPasswordConfirm, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(accountButtonBox), clearForm, TRUE, FALSE, 0); /* buttons box */
    gtk_box_pack_start(GTK_BOX(accountButtonBox), createAccountButton, TRUE, FALSE, 0);

    gtk_fixed_put(GTK_FIXED(checkBoxFixed), showPasswordCheckBox, 450, 0); /* putting check box to the right */

    gtk_box_pack_start(GTK_BOX(accountCreationVBox), newUserBox, FALSE, FALSE, 0); /* vBox packing */
    gtk_box_pack_start(GTK_BOX(accountCreationVBox), newPassBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountCreationVBox), newPassConfirmBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountCreationVBox), checkBoxFixed, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountCreationVBox), accountButtonBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountCreationVBox), warningAccountLabel, FALSE, FALSE, 0);

    /** final packing **/
    gtk_container_add(GTK_CONTAINER(accountFrame), accountCreationVBox);
    gtk_container_add(GTK_CONTAINER(accountCreationScreen), accountFrame);

    /**** SIGNALS ****/

    /* showing widgets */
    gtk_widget_show(showPasswordCheckBox);
    gtk_widget_show(checkBoxFixed);
    gtk_widget_show(accountFrame);
    gtk_widget_show(accountFixed);
    gtk_widget_show(newUsernameLabel);
    gtk_widget_show(newPasswordLabel);
    gtk_widget_show(newPasswordConfirmLabel);
    gtk_widget_show(clearForm);
    gtk_widget_show(createAccountButton);
    gtk_widget_show(newUsername);
    gtk_widget_show(newPassword);
    gtk_widget_show(newPasswordConfirm);
    gtk_widget_show(newUserBox);
    gtk_widget_show(newPassBox);
    gtk_widget_show(newPassConfirmBox);
    gtk_widget_show(accountButtonBox);
    gtk_widget_show(warningAccountLabel);
    gtk_widget_show(accountCreationVBox);
    //    gtk_widget_show(accountCreationScreen);

    /*************************************************************************************************/
    /*******************************/
    /******   Message Popup  ******/
    /*****************************/
    /*******************************/
    /*****      VARIABLES     *****/
    /*****************************/

    /* Notebook Variables */
    GtkWidget *tabs;
    tabs = gtk_notebook_new();                             /* initializing tabs */
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(tabs), TRUE); /* make the tabs scrollable */

    /* messageScreen Variables */
    /* Welcome Screen */
    GtkWidget *messageScreen; /* screen to display messages */
    GtkTextBuffer *buffer;    /* the screen's text */
    GtkWidget *tabLabel;      /* the tab's label */
    GtkWidget *scrollWindow;  /* the scroll bar */

    tabLabel = gtk_label_new("Welcome");                                   /* initializing label */
    messageScreen = gtk_text_view_new_with_buffer(NULL);                   /* initializing the message screen */
    scrollWindow = gtk_scrolled_window_new(NULL, NULL);                    /* initializing new scrolled window with no adjustments */
    SetWelcomeScreen(tabs, tabLabel, scrollWindow, messageScreen, buffer); /* setting the first welcome message screen */

    /* screen 0*/
    GtkWidget *messageScreen0; /* screen to display messages */
    GtkWidget *tabLabel0;      /* the tab's label */
    GtkWidget *scrollWindow0;  /* the scroll bar */

    tabLabel0 = gtk_label_new("Friend 0");                /* initializing label */
    messageScreen0 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow0 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 1*/
    GtkWidget *messageScreen1; /* screen to display messages */
    GtkWidget *tabLabel1;      /* the tab's label */
    GtkWidget *scrollWindow1;  /* the scroll bar */

    tabLabel1 = gtk_label_new("Friend 1");                /* initializing label */
    messageScreen1 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow1 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 2*/
    GtkWidget *messageScreen2; /* screen to display messages */
    GtkWidget *tabLabel2;      /* the tab's label */
    GtkWidget *scrollWindow2;  /* the scroll bar */

    tabLabel2 = gtk_label_new("Friend 2");                /* initializing label */
    messageScreen2 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow2 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 3*/
    GtkWidget *messageScreen3; /* screen to display messages */
    GtkWidget *tabLabel3;      /* the tab's label */
    GtkWidget *scrollWindow3;  /* the scroll bar */

    tabLabel3 = gtk_label_new("Friend 3");                /* initializing label */
    messageScreen3 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow3 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 4*/
    GtkWidget *messageScreen4; /* screen to display messages */
    GtkWidget *tabLabel4;      /* the tab's label */
    GtkWidget *scrollWindow4;  /* the scroll bar */

    tabLabel4 = gtk_label_new("Friend 4");                /* initializing label */
    messageScreen4 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow4 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 5*/
    GtkWidget *messageScreen5; /* screen to display messages */
    GtkWidget *tabLabel5;      /* the tab's label */
    GtkWidget *scrollWindow5;  /* the scroll bar */

    tabLabel5 = gtk_label_new("Friend 5");                /* initializing label */
    messageScreen5 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow5 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 6*/
    GtkWidget *messageScreen6; /* screen to display messages */
    GtkWidget *tabLabel6;      /* the tab's label */
    GtkWidget *scrollWindow6;  /* the scroll bar */

    tabLabel6 = gtk_label_new("Friend 6");                /* initializing label */
    messageScreen6 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow6 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 7*/
    GtkWidget *messageScreen7; /* screen to display messages */
    GtkWidget *tabLabel7;      /* the tab's label */
    GtkWidget *scrollWindow7;  /* the scroll bar */

    tabLabel7 = gtk_label_new("Friend 7");                /* initializing label */
    messageScreen7 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow7 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 8*/
    GtkWidget *messageScreen8; /* screen to display messages */
    GtkWidget *tabLabel8;      /* the tab's label */
    GtkWidget *scrollWindow8;  /* the scroll bar */

    tabLabel8 = gtk_label_new("Friend 8");                /* initializing label */
    messageScreen8 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow8 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */
    /* screen 9*/
    GtkWidget *messageScreen9; /* screen to display messages */
    GtkWidget *tabLabel9;      /* the tab's label */
    GtkWidget *scrollWindow9;  /* the scroll bar */

    tabLabel9 = gtk_label_new("Friend 9");                /* initializing label */
    messageScreen9 = gtk_text_view_new_with_buffer(NULL); /* initializing the message screen */
    scrollWindow9 = gtk_scrolled_window_new(NULL, NULL);  /* initializing new scrolled window with no adjustments */

    /* textBox Variables */
    GtkWidget *textBox;
    textBox = gtk_entry_new();
    gtk_widget_set_size_request(textBox, 500, 50);

    /* Alignment */
    GtkWidget *alignment;
    gtk_alignment_new(0, 0, 1, 1);

    /* Buttons */
    GtkWidget *sendButton;
    sendButton = gtk_button_new_with_label("SEND");
    gtk_widget_set_size_request(sendButton, 100, 50);

    GtkWidget *emojiButton;
    emojiButton = emoji_popup(GTK_ENTRY(textBox));

    /* Fixed Placements */
    GtkWidget *fixed;
    fixed = gtk_fixed_new(); /* creating "fixed" with no adjustments */

    /* Vertical Boxes */
    GtkWidget *vBox;
    vBox = gtk_vbox_new(FALSE, 0); /* (same size?, spacing) */

    /* Horizontal Boxes */
    GtkWidget *hBox;
    hBox = gtk_hbox_new(FALSE, 0);

    GtkWidget *hBox2;
    hBox2 = gtk_hbox_new(FALSE, 20);

    /* Vertical Separator */
    GtkWidget *vLine; /* vertical line separator (just for formatting */
    vLine = gtk_vseparator_new();

    GtkWidget *vLine2;
    vLine2 = gtk_vseparator_new();

    /* Frame */
    GtkWidget *frame;
    frame = gtk_frame_new("TOPH Chat");                                /* add frame around */
    gtk_frame_set_label_align(GTK_FRAME(frame), .5, .5);               /* put label in center */
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN); /* setting shadow type */

    GtkWidget *friendListFrame;
    friendListFrame = gtk_frame_new("");
    gtk_frame_set_label_align(GTK_FRAME(friendListFrame), .5, .5);
    gtk_container_set_border_width(GTK_CONTAINER(friendListFrame), 0);

    /* Option Menu */
    GtkWidget *options;
    options = gtk_menu_new(); /* Options menu */
    gtk_menu_set_title(GTK_MENU(options), "Options");

    GtkWidget *messageButton; /* message button */
    messageButton = gtk_menu_item_new_with_label("Message");

    GtkWidget *blockButton; /* block button */
    blockButton = gtk_menu_item_new_with_label("Block");

    gtk_menu_attach(GTK_MENU(options), messageButton, 0, 1, 0, 1);
    gtk_menu_attach(GTK_MENU(options), blockButton, 0, 1, 1, 2);

    /* Expander */
    GtkWidget *expander;
    expander = gtk_expander_new("Friends List");

    /* Friends List */
    GtkWidget *friendScrollWindow;

    GtkWidget *addFriend;
    addFriend = gtk_button_new_with_label("Add Friend");
    gtk_widget_show(addFriend);

    GtkWidget *createRoomButton;
    createRoomButton = gtk_button_new_with_label("Create Room");
    gtk_widget_show(createRoomButton);

    GtkWidget *friendVBox;
    friendVBox = gtk_vbox_new(FALSE, 10);
    gtk_widget_show(friendVBox);

    /************************************************************************************************/

    /************************************/
    /****    MAIN PROGRAM    ***********/
    /**********************************/

    gtk_box_pack_start(GTK_BOX(hBox), textBox, FALSE, TRUE, FALSE);     /* HBOX textBox */
    gtk_box_pack_start(GTK_BOX(hBox), emojiButton, FALSE, TRUE, FALSE); /* HBOX textBox, emojiButton */
    gtk_box_pack_start(GTK_BOX(hBox), sendButton, FALSE, TRUE, FALSE);  /* HBOX textBox, emojiButton, sendButton */

    gtk_box_pack_start(GTK_BOX(vBox), tabs, TRUE, TRUE, FALSE);  /* VBOX tabs */
    gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, FALSE); /* VBOX tabs, HBOX */

    //    gtk_box_pack_start(GTK_BOX(friendVBox), friendScrollWindow, FALSE, FALSE, 0); /* packing friend list box with addFriend button */
    //    gtk_box_pack_start(GTK_BOX(friendVBox), addFriend, FALSE, FALSE, 0);
    //    gtk_box_pack_start(GTK_BOX(friendVBox), createRoomButton, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(hBox2), vBox, FALSE, FALSE, FALSE);      /* HBOX vBox */
    gtk_box_pack_end(GTK_BOX(hBox2), vLine, FALSE, TRUE, FALSE);        /* HBOX vBox, vLine */
    gtk_box_pack_start(GTK_BOX(hBox2), friendVBox, FALSE, TRUE, FALSE); /* HBOX vBox, vLine, friendslist */

    /* final packing */
    gtk_container_add(GTK_CONTAINER(frame), hBox2);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 10);

    gtk_fixed_put(GTK_FIXED(fixed), frame, 0, 0); /* placing container in top left corner */

    gtk_container_add(GTK_CONTAINER(window), fixed); /* adding container to window */

    /***** Widget Arrays *******/
    GtkWidget *messageWidgetArray[3];
    messageWidgetArray[1] = messageScreen;
    messageWidgetArray[2] = textBox;

    GtkWidget *tabsArray[3];
    tabsArray[0] = tabs;
    tabsArray[1] = messageScreen0;
    tabsArray[2] = tabLabel0;

    /* Friend 0 */
    /*    GtkWidget *optionsArray0[7];
    optionsArray0[0] = options;
    optionsArray0[1] = messageButton;
    optionsArray0[2] = tabs;
    optionsArray0[3] = scrollWindow0;
    optionsArray0[4] = messageScreen0;
    optionsArray0[5] = tabLabel0;
    optionsArray0[6] = blockButton;
*/ /* Friend 1 */
    /*    GtkWidget *optionsArray1[7];
    optionsArray1[0] = options;
    optionsArray1[1] = messageButton;
    optionsArray1[2] = tabs;
    optionsArray1[3] = scrollWindow1;
    optionsArray1[4] = messageScreen1;
    optionsArray1[5] = tabLabel1;
    optionsArray1[6] = blockButton;
*/ /* Friend 2 */
    /*    GtkWidget *optionsArray2[7];
    optionsArray2[0] = options;
    optionsArray2[1] = messageButton;
    optionsArray2[2] = tabs;
    optionsArray2[3] = scrollWindow2;
    optionsArray2[4] = messageScreen2;
    optionsArray2[5] = tabLabel2;
    optionsArray2[6] = blockButton;
*/ /* Friend 3 */
    /*    GtkWidget *optionsArray3[7];
    optionsArray3[0] = options;
    optionsArray3[1] = messageButton;
    optionsArray3[2] = tabs;
    optionsArray3[3] = scrollWindow3;
    optionsArray3[4] = messageScreen3;
    optionsArray3[5] = tabLabel3;
    optionsArray3[6] = blockButton;
*/ /* Friend 4 */
    /*    GtkWidget *optionsArray4[7];
    optionsArray4[0] = options;
    optionsArray4[1] = messageButton;
    optionsArray4[2] = tabs;
    optionsArray4[3] = scrollWindow4;
    optionsArray4[4] = messageScreen4;
    optionsArray4[5] = tabLabel4;
    optionsArray4[6] = blockButton;
*/ /* Friend 5 */
    /*    GtkWidget *optionsArray5[7];
    optionsArray5[0] = options;
    optionsArray5[1] = messageButton;
    optionsArray5[2] = tabs;
    optionsArray5[3] = scrollWindow5;
    optionsArray5[4] = messageScreen5;
    optionsArray5[5] = tabLabel5;
    optionsArray5[6] = blockButton;
*/ /* Friend 6 */
    /*    GtkWidget *optionsArray6[7];
    optionsArray6[0] = options;
    optionsArray6[1] = messageButton;
    optionsArray6[2] = tabs;
    optionsArray6[3] = scrollWindow6;
    optionsArray6[4] = messageScreen6;
    optionsArray6[5] = tabLabel6;
    optionsArray6[6] = blockButton;
*/ /* Friend 7 */
    /*    GtkWidget *optionsArray7[7];
    optionsArray7[0] = options;
    optionsArray7[1] = messageButton;
    optionsArray7[2] = tabs;
    optionsArray7[3] = scrollWindow7;
    optionsArray7[4] = messageScreen7;
    optionsArray7[5] = tabLabel7;
    optionsArray7[6] = blockButton;
*/ /* Friend 8 */
    /*    GtkWidget *optionsArray8[7];
    optionsArray8[0] = options;
    optionsArray8[1] = messageButton;
    optionsArray8[2] = tabs;
    optionsArray8[3] = scrollWindow8;
    optionsArray8[4] = messageScreen8;
    optionsArray8[5] = tabLabel8;
    optionsArray8[6] = blockButton;
*/ /* Friend 9 */
    /*    GtkWidget *optionsArray9[7];
    optionsArray9[0] = options;
    optionsArray9[1] = messageButton;
    optionsArray9[2] = tabs;
    optionsArray9[3] = scrollWindow9;
    optionsArray9[4] = messageScreen9;
    optionsArray9[5] = tabLabel9;
    optionsArray9[6] = blockButton;
*/
    /****** CLIENT SERVER INITIALIZATION ******/

    // room stuffs bypassing room asking
    roomList *AllRoom = roomsetInit();
    (AllRoom->roomList[0]).status = ROOM_WAITING;
    receiveRoom(AllRoom, 0);
    AllRoom->roomList[0].roomNum = 0;
    inboxInformationStruct *infoStruct = createInboxInfoStruct();

    fifo *outputBuffer = initBuffer(CLIENT_OUTPUT_FIFO_MAX);
    inboxQueue *inbox = initInboxQueue();

    char **allFriend = createFriendList(MAX_USER_FRIEND);
    char **allOnlineFriend = createFriendList(MAX_USER_FRIEND);

    /* server initialization */
    serverConnection *server = openConnection();
    assert(server);
    infoStruct->server = server;
    infoStruct->outputBuffer = outputBuffer;
    /* Message Struct Initialization */
    MESSAGE_STRUCT *messageStruct;
    messageStruct = CreateMessageStruct(vBox, window, server, AllRoom, outputBuffer, inbox, "", "");

    /* Login Struct */
    MESSAGE_STRUCT *loginStruct;
    loginStruct = CreateMessageStruct(loginVBox, loginScreen, server, AllRoom, outputBuffer, inbox, "", "");

    /* Account Struct */
    MESSAGE_STRUCT *accountStruct;
    accountStruct = CreateMessageStruct(accountCreationVBox, accountCreationScreen, server, AllRoom, outputBuffer, inbox, "", "");

    /* MessageStruct Array for Login */
    MESSAGE_STRUCT *messageStructArray[2];
    messageStructArray[0] = messageStruct;
    messageStructArray[1] = loginStruct;

    /* CreateRoomStruct */
    MESSAGE_STRUCT *createRoomStruct;
    createRoomStruct = CreateMessageStruct(tabs, NULL, server, AllRoom, outputBuffer, inbox, "", "");
    
     /* Friend List Struct */
    MESSAGE_STRUCT *friendListStruct;
    friendListStruct = CreateMessageStruct(tabs, NULL, server, AllRoom, outputBuffer, inbox, "", "");

    MESSAGE_STRUCT *addFriendStruct;
    addFriendStruct = CreateMessageStruct(NULL, NULL, NULL, AllRoom, outputBuffer, inbox, "", "");

    /* message struct for event handling in main while loop */
    MESSAGE_STRUCT *notiStruct;
    notiStruct = CreateMessageStruct(NULL, NULL, server, AllRoom, outputBuffer, inbox, "", "");

    friendScrollWindow = create_online_friend_list(friendListStruct);
    gtk_widget_set_size_request(friendScrollWindow, 200, 400);
    gtk_box_pack_start(GTK_BOX(friendVBox), friendScrollWindow, FALSE, FALSE, 0); /* packing friend list box with addFriend button */
    gtk_box_pack_start(GTK_BOX(friendVBox), addFriend, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(friendVBox), createRoomButton, FALSE, FALSE, 0);

    /**** SIGNALS ********/
    g_signal_connect(window, "delete-event", G_CALLBACK(CloseWindow), NULL);                                                     /* deletes window */
                                                                                                                                 //    g_signal_connect(accept, "clicked", G_CALLBACK(AcceptMessage), messagePopupScreen);
                                                                                                                                 //    g_signal_connect(messagePopupScreen, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete(messagePopupScreen)), NULL);       /* deletes window */
    g_signal_connect(accountCreationScreen, "delete-event", G_CALLBACK(Hide), accountCreationScreen); /* deletes window */
    g_signal_connect(loginScreen, "delete-event", G_CALLBACK(LoginExit), NULL);                     /* hides window */

    /* Create Account Signals */
    g_signal_connect(newUsername, "key-press-event", G_CALLBACK(Overwrite), newUsername);
    g_signal_connect(newPassword, "key-press-event", G_CALLBACK(HideCharacters), showPasswordCheckBox);
    g_signal_connect(newPasswordConfirm, "key-press-event", G_CALLBACK(HideCharacters), showPasswordCheckBox);
    g_signal_connect(showPasswordCheckBox, "toggled", G_CALLBACK(ShowCharacters), accountCreationVBox);
    g_signal_connect(clearForm, "clicked", G_CALLBACK(ClearForm), accountCreationVBox);
    g_signal_connect(createAccountButton, "clicked", G_CALLBACK(CreateNewAccount), accountStruct);

    /* Messaging Signals */
    g_signal_connect(textBox, "activate", G_CALLBACK(EnterKey), (gpointer)messageStruct);     /* to send message with enter key */
    g_signal_connect(sendButton, "clicked", G_CALLBACK(SendButton), (gpointer)messageStruct); /* to send message with "Send" button */

    /* option clicks for all of friends on list */
    //    g_signal_connect(friends[0], "clicked", G_CALLBACK(OptionsPopup), optionsArray0); /* opens up options popup */
    /*    g_signal_connect(friends[1], "clicked", G_CALLBACK(OptionsPopup), optionsArray1);
    g_signal_connect(friends[2], "clicked", G_CALLBACK(OptionsPopup), optionsArray2);
    g_signal_connect(friends[3], "clicked", G_CALLBACK(OptionsPopup), optionsArray3);
    g_signal_connect(friends[4], "clicked", G_CALLBACK(OptionsPopup), optionsArray4);
    g_signal_connect(friends[5], "clicked", G_CALLBACK(OptionsPopup), optionsArray5);
    g_signal_connect(friends[6], "clicked", G_CALLBACK(OptionsPopup), optionsArray6);
    g_signal_connect(friends[7], "clicked", G_CALLBACK(OptionsPopup), optionsArray7);
    g_signal_connect(friends[8], "clicked", G_CALLBACK(OptionsPopup), optionsArray8);
    g_signal_connect(friends[9], "clicked", G_CALLBACK(OptionsPopup), optionsArray9);
*/
    /* login signals */
    g_signal_connect(quitButton, "clicked", G_CALLBACK(LoginExit), NULL); /* if the user quits */
    g_signal_connect(createAccount, "clicked", G_CALLBACK(CreateAccount), accountCreationScreen);
    g_signal_connect(loginButton, "clicked", G_CALLBACK(Login), (gpointer)messageStructArray); /* if the user wants to log in */

    /* friend List signals */
    g_signal_connect(createRoomButton, "clicked", G_CALLBACK(CreateNewTab), createRoomStruct);              /* create new room */
    g_signal_connect(addFriend, "clicked", G_CALLBACK(friend_invite_clicked), addFriendStruct); /* add new friend */

    /****** SHOWING WIDGETS ******/
    gtk_widget_show(scrollWindow);
    gtk_widget_show(expander);
    gtk_widget_show(blockButton);
    gtk_widget_show(messageButton);
    gtk_widget_show(options);
    gtk_widget_show(friendListFrame);
    gtk_widget_show(frame);
    gtk_widget_show(vLine2);
    //    gtk_widget_show(friendsList);
    gtk_widget_show(vLine);
    gtk_widget_show(hBox2);
    gtk_widget_show(emojiButton);
    gtk_widget_show(sendButton);
    gtk_widget_show(messageScreen);
    gtk_widget_show(textBox);
    gtk_widget_show(tabLabel);
    gtk_widget_show(tabs);
    gtk_widget_show(fixed);
    gtk_widget_show(vBox);
    gtk_widget_show(hBox);
    //    gtk_widget_show(window);
    gtk_widget_show(loginScreen);

    gtk_widget_show(friendScrollWindow);

    GtkTextBuffer *updateBuffer;
    GtkTextIter updateIter;
    GtkTextMark *updateStart;
    //char message[PACKAGE_SIZE];

    char senderName[MAX_USER_NAME];
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";
    infoStruct->inbox = inbox;
    writeBuffer(infoStruct->inbox->messageQueue, "test1");
    char testData[30];
    readBuffer(inbox->messageQueue, testData);
    printf("\nthe test data is %s\n", testData);
    infoStruct->allFriendList = allFriend;
    infoStruct->friendList = allOnlineFriend;
    assert(infoStruct->inbox==inbox);
    GList *child;
    GtkWidget *friendView;
    child = gtk_container_get_children(GTK_CONTAINER(friendScrollWindow));
    friendView = child->data;
    int prevTime = time(0);
    
    while (!client_shutdown)
    {

        UpdateWindow(); /* main event loop */
        sendToServer(outputBuffer, server);
        if (isLogin)
        {
            /* assign username to struct */
            strncpy(createRoomStruct->username,  messageStruct->username, sizeof(char)*MAX_USER_NAME);
            createRoomStruct->username[MAX_USER_NAME-1] = '\0';
            
            strncpy(friendListStruct->username,  messageStruct->username, sizeof(char)*MAX_USER_NAME);
            friendListStruct->username[MAX_USER_NAME-1] = '\0';
            
            strncpy(addFriendStruct->username,  messageStruct->username, sizeof(char)*MAX_USER_NAME);
            addFriendStruct->username[MAX_USER_NAME-1] = '\0';
            
            strncpy(notiStruct->username,  messageStruct->username, sizeof(char)*MAX_USER_NAME);
            notiStruct->username[MAX_USER_NAME-1] = '\0';
            if(strcmp(messageStruct->username, "user1")==0)
            {
                strncpy(notiStruct->targetName,  "user2", sizeof(char)*MAX_USER_NAME);
            }
            else
            {
                strncpy(notiStruct->targetName,  "user1", sizeof(char)*MAX_USER_NAME);
            }
            
            // server communication
            recvMessageFromServer(AllRoom, inbox, server);
            infoStruct->messageStruct = notiStruct; // pass message struct into the parse inbox command function
            parseInboxCommand(infoStruct); /* there is a problem with free in here -> handleCO -> getUserFriendList*/
            if (time(0) - prevTime > 5)
            {
                getAllFriend(notiStruct->username, outputBuffer);
                printf("\nthe list of friend is %s \n", allFriend[0]);
                if(allFriend[0]!=NULL)
                {
                    update_online_friend_model(friendView, allFriend);
                }
                prevTime = time(0);
            }
            sendToServer(outputBuffer, server);

            /*** update message ****/
            if (fetchMessage(&(AllRoom->roomList[0]), packet) >= 0)
            {
                getMessageBody(packet, message);
                getSenderName(senderName, packet);

                updateBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen)); /* gets the buffer for the current screen */

                gtk_text_buffer_get_iter_at_offset(updateBuffer, &updateIter, -1); /* get mark at the end */

                gtk_text_buffer_insert(updateBuffer, &updateIter, "\n\n", -1); /* insert new lines */

                gtk_text_buffer_insert(updateBuffer, &updateIter, senderName, -1); /* adds "username: " */

                gtk_text_buffer_insert(updateBuffer, &updateIter, ": ", 2); /* adds ": " */

                gtk_text_buffer_get_iter_at_offset(updateBuffer, &updateIter, -1); /* get mark at end again */

                /* get mark for emoji display at the point before user text is inserted */
                updateStart = gtk_text_buffer_create_mark(updateBuffer, NULL, &updateIter, TRUE);

                gtk_text_buffer_insert(updateBuffer, &updateIter, message, -1); /* inserts user text */

                /* display emoji on messageScreen */
                insert_emoji(GTK_TEXT_VIEW(messageScreen), updateStart);

                // gtk_entry_set_text(GTK_ENTRY(textBox), ""); /* replaces textBox with empty text again */
            }
        }
    }
    return 0;
}
