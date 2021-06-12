/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_appwin.h - pnid application window class and interface
   declarations */

#ifndef __PNID_APPWIN_H
#define __PNID_APPWIN_H

#include <gtk/gtk.h>

#include "pnid_app.h"

/*
  #PnidAppWindow GObject class declaration
*/
#define PNID_APP_WINDOW_TYPE (pnid_app_window_get_type())
G_DECLARE_FINAL_TYPE(PnidAppWindow, pnid_app_window, PNID, APP_WINDOW, GtkApplicationWindow)

/*
  #PnidAppWindow interface 
*/
PnidAppWindow *pnid_app_window_new         (PnidApp *app);
void           pnid_app_window_empty       (PnidAppWindow *self);
void           pnid_app_window_open        (PnidAppWindow *win, GFile *file);
void           pnid_app_window_page_setup  (PnidAppWindow *self);

#endif /* __PNID_APPWIN_H */
