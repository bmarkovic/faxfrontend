#                                               -*- Autoconf -*-
# $Id: configure.ac,v 1.5 2007/03/04 17:40:41 glenn Exp $
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(faxfrontend, 1.1, gbburkhardt@verizon.net)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h syslog.h sys/wait.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strerror])

AC_CONFIG_FILES([Makefile
                 fax4cups/Makefile fax4cups/hylafax
                 ffe/Makefile])
gw_CHECK_QT

AC_OUTPUT
