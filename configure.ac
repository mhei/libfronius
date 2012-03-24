dnl
dnl Copyright © 2009-2012 Michael Heimpold <mhei@heimpold.de>
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
dnl

dnl
dnl Process this file with autoconf to produce a configure script.
dnl

dnl libfronius package version number, (as distinct from shared library version)
dnl An odd micro number indicates in-progress development from Git
dnl An even micro number indicates a released version
dnl
dnl Making a point release:
dnl - increase libfronius_version_micro to the next even number
dnl
dnl After the release:
dnl - increase libfronius_version_minor to the next odd number
dnl
dnl Take care to update the libtool versioning when required (LIBFRONIUS_LD_*).
dnl http://www.gnu.org/software/libtool/manual/html_node/Libtool-versioning.html
dnl
m4_define([libfronius_version_major], [0])
m4_define([libfronius_version_minor], [0])
m4_define([libfronius_version_micro], [1])

m4_define([libfronius_release_status],
    [m4_if(m4_eval(libfronius_version_minor % 2), [1], [snapshot], [release])])

m4_define([libfronius_version],
    [libfronius_version_major.libfronius_version_minor.libfronius_version_micro])

AC_PREREQ([2.63])
AC_INIT([libfronius], [libfronius_version], [https://github.com/mhei/libfronius/issues])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-bzip2])
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
LT_INIT([disable-static])

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
