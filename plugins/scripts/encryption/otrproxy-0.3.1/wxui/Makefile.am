INCLUDES = -I../src @LIBOTR_CFLAGS@ @LIBGCRYPT_CFLAGS@ @WX_CXXFLAGS@

bin_PROGRAMS = otrproxy

otrproxy_SOURCES = wxotrproxy.cpp wxproxyevent.cpp wxmainframe.cpp \
		    wxotrdialog.cpp wxstatwrtext.cpp wxprefframe.cpp \
		    wxotrpolicy.cpp wxotrpolicybox.cpp

noinst_HEADERS = wxotrproxy.h wxproxyevent.h wxmainframe.h wxotrdialog.h \
		    wxstatwrtext.h wxprefframe.h wxotrpolicy.h \
		    wxotrpolicybox.h

otrproxy_LDADD = ../src/libotrproxy.a @LIBS@ @LIBOTR_LIBS@ @LIBGCRYPT_LIBS@ \
    @WX_LIBS_STATIC@
