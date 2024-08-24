.SECONDEXPANSION:
.PHONY: clean all test
ROOT=../../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue

OBJS		= setfl.o
all: $(LIBRLTAPUE) $(OBJS)

clean:
	$(RM) -f $(OBJS)

include $(ROOT)/Make.librltapue.inc
