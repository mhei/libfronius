dnl
dnl Copyright © 2009-2018 Michael Heimpold <mhei@heimpold.de>
dnl
dnl SPDX-License-Identifier: LGPL-2.1+
dnl

dnl
dnl Take care to update the libtool versioning when required (LIBFRONIUS_LD_*).
dnl
m4_define([libfronius_version_major], [0])
m4_define([libfronius_version_minor], [0])
m4_define([libfronius_version_micro], [1])

m4_define([libfronius_version],
    [libfronius_version_major.libfronius_version_minor.libfronius_version_micro])

AC_PREREQ([2.63])
AC_INIT([libfronius], [libfronius_version], [https://github.com/mhei/libfronius/issues])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])

# enable nice build output on automake1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

LIBFRONIUS_VERSION_MAJOR=libfronius_version_major
LIBFRONIUS_VERSION_MINOR=libfronius_version_minor
LIBFRONIUS_VERSION_MICRO=libfronius_version_micro
LIBFRONIUS_VERSION=libfronius_version
AC_SUBST(LIBFRONIUS_VERSION_MAJOR)
AC_SUBST(LIBFRONIUS_VERSION_MINOR)
AC_SUBST(LIBFRONIUS_VERSION_MICRO)
AC_SUBST(LIBFRONIUS_VERSION)

# ABI version
# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
LIBFRONIUS_LD_CURRENT=1
LIBFRONIUS_LD_REVISION=0
LIBFRONIUS_LD_AGE=0
LIBFRONIUS_LT_VERSION_INFO=$LIBFRONIUS_LD_CURRENT:$LIBFRONIUS_LD_REVISION:$LIBFRONIUS_LD_AGE
AC_SUBST(LIBFRONIUS_LT_VERSION_INFO)

PACKAGE_DESCRIPTION="Library for Fronius' serial communication protocol"
AC_SUBST(PACKAGE_DESCRIPTION)

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_MAKE_SET
LT_INIT([disable-static pic-only])

# Checks for header files
AC_HEADER_STDC

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        src/fronius-version.h
        tests/Makefile
        libfronius.pc
])
AC_OUTPUT
