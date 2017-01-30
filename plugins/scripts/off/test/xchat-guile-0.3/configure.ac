dnl
dnl configure.ac for XChat Guile Plugin
dnl Copyright (C) 2005 Zeeshan Ali, zeenix@gmail.com
dnl
dnl This is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Robots; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl

dnl Process this file with autoconf to produce a configure script.
AC_INIT([XChat Guile], [0.3], [zeeshanak@gnome.org], [xchat-guile])
AC_PREREQ([2.59])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([xchat-plugin.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_CC

dnl libtool stuff
AC_LIBTOOL_DLOPEN
dnl AC_LIBLTDL_CONVENIENCE
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

dnl Check for guile
GUILE_FLAGS

xchatplugindir=${libdir}/xchat/plugins
AC_SUBST(xchatplugindir)

dnl Check for glib2
PKG_CHECK_MODULES(GLIB2,glib-2.0 >= 2.4, HAVE_GLIB2=yes,HAVE_GLIB2=no)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(GLIB2_CFLAGS)

if test "x$HAVE_GLIB2" = "xno"; then
  AC_MSG_ERROR([XChat Guile requires GLib2 to compile.])
fi

dnl See where we are supposed to install guile modules
AC_ARG_WITH([guile-module-dir],
   [use the specified installation path for Guile modules],
   [GUILE_MODULE_DIR="$withval"],
   [GUILE_MODULE_DIR="${datarootdir}/guile/site"])

AC_SUBST(GUILE_MODULE_DIR)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Done.
AC_CONFIG_FILES([Makefile m4/Makefile])
AC_OUTPUT

