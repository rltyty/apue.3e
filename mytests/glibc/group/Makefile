.SECONDEXPANSION:
.PHONY: clean all test
ROOT=../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -l$(LIBRLTAPUE_NAME)
CFLAGS		+= $(CFLAGS.$(BUILD))

OBJS		= shift3.o
BIN		= weak_alias

all: $(BIN)

$(BIN): $(OBJS)

include $(ROOT)/Make.librltapue.inc

clean:
	$(RM) -r $(TEMPFILES)

