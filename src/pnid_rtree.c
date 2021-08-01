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

/*********************
 * R-tree Data Structures:
*********************/

/* #PnidRtree: an R-tree is a height balanced tree with spatial index
   records in it's leaf nodes containing pointers to data objects.

   An R-tree is completely dynamic, inserts and deletes can be
   intermixed with spatial searches and no periodic reorganisation is
   required. */

/* RTMAX: maximum number of records in any node. */
#define RTMAX     4
/* RTMIN: minimum number of records in any node. Must be <= M/2. */
#define RTMIN     2

/* STACKMIN: initial stack size */
#define STACKMIN  50

typedef struct entry   Entry;
typedef struct node    Node;
typedef struct results Results;
typedef PnidBox        Box;

struct pnid_rtree {
  Node     *root;
  Results  *res;	    /* r-tree query results. */
  void     *buf[RTMAX+1];   /* temp buffer for orphan index entries */
};

/* node: a leaf or branch node in the r-tree.

   For a branch node, each occupied index entry represents a child
   node, while in a leaf they contain data entries.

   The index entries of each node, which are of type 'Node' for branch
   nodes and 'Entry' for leaf nodes, are cast to void pointers to
   enable function reuse at any level in the tree.

   The mbr of any index entry can be safely retrieved by casting an
   index entry to type 'Box', as it is the first element of each
   structure. */
struct node {
  Box          I;		/* MBR MUST BE FIRST ELEMENT */
  enum {
    LEAF = 0,
    BRANCH
  }            type;
  Node        *parent;	  
  void        *E[RTMAX+1];		/* index entries */
};

/* entry: an index record in an leaf node. */
struct entry {
  Box      I;			/* MBR MUST BE FIRST ELEMENT */
  PnidObj *tuple;		/* the database entry */
};

/* stack: to hold results of r-tree queries. */
struct results {
  PnidObj  **buf;		/* pnid object stack */
  size_t     rem;		/* empty element count */
  size_t     len;		/* stack buffer size */
};     

/* r-tree insertion algorithms */
static int      insert(PnidRtree *tr, Node *n, void *e);
static Node    *choosenode(Node *n, Box *bbox);
static Node    *chooseleaf(Node *n, Entry *e);
static Node    *adjusttree(Node *n, Node *nn, void **buf);
static void     splitnode(Node *n, Node *nn, void **buf);
static size_t   pickseeds(void **buf, size_t len);
static size_t   picknext(void **buf, size_t len, Box *I, Box *II);
/* r-tree deletion algorithms */
static int      delete(PnidRtree *tr, const Entry *e);
static Node    *findleaf(Node *t, const Entry *e);
static int      condensetree(struct pnid_rtree *tr, Node *n, Node *q);
/* search algorithms */
static Results *search(struct pnid_rtree *tr, const Node *t, const Box *s);
/* results stack */
static int      push(Results *stack, PnidObj *tuple);
static PnidObj *pop(Results *stack);
static PnidObj *peak(Results *stack);
/* mbr calculations */
static void     adjust(Node *n);
static unsigned area(const Box *a);
static void     grow(Box *a, const Box *b);
static int      waste(const Box *a, const Box *b);
static unsigned enlargement(const Box *I, const Box *a);
static int      issubset(const Box *bbox, const Box *mbr);
static int      overlaps(const Box *a, const Box *b);
static int      ismbr(const Node *n);
/* debugging assertions and printing */
static void     checkmbr(const Node *n);
static void     checkparent(const Node *n);
static void     checkdegree(const Node *n);
static void     checkbalance(const Node *n, int depth, int *max); 
static void     printtree(const Node *n, int depth);

/*********************
 * R-tree Interface:
*********************/

/* pnid_rtree_new(): create a new, empty r-tree. Returns handle to the
   tree or NULL on error*/
struct pnid_rtree *
pnid_rtree_new(void)
{
  struct pnid_rtree *tr;

  assert(RTMIN <= RTMAX/2 && "invalid rtree");

  if (!(tr = calloc(1, sizeof *tr)))
    return NULL;
  if (!(tr->root = calloc(1, sizeof *tr->root)))
    return NULL;
  
  return tr;
}

/* pnid_rtree_insert(): insert tuple into tr. Returns less than zero
   on error. */
int
pnid_rtree_insert(struct pnid_rtree *tr, PnidObj *tuple)
{
  Entry *e;			/* new entry to add */
  Node  *l; 			/* target leaf for new entry */

  if (!(e = calloc(1, sizeof *e)))
    return -ENOMEM;

  e->I = pnid_obj_bbox(tuple);
  e->tuple = tuple;
  l = chooseleaf(tr->root, e);

  if (insert(tr, l, e) < 0)
    return -ENOMEM;

  pnid_rtree_check(tr);

  return 0;
}

/* pnid_rtree_delete(): remove tuple from the r-tree. Returns less
   than zero on error */
int
pnid_rtree_delete(struct pnid_rtree *tr, PnidObj *tuple)
{
  Entry e;

  e.I = pnid_obj_bbox(tuple);
  e.tuple = tuple;

  if (delete(tr, &e) < 0)
    return -ENOMEM;

  pnid_obj_delete(tuple);
  pnid_rtree_check(tr);

  return 0;
}

/* pnid_rtree_print(): print the tree to stdout preorder. */
void
pnid_rtree_print(PnidRtree *tr)
{
  printtree(tr->root, 0);
}

/* pnid_rtree_check(): asserts that the r-tree is correctly formed,
   does nothing when NDEBUG is defined.  */
void pnid_rtree_check(struct pnid_rtree *tr)
{
  #ifndef NDEBUG
  int leaf_depth = 0;

  checkparent(tr->root);
  checkdegree(tr->root);
  checkbalance(tr->root, 0, &leaf_depth); 
  checkmbr(tr->root); 
  #endif
}

/*********************
 * R-tree Insertion Algorithms:
*******************/

/* insert(): insert a new index entry e into the rtree at node n. */
static int
insert(PnidRtree *tr, Node *n, void *e)
{
  void **cur;		       /* current index entry */
  Node *nn;		       /* target node's split */
  Node *r, *rr;		       /* root node, it's split */

  /* find or make some space for index entry e in node n  */
  for (cur = n->E; *cur; cur++)
    ;
  if (cur - n->E < RTMAX) {
    *cur = e;
    cur - n->E ? grow(&n->I, e) : (n->I = *(Box *)e);
    nn = NULL;
  } else {			/* n is full, needs to be split */
    if (!(nn = calloc(1, sizeof *nn)))
      return -ENOMEM;
    *tr->buf = e;
    memcpy(tr->buf+1, n->E, RTMAX * sizeof *tr->buf);
    memset(n->E,  0,    RTMAX * sizeof *tr->buf);
    splitnode(n, nn, tr->buf);
  }

  /* propagate any splits up the tree, adjusting mbr's as
     required. The root will change as the tree grows. */
  r = tr->root;
  if (!(rr = adjusttree(n, nn, tr->buf)))
      return -ENOMEM;
  if (rr != r) {		/* if root has split, create new */
    if(!(tr->root = calloc(1, sizeof *tr->root)))
      return -ENOMEM;
    r->parent = tr->root;
    rr->parent = tr->root;
    tr->root->E[0] = r;
    tr->root->E[1] = rr;
    tr->root->type = BRANCH;
  } 

  adjust(tr->root);

  return 0;
}

/* choosenode(): choose the child of branch node n best suited to hold
   an index entry bounded by bbox.

   The best suited node is that whose rectangle needs least
   enlargement to include that of the new entry.

   Ties are resolved by choosing the rectangle of smallest area. */
static Node *
choosenode(Node *n, Box *bbox)
{
  void **cur; 			/* current index entry */
  void  *f;			/* chosen child */
  unsigned d, min;		/* current, minimum enlargement */

  assert(n->type == BRANCH);

  cur = n->E;
  f = *cur;
  min = enlargement(*cur, bbox);
  while (*++cur) {
    d = enlargement(*cur, bbox);
    if (d < min || (d == min && area(*cur) < area(f))) {
      min = d;
      f = *cur;
    }
  }
  return f;
}

/* chooseleaf(): choose the leaf node beneath n best suited to hold
   entry e. */
static Node *
chooseleaf(Node *n, Entry *e)
{
  return n->type == LEAF ? n : chooseleaf(choosenode(n, &e->I), e); 
}

/* splitnode(): distribute RTMAX+1 orphaned index entries in buf
   between two nodes in quadratic time.

   First n and nn are seeded by the least compatible two entries in
   buf. Then the remaining entries are inserted into either node
   dependent on whose covering rectangle will have to be enlarged
   least to accommodate it.

   Ties are resolved by first choosing the node with the smallest mbr
   area, then fewest entries, then finally arbitrarily.

   On completion nodes n and nn will both have at least the minimum
   amount of index entries and their mbr's will have been updated to
   reflect these entries. */
static void
splitnode(Node *n, Node *nn, void **buf)
{
  void **e, **ee;	      /* index entries in n, nn */
  size_t len;		      /* number of entries remaining in buf */
  size_t ij, i;		      /* index of seeds, next assignment */
  int d;		      /* mbr wastefulness metric */

  e = n->E;
  ee = nn->E;
  len = RTMAX+1;

  ij = pickseeds(buf, len);
  *e++ = buf[ij/len];
  memmove(buf+(ij/len), buf+1+(ij/len), (len - 1 -(ij/len)) * sizeof *buf);
  n->I = *(Box *)n->E[0];
  *ee++ = buf[ij%len];
  memmove(buf+(ij%len), buf+1+(ij%len), (len - 1 -(ij%len)) * sizeof *buf);
  nn->I = *(Box *)nn->E[0];
  len -=2;

  while (len) {
    /* To maintain tree balance all nodes must contain the minimum
       number of index entries. */
    assert(len + (e  - n->E)  >= RTMIN && "under full node n");
    if (len + (e - n->E) == RTMIN) {
      memcpy(e, buf, len * sizeof *e);
      adjust(n);
      break;
    }
    assert(len + (ee - nn->E) >= RTMIN && "under full node nn");
    if (len + (ee - nn->E) == RTMIN) {
      memcpy(ee, buf, len * sizeof *ee);
      adjust(nn);
      break;
    }

    i = picknext(buf, len, &n->I, &nn->I); 
    d = waste(&n->I, buf[i]) - waste(&nn->I, buf[i]);
    if (d > 0) {			    /* wastefulness */
      *e++ = buf[i];      
      grow(&n->I, buf[i]);
    } else if (d < 0) {
      *ee++ = buf[i];
      grow(&nn->I, buf[i]);
    } else if (area(&n->I) < area(&nn->I)) { /* smallest area */
      *e++ = buf[i];
      grow(&n->I, buf[i]);
    } else if (area(&n->I) > area(&nn->I)) {
      *ee++ = buf[i];
      grow(&nn->I, buf[i]);
    } else if (e - n->E < ee - nn->E) {	    /* fewest entries */
      *e++ = buf[i];
      grow(&n->I, buf[i]);
    } else {				    
      *ee++ = buf[i];
      grow(&nn->I, buf[i]); 
    }
    memmove(buf+i, buf+1+i, (--len - i) * sizeof *buf);
  }
}

/* pickseeds(): index of the most wasteful pairing in buf */
static size_t
pickseeds(void **buf, size_t len)
{
  size_t i, j, ij;		/* indices 2d, 1d */
  int d, max;			/* wastefulness metric, max */

  for (max=INT_MIN, i=ij=0; i < len; ++i)
    for (j = i+1; j < len; ++j) {
      d = waste(buf[i], buf[j]);
      if (d > max) {
	max = d;
	ij = (i*len) + j;	/* 1d index from 2d index */
      }
    }
  return ij;
}

/* picknext(): return the index of the entry in buf with the strongest
   preference for an mbr at I or II. */
static size_t
picknext(void **buf, size_t len, Box *I, Box *II)
{
  size_t i, imax;	       /* index, max */
  int d, max;		       /* preference, max */

  for (max=INT_MIN, i=0; i < len; ++i) {
    d = abs(enlargement(I, buf[i]) - enlargement(II, buf[i]));
    if (d > max) {
      max = d;
      imax = i;
    }
  }
  return imax;
}

/* adjustree(): Ascend from n to the root adjusting mbrs and
   propagating any node splits.

   If n has been previously split, its split should be provided as nn
   or otherwise be NULL.

   In the case where a split in n propagates up the tree to split the
   root node, this new root node split is returned. If the root has
   not been split, the original root node is returned.

   On memory error, NULL is returned.  */
static Node *
adjusttree(Node *n, Node *nn, void **buf)
{
  Node *p, *pp;			/* parent of n and it's split */
  void **cur;			/* current index entry */

  if (!(p = n->parent))
    return nn ? nn : n;
  pp = NULL;

  adjust(p);

  /* insert split, if there is no room propagate split upwards */
  if (nn) {
    nn->parent = n->parent;
    for (cur = p->E; *cur; cur++)
      ;
    if (cur - p->E < RTMAX) {
      *cur = nn;
      grow(&p->I, &nn->I);
    } else {			/* split in parent */
      if (!(pp = calloc(1, sizeof *pp)))
	return NULL;
      pp->type = p->type;
      *buf = nn;
      memcpy(buf+1, p->E, RTMAX * sizeof *buf);
      memset(p->E,  0,    RTMAX * sizeof *p->E);
      splitnode(p, pp, buf);
      for (cur = pp->E; *cur; cur++)
	((Node *)*cur)->parent = pp;
    }
  }

  return adjusttree(p, pp, buf);
}

/* adjust(): full recalculation of node n's mbr from it's index
   entries. */
static void
adjust(Node *n)
{
  void **cur;

  cur = n->E;
  n->I = **(Box **)cur;
  while (*++cur) 
    grow(&n->I, *cur);
}

/*********************
 * Deletion Algorithms
*******************/

/* delete(): remove entry e from the rtree tr */
static int
delete(struct pnid_rtree *tr, const Entry *e)
{
  Node *r, *l;			/* root, leaf containing e */
  void **cur;			/* current index entry in l */

  r = tr->root;

  /* delete the index entry containing tuple */
  if (!(l = findleaf(r, e)))
    return -1;
  for (cur = l->E; ((Entry *)*cur)->tuple != e->tuple; cur++)
    ;
  assert(cur >= l->E && cur < l->E + RTMAX && "out of array bounds");

  free(*cur);
  memmove(cur, cur+1, (RTMAX - (cur - l->E)) * sizeof *cur);
  if (condensetree(tr, l, NULL) < 0)
      return -1;

  /* the root will change as the tree condenses  */
  assert(r->E[0] && "root is empty");
  if (r->type == BRANCH && !r->E[1]) {
    tr->root = tr->root->E[0];
    free(r);
  }

  return 0;
}

/* findleaf(): starting at t, find the leaf node containing e. */
static Node *
findleaf(Node *t, const Entry *e)
{
  Node *f;			/* node found to contain tuple */
  void **cur;			/* current index entry in t */

  f = NULL;			/* assume not found */

  if (t->type == LEAF) 
    for (cur = t->E; *cur; cur++)
      if (e->tuple == ((Entry *)*cur)->tuple)
	return t;
  if (t->type == BRANCH) 
    for (cur = t->E; !f && *cur; cur++)
      if (issubset(&e->I, *cur)) 
	f = findleaf(*cur, e);
  return f;
}

/* condensetree(): eliminate n and redistribute it's entries if they
   number less than the minimum. Propagate elimination upward as
   necessary while minimising all mbrs up to the root.

   Any nodes that have less than RTMIN index entries after deletion
   are destroyed. These orphaned entries are stored in q and
   reinserted at the same level in the tree.
 */
static int
condensetree(struct pnid_rtree *tr, Node *n, Node *q)
{
  Node *p;			/* parent of n */
  void **cur;			/* current index entry */
  int len;			/* number of entries in n */
  int res;			/* status */

  /* insert orphans */
  if (q) {			
    for (cur = q->E; *cur; cur++) 
      if ((res = insert(tr, n, *cur)) < 0)
	return res;
    q = NULL;
  }
  
  if (!(p = n->parent))		/* finish at root node */
    return 0;

  /* n's reference needs to be deleted from its parent when n has less
     than the minimum number of index entries. */
  for (len = 0; n->E[len]; len++)
    ;
  if (len < RTMIN) {
    for (cur = p->E; *cur != n; cur++)
      ;
    memmove(cur, cur+1, (len - (cur - p->E)) * sizeof *cur);
    q = n;		/* store orphans for later reinsertion */
  } else {
    adjust(n);
  }

  return condensetree(tr, p, q);
}

/*********************
 * Search Algorithms

   The search algorithms can be performed on points, or regions
   inclusively and exclusively. As entries can overlap, multiple
   results can be returned and so are accumulated in a stack which is
   returned.

*******************/

/* search(): populates a list of all entries beneath t whose bounding
   box overlaps the search rectangle s. */
static Results *
search(struct pnid_rtree *tr, const Node *t, const Box *s)
{
  return NULL;
}

/* push(): push tuple to the top of stack. */
static int
push(Results *stack, PnidObj *tuple)
{
  if (!stack->rem) {
    stack->rem = stack->len;
    stack->len *= 2; 
    if (!(stack->buf = realloc(stack->buf, stack->len * sizeof tuple)))
      return -ENOMEM;
  }

  stack->buf[stack->len - stack->rem] = tuple;

  return 0;
}

/* pop(): pop the top of stack, returns null if empty */
static PnidObj *
pop(Results *stack)
{
  return stack->rem == stack->len ? NULL : stack->buf[stack->len - stack->rem++];
}

/* peak(): peak top of stack, returns null if empty */
static PnidObj *
peak(Results *stack)
{
  return stack->rem == stack->len ? NULL : stack->buf[stack->len - stack->rem];
}

/*********************
 * Minimum Bounding Rectangle (MBR) Algorithms

   These mbr functions all take a pointer to one or more 'Box'
   objects. As the first element of both the 'Node' and the 'Entry'
   types are of type 'Box', these types can be safely cast to a (Box
   *) and directly used as arguments to each of these functions.

*******************/

/* area(): area covered by mbr a */
static unsigned
area(const Box *a)
{
  return pnid_box_area(a);
}

/* grow(): grow mbr I, if required, so that bbox is included within
   it's bounds */
static void
grow(Box *I, const Box *bbox)
{
  *I = pnid_box_mbr(bbox, I);
}

/* waste(): wasted area in an mbr containing a and b, will be negative
   when the boxes overlap. */
static int
waste(const Box *a, const Box *b)
{
  const Box mbr = pnid_box_mbr(a, b);

  return pnid_box_area(&mbr) - pnid_box_area(a) - pnid_box_area(b);
}

/* enlargement(): the area by which I must increase to contain a */
static unsigned
enlargement(const Box *I, const Box *a)
{
  const Box mbr = pnid_box_mbr(I, a);

  return pnid_box_area(&mbr) - pnid_box_area(a);
}

/* issubset(): true when bbox is a subset of the mbr. */
static int
issubset(const Box *bbox, const Box *mbr)
{
  return pnid_box_is_subset(bbox, mbr);
}

/* overlaps(): true if a and b overlap */
static int
overlaps(const Box *a, const Box *b)
{
  return !pnid_box_is_separate(a, b);
}

/* ismbr(): true when n's mbr is minimally bounding each of n's index
   entries */
static int
ismbr(const Node *n)
{
  void * const *cur;			    /* current index entry */
  Box mbr;

  cur = n->E;
  mbr = *(Box *)*cur;
  while (*++cur)
    mbr = pnid_box_mbr(&mbr, *cur);
    
  return
    pnid_box_get_left(&mbr)   == pnid_box_get_left(&n->I)  &&
    pnid_box_get_right(&mbr)  == pnid_box_get_right(&n->I) &&
    pnid_box_get_top(&mbr)    == pnid_box_get_top(&n->I)   &&
    pnid_box_get_bottom(&mbr) == pnid_box_get_bottom(&n->I);     
}

/*********************
 * R-tree debugging assertions

   Order of tree traversal varies between function but in each case
   all nodes beneath n will be checked.
   
*******************/

/* checkmbr(): assert all mbrs are contained by their parents and
   minimally bounding */
static void
checkmbr(const Node *n)
{
  void * const *cur;		/* current index entry */

  for (cur = n->E; *cur; cur++) {
    if (n->type == BRANCH)
      checkmbr(*cur);
    assert(issubset(*cur, &n->I) && "entry not contained in mbr");
  }
  assert(ismbr(n) && "mbr not minimally bounding entries");
}

/* checkparent(): assert each node references its parent */
static void
checkparent(const Node *n)
{
  void * const *cur;		/* current index entry */

  if (n->type == LEAF)
    return;
  for (cur = n->E; *cur; cur++) {
    assert(n == ((Node *)*cur)->parent);
    checkparent(*cur);
  }
}

/* checkbalance(): assert that all leaf nodes beneath n have the same
   depth */
static void
checkbalance(const Node *n, int depth, int *max)
{
  void * const *cur;		/* current index entry */

  if (n->type == LEAF) {
    if (*max > 0)
      assert(depth == *max);
    else
      *max = depth;
  } else {
    for (cur = n->E; *cur; cur++)
      checkbalance(*cur, depth + 1, max);
  }
}

/* checkdegree(): assert every node contains between RTMIN and RTMAX
   index records unless it is the root, which has at least two
   children unless it is a leaf. */
static void
checkdegree(const Node *n)
{
  void * const *cur; 		/* current index entry */

  for (cur = n->E; *cur; cur++)
    ;
  assert(cur - n->E <= RTMAX);
  if (n->parent)
    assert(cur - n->E >= RTMIN);
  else if (n->type == BRANCH)	/* root is branch */
    assert(cur - n->E >= 2);
    
  if (n->type == BRANCH)
    for (cur = n->E; *cur; cur++)
      checkdegree(*cur);
}
 
/* printtree(): from node n in preorder */
static void
printtree(const Node *n, int depth)
{
  void * const *cur;		/* current index entry */
  int i;

  for (i = 0; i < depth; ++i) 	/* indent according to depth */
    putchar('-');
  printf("%-8s", n->parent
	 ? (n->type == BRANCH ? "BRANCH" : "LEAF") : "ROOT");
  printf("I(%03u,%03u)(%03u,%03u) E",
	 n->I.nw.x, n->I.nw.y,
	 n->I.se.x, n->I.se.y);
  putchar('[');
  for (cur = n->E; *cur; ++cur) {
    printf(" #%ld(%03u,%03u)(%03u,%03u) ",
	   cur - n->E,
	   ((Box *)*cur)->nw.x, ((Box *)*cur)->nw.y,
	   ((Box *)*cur)->se.x, ((Box *)*cur)->se.y);
  }
  putchar(']');
  putchar('\n'); 
	  
  if (n->type == BRANCH)
    for (cur = n->E; *cur; ++cur)
      printtree(*cur, depth+1);
}
