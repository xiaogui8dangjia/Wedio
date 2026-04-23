#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include <QCheckBox>
#include <QList>
#include <QString>
#include <QColor>
#include <QListWidget>
#include "ThreeDCanvas.h"
#include "RhinoButton.h"

class SettingsDialog;

class QTreeWidget;

namespace ads {
    class CDockManager;
    class CDockWidget;
}

// 图层数据结构（数据驱动架构的核心）
struct LayerData {
    QString name;
    bool isCurrent;
    bool isVisible;
    bool isLocked;
    QColor color;
    QString material;
    
    LayerData(const QString& n = "", bool cur = false, bool vis = true, 
              bool lock = false, const QColor& c = QColor("#95A5A6"), 
              const QString& mat = "默认")
        : name(n), isCurrent(cur), isVisible(vis), isLocked(lock), 
          color(c), material(mat) {}
};

// 布局常量
constexpr int INFO_HEIGHT = 26;
constexpr int MENU_BAR_HEIGHT = 25;
constexpr int TOOLBAR_HEIGHT = 30;
constexpr int COMMAND_MIN_HEIGHT = 85;
constexpr int TITLE_HEIGHT = 22;
constexpr int ROW_HEIGHT = 30;
constexpr int ORIGIN_ROW_HEIGHT = 26;
constexpr int SEPARATOR_HEIGHT = 9;
constexpr int PANEL_MARGIN = 8;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAbout();
    void onSettings();
    void onFileNameChanged(int index, const QString& fileName);
    void onClearRecentList();

private:
    void updateFileNamesFromSettings(SettingsDialog* dialog);
    void addToRecentList(const QString& filePath);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void setupUI();
    ThreeDCanvas* threeDCanvas;

    ads::CDockManager* DockManager;
    
    // 工具栏按钮
    QPushButton* btnEndpoint;
    QPushButton* btnNearest;
    QPushButton* btnPoint;
    QPushButton* btnMidpoint;
    QPushButton* btnCenter;
    QPushButton* btnIntersection;
    QPushButton* btnPerpendicular;
    QPushButton* btnTangent;
    QPushButton* btnQuarter;
    QPushButton* btnVertex;
    QPushButton* btnDisable;
    
    // 过滤器按钮
    QPushButton* btnPointFilter;
    QPushButton* btnCurveFilter;
    QPushButton* btnSurfaceFilter;
    QPushButton* btnMeshFilter;
    QPushButton* btnAnnotationFilter;
    QPushButton* btnOtherFilter;
    QPushButton* btnDisableFilter;

    // 文件管理面板
    ads::CDockWidget* fileDock;
    
    // 最近保存面板
    ads::CDockWidget* recentDock;
    QListWidget* recentList;
    
    // 按钮状态存储
    QMap<QPushButton*, bool> originalChecked;
    QMap<QPushButton*, bool> filterOriginalChecked;
    
    // 图层复选框
    QList<QCheckBox*> layerCheckboxes;
    
    // 图层数据列表（数据驱动架构的核心）
    QList<LayerData> m_layers;
    
    // 图层面板树控件
    QTreeWidget* m_layerTree = nullptr;
    
    // 刷新图层面板UI的方法
    void refreshLayerUI();
    
    // 图层面板右键菜单
    void showLayerContextMenu(const QPoint& pos);
    
    // 工具栏右键菜单
    void showToolbarContextMenu(QWidget* toolbarWidget, QHBoxLayout* toolbarLayout, const QPoint& pos);
};

#endif
