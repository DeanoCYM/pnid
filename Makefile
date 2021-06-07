# This file is part of pnid
# Copyright (C)  Ellis Rhys Thomas <e.rhys.thomas@gmail.com>
# See COPYING file for licence details

CC=cc
CFLAGS=-Wall -Wextra -Wfatal-errors -g3 -DDEBUG
INCLUDE=-I./src
TARGET=pnid
LIBS=
OBJ=
PREFIX=/usr/local

.PHONY: all clean tags build install uninstall

all: $(TARGET)
$(TARGET): $(TARGET).o $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ $(LIBS)
%.o: ./src/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJ)
tags:
	@etags src/*.c src/*.h

build: CFLAGS:=-Wall -Wextra -Werror -Wfatal-errors -DNDEBUG
build: clean all
install: build
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin
uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
