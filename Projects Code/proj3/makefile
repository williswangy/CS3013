CFLAGS = -g

mailbox: main.o utility.o 
	gcc $(CFLAGS) -Wall utility.o main.o -lm -o mailbox -pthread
utility.o:utility.c
	gcc $(CFLAGS) -Wall -c utility.c
clean:
	rm -f main.o utility.o 
all: mailbox
