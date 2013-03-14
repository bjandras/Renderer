
TARGET   = renderer
TEMPLATE = app

## Local user-specific configuration.
##
exists(local.pri) {
    include(local.pri) { message("Including local configuration file.") }
}

## Qt settings.
##
QT     += network printsupport
CONFIG += debug no_keywords

## WebKit
##
isEmpty(WEBKIT_DIR) {
    message("Using built-in WebKit.")
    QT     += webkit webkitwidgets
    WEBKIT += webcore wtf angle
} else {
    message("Using custom WebKit.")
    LIBS           += -L$${WEBKIT_DIR}/lib -lQt5WebKit -lQt5WebKitWidgets -lWebKit1 -lWebKit2 -lWebCore -lJavaScriptCore -lWTF -lANGLE
    INCLUDEPATH    += $${WEBKIT_DIR}/include/QtWebKit $${WEBKIT_DIR}/include/QtWebKitWidgets
    QMAKE_RPATHDIR  = $${WEBKIT_DIR}/lib $${QMAKE_RPATHDIR}
}


SOURCES += \
    application.cpp \
    addressbar.cpp \
    findbar.cpp \
    mainwindow.cpp \
    webinspector.cpp \
    webpage.cpp \
    bookjs.cpp \
    jstext.cpp \
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
    renderer.hpp

RESOURCES += \
    renderer.qrc

QMAKE_RPATHDIR = /usr/local/lib/renderer $${QMAKE_RPATHDIR}
