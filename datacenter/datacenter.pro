QT       += core gui
QT       += serialport
QT       += network
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    diagnosis.cpp \
    main.cpp \
    mainface.cpp \
    parsedata.cpp \
    qcustomplot.cpp \
    serialthread.cpp \
    socketthread.cpp \
    sqlthread.cpp \
    widget.cpp

HEADERS += \
    diagnosis.h \
    mainface.h \
    parsedata.h \
    qcustomplot.h \
    serialthread.h \
    socketthread.h \
    sqlthread.h \
    widget.h

FORMS += \
    mainface.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
