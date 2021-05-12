QT       += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#TEMPLATE = lib
#CONFIG += plugin

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Common.cpp \
    addlc502dialog.cpp \
    caesclass.cpp \
    cipsetdialog.cpp \
    command.cpp \
    commframe.cpp \
    communication.cpp \
    crc16_sansi.cpp \
    cudpdataframe.cpp \
    lc502controller.cpp \
    main.cpp \
    mainwindow.cpp \
    mystruct.cpp \
    temp.cpp \
    udpthread.cpp
HEADERS += \
    Common.h \
    addlc502dialog.h \
    caesclass.h \
    cipsetdialog.h \
    command.h \
    commframe.h \
    communication.h \
    crc16_sansi.h \
    cudpdataframe.h \
    lc502controller.h \
    mainwindow.h \
    mystruct.h \
    temp.h \
    udpthread.h

FORMS += \
    addlc502dialog.ui \
    cipsetdialog.ui \
    mainwindow.ui \
    temp.ui

QT += widgets

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

INCLUDEPATH += $${PWD}/../lib/boost/include/boost-1_61#boost
LIBS        +=  -lwsock32
LIBS        +=  -lws2_32
LIBS        += -L"$${PWD}/../lib/boost/lib" -llibboost_system-mgw53-mt-1_61

INCLUDEPATH += ../toolkit
