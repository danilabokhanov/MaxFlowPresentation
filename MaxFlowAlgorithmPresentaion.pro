QT       += core gui

QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_LFLAGS += -fsanitize=address

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 warn_on

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Qwt
include($${OUT_PWD}/conanbuildinfo.pri)
INCLUDEPATH += $$CONAN_INCLUDEPATH_QWT
LIBS += $$CONAN_LIBDIRS_QWT $$CONAN_LIBS_QWT

SOURCES += \
    application.cpp \
    Kernel/controller.cpp \
    Interface/drawer.cpp \
    main.cpp \
    Interface/mainwindow.cpp \
    Kernel/max_flow.cpp \
    Kernel/view.cpp

HEADERS += \
    Kernel/mvc_messages.h \
    application.h \
    Kernel/controller.h \
    Interface/drawer.h \
    Interface/mainwindow.h \
    Kernel/max_flow.h \
    Library/observer_pattern.h \
    Kernel/view.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
