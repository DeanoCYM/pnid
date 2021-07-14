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
#include <errno.h>

#include "pnid_rtree.h"

#include "pnid_obj.h"
#include "pnid_box.h"

/* MBR(): Retreive address of mbr from a Node or Entry pointer. */
#define MBR(A) ((Box *)A)

/*********************
 * R-tree Data Structures:
*********************/

/* #PnidRtree: an R-tree is a height balanced tree with spatial index
   records in it's leaf nodes containing pointers to data objects.

   An R-tree is completely dynamic, inserts and deletes can be
   intermixed with spatial searches and no periodic reorgaisation is
   required. */

/* M: maximum number of records in any node. */
#define M   2
/* m: minimum number of records in any node. Must be <= M/2. */
#define m   1

typedef PnidBox      Box;
typedef struct entry Entry;
typedef struct node  Node;

struct pnid_rtree {
  Node *root;
  void *buf[M+1];		/* unnassigned index entries */
};

/* node: a leaf or branch node in the r-tree.

   For a branch node, each occupied index entry represents a child
   node, while in a leaf they contain data entries. */
struct node {
  Box          I;		/* MBR MUST BE FIRST ELEMENT */
  enum {
    LEAF = 0,
    BRANCH
  }            type;
  Node        *parent;	  
  void        *E[M+1];		/* index entries */
};

/* entry: an index record in an leaf node. */
struct entry {
  Box      I;			/* MBR MUST BE FIRST ELEMENT */
  PnidObj *tuple;		/* the database entry */
};

/* r-tree insertion algorithms */
static int     insert(struct pnid_rtree *tr, Entry *e);
static Node   *chooseleaf(Node *n, Entry *e);
static Node   *adjusttree(Node *n, Node *nn, void **buf);
static void    splitnode(Node *n, Node *nn, void **buf);
static size_t  pickseeds(void **buf, size_t len);
static size_t  picknext(void **buf, size_t len, Box *I, Box *II);

/* mbr calculations */
static unsigned area(const Box *a);
static void     grow(Box *a, const Box *b);
static int      waste(const Box *a, const Box *b);
static unsigned growth(const Box *I, const Box *a);

/*********************
 * R-tree Interface:
*********************/

/* pnid_rtree_new(): create a new, empty r-tree.

  Return:   new R-tree handle on success
	    NULL on error */
struct pnid_rtree *
pnid_rtree_new(void)
{
  struct pnid_rtree *tr;

  assert(m <= M/2);		/* requirement of an R-tree */

  if (!(tr = calloc(1, sizeof *tr)))
    return NULL;
  if (!(tr->root = calloc(1, sizeof *tr->root)))
    return NULL;
  
  return tr;
}

/* pnid_rtree_insert(): insert an object into the r-tree

  Return:   == 0 success.
	     < 0 error.   */
int
pnid_rtree_insert(struct pnid_rtree *tr, PnidObj *toadd)
{
  struct entry *e;

  if (!(e = calloc(1, sizeof *e)))
    return -ENOMEM;

  e->I = pnid_obj_bbox(toadd);
  e->tuple = toadd;

  if (insert(tr, e) < 0)
    return -ENOMEM;

  return 0;
}

/*********************
 * R-tree Algorithms:
*******************/

/* insert(): insert a new index entry e into the r-tree tr. */
static int
insert(struct pnid_rtree *tr, Entry *e)
{
  Node *r, *rr;		       /* root node, it's split */
  Node *l, *ll;		       /* leaf node, it's split */
  Entry **entries, **mt;       /* leaf node entries, empty entry */

  r = tr->root;

  /* e will be added to the most appropirate leaf according to it's
     mbr, this leaf maybe full and then will require splitting */
  l = chooseleaf(r, e);		
  assert(l && l->type == LEAF);
  for (entries = (Entry **)l->E, mt = entries; *mt; mt++)
    ;
  if (mt - entries < M) {
    *mt = e;
    mt - entries ? grow(MBR(l), MBR(e)) : (l->I = e->I);
    ll = NULL;
  } else {			/* l is full, needs to be split */
    if (!(ll = calloc(1, sizeof *ll)))
      return -ENOMEM;
    *tr->buf = e;
    memcpy(tr->buf+1, l->E, M * sizeof *tr->buf);
    memset(l->E,  0,    M * sizeof *tr->buf);
    splitnode(l, ll, tr->buf);
  }

  /* update mbr's and propagate any splits up the tree, the root can
     change as the tree grows and/or condenses */
  if (!(rr = adjusttree(l, ll, tr->buf)))
      return -ENOMEM;
  if (rr != r) {		/* root has split, create new */
    if(!(tr->root = calloc(1, sizeof *tr->root)))
      return -ENOMEM;
    tr->root->I = r->I;
    grow(MBR(tr->root), MBR(rr));
    tr->root->type = BRANCH;
    tr->root->E[0] = r;
    tr->root->E[1] = rr;
    r->parent = tr->root;
    rr->parent = tr->root;
  }


  return 0;
}

/* chooseleaf(): choose the leaf node beneath n best suited to hold
   entry e.

   The chosen leaf node is that whose rectangle needs least enlargement to
   include that of the new entry.

   Any ties are resolved by choosing the rectangle of smallest area. */
static Node *
chooseleaf(Node *n, Entry *e)
{
  Node **c, *f;			/* current child, chosen node */
  unsigned cur, min;		/* enlargement current, min  */

  if (n->type == LEAF)
    return n;

  c = (Node **)n->E;
  f = *c;
  min = growth(MBR(*c++), MBR(e));
  assert(f && "empty branch!");

  while (*++c) {
    cur = growth(MBR(*c), MBR(e));
    if (cur < min || (cur == min && area(MBR(*c)) < area(MBR(f)))) {
      min = cur;
      f = *c;
    }
  }
  return chooseleaf(f, e);	/* recurse to leaf node */
}

/* splitnode(): distribute the M+1 index entries in buf between two
   nodes. */
static void
splitnode(Node *n, Node *nn, void **buf)
{
  void **e, **ee;	      /* index entries in l, ll */
  size_t len;		      /* number of entries remaining in buf */
  size_t ij, i;		      /* index of seed, next assignment */
  int d;		      /* mbr wastefulness metric */

  e = n->E;
  ee = nn->E;
  len = M+1;

  /* The most incompatible pair of entries in buf are used to seed
     each node */
  ij = pickseeds(buf, len);
  *e++ = buf[ij/len];
  memmove(buf+(ij/len), buf+1+(ij/len), (len - 1 -(ij/len)) * sizeof *buf);
  *ee++ = buf[ij%len];
  memmove(buf+(ij%len), buf+1+(ij%len), (len - 1 -(ij%len)) * sizeof *buf);
  len -=2;

  /* Remaining index entries are added to the most appropriate node
     according to the compatability of their mbr's */
  while (len) {
    /* To maintain tree balance all nodes must contain the minimum
       number of index entries. */
    if (len + (e - n->E) == m) {
      memcpy(e, buf, len * sizeof *e);
      while (--len) 
	grow(MBR(e), buf[len]);
      return;
    }
    if (len + (ee - nn->E) == m) {
      memcpy(ee, buf, len * sizeof *ee);
      while (--len) 
	grow(MBR(ee), buf[len]);
      return;
    }

    i = picknext(buf, len, MBR(n), MBR(nn)); 

    /* Insert next entry in the node whose covering rectangle will
       have to be enlarged least to accommodate it. Resolve ties by
       first choosing the node with the smallest mbr area, then fewest
       entries, then just give up and pick the first . */
    d = waste(MBR(n), MBR(buf[i])) - waste(MBR(nn), MBR(buf[i]));
    if (d > 0) {
      *e++ = buf[i];      
      grow(MBR(n), MBR(buf[i]));
    } else if (d < 0) {
      *ee++ = buf[i];
      grow(MBR(nn), MBR(buf[i]));
    } else if (area(MBR(n)) < area(MBR(nn))) {
      *e++ = buf[i];
      grow(MBR(n), MBR(buf[i]));
    } else if (area(MBR(n)) > area(MBR(nn))) {
      *ee++ = buf[i];
      grow(MBR(nn), MBR(buf[i]));
    } else if (e - n->E < ee - nn->E) { /* fewest entries */
      *e++ = buf[i];
      grow(MBR(n), MBR(buf[i]));
    } else {
      *ee++ = buf[i];
      grow(MBR(nn), MBR(buf[i])); 
    }

    memmove(buf+i, buf+1+i, (--len - i) * sizeof *buf);
  }
}

/* pickseeds(): index of the most wasteful pairing in buf */
static size_t
pickseeds(void **buf, size_t len)
{
  size_t i, j, ij;		/* indices 2d, 1d */
  int cur, max;			/* wastefulness metric current, max */

  for (max=INT_MIN, i=ij=0; i < len; ++i)
    for (j = i+1; j < len; ++j) {
      cur = waste(MBR(buf[i]), MBR(buf[j]));
      if (cur > max) {
	max = cur;
	ij = (i*len) + j;	/* 1D index from 2D index */
      }
    }
  return ij;
}

/* picknext(): return the index of the entry in buf with the strongest
   preference for an mbr at I or II. */
static size_t
picknext(void **buf, size_t len, Box *I, Box *II)
{
  size_t i, imax;	       /* index of current index entry, max */
  int cur, max;		       /* preference current, max */

  for (max=INT_MIN, i=0; i < len; ++i) {
    cur = abs(growth(I, MBR(buf[i])) - growth(II, MBR(buf[i])));
    if (cur > max) {
      max = cur;
      imax = i;
    }
  }
  return imax;
}

/* adjustree(): Ascend from n to the root adjusting mbrs and
   propagating node splits.

   In the case where the root node is split, this new split is
   returned. Where the root node is not split, the root node itself is
   returned. NULL will be returned in the case of a memory error. 
   */
static Node *
adjusttree(Node *n, Node *nn, void **buf)
{
  Node *p, **c, *pp;	    /* parent, it's children, it's split */
  void **I;

  if (!(p = n->parent))
    return nn ? nn : n;

  pp = NULL;

  /* adjust the bounding box of the current node */
  I = n->E;
  n->I = *MBR(*I);
  while (*++I)
    n->I = pnid_box_mbr(&n->I, MBR(*I));

  /* insert split, if there is no room propagate split upwards */
  if (nn) {
    nn->parent = n->parent;
    for (c = (Node **)p->E; *c; c++)
      ;
    if (c - (Node **)p->E < M) {
      *c = nn;
    } else {			/* split in parent */
      if (!(pp = calloc(1, sizeof *pp)))
	return NULL;
      pp->type = p->type;
      *buf = nn;
      memcpy(buf+1, p->E, M * sizeof *buf);
      memset(p->E,  0,    M * sizeof *p->E);
      splitnode(p, pp, buf);
    }
  }

  return adjusttree(p, pp, buf);
}

/*********************
 * Minimum Bounding Rectangle (MBR) Algorithms:
*******************/

/* area(): area covered by mbr a */
static unsigned
area(const Box *a)
{
  return pnid_box_area(a);
}

/* grow(): grow mbr a so that b is included within it's bounds */
static void
grow(Box *a, const Box *b)
{
  *a = pnid_box_mbr(a, b);
}

/* waste(): wasted area in an mbr containing a and b, will be negative
   when the boxes overlap. */
static int
waste(const Box *a, const Box *b)
{
  const Box mbr = pnid_box_mbr(a, b);

  return pnid_box_area(&mbr) - pnid_box_area(a) - pnid_box_area(b);
}

/* mrbgrowth(): the area by which I must increase to contain a */
static unsigned
growth(const Box *I, const Box *a)
{
  const Box mbr = pnid_box_mbr(I, a);

  return pnid_box_area(&mbr) - pnid_box_area(a);
}


#ifndef NDEBUG
/*********************
 * Debugging and Testing: 
*******************/

static void printtree(struct node *n, int level, int *entries);

/* pnid_rtree_get_max(): maximum index entries in a node */
unsigned
pnid_rtree_get_max(void)
{
  return M;
}

/* pnid_rtree_get_max(): minimum index entries in a node */
unsigned
pnid_rtree_get_min(void)
{
  return m;
}

/* pnid_rtree_print(): print entire tree to stdout */
int
pnid_rtree_print(struct pnid_rtree *tr)
{
  int entries = 0;

  printtree(rtr->root, 0, &entries);

  return entries;
}

/* printtree(): print tree in pre-order starting at n */
static void
printtree(struct node *n, int depth, int* entries)
{
  void **cur;			/* current index entry */
  int    i;

  for (i = 0; i < depth; ++i) 	/* indent according to depth */
    putchar('-');

  printf("%-8s", n->parent
	 ? (n->type == BRANCH ? "BRANCH" : "LEAF") : "ROOT");
  printf("I(%03u,%03u)(%03u,%03u) E",
	 MBR(n)->nw.x, MBR(n)->nw.y,
	 MBR(n)->se.x, MBR(n)->se.y);

  putchar('[');
  for (cur = n->E; *cur; ++cur) {
    printf("#%ld(%03u,%03u)(%03u,%03u)",
	   cur - n->E,
	   MBR(*cur)->nw.x, MBR(*cur)->nw.y,
	   MBR(*cur)->se.x, MBR(*cur)->se.y);
    if (n->type == LEAF)
      ++*entries;
  }
  putchar(']');
  putchar('\n'); 
	  
  if (n->type == BRANCH)
    for (cur = n->E; *cur; ++cur)
      printtree(*cur, depth+1, entries);
}

#endif	/* NDEBUG */
