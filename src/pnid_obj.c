/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.c - a pnid object */

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pnid_box.h"

#include "pnid_obj.h" 

struct pnid_obj {
  const char *key; 		/* unique identifier */
  int         data;
  PnidBox     mbr;		/* minimal bounding region */
};

/* pnid_obj_new(): allocate memory for a new pnid object */
struct pnid_obj * 
pnid_obj_new(const char *key)
{
  PnidObj *new;

  if (!(new = malloc(sizeof *new)))
    exit(1);
  new->key = key;

  return new;
}

/* pnid_obj_free(): free all memory associated with a pnid object */
void
pnid_obj_delete(struct pnid_obj *self)
{
  if (self)
    free(self);
}

/* pnid_obj_get_data(): returns the data property of a pnid object */
int
pnid_obj_get_data(struct pnid_obj *self)
{
  assert(self);
  return self->data;
}

/* pnid_obj_get_box(): bounding box the pnid object. */
PnidBox 
pnid_obj_bbox(PnidObj *self)
{
  return self->mbr;
}

/* pnid_obj_set_data(): set the data property of a pnid object */
void
pnid_obj_set_data(struct pnid_obj *self, int data)
{
  assert(self);

  self->data = data;

  pnid_box_set_left   (&self->mbr, 0.5*data);
  pnid_box_set_right  (&self->mbr, data);
  pnid_box_set_top    (&self->mbr, 2*data);
  pnid_box_set_bottom (&self->mbr, data);
}

/* pnid_obj_cmp_key(): three way comparison of key field  */
int
pnid_obj_cmp_key(const struct pnid_obj *a, const struct pnid_obj *b)
{
  return strcmp(a->key, b->key);
}
