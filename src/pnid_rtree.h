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
void       pnid_rtree_destroy(PnidRtree *tr); 

/* Add and remove individual entries to and from the database*/
int pnid_rtree_insert(PnidRtree *tr, PnidObj *tuple);
int pnid_rtree_delete(PnidRtree *tr, PnidObj *tuple);

/* Query the database */

/* Debugging and testing: */

/* printtree(): print rtree to stdout preorder */
void pnid_rtree_print(PnidRtree *tr);

/* pnid_rtree_check(): asserts that the r-tree is correctly formed,
   does nothing when NDEBUG is defined.  */
void pnid_rtree_check(struct pnid_rtree *tr);






#endif /* __PNID_RTREE_H */
