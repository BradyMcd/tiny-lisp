
# A teensy little Lisp

Inspired loosely by the Lisp seen [here](www.buildyourownlisp.com/). Mostly it
takes the parsing technology and the nomenclature. Notably Q-expressions are 
the thing I bring over, denoted by {} Q-expressions are loosly equivalent to 
LISP macros in that they stop evaluation of whatever is contained within them.

To build tests I suggest 'make -s tests'

# Goals

The goal isn't necessarily to create a REPL (I might implement one though), 
but rather to make an easily embeddable Lisp interpreter for use in larger 
programs to do things C normally wouldn't pull of comfortably (Read: AI and
metaprogramming and Greenspun's 10th)

## Status, By Module

### Memory
Currently working, though untested for arbitrarily large loads
Needs to be decoupled from lval modules in a big way. Currently the overlap
in function between lval manipulation and memory manipulation is causing
blockers in progress to the evaluator and environment.

### Parser
Reading from strings is currently implemented, I doubt it will change much. 
That said it's not been tested on anything other than very simple valid and
invalid expressions.

Printing the structure resulting from the read step is implemented both to
strings and file pointers

### Lval
A work in progress, all manipulations of the lval type which aren't directly
related to managing memory are going through this module. 

### Environment
Simple Environment management has been started on; C functions can be bound to
any valid symbol name.

Environments can now have parent/child relationships. 

In language function binding and variable binding need to be implemented
