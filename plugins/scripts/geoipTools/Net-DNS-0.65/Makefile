# This Makefile is for the Net::DNS extension to perl.
#
# It was generated automatically by MakeMaker version
# 6.42 (Revision: 41145) from the contents of
# Makefile.PL. Don't edit this file, edit Makefile.PL instead.
#
#       ANY CHANGES MADE HERE WILL BE LOST!
#
#   MakeMaker ARGV: ()
#
#   MakeMaker Parameters:

#     ABSTRACT => q[Perl DNS resolver module]
#     AUTHOR => q[Olaf Kolkman <olaf@net-dns.org>]
#     MYEXTLIB => q[netdns$(LIB_EXT)]
#     NAME => q[Net::DNS]
#     OBJECT => q[$(O_FILES)]
#     PREREQ_PM => { Test::More=>q[0.18], Digest::HMAC_MD5=>q[1], MIME::Base64=>q[2.11], IO::Socket=>q[0], Digest::MD5=>q[2.12], Net::IP=>q[1.2] }
#     VERSION_FROM => q[lib/Net/DNS.pm]
#     clean => { FILES=>q[t/IPv6.enabled t/online.enabled compile.* DNS.o DNS.c DNS.bs] }

# --- MakeMaker post_initialize section:


# --- MakeMaker const_config section:

# These definitions are from config.sh (via /usr/lib/perl5/5.10.0/x86_64-linux-thread-multi/Config.pm)

# They may have been overridden via Makefile.PL or on the command line
AR = ar
CC = cc
CCCDLFLAGS = -fPIC
CCDLFLAGS = -Wl,-E -Wl,-rpath,/usr/lib/perl5/5.10.0/x86_64-linux-thread-multi/CORE
DLEXT = so
DLSRC = dl_dlopen.xs
EXE_EXT = 
FULL_AR = /usr/bin/ar
LD = cc
LDDLFLAGS = -shared -L/usr/local/lib64
LDFLAGS =  -L/usr/local/lib64
LIBC = /lib64/libc-2.9.so
LIB_EXT = .a
OBJ_EXT = .o
OSNAME = linux
OSVERS = 2.6.27
RANLIB = :
SITELIBEXP = /usr/lib/perl5/site_perl/5.10.0
SITEARCHEXP = /usr/lib/perl5/site_perl/5.10.0/x86_64-linux-thread-multi
SO = so
VENDORARCHEXP = /usr/lib/perl5/vendor_perl/5.10.0/x86_64-linux-thread-multi
VENDORLIBEXP = /usr/lib/perl5/vendor_perl/5.10.0


# --- MakeMaker constants section:
AR_STATIC_ARGS = cr
DIRFILESEP = /
DFSEP = $(DIRFILESEP)
NAME = Net::DNS
NAME_SYM = Net_DNS
VERSION = 0.65
VERSION_MACRO = VERSION
VERSION_SYM = 0_65
DEFINE_VERSION = -D$(VERSION_MACRO)=\"$(VERSION)\"
XS_VERSION = 0.65
XS_VERSION_MACRO = XS_VERSION
XS_DEFINE_VERSION = -D$(XS_VERSION_MACRO)=\"$(XS_VERSION)\"
INST_ARCHLIB = blib/arch
INST_SCRIPT = blib/script
INST_BIN = blib/bin
INST_LIB = blib/lib
INST_MAN1DIR = blib/man1
INST_MAN3DIR = blib/man3
MAN1EXT = 1
MAN3EXT = 3pm
INSTALLDIRS = site
DESTDIR = 
PREFIX = $(SITEPREFIX)
PERLPREFIX = /usr
SITEPREFIX = /usr
VENDORPREFIX = /usr
INSTALLPRIVLIB = /usr/lib/perl5/5.10.0
DESTINSTALLPRIVLIB = $(DESTDIR)$(INSTALLPRIVLIB)
INSTALLSITELIB = /usr/lib/perl5/site_perl/5.10.0
DESTINSTALLSITELIB = $(DESTDIR)$(INSTALLSITELIB)
INSTALLVENDORLIB = /usr/lib/perl5/vendor_perl/5.10.0
DESTINSTALLVENDORLIB = $(DESTDIR)$(INSTALLVENDORLIB)
INSTALLARCHLIB = /usr/lib/perl5/5.10.0/x86_64-linux-thread-multi
DESTINSTALLARCHLIB = $(DESTDIR)$(INSTALLARCHLIB)
INSTALLSITEARCH = /usr/lib/perl5/site_perl/5.10.0/x86_64-linux-thread-multi
DESTINSTALLSITEARCH = $(DESTDIR)$(INSTALLSITEARCH)
INSTALLVENDORARCH = /usr/lib/perl5/vendor_perl/5.10.0/x86_64-linux-thread-multi
DESTINSTALLVENDORARCH = $(DESTDIR)$(INSTALLVENDORARCH)
INSTALLBIN = /usr/bin
DESTINSTALLBIN = $(DESTDIR)$(INSTALLBIN)
INSTALLSITEBIN = /usr/bin
DESTINSTALLSITEBIN = $(DESTDIR)$(INSTALLSITEBIN)
INSTALLVENDORBIN = /usr/bin
DESTINSTALLVENDORBIN = $(DESTDIR)$(INSTALLVENDORBIN)
INSTALLSCRIPT = /usr/bin
DESTINSTALLSCRIPT = $(DESTDIR)$(INSTALLSCRIPT)
INSTALLSITESCRIPT = /usr/bin
DESTINSTALLSITESCRIPT = $(DESTDIR)$(INSTALLSITESCRIPT)
INSTALLVENDORSCRIPT = /usr/bin
DESTINSTALLVENDORSCRIPT = $(DESTDIR)$(INSTALLVENDORSCRIPT)
INSTALLMAN1DIR = /usr/share/man/man1
DESTINSTALLMAN1DIR = $(DESTDIR)$(INSTALLMAN1DIR)
INSTALLSITEMAN1DIR = /usr/share/man/man1
DESTINSTALLSITEMAN1DIR = $(DESTDIR)$(INSTALLSITEMAN1DIR)
INSTALLVENDORMAN1DIR = /usr/share/man/man1
DESTINSTALLVENDORMAN1DIR = $(DESTDIR)$(INSTALLVENDORMAN1DIR)
INSTALLMAN3DIR = /usr/share/man/man3
DESTINSTALLMAN3DIR = $(DESTDIR)$(INSTALLMAN3DIR)
INSTALLSITEMAN3DIR = /usr/share/man/man3
DESTINSTALLSITEMAN3DIR = $(DESTDIR)$(INSTALLSITEMAN3DIR)
INSTALLVENDORMAN3DIR = /usr/share/man/man3
DESTINSTALLVENDORMAN3DIR = $(DESTDIR)$(INSTALLVENDORMAN3DIR)
PERL_LIB = /usr/lib/perl5/5.10.0
PERL_ARCHLIB = /usr/lib/perl5/5.10.0/x86_64-linux-thread-multi
LIBPERL_A = libperl.a
MYEXTLIB = netdns$(LIB_EXT)
FIRST_MAKEFILE = Makefile
MAKEFILE_OLD = Makefile.old
MAKE_APERL_FILE = Makefile.aperl
PERLMAINCC = $(CC)
PERL_INC = /usr/lib/perl5/5.10.0/x86_64-linux-thread-multi/CORE
PERL = /usr/bin/perl
FULLPERL = /usr/bin/perl
ABSPERL = $(PERL)
PERLRUN = $(PERL)
FULLPERLRUN = $(FULLPERL)
ABSPERLRUN = $(ABSPERL)
PERLRUNINST = $(PERLRUN) "-I$(INST_ARCHLIB)" "-I$(INST_LIB)"
FULLPERLRUNINST = $(FULLPERLRUN) "-I$(INST_ARCHLIB)" "-I$(INST_LIB)"
ABSPERLRUNINST = $(ABSPERLRUN) "-I$(INST_ARCHLIB)" "-I$(INST_LIB)"
PERL_CORE = 0
PERM_RW = 644
PERM_RWX = 755

MAKEMAKER   = /usr/lib/perl5/5.10.0/ExtUtils/MakeMaker.pm
MM_VERSION  = 6.42
MM_REVISION = 41145

# FULLEXT = Pathname for extension directory (eg Foo/Bar/Oracle).
# BASEEXT = Basename part of FULLEXT. May be just equal FULLEXT. (eg Oracle)
# PARENT_NAME = NAME without BASEEXT and no trailing :: (eg Foo::Bar)
# DLBASE  = Basename part of dynamic library. May be just equal BASEEXT.
MAKE = make
FULLEXT = Net/DNS
BASEEXT = DNS
PARENT_NAME = Net
DLBASE = $(BASEEXT)
VERSION_FROM = lib/Net/DNS.pm
OBJECT = $(O_FILES)
LDFROM = $(OBJECT)
LINKTYPE = dynamic
BOOTDEP = 

# Handy lists of source code files:
XS_FILES = DNS.xs
C_FILES  = DNS.c \
	netdns.c
O_FILES  = DNS.o \
	netdns.o
H_FILES  = netdns.h
MAN1PODS = 
MAN3PODS = lib/Net/DNS.pm \
	lib/Net/DNS/FAQ.pod \
	lib/Net/DNS/Header.pm \
	lib/Net/DNS/Nameserver.pm \
	lib/Net/DNS/Packet.pm \
	lib/Net/DNS/Question.pm \
	lib/Net/DNS/RR.pm \
	lib/Net/DNS/RR/A.pm \
	lib/Net/DNS/RR/AAAA.pm \
	lib/Net/DNS/RR/AFSDB.pm \
	lib/Net/DNS/RR/APL.pm \
	lib/Net/DNS/RR/APL/ApItem.pm \
	lib/Net/DNS/RR/CERT.pm \
	lib/Net/DNS/RR/CNAME.pm \
	lib/Net/DNS/RR/DNAME.pm \
	lib/Net/DNS/RR/EID.pm \
	lib/Net/DNS/RR/HINFO.pm \
	lib/Net/DNS/RR/IPSECKEY.pm \
	lib/Net/DNS/RR/ISDN.pm \
	lib/Net/DNS/RR/LOC.pm \
	lib/Net/DNS/RR/MB.pm \
	lib/Net/DNS/RR/MG.pm \
	lib/Net/DNS/RR/MINFO.pm \
	lib/Net/DNS/RR/MR.pm \
	lib/Net/DNS/RR/MX.pm \
	lib/Net/DNS/RR/NAPTR.pm \
	lib/Net/DNS/RR/NIMLOC.pm \
	lib/Net/DNS/RR/NS.pm \
	lib/Net/DNS/RR/NSAP.pm \
	lib/Net/DNS/RR/NULL.pm \
	lib/Net/DNS/RR/OPT.pm \
	lib/Net/DNS/RR/PTR.pm \
	lib/Net/DNS/RR/PX.pm \
	lib/Net/DNS/RR/RP.pm \
	lib/Net/DNS/RR/RT.pm \
	lib/Net/DNS/RR/SOA.pm \
	lib/Net/DNS/RR/SPF.pm \
	lib/Net/DNS/RR/SRV.pm \
	lib/Net/DNS/RR/SSHFP.pm \
	lib/Net/DNS/RR/TKEY.pm \
	lib/Net/DNS/RR/TSIG.pm \
	lib/Net/DNS/RR/TXT.pm \
	lib/Net/DNS/RR/Unknown.pm \
	lib/Net/DNS/RR/X25.pm \
	lib/Net/DNS/Resolver.pm \
	lib/Net/DNS/Resolver/Base.pm \
	lib/Net/DNS/Resolver/Cygwin.pm \
	lib/Net/DNS/Resolver/Recurse.pm \
	lib/Net/DNS/Resolver/UNIX.pm \
	lib/Net/DNS/Resolver/Win32.pm \
	lib/Net/DNS/Update.pm

# Where is the Config information that we are using/depend on
CONFIGDEP = $(PERL_ARCHLIB)$(DFSEP)Config.pm $(PERL_INC)$(DFSEP)config.h

# Where to build things
INST_LIBDIR      = $(INST_LIB)/Net
INST_ARCHLIBDIR  = $(INST_ARCHLIB)/Net

INST_AUTODIR     = $(INST_LIB)/auto/$(FULLEXT)
INST_ARCHAUTODIR = $(INST_ARCHLIB)/auto/$(FULLEXT)

INST_STATIC      = $(INST_ARCHAUTODIR)/$(BASEEXT)$(LIB_EXT)
INST_DYNAMIC     = $(INST_ARCHAUTODIR)/$(DLBASE).$(DLEXT)
INST_BOOT        = $(INST_ARCHAUTODIR)/$(BASEEXT).bs

# Extra linker info
EXPORT_LIST        = 
PERL_ARCHIVE       = 
PERL_ARCHIVE_AFTER = 


TO_INST_PM = lib/Net/DNS.pm \
	lib/Net/DNS/FAQ.pod \
	lib/Net/DNS/Header.pm \
	lib/Net/DNS/Nameserver.pm \
	lib/Net/DNS/Packet.pm \
	lib/Net/DNS/Question.pm \
	lib/Net/DNS/RR.pm \
	lib/Net/DNS/RR/A.pm \
	lib/Net/DNS/RR/AAAA.pm \
	lib/Net/DNS/RR/AFSDB.pm \
	lib/Net/DNS/RR/APL.pm \
	lib/Net/DNS/RR/APL/ApItem.pm \
	lib/Net/DNS/RR/CERT.pm \
	lib/Net/DNS/RR/CNAME.pm \
	lib/Net/DNS/RR/DNAME.pm \
	lib/Net/DNS/RR/EID.pm \
	lib/Net/DNS/RR/HINFO.pm \
	lib/Net/DNS/RR/IPSECKEY.pm \
	lib/Net/DNS/RR/ISDN.pm \
	lib/Net/DNS/RR/LOC.pm \
	lib/Net/DNS/RR/MB.pm \
	lib/Net/DNS/RR/MG.pm \
	lib/Net/DNS/RR/MINFO.pm \
	lib/Net/DNS/RR/MR.pm \
	lib/Net/DNS/RR/MX.pm \
	lib/Net/DNS/RR/NAPTR.pm \
	lib/Net/DNS/RR/NIMLOC.pm \
	lib/Net/DNS/RR/NS.pm \
	lib/Net/DNS/RR/NSAP.pm \
	lib/Net/DNS/RR/NULL.pm \
	lib/Net/DNS/RR/OPT.pm \
	lib/Net/DNS/RR/PTR.pm \
	lib/Net/DNS/RR/PX.pm \
	lib/Net/DNS/RR/RP.pm \
	lib/Net/DNS/RR/RT.pm \
	lib/Net/DNS/RR/SOA.pm \
	lib/Net/DNS/RR/SPF.pm \
	lib/Net/DNS/RR/SRV.pm \
	lib/Net/DNS/RR/SSHFP.pm \
	lib/Net/DNS/RR/TKEY.pm \
	lib/Net/DNS/RR/TSIG.pm \
	lib/Net/DNS/RR/TXT.pm \
	lib/Net/DNS/RR/Unknown.pm \
	lib/Net/DNS/RR/X25.pm \
	lib/Net/DNS/Resolver.pm \
	lib/Net/DNS/Resolver/Base.pm \
	lib/Net/DNS/Resolver/Cygwin.pm \
	lib/Net/DNS/Resolver/Recurse.pm \
	lib/Net/DNS/Resolver/UNIX.pm \
	lib/Net/DNS/Resolver/Win32.pm \
	lib/Net/DNS/Update.pm

PM_TO_BLIB = lib/Net/DNS/RR/A.pm \
	blib/lib/Net/DNS/RR/A.pm \
	lib/Net/DNS/Resolver/Win32.pm \
	blib/lib/Net/DNS/Resolver/Win32.pm \
	lib/Net/DNS/RR/Unknown.pm \
	blib/lib/Net/DNS/RR/Unknown.pm \
	lib/Net/DNS/RR/IPSECKEY.pm \
	blib/lib/Net/DNS/RR/IPSECKEY.pm \
	lib/Net/DNS/RR/EID.pm \
	blib/lib/Net/DNS/RR/EID.pm \
	lib/Net/DNS/RR/ISDN.pm \
	blib/lib/Net/DNS/RR/ISDN.pm \
	lib/Net/DNS/RR/MX.pm \
	blib/lib/Net/DNS/RR/MX.pm \
	lib/Net/DNS/Header.pm \
	blib/lib/Net/DNS/Header.pm \
	lib/Net/DNS/RR/SOA.pm \
	blib/lib/Net/DNS/RR/SOA.pm \
	lib/Net/DNS/RR/OPT.pm \
	blib/lib/Net/DNS/RR/OPT.pm \
	lib/Net/DNS/RR/NIMLOC.pm \
	blib/lib/Net/DNS/RR/NIMLOC.pm \
	lib/Net/DNS/RR/TXT.pm \
	blib/lib/Net/DNS/RR/TXT.pm \
	lib/Net/DNS/RR/DNAME.pm \
	blib/lib/Net/DNS/RR/DNAME.pm \
	lib/Net/DNS/RR/AAAA.pm \
	blib/lib/Net/DNS/RR/AAAA.pm \
	lib/Net/DNS.pm \
	blib/lib/Net/DNS.pm \
	lib/Net/DNS/RR/RP.pm \
	blib/lib/Net/DNS/RR/RP.pm \
	lib/Net/DNS/RR/X25.pm \
	blib/lib/Net/DNS/RR/X25.pm \
	lib/Net/DNS/Resolver/Cygwin.pm \
	blib/lib/Net/DNS/Resolver/Cygwin.pm \
	lib/Net/DNS/Resolver/Recurse.pm \
	blib/lib/Net/DNS/Resolver/Recurse.pm \
	lib/Net/DNS/RR/APL.pm \
	blib/lib/Net/DNS/RR/APL.pm \
	lib/Net/DNS/RR/PTR.pm \
	blib/lib/Net/DNS/RR/PTR.pm \
	lib/Net/DNS/RR/NS.pm \
	blib/lib/Net/DNS/RR/NS.pm \
	lib/Net/DNS/RR/TSIG.pm \
	blib/lib/Net/DNS/RR/TSIG.pm \
	lib/Net/DNS/Update.pm \
	blib/lib/Net/DNS/Update.pm \
	lib/Net/DNS/Resolver.pm \
	blib/lib/Net/DNS/Resolver.pm \
	lib/Net/DNS/RR/PX.pm \
	blib/lib/Net/DNS/RR/PX.pm \
	lib/Net/DNS/Nameserver.pm \
	blib/lib/Net/DNS/Nameserver.pm \
	lib/Net/DNS/RR/LOC.pm \
	blib/lib/Net/DNS/RR/LOC.pm \
	lib/Net/DNS/RR/NULL.pm \
	blib/lib/Net/DNS/RR/NULL.pm \
	lib/Net/DNS/RR/RT.pm \
	blib/lib/Net/DNS/RR/RT.pm \
	lib/Net/DNS/RR/SRV.pm \
	blib/lib/Net/DNS/RR/SRV.pm \
	lib/Net/DNS/RR/AFSDB.pm \
	blib/lib/Net/DNS/RR/AFSDB.pm \
	lib/Net/DNS/RR/MR.pm \
	blib/lib/Net/DNS/RR/MR.pm \
	lib/Net/DNS/RR/MINFO.pm \
	blib/lib/Net/DNS/RR/MINFO.pm \
	lib/Net/DNS/FAQ.pod \
	blib/lib/Net/DNS/FAQ.pod \
	lib/Net/DNS/RR/CNAME.pm \
	blib/lib/Net/DNS/RR/CNAME.pm \
	lib/Net/DNS/Question.pm \
	blib/lib/Net/DNS/Question.pm \
	lib/Net/DNS/RR/SSHFP.pm \
	blib/lib/Net/DNS/RR/SSHFP.pm \
	lib/Net/DNS/RR/NAPTR.pm \
	blib/lib/Net/DNS/RR/NAPTR.pm \
	lib/Net/DNS/RR/SPF.pm \
	blib/lib/Net/DNS/RR/SPF.pm \
	lib/Net/DNS/RR/APL/ApItem.pm \
	blib/lib/Net/DNS/RR/APL/ApItem.pm \
	lib/Net/DNS/Packet.pm \
	blib/lib/Net/DNS/Packet.pm \
	lib/Net/DNS/RR.pm \
	blib/lib/Net/DNS/RR.pm \
	lib/Net/DNS/RR/HINFO.pm \
	blib/lib/Net/DNS/RR/HINFO.pm \
	lib/Net/DNS/RR/MG.pm \
	blib/lib/Net/DNS/RR/MG.pm \
	lib/Net/DNS/RR/TKEY.pm \
	blib/lib/Net/DNS/RR/TKEY.pm \
	lib/Net/DNS/RR/NSAP.pm \
	blib/lib/Net/DNS/RR/NSAP.pm \
	lib/Net/DNS/RR/CERT.pm \
	blib/lib/Net/DNS/RR/CERT.pm \
	lib/Net/DNS/Resolver/UNIX.pm \
	blib/lib/Net/DNS/Resolver/UNIX.pm \
	lib/Net/DNS/RR/MB.pm \
	blib/lib/Net/DNS/RR/MB.pm \
	lib/Net/DNS/Resolver/Base.pm \
	blib/lib/Net/DNS/Resolver/Base.pm


# --- MakeMaker platform_constants section:
MM_Unix_VERSION = 6.42
PERL_MALLOC_DEF = -DPERL_EXTMALLOC_DEF -Dmalloc=Perl_malloc -Dfree=Perl_mfree -Drealloc=Perl_realloc -Dcalloc=Perl_calloc


# --- MakeMaker tool_autosplit section:
# Usage: $(AUTOSPLITFILE) FileToSplit AutoDirToSplitInto
AUTOSPLITFILE = $(ABSPERLRUN)  -e 'use AutoSplit;  autosplit($$ARGV[0], $$ARGV[1], 0, 1, 1)' --



# --- MakeMaker tool_xsubpp section:

XSUBPPDIR = /usr/lib/perl5/5.10.0/ExtUtils
XSUBPP = $(XSUBPPDIR)$(DFSEP)xsubpp
XSUBPPRUN = $(PERLRUN) $(XSUBPP)
XSPROTOARG = 
XSUBPPDEPS = /usr/lib/perl5/5.10.0/ExtUtils/typemap $(XSUBPP)
XSUBPPARGS = -typemap /usr/lib/perl5/5.10.0/ExtUtils/typemap
XSUBPP_EXTRA_ARGS = 


# --- MakeMaker tools_other section:
SHELL = /bin/sh
CHMOD = chmod
CP = cp
MV = mv
NOOP = $(SHELL) -c true
NOECHO = @
RM_F = rm -f
RM_RF = rm -rf
TEST_F = test -f
TOUCH = touch
UMASK_NULL = umask 0
DEV_NULL = > /dev/null 2>&1
MKPATH = $(ABSPERLRUN) "-MExtUtils::Command" -e mkpath
EQUALIZE_TIMESTAMP = $(ABSPERLRUN) "-MExtUtils::Command" -e eqtime
ECHO = echo
ECHO_N = echo -n
UNINST = 0
VERBINST = 0
MOD_INSTALL = $(ABSPERLRUN) -MExtUtils::Install -e 'install({@ARGV}, '\''$(VERBINST)'\'', 0, '\''$(UNINST)'\'');' --
DOC_INSTALL = $(ABSPERLRUN) "-MExtUtils::Command::MM" -e perllocal_install
UNINSTALL = $(ABSPERLRUN) "-MExtUtils::Command::MM" -e uninstall
WARN_IF_OLD_PACKLIST = $(ABSPERLRUN) "-MExtUtils::Command::MM" -e warn_if_old_packlist
MACROSTART = 
MACROEND = 
USEMAKEFILE = -f
FIXIN = $(PERLRUN) "-MExtUtils::MY" -e "MY->fixin(shift)"


# --- MakeMaker makemakerdflt section:
makemakerdflt : all
	$(NOECHO) $(NOOP)


# --- MakeMaker dist section:
TAR = tar
TARFLAGS = cvf
ZIP = zip
ZIPFLAGS = -r
COMPRESS = gzip --best
SUFFIX = .gz
SHAR = shar
PREOP = $(NOECHO) $(NOOP)
POSTOP = $(NOECHO) $(NOOP)
TO_UNIX = $(NOECHO) $(NOOP)
CI = ci -u
RCS_LABEL = rcs -Nv$(VERSION_SYM): -q
DIST_CP = best
DIST_DEFAULT = tardist
DISTNAME = Net-DNS
DISTVNAME = Net-DNS-0.65


# --- MakeMaker macro section:


# --- MakeMaker depend section:


# --- MakeMaker cflags section:

CCFLAGS = -D_REENTRANT -D_GNU_SOURCE -DPERL_USE_SAFE_PUTENV -DDEBUGGING -fno-strict-aliasing -pipe -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
OPTIMIZE = -fmessage-length=0 -O2 -Wall -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -g -Wall -pipe
PERLTYPE = 
MPOLLUTE = 


# --- MakeMaker const_loadlibs section:

# Net::DNS might depend on some other libraries:
# See ExtUtils::Liblist for details
#


# --- MakeMaker const_cccmd section:
CCCMD = $(CC) -c $(PASTHRU_INC) $(INC) \
	$(CCFLAGS) $(OPTIMIZE) \
	$(PERLTYPE) $(MPOLLUTE) $(DEFINE_VERSION) \
	$(XS_DEFINE_VERSION)

# --- MakeMaker post_constants section:


# --- MakeMaker pasthru section:

PASTHRU = LIBPERL_A="$(LIBPERL_A)"\
	LINKTYPE="$(LINKTYPE)"\
	OPTIMIZE="$(OPTIMIZE)"\
	PREFIX="$(PREFIX)"


# --- MakeMaker special_targets section:
.SUFFIXES : .xs .c .C .cpp .i .s .cxx .cc $(OBJ_EXT)

.PHONY: all config static dynamic test linkext manifest blibdirs clean realclean disttest distdir



# --- MakeMaker c_o section:

.c.i:
	cc -E -c $(PASTHRU_INC) $(INC) \
	$(CCFLAGS) $(OPTIMIZE) \
	$(PERLTYPE) $(MPOLLUTE) $(DEFINE_VERSION) \
	$(XS_DEFINE_VERSION) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.c > $*.i

.c.s:
	$(CCCMD) -S $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.c

.c$(OBJ_EXT):
	$(CCCMD) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.c

.cpp$(OBJ_EXT):
	$(CCCMD) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.cpp

.cxx$(OBJ_EXT):
	$(CCCMD) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.cxx

.cc$(OBJ_EXT):
	$(CCCMD) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.cc

.C$(OBJ_EXT):
	$(CCCMD) $*.C


# --- MakeMaker xs_c section:

.xs.c:
	$(XSUBPPRUN) $(XSPROTOARG) $(XSUBPPARGS) $(XSUBPP_EXTRA_ARGS) $*.xs > $*.xsc && $(MV) $*.xsc $*.c


# --- MakeMaker xs_o section:

.xs$(OBJ_EXT):
	$(XSUBPPRUN) $(XSPROTOARG) $(XSUBPPARGS) $*.xs > $*.xsc && $(MV) $*.xsc $*.c
	$(CCCMD) $(CCCDLFLAGS) "-I$(PERL_INC)" $(PASTHRU_DEFINE) $(DEFINE) $*.c


# --- MakeMaker top_targets section:
all :: pure_all manifypods
	$(NOECHO) $(NOOP)


pure_all :: config pm_to_blib subdirs linkext
	$(NOECHO) $(NOOP)

subdirs :: $(MYEXTLIB)
	$(NOECHO) $(NOOP)

config :: $(FIRST_MAKEFILE) blibdirs
	$(NOECHO) $(NOOP)

$(O_FILES): $(H_FILES)

help :
	perldoc ExtUtils::MakeMaker


# --- MakeMaker blibdirs section:
blibdirs : $(INST_LIBDIR)$(DFSEP).exists $(INST_ARCHLIB)$(DFSEP).exists $(INST_AUTODIR)$(DFSEP).exists $(INST_ARCHAUTODIR)$(DFSEP).exists $(INST_BIN)$(DFSEP).exists $(INST_SCRIPT)$(DFSEP).exists $(INST_MAN1DIR)$(DFSEP).exists $(INST_MAN3DIR)$(DFSEP).exists
	$(NOECHO) $(NOOP)

# Backwards compat with 6.18 through 6.25
blibdirs.ts : blibdirs
	$(NOECHO) $(NOOP)

$(INST_LIBDIR)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_LIBDIR)
	$(NOECHO) $(CHMOD) 755 $(INST_LIBDIR)
	$(NOECHO) $(TOUCH) $(INST_LIBDIR)$(DFSEP).exists

$(INST_ARCHLIB)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_ARCHLIB)
	$(NOECHO) $(CHMOD) 755 $(INST_ARCHLIB)
	$(NOECHO) $(TOUCH) $(INST_ARCHLIB)$(DFSEP).exists

$(INST_AUTODIR)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_AUTODIR)
	$(NOECHO) $(CHMOD) 755 $(INST_AUTODIR)
	$(NOECHO) $(TOUCH) $(INST_AUTODIR)$(DFSEP).exists

$(INST_ARCHAUTODIR)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_ARCHAUTODIR)
	$(NOECHO) $(CHMOD) 755 $(INST_ARCHAUTODIR)
	$(NOECHO) $(TOUCH) $(INST_ARCHAUTODIR)$(DFSEP).exists

$(INST_BIN)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_BIN)
	$(NOECHO) $(CHMOD) 755 $(INST_BIN)
	$(NOECHO) $(TOUCH) $(INST_BIN)$(DFSEP).exists

$(INST_SCRIPT)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_SCRIPT)
	$(NOECHO) $(CHMOD) 755 $(INST_SCRIPT)
	$(NOECHO) $(TOUCH) $(INST_SCRIPT)$(DFSEP).exists

$(INST_MAN1DIR)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_MAN1DIR)
	$(NOECHO) $(CHMOD) 755 $(INST_MAN1DIR)
	$(NOECHO) $(TOUCH) $(INST_MAN1DIR)$(DFSEP).exists

$(INST_MAN3DIR)$(DFSEP).exists :: Makefile.PL
	$(NOECHO) $(MKPATH) $(INST_MAN3DIR)
	$(NOECHO) $(CHMOD) 755 $(INST_MAN3DIR)
	$(NOECHO) $(TOUCH) $(INST_MAN3DIR)$(DFSEP).exists



# --- MakeMaker linkext section:

linkext :: $(LINKTYPE)
	$(NOECHO) $(NOOP)


# --- MakeMaker dlsyms section:


# --- MakeMaker dynamic section:

dynamic :: $(FIRST_MAKEFILE) $(INST_DYNAMIC) $(INST_BOOT)
	$(NOECHO) $(NOOP)


# --- MakeMaker dynamic_bs section:
BOOTSTRAP = $(BASEEXT).bs

# As Mkbootstrap might not write a file (if none is required)
# we use touch to prevent make continually trying to remake it.
# The DynaLoader only reads a non-empty file.
$(BOOTSTRAP) : $(FIRST_MAKEFILE) $(BOOTDEP) $(INST_ARCHAUTODIR)$(DFSEP).exists
	$(NOECHO) $(ECHO) "Running Mkbootstrap for $(NAME) ($(BSLOADLIBS))"
	$(NOECHO) $(PERLRUN) \
		"-MExtUtils::Mkbootstrap" \
		-e "Mkbootstrap('$(BASEEXT)','$(BSLOADLIBS)');"
	$(NOECHO) $(TOUCH) $@
	$(CHMOD) $(PERM_RW) $@

$(INST_BOOT) : $(BOOTSTRAP) $(INST_ARCHAUTODIR)$(DFSEP).exists
	$(NOECHO) $(RM_RF) $@
	- $(CP) $(BOOTSTRAP) $@
	$(CHMOD) $(PERM_RW) $@


# --- MakeMaker dynamic_lib section:

# This section creates the dynamically loadable $(INST_DYNAMIC)
# from $(OBJECT) and possibly $(MYEXTLIB).
ARMAYBE = :
OTHERLDFLAGS = 
INST_DYNAMIC_DEP = 
INST_DYNAMIC_FIX = 

$(INST_DYNAMIC): $(OBJECT) $(MYEXTLIB) $(BOOTSTRAP) $(INST_ARCHAUTODIR)$(DFSEP).exists $(EXPORT_LIST) $(PERL_ARCHIVE) $(PERL_ARCHIVE_AFTER) $(INST_DYNAMIC_DEP)
	$(RM_F) $@
	$(LD)  $(LDDLFLAGS) $(LDFROM) $(OTHERLDFLAGS) -o $@ $(MYEXTLIB)	\
	  $(PERL_ARCHIVE) $(LDLOADLIBS) $(PERL_ARCHIVE_AFTER) $(EXPORT_LIST)	\
	  $(INST_DYNAMIC_FIX)
	$(CHMOD) $(PERM_RWX) $@


# --- MakeMaker static section:

## $(INST_PM) has been moved to the all: target.
## It remains here for awhile to allow for old usage: "make static"
static :: $(FIRST_MAKEFILE) $(INST_STATIC)
	$(NOECHO) $(NOOP)


# --- MakeMaker static_lib section:

$(INST_STATIC) : $(OBJECT) $(MYEXTLIB) $(INST_ARCHAUTODIR)$(DFSEP).exists
	$(RM_RF) $@
	$(CP) $(MYEXTLIB) $@
	$(FULL_AR) $(AR_STATIC_ARGS) $@ $(OBJECT) && $(RANLIB) $@
	$(CHMOD) $(PERM_RWX) $@
	$(NOECHO) $(ECHO) "$(EXTRALIBS)" > $(INST_ARCHAUTODIR)/extralibs.ld


# --- MakeMaker manifypods section:

POD2MAN_EXE = $(PERLRUN) "-MExtUtils::Command::MM" -e pod2man "--"
POD2MAN = $(POD2MAN_EXE)


manifypods : pure_all  \
	lib/Net/DNS/RR/A.pm \
	lib/Net/DNS/Resolver/Win32.pm \
	lib/Net/DNS/RR/Unknown.pm \
	lib/Net/DNS/RR/IPSECKEY.pm \
	lib/Net/DNS/RR/EID.pm \
	lib/Net/DNS/RR/ISDN.pm \
	lib/Net/DNS/RR/MX.pm \
	lib/Net/DNS/Header.pm \
	lib/Net/DNS/RR/SOA.pm \
	lib/Net/DNS/RR/OPT.pm \
	lib/Net/DNS/RR/NIMLOC.pm \
	lib/Net/DNS/RR/TXT.pm \
	lib/Net/DNS/RR/DNAME.pm \
	lib/Net/DNS/RR/AAAA.pm \
	lib/Net/DNS.pm \
	lib/Net/DNS/RR/RP.pm \
	lib/Net/DNS/RR/X25.pm \
	lib/Net/DNS/Resolver/Cygwin.pm \
	lib/Net/DNS/Resolver/Recurse.pm \
	lib/Net/DNS/RR/APL.pm \
	lib/Net/DNS/RR/PTR.pm \
	lib/Net/DNS/RR/NS.pm \
	lib/Net/DNS/RR/TSIG.pm \
	lib/Net/DNS/Update.pm \
	lib/Net/DNS/Resolver.pm \
	lib/Net/DNS/RR/PX.pm \
	lib/Net/DNS/Nameserver.pm \
	lib/Net/DNS/RR/LOC.pm \
	lib/Net/DNS/RR/NULL.pm \
	lib/Net/DNS/RR/RT.pm \
	lib/Net/DNS/RR/SRV.pm \
	lib/Net/DNS/RR/AFSDB.pm \
	lib/Net/DNS/RR/MR.pm \
	lib/Net/DNS/RR/MINFO.pm \
	lib/Net/DNS/FAQ.pod \
	lib/Net/DNS/RR/CNAME.pm \
	lib/Net/DNS/Question.pm \
	lib/Net/DNS/RR/SSHFP.pm \
	lib/Net/DNS/RR/NAPTR.pm \
	lib/Net/DNS/RR/SPF.pm \
	lib/Net/DNS/RR/APL/ApItem.pm \
	lib/Net/DNS/Packet.pm \
	lib/Net/DNS/RR.pm \
	lib/Net/DNS/RR/HINFO.pm \
	lib/Net/DNS/RR/MG.pm \
	lib/Net/DNS/RR/TKEY.pm \
	lib/Net/DNS/RR/NSAP.pm \
	lib/Net/DNS/RR/CERT.pm \
	lib/Net/DNS/Resolver/UNIX.pm \
	lib/Net/DNS/RR/MB.pm \
	lib/Net/DNS/Resolver/Base.pm
	$(NOECHO) $(POD2MAN) --section=3 --perm_rw=$(PERM_RW) \
	  lib/Net/DNS/RR/A.pm $(INST_MAN3DIR)/Net::DNS::RR::A.$(MAN3EXT) \
	  lib/Net/DNS/Resolver/Win32.pm $(INST_MAN3DIR)/Net::DNS::Resolver::Win32.$(MAN3EXT) \
	  lib/Net/DNS/RR/Unknown.pm $(INST_MAN3DIR)/Net::DNS::RR::Unknown.$(MAN3EXT) \
	  lib/Net/DNS/RR/IPSECKEY.pm $(INST_MAN3DIR)/Net::DNS::RR::IPSECKEY.$(MAN3EXT) \
	  lib/Net/DNS/RR/EID.pm $(INST_MAN3DIR)/Net::DNS::RR::EID.$(MAN3EXT) \
	  lib/Net/DNS/RR/ISDN.pm $(INST_MAN3DIR)/Net::DNS::RR::ISDN.$(MAN3EXT) \
	  lib/Net/DNS/RR/MX.pm $(INST_MAN3DIR)/Net::DNS::RR::MX.$(MAN3EXT) \
	  lib/Net/DNS/Header.pm $(INST_MAN3DIR)/Net::DNS::Header.$(MAN3EXT) \
	  lib/Net/DNS/RR/SOA.pm $(INST_MAN3DIR)/Net::DNS::RR::SOA.$(MAN3EXT) \
	  lib/Net/DNS/RR/OPT.pm $(INST_MAN3DIR)/Net::DNS::RR::OPT.$(MAN3EXT) \
	  lib/Net/DNS/RR/NIMLOC.pm $(INST_MAN3DIR)/Net::DNS::RR::NIMLOC.$(MAN3EXT) \
	  lib/Net/DNS/RR/TXT.pm $(INST_MAN3DIR)/Net::DNS::RR::TXT.$(MAN3EXT) \
	  lib/Net/DNS/RR/DNAME.pm $(INST_MAN3DIR)/Net::DNS::RR::DNAME.$(MAN3EXT) \
	  lib/Net/DNS/RR/AAAA.pm $(INST_MAN3DIR)/Net::DNS::RR::AAAA.$(MAN3EXT) \
	  lib/Net/DNS.pm $(INST_MAN3DIR)/Net::DNS.$(MAN3EXT) \
	  lib/Net/DNS/RR/RP.pm $(INST_MAN3DIR)/Net::DNS::RR::RP.$(MAN3EXT) \
	  lib/Net/DNS/RR/X25.pm $(INST_MAN3DIR)/Net::DNS::RR::X25.$(MAN3EXT) \
	  lib/Net/DNS/Resolver/Cygwin.pm $(INST_MAN3DIR)/Net::DNS::Resolver::Cygwin.$(MAN3EXT) \
	  lib/Net/DNS/Resolver/Recurse.pm $(INST_MAN3DIR)/Net::DNS::Resolver::Recurse.$(MAN3EXT) \
	  lib/Net/DNS/RR/APL.pm $(INST_MAN3DIR)/Net::DNS::RR::APL.$(MAN3EXT) \
	  lib/Net/DNS/RR/PTR.pm $(INST_MAN3DIR)/Net::DNS::RR::PTR.$(MAN3EXT) \
	  lib/Net/DNS/RR/NS.pm $(INST_MAN3DIR)/Net::DNS::RR::NS.$(MAN3EXT) \
	  lib/Net/DNS/RR/TSIG.pm $(INST_MAN3DIR)/Net::DNS::RR::TSIG.$(MAN3EXT) \
	  lib/Net/DNS/Update.pm $(INST_MAN3DIR)/Net::DNS::Update.$(MAN3EXT) \
	  lib/Net/DNS/Resolver.pm $(INST_MAN3DIR)/Net::DNS::Resolver.$(MAN3EXT) \
	  lib/Net/DNS/RR/PX.pm $(INST_MAN3DIR)/Net::DNS::RR::PX.$(MAN3EXT) \
	  lib/Net/DNS/Nameserver.pm $(INST_MAN3DIR)/Net::DNS::Nameserver.$(MAN3EXT) \
	  lib/Net/DNS/RR/LOC.pm $(INST_MAN3DIR)/Net::DNS::RR::LOC.$(MAN3EXT) \
	  lib/Net/DNS/RR/NULL.pm $(INST_MAN3DIR)/Net::DNS::RR::NULL.$(MAN3EXT) \
	  lib/Net/DNS/RR/RT.pm $(INST_MAN3DIR)/Net::DNS::RR::RT.$(MAN3EXT) \
	  lib/Net/DNS/RR/SRV.pm $(INST_MAN3DIR)/Net::DNS::RR::SRV.$(MAN3EXT) \
	  lib/Net/DNS/RR/AFSDB.pm $(INST_MAN3DIR)/Net::DNS::RR::AFSDB.$(MAN3EXT) \
	  lib/Net/DNS/RR/MR.pm $(INST_MAN3DIR)/Net::DNS::RR::MR.$(MAN3EXT) \
	  lib/Net/DNS/RR/MINFO.pm $(INST_MAN3DIR)/Net::DNS::RR::MINFO.$(MAN3EXT) \
	  lib/Net/DNS/FAQ.pod $(INST_MAN3DIR)/Net::DNS::FAQ.$(MAN3EXT) \
	  lib/Net/DNS/RR/CNAME.pm $(INST_MAN3DIR)/Net::DNS::RR::CNAME.$(MAN3EXT) \
	  lib/Net/DNS/Question.pm $(INST_MAN3DIR)/Net::DNS::Question.$(MAN3EXT) \
	  lib/Net/DNS/RR/SSHFP.pm $(INST_MAN3DIR)/Net::DNS::RR::SSHFP.$(MAN3EXT) \
	  lib/Net/DNS/RR/NAPTR.pm $(INST_MAN3DIR)/Net::DNS::RR::NAPTR.$(MAN3EXT) \
	  lib/Net/DNS/RR/SPF.pm $(INST_MAN3DIR)/Net::DNS::RR::SPF.$(MAN3EXT) \
	  lib/Net/DNS/RR/APL/ApItem.pm $(INST_MAN3DIR)/Net::DNS::RR::APL::ApItem.$(MAN3EXT) \
	  lib/Net/DNS/Packet.pm $(INST_MAN3DIR)/Net::DNS::Packet.$(MAN3EXT) \
	  lib/Net/DNS/RR.pm $(INST_MAN3DIR)/Net::DNS::RR.$(MAN3EXT) \
	  lib/Net/DNS/RR/HINFO.pm $(INST_MAN3DIR)/Net::DNS::RR::HINFO.$(MAN3EXT) 
	$(NOECHO) $(POD2MAN) --section=3 --perm_rw=$(PERM_RW) \
	  lib/Net/DNS/RR/MG.pm $(INST_MAN3DIR)/Net::DNS::RR::MG.$(MAN3EXT) \
	  lib/Net/DNS/RR/TKEY.pm $(INST_MAN3DIR)/Net::DNS::RR::TKEY.$(MAN3EXT) \
	  lib/Net/DNS/RR/NSAP.pm $(INST_MAN3DIR)/Net::DNS::RR::NSAP.$(MAN3EXT) \
	  lib/Net/DNS/RR/CERT.pm $(INST_MAN3DIR)/Net::DNS::RR::CERT.$(MAN3EXT) \
	  lib/Net/DNS/Resolver/UNIX.pm $(INST_MAN3DIR)/Net::DNS::Resolver::UNIX.$(MAN3EXT) \
	  lib/Net/DNS/RR/MB.pm $(INST_MAN3DIR)/Net::DNS::RR::MB.$(MAN3EXT) \
	  lib/Net/DNS/Resolver/Base.pm $(INST_MAN3DIR)/Net::DNS::Resolver::Base.$(MAN3EXT) 




# --- MakeMaker processPL section:


# --- MakeMaker installbin section:


# --- MakeMaker subdirs section:

# none

# --- MakeMaker clean_subdirs section:
clean_subdirs :
	$(NOECHO) $(NOOP)


# --- MakeMaker clean section:

# Delete temporary files but do not touch installed files. We don't delete
# the Makefile here so a later make realclean still has a makefile to use.

clean :: clean_subdirs
	- $(RM_F) \
	  *$(LIB_EXT) core \
	  core.[0-9] $(INST_ARCHAUTODIR)/extralibs.all \
	  core.[0-9][0-9] $(BASEEXT).bso \
	  pm_to_blib.ts core.[0-9][0-9][0-9][0-9] \
	  $(BASEEXT).x $(BOOTSTRAP) \
	  perl$(EXE_EXT) tmon.out \
	  *$(OBJ_EXT) pm_to_blib \
	  $(INST_ARCHAUTODIR)/extralibs.ld blibdirs.ts \
	  core.[0-9][0-9][0-9][0-9][0-9] *perl.core \
	  core.*perl.*.? $(MAKE_APERL_FILE) \
	  perl $(BASEEXT).def \
	  core.[0-9][0-9][0-9] mon.out \
	  lib$(BASEEXT).def perlmain.c \
	  perl.exe DNS.c \
	  so_locations $(BASEEXT).exp 
	- $(RM_RF) \
	  DNS.o t/online.enabled \
	  DNS.bs DNS.c \
	  t/IPv6.enabled compile.* \
	  blib 
	- $(MV) $(FIRST_MAKEFILE) $(MAKEFILE_OLD) $(DEV_NULL)


# --- MakeMaker realclean_subdirs section:
realclean_subdirs :
	$(NOECHO) $(NOOP)


# --- MakeMaker realclean section:
# Delete temporary files (via clean) and also delete dist files
realclean purge ::  clean realclean_subdirs
	- $(RM_F) \
	  $(OBJECT) $(MAKEFILE_OLD) \
	  $(FIRST_MAKEFILE) 
	- $(RM_RF) \
	  $(DISTVNAME) 


# --- MakeMaker metafile section:
metafile : create_distdir
	$(NOECHO) $(ECHO) Generating META.yml
	$(NOECHO) $(ECHO) '--- #YAML:1.0' > META_new.yml
	$(NOECHO) $(ECHO) 'name:                Net-DNS' >> META_new.yml
	$(NOECHO) $(ECHO) 'version:             0.65' >> META_new.yml
	$(NOECHO) $(ECHO) 'abstract:            Perl DNS resolver module' >> META_new.yml
	$(NOECHO) $(ECHO) 'license:             ~' >> META_new.yml
	$(NOECHO) $(ECHO) 'author:              ' >> META_new.yml
	$(NOECHO) $(ECHO) '    - Olaf Kolkman <olaf@net-dns.org>' >> META_new.yml
	$(NOECHO) $(ECHO) 'generated_by:        ExtUtils::MakeMaker version 6.42' >> META_new.yml
	$(NOECHO) $(ECHO) 'distribution_type:   module' >> META_new.yml
	$(NOECHO) $(ECHO) 'requires:     ' >> META_new.yml
	$(NOECHO) $(ECHO) '    Digest::HMAC_MD5:              1' >> META_new.yml
	$(NOECHO) $(ECHO) '    Digest::MD5:                   2.12' >> META_new.yml
	$(NOECHO) $(ECHO) '    IO::Socket:                    0' >> META_new.yml
	$(NOECHO) $(ECHO) '    MIME::Base64:                  2.11' >> META_new.yml
	$(NOECHO) $(ECHO) '    Net::IP:                       1.2' >> META_new.yml
	$(NOECHO) $(ECHO) '    Test::More:                    0.18' >> META_new.yml
	$(NOECHO) $(ECHO) 'meta-spec:' >> META_new.yml
	$(NOECHO) $(ECHO) '    url:     http://module-build.sourceforge.net/META-spec-v1.3.html' >> META_new.yml
	$(NOECHO) $(ECHO) '    version: 1.3' >> META_new.yml
	-$(NOECHO) $(MV) META_new.yml $(DISTVNAME)/META.yml
	$(NOECHO) $(ECHO) 'license: perl' >> META.yml


# --- MakeMaker signature section:
signature :
	cpansign -s


# --- MakeMaker dist_basics section:
distclean :: realclean distcheck
	$(NOECHO) $(NOOP)

distcheck :
	$(PERLRUN) "-MExtUtils::Manifest=fullcheck" -e fullcheck

skipcheck :
	$(PERLRUN) "-MExtUtils::Manifest=skipcheck" -e skipcheck

manifest :
	$(PERLRUN) "-MExtUtils::Manifest=mkmanifest" -e mkmanifest

veryclean : realclean
	$(RM_F) *~ */*~ *.orig */*.orig *.bak */*.bak *.old */*.old 



# --- MakeMaker dist_core section:

dist : $(DIST_DEFAULT) $(FIRST_MAKEFILE)
	$(NOECHO) $(ABSPERLRUN) -l -e 'print '\''Warning: Makefile possibly out of date with $(VERSION_FROM)'\''' \
	  -e '    if -e '\''$(VERSION_FROM)'\'' and -M '\''$(VERSION_FROM)'\'' < -M '\''$(FIRST_MAKEFILE)'\'';' --

tardist : $(DISTVNAME).tar$(SUFFIX)
	$(NOECHO) $(NOOP)

uutardist : $(DISTVNAME).tar$(SUFFIX)
	uuencode $(DISTVNAME).tar$(SUFFIX) $(DISTVNAME).tar$(SUFFIX) > $(DISTVNAME).tar$(SUFFIX)_uu

$(DISTVNAME).tar$(SUFFIX) : distdir
	$(PREOP)
	$(TO_UNIX)
	$(TAR) $(TARFLAGS) $(DISTVNAME).tar $(DISTVNAME)
	$(RM_RF) $(DISTVNAME)
	$(COMPRESS) $(DISTVNAME).tar
	$(POSTOP)

zipdist : $(DISTVNAME).zip
	$(NOECHO) $(NOOP)

$(DISTVNAME).zip : distdir
	$(PREOP)
	$(ZIP) $(ZIPFLAGS) $(DISTVNAME).zip $(DISTVNAME)
	$(RM_RF) $(DISTVNAME)
	$(POSTOP)

shdist : distdir
	$(PREOP)
	$(SHAR) $(DISTVNAME) > $(DISTVNAME).shar
	$(RM_RF) $(DISTVNAME)
	$(POSTOP)


# --- MakeMaker distdir section:
create_distdir :
	$(RM_RF) $(DISTVNAME)
	$(PERLRUN) "-MExtUtils::Manifest=manicopy,maniread" \
		-e "manicopy(maniread(),'$(DISTVNAME)', '$(DIST_CP)');"

distdir : create_distdir distmeta 
	$(NOECHO) $(NOOP)



# --- MakeMaker dist_test section:
disttest : distdir
	cd $(DISTVNAME) && $(ABSPERLRUN) Makefile.PL 
	cd $(DISTVNAME) && $(MAKE) $(PASTHRU)
	cd $(DISTVNAME) && $(MAKE) test $(PASTHRU)



# --- MakeMaker dist_ci section:

ci :
	$(PERLRUN) "-MExtUtils::Manifest=maniread" \
	  -e "@all = keys %{ maniread() };" \
	  -e "print(qq{Executing $(CI) @all\n}); system(qq{$(CI) @all});" \
	  -e "print(qq{Executing $(RCS_LABEL) ...\n}); system(qq{$(RCS_LABEL) @all});"


# --- MakeMaker distmeta section:
distmeta : create_distdir metafile
	$(NOECHO) cd $(DISTVNAME) && $(ABSPERLRUN) -MExtUtils::Manifest=maniadd -e 'eval { maniadd({q{META.yml} => q{Module meta-data (added by MakeMaker)}}) } ' \
	  -e '    or print "Could not add META.yml to MANIFEST: $${'\''@'\''}\n"' --



# --- MakeMaker distsignature section:
distsignature : create_distdir
	$(NOECHO) cd $(DISTVNAME) && $(ABSPERLRUN) -MExtUtils::Manifest=maniadd -e 'eval { maniadd({q{SIGNATURE} => q{Public-key signature (added by MakeMaker)}}) } ' \
	  -e '    or print "Could not add SIGNATURE to MANIFEST: $${'\''@'\''}\n"' --
	$(NOECHO) cd $(DISTVNAME) && $(TOUCH) SIGNATURE
	cd $(DISTVNAME) && cpansign -s



# --- MakeMaker install section:

install :: all pure_install doc_install
	$(NOECHO) $(NOOP)

install_perl :: all pure_perl_install doc_perl_install
	$(NOECHO) $(NOOP)

install_site :: all pure_site_install doc_site_install
	$(NOECHO) $(NOOP)

install_vendor :: all pure_vendor_install doc_vendor_install
	$(NOECHO) $(NOOP)

pure_install :: pure_$(INSTALLDIRS)_install
	$(NOECHO) $(NOOP)

doc_install :: doc_$(INSTALLDIRS)_install
	$(NOECHO) $(NOOP)

pure__install : pure_site_install
	$(NOECHO) $(ECHO) INSTALLDIRS not defined, defaulting to INSTALLDIRS=site

doc__install : doc_site_install
	$(NOECHO) $(ECHO) INSTALLDIRS not defined, defaulting to INSTALLDIRS=site

pure_perl_install ::
	$(NOECHO) $(MOD_INSTALL) \
		read $(PERL_ARCHLIB)/auto/$(FULLEXT)/.packlist \
		write $(DESTINSTALLARCHLIB)/auto/$(FULLEXT)/.packlist \
		$(INST_LIB) $(DESTINSTALLPRIVLIB) \
		$(INST_ARCHLIB) $(DESTINSTALLARCHLIB) \
		$(INST_BIN) $(DESTINSTALLBIN) \
		$(INST_SCRIPT) $(DESTINSTALLSCRIPT) \
		$(INST_MAN1DIR) $(DESTINSTALLMAN1DIR) \
		$(INST_MAN3DIR) $(DESTINSTALLMAN3DIR)
	$(NOECHO) $(WARN_IF_OLD_PACKLIST) \
		$(SITEARCHEXP)/auto/$(FULLEXT)


pure_site_install ::
	$(NOECHO) $(MOD_INSTALL) \
		read $(SITEARCHEXP)/auto/$(FULLEXT)/.packlist \
		write $(DESTINSTALLSITEARCH)/auto/$(FULLEXT)/.packlist \
		$(INST_LIB) $(DESTINSTALLSITELIB) \
		$(INST_ARCHLIB) $(DESTINSTALLSITEARCH) \
		$(INST_BIN) $(DESTINSTALLSITEBIN) \
		$(INST_SCRIPT) $(DESTINSTALLSITESCRIPT) \
		$(INST_MAN1DIR) $(DESTINSTALLSITEMAN1DIR) \
		$(INST_MAN3DIR) $(DESTINSTALLSITEMAN3DIR)
	$(NOECHO) $(WARN_IF_OLD_PACKLIST) \
		$(PERL_ARCHLIB)/auto/$(FULLEXT)

pure_vendor_install ::
	$(NOECHO) $(MOD_INSTALL) \
		read $(VENDORARCHEXP)/auto/$(FULLEXT)/.packlist \
		write $(DESTINSTALLVENDORARCH)/auto/$(FULLEXT)/.packlist \
		$(INST_LIB) $(DESTINSTALLVENDORLIB) \
		$(INST_ARCHLIB) $(DESTINSTALLVENDORARCH) \
		$(INST_BIN) $(DESTINSTALLVENDORBIN) \
		$(INST_SCRIPT) $(DESTINSTALLVENDORSCRIPT) \
		$(INST_MAN1DIR) $(DESTINSTALLVENDORMAN1DIR) \
		$(INST_MAN3DIR) $(DESTINSTALLVENDORMAN3DIR)

doc_perl_install ::
	$(NOECHO) $(ECHO) Appending installation info to $(DESTINSTALLARCHLIB)/perllocal.pod
	-$(NOECHO) $(MKPATH) $(DESTINSTALLARCHLIB)
	-$(NOECHO) $(DOC_INSTALL) \
		"Module" "$(NAME)" \
		"installed into" "$(INSTALLPRIVLIB)" \
		LINKTYPE "$(LINKTYPE)" \
		VERSION "$(VERSION)" \
		EXE_FILES "$(EXE_FILES)" \
		>> $(DESTINSTALLARCHLIB)/perllocal.pod

doc_site_install ::
	$(NOECHO) $(ECHO) Appending installation info to $(DESTINSTALLARCHLIB)/perllocal.pod
	-$(NOECHO) $(MKPATH) $(DESTINSTALLARCHLIB)
	-$(NOECHO) $(DOC_INSTALL) \
		"Module" "$(NAME)" \
		"installed into" "$(INSTALLSITELIB)" \
		LINKTYPE "$(LINKTYPE)" \
		VERSION "$(VERSION)" \
		EXE_FILES "$(EXE_FILES)" \
		>> $(DESTINSTALLARCHLIB)/perllocal.pod

doc_vendor_install ::
	$(NOECHO) $(ECHO) Appending installation info to $(DESTINSTALLARCHLIB)/perllocal.pod
	-$(NOECHO) $(MKPATH) $(DESTINSTALLARCHLIB)
	-$(NOECHO) $(DOC_INSTALL) \
		"Module" "$(NAME)" \
		"installed into" "$(INSTALLVENDORLIB)" \
		LINKTYPE "$(LINKTYPE)" \
		VERSION "$(VERSION)" \
		EXE_FILES "$(EXE_FILES)" \
		>> $(DESTINSTALLARCHLIB)/perllocal.pod


uninstall :: uninstall_from_$(INSTALLDIRS)dirs
	$(NOECHO) $(NOOP)

uninstall_from_perldirs ::
	$(NOECHO) $(UNINSTALL) $(PERL_ARCHLIB)/auto/$(FULLEXT)/.packlist

uninstall_from_sitedirs ::
	$(NOECHO) $(UNINSTALL) $(SITEARCHEXP)/auto/$(FULLEXT)/.packlist

uninstall_from_vendordirs ::
	$(NOECHO) $(UNINSTALL) $(VENDORARCHEXP)/auto/$(FULLEXT)/.packlist


# --- MakeMaker force section:
# Phony target to force checking subdirectories.
FORCE :
	$(NOECHO) $(NOOP)


# --- MakeMaker perldepend section:

PERL_HDRS = \
	$(PERL_INC)/EXTERN.h		\
	$(PERL_INC)/INTERN.h		\
	$(PERL_INC)/XSUB.h		\
	$(PERL_INC)/av.h		\
	$(PERL_INC)/cc_runtime.h	\
	$(PERL_INC)/config.h		\
	$(PERL_INC)/cop.h		\
	$(PERL_INC)/cv.h		\
	$(PERL_INC)/dosish.h		\
	$(PERL_INC)/embed.h		\
	$(PERL_INC)/embedvar.h		\
	$(PERL_INC)/fakethr.h		\
	$(PERL_INC)/form.h		\
	$(PERL_INC)/gv.h		\
	$(PERL_INC)/handy.h		\
	$(PERL_INC)/hv.h		\
	$(PERL_INC)/intrpvar.h		\
	$(PERL_INC)/iperlsys.h		\
	$(PERL_INC)/keywords.h		\
	$(PERL_INC)/mg.h		\
	$(PERL_INC)/nostdio.h		\
	$(PERL_INC)/op.h		\
	$(PERL_INC)/opcode.h		\
	$(PERL_INC)/patchlevel.h	\
	$(PERL_INC)/perl.h		\
	$(PERL_INC)/perlio.h		\
	$(PERL_INC)/perlsdio.h		\
	$(PERL_INC)/perlsfio.h		\
	$(PERL_INC)/perlvars.h		\
	$(PERL_INC)/perly.h		\
	$(PERL_INC)/pp.h		\
	$(PERL_INC)/pp_proto.h		\
	$(PERL_INC)/proto.h		\
	$(PERL_INC)/regcomp.h		\
	$(PERL_INC)/regexp.h		\
	$(PERL_INC)/regnodes.h		\
	$(PERL_INC)/scope.h		\
	$(PERL_INC)/sv.h		\
	$(PERL_INC)/thread.h		\
	$(PERL_INC)/unixish.h		\
	$(PERL_INC)/util.h

$(OBJECT) : $(PERL_HDRS)

DNS.c : $(XSUBPPDEPS)


# --- MakeMaker makefile section:

$(OBJECT) : $(FIRST_MAKEFILE)

# We take a very conservative approach here, but it's worth it.
# We move Makefile to Makefile.old here to avoid gnu make looping.
$(FIRST_MAKEFILE) : Makefile.PL $(CONFIGDEP)
	$(NOECHO) $(ECHO) "Makefile out-of-date with respect to $?"
	$(NOECHO) $(ECHO) "Cleaning current config before rebuilding Makefile..."
	-$(NOECHO) $(RM_F) $(MAKEFILE_OLD)
	-$(NOECHO) $(MV)   $(FIRST_MAKEFILE) $(MAKEFILE_OLD)
	- $(MAKE) $(USEMAKEFILE) $(MAKEFILE_OLD) clean $(DEV_NULL)
	$(PERLRUN) Makefile.PL 
	$(NOECHO) $(ECHO) "==> Your Makefile has been rebuilt. <=="
	$(NOECHO) $(ECHO) "==> Please rerun the $(MAKE) command.  <=="
	false



# --- MakeMaker staticmake section:

# --- MakeMaker makeaperl section ---
MAP_TARGET    = perl
FULLPERL      = /usr/bin/perl

$(MAP_TARGET) :: static $(MAKE_APERL_FILE)
	$(MAKE) $(USEMAKEFILE) $(MAKE_APERL_FILE) $@

$(MAKE_APERL_FILE) : $(FIRST_MAKEFILE) pm_to_blib
	$(NOECHO) $(ECHO) Writing \"$(MAKE_APERL_FILE)\" for this $(MAP_TARGET)
	$(NOECHO) $(PERLRUNINST) \
		Makefile.PL DIR= \
		MAKEFILE=$(MAKE_APERL_FILE) LINKTYPE=static \
		MAKEAPERL=1 NORECURS=1 CCCDLFLAGS=


# --- MakeMaker test section:

TEST_VERBOSE=0
TEST_TYPE=test_$(LINKTYPE)
TEST_FILE = test.pl
TEST_FILES = t/*.t
TESTDB_SW = -d

testdb :: testdb_$(LINKTYPE)

test :: $(TEST_TYPE) subdirs-test

subdirs-test ::
	$(NOECHO) $(NOOP)


test_dynamic :: pure_all
	PERL_DL_NONLAZY=1 $(FULLPERLRUN) "-MExtUtils::Command::MM" "-e" "test_harness($(TEST_VERBOSE), '$(INST_LIB)', '$(INST_ARCHLIB)')" $(TEST_FILES)

testdb_dynamic :: pure_all
	PERL_DL_NONLAZY=1 $(FULLPERLRUN) $(TESTDB_SW) "-I$(INST_LIB)" "-I$(INST_ARCHLIB)" $(TEST_FILE)

test_ : test_dynamic

test_static :: pure_all $(MAP_TARGET)
	PERL_DL_NONLAZY=1 ./$(MAP_TARGET) "-MExtUtils::Command::MM" "-e" "test_harness($(TEST_VERBOSE), '$(INST_LIB)', '$(INST_ARCHLIB)')" $(TEST_FILES)

testdb_static :: pure_all $(MAP_TARGET)
	PERL_DL_NONLAZY=1 ./$(MAP_TARGET) $(TESTDB_SW) "-I$(INST_LIB)" "-I$(INST_ARCHLIB)" $(TEST_FILE)



# --- MakeMaker ppd section:
# Creates a PPD (Perl Package Description) for a binary distribution.
ppd :
	$(NOECHO) $(ECHO) '<SOFTPKG NAME="$(DISTNAME)" VERSION="0,65,0,0">' > $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '    <TITLE>$(DISTNAME)</TITLE>' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '    <ABSTRACT>Perl DNS resolver module</ABSTRACT>' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '    <AUTHOR>Olaf Kolkman &lt;olaf@net-dns.org&gt;</AUTHOR>' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '    <IMPLEMENTATION>' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="Digest-HMAC_MD5" VERSION="1,0,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="Digest-MD5" VERSION="2,12,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="IO-Socket" VERSION="0,0,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="MIME-Base64" VERSION="2,11,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="Net-IP" VERSION="1,2,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <DEPENDENCY NAME="Test-More" VERSION="0,18,0,0" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <OS NAME="$(OSNAME)" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <ARCHITECTURE NAME="x86_64-linux-thread-multi-5.1" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '        <CODEBASE HREF="" />' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '    </IMPLEMENTATION>' >> $(DISTNAME).ppd
	$(NOECHO) $(ECHO) '</SOFTPKG>' >> $(DISTNAME).ppd


# --- MakeMaker pm_to_blib section:

pm_to_blib : $(TO_INST_PM)
	$(NOECHO) $(ABSPERLRUN) -MExtUtils::Install -e 'pm_to_blib({@ARGV}, '\''$(INST_LIB)/auto'\'', '\''$(PM_FILTER)'\'')' -- \
	  lib/Net/DNS/RR/A.pm blib/lib/Net/DNS/RR/A.pm \
	  lib/Net/DNS/Resolver/Win32.pm blib/lib/Net/DNS/Resolver/Win32.pm \
	  lib/Net/DNS/RR/Unknown.pm blib/lib/Net/DNS/RR/Unknown.pm \
	  lib/Net/DNS/RR/IPSECKEY.pm blib/lib/Net/DNS/RR/IPSECKEY.pm \
	  lib/Net/DNS/RR/EID.pm blib/lib/Net/DNS/RR/EID.pm \
	  lib/Net/DNS/RR/ISDN.pm blib/lib/Net/DNS/RR/ISDN.pm \
	  lib/Net/DNS/RR/MX.pm blib/lib/Net/DNS/RR/MX.pm \
	  lib/Net/DNS/Header.pm blib/lib/Net/DNS/Header.pm \
	  lib/Net/DNS/RR/SOA.pm blib/lib/Net/DNS/RR/SOA.pm \
	  lib/Net/DNS/RR/OPT.pm blib/lib/Net/DNS/RR/OPT.pm \
	  lib/Net/DNS/RR/NIMLOC.pm blib/lib/Net/DNS/RR/NIMLOC.pm \
	  lib/Net/DNS/RR/TXT.pm blib/lib/Net/DNS/RR/TXT.pm \
	  lib/Net/DNS/RR/DNAME.pm blib/lib/Net/DNS/RR/DNAME.pm \
	  lib/Net/DNS/RR/AAAA.pm blib/lib/Net/DNS/RR/AAAA.pm \
	  lib/Net/DNS.pm blib/lib/Net/DNS.pm \
	  lib/Net/DNS/RR/RP.pm blib/lib/Net/DNS/RR/RP.pm \
	  lib/Net/DNS/RR/X25.pm blib/lib/Net/DNS/RR/X25.pm \
	  lib/Net/DNS/Resolver/Cygwin.pm blib/lib/Net/DNS/Resolver/Cygwin.pm \
	  lib/Net/DNS/Resolver/Recurse.pm blib/lib/Net/DNS/Resolver/Recurse.pm \
	  lib/Net/DNS/RR/APL.pm blib/lib/Net/DNS/RR/APL.pm \
	  lib/Net/DNS/RR/PTR.pm blib/lib/Net/DNS/RR/PTR.pm \
	  lib/Net/DNS/RR/NS.pm blib/lib/Net/DNS/RR/NS.pm \
	  lib/Net/DNS/RR/TSIG.pm blib/lib/Net/DNS/RR/TSIG.pm \
	  lib/Net/DNS/Update.pm blib/lib/Net/DNS/Update.pm \
	  lib/Net/DNS/Resolver.pm blib/lib/Net/DNS/Resolver.pm \
	  lib/Net/DNS/RR/PX.pm blib/lib/Net/DNS/RR/PX.pm \
	  lib/Net/DNS/Nameserver.pm blib/lib/Net/DNS/Nameserver.pm \
	  lib/Net/DNS/RR/LOC.pm blib/lib/Net/DNS/RR/LOC.pm \
	  lib/Net/DNS/RR/NULL.pm blib/lib/Net/DNS/RR/NULL.pm \
	  lib/Net/DNS/RR/RT.pm blib/lib/Net/DNS/RR/RT.pm \
	  lib/Net/DNS/RR/SRV.pm blib/lib/Net/DNS/RR/SRV.pm \
	  lib/Net/DNS/RR/AFSDB.pm blib/lib/Net/DNS/RR/AFSDB.pm \
	  lib/Net/DNS/RR/MR.pm blib/lib/Net/DNS/RR/MR.pm \
	  lib/Net/DNS/RR/MINFO.pm blib/lib/Net/DNS/RR/MINFO.pm \
	  lib/Net/DNS/FAQ.pod blib/lib/Net/DNS/FAQ.pod \
	  lib/Net/DNS/RR/CNAME.pm blib/lib/Net/DNS/RR/CNAME.pm \
	  lib/Net/DNS/Question.pm blib/lib/Net/DNS/Question.pm \
	  lib/Net/DNS/RR/SSHFP.pm blib/lib/Net/DNS/RR/SSHFP.pm \
	  lib/Net/DNS/RR/NAPTR.pm blib/lib/Net/DNS/RR/NAPTR.pm \
	  lib/Net/DNS/RR/SPF.pm blib/lib/Net/DNS/RR/SPF.pm \
	  lib/Net/DNS/RR/APL/ApItem.pm blib/lib/Net/DNS/RR/APL/ApItem.pm \
	  lib/Net/DNS/Packet.pm blib/lib/Net/DNS/Packet.pm \
	  lib/Net/DNS/RR.pm blib/lib/Net/DNS/RR.pm \
	  lib/Net/DNS/RR/HINFO.pm blib/lib/Net/DNS/RR/HINFO.pm \
	  lib/Net/DNS/RR/MG.pm blib/lib/Net/DNS/RR/MG.pm \
	  lib/Net/DNS/RR/TKEY.pm blib/lib/Net/DNS/RR/TKEY.pm \
	  lib/Net/DNS/RR/NSAP.pm blib/lib/Net/DNS/RR/NSAP.pm \
	  lib/Net/DNS/RR/CERT.pm blib/lib/Net/DNS/RR/CERT.pm \
	  lib/Net/DNS/Resolver/UNIX.pm blib/lib/Net/DNS/Resolver/UNIX.pm \
	  lib/Net/DNS/RR/MB.pm blib/lib/Net/DNS/RR/MB.pm \
	  lib/Net/DNS/Resolver/Base.pm blib/lib/Net/DNS/Resolver/Base.pm 
	$(NOECHO) $(TOUCH) pm_to_blib


# --- MakeMaker selfdocument section:


# --- MakeMaker postamble section:

test_cover : pure_all
	cover -delete
	HARNESS_PERL_SWITCHES=-MDevel::Cover $(MAKE) test
	cover

netdns$(LIB_EXT): netdns$(OBJ_EXT)
	$(AR) cr netdns$(LIB_EXT) netdns$(OBJ_EXT)



# End.
