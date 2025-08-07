# Makefile for mytests.
#
# make [BUILD=Debug|Release] [SHARED=Y|N]
# NOTE: `SHARED=Y` means build tests against dynamic shared librltapue

.SECONDEXPANSION:
ROOT = ..
PLATFORM = $(shell $(ROOT)/systype.sh)

include $(ROOT)/Make.defines.$(PLATFORM)

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

OBJS := $(patsubst %.c, $(BUILD)/%.o, $(SRCS))
BINS := $(OBJS:.o=)

# NOTE: `=` does recursive expansion, meaning
# if `a = $(b) b = $(c) c = d`, then `$(a)` reflects `d`.
# Thus, `=` cannot be used to self-assignment, like `CFLAGS = $(CFLAGS) -g3`,
# use `:=` instead, e.g. `CFLAGS := $(CFLAGS) -g3`

all: msg0 prep $(OBJS) $(BINS) $(SINGLETONS) msg1

prep:
	mkdir -p tmp && cp -r data/ tmp/

msg0:
	@printf "Build mytests in [32m[%s][m mode.\n" $(BUILD)
	@printf "Platform:[32m[%s][m\n" $(PLATFORM)
	@printf "Ignore directory: [%s]\n" $(EXCLUDE_DIRS)

msg1:
	@printf "Build finished [32m[%s][m.\n\n" $(BUILD)

include $(ROOT)/Make.librltapue.inc

LDFLAGS		= -L$(ROOT)/librlt
LDLIBS		= -l$(LIBRLTAPUE_NAME)
CFLAGS		+= $(CFLAGS.$(BUILD)) -MMD -MP
ifeq ($(SHARED), Y)
LDFLAGS		+= $(BIN_DYNLIB_RPATH)
endif

############################################################
$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%: $(BUILD)/%.o $(LIBRLTAPUE_FILE)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)
############################################################
DEPS := $(OBJS:.o=.d)
-include $(DEPS)
.PRECIOUS: $(OBJS) # prevent from removal
############################################################
$(SINGLETONS):
	@printf "\nMaking %s in mytests...\n" $(SINGLETONS)
	@for dir in $(shell fd $(SINGLETONS)); do \
		echo "Making $$dir" && $(MAKE) BUILD=$(BUILD) -C $$dir; \
	done
############################################################
clean:
	$(RM) -r Release Debug release debug
	@for dir in $(shell fd $(SINGLETONS)); do \
		echo "Making clean $$dir" && $(MAKE) -C $$dir clean; \
	done
	$(RM) -r $(TEMPFILES)
############################################################
test:
############################################################
.PHONY: clean test all msg0 $(SINGLETONS) msg1 prep

# NOTE:
# 1. Run without echoing commands by
# `make -s`
# To pass `-s` flag (contained in MAKEFLAGS) to all sub-make builds,
# use `$(MAKE)` instead of `make`
# 2. Incremental compilation and linking
# 2.1 Trace header file, if headers are updated, then C sources including
# them will be recompiled. [CFLAGS: `-MMD -MP`]
# 2.2. Keep intermediate files OBJS from auto removal. If C sources are
# updated, corresponding object files will be recompiled and executables will
# be relinked. [.PRECIOUS: $(OBJS)]
# 3. Build shared library: [LDFLAGS: -dynamiclib (macOS), -shared (Linux)]
# 3.1 Check shared library run path on Linux
# readelf -d ../librlt/librltapue.so.3 | grep -E "RUNPATH|SONAME"
# 0x000000000000000e (SONAME)             Library soname: [librltapue.so.3]
# 0x000000000000001d (RUNPATH)            Library runpath: [$ORIGIN]
# 3.2 Check shared library run path on macOS
# otool -l ../librlt/librltapue.dylib.3 | grep -A2 LC_ID_DYLIB
#          cmd LC_ID_DYLIB
#      cmdsize 56
#         name @rpath/librltapue.dylib.3 (offset 24)
# 4. Build executable, link shared library: [LDFLAGS: -Wl,-rpath,$(LIBDIR_ABS)]
# 4.1 Check executable library run path on Linux:
# readelf -d ./Debug/filedir/Ex4_1 | grep -E "RUNPATH|NEEDED"
# 0x0000000000000001 (NEEDED)             Shared library: [librltapue.so.3]
# 0x000000000000001d (RUNPATH)            Library runpath: [/home/rbach/wksp/apue.3e/librlt]
# 4.2 Check executable library run path on macOS:
# otool -l ./Debug/filedir/Ex4_1 | grep -E -A2 "LC_RPATH|LC_LOAD_DYLIB"
#         cmd LC_LOAD_DYLIB
#     cmdsize 56
#        name @rpath/librltapue.dylib.3 (offset 24)
#         cmd LC_RPATH
#     cmdsize 48
#        path /Users/gpanda/wksp/apue.3e/librlt (offset 12)
# 5. Concurrent build: [make -j, unlimited parallel; make -j4, 4 jobs at most]
#    -j equivalent to -j$(nproc)
#    Comaprison of -j, -j4, w/o -j on a $(nproc) = 8 system
#    > 78.91s user 17.90s system 484% cpu 20.001 total (-j)
#    > 53.32s user 13.46s system 286% cpu 23.292 total (-j4)
#    > 50.03s user 12.33s system 94% cpu 1:05.72 total (no parallel)
