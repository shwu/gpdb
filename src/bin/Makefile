#-------------------------------------------------------------------------
#
# Makefile for src/bin (client programs)
#
# Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# $PostgreSQL: pgsql/src/bin/Makefile,v 1.54 2009/01/01 17:23:53 momjian Exp $
#
#-------------------------------------------------------------------------

subdir = src/bin
top_builddir = ../..
nbu_libdir = $(shell cd ../../gpAux/ext/$(BLD_ARCH)/Netbackup/nbu75/lib; pwd)
include $(top_builddir)/src/Makefile.global

ifeq "$(findstring -ftest-coverage,$(CFLAGS))" ""
unittest-check:
	$(MAKE) -e LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(nbu_libdir) -C pg_dump/cdb/test check
	$(MAKE) -C pg_dump/test check
else
unittest-check:
	$(MAKE) -C pg_dump/test check
endif

DIRS = initdb ipcclean pg_basebackup pg_ctl pg_dump \
	psql scripts pg_config pg_controldata pg_resetxlog \
	gpmirrortransition \
	gp_workfile_mgr \
	gp_session_state

ifeq ($(PORTNAME), win32)
DIRS+=pgevent
endif

ifeq ($(enable_gpfdist), yes)
DIRS+=gpfdist
endif

all install installdirs uninstall distprep clean distclean maintainer-clean:
	@for dir in $(DIRS); do $(MAKE) -C $$dir $@ || exit; done
