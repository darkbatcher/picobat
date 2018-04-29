#/bin/sh
#
# Generating all the autotools system, for dummies

echo "* Creating m4/ and config/"
mkdir -p m4
mkdir -p config
echo "* Setting up top-level dir"
echo "	touch config/config.rpath"
touch config/config.rpath
echo "	aclocal -I m4 --install"
aclocal -I m4 --install
echo "	autoheader"
autoheader
echo "  libtoolize --install"
libtoolize --install
echo "	automake --add-missing"
automake --add-missing
echo "	libtoolize --install"
libtoolize --install
echo "	autoconf"
autoconf
cd libmatheval
echo "* Setting up libmatheval/"
echo "	aclocal -I ../m4"
aclocal -I ../m4 --install
echo "	autoheader"
autoheader
echo "	automake --add-missing"
automake --add-missing
echo "	autoconf"
autoconf
cd ../libcu8
echo "* Setting up libcu8/"
echo "	aclocal -I ../m4"
aclocal -I ../m4 --install
echo "	automake --add-missing"
automake --add-missing
echo "	autoconf"
autoconf
cd ..
echo "* Building all Makefile.in files"
echo "	automake"
automake
