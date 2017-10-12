CFLAGS = -g

proj4: main.o utility.o thread.o
	gcc $(CFLAGS) -Wall main.o utility.o thread.o  -lm -o proj4 -pthread
main.o:main.c
	gcc $(CFLAGS) -Wall -c main.c
utility.o:utility.c
	gcc $(CFLAGS) -Wall -c utility.c
routines.o:thread.c
	gcc $(CFALGS) -Wall -c thread.c
clean:
	rm -f main.o utility.o thread.o

all: proj4
