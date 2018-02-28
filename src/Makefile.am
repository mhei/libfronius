#
# Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
#
# SPDX-License-Identifier: LGPL-2.1+
#

AM_CFLAGS = -Wall -pedantic

lib_LTLIBRARIES = libfronius.la
libfronius_la_SOURCES = \
    fronius_open.c \
    fronius_probe.c \
    fronius_close.c \
    fronius_fd.c \
    fronius_cmds.c \
    fronius_pkt_send.c \
    fronius_pkt_recv.c \
    fronius_pkt_tools.c \
    fronius-private.h \
    fronius-version.h

libfronius_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBFRONIUS_LT_VERSION_INFO) \
                        -no-undefined -export-dynamic

libfroniusincludedir = $(includedir)/libfronius
libfroniusinclude_HEADERS = fronius.h fronius-version.h

DISTCLEANFILES = fronius-version.h
EXTRA_DIST = fronius-version.h.in
CLEANFILES = *~
