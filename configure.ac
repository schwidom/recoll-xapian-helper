#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([recoll-xapian-helper], [0.3], [schwidom@gmx.net])

AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

AC_CHECK_HEADERS([xapian.h])

# Checks for typedefs, structures, and compiler characteristics.
CXXFLAGS="-std=c++11 -Wall"
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
LIBS=-lxapian
AC_CHECK_FUNCS([isascii])

AC_OUTPUT
