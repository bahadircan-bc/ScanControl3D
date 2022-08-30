QT       += core gui datavisualization widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lasercommunicator.cpp \
    main.cpp \
    mainwindow.cpp \
    serialcommunicator.cpp \
    visualizer.cpp

HEADERS += \
    lasercommunicator.h \
    mainwindow.h \
    serialcommunicator.h \
    visualizer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -L$$PWD/lib/ -lLLT

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

unix|win32: LIBS += -L$$PWD/lib/ -lOpen3D

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
