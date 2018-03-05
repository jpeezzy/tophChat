#ifndef GTK
#define GTK
#include <gtk/gtk.h>


gboolean CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data);    /* Exits out of the window and the program */
//void click(GtkWidget *widget, GdkEvent *event, gpointer data);
//GtkWidget *CreateBox(GtkWidget *window, GtkWidget *button);
void EnterKey(GtkWidget *entry, GtkWidget *messageScreen);  /* when the user presses enter in the textBox */
void SendButton(GtkWidget *widget, GtkWidget *widgets[]);   /* when user presses send button */
void OptionsPopup(GtkWidget *button, GtkWidget *options[]); /* option box pops up when friend is clicked on */
void MessageUser(GtkWidget *widget, GtkWidget *tabCreation[]);    /* when "message" button pressed. creates new tab for the friend" */
#endif 
