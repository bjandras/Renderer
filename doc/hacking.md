
Compiling WebKit
================

The BookJS library at times uses features inside WebKit which are present only
in the development versions of WebKit.  To make use of this features you will
have to obtain and compile WebKit yourself from source.

Get the sources from WebKit's SVN repository (for example the chromium/1271
branch of WebKit, a.k.a. Chromium 23):

    svn checkout https://svn.webkit.org/repository/webkit/branches/chromium/1271 WebKit

This will check out the sources in the WebKit directory inside the working
directory.  From there on follow the build instructions for the Qt port of
WebKit (QtQebKit) which are available on http://trac.webkit.org/wiki/QtWebKit.
In a nutshell:

    export QT_SELECT=qt5
    WEBKITOUTPUTDIR=$PWD/WebKit-build WebKit/Tools/Scripts/build-webkit --qt --release

where WEBKITOUTPUTDIR specifies the output directory for the WebKit build.

To compile the Renderer with this custom build of WebKit:

    QT_SELECT=qt5 qmake WEBKIT_DIR=WebKit-build

substituting "WebKit-build" with the full path to the WebKit build directory.
