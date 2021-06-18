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
    GtkPageSetup     *page_setup;
    GtkPrintSettings *print_settings;
    GtkWidget        *notebook;
    GtkWidget        *headerbar;
    GtkWidget        *menu_button;
    GtkWidget        *canvas; 
};
G_DEFINE_TYPE(PnidAppWindow, pnid_app_window, GTK_TYPE_APPLICATION_WINDOW);

/* Interface */
PnidAppWindow *pnid_app_window_new(PnidApp *app);
void pnid_app_window_empty(PnidAppWindow *self);
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

/* pnid_app_window_empty(): interface for creating a new window with a
   blank canvas. // NOT IMPLEMENTED YET */
void
pnid_app_window_empty(PnidAppWindow *self)
{
    g_assert(NULL && "NOT IMPLEMENTED");
}

/* pnid_app_window_open(): Open pnid drawing file in a new tab.

   The notebook has already been initialised, so here a new tab is
   added. The pnid drawing file is loaded as an instance of
   #PnidCanvas. The canvas is itself placed within a scrolled
   window. 

   -> #GtkNotebook (already initialised)
   --> #GtkScrolledWindow // NOT IMPLEMENTED YET
   ---> #PnidCanvas

   The size of the drawing is controlled by the #PnidCanvas
   properties. */
void
pnid_app_window_open(PnidAppWindow *self, GFile *file)
{
    g_assert(file);
    g_assert(self->page_setup);
    g_assert(self->notebook);

    self->canvas = GTK_WIDGET(pnid_canvas_new(gtk_page_setup_get_paper_size(self->page_setup), 1));
    gtk_notebook_append_page(GTK_NOTEBOOK(self->notebook), self->canvas, gtk_label_new(g_file_get_basename(file)));

    gtk_widget_queue_draw(self->notebook);
}

/* pnid_app_window_page_setup(): open the page setup dialogue and
   update the PnidCanvas properties. */
void
pnid_app_window_page_setup(PnidAppWindow *self)
{
    GtkPageSetup *new;

    g_assert(self->print_settings);
    g_assert(self->page_setup);

    new = gtk_print_run_page_setup_dialog(GTK_WINDOW(self),
					  self->page_setup,
					  self->print_settings);
    g_object_unref(self->page_setup);
    self->page_setup = new;

    if (self->canvas) {
	g_object_set(self->canvas,
		     "page_width",    gtk_page_setup_get_page_width(new, GTK_UNIT_POINTS),
		     "page_height",   gtk_page_setup_get_page_height(new, GTK_UNIT_POINTS),
		     "top_margin",    gtk_page_setup_get_top_margin(new, GTK_UNIT_POINTS),
		     "bottom_margin", gtk_page_setup_get_bottom_margin(new, GTK_UNIT_POINTS),
		     "left_margin",   gtk_page_setup_get_left_margin(new, GTK_UNIT_POINTS),
		     "right_margin",  gtk_page_setup_get_right_margin(new, GTK_UNIT_POINTS),
		     NULL);
	gtk_widget_queue_draw(GTK_WIDGET(self->canvas));
    }
}

/* pnid_app_window_class_init(): pnid application window class
   constructor, executed only once before the first instance is
   constructed. */
static void
pnid_app_window_class_init(PnidAppWindowClass *class)
{
}

/* pnid_app_window_init(): Creates a new window, provides a state
   where a pnid drawing file can be opened using
   pnid_app_window_open().

   In the new window, a header bar contaning a menu button and window
   controls is initialised. Beneath this, an empty notebook is
   initialised to which tabs later be added to. 

   -> #PnidAppWindow
   --> #GtkHeaderBar
   ---> #GtkMenuButton
   --> #GtkNotebook
*/
static void
pnid_app_window_init(PnidAppWindow *self)
{
    GtkBuilder *builder;
    GMenuModel *menu;

    self->print_settings = gtk_print_settings_new();
    self->page_setup = gtk_page_setup_new();

    self->menu_button = gtk_menu_button_new();
    builder = gtk_builder_new_from_resource("/cymru/ert/pnid/data/ui/menu.ui");
    menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->menu_button), menu);
    g_object_unref(builder);

    self->headerbar = gtk_header_bar_new();
    gtk_header_bar_pack_end(GTK_HEADER_BAR(self->headerbar), self->menu_button);
    gtk_window_set_titlebar(GTK_WINDOW(self), GTK_WIDGET(self->headerbar));

    self->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(self->notebook), GTK_POS_TOP);
    gtk_window_set_child(GTK_WINDOW(self), self->notebook);
    
    return; 
}
