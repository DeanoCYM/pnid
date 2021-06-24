/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_obj.c - a pnid object */

#include <stdlib.h>
#include <assert.h>

#include "pnid_bbox.h"

#include "pnid_obj.h" 

struct pnid_obj {
    int       data;
    PnidBBox *mbr;		/* minimal bounding region */
};

/* pnid_obj_new(): allocate memory for a new pnid object */
struct pnid_obj * 
pnid_obj_new(void)
{
    return malloc(sizeof (struct pnid_obj));
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

/* pnid_obj_get_bbox(): returns the minimal bounding region of the
   pnid object. */
struct pnid_obj *
pnid_obj_get_bbox(PnidObj *self)
{
    return self->mbr;
}

/* pnid_obj_set_data(): set the data property of a pnid object */
void
pnid_obj_set_data(struct pnid_obj *self, int data)
{
    assert(self);
    self->data = data;
}
    


    
