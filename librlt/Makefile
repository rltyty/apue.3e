# Makefile for rltapue library.
#
# `make [BUILD=Debug|Release] [SHARED=Y|N]
# `make test [BUILD=Debug|Release] [SHARED=Y|N]

# Environment
ROOT		= ..
PLATFORM	= $(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)
SHARED		= N
VERSION		= 1

# clang/gcc, CFLAGS
ifneq ($(BUILD), Release)
ifneq ($(BUILD), Debug)
$(error Invalid BUILD value: $(BUILD), Release/Debug expected.)
endif
endif
CFLAGS		+= $(CFLAGS.$(BUILD))

# ld
LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -l$(LIBRLTAPUE_NAME)

# static library
LIBRLTAPUE.S	= $(LIBRLTAPUE).a
# dynamic shared library
LIBRLTAPUE.D	= $(LIBRLTAPUE).$(DYNLIB.EXT)

# targets
SRCS		= error.c openmax.c setfl.c profiling.c ft.c
OBJS		= $(SRCS:.c=.o)
TESTBINS	= $(SRCS:.c=)
ifeq ($(SHARED), Y)
LIBTARGET	= $(LIBRLTAPUE.D).$(VERSION)
LDFLAGS        += $(DYNLIB.FLAGS) $(DYNLIB.LOADPATH)$(LIBTARGET)
else
LIBTARGET	= $(LIBRLTAPUE.S)
endif

all:	$(LIBTARGET)

$(LIBRLTAPUE.S): $(OBJS)
	@echo "Creating static library [$@]"
	$(AR) rcs $@ $?

$(LIBRLTAPUE.D).$(VERSION):
	@echo "Creating dynamic shared library [$@]"
	$(CC) $(CFLAGS) -fPIC -c $(SRCS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	ln -sf $@ $(LIBRLTAPUE.D)

test: $(TESTBINS)
	mkdir -p tmp && cp -r data/ tmp/
# Note:
#   When compile with debug information and without optimization,
#   i.e. `-g -O0`, we need to provide complete information for the
#   linker to locate symbols like functions in other source files
#   On macOS, when compile Release version, `LDFLAGS, LDLIBS` could
#   be omitted. It will search necessary dependences like functions
#   for you. On Linux, no matter Release or Debug build, complete
#   information are always need to be provided.

$(TESTBINS): $(LIBTARGET)
	$(CC) -Dtest_$@ $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(TESTBINS) $(LIBRLTAPUE.S) \
		$(LIBRLTAPUE.D) $(LIBRLTAPUE.D).*
	$(RM) -r $(TEMPFILES)

.PHONY: all clean test
