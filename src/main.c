/* This file is part of pnid
   Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
   See COPYING file for licence details */

/* main.c - instantiate and run the pnid application */

#include <gtk/gtk.h>

#include "pnid_app.h"

int main
(int argc, char **argv)
{
    return g_application_run(G_APPLICATION(pnid_app_new()), argc, argv);
}

