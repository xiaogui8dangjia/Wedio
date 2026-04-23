#include "PanelManager.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>

PanelManager* PanelManager::_instance = nullptr;

PanelManager* PanelManager::instance()
{
    if (!_instance) {
        _instance = new PanelManager();
    }
    return _instance;
}

void PanelManager::initialize(QWidget* parent)
{
    // 创建各个方向的容器
    leftPanelContainer = new QWidget(parent);
    leftLayout = new QVBoxLayout(leftPanelContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(2);
    
    rightPanelContainer = new QWidget(parent);
    rightLayout = new QVBoxLayout(rightPanelContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(2);
    
    topPanelContainer = new QWidget(parent);
    topLayout = new QHBoxLayout(topPanelContainer);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(2);
    
    bottomPanelContainer = new QWidget(parent);
    bottomLayout = new QHBoxLayout(bottomPanelContainer);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(2);
    
    centerContainer = new QWidget(parent);
    
    // 创建吸附提示窗口
    dropIndicatorWidget = new DropIndicatorWidget(parent);
}

void PanelManager::registerPanel(DockablePanel* panel, DockPosition defaultPosition)
{
    panels.append(panel);
    panelPositions[panel] = defaultPosition;
    
    // 连接信号
    connect(panel, &DockablePanel::dragStarted, this, &PanelManager::onDragStarted);
    connect(panel, &DockablePanel::requestShowDropIndicator, this, &PanelManager::showDropIndicator);
    connect(panel, &DockablePanel::requestHideDropIndicator, this, &PanelManager::hideDropIndicator);
    
    // 将面板添加到对应的容器
    movePanel(panel, defaultPosition);
}

void PanelManager::removePanel(DockablePanel* panel)
{
    panels.removeOne(panel);
    panelPositions.remove(panel);
}

void PanelManager::onDragStarted(DockablePanel* panel)
{
    currentDragPanel = panel;
}

void PanelManager::onDragFinished(DockablePanel* /* panel */)
{
    if (currentDragPanel) {
        DockPosition newPosition = calculateDropPosition(
            QCursor::pos(), currentDragPanel);
        
        if (newPosition != panelPositions[currentDragPanel]) {
            movePanel(currentDragPanel, newPosition);
            emit panelPositionChanged(currentDragPanel, newPosition);
        }
        
        currentDragPanel = nullptr;
    }
}

void PanelManager::showDropIndicator(QPoint globalPos, DockablePanel* sourcePanel)
{
    if (dropIndicatorWidget) {
        dropIndicatorWidget->showIndicator(globalPos, sourcePanel);
    }
}

void PanelManager::hideDropIndicator()
{
    if (dropIndicatorWidget) {
        dropIndicatorWidget->hideIndicator();
    }
}

DockPosition PanelManager::calculateDropPosition(QPoint globalPos, DockablePanel* sourcePanel)
{
    // 获取所有面板的几何信息
    QWidget* mainWidget = sourcePanel->window();
    QRect mainRect = mainWidget->rect();
    QPoint localPos = mainWidget->mapFromGlobal(globalPos);
    
    // 计算距离各个边缘的距离
    int distToLeft = localPos.x();
    int distToRight = mainRect.width() - localPos.x();
    int distToTop = localPos.y();
    int distToBottom = mainRect.height() - localPos.y();
    
    // 找到最近的边缘
    int minDist = qMin(qMin(distToLeft, distToRight), qMin(distToTop, distToBottom));
    
    // 判断是否在中心区域
    bool isInCenter = (localPos.x() > mainRect.width() * 0.25f && 
                       localPos.x() < mainRect.width() * 0.75f &&
                       localPos.y() > mainRect.height() * 0.25f &&
                       localPos.y() < mainRect.height() * 0.75f);
    
    if (isInCenter && minDist > 100) {
        return DockPosition::Floating;
    }
    
    if (minDist == distToLeft && distToLeft < 150) {
        return DockPosition::Left;
    } else if (minDist == distToRight && distToRight < 150) {
        return DockPosition::Right;
    } else if (minDist == distToTop && distToTop < 150) {
        return DockPosition::Top;
    } else if (minDist == distToBottom && distToBottom < 150) {
        return DockPosition::Bottom;
    }
    
    return DockPosition::Floating;
}

void PanelManager::movePanel(DockablePanel* panel, DockPosition newPosition)
{
    // 从旧容器中移除
    panel->setParent(nullptr);
    
    // 添加到新容器
    switch (newPosition) {
        case DockPosition::Left:
            leftLayout->addWidget(panel);
            break;
        case DockPosition::Right:
            rightLayout->addWidget(panel);
            break;
        case DockPosition::Top:
            topLayout->addWidget(panel);
            break;
        case DockPosition::Bottom:
            bottomLayout->addWidget(panel);
            break;
        case DockPosition::Floating:
            // 浮动模式需要特殊处理
            panel->setParent(centerContainer);
            break;
    }
    
    panelPositions[panel] = newPosition;
    panel->setDockPosition(newPosition);
    panel->show();
}

// DropIndicatorWidget 实现
DropIndicatorWidget::DropIndicatorWidget(QWidget* parent)
    : QWidget(parent), targetWidget(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    hide();
}

void DropIndicatorWidget::showIndicator(QPoint globalPos, DockablePanel* sourcePanel)
{
    targetWidget = sourcePanel->window();
    QRect targetRect = targetWidget->rect();
    QPoint targetGlobal = targetWidget->mapToGlobal(QPoint(0, 0));
    
    // 计算相对于目标窗口的位置
    QPoint localPos = targetWidget->mapFromGlobal(globalPos);
    
    // 计算距离各个边缘的距离
    int distToLeft = localPos.x();
    int distToRight = targetRect.width() - localPos.x();
    int distToTop = localPos.y();
    int distToBottom = targetRect.height() - localPos.y();
    
    // 找到最近的边缘
    (void)qMin(qMin(distToLeft, distToRight), qMin(distToTop, distToBottom));
    
    // 判断是否在中心区域
    (void)(localPos.x() > targetRect.width() * 0.25f && 
           localPos.x() < targetRect.width() * 0.75f &&
           localPos.y() > targetRect.height() * 0.25f &&
           localPos.y() < targetRect.height() * 0.75f);
    
    setGeometry(targetGlobal.x(), targetGlobal.y(), targetRect.width(), targetRect.height());
    show();
    raise();
}

void DropIndicatorWidget::hideIndicator()
{
    hide();
}

void DropIndicatorWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    if (!targetWidget) return;
    
    QPoint globalPos = QCursor::pos();
    QPoint localPos = targetWidget->mapFromGlobal(globalPos);
    QRect targetRect = rect();
    
    int distToLeft = localPos.x();
    int distToRight = targetRect.width() - localPos.x();
    int distToTop = localPos.y();
    int distToBottom = targetRect.height() - localPos.y();
    
    int minDist = qMin(qMin(distToLeft, distToRight), qMin(distToTop, distToBottom));
    
    bool isInCenter = (localPos.x() > targetRect.width() * 0.25f && 
                       localPos.x() < targetRect.width() * 0.75f &&
                       localPos.y() > targetRect.height() * 0.25f &&
                       localPos.y() < targetRect.height() * 0.75f);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor hoverColor(0, 120, 212, 80);
    QColor borderColor(0, 120, 212, 200);
    QBrush brush(hoverColor);
    QPen pen(borderColor, 2);
    
    painter.setBrush(brush);
    painter.setPen(pen);
    
    const int indicatorSize = 60;
    const int margin = 20;
    
    // 绘制左侧吸附提示
    if (distToLeft < 150) {
        QRect leftRect(margin, (height() - indicatorSize) / 2, indicatorSize / 2, indicatorSize);
        painter.drawRect(leftRect);
        
        // 绘制箭头
        QPainterPath arrowPath;
        arrowPath.moveTo(leftRect.right() + 5, leftRect.top() + indicatorSize / 4.0);
        arrowPath.lineTo(leftRect.right() + 15, leftRect.top() + indicatorSize / 2.0);
        arrowPath.lineTo(leftRect.right() + 5, leftRect.bottom() - indicatorSize / 4.0);
        arrowPath.closeSubpath();
        painter.fillPath(arrowPath, borderColor);
    }
    
    // 绘制右侧吸附提示
    if (distToRight < 150) {
        QRect rightRect(width() - margin - indicatorSize / 2, (height() - indicatorSize) / 2, indicatorSize / 2, indicatorSize);
        painter.drawRect(rightRect);
        
        // 绘制箭头
        QPainterPath arrowPath;
        arrowPath.moveTo(rightRect.left() - 5, rightRect.top() + indicatorSize / 4.0);
        arrowPath.lineTo(rightRect.left() - 15, rightRect.top() + indicatorSize / 2.0);
        arrowPath.lineTo(rightRect.left() - 5, rightRect.bottom() - indicatorSize / 4.0);
        arrowPath.closeSubpath();
        painter.fillPath(arrowPath, borderColor);
    }
    
    // 绘制顶部吸附提示
    if (distToTop < 150) {
        QRect topRect((width() - indicatorSize) / 2, margin, indicatorSize, indicatorSize / 2);
        painter.drawRect(topRect);
        
        // 绘制箭头
        QPainterPath arrowPath;
        arrowPath.moveTo(topRect.left() + indicatorSize / 4.0, topRect.bottom() + 5);
        arrowPath.lineTo(topRect.left() + indicatorSize / 2.0, topRect.bottom() + 15);
        arrowPath.lineTo(topRect.right() - indicatorSize / 4.0, topRect.bottom() + 5);
        arrowPath.closeSubpath();
        painter.fillPath(arrowPath, borderColor);
    }
    
    // 绘制底部吸附提示
    if (distToBottom < 150) {
        QRect bottomRect((width() - indicatorSize) / 2, height() - margin - indicatorSize / 2, indicatorSize, indicatorSize / 2);
        painter.drawRect(bottomRect);
        
        // 绘制箭头
        QPainterPath arrowPath;
        arrowPath.moveTo(bottomRect.left() + indicatorSize / 4.0, bottomRect.top() - 5);
        arrowPath.lineTo(bottomRect.left() + indicatorSize / 2.0, bottomRect.top() - 15);
        arrowPath.lineTo(bottomRect.right() - indicatorSize / 4.0, bottomRect.top() - 5);
        arrowPath.closeSubpath();
        painter.fillPath(arrowPath, borderColor);
    }
    
    // 绘制中心吸附提示
    if (isInCenter && minDist > 100) {
        QRect centerRect((width() - indicatorSize) / 2, (height() - indicatorSize) / 2, indicatorSize, indicatorSize);
        painter.drawRect(centerRect);
    }
}
