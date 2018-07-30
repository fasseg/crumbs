CC=gcc
CFLAGS=-I. -g -Werror -Wall
DEPS=crumbs.h
TARGET=target
BUILD=build
DOC=doc
MAN=/usr/local/man/man1/crumbs.1

%.o: %.c $(DEPS) directories
	$(CC) -g -c -o $(BUILD)/$@ $< $(CFLAGS)

all: crumbs.o 
	$(CC) $(CFLAGS) -o $(TARGET)/crumbs $(BUILD)/crumbs.o
	gzip -c $(DOC)/crumbs.man >> $(TARGET)/crumbs.1

directories: 
	mkdir -p $(TARGET)
	mkdir -p $(BUILD)

clean:
	rm -Rf $(BUILD)
	rm -Rf $(TARGET)

install:
	install -m 755 -o root $(TARGET)/crumbs /usr/local/bin/
	install -m 755 -o root crumbs.conf /etc/crumbs.conf
	install -m 644 -o root $(TARGET)/crumbs.1 $(MAN)
