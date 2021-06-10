/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_appwin.c - pnid application window class definition */

#include <gtk/gtk.h>

#include "pnid_app.h"
#include "pnid_canvas.h"
#include "pnid_appwin.h"

/* #PnidAppWin - pnid application window GObject class definition */
struct _PnidAppWindow {
    GtkApplicationWindow parent;

    /* instance members */
    GtkWidget     *notebook;
    GtkWidget     *headerbar;
    GtkWidget     *menu_button; 
};
G_DEFINE_TYPE(PnidAppWindow, pnid_app_window, GTK_TYPE_APPLICATION_WINDOW);

/* Interface */
PnidAppWindow *pnid_app_window_new(PnidApp *app);
void pnid_app_window_open(PnidAppWindow *win, GFile *file);
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
pnid_app_window_open(PnidAppWindow *self, GFile *file)
{
    GtkWidget *canvas;		/* pnid drawing area */
    GtkWidget *scrolled_window;
    
    canvas = GTK_WIDGET(pnid_canvas_new(800, 600));
    scrolled_window = gtk_scrolled_window_new();
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), canvas);
    gtk_notebook_append_page(GTK_NOTEBOOK(self->notebook),
			     scrolled_window,
			     gtk_label_new(g_file_get_basename(file)));

    gtk_widget_queue_draw(self->notebook);

    return;
}

/* pnid_app_window_class_init(): pnid application window class
   constructor, executed only once before the first instance is
   constructed. */
static void
pnid_app_window_class_init(PnidAppWindowClass *class)
{
}

/* pnid_app_window_init(): pnid application window object constructor,
   instantiates the class and stores the resulting object at the
   address pointed to by self. */
static void
pnid_app_window_init(PnidAppWindow *self)
{
    GtkBuilder *builder;
    GMenuModel *menu;

    /* menu button */
    self->menu_button = gtk_menu_button_new();
    builder = gtk_builder_new_from_resource("/cymru/ert/pnid/data/ui/menu.ui");
    menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->menu_button), menu);
    g_object_unref(builder);

    /* header bar */
    self->headerbar = gtk_header_bar_new();
    gtk_header_bar_pack_end(GTK_HEADER_BAR(self->headerbar), self->menu_button);
    gtk_window_set_titlebar(GTK_WINDOW(self), GTK_WIDGET(self->headerbar));

    /* notebook tabs */
    self->notebook  = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(self->notebook), GTK_POS_TOP);
    gtk_window_set_child(GTK_WINDOW(self), self->notebook);

    return; 
}