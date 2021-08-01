/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* tests.h - shared testing utilites */

#ifndef __PNID_TESTS_H
#define __PNID_TESTS_H

/* RAND100: pseudorandomish number between 1-100 */
#define RAND100 ((rand() % 100)) 

void test_rtree (void);
void test_bst   (void);

#endif /* __PNID_TESTS_H */
