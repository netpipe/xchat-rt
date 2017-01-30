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
AC_INIT([XChat Guile], [0.1], [zeenix@gmail.com], [xchat-guile])
AC_PREREQ([2.59])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_SRCDIR([xchat-plugin.h])
AM_INIT_AUTOMAKE

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
GUILE_SITE_DIR

dnl Check for xchat and it's path
AC_CHECK_PROG(HAVE_XCHAT,xchat,yes,no) 

if test "x$HAVE_XCHAT" = "xno"; then
  AC_MSG_ERROR([XChat Guile requires XChat])
else
  AC_MSG_CHECKING([for XChat version])
  XCHAT_VERSION=`xchat --version|cut -d' ' -f2`
  AC_MSG_RESULT(["$XCHAT_VERSION"])
  XCHAT_VERSION_MAJOR=`echo $XCHAT_VERSION|cut -d'.' -f1`
  XCHAT_VERSION_MINOR=`echo $XCHAT_VERSION|cut -d'.' -f2`
  XCHAT_VERSION_MICRO=`echo $XCHAT_VERSION|cut -d'.' -f3`
  if test "$XCHAT_VERSION_MAJOR" -lt "2" ||
  	 test "$XCHAT_VERSION_MAJOR" -eq "2" -a \
  		  "$XCHAT_VERSION_MINOR" -lt "4" ||
  	 test "$XCHAT_VERSION_MAJOR" -eq "2" -a \
  		  "$XCHAT_VERSION_MINOR" -eq "4" -a \
          "$XCHAT_VERSION_MICRO" -lt "1"; then
	 	AC_MSG_ERROR([XChat Guile requires XChat >= 2.4.1])	
  fi
fi

AC_PREFIX_PROGRAM(xchat)

xchatplugindir=${libdir}/xchat/plugins
AC_SUBST(xchatplugindir)

dnl Check for glib2
PKG_CHECK_MODULES(GLIB2,glib-2.0 >= 2.4 gthread-2.0,HAVE_GLIB2=yes,HAVE_GLIB2=no)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(GLIB2_CFLAGS)

if test "x$HAVE_GLIB2" = "xno"; then
  AC_MSG_ERROR([XChat Guile requires GLib2 to compile.])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Done.
AC_CONFIG_FILES([Makefile m4/Makefile])
AC_OUTPUT

