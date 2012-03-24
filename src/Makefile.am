#
# Copyright © 2009-2012 Michael Heimpold <mhei@heimpold.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

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
