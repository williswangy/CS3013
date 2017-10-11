CS3013 Project 3 Yuan Wang

REFERENCE:
1. After discussed with classmates, we found that it is much more easier to put the values in a file(test.txt) and then parse it through the command line.
2. About the additional work, we believe that using the stack structure is the best way to store data and wait until it found a link.
3.Some basic ideas are shared with classmates.

HOW TO RUN?

In the corresponding dictionary, open in terminal, type make. And type ./mailbox VALUE[THEARDS] INPUTFILE NB[OPTION].
The corresponding process record is stored in script.txt

Program structure:

main.c： handling - parsing from the input file main function.
utility.c : a list of functions to help build the main thread, child threads, mailbox, stackNode for the nb section. And the corresponding functions.