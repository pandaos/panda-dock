include (../../interfaces/interfaces.pri)

QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig pulse

TARGET          = $$qtLibraryTarget(volume)
#DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += volume.json

HEADERS += \
    volumeplugin.h \
    volumewidget.h

SOURCES += \
    volumeplugin.cpp \
    volumewidget.cpp

target.path = $${PREFIX}/lib/ibyte-dock/plugins/
INSTALLS += target
