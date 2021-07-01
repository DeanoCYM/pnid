/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_rtree.h - spatial database for pnid drawing objects
   implemented as an R-tree.  */

#ifndef __PNID_RTREE_H
#define __PNID_RTREE_H

#include "pnid_obj.h"
#include "pnid_box.h"

/* #PnidRtree: the spatial database  */
typedef struct pnid_rtree PnidRtree;

/* Create and destroy the database */
PnidRtree *pnid_rtree_new(void);
void       pnid_rtree_delete(PnidRtree *db); 

/* Add and remove entries to the database */
int pnid_rtree_insert(PnidRtree *db, PnidObj *entry);

#endif /* __PNID_RTREE_H */
