#include <QApplication>
#include <QSurfaceFormat>
#include <QFile>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // 设置OpenGL版本和配置
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4); // 4x多重采样抗锯齿
    QSurfaceFormat::setDefaultFormat(format);
    
    // 创建应用程序
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("3D建模软件");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Wedio");
    
    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 加载样式表
    QFile styleFile(":/style.css");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning("无法加载样式表文件");
    }
    
    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();
    
    // 运行应用程序
    return app.exec();
}