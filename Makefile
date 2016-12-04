CC = clang
FLAGS = -Wall -Wextra -g
OBJECTS = memory.o parser.o lval.o environment.o
SRC = $(OBJECTS:.o=.c)
CDIR = $(shell pwd)
INCLUDE = -I$(CDIR)

DEP = mpc/mpc.o

all: #Purposefully blank until I make this an actual embeddable module

$(CDIR)/bin/:
	echo binary directory not found, adding ./bin/
	mkdir bin

bin/%:tests/%.c $(OBJECTS) $(DEP)
	echo Building test: $@ ...
	$(CC) $(FLAGS) $(OBJECTS) $(DEP) $< -o $@ $(INCLUDE)
	echo  Done

tests: $(CDIR)/bin/ bin/read_test bin/add_test
	echo All tests built

$(DEP):
	echo Building dependency: $@ ...
	$(CC) $(FLAGS) -c mpc/mpc.c -o $@
	echo  Done

%.o:%.c
	echo Building module: $@ ...
	$(CC) $(FLAGS) -c $< -o $@ $(INCLUDE)
	echo  Done

.PHONY: clean done

clean:
	rm -f $(OBJECTS)

done: clean
	rm -rf bin *~
