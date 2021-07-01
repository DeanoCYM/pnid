# This file is part of pnid
# Copyright (C) 2021 Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
# See COPYING file for licence details

CC=cc
CFLAGS=-Wall -Wfatal-errors -g3 -O0 -DDEBUG -D_GNU_SOURCE
INCLUDE=$(shell pkg-config --cflags gtk4) -I./src
TARGET=pnid
TESTS=pnid_tests
LIBS=$(shell pkg-config --libs gtk4) -lm
OBJ=pnid_app.o pnid_appwin.o pnid_canvas.o pnid_resources.o pnid_draw.o pnid_box.o pnid_obj.o pnid_rtree.o
TESTOBJ=test_rtree.o
APPLICATION_ID=cymru.ert.$(TARGET)
PREFIX=/usr/local

.PHONY: all clean tags tests

all: tags $(TARGET)  

# Data files and source generation
src/pnid_resources.c: data/pnid.gresource.xml data/ui/menu.ui data/valve.png
	glib-compile-resources $< --target=$@ --generate-source

# Any files other than the src that an object depends on 
pnid_obj.o:    src/pnid_obj.h src/pnid_box.h
pnid_box.o:   src/pnid_box.h
pnid_rtree.o:  src/pnid_rtree.h src/pnid_box.h src/pnid_obj.h
pnid_draw.o:   src/pnid_draw.h   
pnid_canvas.o: src/pnid_canvas.h src/pnid_draw.h
pnid_appwin.o: src/pnid_app.h src/pnid_appwin.h src/pnid_canvas.h src/pnid_resources.c
pnid_app.o:    src/pnid_app.h src/pnid_appwin.h src/pnid_resources.c 
main.o:        src/pnid_app.h
%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)

# Target executable generation
$(TARGET): main.o $(OBJ) 
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $< -o $@ $(LIBS)

# Testing
tests: $(OBJ) $(TESTS) tests/pnid_tests.h
	tests/memcheck.sh ./pnid_tests
test_%.o: tests/test_%.c pnid_%.o
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)
$(TESTS): tests/test_main.o $(TESTOBJ)
	$(CC) $(CFnLAGS) $(INCLUDE) $(TESTOBJ) $(OBJ) $< -o $@ $(LIBS)

# Utilities
clean:
	rm -f vgdump
	rm -f src/TAGS
	rm -f src/pnid_resources.c
	rm -f $(TESTOBJ)
	rm -f $(TESTS)
	rm -f $(OBJ)
	rm -f {test_,}main.o
	rm -f $(TARGET)
tags:
	@etags src/*.c src/*.h --output=src/TAGS
