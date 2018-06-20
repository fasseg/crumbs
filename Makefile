CC=gcc
CFLAGS=-I. -g
DEPS=crumbs.h
TARGET=target
BUILD=build

%.o: %.c $(DEPS) directories
	$(CC) -g -c -o $(BUILD)/$@ $< $(CFLAGS)

all: crumbs.o 
	$(CC) $(CFLAGS) -o $(TARGET)/crumbs $(BUILD)/crumbs.o

directories: 
	mkdir -p $(TARGET)
	mkdir -p $(BUILD)

clean:
	rm -Rf $(BUILD)
	rm -Rf $(TARGET)

install:
	install -m 755 -o root crumbs /usr/local/bin/
