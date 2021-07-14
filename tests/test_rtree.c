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

int test_rtree(void)
{
  PnidRtree *db;
  PnidObj   *o; 
  int i, n;

  o = pnid_obj_new();  
  
  printf("\n========== Pnid R-tree ==========\n");
  printf("New R-tree (M=%d, m=%d)... ",
	 pnid_rtree_get_max(), pnid_rtree_get_min());
  db = pnid_rtree_new();
  printf("%s\n", db!=NULL ? "PASS." : "FAIL!");
  assert(db != NULL);

  for (i = 0; i < 10; ++i) {
    pnid_obj_set_data(o, RANDOM);
    pnid_rtree_insert(db, o);
    n = pnid_rtree_print(db);
    printf("%d objects inserted, %d objects read... %s\n\n",
	   i+1, n, i+1==n ? "PASS." : "FAIL!");
    assert(i+1 == n);
  }

  return 0;
}


