/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_box.c - pnid bounding box, a rectangle */

#include <assert.h>

#include "pnid_box.h"

static unsigned umin(unsigned x, unsigned y);
static unsigned umax(unsigned x, unsigned y);

/* pnid_box_get_left(): return left side of rectangle a */
unsigned
pnid_box_get_left(const struct pnid_box *a)
{
    return a->nw.x;
}

/* pnid_box_get_left(): set left side of rectangle a */
void
pnid_box_set_left(struct pnid_box *a, unsigned left)
{
    a->nw.x = left;
}

/* pnid_box_get_right(): return right edge of rectangle a */
unsigned
pnid_box_get_right(const struct pnid_box *a)
{
    return a->se.x;
}

/* pnid_box_set_right(): return right edge of rectangle a */
void
pnid_box_set_right(struct pnid_box *a, unsigned right)
{
    a->se.x = right;
}

/* pnid_box_get_top(): top edge of rectangle a */
unsigned
pnid_box_get_top(const struct pnid_box *a)
{
    return a->nw.y;
}

/* pnid_box_set_top(): top edge of rectangle a */
void
pnid_box_set_top(struct pnid_box *a, unsigned top)
{
    a->nw.y = top;
}

/* pnid_box_get_bottom(): return bottom edge of rectangle a */
unsigned
pnid_box_get_bottom(const struct pnid_box *a)
{
    return a->se.y;
}

/* pnid_box_set_bottom(): set bottom edge of rectangle a */
void
pnid_box_set_bottom(struct pnid_box *a, unsigned bottom)
{
    a->se.y = bottom;
}

/* pnid_box_height(): return vertical extent of rectangle a */
unsigned
pnid_box_height(const struct pnid_box *a)
{
    return pnid_box_get_bottom(a) - pnid_box_get_top(a); 
}

/* pnid_box_copy(): returns a copy of the rectangle */
struct pnid_box
pnid_box_copy(const struct pnid_box *a)
{
  return *a;
}

/* pnid_box_width(): horizontal extent of rectangle b */
unsigned
pnid_box_width(const struct pnid_box *a)
{
    return pnid_box_get_right(a) - pnid_box_get_left(a);
}

/* pnid_box_perimeter(): length of the perimiter */
unsigned
pnid_box_perimeter(const struct pnid_box *a)
{
    return 2 * (pnid_box_width(a) + pnid_box_height(a));
}

/* pnid_box_area(): area of rectangle a */
unsigned
pnid_box_area(const struct pnid_box *a)
{
    return pnid_box_width(a) * pnid_box_height(a);
}

/* pnid_box_is_separate(): true only when rectangles share no common
   areas. */
int
pnid_box_is_separate(const struct pnid_box *a, const struct pnid_box *b)
{
    return
	pnid_box_get_left(a)   >= pnid_box_get_right(b)  &&
	pnid_box_get_right(a)  <= pnid_box_get_left(b)   &&
	pnid_box_get_top(a)    >= pnid_box_get_bottom(b) &&
	pnid_box_get_bottom(a) <= pnid_box_get_top(b);
}

/* pnid_box_mbr(): minimum bounding rectangle of a and b. */
struct pnid_box
pnid_box_mbr(const struct pnid_box *a, const struct pnid_box *b)
{
    struct pnid_box ab;

    ab.nw.x = umin(a->nw.x, b->nw.x); /* left */
    ab.nw.y = umin(a->nw.y, b->nw.y); /* top */
    ab.se.x = umax(a->se.x, b->se.x); /* right */
    ab.se.y = umax(a->se.y, b->se.y); /* bottom */

    return ab;
}

/* pnid_box_enlargement(): area increase in mbr when it is grown to
   include a. */
unsigned
pnid_box_mbr_enlargement(const PnidBox *mbr, const PnidBox *a)
{
    const struct pnid_box ab = pnid_box_mbr(mbr, a);

    return pnid_box_area(&ab) - pnid_box_area(a);
}

/* pnid_box_mbr_waste(): returns the wastefulness of a minimum
   bounding rectangle containing rectangles a and b.

   The wastefulness is calculated as excess in mbr area of above that
   of the individual areas of a and b. Concequently the wastefulness
   can be negative when the rectangles overlap. */
int
pnid_box_mbr_waste(const struct pnid_box *a, const struct pnid_box *b)
{
  const struct pnid_box mbr = pnid_box_mbr(a, b);

  return pnid_box_area(&mbr) - pnid_box_area(a) - pnid_box_area(b);
}

/* pnid_box_mbr_grow(): update mbr so that it can contain a. */
void
pnid_box_mbr_grow(PnidBox *mbr, const PnidBox *a)
{
  *mbr = pnid_box_mbr(mbr, a);
}

/* pnid_box_overlap_area(): area of overlap between rectangles a and b */
unsigned
pnid_box_overlap_area(const struct pnid_box *a, const struct pnid_box *b)
{
    struct pnid_box ab;

    if (pnid_box_is_separate(a, b))
	return 0;

    ab.nw.x = umax(a->nw.x, b->nw.x); /* left */
    ab.se.x = umin(a->se.x, b->se.x); /* right */
    ab.nw.y = umax(a->nw.y, b->nw.y); /* top */
    ab.se.y = umin(a->se.y, b->se.y); /* bottom */

    return pnid_box_area(&ab);
}

/* umin(): returns the smallest unsigned integer */
static unsigned
umin(unsigned x, unsigned y)
{
    return x < y ? x : y;
}

/* umin(): returns the largest unsigned integer */
static unsigned
umax(unsigned x, unsigned y)
{
    return x > y ? x : y; 
}
    

