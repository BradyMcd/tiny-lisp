
# A teensy little Lisp

Inspired loosely by the Lisp seen [here](www.buildyourownlisp.com/). Mostly it
takes the parsing technology and the nomacular. Notably Q-expressions are the
thing I bring over, denoted by {} Q-expressions are loosly equivalent to LISP
macros in that they stop evaluation of whatever is contained within them.

# Goals

The goal isn't necessarily to create a REPL, but rather to make an easily 
embeddable Lisp interpreter for use in larger programs to do things C normally
wouldn't pull of comfortably.

## Status

### Memory
Currently working, though untested for arbitrarily large loads

### Parser
Reading from strings is currently implemented, I doubt it will change much. 
That said it's not been tested on anything other than very simple valid and
invalid expressions.

Printing the structure resulting from the read step is implemented both to
strings and file pointers

Eval has seen its first pass. I have no way of knowing if it's correct or not
yet and won't until I have better environment management to the point where I
have C function binding implemented and tests built.

### Lval
A work in progress, all manipulations of the lval type which aren't directly
related to managing memory are going through this module. 

Simple Environment management has been started on; C functions can be bound to
any valid symbol name.

In language function binding and variable binding need to be implemented
