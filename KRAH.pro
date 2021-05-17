QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:RC_ICONS += icon.ico

SOURCES += \
        cvgui.cpp \
        framegrabber.cpp \
        gamealgorithm.cpp \
        main.cpp \
        performance.cpp \
        puckdetector.cpp \
        puckpredictor.cpp \
        settings.cpp \
        trectdetector.cpp \
        udpsender.cpp


HEADERS += \
    cvgui.h \
    framegrabber.h \
    gamealgorithm.h \
    performance.h \
    puckdetector.h \
    puckpredictor.h \
    settings.h \
    sortcvpoints.h \
    trectdetector.h \
    udpsender.h

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

DISTFILES += \
    SetupScript.iss \
    basler_acA2000-165um_22729612.pfs \
    krah.ini \
    robot/program.as

