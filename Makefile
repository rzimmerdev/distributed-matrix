CC=mpicc

EXE=main

N ?= 3
S ?= 42
T ?= 3

HOSTFILE=hostfile.txt

CFLAGS=-Wall

SRC=main.c

OBJ=$(SRC:.c=.o)

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: $(EXE)
	mpirun -np $(T) --hostfile $(HOSTFILE) ./$< $(N) $(S)

clean:
	rm -f $(EXE) $(OBJ)

.PHONY: all clean run

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<
