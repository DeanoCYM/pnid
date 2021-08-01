/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.h - pnid object */

#ifndef __PNID_OBJ_H
#define __PNID_OBJ_H

#include "pnid_box.h"

typedef struct pnid_obj PnidObj; 

struct pnid_obj {
  PnidBox bbox;
};

/* Create and destroy pnid objects */
PnidObj    *pnid_obj_new(void);
void        pnid_obj_delete(PnidObj *self);

/* Calculate and return the bounding box of a pnid object*/
PnidBox     pnid_obj_bbox(PnidObj *self);

#endif /* PNID_OBJ_H */
