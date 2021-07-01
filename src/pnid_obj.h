/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.h - pnid object */

#ifndef __PNID_OBJ_H
#define __PNID_OBJ_H

#include "pnid_box.h"

typedef struct pnid_obj PnidObj; 

PnidObj  *pnid_obj_new(void);
void      pnid_obj_delete(PnidObj *self);

int       pnid_obj_get_data(PnidObj *self);
PnidBox  *pnid_obj_get_box(PnidObj *self);

void      pnid_obj_set_data(PnidObj *self, int data);


#endif /* PNID_OBJ_H */
