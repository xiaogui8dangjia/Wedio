#ifndef DOCKABLEPANEL_H
#define DOCKABLEPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>

// 面板位置枚举
enum class DockPosition {
    Left,
    Right,
    Top,
    Bottom,
    Floating
};

// 吸附方向枚举
enum class DropIndicator {
    None,
    Top,
    Bottom,
    Left,
    Right,
    Center
};

class DockablePanel : public QWidget {
    Q_OBJECT

public:
    explicit DockablePanel(const QString& title, QWidget* parent = nullptr);
    ~DockablePanel();

    // 获取面板内容区域
    QWidget* getContentWidget() const { return contentWidget; }
    
    // 获取面板标题
    QString getTitle() const { return titleLabel->text(); }
    
    // 设置面板位置
    void setDockPosition(DockPosition pos) { dockPosition = pos; }
    
    // 获取面板位置
    DockPosition getDockPosition() const { return dockPosition; }
    
    // 设置当前是否为拖动状态
    void setDragging(bool dragging) { isDragging = dragging; }
    
    // 获取是否为拖动状态
    bool isDraggingState() const { return isDragging; }
    
    // 设置吸附指示器
    void setDropIndicator(DropIndicator indicator) { dropIndicator = indicator; update(); }
    
    // 获取吸附指示器
    DropIndicator getDropIndicator() const { return dropIndicator; }

signals:
    // 面板开始拖动信号
    void dragStarted(DockablePanel* panel);
    
    // 面板拖动结束信号
    void dragFinished(DockablePanel* panel, DockPosition newPosition);
    
    // 请求显示吸附提示
    void requestShowDropIndicator(QPoint globalPos, DockablePanel* sourcePanel);
    
    // 请求隐藏吸附提示
    void requestHideDropIndicator();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    // 标题栏
    QWidget* titleBar;
    QLabel* titleLabel;
    QPushButton* closeButton;
    
    // 内容区域
    QWidget* contentWidget;
    QVBoxLayout* mainLayout;
    
    // 拖动相关
    bool isDragging;
    QPoint dragStartPos;
    QWidget* dragPreview;
    
    // 面板位置
    DockPosition dockPosition;
    
    // 吸附指示器
    DropIndicator dropIndicator;
};

#endif // DOCKABLEPANEL_H
