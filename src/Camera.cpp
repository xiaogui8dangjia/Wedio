#include "Camera.h"
#include <cmath>

Camera::Camera()
    : position(0.0f, 0.0f, 5.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , fov(45.0f)
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , rotationSpeed(0.5f)
    , panSpeed(0.01f)
    , zoomSpeed(0.1f)
{
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::rotate(float dx, float dy)
{
    // 计算相机到目标的向量
    QVector3D direction = (target - position).normalized();
    
    // 计算右向量和上向量
    QVector3D right = QVector3D::crossProduct(direction, up).normalized();
    QVector3D cameraUp = QVector3D::crossProduct(right, direction).normalized();
    
    // 绕上向量旋转（水平旋转）
    float horizontalAngle = dx * rotationSpeed;
    QQuaternion horizontalRotation = QQuaternion::fromAxisAndAngle(cameraUp, horizontalAngle);
    direction = horizontalRotation.rotatedVector(direction);
    
    // 绕右向量旋转（垂直旋转）
    float verticalAngle = dy * rotationSpeed;
    QQuaternion verticalRotation = QQuaternion::fromAxisAndAngle(right, verticalAngle);
    direction = verticalRotation.rotatedVector(direction);
    
    // 更新相机位置
    float distance = position.distanceToPoint(target);
    position = target - direction * distance;
    
    // 更新上向量
    up = cameraUp;
    
    updateViewMatrix();
}

void Camera::pan(float dx, float dy)
{
    // 计算相机到目标的向量
    QVector3D direction = (target - position).normalized();
    
    // 计算右向量和实际上向量
    QVector3D right = QVector3D::crossProduct(direction, up).normalized();
    QVector3D cameraUp = QVector3D::crossProduct(right, direction).normalized();
    
    // 计算平移量
    QVector3D translation = -right * dx * panSpeed + cameraUp * dy * panSpeed;
    
    // 更新相机位置和目标
    position += translation;
    target += translation;
    
    updateViewMatrix();
}

void Camera::zoom(float delta)
{
    // 计算相机到目标的向量
    QVector3D direction = (target - position).normalized();
    float distance = position.distanceToPoint(target);
    
    // 限制最小和最大距离
    float minDistance = 0.1f;
    float maxDistance = 100.0f;
    
    // 计算新的距离
    float newDistance = distance * (1.0f - delta * zoomSpeed);
    if (newDistance < minDistance) newDistance = minDistance;
    if (newDistance > maxDistance) newDistance = maxDistance;
    
    // 更新相机位置
    position = target - direction * newDistance;
    
    updateViewMatrix();
}

void Camera::setViewport(int width, int height)
{
    if (height > 0) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        updateProjectionMatrix();
    }
}

void Camera::reset()
{
    position = QVector3D(0.0f, 0.0f, 5.0f);
    target = QVector3D(0.0f, 0.0f, 0.0f);
    up = QVector3D(0.0f, 1.0f, 0.0f);
    fov = 45.0f;
    
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::updateViewMatrix()
{
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(position, target, up);
}

void Camera::updateProjectionMatrix()
{
    projectionMatrix.setToIdentity();
    float radians = fov * M_PI / 180.0f;
    projectionMatrix.perspective(radians * 180.0f / M_PI, aspectRatio, nearPlane, farPlane);
}