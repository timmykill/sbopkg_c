CC = gcc
CFLAGS = -ansi -pedantic

TARGET = sbopkg

all: sbo.o update.o
	$(CC) $(CFLAGS) sbo.c update.c -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
