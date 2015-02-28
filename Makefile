CC = gcc

CFLAGS = -Wall
CFLAGS += -Wextra
#CFLAGS += -pedantic
CFLAGS += `pkg-config --cflags sqlite3`

LIBS = `pkg-config --libs sqlite3`

all: backup

backup: init bin/obj/walk.o bin/obj/processor.o bin/obj/path_helper.o bin/obj/backup.o bin/obj/database.o
	$(CC) bin/obj/walk.o bin/obj/processor.o bin/obj/path_helper.o bin/obj/backup.o bin/obj/database.o $(CFLAGS) -o bin/backup $(LIBS)

tests: init
	$(CC) src/test/test_array.c -Wall -Wextra -o bin/test_array

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

bin/obj/database.o: src/database.c
	$(CC) $(CFLAGS) -c -o bin/obj/database.o src/database.c $(LIBS)

clean:
	rm -Rf bin
