/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_box.h - pnid boundary boxes */

#ifndef __PNID_BOX_H
#define __PNID_BOX_H

#include <stdarg.h>

struct pnid_coord {
    unsigned x;
    unsigned y;
};

struct pnid_box {
    struct pnid_coord nw;
    struct pnid_coord se;
};

typedef struct pnid_coord PnidCoord;
typedef struct pnid_box   PnidBox;

PnidBox pnid_box_copy(const struct pnid_box *a);

/* Get properties of the rectangle a */
unsigned pnid_box_get_left   (const PnidBox *a);
unsigned pnid_box_get_right  (const PnidBox *a);
unsigned pnid_box_get_top    (const PnidBox *a);
unsigned pnid_box_get_bottom (const PnidBox *a);

/* Calculations on a rectangle */
unsigned pnid_box_height    (const PnidBox *a);
unsigned pnid_box_width     (const PnidBox *a);
unsigned pnid_box_perimeter (const PnidBox *a);
unsigned pnid_box_area      (const PnidBox *a);  

/* Calculations on two rectangles */
int      pnid_box_is_subset    (const PnidBox *a, const PnidBox *b);
int      pnid_box_is_separate  (const PnidBox *a, const PnidBox *b);
unsigned pnid_box_overlap_area (const PnidBox *a, const PnidBox *b);
PnidBox  pnid_box_mbr          (const PnidBox *a, const PnidBox *b);

/* Set basic properties of a rectangle a */
void pnid_box_set_left   (PnidBox *a, unsigned left);
void pnid_box_set_right  (PnidBox *a, unsigned right);
void pnid_box_set_top    (PnidBox *a, unsigned top);
void pnid_box_set_bottom (PnidBox *a, unsigned bottom);

#endif /* PNID_H */
