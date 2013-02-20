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

target=$builddir/src/renderer

version=$(xvfb-run $target -version | cut -f 3 -d ' ')
arch=$(xvfb-run $target -version | cut -f 4 -d ' ')

cp $target $destdir/$prefix/bin/
chmod a+x $destdir/$prefix/bin/renderer

# webkit library files
#
libs="libWebKit1 libQtWebKit libWebCore libJavaScriptCore libWTF libANGLE"
for lib in $libs; do
  src=$(ldd $builddir/src/renderer | grep $lib | cut -f 3 -d ' ')
  if test -n "$src"; then
    cp $src $destdir/$prefix/lib/renderer/
  fi
done

output_file="renderer-${version}-${arch}.tar.bz2"
tar -C $destdir -cvf - $prefix | bzip2 -c - > $output_file

echo "Removing $destdir"
rm -r $destdir

echo "Created $output_file"
