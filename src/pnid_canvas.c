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
  gdouble          page_height;
  gdouble          page_width;
  gdouble          top_margin;
  gdouble          bottom_margin;
  gdouble          left_margin;
  gdouble          right_margin;
  uint             zoom_level;
};

G_DEFINE_TYPE(PnidCanvas, pnid_canvas, GTK_TYPE_DRAWING_AREA);

typedef enum  {
  PROP_PAGE_HEIGHT = 1,
  PROP_PAGE_WIDTH,
  PROP_TOP_MARGIN,
  PROP_BOTTOM_MARGIN,
  PROP_LEFT_MARGIN,
  PROP_RIGHT_MARGIN,
  PROP_ZOOM_LEVEL,
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
pnid_canvas_new(GtkPaperSize *paper_size, uint zoom_level) 
{
  return g_object_new(PNID_CANVAS_TYPE,
	     "page_width",    gtk_paper_size_get_width(paper_size, GTK_UNIT_POINTS),
	     "page_height",   gtk_paper_size_get_height(paper_size, GTK_UNIT_POINTS),
	     "top_margin",    gtk_paper_size_get_default_top_margin(paper_size, GTK_UNIT_POINTS),
	     "bottom_margin", gtk_paper_size_get_default_bottom_margin(paper_size, GTK_UNIT_POINTS),
	     "left_margin",   gtk_paper_size_get_default_left_margin(paper_size, GTK_UNIT_POINTS),
	     "right_margin",  gtk_paper_size_get_default_right_margin(paper_size, GTK_UNIT_POINTS),
	     "zoom-level",    zoom_level,
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
  case PROP_PAGE_WIDTH:
    PNID_CANVAS(self)->page_width = g_value_get_double(value);
    break;
  case PROP_PAGE_HEIGHT:
    PNID_CANVAS(self)->page_height = g_value_get_double(value);
    break;
  case PROP_TOP_MARGIN:
    PNID_CANVAS(self)->top_margin = g_value_get_double(value);
    break;
  case PROP_BOTTOM_MARGIN:
    PNID_CANVAS(self)->bottom_margin = g_value_get_double(value);
    break;
  case PROP_LEFT_MARGIN:
    PNID_CANVAS(self)->left_margin = g_value_get_double(value);
    break;
  case PROP_RIGHT_MARGIN:
    PNID_CANVAS(self)->right_margin = g_value_get_double(value);
    break;
  case PROP_ZOOM_LEVEL:
    PNID_CANVAS(self)->zoom_level = g_value_get_uint(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
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
  case PROP_PAGE_WIDTH:
    g_value_set_double(value, PNID_CANVAS(self)->page_width);
    break;
  case PROP_PAGE_HEIGHT:
    g_value_set_double(value, PNID_CANVAS(self)->page_height);
    break;
  case PROP_TOP_MARGIN:
    g_value_set_double(value, PNID_CANVAS(self)->top_margin);
    break;
  case PROP_BOTTOM_MARGIN:
    g_value_set_double(value, PNID_CANVAS(self)->bottom_margin);
    break;
  case PROP_LEFT_MARGIN:
    g_value_set_double(value, PNID_CANVAS(self)->left_margin);
    break;
  case PROP_RIGHT_MARGIN:
    g_value_set_double(value, PNID_CANVAS(self)->right_margin);
    break;
  case PROP_ZOOM_LEVEL:
    g_value_set_uint(value, PNID_CANVAS(self)->zoom_level);
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
    
  obj_properties[PROP_PAGE_WIDTH] =
    g_param_spec_double("page-width", "Page width",
			"Width of the page in points",
			1.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_PAGE_HEIGHT] =
    g_param_spec_double("page-height", "Page height",
			"Height of the page in points",
			1.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_TOP_MARGIN] =
    g_param_spec_double("top-margin", "Top margin",
			"The top printable margin in points",
			0.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_BOTTOM_MARGIN] =
    g_param_spec_double("bottom-margin", "Bottom margin",
			"The bottom printable margin in points",
			0.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_LEFT_MARGIN] =
    g_param_spec_double("left-margin", "Left margin",
			"The left printable margin in points",
			0.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_RIGHT_MARGIN] =
    g_param_spec_double("right-margin", "Right margin",
			"The right printable margin in points",
			0.0, 5000.0, 460.0, /* min, max, default */
			G_PARAM_READWRITE);
  obj_properties[PROP_ZOOM_LEVEL] =
    g_param_spec_uint("zoom-level", "Zoom level",
		      "Zoom level to view the canvas",
		      1, 5, 2,
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
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(self), redraw, NULL, NULL);
}

/* redraw(): redraw the rectangle between width and height */
static void
redraw(GtkDrawingArea *area,
       cairo_t        *cr,
       int             width, int height,
       gpointer        data)
{
  PnidCanvas *self = PNID_CANVAS(area); 

  #ifndef G_DISABLE_ASSERT
  fputs("==== #PnidCanvas::redraw() callback ====\n", stderr);  
  fprintf(stderr, "#PnidCanvas::redraw(width=%d, height=%d)\n", width, height);
  fprintf(stderr, "#PnidCanvas:page_height=%gpt\n", self->page_height);  
  fprintf(stderr, "#PnidCanvas:page_width=%gpt\n", self->page_width);  
  fprintf(stderr, "#PnidCanvas:top_margin=%gpt\n", self->top_margin);  
  fprintf(stderr, "#PnidCanvas:bottom_margin=%gpt\n", self->bottom_margin);
  fprintf(stderr, "#PnidCanvas:left_margin=%gpt\n", self->left_margin);
  fprintf(stderr, "#PnidCanvas:right_margin=%gpt\n", self->right_margin);  
  fprintf(stderr, "#PnidCanvas:zoom_level=%u\n", self->zoom_level);  
  fputc('\n', stderr);
  #endif

  cairo_scale(cr, self->zoom_level, self->zoom_level); 

  /* blank page with margins */
  cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
  cairo_rectangle(cr, 0, 0, self->page_width, self->page_height);
  cairo_fill(cr);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr,
		  self->left_margin,
		  self->top_margin,
		  self->page_width  - self->right_margin  - self->left_margin,
		  self->page_height - self->bottom_margin - self->top_margin);
  cairo_fill(cr);
}
