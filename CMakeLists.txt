cmake_minimum_required(VERSION 3.5)
project(panda-dock)

set(TARGET panda-dock)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file (GLOB_RECURSE SRCS
  "${panda-dock_SOURCE_DIR}/item/*.cpp"
  "${panda-dock_SOURCE_DIR}/utils/*.cpp"
)

set(NEW_SOURCE main.cpp
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
               utils/desktopproperties.cpp
               utils/dockitemmanager.cpp
               utils/docksettings.cpp
               utils/eventmonitor.cpp
               utils/utils.h
)

set(SOURCES main.cpp mainpanel.cpp mainwindow.cpp appscrollarea.cpp ${SRCS})
set(HEADERS mainpanel.h mainwindow.h appscrollarea.h)
set(FORMS "")
set(RESOURCES resources.qrc)

set(QT Core Gui Widgets DBus Svg X11Extras)
find_package(Qt5 REQUIRED ${QT})
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
find_package(KF5 REQUIRED COMPONENTS WindowSystem)


# add_executable(${TARGET} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})
add_executable(${TARGET} ${NEW_SOURCE})

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
