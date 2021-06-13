/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_canvas.h - pnid drawing canvas class definition */

#ifndef __PNID_CANVAS_H
#define __PNID_CANVAS_H

#include <gtk/gtk.h>

/*
  #PnidCanvas GObject class declaration
*/

#define PNID_CANVAS_TYPE (pnid_canvas_get_type())
G_DECLARE_FINAL_TYPE(PnidCanvas, pnid_canvas, PNID, CANVAS, GtkDrawingArea);

/*
  #PnidCanvas interface
*/
PnidCanvas *pnid_canvas_new(uint zoom_level);

#endif /* __PNID_CANVAS_H */
