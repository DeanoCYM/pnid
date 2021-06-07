/* This file is part of pnid
   Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_appwin.c - pnid application window class definition */

#include <gtk/gtk.h>

#include "pnid_app.h"
#include "pnid_appwin.h"

/* #PnidAppWin - pnid application window GObject class definition */
struct _PnidAppWindow {
    GtkApplicationWindow parent;

    /* instance members */
};

G_DEFINE_TYPE(PnidAppWindow, pnid_app_window, GTK_TYPE_APPLICATION_WINDOW);

/* Interface */
PnidAppWindow *pnid_app_window_new(PnidApp *app);
void pnid_app_window_open (PnidAppWindow *win, GFile *file);
/* Constructors */
static void pnid_app_window_class_init(PnidAppWindowClass *class);
static void pnid_app_window_init(PnidAppWindow *self);
    
/* pnid_app_window_new(): interface for creating a new empty pnid
   application window. */
PnidAppWindow *
pnid_app_window_new(PnidApp *app)
{
    return g_object_new(PNID_APP_WINDOW_TYPE,
			"application", app,
			NULL);
}

/* pnid_app_window_open(): interface for creating a new window with
   file contents loaded. */
void
pnid_app_window_open(PnidAppWindow *win, GFile *file)
{
    // need to load the file contents and display here.
    return;
}

/* pnid_app_window_class_init(): pnid application window class
   constructor, executed only once before the first instance is
   constructed.*/
static void
pnid_app_window_class_init(PnidAppWindowClass *class)
{
    return;
}

/* pnid_app_window_init(): pnid application window object constructor,
   instantiates the class and stores the resulting object at the
   address pointed to by self. */
static void
pnid_app_window_init(PnidAppWindow *class)
{
    return; 
}
