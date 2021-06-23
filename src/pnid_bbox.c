/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_bbox.c - pnid bounding box, a rectangle */

#include "pnid_bbox.h"

static unsigned min(unsigned x, unsigned y);
static unsigned max(unsigned x, unsigned y);

/* pnid_bbox_get_left(): return left side of rectangle a */
unsigned
pnid_bbox_get_left(const struct pnid_bbox *a)
{
    return a->nw.x;
}

/* pnid_bbox_get_left(): set left side of rectangle a */
void
pnid_bbox_set_left(struct pnid_bbox *a, unsigned left)
{
    a->nw.x = left;
}

/* pnid_bbox_get_right(): return right edge of rectangle a */
unsigned
pnid_bbox_get_right(const struct pnid_bbox *a)
{
    return a->se.x;
}

/* pnid_bbox_set_right(): return right edge of rectangle a */
void
pnid_bbox_set_right(struct pnid_bbox *a, unsigned right)
{
    a->se.x = right;
}

/* pnid_bbox_get_top(): top edge of rectangle a */
unsigned
pnid_bbox_get_top(const struct pnid_bbox *a)
{
    return a->nw.y;
}

/* pnid_bbox_set_top(): top edge of rectangle a */
void
pnid_bbox_set_top(struct pnid_bbox *a, unsigned top)
{
    a->nw.y = top;
}

/* pnid_bbox_get_bottom(): return bottom edge of rectangle a */
unsigned
pnid_bbox_get_bottom(const struct pnid_bbox *a)
{
    return a->se.y;
}

/* pnid_bbox_set_bottom(): set bottom edge of rectangle a */
void
pnid_bbox_set_bottom(struct pnid_bbox *a, unsigned bottom)
{
    a->se.y = bottom;
}

/* pnid_bbox_get_height(): return vertical extent of rectangle a */
unsigned
pnid_bbox_get_height(const struct pnid_bbox *a)
{
    return pnid_bbox_get_bottom(a) - pnid_bbox_get_top(a); 
}

/* pnid_bbox_get_width(): horizontal extent of rectangle b */
unsigned
pnid_bbox_get_width(const struct pnid_bbox *a)
{
    return pnid_bbox_get_left(a) - pnid_bbox_get_right(a);
}

/* pnid_bbox_get_perimeter(): length of the perimiter */
unsigned
pnid_bbox_get_perimeter(const struct pnid_bbox *a)
{
    return 2 * (pnid_bbox_get_width(a) + pnid_bbox_get_width(a));
}

/* pnid_bbox_is_separate(): truthy only when rectangles share no common
   areas. */
int
pnid_bbox_is_separate(const struct pnid_bbox *a, const struct pnid_bbox *b)
{
     return
	 pnid_bbox_get_left(a)   > pnid_bbox_get_right(b)  &&
	 pnid_bbox_get_right(a)  < pnid_bbox_get_left(b)   &&
	 pnid_bbox_get_top(a)    > pnid_bbox_get_bottom(b) &&
	 pnid_bbox_get_bottom(a) < pnid_bbox_get_top(b);
}

/* pnid_bbox_is_overlap(): truthy only when rectangles share some common
   area */
int
pnid_bbox_is_overlap(const struct pnid_bbox *a, const struct pnid_bbox *b)
{
    return !pnid_bbox_is_separate(a, b);
}

/* pnid_bbox_mbr(): returns minimum bounding region of two
   rectangles */
struct pnid_bbox  
pnid_bbox_get_mbr(const struct pnid_bbox *a, const struct pnid_bbox *b)
{
    struct pnid_bbox mbr;

    mbr.nw.x = min(a->nw.x, b->nw.x); /* left */
    mbr.se.x = max(a->se.x, b->se.x); /* right */
    mbr.nw.y = min(a->nw.y, b->nw.y); /* top */
    mbr.se.y = max(a->se.y, b->se.y); /* bottom */

    return mbr;
}

static unsigned
min(unsigned x, unsigned y)
{
    return x < y ? x : y;
}

static unsigned
max(unsigned x, unsigned y)
{
    return x > y ? x : y; 
}
    

