.SECONDEXPANSION:
.PHONY: clean all test init stage
ROOT=../../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

CFLAGS := $(CFLAGS.$(BUILD))

PROGS =	test_openmax

all: $(LIBAPUE) $(PROGS)

test_openmax: my_openmax.o

clean:
	$(RM) $(PROGS) $(TEMPFILES)
	$(RM) -r *.dSYM

include $(ROOT)/Make.libapue.inc
