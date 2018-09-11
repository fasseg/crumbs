#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.53])
AC_INIT([crumbs], [0.0.1], [frank.asseg@objecthunter.net])

# Set the default sysconf dir if it is not explicitly set by the user
test "$prefix" = NONE && test "$sysconfdir" = '${prefix}/etc' && sysconfdir='/etc'

AC_CONFIG_SRCDIR([crumbs.h])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset mkdir strrchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
