# Wrapper around scons for gdb and people who can't read.

all:
	scons

dist:
	scons dist

clean:
	scons -c plutocracy gendoc
distclean: clean
realclean: clean

install:
	scons install prefix=$PREFIX

check:

distcheck:

.PHONY: all dist clean distclean realclean install check distcheck

