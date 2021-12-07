QT     += core gui widgets
CONFIG += c++20

SOURCES += \
    main.cpp \
    menubase.cpp \
    pathmenu.cpp \
    traymenu.cpp

HEADERS += \
    menubase.h \
    pathmenu.h \
    traymenu.h

win* {
    LIBS += -lole32
}
