#
# sblim-cmpi-base-1.2.3.spec
#
# Package spec for CMPI Base
#

BuildRoot: /var/tmp/buildroot

Summary: SBLIM Base Instrumentation
Name: sblim-cmpi-base
Version: 1.2.3
Release: 3
Group: Systems Management/Base
Copyright: Common Public Licence http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
Packager: Viktor Mihajlovski <mihajlov@de.ibm.com>

BuildRequires: cmpi-devel

Source0: http://www-126-ibm.com/pub/sblim/sblim-cmpi-base/%{name}-%{version}-%{release}.tar.gz

%Description
Standards Based Linux Instrumentation Base Providers

%Package devel
Summary: SBLIM Base Instrumentation Header Development Files
Group: Systems Management/Base
Requires: %{name} = %{version}

%Description devel
SBLIM Base Provider Development Package


%prep

%setup -n %{name}-%{version}-%{release}

export PATCH_GET=0

#%patch0 -p0

%build

make PEGASUS= STANDALONE=1

%clean

if [ `id -ur` != 0 ]
then
# paranoia check 
	rm -rf $RPM_BUILD_ROOT 
fi

%install

mkdir -p $RPM_BUILD_ROOT/usr/lib/cmpi
mkdir -p $RPM_BUILD_ROOT/usr/include/cmpi
mkdir -p $RPM_BUILD_ROOT/usr/share/cmpi/mof
mkdir -p $RPM_BUILD_ROOT/usr/share/cmpi/tests/cim
mkdir -p $RPM_BUILD_ROOT/usr/share/cmpi/tests/system/linux

make install INSTALL_ROOT=$RPM_BUILD_ROOT PEGASUS= STANDALONE=1
make -C test install TESTSUITE=$RPM_BUILD_ROOT/usr/share/cmpi/tests

# SUSE specific
install -d $RPM_BUILD_ROOT/sbin/conf.d
install -m755 contrib/SuSEconfig.%{name} $RPM_BUILD_ROOT/sbin/conf.d

%files

%defattr(-,root,root) 
/usr/lib
/usr/share/cmpi

# SUSE specific
/sbin/conf.d

%files devel

%defattr(-,root,root)
/usr/include/cmpi

# SUSE specific too
%post
echo "#run me once" > /var/adm/SuSEconfig/run-%{name} || true
