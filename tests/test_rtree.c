/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* test_rtree.c - tests for pnid R-tree */

#include <stdio.h>
#include <assert.h>

#include "pnid_rtree.h"

#include "pnid_tests.h"

int test_rtree(void)
{
    printf("Creating new R-tree... ");
    PnidRtree *db = pnid_rtree_new();
    assert(db != NULL);
    printf("OK.\n");

    return 0;
}


