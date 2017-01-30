dnl Process this file with autoconf to produce configure.

AC_INIT(wxui/wxotrproxy.cpp)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(otrproxy, 0.3.1)

AM_OPTIONS_WXCONFIG

dnl @synopsis TYPE_SOCKLEN_T
dnl
dnl Check whether sys/socket.h defines type socklen_t. Please note that
dnl some systems require sys/types.h to be included before sys/socket.h
dnl can be compiled.
dnl
dnl @category Misc
dnl @author Lars Brinkhoff <lars@nocrew.org>
dnl @version 2005-01-11
dnl @license GPLWithACException

AC_DEFUN([TYPE_SOCKLEN_T],
[AC_CACHE_CHECK([for socklen_t], ac_cv_type_socklen_t,
[
  AC_TRY_COMPILE(
  [#include <sys/types.h>
   #include <sys/socket.h>],
  [socklen_t len = 42; return 0;],
  ac_cv_type_socklen_t=yes,
  ac_cv_type_socklen_t=no)
])
  if test $ac_cv_type_socklen_t != yes; then
    AC_DEFINE(socklen_t, unsigned int, [Substitute for socklen_t])
  fi
])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

TYPE_SOCKLEN_T

AM_PATH_LIBGCRYPT(1:1.2.0,,AC_MSG_ERROR(libgcrypt 1.2.0 or newer is required.))

AM_PATH_LIBOTR(4.0.0,,AC_MSG_ERROR(libotr 3.0.0 or newer is required.))

AM_PATH_WXCONFIG(2.5.0,,AC_MSG_ERROR(wxWidgets 2.5.0 or newer is required.))

dnl On some platforms (e.g. OSX) we want to link libgcrypt, libgpg-error, and
dnl libotr statically.  Note that wxwidgets is always linked statically.
dnl Note that we need to do this *after* AM_PATH_LIBGCRYPT and AM_PATH_LIBOTR.

AC_DEFUN([CHECK_SEMISTATIC],
[AC_ARG_ENABLE([semistatic-link],
               AC_HELP_STRING([--enable-semistatic-link],
                   [link libgpg-error, libgcrypt, and libotr statically]))

if test "${enable_semistatic_link-no}" != "no" ; then
    if test "$libgcrypt_config_prefix" = "" ; then
        AC_MSG_ERROR(You must specify --with-libgcrypt-prefix if you specify --enable-semistatic-link.)
    fi
    if test "$libotr_prefix" = "" ; then
        AC_MSG_ERROR(You must specify --with-libotr-prefix if you specify --enable-semistatic-link.)
    fi
    LIBGCRYPT_LIBS="${libgcrypt_config_prefix}/lib/libgcrypt.a ${libgcrypt_config_prefix}/lib/libgpg-error.a"
    LIBOTR_LIBS="${libotr_prefix}/libotr.so.2"
fi
])
CHECK_SEMISTATIC

AC_OUTPUT([Makefile src/Makefile wxui/Makefile])

