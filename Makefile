CC      = g++
CFLAGS  = -c

OBJS = grabber.o
TARGET = grabber
DESTDIR ?= /usr/local/bin/

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

install: all
	install -m 755 $(TARGET) $(DESTDIR) 

clean:
	rm -f $(TARGET) $(OBJS) core* *~ *.d
