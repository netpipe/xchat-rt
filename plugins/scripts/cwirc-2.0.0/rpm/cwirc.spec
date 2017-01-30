%define name cwirc
%define version 2.0.0
%define release 1

Summary: X-Chat Morse plugin
Name: %{name}
Version: %{version}
Release: %{release}
Source: http://users.skynet.be/ppc/cwirc/download/%{name}-%{version}.tar.gz
License: GPL
Group: Networking/IRC
BuildRoot: %{_builddir}/%{name}-buildroot
Prefix: %{_prefix}
BuildRequires: libgtk+2.0_0-devel
Requires: xchat >= 2.0.2

%description
X-Chat plugin for sending and receiving raw morse code over IRC.

%prep
%setup -q

%build
make TARGET_OS=LINUX PLUGIN_INSTALL_DIRECTORY=dummy			\
	FRONTEND_INSTALL_DIRECTORY=dummy				\
	CWIRC_EXTENSIONS_DIRECTORY=%{_prefix}/lib/cwirc/extensions

%install
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/lib/xchat/plugins
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/bin
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/lib/cwirc/extensions
cp cwirc.so $RPM_BUILD_ROOT/%{_prefix}/lib/xchat/plugins
cp cwirc_frontend $RPM_BUILD_ROOT/%{_prefix}/bin

%clean
rm -rf $RPM_BUILD_ROOT

%post

%postun

%files
%defattr(-,root,root)
%{_prefix}/lib/xchat/plugins/cwirc.so
%{_prefix}/bin/cwirc_frontend
%{_prefix}/lib/cwirc/extensions
%doc README LISEZMOI Changelog COPYING schematics/cw_oscillator.jpg schematics/rs232_key_connection.jpg

%changelog
* Mon Jan 12 2004 P.P. Coupard <pcoupard@easyconnect.fr>
- First draft of the spec file
