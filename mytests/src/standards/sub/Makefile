.SECONDEXPANSION:
.PHONY: clean all test init stage
ROOT=../../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue
CFLAGS		+= $(CFLAGS.$(BUILD))

PROGS =	test_openmax

all: $(LIBRLTAPUE) $(PROGS)

test_openmax: my_openmax.o

clean:
	$(RM) $(PROGS)
	$(RM) -r $(TEMPFILES)

include $(ROOT)/Make.librltapue.inc
