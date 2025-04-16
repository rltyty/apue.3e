.SECONDEXPANSION:
.PHONY: clean all test
ROOT=../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue
CFLAGS		+= $(CFLAGS.$(BUILD))


OBJS		= stub.o
all: $(LIBRLTAPUE) $(OBJS)

clean:
	$(RM) -r $(TEMPFILES)

include $(ROOT)/Make.librltapue.inc
