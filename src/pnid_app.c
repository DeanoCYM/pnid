/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_app.c - pnid application class definition */

#include <gtk/gtk.h>

#include "pnid_app.h" 
#include "pnid_appwin.h"

/* app_entries[]: pnid application actions */
static void pagesetup_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static void print_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static void preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static void about_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static void help_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
static GActionEntry app_entries[] =
{
    { "pagesetup",   pagesetup_activated,   NULL, NULL, NULL },
    { "print",       print_activated,       NULL, NULL, NULL },
    { "preferences", preferences_activated, NULL, NULL, NULL },
    { "about",       about_activated,       NULL, NULL, NULL },
    { "help",        help_activated,        NULL, NULL, NULL },
    { "quit",        quit_activated,        NULL, NULL, NULL }
};

/* #PnidApp - pnid application GObject class definition */
struct _PnidApp {
    GtkApplication parent;

    /* instance members */
};
G_DEFINE_TYPE(PnidApp, pnid_app, GTK_TYPE_APPLICATION);
PnidApp *pnid_app_new(void);			      /* interface */
static void pnid_app_class_init(PnidAppClass *class); /* constructors */
static void pnid_app_init(PnidApp *self);
static void pnid_app_startup(GApplication *app);      /* signal handlers */
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
    /* chain-up so parent's startup proceedure is performed first */
    G_APPLICATION_CLASS(pnid_app_parent_class)->startup(app);

    /* register application wide actions */
    g_action_map_add_action_entries(G_ACTION_MAP(app),
				    app_entries,
				    G_N_ELEMENTS(app_entries),
				    app);

    /* register keybinds */
    gtk_application_set_accels_for_action(GTK_APPLICATION(app),
					  "app.quit",
					  (const char *[]){ "<Ctrl>Q", NULL }); 
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
    GList *windows;
    PnidAppWindow *win;

    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows)
	win = PNID_APP_WINDOW(windows->data); /* use 1st existing window */
    else
	win = pnid_app_window_new(PNID_APP(app));

    gtk_window_present(GTK_WINDOW(win));    
}

/* pnid_app_open(): GApplication::open handler, the files provided in
   the list should be displayed in response to this signal. If a
   window is already open, then it should be used. */
static void
pnid_app_open(GApplication *app, GFile **files, int n_files, const char *hint)
{
    GList *windows;
    PnidAppWindow *win;
    int i;

    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows)
	win = PNID_APP_WINDOW(windows->data); /* use 1st existing window */
    else
	win = pnid_app_window_new(PNID_APP(app));
    for (i = 0; i < n_files; i++)
	pnid_app_window_open(win, files[i]);

    gtk_window_present(GTK_WINDOW(win));    
}

/* pagesetup_activated(): app.pagesetup action, open page setup
   dialogue. */
static void
pagesetup_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    printf("pagesetup_activated\n");
}

/* print_activated(): app.print action, open print dialogue */
static void
print_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    printf("print_activated\n");
}

/* preferences_activated(): app.preferences action, open preferences
   dialogue */
static void
preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    printf("preferences_activated\n");
}

/* about_activated(): app.about action, open about dialogue */
static void
about_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    gtk_show_about_dialog( NULL,
			   "program-name", "cymru.ert.pnid",
			   "version",      "v0.0-alpha",
			   "comments",     "Piping and instrumentation drawing canvas",
			   "logo",         gdk_texture_new_from_resource("/cymru/ert/pnid/data/valve.png"),
			   "authors",      (const char *[]){"Ellis Rhys Thomas <e.rhys.thomas@gmail.com>", NULL},
			   "copyright",    "© 2021 Ellis Rhys Thomas",
			   "license-type", GTK_LICENSE_GPL_3_0,
			   "website",      "https://github.com/DeanoCYM/pnid",
			   NULL);
}

/* help_activated(): app.help action, open help dialogue */
static void
help_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    printf("help_activated\n");
}

/* quit_activated(): app.quit action, exit application */
static void
quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    g_application_quit(G_APPLICATION(app));
}



