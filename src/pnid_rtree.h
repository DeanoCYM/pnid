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

/* Create and destroy the entire database */
PnidRtree *pnid_rtree_new(void);
void       pnid_rtree_delete(PnidRtree *db); 

/* Add and remove individual entries to and from the database*/
int pnid_rtree_insert(PnidRtree *db, PnidObj *entry);

/* Query the database */

#ifndef NDEBUG

unsigned pnid_rtree_get_max(void);
unsigned pnid_rtree_get_min(void);

/* Print R-tree in pre-order to stdout, returns total number of objects */
int pnid_rtree_print(PnidRtree *rtr);

#endif	/* NDEBUG */


#endif /* __PNID_RTREE_H */
