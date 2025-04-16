# e.g. EXCLUDE := mytests/test_static.c
# e.g. EXCLUDE_DIR := tmp hidden
# tmp is excluded by make default
EXCLUDE :=
EXCLUDE_DIRS :=

# SINGLETONS build, small scenarios mainly for scratch tests which need
# a separate Makefile to build.
SINGLETONS = demo
