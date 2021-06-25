#!/bin/bash

# This file is part of pnid
# Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
# See COPYING file for licence details

# memcheck.sh - memory check with valgrind, run from project root.

#export G_SLICE=debug-blocks

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=definite \
    --leak-resolution=high \
    --num-callers=20 \
    --log-file=vgdump \
    --suppressions="/usr/share/glib-2.0/valgrind/glib.supp" \
    --suppressions="/usr/share/gtk-4.0/valgrind/gtk.supp"   \
    "$@"






