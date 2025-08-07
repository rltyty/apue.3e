DIRS = lib librlt intro sockets advio daemons datafiles db environ \
	fileio filedir ipc1 ipc2 proc pty relation signals standards \
	stdio termios threadctl threads printer exercises mytests

TESTS = librlt mytests

# Set default MAKEFLAGS for parallel execution if not already set by command line
MAKEFLAGS ?= -j

all:
	for dir in $(DIRS); do \
		(echo "Making $$dir" && $(MAKE) -C $$dir) || exit 1; \
	done

clean:
	for dir in $(DIRS); do \
		(echo "Cleaning $$dir" && $(MAKE) -C $$dir clean) || exit 1; \
	done

test:
	for dir in $(TESTS); do \
		(echo "Testing $$dir" && $(MAKE) -C $$dir test) || exit 1; \
	done

.PHONY: all clean test
