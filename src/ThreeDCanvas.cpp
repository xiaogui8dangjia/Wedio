#include "ThreeDCanvas.h"
#include <QOpenGLShader>
#include <QOpenGLContext>
#include <QDebug>

ThreeDCanvas::ThreeDCanvas(QWidget* parent)
    : QOpenGLWidget(parent)
    , shaderProgram(nullptr)
    , backgroundColor(QColor(255, 255, 255))  // #FFFFFF
    , showCoordinateSystemFlag(true)
    , showGridFlag(true)
    , gridSpacing(1.0f)
    , isRotating(false)
    , isPanning(false)
    , coordinateVertexCount(0)
    , gridVertexCount(0)
{
    // 设置鼠标跟踪
    setMouseTracking(true);
    
    // 设置焦点策略
    setFocusPolicy(Qt::StrongFocus);
    
    // 设置Qt背景色，确保UI正确显示
    setStyleSheet("background-color: white;");
}

ThreeDCanvas::~ThreeDCanvas()
{
    makeCurrent();
    
    // 清理OpenGL资源
    coordinateVAO.destroy();
    gridVAO.destroy();
    coordinateVBO.destroy();
    gridVBO.destroy();
    
    delete shaderProgram;
    
    doneCurrent();
}

void ThreeDCanvas::setBackgroundColor(const QColor& color)
{
    backgroundColor = color;
    update();
}

void ThreeDCanvas::showCoordinateSystem(bool show)
{
    showCoordinateSystemFlag = show;
    update();
}

void ThreeDCanvas::showGrid(bool show)
{
    showGridFlag = show;
    update();
}

void ThreeDCanvas::rotateView(float dx, float dy)
{
    camera.rotate(dx, dy);
    update();
}

void ThreeDCanvas::panView(float dx, float dy)
{
    camera.pan(dx, dy);
    update();
}

void ThreeDCanvas::zoomView(float delta)
{
    camera.zoom(delta);
    update();
}

void ThreeDCanvas::resetView()
{
    camera.reset();
    update();
}

void ThreeDCanvas::initializeGL()
{
    // 初始化OpenGL函数
    initializeOpenGLFunctions();
    
    // 设置OpenGL状态
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
    // 初始化着色器
    initShaders();
    
    // 初始化坐标系
    initCoordinateSystem();
    
    // 初始化网格
    initGrid();
    
    // 设置相机视口
    camera.setViewport(width(), height());
}

void ThreeDCanvas::resizeGL(int w, int h)
{
    camera.setViewport(w, h);
}

void ThreeDCanvas::paintGL()
{
    // 清除缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 设置背景色
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);
    
    // 使用着色器程序
    shaderProgram->bind();
    
    // 设置视图和投影矩阵
    shaderProgram->setUniformValue("view", camera.getViewMatrix());
    shaderProgram->setUniformValue("projection", camera.getProjectionMatrix());
    
    // 渲染坐标系
    if (showCoordinateSystemFlag) {
        renderCoordinateSystem();
    }
    
    // 渲染网格
    if (showGridFlag) {
        renderGrid();
    }
    
    // 解绑着色器程序
    shaderProgram->release();
}

void ThreeDCanvas::mousePressEvent(QMouseEvent* event)
{
    lastMousePos = event->pos();
    
    if (event->button() == Qt::LeftButton) {
        isRotating = true;
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::MiddleButton) {
        isPanning = true;
        setCursor(Qt::SizeAllCursor);
    }
}

void ThreeDCanvas::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - lastMousePos;
    
    if (isRotating) {
        rotateView(delta.x(), delta.y());
    } else if (isPanning) {
        panView(delta.x(), delta.y());
    }
    
    lastMousePos = event->pos();
}

void ThreeDCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isRotating = false;
    } else if (event->button() == Qt::MiddleButton) {
        isPanning = false;
    }
    
    setCursor(Qt::ArrowCursor);
}

void ThreeDCanvas::wheelEvent(QWheelEvent* event)
{
    float delta = event->angleDelta().y() > 0 ? 1.0f : -1.0f;
    zoomView(delta);
    event->accept();
}

void ThreeDCanvas::initShaders()
{
    shaderProgram = new QOpenGLShaderProgram(this);
    
    // 加载顶点着色器
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic.vert")) {
        qDebug() << "顶点着色器编译错误:" << shaderProgram->log();
        return;
    }
    
    // 加载片段着色器
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic.frag")) {
        qDebug() << "片段着色器编译错误:" << shaderProgram->log();
        return;
    }
    
    // 链接着色器程序
    if (!shaderProgram->link()) {
        qDebug() << "着色器程序链接错误:" << shaderProgram->log();
        return;
    }
}

void ThreeDCanvas::initCoordinateSystem()
{
    // 创建坐标系顶点数据
    // X轴 (红色)
    // Y轴 (绿色)
    // Z轴 (蓝色)
    // 每个轴2个顶点，共6个顶点
    
    const int vertexCount = 6;
    const float axisLength = 2.0f;
    
    // 位置和颜色数据交错存储
    float vertices[] = {
        // X轴 (红色)
        0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,  // 起点
        axisLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 终点
        
        // Y轴 (绿色)
        0.0f, 0.0f, 0.0f,     0.0f, 1.0f, 0.0f,  // 起点
        0.0f, axisLength, 0.0f, 0.0f, 1.0f, 0.0f,  // 终点
        
        // Z轴 (蓝色)
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,  // 起点
        0.0f, 0.0f, axisLength, 0.0f, 0.0f, 1.0f,  // 终点
    };
    
    coordinateVertexCount = vertexCount;
    
    // 创建和绑定VAO
    coordinateVAO.create();
    coordinateVAO.bind();
    
    // 创建和绑定VBO
    coordinateVBO.create();
    coordinateVBO.bind();
    coordinateVBO.allocate(vertices, sizeof(vertices));
    
    // 设置顶点属性指针
    shaderProgram->bind();
    
    // 位置属性
    shaderProgram->enableAttributeArray(0);
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));
    
    // 颜色属性
    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));
    
    shaderProgram->release();
    coordinateVAO.release();
}

void ThreeDCanvas::initGrid()
{
    // 创建网格顶点数据
    // 网格大小为10x10，间距为gridSpacing
    
    const int gridSize = 10;  // 每边10条线
    const int lineCount = (gridSize + 1) * 2;  // X方向和Z方向各11条线
    const int vertexCount = lineCount * 2;  // 每条线2个顶点
    
    float vertices[vertexCount * 6];  // 每个顶点6个float (位置+颜色)
    int index = 0;
    
    float halfSize = gridSize * gridSpacing * 0.5f;
    float gridColor = 0.3f;  // 灰色
    
    // X方向的线 (平行于X轴)
    for (int i = 0; i <= gridSize; i++) {
        float z = -halfSize + i * gridSpacing;
        
        // 起点
        vertices[index++] = -halfSize;
        vertices[index++] = 0.0f;
        vertices[index++] = z;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        
        // 终点
        vertices[index++] = halfSize;
        vertices[index++] = 0.0f;
        vertices[index++] = z;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
    }
    
    // Z方向的线 (平行于Z轴)
    for (int i = 0; i <= gridSize; i++) {
        float x = -halfSize + i * gridSpacing;
        
        // 起点
        vertices[index++] = x;
        vertices[index++] = 0.0f;
        vertices[index++] = -halfSize;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        
        // 终点
        vertices[index++] = x;
        vertices[index++] = 0.0f;
        vertices[index++] = halfSize;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
        vertices[index++] = gridColor;
    }
    
    gridVertexCount = vertexCount;
    
    // 创建和绑定VAO
    gridVAO.create();
    gridVAO.bind();
    
    // 创建和绑定VBO
    gridVBO.create();
    gridVBO.bind();
    gridVBO.allocate(vertices, sizeof(vertices));
    
    // 设置顶点属性指针
    shaderProgram->bind();
    
    // 位置属性
    shaderProgram->enableAttributeArray(0);
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));
    
    // 颜色属性
    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));
    
    shaderProgram->release();
    gridVAO.release();
}

void ThreeDCanvas::renderCoordinateSystem()
{
    coordinateVAO.bind();
    shaderProgram->bind();
    
    // 设置模型矩阵为单位矩阵
    QMatrix4x4 model;
    model.setToIdentity();
    shaderProgram->setUniformValue("model", model);
    
    // 绘制坐标系
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, coordinateVertexCount);
    glLineWidth(1.0f);
    
    shaderProgram->release();
    coordinateVAO.release();
}

void ThreeDCanvas::renderGrid()
{
    gridVAO.bind();
    shaderProgram->bind();
    
    // 设置模型矩阵为单位矩阵
    QMatrix4x4 model;
    model.setToIdentity();
    shaderProgram->setUniformValue("model", model);
    
    // 绘制网格
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, gridVertexCount);
    
    shaderProgram->release();
    gridVAO.release();
}

void ThreeDCanvas::renderScene()
{
    // 这里可以添加其他3D对象的渲染
    // 目前只渲染坐标系和网格
}

QVector3D ThreeDCanvas::qColorToVector3D(const QColor& color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}