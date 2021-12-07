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
    traymenu.h \
    version.h

FORMS += \
    settingsdialog.ui

RESOURCES += \
    res.qrc

RC_ICONS = res/icon.ico

win* {
    LIBS += -lole32
}
