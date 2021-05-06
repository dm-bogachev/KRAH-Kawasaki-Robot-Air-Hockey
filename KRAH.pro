QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        settings.cpp


HEADERS += \
    settings.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OpenCV Include and Library files
INCLUDEPATH += C:\opencv\msvc\include
CONFIG( debug, debug|release ) {
    LIBS += $$files(C:\opencv\msvc\lib\debug\*.lib)
} else {
    LIBS += $$files(C:\opencv\msvc\lib\release\*.lib)
}

