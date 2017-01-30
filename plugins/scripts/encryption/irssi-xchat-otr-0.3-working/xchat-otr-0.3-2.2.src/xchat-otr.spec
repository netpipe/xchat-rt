#
# spec file for package xchat-otr
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


Name:           xchat-otr
Version:        0.3
Release:        2.2
Summary:        Off-The-Record Messaging plugin for xchat
License:        GPL-2.0+
Group:          Productivity/Networking/Instant Messenger
Url:            http://irssi-otr.tuxfamily.org/
# compare irssi only fork https://github.com/cryptodotis/irssi-otr
Source0:        irssi-otr-%{version}.tar.bz2
Source1:        http://www.xchat.org/docs/xchat-plugin.h
Patch0:         irc-otr-build-xchat-0.3.patch
# PATCH-FIX-OPENSUSE irssi-otr-0.3-cmake-2.8.8-bug13125-fix.patch andreas.stieger@gmx.de
Patch1:         irssi-otr-0.3-cmake-2.8.8-bug13125-fix.patch
BuildRequires:  cmake >= 2.4.7
BuildRequires:  gcc-c++
BuildRequires:  glib2-devel >= 2.12
BuildRequires:  irssi-devel
BuildRequires:  pkg-config
BuildRequires:  python >= 2.5
Requires:       xchat
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
# Package does not build against libotr 4.0.0 in factory
%if 0%{?suse_version} > 1220
BuildRequires:  libotr2-devel >= 3.1.0
BuildConflicts:	libotr-devel >= 4.0.0
%else
BuildRequires:  libotr-devel >= 3.1.0
%endif

%description
This plugin adds Off-the-Record messaging support for the xchat IRC client.
Although primarily designed for use with the bitlbee IRC2IM gateway, it
works within any query window, provided that the conversation partner's IRC
client supports OTR.

OTR allows you to have private conversations over IM by providing:

 - Encryption
   - No one else can read your instant messages.
 - Authentication
   - You are assured the correspondent is who you think it is.
 - Deniability
   - The messages you send do _not_ have digital signatures that are
     checkable by a third party.  Anyone can forge messages after a
     conversation to make them look like they came from you.  However,
     _during_ a conversation, your correspondent is assured the messages
     he sees are authentic and unmodified.
 - Perfect forward secrecy
   - If you lose control of your private keys, no previous conversation
     is compromised.

%prep
%setup -q -n irssi-otr-%{version}
%patch0
%patch1
mkdir xchat
cp %{SOURCE1} xchat/
ln -s xchat/xchat-plugin.h .

%build
export CFLAGS="%{optflags}"
# FIXME: you should use %%cmake macros
cmake -DCMAKE_INSTALL_PREFIX="%{_prefix}" -DDOCDIR="%{_docdir}/%{name}" -DLIB_SUFFIX="$(echo %{_lib} | cut -b4-)" .
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install %{?_smp_mflags}
rm %{buildroot}/%{_libdir}/irssi/modules/libotr.so

%files
%defattr(-,root,root)
%doc LICENSE README README.xchat ChangeLog
%dir %{_libdir}/xchat
%dir %{_libdir}/xchat/plugins
%{_libdir}/xchat/plugins/libxchatotr.so

%changelog
* Fri Sep  5 2014 andreas.stieger@gmx.de
- explicity add LICENCE to %%doc previously included via wildcard
* Wed Sep  3 2014 andreas.stieger@gmx.de
- split xchat-otr from irc-otr source pcackage: irssi-otr has
  since moved on to 1.0.0 / libotr 4.0.0 and no longer contains
  xchat support.
* Sun Nov 11 2012 andreas.stieger@gmx.de
- The package currently does not build against libotr 4.0.0.
  For openSUSE Factory, build against older version of the library
  be provided by libotr2, [bnc#789175]
* Sat Apr 21 2012 andreas.stieger@gmx.de
- fix build with CMake 2.8.8, this is a CMake bug
  see http://www.cmake.org/Bug/view.php?id=13125
  This patch will not longer be required for CMake > 2.8.8
* Tue Mar 13 2012 andreas.stieger@gmx.de
- fix factory build
* Wed Nov 23 2011 andreas.stieger@gmx.de
- fix 64 bit build
* Tue Nov 22 2011 andreas.stieger@gmx.de
- initial package
