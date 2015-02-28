CC = gcc

CFLAGS = -Wall
CFLAGS += -Wextra
#CFLAGS += -pedantic

#LIBS = ?

all: backup

backup: init bin/obj/walk.o bin/obj/processor.o
	$(CC) bin/obj/walk.o bin/obj/processor.o $(CFLAGS) -o bin/backup $(LIBS)

init:
	mkdir -p bin
	mkdir -p bin/obj

bin/obj/walk.o: src/walk.c
	$(CC) $(CFLAGS) -c -o bin/obj/walk.o src/walk.c $(LIBS)

bin/obj/processor.o: src/processor.c
	$(CC) $(CFLAGS) -c -o bin/obj/processor.o src/processor.c $(LIBS)

clean:
	rm -Rf bin
