CC=gcc
CFLAGS=-I. -g -Werror -Wall
DEPS=crumbs.h
TARGET=target
BUILD=build
DOC=doc
MANPATH=/usr/share/man/man1

%.o: %.c $(DEPS) directories
	$(CC) -g -c -o $(BUILD)/$@ $< $(CFLAGS)

all: crumbs.o 
	$(CC) $(CFLAGS) -o $(TARGET)/crumbs $(BUILD)/crumbs.o
	gzip -c $(DOC)/crumbs.man > $(TARGET)/crumbs.1

directories: 
	mkdir -p $(TARGET)
	mkdir -p $(BUILD)

clean:
	rm -Rf $(BUILD)
	rm -Rf $(TARGET)

install:
	install -m 755 -o root $(TARGET)/crumbs /usr/local/bin/
	install -m 755 -o root crumbs.conf /etc/crumbs.conf
	@[[ -d $(MANPATH) ]] && install -m 644 -o root $(TARGET)/crumbs.1 $(MANPATH)/crumbs.1 || echo -e "\nWARN: Man page directory does not exists at $(MANPATH). But you can copy the file target/crumbs.1 to the man page directory yourself"
