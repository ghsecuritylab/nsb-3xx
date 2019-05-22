VERSION=0.8

# optional features
PLUGIN_SUPPORT=1
DEBUG=0
DEBUG_TYPECAST=0

include Makefile.inc

LIBS=-lc -ldl
SHLIB_FILE=libuci.$(SHLIB_EXT).$(VERSION)

define add_feature
	@echo "$(if $(findstring 1,$($(1))),#define UCI_$(1) 1,#undef UCI_$(1))" >> $@.tmp
endef

define add_dep
$(1).shared.o: $(2)
$(1).static.o: $(2)
endef

SOURCES = libuci.c file.c ucimap.c util.c delta.c

all: uci libuci.$(SHLIB_EXT) uci-static ucimap-example

$(eval $(call add_dep,libuci,list.c uci.h uci_config.h uci_internal.h))
$(eval $(call add_dep,ucimap,uci.h uci_config.h ucimap.h))

cli.o: cli.c uci.h uci_config.h

uci_config.h: FORCE
	@rm -f "$@.tmp"
	@echo "#define UCI_PREFIX \"$(prefix)\"" > "$@.tmp"
	$(call add_feature,PLUGIN_SUPPORT)
	$(call add_feature,DEBUG)
	$(call add_feature,DEBUG_TYPECAST)
	@if [ \! -f "$@" ] || ! cmp "$@.tmp" "$@" >/dev/null; then \
		mv "$@.tmp" "$@"; \
	else \
		rm -f "$@.tmp"; \
	fi

%.o: %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

%.static.o: %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

%.shared.o: %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(FPIC) $<

uci: cli.o libuci.$(SHLIB_EXT)
	$(CC) -o $@ $< -L. -luci $(LIBS)

uci-static: cli.o libuci.a
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

ucimap.c: ucimap.h uci.h

libuci.a: $(patsubst %.c,%.static.o, $(SOURCES))
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@

libuci.$(SHLIB_EXT): $(patsubst %.c,%.shared.o, $(SOURCES))
	$(LINK) $(SHLIB_FLAGS) -o $(SHLIB_FILE) $^ $(LIBS)
	ln -sf $(SHLIB_FILE) $@

ucimap-example.c: list.h
ucimap-example: ucimap-example.o libuci.a
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f uci uci-static *.[oa] *.so* *.dylib* uci_config.h ucimap-example

install: install-bin install-dev

install-dev: all
	$(MKDIR) -p $(DESTDIR)$(prefix)/$(LIBDIRNAME)
	$(MKDIR) -p $(DESTDIR)$(prefix)/include
	$(INSTALL) -m0644 libuci.a $(DESTDIR)$(prefix)/$(LIBDIRNAME)/
	$(INSTALL) -m0644 uci_config.h uci.h ucimap.h $(DESTDIR)$(prefix)/include/

install-bin: all
	$(MKDIR) -p $(DESTDIR)$(prefix)/$(LIBDIRNAME)
	$(INSTALL) -m0755 $(SHLIB_FILE) $(DESTDIR)$(prefix)/$(LIBDIRNAME)/
	ln -sf $(SHLIB_FILE) $(DESTDIR)$(prefix)/$(LIBDIRNAME)/libuci.$(SHLIB_EXT)
	$(MKDIR) -p $(DESTDIR)$(prefix)/bin
	$(INSTALL) -m0755 uci $(DESTDIR)$(prefix)/bin/

test: all ucimap-example
	make -C test

FORCE: ;
.PHONY: FORCE
