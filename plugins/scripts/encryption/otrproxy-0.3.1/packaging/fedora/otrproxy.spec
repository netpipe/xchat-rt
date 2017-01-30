Summary: Off-The-Record Messaging proxy server
Name: otrproxy
%define majver 0
%define minver 3.1
Version: %{majver}.%{minver}
%define debug_package %{nil}
%define ourrelease 1
Release: %{ourrelease}
Source: http://www.cypherpunks.ca/otr/otrproxy-%{majver}.%{minver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-buildroot
Url: http://www.cypherpunks.ca/otr/
Vendor: OTR Dev Team <otr@cypherpunks.ca>
Packager: Paul Wouters <paul@cypherpunks.ca>
License: GPL
Group: Applications/Internet
Provides: otrproxy
BuildRequires: libotr-devel >= 3.0.0, wxGTK >= 2.5
Requires: libotr >= 3.0.0
%define __spec_install_post /usr/lib/rpm/brp-compress || :

%description 

                     Off-the-Record Messaging Proxy
                       version 0.3.1,  2 Nov 2005

This is a localhost AIM proxy which implements Off-the-Record (OTR)
Messaging.  It allows you to use OTR with almost any IM client,
on Linux, OSX, Windows, and other platforms.

*** NOTE ***  This is a really early version of the proxy.  It has some
              known bugs, and is missing many features.  If you use it,
              please be prepared to give feedback to the otr-users
              mailing list.  You should certainly join that list and the
              otr-announce list.  See "MAILING LISTS" below for more
              information about the mailing lists.


For more information on Off-the-Record Messaging, see
http://www.cypherpunks.ca/otr/

%prep
%setup -q -n otrproxy-%{majver}.%{minver}

%build
%configure
%{__make} \
	CFLAGS="${RPM_OPT_FLAGS}" \
	all

%install
rm -rf ${RPM_BUILD_ROOT}
%{__make} \
	DESTDIR=${RPM_BUILD_ROOT} \
	install

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%attr(0755,root,root) %{_bindir}/otrproxy
%{_mandir}/man1/*
%doc README COPYING 

%changelog
* Fri Feb 25 2005 Paul Wouters <paul@cypherpunks.ca>
  - Updated for additional GUI requirements and configure
* Wed Jan 19 2005 Paul Wouters <paul@cypherpunks.ca>
- Initial version

