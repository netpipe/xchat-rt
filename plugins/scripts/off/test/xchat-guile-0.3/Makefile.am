##
## Makefile.am
##
## This is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Robots; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##

AUTOMAKE_OPTIONS = 1.8.5

plugindir = $(xchatplugindir)

plugin_LTLIBRARIES = guile.la
guile_la_SOURCES = guile.c
guile_la_CFLAGS = $(GUILE_CFLAGS) $(GLIB2_CFLAGS)
guile_la_LDFLAGS = -avoid-version -module
guile_la_LIBADD = $(GUILE_LDFLAGS) $(GLIB2_LIBS)

noinst_HEADERS = xchat-plugin.h

schemedir = $(GUILE_MODULE_DIR)/xchat-guile

scheme_SCRIPTS =\
 plugin-system.scm

EXTRA_DIST =\
 plugin-system.scm\
 example.scm\
 ChangeLog

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure

SUBDIRS = m4

ACLOCAL_AMFLAGS = -I m4
