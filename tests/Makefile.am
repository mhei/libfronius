#
# Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
#
# SPDX-License-Identifier: GPL-3.0+
#

INCLUDES                = -I$(top_srcdir)/src

bin_PROGRAMS            = fronius_test

fronius_test_SOURCES    = fronius_test.c
fronius_test_LDADD      = $(top_builddir)/src/libfronius.la -lm
