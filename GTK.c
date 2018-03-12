#include <gtk/gtk.h>
#include "GTK.h"
#include "tcpGUI.h"

#include <assert.h>
#include <string.h>

gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("You exited out the window! \n");
    g_print("See you later, %s!\n", (gchar *)data);
    gtk_main_quit(); /* leaves event loop */
    return FALSE;    /* return false to destroy window */
}

gboolean LoginExit(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_print("\nSee you later! \n");
    gtk_main_quit();
    return FALSE;
}

void Login(GtkWidget *widget, GtkWidget *vBox[])
{
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

    gtk_widget_destroy(vBox[1]);
    gtk_widget_show(vBox[2]);
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

void EnterKey(GtkWidget *entry, GtkWidget *tabs)
{
    GtkTextIter iter;
    guint16 check = 0;
    int currentPage = 0;
    GtkWidget *scrolledWindow;
    GList *child;

    check = gtk_entry_get_text_length(GTK_ENTRY(entry));                         /* checking length of text input */
    currentPage = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));             /* get current page */
    scrolledWindow = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), currentPage); /* notebook child */
    child = gtk_container_get_children(GTK_CONTAINER(scrolledWindow));

    if (check != 0) /* only run this if there is a text input */
    {
        GtkTextBuffer *buffer;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(child->data)); /* gets the buffer for the current screen */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at the end */

        gtk_text_buffer_insert(buffer, &iter, "\n\n", -1); /* insert new lines */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at end again */

        gtk_text_buffer_insert(buffer, &iter, gtk_entry_get_text(GTK_ENTRY(entry)), -1); /* inserts user text */

        gtk_entry_set_text(GTK_ENTRY(entry), ""); /* replaces textBox with empty text again */
    }
}

void SendButton(GtkWidget *widget, GtkWidget *vBox)
{
    GtkTextIter iter;
    GtkWidget *tabs;
    GtkWidget *messageScreen;
    GtkWidget *scrolledWindow;
    GList *child;
    int currentPage = 0;
    GList *list;
    GList *list2;

    guint16 check = 0;

    /* list->data = tabs, list->next->data = hbox*/
    list = gtk_container_get_children(GTK_CONTAINER(vBox));              /* list of vBox: tabs, hBox */
    list2 = gtk_container_get_children(GTK_CONTAINER(list->next->data)); /* list of hBox: textBox, sendButton */

    tabs = list->data;
    currentPage = gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs));             /* getting current page */
    scrolledWindow = gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs), currentPage); /* getting scrolled view */
    child = gtk_container_get_children(GTK_CONTAINER(scrolledWindow));           /* child->data is messageScreen */

    messageScreen = child->data;

    check = gtk_entry_get_text_length(GTK_ENTRY(list2->data)); /* checking length of text input */

    if (check != 0) /* only run this if there is a text input */
    {
        GtkTextBuffer *buffer;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageScreen)); /* gets the buffer for the current screen */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at the end */

        gtk_text_buffer_insert(buffer, &iter, "\n\n", -1); /* insert new lines */

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1); /* get mark at end again */

        gtk_text_buffer_insert(buffer, &iter, gtk_entry_get_text(GTK_ENTRY(list2->data)), -1); /* inserts user text */

        gtk_entry_set_text(GTK_ENTRY(list2->data), ""); /* replaces textBox with empty text again */
    }
}

void OptionsPopup(GtkWidget *button, GtkWidget *options[])
{
    /*    GtkWidget *optionsArray0[7];
    optionsArray0[0] = options;
    optionsArray0[1] = messageButton;
    optionsArray0[2] = tabs;
    optionsArray0[3] = scrollWindow0;
    optionsArray0[4] = messageScreen0;
    optionsArray0[5] = tabLabel0;
    optionsArray0[6] = blockButton;
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
