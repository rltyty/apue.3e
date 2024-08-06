.SECONDEXPANSION:
.PHONY: clean all test
ROOT=../../../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

all: $(LIBAPUE) setfl.o
	mkdir -p ../../../tmp && cp -r ../../../data/fileio ../../../tmp/

clean:
	$(RM) -r ../../../tmp/fileio
