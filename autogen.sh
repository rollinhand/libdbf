#!/bin/sh
#
# autogen.sh glue for CMU Cyrus IMAP
# $Id$
#
# Requires: automake, autoconf, dpkg-dev
set -e

# Check if needed software is available on system
for tool in aclocal autoheader automake libtoolize intltoolize autoconf; do
	if [ ! `whereis ${tool} | awk '{print $2}'` ] ; then
		echo "${tool} not found - please install first!"
		exit
	else 
		echo "${tool} found - OK!"		
	fi	
done

# Refresh GNU autotools toolchain.
# Test if /usr/share/automake exists, else prove for automake-$version
# This test-case is e.g. needed for SuSE distributions
automk="automake"
test ! -d /usr/share/automake && {
	version=`automake --version | head -n 1 | awk '{print $4}' | awk -F. '{print $1$2}'`
	automk="automake-${version}"
}
 	
for i in config.guess config.sub missing install-sh mkinstalldirs ; do
	test -r /usr/share/${automk}/${i} && {
		rm -f ${i}
		cp /usr/share/automake/${i} .
	}
	chmod 755 ${i}
done

aclocal
autoheader
automake --verbose --force --copy --add-missing
libtoolize --copy --force
intltoolize --copy --force
autoconf

# For the Debian build
test -d debian && {
	# Kill executable list first
	rm -f debian/executable.files

	# Make sure our executable and removable lists won't be screwed up
	debclean && echo Cleaned buildtree just in case...

	# refresh list of executable scripts, to avoid possible breakage if
	# upstream tarball does not include the file or if it is mispackaged
	# for whatever reason.
	echo Generating list of executable files...
	rm -f debian/executable.files
	find -type f -perm +111 ! -name '.*' -fprint debian/executable.files

	# link these in Debian builds
#	rm -f config.sub config.guess
#	ln -s /usr/share/misc/config.sub .
#	ln -s /usr/share/misc/config.guess .
}

exit 0
