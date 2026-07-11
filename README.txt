HW 2
Conner McKelvy 
mckelvyc

Files:
    mckelvyc_HW02.c
    Makefile
    README.txt

To compile:
    make

To run:
    ./homework [-S] [-s size] [-f pattern depth] [-t f|d] [directory]

Examples:
    ./homework
    ./homework ../programs
    ./homework -S
    ./homework -s 1024
    ./homework -f jpg 1
    ./homework -t d

Notes:
    - Uses function pointers: a printer function pointer picks how each
      entry is displayed (plain, or with -S details), and predicate
      function pointers check the -s/-f/-t filters. All active filters
      must pass for an entry to be listed.
    - Symbolic links are shown as "name (target)".

Testing:
    Tested with the sample projects/ folder from the assignment (all
    options and combinations produced correct output), and with a small
    test folder containing a symlink to check the -f, -s, and -t
    filters and combinations of them.