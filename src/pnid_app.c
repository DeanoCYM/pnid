/* This file is part of pnid
   Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_app.c - pnid application class definition */

#include <gtk/gtk.h>

#include "pnid_app.h" 
#include "pnid_appwin.h"

/* #PnidApp - pnid application GObject class definition */
struct _PnidApp {
    GtkApplication parent;

    /* instance members */
};
G_DEFINE_TYPE(PnidApp, pnid_app, GTK_TYPE_APPLICATION);

/* Interface */
PnidApp *pnid_app_new(void);
/* Constructors */
static void pnid_app_class_init(PnidAppClass *class);
static void pnid_app_init(PnidApp *self);
/* Parent class signal handlers */
static void pnid_app_startup(GApplication *app);
static void pnid_app_shutdown(GApplication *app);
static void pnid_app_activate(GApplication *app);
static void pnid_app_open(GApplication *app, GFile **files, int n_files, const char *hint);

/* pnid_app_new(): interface for pnid application creation, returns a
   pointer to a new pnid application object. */
PnidApp *
pnid_app_new(void)
{
    return g_object_new(PNID_APP_TYPE,
			"application-id", APP_ID,
			"flags", G_APPLICATION_HANDLES_OPEN,
			NULL);
}

/* pnid_app_class_init(): pnid application class constructor, executed
   only once before the first instance is constructed. */
static void pnid_app_class_init(PnidAppClass *class)
{
    /* override parent class signal handlers */
    G_APPLICATION_CLASS(class)->startup  = pnid_app_startup;
    G_APPLICATION_CLASS(class)->shutdown = pnid_app_shutdown;
    G_APPLICATION_CLASS(class)->activate = pnid_app_activate;
    G_APPLICATION_CLASS(class)->open     = pnid_app_open;
    
    return;
}

/* pnid_app_init(): pnid application object constructor, instantiates
   the class and stores the resulting object at the address pointed to
   by self. */
static void pnid_app_init(PnidApp *self)
{
    return;
}

/* pnid_app_startup(): #GApplication::startup handler, received
   immediately after registration of the primary instance. */
static void pnid_app_startup(GApplication *app)
{
    /* chaining up now so that parents startup actions are performed
       first */
    G_APPLICATION_CLASS(pnid_app_parent_class)->startup(app);
}

/* pnid_app_shutdown(): #GApplication::shutdown handler, received by
   the primary instance after the gtk event loop is terminated. If
   there are any unsaved open files, a confirmation dialog should be
   presented to the user in response to this signal. */
static void pnid_app_shutdown(GApplication *app)
{
    // add save dialogue

    /* chain-up AFTER required actions are completed*/
    G_APPLICATION_CLASS(pnid_app_parent_class)->shutdown(app); 
}

/* pnid_app_activate(): #GApplication::activate handler, a UI with no
   files loaded should be opened in response to this signal. */
static void 
pnid_app_activate(GApplication *app)
{
    gtk_window_present(GTK_WINDOW(pnid_app_window_new(PNID_APP(app))));
}

/* pnid_app_open(): GApplication::open handler, the files provided
   should be displayed in response to this signal. */
static void
pnid_app_open(GApplication *app, GFile **files, int n_files, const char *hint)
{
    // this should run pnid_app_window_open() in a loop for each file.
}
