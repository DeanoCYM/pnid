/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_bbox.h - pnid boundary boxes */

#ifndef __PNID_BBOX_H
#define __PNID_BBOX_H

#include <stdarg.h>

struct pnid_coord {
    unsigned x;
    unsigned y;
};

struct pnid_bbox {
    struct pnid_coord nw;
    struct pnid_coord se;
};

typedef struct pnid_coord PnidCoord;
typedef struct pnid_bbox  PnidBBox;

/* Get basic properties of the rectangle a */
unsigned pnid_bbox_get_left   	  (PnidBBox *a);
unsigned pnid_bbox_get_right  	  (PnidBBox *a);
unsigned pnid_bbox_get_top    	  (PnidBBox *a);
unsigned pnid_bbox_get_bottom 	  (PnidBBox *a);
unsigned pnid_bbox_get_height 	  (PnidBBox *a);
unsigned pnid_bbox_get_width  	  (PnidBBox *a);
unsigned pnid_bbox_get_perimeter  (PnidBBox *a);

/* Returns the minimum bounding rectangle from a list of count
   rectangles */
PnidBBox pnid_bbox_get_mbr (int n, PnidBBox *a, ...);

/* Determine if the area of two rectangles overlap or not */
int pnid_bbox_is_separate (PnidBBox *a, PnidBBox *b);
int pnid_bbox_is_overlap  (PnidBBox *a, PnidBBox *b);

/* Set basic properties of a rectangle a */
void pnid_bbox_set_left   (PnidBBox *a, unsigned left);
void pnid_bbox_set_right  (PnidBBox *a, unsigned right);
void pnid_bbox_set_top    (PnidBBox *a, unsigned top);
void pnid_bbox_set_bottom (PnidBBox *a, unsigned bottom);

#endif /* PNID_H */
