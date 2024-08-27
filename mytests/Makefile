# Makefile for mytests.
#
# make [BUILD=Debug|Release] [SHARED=Y|N]
# NOTE: `SHARED=Y` means build tests against dynamic shared librlapue

.SECONDEXPANSION:
ROOT = ..
PLATFORM = $(shell $(ROOT)/systype.sh)

include $(ROOT)/Make.defines.$(PLATFORM)
include ./exclude.inc

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue
CFLAGS		+= $(CFLAGS.$(BUILD))

# add "-E " option prefix of `fd` to each excluded dir defined in exclude.inc
FD_EXCLUDE_DIRS := $(addprefix -E , $(EXCLUDE_DIRS))

# find all c sources using `fd` under "src/"
SRCS := $(shell fd -e c $(FD_EXCLUDE_DIRS) . src)
# NOTE: = is recursive expansion, meaning
# a = $(b) b = $(c) c = d, then $(a) reflects d.
# So = cannot be used to self-assign like cflags = $(cflags) -g3, use :=
# instead, cflags := $(cflags) -g3
SRCS := $(patsubst src/%.c, %.c, $(SRCS))
SRCS := $(filter-out $(EXCLUDE), $(SRCS))
BINS := $(patsubst %.c, %, $(SRCS))
BINS := $(addprefix $(BUILD)/, $(BINS))

# sub make
SUB = sub

all: msg0 cpdata $(BUILD) msg1

msg0:
	@printf "Start building [32m[%s][m\n" $(BUILD)
	@printf "Platform:[32m[%s][m\n" $(PLATFORM)
	@printf "Ignore directory: [%s]\n" $(EXCLUDE_DIRS)

msg1:
	@printf "Build finished [32m[%s][m.\n\n" $(BUILD)

cpdata:
	mkdir -p tmp && cp -r data/ tmp/

############################################################
$(BUILD): $(BINS) $(SUB)

$(BUILD)/%: src/%.c $$(@D)/.f $(LIBRLTAPUE)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
############################################################
.PRECIOUS: %/.f # prevent the deletion of the target
%/.f:
	@mkdir -p $(dir $@)
	@touch $@
############################################################
$(SUB):
	@printf "\nMaking mytests.%ss\n" $(SUB)
	@for dir in $(shell fd $(SUB)); do \
		echo "Making $$dir" && make BUILD=$(BUILD) -C $$dir; \
	done
############################################################
clean:
	$(RM) -r Release Debug release debug
	@for dir in $(shell fd $(SUB)); do \
		echo "Making clean $$dir" && make -C $$dir clean; \
	done
	$(RM) -r tmp
############################################################
test:
############################################################
include $(ROOT)/Make.librltapue.inc

.PHONY: clean test all msg0 $(BUILD) $(SUB) msg1 cpdata
# NOTE:
# Run without echoing commands by
# make -s
