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
    cairo_surface_t *surface;

    /* properties */
    uint             zoom_level;
    uint             dpi;
    gfloat           height;
    gfloat           width;
};

G_DEFINE_TYPE(PnidCanvas, pnid_canvas, GTK_TYPE_DRAWING_AREA);

typedef enum
{
  PROP_ZOOM_LEVEL = 1,
  PROP_DPI,
  PROP_HEIGHT,
  PROP_WIDTH,
  N_PROPERTIES
} PnidCanvasProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

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
pnid_canvas_new(uint zoom_level, uint dpi, gfloat width, gfloat height)
{
    return g_object_new(PNID_CANVAS_TYPE,
			"zoom-level", zoom_level,
			"dpi"       , dpi,
			"width"     , width,
			"height"    , height,
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
    case PROP_DPI:
	PNID_CANVAS(self)->dpi = g_value_get_uint(value);
	break;
    case PROP_HEIGHT:
	PNID_CANVAS(self)->height = g_value_get_float(value);
	break;
    case PROP_WIDTH:
	PNID_CANVAS(self)->width = g_value_get_float(value);
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
    case PROP_DPI:
	g_value_set_uint(value, PNID_CANVAS(self)->dpi);
	break;
    case PROP_HEIGHT:
	g_value_set_float(value, PNID_CANVAS(self)->height);
	break;
    case PROP_WIDTH:
	g_value_set_float(value, PNID_CANVAS(self)->width);
	break;
    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
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
			  1, 5, 2,              /* min, max, default */
			  G_PARAM_READWRITE);
    obj_properties[PROP_DPI] =
	g_param_spec_uint("dpi", "Dots per inch",
			  "DPI of the canvas",
			  72, 720, 72,          /* min, max, default */
			  G_PARAM_READWRITE);
    obj_properties[PROP_HEIGHT] =
	g_param_spec_float("height", "Canvas height",
			  "Height of the canvas in inches",
			  1.0f, 100.0f, 10.0f,  /* min, max, default */
			  G_PARAM_READWRITE);
    obj_properties[PROP_WIDTH] =
	g_param_spec_float("width", "Canvas width",
			  "Width of the canvas in inches",
			  1.0f, 100.0f, 10.0f, /* min, max, default */
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
    uint x, y;		/* resolution of canvas */

    x = ceil(self->width  * self->dpi); 
    y = ceil(self->height * self->dpi);
    
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(self), redraw, NULL, NULL);
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(self), x);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(self), y);
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
    
    context = gtk_widget_get_style_context(GTK_WIDGET(area));

    pnid_draw_circle(cr, width, height);

    gtk_style_context_get_color(context, &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_fill(cr);
}
