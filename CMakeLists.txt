cmake_minimum_required(VERSION 3.16)

project(MemoAssistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_SDK_DIR C:/Qt/6.9.0/msvc2022_64)
list(APPEND CMAKE_PREFIX_PATH ${QT_SDK_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets sql)

FILE(GLOB ORIGIN *.cpp *.h)
FILE(GLOB PAGES Pages/*.cpp Pages/*.h)
FILE(GLOB CUSTOMWIDGETS CustomWidgets/*.cpp CustomWidgets/*.h)

source_group(Pages FILES ${PAGES})
source_group(CustomWidgets FILES ${CUSTOMWIDGETS})

set(PROJECT_SOURCES
        ${ORIGIN}
        ${PAGES}
        ${CUSTOMWIDGETS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MemoAssistant
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/ElaWidgetTools.qrc
        res/res.qrc

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MemoAssistant APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MemoAssistant SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MemoAssistant
            ${PROJECT_SOURCES}
        )
    endif()
endif()
include_directories(
    include
)
target_link_libraries(MemoAssistant PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ElaWidgetTools.lib
    Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MemoAssistant)
endif()
set_target_properties(MemoAssistant PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MemoAssistant
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MemoAssistant)
endif()
