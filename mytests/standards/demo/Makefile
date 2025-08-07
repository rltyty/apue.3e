.SECONDEXPANSION:
.PHONY: clean all test init stage
ROOT=../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -l$(LIBRLTAPUE_NAME)
CFLAGS		+= $(CFLAGS.$(BUILD))

PROGS =	test_openmax


all: $(PROGS)

include $(ROOT)/Make.librltapue.inc

test_openmax: my_openmax.o $(LIBRLTAPUE_FILE)

clean:
	$(RM) $(PROGS)
	$(RM) -r $(TEMPFILES)

