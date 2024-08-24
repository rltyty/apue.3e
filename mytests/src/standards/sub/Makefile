.SECONDEXPANSION:
.PHONY: clean all test init stage
ROOT=../../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue

PROGS =	test_openmax

all: $(LIBRLTAPUE) $(PROGS)

test_openmax: my_openmax.o

clean:
	$(RM) $(PROGS) $(TEMPFILES)
	$(RM) -r *.dSYM

include $(ROOT)/Make.librltapue.inc
