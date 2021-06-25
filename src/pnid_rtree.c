/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_rtree.c - An R-tree to search, insert and delete #PnidObj
   using positive integer spatial indices.

   References:

   A. Guttman (1984) R Trees: A Dynamic Index Structure for Spatial
   Searching. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pnid_rtree.h"
#include "pnid_obj.h"
#include "pnid_bbox.h"

/* RTREE_MAX: maximum number of records in any node. */
#define RTREE_MAX   6

struct rtree_node {
    /* rtree_node.mbr: minimal bounding rectangle, the smallest
       rectangle to spatially contain all #PnidObj entries. */
    PnidBBox             mbr;

    /* rtree_node.type: type of node in the r-tree. */
    enum {
	NODE_TYPE_BRANCH,
	NODE_TYPE_LEAF
    }                    type;

    /* node.(child|entry): data entries in for a leaf and children of a
       branch node. */
    union {
	struct rtree_node *child[RTREE_MAX];
	PnidObj           *entry[RTREE_MAX];
    };

    /* node.parent: points the parent or NULL if this is the root */
    struct rtree_node   *parent;
};

/* rtree_search(): starting at node t, find all index records whose
   rectangles overlap a search rectangle s and apply an action to
   them. */
static void
pnid_rtree_search(const struct rtree_node *t, const PnidBBox *s,
	     void (*action)(void *, PnidObj *), void *results)
{
    int i;

    assert(t && s && action);

    if (t->type == NODE_TYPE_BRANCH)
	for (i = 0; i < RTREE_MAX && t->child[i]; ++i)
	    if (pnid_bbox_is_overlap(s, &t->child[i]->mbr))
		pnid_rtree_search(t->child[i], s, action, results);

    if (t->type == NODE_TYPE_LEAF)
	for (i = 0; i < RTREE_MAX && t->entry[i]; ++i)
	    if (pnid_bbox_is_overlap(s, &t->mbr))
		action(results, t->entry[i]);
}

