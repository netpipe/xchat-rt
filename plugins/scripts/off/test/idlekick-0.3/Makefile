PACKAGE = idlekick
VERSION =0.4
CC = gcc
LIBS = `pkg-config --libs glib-2.0`  
DFLAGS =
CFLAGS = -O -Wall -g -O2 `pkg-config --cflags glib-2.0` -I. -I/usr/local/include -I/public/include -I/usr/include/
LDFLAGS =  $(LIBS) $(DFLAGS) --shared

OBJS = idlekick.o 
SOURCES = $(OBJS:.o=.c)
HEADERS = 
TARGET = idlekick.so

all: idlekick

clean:
	rm -f $(TARGET) *.o

distclean: clean
	rm -rf config.* Makefile tmp *.tar.gz

install: idlekick 
	echo "Load it with \"Xchat->Window->Plugins and Scripts\"";	

idlekick: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

idlekick.o: idlekick.c
