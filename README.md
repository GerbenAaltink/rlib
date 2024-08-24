# RLIB

Collection of frequently used C functions by me. It contains an advanced benchmark system, an alternative to printf supporting printing of time between messages for e.g. benchmarking and colors, Super fast tree map (much faster than hash table), Stdout redirection, progress bar. multi purpose lexer, custom malloc for counting allocations and free's, simple test library that works like assert that checks memory, creates summary and provides exit code, serveral time functions supporting nano seconds, math functions for if not available by std, arena blazing fast memory.

## ENVIRONMENT VARIABLES

###  Disabling color
Set env RDISABLE_COLORS = 1 to disable color on stdout. stderr will still have colors for gui.




/** 
 * NOTES:
 * 
 * How te dump assembly of an application:
 * objdump -d rlib.so
 * 
 * How to see the signatures of library:
 * nm -D rlib.so
 * 
 * 
 * 
 */