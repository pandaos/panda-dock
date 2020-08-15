cmake_minimum_required(VERSION 3.5)
project(panda-dock)

set(TARGET panda-dock)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS main.cpp
               mainwindow.cpp
               mainpanel.cpp
               appscrollarea.cpp
               item/appitem.cpp
               item/blurwindow.cpp
               item/dockitem.cpp
               item/highlighteffect.cpp
               item/launcheritem.cpp
               item/trashitem.cpp
               utils/appwindowmanager.cpp
               utils/apprunthread.cpp
               utils/desktopproperties.cpp
               utils/dockitemmanager.cpp
               utils/docksettings.cpp
               utils/eventmonitor.cpp
               utils/utils.h
)

set(FORMS "")
set(RESOURCES resources.qrc)

set(QT Core Gui Widgets DBus Svg X11Extras)
find_package(Qt5 REQUIRED ${QT})
find_package(KF5WindowSystem)
find_package(X11 REQUIRED)

add_executable(${TARGET} ${SRCS} ${FORMS} ${RESOURCES})
target_link_libraries(${TARGET}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::X11Extras
        KF5::WindowSystem
        ${X11_LIBRARIES}
        ${XCOMPOSITE_LDFLAGS}
        ${XDAMAGE_LIBRARIES}
        ${XRENDER_LIBRARIES}
        -lxcb
        -lX11
        -lXtst
)

install(TARGETS ${TARGET} DESTINATION /usr/bin)
