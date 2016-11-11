
# A teensy little Lisp

Inspired by the Lisp seen [here](www.buildyourownlisp.com/). Rather than the 
top-down type approach taken in the source material my Lisp is being built 
bottom up which means the memory module was the first thing to be designed. 

# Goals

The goal isn't necessarily to create a REPL, but rather to make an easily 
embeddable Lisp interpreter for use in larger programs to do things C normally
wouldn't pull of comfortably.

## Status

### Memory
Currently working, though untested for arbitrarily large loads

Environment management has been started on, but is incomplete as of yet

### Parser
Reading from strings is currently implemented, I doubt it will change much. 
That said it's not been tested on anything other than very simple valid and
invalid expressions.

Printing the structure resulting from the read step is implemented both to
strings and file pointers
