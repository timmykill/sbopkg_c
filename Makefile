CC = gcc
CFLAGS = -pedantic -I/usr/local/Cellar/curl/7.64.1/include
LINK = -L/usr/local/Cellar/curl/7.64.1/lib -lcurl -lldap -lz
TARGET = sbopkg
objects = sbo.o update.o entities.o search_install.o

all: $(objects)
	$(CC) $(CFLAGS) $(LINK) $(objects) -o $(TARGET)

getinmemory: getinmemory.o
	$(CC) $(CFLAGS) $(LINK) getinmemory.o -o getinmemory

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o repo.bin
