/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.c - a pnid object to be drawn on the canvas */

#include <stdlib.h>

#include "pnid_box.h"
#include "pnid_obj.h"

/* pnid_obj_new(): create a new pnid object */
PnidObj *
pnid_obj_new(void)
{
  PnidObj *new;

  if (!(new = malloc(sizeof *new)))
    return NULL;

  return new;
}

/* pnid_obj_delete(): delete and free memory */
void
pnid_obj_delete(PnidObj *self)
{  
  if (self)
    free(self);
}

PnidBox
pnid_obj_bbox(PnidObj *self)
{
  return self->bbox;
}
