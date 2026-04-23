cmake_minimum_required(VERSION 3.16)
project(Medio)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# IDE 需要知道这些包含路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Qt-Advanced-Docking-System-4.5.0/src
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(QT_DIR "D:/Qt/6.11.0/msvc2022_64")
    set(CMAKE_PREFIX_PATH ${QT_DIR} ${CMAKE_PREFIX_PATH})
    
    # 项目代码严格编译
    add_compile_options(/W4 /WX /permissive-)
    add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
else()
    set(QT_DIR "D:/Qt/6.11.0/mingw_64")
    include_directories(
        ${QT_DIR}/include
        ${QT_DIR}/include/QtCore
        ${QT_DIR}/include/QtGui
        ${QT_DIR}/include/QtWidgets
        ${QT_DIR}/include/QtOpenGL
        ${QT_DIR}/include/QtOpenGLWidgets
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(ADS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Qt-Advanced-Docking-System-4.5.0")
set(CMAKE_MODULE_PATH ${ADS_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
set(ADS_VERSION "4.5.0")
set(VERSION_SHORT "4.5.0")
set(VERSION_SONAME "4")
set(VERSION_MAJOR 4)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)
set(PROJECT_VERSION_MAJOR 4)
set(PROJECT_VERSION_MINOR 5)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_STRING "4.5.0")
set(PROJECT_GIT_HASH "e908062")
set(PROJECT_GIT_HASH_SHORT "e908062")
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ADS_DISABLE_VERSION_RESOURCES ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${ADS_DIR} ads_build)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ThreeDCanvas.cpp
    src/Camera.cpp
    src/DockablePanel.cpp
    src/PanelManager.cpp
    src/SettingsDialog.cpp
    src/RhinoButton.cpp
)

set(HEADERS
    include/MainWindow.h
    include/ThreeDCanvas.h
    include/Camera.h
    include/DockablePanel.h
    include/PanelManager.h
    include/SettingsDialog.h
    include/RhinoButton.h
)

qt6_standard_project_setup()

qt6_add_resources(RESOURCES src/resources/resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets ads::qtadvanceddocking-qt6)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Qt-Advanced-Docking-System-4.5.0/src)
