CFLAGS=-std=gnu99 -Wall

all: parent child

parent: parent.o io_helper.o

child: child.o io_helper.o

parent.o: parent.c io_helper.h

child.o: child.c io_helper.h

io_helper.o: io_helper.c

clean:
	$(RM) parent
	$(RM) parent.o
	$(RM) child
	$(RM) child.o
	$(RM) io_helper.o
