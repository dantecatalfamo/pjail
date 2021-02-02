CFLAGS = -O2 -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wshadow -pedantic
TARGET = pjail
PREFIX=/usr/local

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
