# This file is part of pnid
# Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
# See COPYING file for licence details

CC=cc
CFLAGS=-Wall -Wfatal-errors -g3 -DDEBUG
INCLUDE=$(shell pkg-config --cflags gtk4) -I./src
TARGET=pnid
LIBS=$(shell pkg-config --libs gtk4)
OBJ=main.o pnid_app.o pnid_appwin.o
APPLICATION_ID=cymru.ert.$(TARGET)
PREFIX=/usr/local

.PHONY: all clean tags

all: $(TARGET) tags

# Generation of an object file depends on: its respective src file any
# local header files included, and any data files referenced, in that
# source file.
pnid_appwin.o: src/pnid_app.h src/pnid_appwin.h
pnid_app.o: src/pnid_app.h src/pnid_appwin.h
main.o: src/pnid_app.h
%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)

# Target executable generation
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) -o $@ $(LIBS)

# Utilities
clean:
	rm -f src/TAGS
	rm -f $(OBJ)
	rm -f $(TARGET)
tags:
	@etags src/*.c src/*.h --output=src/TAGS

