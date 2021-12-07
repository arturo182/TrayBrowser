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

win* {
    LIBS += -lole32
}
