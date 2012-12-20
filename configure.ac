# Copyright (C) 2006-2012 Free Software Foundation, Inc.
# This configure.ac script is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

AC_INIT([gio-automount], [0.1], [alepic@geckoblu.net])

AM_INIT_AUTOMAKE([-Wall -Werror])

AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 data/Makefile
])

PKG_CHECK_MODULES(DEPS, gtk+-2.0 >= 2.2 glib-2.0 >= 2.2)

dnl Suggested by autoreconf
AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT
