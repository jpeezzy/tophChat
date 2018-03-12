#ifndef EMOJI_H
#define EMOJI_H

#include <gtk/gtk.h>

typedef struct emoji_struct EMOJI_STRUCT;

struct emoji_struct
{
    char *filename;
    GtkTextView *view;
};

EMOJI_STRUCT* create_emoji_struct(char* filename, GtkTextView *view);

void delete_emoji_struct(EMOJI_STRUCT *emoji_struct);

GtkWidget* emoji_popup(GtkTextView *view);

void insert_emoji (GtkTextView *view, GtkTextMark *start);

#endif 
