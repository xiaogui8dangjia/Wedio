# 基本配置
QT += core gui opengl openglwidgets widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3DModelingSoftware
TEMPLATE = app
CONFIG += c++17

# 源文件
SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/ThreeDCanvas.cpp \
    src/Camera.cpp \
    src/DockablePanel.cpp \
    src/PanelManager.cpp \
    src/RhinoButton.cpp

# 头文件
HEADERS += \
    include/MainWindow.h \
    include/ThreeDCanvas.h \
    include/Camera.h \
    include/DockablePanel.h \
    include/PanelManager.h \
    include/RhinoButton.h

# 资源文件
RESOURCES += src/resources/resources.qrc

# 包含路径
INCLUDEPATH += include

# 发布配置
win32 {
    # Windows特定设置
    RC_ICONS = src/resources/icons/app_icon.ico
}

# 链接库
LIBS += -lopengl32