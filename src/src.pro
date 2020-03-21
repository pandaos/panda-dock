#-------------------------------------------------
#
# Project created by QtCreator 2020-01-27T23:22:13
#
#-------------------------------------------------

QT       += core gui svg x11extras KWindowSystem

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = panda-dock
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 link_pkgconfig
PKGCONFIG += xcb-ewmh x11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mainpanel.cpp \
    xcb/xcbmisc.cpp \
    item/dockitem.cpp \
    item/launcheritem.cpp \
    utils/imagefactory.cpp \
    utils/imageutil.cpp \
    utils/themeappicon.cpp \
    controller/dockitemmanager.cpp \
    components/hoverhighlighteffect.cpp \
    controller/appwindowmanager.cpp \
    item/appitem.cpp \
    controller/docksettings.cpp \
    utils/dockpopupwindow.cpp \
    item/showdesktopitem.cpp \
    appscrollarea.cpp

HEADERS += \
        mainwindow.h \
    mainpanel.h \
    xcb/xcbmisc.h \
    item/dockitem.h \
    item/launcheritem.h \
    utils/imagefactory.h \
    utils/imageutil.h \
    utils/themeappicon.h \
    utils/utils.h \
    controller/dockitemmanager.h \
    components/hoverhighlighteffect.h \
    controller/appwindowmanager.h \
    item/appitem.h \
    controller/docksettings.h \
    utils/dockpopupwindow.h \
    item/showdesktopitem.h \
    appscrollarea.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
