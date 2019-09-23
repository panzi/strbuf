CC=gcc
AR=ar
BINEXT=
SOEXT=.so
BUILDDIR=build
BUILDDIR_BIN=$(BUILDDIR)/$(TARGET)
INCLUDE=-Isrc
TARGET=$(shell uname|tr '[A-Z]' '[a-z]')$(shell getconf LONG_BIT)
COMMON_CFLAGS=-Wall -Werror -Wextra -std=gnu11 $(INCLUDE)
ifeq ($(DEBUG),ON)
	COMMON_CFLAGS+=-g
else
	COMMON_CFLAGS+=-O2 -DNDEBUG
endif
POSIX_CFLAGS=$(COMMON_CFLAGS) -pedantic -fdiagnostics-color
CFLAGS=$(COMMON_CFLAGS)
ARCH_FLAGS=

EXAMPLES=$(BUILDDIR_BIN)/shared/bin/example1$(BINEXT) \
         $(BUILDDIR_BIN)/static/bin/example1$(BINEXT) \
         $(BUILDDIR_BIN)/shared/bin/example2$(BINEXT) \
         $(BUILDDIR_BIN)/static/bin/example2$(BINEXT)

WINDOWS=0
ifeq ($(TARGET),win32)
	CC=i686-w64-mingw32-gcc
	WINDRES=i686-w64-mingw32-windres
	ARCH_FLAGS=-m32
	WINDOWS=1
else
ifeq ($(TARGET),win64)
	CC=x86_64-w64-mingw32-gcc
	WINDRES=x86_64-w64-mingw32-windres
	ARCH_FLAGS=-m64
	WINDOWS=1
else
ifeq ($(TARGET),linux32)
	CFLAGS=$(POSIX_CFLAGS)
	BINEXT=-linux-i686
	ARCH_FLAGS=-m32
else
ifeq ($(TARGET),linux64)
	CFLAGS=$(POSIX_CFLAGS)
	ARCH_FLAGS=-m64
else
ifeq ($(TARGET),darwin32)
	CC=clang
	CFLAGS=$(POSIX_CFLAGS)
	ARCH_FLAGS=-m32
else
ifeq ($(TARGET),darwin64)
	CC=clang
	CFLAGS=$(POSIX_CFLAGS)
	ARCH_FLAGS=-m64
endif
endif
endif
endif
endif
endif

ifeq ($(WINDOWS),1)
	BINEXT=.exe
	SOEXT=.dll
endif

.PHONY: all clean examples shared static setup run-static-example1 run-shared-example1

# keep intermediary files:
.SECONDARY:

all: shared static examples

examples: $(EXAMPLES)

run-shared-example%: $(BUILDDIR_BIN)/shared/bin/example%$(BINEXT)
	LD_LIBRARY_PATH=$(BUILDDIR_BIN)/shared/lib $<

run-static-example%: $(BUILDDIR_BIN)/static/bin/example%$(BINEXT)
	$<

shared: $(BUILDDIR_BIN)/shared/lib/libstrbuf$(SOEXT) $(BUILDDIR_BIN)/shared/lib/libsstrbuf$(SOEXT)

static: $(BUILDDIR_BIN)/static/lib/libstrbuf.a $(BUILDDIR_BIN)/static/lib/libsstrbuf.a

setup:
	mkdir -p $(BUILDDIR_BIN)/shared/lib \
	         $(BUILDDIR_BIN)/shared/obj \
	         $(BUILDDIR_BIN)/shared/bin \
	         $(BUILDDIR_BIN)/static/lib \
	         $(BUILDDIR_BIN)/static/obj \
	         $(BUILDDIR_BIN)/static/bin \
	         $(BUILDDIR_BIN)/examples/obj

clean:
	rm $(BUILDDIR_BIN)/shared/lib/*$(SOEXT) \
	   $(BUILDDIR_BIN)/shared/obj/*.o \
	   $(BUILDDIR_BIN)/shared/bin/*$(BINEXT) \
	   $(BUILDDIR_BIN)/static/lib/*.a \
	   $(BUILDDIR_BIN)/static/obj/*.o \
	   $(BUILDDIR_BIN)/static/bin/*$(BINEXT) \
	   $(BUILDDIR_BIN)/examples/obj/*.o

$(BUILDDIR_BIN)/shared/bin/%$(BINEXT): $(BUILDDIR_BIN)/examples/obj/%.o $(BUILDDIR_BIN)/shared/lib/libstrbuf$(SOEXT) $(BUILDDIR_BIN)/shared/lib/libsstrbuf$(SOEXT)
	$(CC) $(ARCH_FLAGS) $(CFLAGS) -L$(BUILDDIR_BIN)/shared/lib $< -o $@ -lstrbuf -lsstrbuf

$(BUILDDIR_BIN)/static/bin/%$(BINEXT): $(BUILDDIR_BIN)/examples/obj/%.o $(BUILDDIR_BIN)/static/lib/libstrbuf.a $(BUILDDIR_BIN)/static/lib/libsstrbuf.a
	$(CC) $(ARCH_FLAGS) $(CFLAGS) -L$(BUILDDIR_BIN)/static/lib $< -o $@ -static -lstrbuf -lsstrbuf

$(BUILDDIR_BIN)/shared/lib/lib%$(SOEXT): $(BUILDDIR_BIN)/shared/obj/%.o
	$(CC) $(ARCH_FLAGS) $(CFLAGS) $< -o $@ -fPIC -shared

$(BUILDDIR_BIN)/static/lib/lib%.a: $(BUILDDIR_BIN)/static/obj/%.o
	$(AR) rcs $@ $<

$(BUILDDIR_BIN)/shared/obj/%.o: src/%.c src/%.h
	$(CC) $(ARCH_FLAGS) $(CFLAGS) -DWIN_EXPORT $< -o $@ -c -fPIC

$(BUILDDIR_BIN)/static/obj/%.o: src/%.c src/%.h
	$(CC) $(ARCH_FLAGS) $(CFLAGS) -DWIN_EXPORT $< -o $@ -c

$(BUILDDIR_BIN)/examples/obj/%.o: examples/%.c examples/example.h
	$(CC) $(ARCH_FLAGS) $(CFLAGS) $< -o $@ -c