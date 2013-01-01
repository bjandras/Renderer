#!/bin/bash
#

# build directory
builddir=$PWD

if ! test -e $builddir/src/renderer; then
    echo "ERROR: the $builddir/renderer file is missing."
    exit 1
fi

destdir=$(mktemp --directory)
prefix=usr/local

# create directory structure
#
mkdir --parents $destdir/$prefix || exit 1
mkdir --parents $destdir/$prefix/bin
mkdir --parents $destdir/$prefix/lib/renderer

# webkit library file
libwebkit=$(ldd $builddir/src/renderer | grep libQtWebKit | ldd src/renderer | grep libQtWebKit  | cut -f 3 -d ' ')

# copy files
#
cp $libwebkit $destdir/$prefix/lib/renderer/
cp $builddir/src/renderer $destdir/$prefix/bin/
chmod a+x $destdir/$prefix/bin/renderer

tar -C $destdir -cvf - $prefix | bzip2 -c - > renderer.tar.bz2

echo "Removing $destdir"
rm -r $destdir
