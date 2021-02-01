CC = gcc
CFLAGS = -Wall
TARGET = pjail
PREFIX=/usr/local

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)

install:
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin
