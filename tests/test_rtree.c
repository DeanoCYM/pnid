/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* test_rtree.c - tests for pnid R-tree */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pnid_rtree.h"
#include "pnid_tests.h"

#define RANDOM ((rand() % 100))	/* randomish number between 1-100 */

PnidRtree *tr;
PnidObj   *o;

/* test_new(): unit test for pnid_rtree_new(). */
static void test_new(void)
{
  tr = pnid_rtree_new();
  assert(tr);
}
/* test_insert(): unit test for pnid_rtree_insert(). */
static void test_insert(void)
{
  assert(tr && o); 
  pnid_rtree_insert(tr, o);
}

static void test_delete(void)
{
  assert(tr);
  pnid_rtree_delete(tr, o);
}

int test_rtree(void)
{
  int i;

  fprintf(stderr, "Testing pnid_rtree.o... \n");

  test_new();

  for (i=50; i; i--) {
    o = pnid_obj_new();  
    pnid_obj_set_data(o, RANDOM);
    test_insert();
  }

  pnid_rtree_print(tr);

  putchar('\n');

  test_delete();

  return 0;
}


