include (../../interfaces/interfaces.pri)

QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig

TARGET          = $$qtLibraryTarget(datetime)
#DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += datetime.json

HEADERS += \
    datetimeplugin.h

SOURCES += \
    datetimeplugin.cpp

target.path = $${PREFIX}/lib/flyma-taskbar/plugins/
INSTALLS += target
