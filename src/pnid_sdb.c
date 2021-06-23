/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_sdb.c - Search, insert and delete #PnidObj using spatial
   coordinates.

   References:

   A. Guttman (1984) R Trees: A Dynamic Index Structure for Spatial
   Searching.
   
*/

#include <stdlib.h>
#include <assert.h>

#include "pnid_sdb.h"
#include "pnid_obj.h"
#include "pnid_bbox.h"

/* 
 * rtree_node: region tree node
*/

/* MAX_RECORDS: maximum number of index records in any leaf node. */
#define PNID_SDB_MAX_RECORDS   10
/* MIN_RECORDS: minimum number of index records in any leaf node,
   excepting the root. */
#define PNID_SDB_MIN_RECORDS   (PNID_SDB_MAX_RECORDS / 2) 

struct rtree_node {
  /* node.mbr: minimal bounding rectangle, the smallest rectangle to
     spatially contain all #PnidObj entries, including within any
     children. */
  PnidBBox             mbr;

  /* node.type: A node in an R-tree can be either a branch with child
     nodes, or a leaf containing a #PnidObj entry */
  enum {
    NODE_TYPE_BRANCH,
    NODE_TYPE_LEAF
  }                    type;

  union {
    struct rtree_node *child[PNID_SDB_MAX_RECORDS];
    PnidObj           *entry[PNID_SDB_MAX_RECORDS];
  };
};

/* rtree_search(): starting at node t, find all index records whose
   rectangles overlap a search rectangle s - then apply action to
   them. */
void
rtree_search(const struct rtree_node *t, const PnidBBox *s,
	     void (*action)(void *, PnidObj *), void *out)
{
  int i;

  if (t->type == NODE_TYPE_BRANCH)
    for (i = 0; i < PNID_SDB_MAX_RECORDS && t->child[i]; ++i)
      if (pnid_bbox_is_overlap(s, &t->child[i]->mbr))
	rtree_search(t->child[i], s, action, out);

  if (t->type == NODE_TYPE_LEAF)
    for (i = 0; i < PNID_SDB_MAX_RECORDS && t->entry[i]; ++i)
      if (pnid_bbox_is_overlap(s, &t->mbr))
	action(out, t->entry[i]);
}

