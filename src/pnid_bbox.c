/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_bbox.c - pnid bounding box, a rectangle */

#include <stdlib.h>
#include <stdarg.h>

#include "pnid_bbox.h"

/* pnid_bbox_get_left(): return left side of rectangle a */
unsigned
pnid_bbox_get_left(struct pnid_bbox *a)
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
pnid_bbox_get_right(struct pnid_bbox *a)
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
pnid_bbox_get_top(struct pnid_bbox *a)
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
pnid_bbox_get_bottom(struct pnid_bbox *a)
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
pnid_bbox_get_height(struct pnid_bbox *a)
{
    return pnid_bbox_get_bottom(a) - pnid_bbox_get_top(a); 
}

/* pnid_bbox_get_width(): horizontal extent of rectangle b */
unsigned
pnid_bbox_get_width(struct pnid_bbox *a)
{
    return pnid_bbox_get_left(a) - pnid_bbox_get_right(a);
}

/* pnid_bbox_get_perimeter(): length of the perimiter */
unsigned
pnid_bbox_get_perimeter(struct pnid_bbox *a)
{
    return 2 * (pnid_bbox_get_width(a) + pnid_bbox_get_width(a));
}

/* pnid_bbox_is_separate(): truthy only when rectangles share no common
   areas. */
static int
pnid_bbox_is_separate(struct pnid_bbox *a, struct pnid_bbox *b)
{
     return
	 pnid_bbox_get_left(a)   > pnid_bbox_get_right(b)  &&
	 pnid_bbox_get_right(a)  < pnid_bbox_get_left(b)   &&
	 pnid_bbox_get_top(a)    > pnid_bbox_get_bottom(b) &&
	 pnid_bbox_get_bottom(a) < pnid_bbox_get_top(b);
}

/* pnid_bbox_is_overlap(): truthy only when rectangles share some common
   area */
static int
pnid_bbox_is_overlap(struct pnid_bbox *a, struct pnid_bbox *b)
{
    return !pnid_bbox_is_separate(a, b);
}

/* pnid_bbox_mbr(): minimum bounding region of all supplied
   rectangles. */
struct pnid_bbox  
pnid_bbox_get_mbr(int n, struct pnid_bbox *a, ...)
{
    va_list ap;
    struct pnid_bbox mbr, *b;
    unsigned x;

    va_start(ap, a);

    mbr = *a;
    while (--n) {
	b = va_arg(ap, struct pnid_bbox *);  
	if (b == NULL)
	    break;

	if (pnid_bbox_get_left(&mbr) > (x = pnid_bbox_get_left(b)))
	    pnid_bbox_set_left(&mbr, x);
	if (pnid_bbox_get_right(&mbr) < (x = pnid_bbox_get_right(b)))
	    pnid_bbox_set_right(&mbr, x);
	if (pnid_bbox_get_top(&mbr) > (x = pnid_bbox_get_top(b)))
	    pnid_bbox_set_top(&mbr, x);
	if (pnid_bbox_get_bottom(&mbr) < (x = pnid_bbox_get_bottom(b)))
	    pnid_bbox_set_bottom(&mbr, x);
    }

    va_end(ap);

    return mbr;
}
