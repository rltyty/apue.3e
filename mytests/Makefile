# Makefile for mytests.
#
# make [BUILD=Debug|Release] [SHARED=Y|N]
# NOTE: `SHARED=Y` means build tests against dynamic shared librltapue

.SECONDEXPANSION:
ROOT = ..
PLATFORM = $(shell $(ROOT)/systype.sh)

include $(ROOT)/Make.defines.$(PLATFORM)

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -lrltapue
CFLAGS		+= $(CFLAGS.$(BUILD))

# source files to be excluded from the main build
# add "-E " option of `fd` command to each excluded file or directory
include ./mytests.inc
MAIN_SRC_EXCLUDED := $(addprefix -E , $(EXCLUDE_DIRS))
MAIN_SRC_EXCLUDED += $(addprefix -E , $(SINGLETONS))

# # find all c sources using `fd` under "src/"
# SRCS := $(shell fd -e c $(MAIN_SRC_EXCLUDED) . src)
# SRCS := $(patsubst src/%.c, %.c, $(SRCS))
#
# Now for easy debug configuration, let's flatten the sources by removal
# of the top level "src" folder.
SRCS := $(shell fd -e c $(MAIN_SRC_EXCLUDED))
SRCS := $(filter-out $(EXCLUDE), $(SRCS))
BINS := $(patsubst %.c, %, $(SRCS))
BINS := $(addprefix $(BUILD)/, $(BINS))

# NOTE: `=` does recursive expansion, meaning
# if `a = $(b) b = $(c) c = d`, then `$(a)` reflects `d`.
# Thus, `=` cannot be used to self-assignment, like `CFLAGS = $(CFLAGS) -g3`,
# use `:=` instead, e.g. `CFLAGS := $(CFLAGS) -g3`

all: msg0 cpdata $(BUILD) msg1

cpdata:
	mkdir -p tmp && cp -r data/ tmp/


msg0:
	@printf "Start building [32m[%s][m\n" $(BUILD)
	@printf "Platform:[32m[%s][m\n" $(PLATFORM)
	@printf "Ignore directory: [%s]\n" $(EXCLUDE_DIRS)

msg1:
	@printf "Build finished [32m[%s][m.\n\n" $(BUILD)

############################################################
$(BUILD): $(BINS) $(SINGLETONS)

# $(BUILD)/%: src/%.c $$(@D)/.f $(LIBRLTAPUE)
$(BUILD)/%: %.c $$(@D)/.f $(LIBRLTAPUE)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
############################################################
.PRECIOUS: %/.f # prevent the deletion of the target
%/.f:
	@mkdir -p $(dir $@)
	@touch $@
############################################################
$(SINGLETONS):
	@printf "\nMaking %s in mytests...\n" $(SINGLETONS)
	@for dir in $(shell fd $(SINGLETONS)); do \
		echo "Making $$dir" && make BUILD=$(BUILD) -C $$dir; \
	done
############################################################
clean:
	$(RM) -r Release Debug release debug
	@for dir in $(shell fd $(SINGLETONS)); do \
		echo "Making clean $$dir" && make -C $$dir clean; \
	done
	$(RM) -r $(TEMPFILES)
############################################################
test:
############################################################
include $(ROOT)/Make.librltapue.inc

.PHONY: clean test all msg0 $(BUILD) $(SINGLETONS) msg1 cpdata
# NOTE:
# Run without echoing commands by
# make -s
