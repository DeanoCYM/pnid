/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_draw.c - pnid drawing with cario */

#include <cairo.h>
#include <glib.h>

void
pnid_draw_circle(cairo_t *cr, int width, int height)
{
    cairo_arc(cr,
	      width / 2.5, height / 2.5,
	      MIN(width, height) / 2.5,
	      0, 2 * G_PI);
}
