QT       += core gui

# QMAKE_CXXFLAGS += -fsanitize=address
QMAKE_CXXFLAGS += -std=c++20
# QMAKE_LFLAGS += -fsanitize=address

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 warn_on

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Qwt
include($${OUT_PWD}/conanbuildinfo.pri)
INCLUDEPATH += $$CONAN_INCLUDEPATH_QWT
LIBS += $$CONAN_LIBDIRS_QWT $$CONAN_LIBS_QWT

SOURCES += \
    Kernel/max_flow.cpp \
    Kernel/kernel_messages.cpp \
    Kernel/controller.cpp \
    Interface/geom_model.cpp \
    Interface/view.cpp \
    Interface/mainwindow.cpp \
    Interface/drawer.cpp \
    Interface/drawer_helper.cpp \
    application.cpp \
    main.cpp \

HEADERS += \
    Kernel/max_flow.h \
    Kernel/controller.h \
    Kernel/kernel_messages.h \
    Interface/geom_model.h \
    Interface/view.h \
    Interface/mainwindow.h \
    Interface/drawer.h \
    Interface/drawer_helper.h \
    application.h \
    Library/observer_pattern.h \
    Interface/interface_messages.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
