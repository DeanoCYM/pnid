/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_canvas.c - pnid drawing canvas class definition  */

#include <gtk/gtk.h>

#include "pnid_draw.h"
#include "pnid_canvas.h"

struct _PnidCanvas {
    GtkDrawingArea parent;

    /* instance members */
    

};
G_DEFINE_TYPE(PnidCanvas, pnid_canvas, GTK_TYPE_DRAWING_AREA);

/* Interface */
PnidCanvas *pnid_canvas_new(int width, int height);
/* Constructors */
static void pnid_canvas_class_init(PnidCanvasClass *class);
static void pnid_canvas_init(PnidCanvas *self);
/* Drawing */
static void redraw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);

/* pnid_canvas_new(): interface for creating a new empty pnid canvas */
PnidCanvas *
pnid_canvas_new(int width, int height)
{
    return g_object_new(PNID_CANVAS_TYPE,
			"content-width", width,
			"content-height", height,
			NULL);
}

/* pnid_canvas_class_init(): pnid canvas class constructor, executed
   only once before the first instance is constructed. */
static void
pnid_canvas_class_init(PnidCanvasClass *class)
{
    return; 
}

/* pnid_canvas_init(): pnid canvas object constructor, instantiates
   the class and stores the resulting object at the address pointed to
   by self. */
static void
pnid_canvas_init(PnidCanvas *self)
{
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(self), redraw, NULL, NULL);
}

/* redraw(): redraw the rectangle between width and height */
static void
redraw(GtkDrawingArea *area,
       cairo_t        *cr,
       int             width, int height,
       gpointer        data)
{
    GdkRGBA color;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context(GTK_WIDGET(area));

    pnid_draw_circle(cr, width, height);

    gtk_style_context_get_color(context, &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_fill(cr);
}
