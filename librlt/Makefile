# Makefile for rltapue library.
#
ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue
LIBMISC		= librltapue.a
OBJS		= stub.o error.o openmax.o
TESTBINS	= error openmax

.PHONY: all clean test
all:	$(LIBMISC)

$(LIBMISC):	$(OBJS)
	$(AR) rv $(LIBMISC) $?
	$(RANLIB) $(LIBMISC)

test: $(TESTBINS)

$(TESTBINS):
	$(CC) -Dtest_$@ $(CFLAGS.$(BUILD)) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o a.out core temp.* $(LIBMISC) $(TESTBINS)

include $(ROOT)/Make.librltapue.inc
