/******************************************
 * Programmer: Jason Duong                *
 * Affiliated with: UCI                   *
 * Date Created: 2/28/18                  *
 * Last modification: 3/11/2018, 3:51 AM  *
 * ****************************************/

#include <stdio.h>
#include <gtk/gtk.h>

#include "GTK.h"
#include "tcpGUI.h"

int main(int argc, char *argv[])
{

    /************************************/
    /*****   INITIALIZATION   **********/
    /**********************************/
    GtkWidget *window; /* declaring a window */

    char name[50];

    g_print("Enter your name: ");
    scanf("%50s", &name);

    gtk_init(&argc, &argv); /* initializing GTK environment */

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); /* initialize window to be on top */

    gtk_window_set_title(GTK_WINDOW(window), "Toph Chat Alpha Version"); /* make title for window */
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);            /* sets the width from the inside */

    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    g_signal_connect(window, "delete-event", G_CALLBACK(CloseWindow), (gpointer)name); /* deletes window */
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
    g_signal_connect(loginScreen, "delete-event", G_CALLBACK(CloseWindow), (gpointer)name); /* deletes window */

    /* frame */
    GtkWidget *loginFrame;
    loginFrame = gtk_frame_new("Toph Chat");

    /* Labels */
    GtkWidget *usernameLabel;
    usernameLabel = gtk_label_new("Username: ");

    GtkWidget *passwordLabel;
    passwordLabel = gtk_label_new("Password: ");

    GtkWidget *createAccountLabel;
    createAccountLabel = gtk_label_new("Don't have an account? Create a new one");

    GtkWidget *forgotAccountLabel;
    forgotAccountLabel = gtk_label_new("Forgot account? Recover it");

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

    GtkWidget *forgotAccountBox;
    forgotAccountBox = gtk_hbox_new(FALSE, 0);

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

    GtkWidget *forgotAccount;
    forgotAccount = gtk_button_new_with_label("here!");
    gtk_button_set_relief(GTK_BUTTON(forgotAccount), GTK_RELIEF_NONE);

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
    gtk_box_pack_start(GTK_BOX(forgotAccountBox), forgotAccountLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(forgotAccountBox), forgotAccount, FALSE, FALSE, 0);

    /* account box */
    gtk_box_pack_start(GTK_BOX(accountVBox), createAccountBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(accountVBox), forgotAccountBox, FALSE, FALSE, 0);

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
    gtk_widget_show(forgotAccount);
    gtk_widget_show(forgotAccountBox);
    gtk_widget_show(forgotAccountLabel);
    gtk_widget_show(createAccount);
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
    g_signal_connect(accountCreationScreen, "delete-event", G_CALLBACK(CloseWindow), (gpointer)name); /* deletes window */

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

    /** final packing **/
    gtk_container_add(GTK_CONTAINER(accountFrame), accountCreationVBox);
    gtk_container_add(GTK_CONTAINER(accountCreationScreen), accountFrame);

    /**** SIGNALS ****/
    g_signal_connect(newUsername, "key-press-event", G_CALLBACK(Overwrite), newUsername);
    g_signal_connect(newPassword, "key-press-event", G_CALLBACK(HideCharacters), showPasswordCheckBox);
    g_signal_connect(newPasswordConfirm, "key-press-event", G_CALLBACK(HideCharacters), showPasswordCheckBox);
    g_signal_connect(showPasswordCheckBox, "toggled", G_CALLBACK(ShowCharacters), accountCreationVBox);
    g_signal_connect(clearForm, "clicked", G_CALLBACK(ClearForm), accountCreationVBox);

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
    gtk_widget_show(accountCreationVBox);
    //    gtk_widget_show(accountCreationScreen);

    /*************************************************************************************************/
    /*******************************/
    /******   Message Popup  ******/
    /*****************************/
    GtkWidget *messagePopupScreen;

    messagePopupScreen = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(messagePopupScreen), "New Message Request!"); /* sets title of window */
    gtk_widget_set_size_request(messagePopupScreen, 270, 100);
    gtk_container_set_border_width(GTK_CONTAINER(messagePopupScreen), 10);
    gtk_window_set_resizable(GTK_WINDOW(messagePopupScreen), FALSE);
    g_signal_connect(messagePopupScreen, "delete-event", G_CALLBACK(CloseWindow), (gpointer)name); /* deletes window */

    /* labels */
    GtkWidget *newMessage;
    newMessage = gtk_label_new("You've received a new message!");

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

    /* SIGNALS */
    g_signal_connect(accept, "clicked", G_CALLBACK(AcceptMessage), messagePopupScreen);

    /* Show widgets */
    gtk_widget_show(newMessageVBox);
    gtk_widget_show(newMessageHBox);
    gtk_widget_show(reject);
    gtk_widget_show(accept);
    gtk_widget_show(newMessage);
    //    gtk_widget_show(messagePopupScreen);

    /*******************************/
    /*****      VARIABLES     *****/
    /*****************************/

    /* Notebook Variables */
    GtkWidget *tabs;
    tabs = gtk_notebook_new();                             /* initializing tabs */
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(tabs), TRUE); /* make the tabs scrollable */

    int success = 0;

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

    GtkWidget *friends[10];
    for (int i = 0; i < 10; i++) /* creating new friend buttons */
    {
        friends[i] = gtk_button_new_with_label("[Friend]");
        gtk_button_set_relief(GTK_BUTTON(friends[i]), GTK_RELIEF_NONE);
        gtk_widget_set_size_request(friends[i], 100, 30);
        gtk_widget_show(friends[i]);
    }

    /* Fixed Placements */
    GtkWidget *fixed;
    fixed = gtk_fixed_new(); /* creating "fixed" with no adjustments */

    /* Vertical Boxes */
    GtkWidget *vBox;
    vBox = gtk_vbox_new(FALSE, 0); /* (same size?, spacing) */

    GtkWidget *friendsList; /* making friends list vbox*/
    friendsList = gtk_vbox_new(TRUE, 2);

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

    /************************************************************************************************/

    /************************************/
    /****    MAIN PROGRAM    ***********/
    /**********************************/

    gtk_box_pack_start(GTK_BOX(hBox), textBox, FALSE, TRUE, FALSE);  /* HBOX textBox */
    gtk_box_pack_end(GTK_BOX(hBox), sendButton, FALSE, TRUE, FALSE); /* HBOX textBox, sendButton */

    gtk_box_pack_start(GTK_BOX(vBox), tabs, TRUE, TRUE, FALSE);  /* VBOX tabs */
    gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, FALSE); /* VBOX tabs, HBOX */

    gtk_box_pack_start(GTK_BOX(friendsList), friends[0], TRUE, TRUE, FALSE); /* building friends list */
    for (int i = 1; i < 9; i++)
    {
        gtk_box_pack_end(GTK_BOX(friendsList), friends[i], TRUE, TRUE, FALSE);
    }
    gtk_container_add(GTK_CONTAINER(friendListFrame), friendsList); /* framing friends list */

    gtk_container_add(GTK_CONTAINER(expander), friendListFrame); /* putting friendsList into an expanding box */

    gtk_box_pack_start(GTK_BOX(hBox2), vBox, FALSE, FALSE, FALSE);    /* HBOX vBox */
    gtk_box_pack_end(GTK_BOX(hBox2), vLine, FALSE, TRUE, FALSE);      /* HBOX vBox, vLine */
    gtk_box_pack_start(GTK_BOX(hBox2), expander, FALSE, TRUE, FALSE); /* HBOX vBox, vLine, friendslist */

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
    GtkWidget *optionsArray0[7];
    optionsArray0[0] = options;
    optionsArray0[1] = messageButton;
    optionsArray0[2] = tabs;
    optionsArray0[3] = scrollWindow0;
    optionsArray0[4] = messageScreen0;
    optionsArray0[5] = tabLabel0;
    optionsArray0[6] = blockButton;
    /* Friend 1 */
    GtkWidget *optionsArray1[7];
    optionsArray1[0] = options;
    optionsArray1[1] = messageButton;
    optionsArray1[2] = tabs;
    optionsArray1[3] = scrollWindow1;
    optionsArray1[4] = messageScreen1;
    optionsArray1[5] = tabLabel1;
    optionsArray1[6] = blockButton;
    /* Friend 2 */
    GtkWidget *optionsArray2[7];
    optionsArray2[0] = options;
    optionsArray2[1] = messageButton;
    optionsArray2[2] = tabs;
    optionsArray2[3] = scrollWindow2;
    optionsArray2[4] = messageScreen2;
    optionsArray2[5] = tabLabel2;
    optionsArray2[6] = blockButton;
    /* Friend 3 */
    GtkWidget *optionsArray3[7];
    optionsArray3[0] = options;
    optionsArray3[1] = messageButton;
    optionsArray3[2] = tabs;
    optionsArray3[3] = scrollWindow3;
    optionsArray3[4] = messageScreen3;
    optionsArray3[5] = tabLabel3;
    optionsArray3[6] = blockButton;
    /* Friend 4 */
    GtkWidget *optionsArray4[7];
    optionsArray4[0] = options;
    optionsArray4[1] = messageButton;
    optionsArray4[2] = tabs;
    optionsArray4[3] = scrollWindow4;
    optionsArray4[4] = messageScreen4;
    optionsArray4[5] = tabLabel4;
    optionsArray4[6] = blockButton;
    /* Friend 5 */
    GtkWidget *optionsArray5[7];
    optionsArray5[0] = options;
    optionsArray5[1] = messageButton;
    optionsArray5[2] = tabs;
    optionsArray5[3] = scrollWindow5;
    optionsArray5[4] = messageScreen5;
    optionsArray5[5] = tabLabel5;
    optionsArray5[6] = blockButton;
    /* Friend 6 */
    GtkWidget *optionsArray6[7];
    optionsArray6[0] = options;
    optionsArray6[1] = messageButton;
    optionsArray6[2] = tabs;
    optionsArray6[3] = scrollWindow6;
    optionsArray6[4] = messageScreen6;
    optionsArray6[5] = tabLabel6;
    optionsArray6[6] = blockButton;
    /* Friend 7 */
    GtkWidget *optionsArray7[7];
    optionsArray7[0] = options;
    optionsArray7[1] = messageButton;
    optionsArray7[2] = tabs;
    optionsArray7[3] = scrollWindow7;
    optionsArray7[4] = messageScreen7;
    optionsArray7[5] = tabLabel7;
    optionsArray7[6] = blockButton;
    /* Friend 8 */
    GtkWidget *optionsArray8[7];
    optionsArray8[0] = options;
    optionsArray8[1] = messageButton;
    optionsArray8[2] = tabs;
    optionsArray8[3] = scrollWindow8;
    optionsArray8[4] = messageScreen8;
    optionsArray8[5] = tabLabel8;
    optionsArray8[6] = blockButton;
    /* Friend 9 */
    GtkWidget *optionsArray9[7];
    optionsArray9[0] = options;
    optionsArray9[1] = messageButton;
    optionsArray9[2] = tabs;
    optionsArray9[3] = scrollWindow9;
    optionsArray9[4] = messageScreen9;
    optionsArray9[5] = tabLabel9;
    optionsArray9[6] = blockButton;

    /* login */
    GtkWidget *loginArray[3];
    loginArray[0] = loginVBox;
    loginArray[1] = loginScreen;
    loginArray[2] = window;

    /**** SIGNALS ********/
    g_signal_connect(textBox, "activate", G_CALLBACK(EnterKey), tabs);     /* to send message with enter key */
    g_signal_connect(sendButton, "clicked", G_CALLBACK(SendButton), vBox); /* to send message with "Send" button */

    /* option clicks for all of friends on list */
    g_signal_connect(friends[0], "clicked", G_CALLBACK(OptionsPopup), optionsArray0); /* opens up options popup */
    g_signal_connect(friends[1], "clicked", G_CALLBACK(OptionsPopup), optionsArray1);
    g_signal_connect(friends[2], "clicked", G_CALLBACK(OptionsPopup), optionsArray2);
    g_signal_connect(friends[3], "clicked", G_CALLBACK(OptionsPopup), optionsArray3);
    g_signal_connect(friends[4], "clicked", G_CALLBACK(OptionsPopup), optionsArray4);
    g_signal_connect(friends[5], "clicked", G_CALLBACK(OptionsPopup), optionsArray5);
    g_signal_connect(friends[6], "clicked", G_CALLBACK(OptionsPopup), optionsArray6);
    g_signal_connect(friends[7], "clicked", G_CALLBACK(OptionsPopup), optionsArray7);
    g_signal_connect(friends[8], "clicked", G_CALLBACK(OptionsPopup), optionsArray8);
    g_signal_connect(friends[9], "clicked", G_CALLBACK(OptionsPopup), optionsArray9);

    /* login signals */
    g_signal_connect(quitButton, "clicked", G_CALLBACK(LoginExit), NULL); /* if the user quits */
    g_signal_connect(createAccount, "clicked", G_CALLBACK(CreateAccount), accountCreationScreen);
    g_signal_connect(loginButton, "clicked", G_CALLBACK(Login), loginArray); /* if the user wants to log in */

    /* message signals */
    //    g_signal_connect(

    /****** SHOWING WIDGETS ******/
    gtk_widget_show(scrollWindow);
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
    //    gtk_widget_show(window);
    gtk_widget_show(loginScreen);

    printf("hello \n");

    gtk_main(); /* main event loop */

    return 0;
}
