/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.h - pnid object */

#ifndef __PNID_OBJ_H
#define __PNID_OBJ_H

#include "pnid_box.h"

typedef struct pnid_obj PnidObj; 

PnidObj  *pnid_obj_new(const char *key);
void      pnid_obj_delete(PnidObj *self);

int       pnid_obj_get_data(PnidObj *self);
void      pnid_obj_set_data(PnidObj *self, int data);

PnidBox   pnid_obj_bbox(PnidObj *self);

/* Three way comparisons return an integer that is zero, negative or
   positve when a == b, a < b, or a > b respectively. */
int       pnid_obj_cmp_id(const PnidObj *a, const PnidObj *b); 


#endif /* PNID_OBJ_H */
