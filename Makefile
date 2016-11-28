CC = clang
FLAGS = -Wall -Wextra -g
OBJECTS = memory.o parser.o lval.o environment.o
SRC = $(OBJECTS:.o=.c)
CDIR = $(shell pwd)
INCLUDE = -I$(CDIR)

DEP = mpc/mpc.o

all: #Purposefully blank until I make this an actual embeddable module

$(CDIR)/bin/:
	mkdir bin

bin/add_test: $(CDIR)/tests/add_test.c $(OBJECTS) $(DEP)
	$(CC) $(FLAGS) $(OBJECTS) $(DEP) $< -o $@ $(INCLUDE)

bin/read_test: $(CDIR)/tests/read_test.c $(OBJECTS) $(DEP)
	$(CC) $(FLAGS) $(OBJECTS) $(DEP) $< -o $@ $(INCLUDE)

tests: $(CDIR)/bin/ bin/read_test bin/add_test

$(DEP):
	$(CC) $(FLAGS) -c mpc/mpc.c -o $@

%.o:%.c
	$(CC) $(FLAGS) -c $< -o $@ $(INCLUDE)

.PHONY: clean done

clean:
	rm -f $(OBJECTS)

done: clean
	rm -rf bin *~
