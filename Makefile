CC = gcc
CFLAGS = -g -pedantic -I/usr/local/Cellar/curl/7.64.1/include
LINK = -L/usr/local/Cellar/curl/7.64.1/lib -lcurl -lldap -lz
TARGET = sbopkg_c
objects = sbo.o update.o entities.o search_install.o curl_helper.o

all: $(objects)
	$(CC) $(CFLAGS) $(LINK) $(objects) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	mkdir -p $(DESTDIR)/usr/bin/
	cp ./sbopkg_c $(DESTDIR)/usr/bin/sbopkg_c

clean:
	rm -f *.o
