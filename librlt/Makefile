# Makefile for rltapue library.
#
# `make` or `make BUILD=Debug` to build library in Release or Debug mode.
# `make test` or `make test BUILD=Debug` to build unit test executables in
# either mode.

ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LIBMISC		= librltapue.a
OBJS		= error.o openmax.o
TESTBINS	= error openmax
LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue

CFLAGS		+= $(CFLAGS.$(BUILD))	# BUILD: Release(default), Debug

.PHONY: all clean test

all:	$(LIBMISC)

$(LIBMISC):	$(OBJS)
	$(AR) rcs $(LIBMISC) $?

test: $(TESTBINS)

# Note:
#   When compile with debug information and without optimization,
#   i.e. `-g -O0`, we need to provide complete information for the
#   linker to locate symbols like functions in other source files

$(TESTBINS): $(LIBMISC)
	@if [ "$(BUILD)" = "Release" ]; \
	then $(CC) -Dtest_$@ $(CFLAGS) $@.c -o $@ ; \
	elif [ $(BUILD) = "Debug" ]; \
	then $(CC) -Dtest_$@ $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS) ; \
	else printf "Wrong build mode: %s." "$(BUILD)" ; \
	fi

clean:
	$(RM) $(LIBMISC) $(TESTBINS)
	$(RM) -r $(TEMPFILES)

include $(ROOT)/Make.librltapue.inc
