QT     += core gui widgets
CONFIG += c++20

SOURCES += \
    main.cpp \
    menubase.cpp \
    pathmenu.cpp \
    settingsdialog.cpp \
    traymenu.cpp

HEADERS += \
    menubase.h \
    pathmenu.h \
    settingsdialog.h \
    traymenu.h

FORMS += \
    settingsdialog.ui

RESOURCES += \
    res.qrc

RC_ICONS = res/icon.ico

VERSION = 0.1
VERSION_PARTS = $$split(VERSION, ".")

DEFINES += \
    VER_MINOR="$$member(VERSION_PARTS, 1)" \
    VER_MAJOR="$$member(VERSION_PARTS, 0)"

win* {
    LIBS += -lole32
}
