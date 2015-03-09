# Copyright (C) 2015 NIPE-SYSTEMS
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc

CFLAGS = -Wall
CFLAGS += -Wextra
#CFLAGS += -pedantic
CFLAGS += `pkg-config --cflags libarchive`

LIBS  = `pkg-config --libs libarchive`

.PHONY: all backup tests init clean

all: backup

backup: init bin/obj/walk.o bin/obj/processor.o bin/obj/path_helper.o bin/obj/backup.o bin/obj/archive.o bin/obj/index.o bin/obj/arguments.o
	$(CC) bin/obj/walk.o bin/obj/processor.o bin/obj/path_helper.o bin/obj/backup.o bin/obj/archive.o bin/obj/index.o bin/obj/arguments.o $(CFLAGS) -o bin/backup $(LIBS)

tests: init
	$(CC) src/test/test_array.c -Wall -Wextra -o bin/test_array
	$(CC) src/test/test_if.c -Wall -Wextra -o bin/test_if
	$(CC) src/test/test_libarchive.c -Wall -Wextra `pkg-config --cflags libarchive` -o bin/test_libarchive `pkg-config --libs libarchive`

init:
	mkdir -p bin
	mkdir -p bin/obj

bin/obj/backup.o: src/backup.c
	$(CC) $(CFLAGS) -c -o bin/obj/backup.o src/backup.c $(LIBS)

bin/obj/walk.o: src/walk.c
	$(CC) $(CFLAGS) -c -o bin/obj/walk.o src/walk.c $(LIBS)

bin/obj/processor.o: src/processor.c
	$(CC) $(CFLAGS) -c -o bin/obj/processor.o src/processor.c $(LIBS)

bin/obj/path_helper.o: src/path_helper.c
	$(CC) $(CFLAGS) -c -o bin/obj/path_helper.o src/path_helper.c $(LIBS)

bin/obj/archive.o: src/archive.c
	$(CC) $(CFLAGS) -c -o bin/obj/archive.o src/archive.c $(LIBS)

bin/obj/index.o: src/index.c
	$(CC) $(CFLAGS) -c -o bin/obj/index.o src/index.c $(LIBS)

bin/obj/arguments.o: src/arguments.c
	$(CC) $(CFLAGS) -c -o bin/obj/arguments.o src/arguments.c $(LIBS)

clean:
	rm -Rf bin
