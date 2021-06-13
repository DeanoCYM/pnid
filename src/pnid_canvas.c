/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_canvas.c - pnid drawing canvas class definition  */

#include <gtk/gtk.h>
#include <math.h>

#include "pnid_draw.h"
#include "pnid_canvas.h"

/* #PnidCanvas class definition */
struct _PnidCanvas {
    GtkDrawingArea   parent;

    /* instance members */
    GtkPaperSize    *paper_size;
    cairo_surface_t *surface;

    /* properties */
    uint             zoom_level;
};

G_DEFINE_TYPE(PnidCanvas, pnid_canvas, GTK_TYPE_DRAWING_AREA);

typedef enum
{
  PROP_ZOOM_LEVEL = 1,
  N_PROPERTIES
} PnidCanvasProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

/* Interface */
PnidCanvas *pnid_canvas_new(uint zoom_level);
/* Constructors */
static void pnid_canvas_class_init(PnidCanvasClass *class);
static void pnid_canvas_init(PnidCanvas *self);
/* Property getter/setter methods */
static void pnid_canvas_get_property(GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void pnid_canvas_set_property(GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
/* Drawing */
static void redraw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);

/* pnid_canvas_new(): interface for creating a new empty pnid canvas */
PnidCanvas *
pnid_canvas_new(uint zoom_level)
{
    return g_object_new(PNID_CANVAS_TYPE,
			"zoom-level", zoom_level,
			NULL);
}

/* pnid_canvas_set_property(): property setter */
static void
pnid_canvas_set_property(GObject      *self,
			 guint         property_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
    switch ((PnidCanvasProperty)property_id) {
    case PROP_ZOOM_LEVEL:
	PNID_CANVAS(self)->zoom_level = g_value_get_uint(value);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
	break;
    }
}

/* pnid_canvas_get_property(): property getter */
static void
pnid_canvas_get_property(GObject      *self,
			 guint         property_id,
			 GValue       *value,
			 GParamSpec   *pspec)
{
    switch ((PnidCanvasProperty)property_id) {
    case PROP_ZOOM_LEVEL:
	g_value_set_uint(value, PNID_CANVAS(self)->zoom_level);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
	break;
    }
}

/* pnid_canvas_class_init(): pnid canvas class constructor, executed
   only once before the first instance is constructed. */
static void
pnid_canvas_class_init(PnidCanvasClass *class)
{
    G_OBJECT_CLASS(class)->set_property = pnid_canvas_set_property;
    G_OBJECT_CLASS(class)->get_property = pnid_canvas_get_property;
    
    obj_properties[PROP_ZOOM_LEVEL] =
	g_param_spec_uint("zoom-level", "Zoom level",
			  "Zoom level to view the canvas",
			  1, 5, 2, /* min, max, default */
			  G_PARAM_READWRITE);
    
    g_object_class_install_properties(G_OBJECT_CLASS(class),
				      N_PROPERTIES,
				      obj_properties);
    
}

/* pnid_canvas_init(): pnid canvas object constructor, instantiates
   the class and stores the resulting object at the address pointed to
   by self. */
static void
pnid_canvas_init(PnidCanvas *self)
{
    self->paper_size = gtk_page_setup_get_paper_size(gtk_page_setup_new());     // tempory until I pass the papersize

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(self), redraw, NULL, NULL);
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(self),
				       1 * ceil(gtk_paper_size_get_width(self->paper_size, GTK_UNIT_MM)));
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(self),
					1 * ceil(gtk_paper_size_get_height(self->paper_size, GTK_UNIT_MM)));
}

/* redraw(): redraw the rectangle between width and height */
static void
redraw(GtkDrawingArea *area,
       cairo_t        *cr,
       int             width, int height,
       gpointer        data)
{
    GdkRGBA          color;
    GtkStyleContext *context;
    uint             zoom_level;

    
    context = gtk_widget_get_style_context(GTK_WIDGET(area));
    g_object_get(G_OBJECT(area), "zoom-level", &zoom_level, NULL);

    pnid_draw_circle(cr, width*zoom_level, height*zoom_level);

    gtk_style_context_get_color(context, &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_fill(cr);
}
