/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_rtree.h - spatial database for pnid drawing objects
   implemented as an R-tree.  */

#ifndef __PNID_RTREE_H
#define __PNID_RTREE_H

#include "pnid_obj.h"
#include "pnid_bbox.h"

/* #PnidRtree: the spatial database  */
typedef struct pnid_rtee PnidRtree;

/* PnidAction: callback to apply to found entries when searching
   spatial the R-tree */
typedef void (*PnidRtreeCb)(void *, PnidObj *);

/* Create and destroy a spatial database */
PnidRtree *pnid_rtree_new(void);


#endif /* __PNID_RTREE_H */
