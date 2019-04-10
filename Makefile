CC = gcc
CFLAGS = -pedantic

TARGET = sbopkg

objects = sbo.o update.o entities.o search_install.o

all: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o repo.bin
