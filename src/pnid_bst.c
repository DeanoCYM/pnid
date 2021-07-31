/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* pnid_bst.c - a binary search tree */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "pnid_obj.h"

typedef struct node Node;
typedef struct bst  PnidBst;

struct node {
  PnidObj        *tuple;
  struct bstnode *left;
  struct bstnode *right;
};

struct bst {
  struct node   *root;
  pnid_obj_cmp  *cb;
}

/*********************
 * Binary Search Tree Interface:
*********************/

/* pnid_bst_new(): create a new, empty binary search tree. Returns
   handle to the tree or NULL on error. */
struct pnid_bst *
pnid_bst_new(pnid_obj_cmp cb);
{
  struct pnid_bst *tr;

  if (!(tr = malloc(sizeof *tr)))
    return NULL;

  tr->root = NULL;
  tr->cb = cb;

  return tr;
}

/* pnid_bst_insert(): add tuple to tr. */
int
pnid_bst_insert(struct bst *tr, const PnidObj *tuple)
{
  if (!(tr->root = insert(tr->root, tuple, tr->cmp))
      return -1;
}

/*********************
 * Binary Search Tree Algorithms:
*******************/

/* push(): add a tuple to the binary search tree. Returns current node
   or NULL on error. */
static Node *
insert(Node *cur, PnidObj *tuple, pnid_obj_cmp cmp)
{
  int res;

  if (!cur) {
    if (!(cur = malloc(sizeof *cur)))
      return NULL;
    cur->tuple = tuple;
    cur->count = 1;
    cur->left = cur->right = NULL; 
    return cur;
  } 
  
  if ((res = cmp(cur->tuple, tuple)) == 0)
    cur->count++;
  else if (res < 0)
    cur->left = insert(cur->left, tuple, cmp); 
  else
    cur->right = insert(cur->right, tuple, cmp); 
    
  return cur;
}

/* pop(): returns first tuple in the tree, deleting its node the
   tree. Returns NULL if tree is empty. */
static PnidObj *
pop(Node *cur)
{

}



