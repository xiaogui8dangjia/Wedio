#ifndef THREEDCANVAS_H
#define THREEDCANVAS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColor>
#include "Camera.h"

class ThreeDCanvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    explicit ThreeDCanvas(QWidget* parent = nullptr);
    ~ThreeDCanvas();
    
    // 场景控制
    void setBackgroundColor(const QColor& color);
    void showCoordinateSystem(bool show);
    void showGrid(bool show);
    
    // 视图控制
    void rotateView(float dx, float dy);
    void panView(float dx, float dy);
    void zoomView(float delta);
    
    // 重置视图
    void resetView();
    
protected:
    // OpenGL相关
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    // 事件处理
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    // 初始化函数
    void initShaders();
    void initCoordinateSystem();
    void initGrid();
    
    // 渲染函数
    void renderCoordinateSystem();
    void renderGrid();
    void renderScene();
    
    // 工具函数
    QVector3D qColorToVector3D(const QColor& color);
    
private:
    // OpenGL对象
    QOpenGLShaderProgram* shaderProgram;
    QOpenGLBuffer coordinateVBO;
    QOpenGLBuffer gridVBO;
    QOpenGLVertexArrayObject coordinateVAO;
    QOpenGLVertexArrayObject gridVAO;
    
    // 相机
    Camera camera;
    
    // 场景状态
    QColor backgroundColor;
    bool showCoordinateSystemFlag;
    bool showGridFlag;
    float gridSpacing;
    
    // 鼠标交互状态
    QPoint lastMousePos;
    bool isRotating;
    bool isPanning;
    
    // 几何数据
    int coordinateVertexCount;
    int gridVertexCount;
};

#endif // THREEDCANVAS_H