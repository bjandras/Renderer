#!/usr/bin/env python2

import os
import sys
import shutil
import subprocess


DEBIAN_CONTROL = """
Package: renderer
Version: 0.6-1
Section: universe/utils
Priority: extra
Architecture: i386
Depends: libc6 (>= 2.11), libstdc++6 (>= 4.6), libqt5core5 (>= 5.0.0), libqt5gui5 (>= 5.0.0), libqt5widgets5 (>= 5.0.0), libqt5network5 (>= 5.0.0), libqt5printsupport5 (>= 5.0.0), libqt5webkit5 (>= 5.0.0)
Suggests: xserver | xvfb
Homepage: https://github.com/bjandras/Renderer
Maintainer: Borko Jandras <borko.jandras@sourcefabric.org>
Description: HTML to PDF renderer using WebKit and BookJS.
 This is the Objavi Renderer, which converts HTML files to PDF.
 It is a part of the Objavi publishing engine (github.com/sourcefabric/Objavi).
 .
 To create the PDF output from HTML pages Objavi Render uses the BookJS
 library (github.com/sourcefabric/BookJS) and the WebKit layout engine.
 .
 This program requires an X11 server to run.
"""


def main():
    # create directory structure
    # copy built files to deb-build-directory
    # create DEBIAN/control file from the boilerplate
    # run dpkg-deb --build deb-build-directory
    pass


if __name__ == "__main__":
    try:
        main()
    except Exception, e:
        print >> sys.stderr, "ERROR:", str(e)
        sys.exit(-1)
