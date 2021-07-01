/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_rtree.c - An R-tree to search, insert and delete #PnidObj
   using positive integer spatial indices.

   References:

   A. Guttman (1984) R Trees: A Dynamic Index Structure for Spatial
   Searching. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "pnid_rtree.h"
#include "pnid_obj.h"
#include "pnid_box.h"

#define count_entries(X)   count_records(X)
#define count_children(X)  count_records(X)

#define GROWTH(MBR, BBOX)  pnid_box_mbr_enlargement(MBR, BBOX)
#define WASTE(A, B)        pnid_box_mbr_waste(A, B)

/* RTREE_MAX: maximum number of records in any node. */
#define RTREE_MAX   4
/* RTREE_MIN: minimum number of records in any node. */
#define RTREE_MIN   2		/* must be <= RTREE_MAX / 2 */

/* entry: the record in an leaf node.  */
struct entry {
  PnidBox *bbox;
  PnidObj *data;
};

/* node: a node in the R-tree */
struct node {
  /* node.parent: points the parent or NULL if this is the root */
  struct node      *parent;

  /* node.mbr: minimal bounding rectangle, the smallest rectangle to
     spatially contain the bounding boxes of all entries, and those of
     any children recursively. */
  PnidBox           mbr;

  /* node.type: type of node in the r-tree. */
  enum {
    LEAF_NODE = 0,
    BRANCH_NODE
  }                 type;

  /* node.(child|entry): a record. */
  union {
    struct node    *children[RTREE_MAX]; /* branch node */
    struct entry   *entries[RTREE_MAX];	 /* leaf node */
  };
};

/* #PnidRtree: an R-tree. */
struct pnid_rtree {
  struct node *root;
};

static int          insert_entry(struct node *r, struct entry *e);
static struct node *choose_leaf(struct node *n, PnidBox *ei);
static struct node *min_mbr_growth(struct node *a, struct node *b, PnidBox *ei); 
static struct node *split_leaf(struct node *n, struct entry *e);
static size_t       pick_seeds(struct entry **pool, size_t len);
static void         update_mbr(struct node *n);
static int          count_records(struct node *n);
static int          is_node_full(struct node *n);
static int          compare_mbr(const void *a, const void *b, void *arg);

/* pnid_rtree_new(): create a new R-tree.

   Allocate memory for an R-tree containing just an empty root node.   
 */
struct pnid_rtree *
pnid_rtree_new(void)
{
  struct pnid_rtree *db;	/* new R-tree */
  struct node *r;		/* empty root node */

  if ((db = calloc(1, sizeof *db)) == NULL)
    return NULL;
  if ((r = calloc(1, sizeof *r)) == NULL)
    return NULL;

  db->root = r;

  return db;
}

/* pnid_rtree_delete(): free rtree and all nodes within it. */
void
pnid_rtree_delete(struct pnid_rtree *db)
{
  if (db) {
    if (db->root) {
      // TODO: free all nodes
      free(db->root);
      db->root = NULL;
    }
    free(db);
  }
}

/* pnid_rtree_insert(): insert entry into the r-tree. */
int
pnid_rtree_insert(struct pnid_rtree *db, PnidObj *entry)
{
  struct entry *e;

  if ((e = malloc(sizeof *e)) == NULL)
    return -1;
  e->data = entry;
  e->bbox = pnid_obj_get_box(entry);

  return insert_entry(db->root, e);
}

/* insert_entry(): adds the entry e to to the R-tree at an appropriate
   leaf node, starting the search at node r.

   If the chosen node is full, it is split. Splits propagate up the
   tree and so address of r can change. */
static int
insert_entry(struct node *r, struct entry *e)
{
  struct node *l, *ll;		/* chosen node and it's split */

  l = choose_leaf(r, e->bbox);

  if (is_node_full(l)) {
    ll = split_leaf(l, e);
    if (ll == NULL)
      return -1;
  }

  // TBC

  return 0;
}
    
/* choose_leaf(): starting a recursive search from node n, return the
   node who's minimum bounding region requires the least growth to
   contain the bounding box ei.  */
static struct node *
choose_leaf(struct node *n, PnidBox *ei)
{
  struct node **cur, *min;	/* current child of n, selected node */

  if (n->type == LEAF_NODE)
    return n;

  cur = n->children;
  min = *cur;
  while (cur++ - n->children < RTREE_MAX)
    min = min_mbr_growth(min, *cur, ei); 


  return choose_leaf(min, ei);
}

/* min_mbr_growth(): returns the node which has the least mbr
   enlargement when the bounding box ei is added to it.

   Empty mbr's will grow by the full value of ei  
 */
static struct node *
min_mbr_growth(struct node *a, struct node *b, PnidBox *ei) 
{
  PnidBox *ai, *bi;		 /* mbr of a, b */

  ai = a ? &a->mbr : NULL;
  bi = b ? &b->mbr : NULL;

  if (!ai && !bi)
    return a;
  if (GROWTH(ai, ei) < GROWTH(bi, ei))
    return a;
  if (GROWTH(ai, ei) > GROWTH(bi, ei))
    return b;

  return pnid_box_area(ai) <= pnid_box_area(bi) ? a : b;
}

/* split_leaf(): Split a full leaf node and new entry e into two leaf
   nodes l and ll. */
static struct node *
split_leaf(struct node *n, struct entry *e)
{
  static const size_t len = RTREE_MAX+1; /* number of entries to split */

  struct node   *pair[2];	/* pair of split nodes */
  struct entry **dest[2];	/* destination cursors for pair */
  struct entry **pool, **src;	/* entry pool, current entry */
  size_t         idx, i;	/* src index, iterator */
  int            d; 		/* difference in mbr growth */

  /* construct pair consiting of original node and its split */
  pair[0] = n;
  dest[0] = pair[0]->entries;

  if (!(pair[1] = calloc(1, sizeof *pair[1])))
    return NULL;
  pair[1]->parent = pair[0]->parent;
  dest[1] = pair[1]->entries;
  
  /* Create the entry pool from which to fill the pair */
  if (!(pool = malloc(len)))
    return NULL;
  *pool = e;
  memcpy(pool+1, pair[0]->entries, RTREE_MAX * sizeof *pool);
  memset(pair[0]->entries, 0, RTREE_MAX * sizeof *pair[0]->entries);

  /* seed pair with least compatible entries in pool */
  idx = pick_seeds(pool, len);
  for (i = 0;  i < 2; ++i) {
    *dest[i]++  = pool[idx/len];
    memmove(pool+1+idx/len, pool+idx/len, (len-idx/len) * sizeof *pool);
    pool[len-1] = NULL;
    update_mbr(pair[i]);
  }

  /* populate pair from pool  */
  src = pool + len-2;
  while (--src - pool) {

    /* check if one group has so few entries that all the rest must be
       assigned for it to have the minimum number of entries. */
    for (i = 0;  i < 2; ++i)
      if (RTREE_MIN == (dest[i] - pair[i]->entries) + (src - pool)) {
	memcpy(dest[i], pool, (src - pool) * sizeof *dest[i]);
	update_mbr(pair[i]);
	break;
      }

    /* sort the pool according to the entry with the greatest
       preference for one group in acending order. */
    qsort_r(pool, src-pool, sizeof *pool, compare_mbr, pair);
    
    d = GROWTH( &(pair[0]->mbr), (*src)->bbox)
      - GROWTH( &(pair[0]->mbr), (*src)->bbox);

    /* assign an entry according to its mbr preference  */
    if (d > 0) {
      *dest[0] = *src--;
      update_mbr(pair[0]);
      dest[0]++;
    } else if (d < 0) {
      *dest[1] = *src--;
      update_mbr(pair[1]);
      dest[1]++;
    } else if (pnid_box_area(&(pair[0]->mbr)) < pnid_box_area(&(pair[0]->mbr))) {
      *dest[0] = *src--;
      update_mbr(pair[0]);
      dest[0]++;
    } else {
      *dest[1] = *src--;
      update_mbr(pair[1]);
      dest[1]++;
    }
  }


  free(pool);

  assert(count_entries(pair[0]) >= RTREE_MIN);
  assert(count_entries(pair[1]) >= RTREE_MIN);

  return pair[1];
}

/* pick_seeds(): returns index of the most wasteful bounding box
   pairing from entries in pool. */
static size_t
pick_seeds(struct entry **pool, size_t len)
{
  int i, j, ij;			/* indices */
  int cur, max;			/* wastefulness current, max */

  for (max=INT_MIN, i=ij=0; i < len; ++i)
    for (j = i+1; j < len; ++j) {
      cur = WASTE(pool[i]->bbox, pool[j]->bbox);
      if (cur > max) {
	max = cur;
	ij = i*j;
      }
    }
  
  return ij;
}

/* update_mbr(): recalculate a nodes minimum bounding rectangle. */
static void
update_mbr(struct node *n)
{
  int i;

  memset(&n->mbr, 0, sizeof n->mbr);

  if (n->type == LEAF_NODE)
    for (i = 0; i < RTREE_MAX && n->entries[i]; ++i) 
      pnid_box_mbr_grow(&n->mbr, n->entries[i]->bbox);

  if (n->type == BRANCH_NODE) {
    for (i = 0; i < RTREE_MAX && n->children[i]; ++i) 
      update_mbr(n->children[i]);
    for (i = 0; i < RTREE_MAX && n->children[i]; ++i)
      pnid_box_mbr_grow(&n->mbr, &n->children[i]->mbr);
  }
}

/* count_records(): number of children/entries in a branch/leaf node
   respectively. */
static int
count_records(struct node *n)
{
  struct node **cur;	/* current union field */
  int count;

  cur = n->children;
  while (cur - n->children < RTREE_MAX)
    if (*cur++)
      ++count;
  
  return count;
}

/* is_node_full(): true only when node is full. */
static int
is_node_full(struct node *n)
{
  return count_records(n) == RTREE_MAX ? 1 : 0; 
}

/* compare_mbr(): compares the growth costs of putting the entries at
   a and b into the two minimum bounding rectangles at arg[0] and
   arg[1].

   For an entry, a large difference in growth when tested against each
   mbr is indicative of a strong preference for a particular mbr.

   Returns positive when a has a stronger preference for an mbr than b
           negative when b has a stronger preference for an mbr than a
           zero when there is no difference in preference for an mbr.   
*/
static int
compare_mbr(const void *a, const void *b, void *arg)
{
  const struct entry *n  = (const struct entry *)a;
  const struct entry *nn = (const struct entry *)b;
  const PnidBox **i = (const PnidBox **)arg;

  int dn, dnn;			/* growth differences */

  dn  = GROWTH(n->bbox,  i[0]) - GROWTH(n->bbox,  i[1]);
  dnn = GROWTH(nn->bbox, i[0]) - GROWTH(nn->bbox, i[1]);

  if (abs(dn) > abs(dnn))
    return 1;
  if (abs(dn) < abs(dnn))
    return -1;

  return 0;
}

///* rtree_search(): starting at node t, find all index records whose
//   rectangles overlap a search rectangle s and apply an action to
//   them. */
//static void
//search_rtree(const struct node *t, const PnidBox *s,
//	     void (*action)(void *, PnidObj *), void *results)
//{
//    int i;
//
//    assert(t && s && action);
//
//    if (t->type == NODE_TYPE_BRANCH)
//	for (i = 0; i < RTREE_MAX && t->child[i]; ++i)
//	    if (pnid_box_is_overlap(s, &t->child[i]->mbr))
//		search_rtree(t->child[i], s, action, results);
//
//    if (t->type == NODE_TYPE_LEAF)
//	for (i = 0; i < RTREE_MAX && t->entry[i]; ++i)
//	    if (pnid_box_is_overlap(s, &t->mbr))
//		action(results, t->entry[i]);
//}

