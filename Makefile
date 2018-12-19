V=1.1.3

ifneq "$(VDEVEL)" ""
V=$(VDEVEL)
endif

CC := $(CC) -std=c99

base_CFLAGS = -Wall -Wextra -pedantic -O2
base_LIBS = -lm

pkgs = vte-2.91 glib-2.0
pkgs_CFLAGS = $(shell pkg-config --cflags $(pkgs))
pkgs_LIBS = $(shell pkg-config --libs $(pkgs))

CPPFLAGS += -DMINITERM_VERSION=\"$(V)\"
CFLAGS := $(base_CFLAGS) $(pkgs_CFLAGS) $(CFLAGS)
LDLIBS := $(base_LIBS) $(pkgs_LIBS)

all: miniterm

miniterm: miniterm.o settings.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDLIBS) miniterm.o settings.o

miniterm.o: miniterm.c config.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(LDLIBS) miniterm.c -o miniterm.o

settings.o: settings.h settings.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(LDLIBS) settings.c -o settings.o

clean:
	$(RM) miniterm miniterm.o settings.o

install: miniterm
	install -Dm755 miniterm $(DESTDIR)/usr/bin/miniterm
	install -Dm755 miniterm.desktop $(DESTDIR)/usr/share/applications/miniterm.desktop
