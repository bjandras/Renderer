
TARGET   = renderer
TEMPLATE = app

## Local user-specific configuration.
##
exists(local.pri) {
    include(local.pri) { message("Including local configuration file.") }
}

## Make sure WebKit qmake stuff is used.
##
QMAKEPATH = $$(QMAKEPATH)
isEmpty(QMAKEPATH) | !exists($${QMAKEPATH}/mkspecs) {
    error("The environment variable QMAKEPATH needs to point to $WEBKITSRC/Tools/qmake")
}

## Qt settings.
##
QT     += network webkit printsupport quick qml
CONFIG += debug no_keywords
WEBKIT += wtf webcore

## Custom WebKit
##
isEmpty(WEBKIT_DIR) {
    unix:  WEBKIT_DIR = /opt/WebKit
    win32: WEBKIT_DIR = C:/SDK/WebKit
}

## Load WebKit build system features.
##
ROOT_BUILD_DIR = $$WEBKIT_DIR
QMAKE_RPATHDIR = /usr/local/lib/renderer $${QMAKE_RPATHDIR}
load(features)


INCLUDEPATH += \
    /usr/local/include \
    $${WEBKIT_DIR}/include/QtWebKit \
    $${ROOT_WEBKIT_DIR}/Source/WTF

LIBS *= -L$${WEBKIT_DIR}/lib

LIBS += -L/usr/local/lib -lpoppler


SOURCES += \
    application.cpp \
    addressbar.cpp \
    findbar.cpp \
    mainwindow.cpp \
    webinspector.cpp \
    webpage.cpp \
    bookjs.cpp \
    jstext.cpp \
    pdf.cpp \
    renderer.cpp \
    main.cpp

HEADERS += \
    application.hpp \
    addressbar.hpp \
    findbar.hpp \
    mainwindow.hpp \
    webinspector.hpp \
    webpage.hpp \
    bookjs.hpp \
    jstext.hpp \
    pdf.hpp \
    renderer.hpp

RESOURCES += \
    renderer.qrc
