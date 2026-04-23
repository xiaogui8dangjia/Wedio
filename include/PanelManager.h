#ifndef PANELMANAGER_H
#define PANELMANAGER_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QList>
#include <QPoint>
#include <QLabel>
#include "DockablePanel.h"

// 吸附提示窗口
class DropIndicatorWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit DropIndicatorWidget(QWidget* parent = nullptr);
    void showIndicator(QPoint globalPos, DockablePanel* sourcePanel);
    void hideIndicator();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    QLabel* topHint;
    QLabel* bottomHint;
    QLabel* leftHint;
    QLabel* rightHint;
    QLabel* centerHint;
    QWidget* targetWidget;
};

class PanelManager : public QObject {
    Q_OBJECT

public:
    static PanelManager* instance();
    
    void initialize(QWidget* parent);
    
    // 注册面板
    void registerPanel(DockablePanel* panel, DockPosition defaultPosition);
    
    // 移除面板
    void removePanel(DockablePanel* panel);
    
    // 获取所有面板
    QList<DockablePanel*> getAllPanels() const { return panels; }
    
    // 处理拖动开始
    void onDragStarted(DockablePanel* panel);
    
    // 处理拖动结束
    void onDragFinished(DockablePanel* panel);
    
    // 显示吸附提示
    void showDropIndicator(QPoint globalPos, DockablePanel* sourcePanel);
    
    // 隐藏吸附提示
    void hideDropIndicator();
    
    // 计算吸附位置
    DockPosition calculateDropPosition(QPoint globalPos, DockablePanel* sourcePanel);
    
    // 移动面板到指定位置
    void movePanel(DockablePanel* panel, DockPosition newPosition);

signals:
    void panelPositionChanged(DockablePanel* panel, DockPosition newPosition);

private:
    PanelManager() : dropIndicatorWidget(nullptr), currentDragPanel(nullptr) {}
    static PanelManager* _instance;
    
    QList<DockablePanel*> panels;
    QMap<DockablePanel*, DockPosition> panelPositions;
    
    // 左侧面板容器
    QWidget* leftPanelContainer;
    QVBoxLayout* leftLayout;
    
    // 右侧面板容器
    QWidget* rightPanelContainer;
    QVBoxLayout* rightLayout;
    
    // 顶部面板容器
    QWidget* topPanelContainer;
    QHBoxLayout* topLayout;
    
    // 底部面板容器
    QWidget* bottomPanelContainer;
    QHBoxLayout* bottomLayout;
    
    // 中心区域（浮动面板）
    QWidget* centerContainer;
    
    // 吸附提示窗口
    DropIndicatorWidget* dropIndicatorWidget;
    
    // 当前拖动的面板
    DockablePanel* currentDragPanel;
    
    // 目标面板（用于计算相对位置）
    DockablePanel* targetPanel;
};

#endif // PANELMANAGER_H
