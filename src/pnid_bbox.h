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

/* Get properties of the rectangle a */
unsigned pnid_bbox_get_left   	  (const PnidBBox *a);
unsigned pnid_bbox_get_right  	  (const PnidBBox *a);
unsigned pnid_bbox_get_top    	  (const PnidBBox *a);
unsigned pnid_bbox_get_bottom 	  (const PnidBBox *a);
unsigned pnid_bbox_get_height 	  (const PnidBBox *a);
unsigned pnid_bbox_get_width  	  (const PnidBBox *a);
unsigned pnid_bbox_get_perimeter  (const PnidBBox *a);
unsigned pnid_bbox_get_area       (const PnidBBox *a);  

/* Stores the minimum bounding rectangle large enough to contain
   rectangles a and b in res and returns the increase in area. */
unsigned pnid_bbox_mbr (const PnidBBox *a, const PnidBBox *b, PnidBBox *mbr);

/* Determine if the area of two rectangles overlap or not */
int pnid_bbox_is_separate (const PnidBBox *a, const PnidBBox *b);
int pnid_bbox_is_overlap  (const PnidBBox *a, const PnidBBox *b);

/* Set basic properties of a rectangle a */
void pnid_bbox_set_left   (PnidBBox *a, unsigned left);
void pnid_bbox_set_right  (PnidBBox *a, unsigned right);
void pnid_bbox_set_top    (PnidBBox *a, unsigned top);
void pnid_bbox_set_bottom (PnidBBox *a, unsigned bottom);

#endif /* PNID_H */
