/* This file is part of pnid
   Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* test_main.c - pnid testing */

#include "pnid_tests.h"

int main(void)
{
    test_rtree();

    puts("All testing passed successfully.");
    return 0;
}
