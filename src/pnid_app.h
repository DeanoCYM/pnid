/* This file is part of pnid
   Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_app.h - pnid application class declarations */

#ifndef __PNID_APP_H
#define __PNID_APP_H

#include <gtk/gtk.h>

#define APP_ID "cymru.ert.pnid"

/* #PnidApp - pnid application class decleration */
#define PNID_APP_TYPE (pnid_app_get_type())
G_DECLARE_FINAL_TYPE(PnidApp, pnid_app, PNID, APP, GtkApplication)

/* pnid_app_new(): returns a new pnid application object */
PnidApp *pnid_app_new(void);

#endif /* __PNID_APP_H */
