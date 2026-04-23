#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <cmath>

class Camera
{
public:
    Camera();
    
    // 相机变换操作
    void rotate(float dx, float dy);
    void pan(float dx, float dy);
    void zoom(float delta);
    
    // 矩阵获取
    QMatrix4x4 getViewMatrix() const { return viewMatrix; }
    QMatrix4x4 getProjectionMatrix() const { return projectionMatrix; }
    
    // 状态获取
    QVector3D getPosition() const { return position; }
    QVector3D getTarget() const { return target; }
    QVector3D getUpVector() const { return up; }
    float getFov() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    
    // 状态设置
    void setPosition(const QVector3D& pos) { position = pos; updateViewMatrix(); }
    void setTarget(const QVector3D& tgt) { target = tgt; updateViewMatrix(); }
    void setUpVector(const QVector3D& upVec) { up = upVec; updateViewMatrix(); }
    void setFov(float f) { fov = f; updateProjectionMatrix(); }
    void setAspectRatio(float ratio) { aspectRatio = ratio; updateProjectionMatrix(); }
    void setViewport(int width, int height);
    
    // 重置相机
    void reset();
    
private:
    void updateViewMatrix();
    void updateProjectionMatrix();
    
private:
    // 相机参数
    QVector3D position;
    QVector3D target;
    QVector3D up;
    
    // 投影参数
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    // 矩阵缓存
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    
    // 旋转灵敏度
    float rotationSpeed;
    float panSpeed;
    float zoomSpeed;
};

#endif // CAMERA_H