#include "MainWindow.h"
#include "SettingsDialog.h"
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QMenu>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QToolBar>
#include <functional>
#include <QStatusBar>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QListWidget>
#include <QTreeWidget>
#include <QColorDialog>
#include <QRandomGenerator>
#include <QDateTime>
#include <QComboBox>
#include <QColor>
#include <functional>

#include <QFrame>

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockSplitter.h"
#include "IconProvider.h"
#include <QPainter>
#include <QEvent>
#include <QContextMenuEvent>

using namespace ads;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , threeDCanvas(nullptr)
    , DockManager(nullptr)
{
    setWindowTitle("Medio");
    resize(1280, 720);

    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, true);
    CDockManager::setConfigFlag(CDockManager::FloatingContainerHasWidgetTitle, true);
    CDockManager::setConfigFlag(CDockManager::FloatingContainerHasWidgetIcon, true);

    // 使用Qt内置图标
    QStyle* style = QApplication::style();

    // 注册自定义图标，确保按钮有图标显示
    CIconProvider& iconProvider = CDockManager::iconProvider();
    iconProvider.registerCustomIcon(ads::TabCloseIcon, style->standardIcon(QStyle::SP_TitleBarCloseButton));
    iconProvider.registerCustomIcon(ads::AutoHideIcon, style->standardIcon(QStyle::SP_TitleBarNormalButton));
    iconProvider.registerCustomIcon(ads::DockAreaMenuIcon, style->standardIcon(QStyle::SP_MessageBoxQuestion));
    iconProvider.registerCustomIcon(ads::DockAreaUndockIcon, style->standardIcon(QStyle::SP_TitleBarNormalButton));
    iconProvider.registerCustomIcon(ads::DockAreaCloseIcon, style->standardIcon(QStyle::SP_TitleBarCloseButton));
    iconProvider.registerCustomIcon(ads::DockAreaMinimizeIcon, style->standardIcon(QStyle::SP_TitleBarMinButton));

    // 创建DockManager
    DockManager = new CDockManager(this);

    // 隐藏Dock面板标题栏上的按钮（关闭、浮动、菜单等）
    DockManager->setStyleSheet(R"(
        #tabsMenuButton, #detachGroupButton, #dockAreaCloseButton, 
        #dockAreaAutoHideButton, #tabCloseButton {
            width: 0px;
            height: 0px;
            padding: 0px;
            margin: 0px;
            border: none;
            background: transparent;
        }
    )");

    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->setFixedHeight(25);
    menuBar->setStyleSheet(
        "QMenuBar::item { padding: 4px 10px; }"
        "QMenu { min-width: 150px; }"
        "QMenu::item { padding: 4px 20px; }"
    );
    setMenuBar(menuBar);

    CDockWidget* commandDock = new CDockWidget(DockManager, "命令栏");
    commandDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);

    QWidget* commandWidget = new QWidget(this);
    QVBoxLayout* commandLayout = new QVBoxLayout(commandWidget);
    commandLayout->setContentsMargins(1, 1, 1, 1);
    commandLayout->setSpacing(0);

    QLabel* commandOutput = new QLabel("命令执行结果显示区域", commandWidget);
    commandOutput->setFrameStyle(QFrame::Box);
    commandOutput->setMinimumSize(0, 0);

    commandOutput->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    commandOutput->setObjectName("commandOutput");

    QWidget* commandInputWidget = new QWidget(commandWidget);
    commandInputWidget->setFixedHeight(25);
    commandInputWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout* inputLayout = new QHBoxLayout(commandInputWidget);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(0);

    QLabel* commandLabel = new QLabel("命令:", commandInputWidget);
    QLineEdit* commandInput = new QLineEdit(commandInputWidget);
    commandInput->setPlaceholderText("输入命令...");

    inputLayout->addWidget(commandLabel);
    inputLayout->addWidget(commandInput);

    // 添加Rhino风格按钮工具栏
    QWidget* rhinoToolbarWidget = new QWidget(commandWidget);
    rhinoToolbarWidget->setFixedHeight(40);
    rhinoToolbarWidget->setMinimumWidth(400);
    rhinoToolbarWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    rhinoToolbarWidget->setObjectName("rhinoToolbar");
    
    // 启用右键菜单
    rhinoToolbarWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    QHBoxLayout* rhinoToolbarLayout = new QHBoxLayout(rhinoToolbarWidget);
    rhinoToolbarLayout->setContentsMargins(5, 5, 5, 5);
    rhinoToolbarLayout->setSpacing(5);
    rhinoToolbarLayout->setAlignment(Qt::AlignLeft);
    
    // 创建Rhino风格按钮
    RhinoButton* btnSelect = new RhinoButton("选择", rhinoToolbarWidget);
    btnSelect->setLeftClickAction("Select");
    btnSelect->setRightClickAction("SelectOptions");
    
    RhinoButton* btnMove = new RhinoButton("移动", rhinoToolbarWidget);
    btnMove->setLeftClickAction("Move");
    btnMove->setRightClickAction("MoveOptions");
    
    RhinoButton* btnRotate = new RhinoButton("旋转", rhinoToolbarWidget);
    btnRotate->setLeftClickAction("Rotate");
    btnRotate->setRightClickAction("RotateOptions");
    
    RhinoButton* btnScale = new RhinoButton("缩放", rhinoToolbarWidget);
    btnScale->setLeftClickAction("Scale");
    btnScale->setRightClickAction("ScaleOptions");
    
    RhinoButton* btnExtrude = new RhinoButton("挤出", rhinoToolbarWidget);
    btnExtrude->setLeftClickAction("Extrude");
    btnExtrude->setRightClickAction("ExtrudeOptions");
    
    RhinoButton* btnLoft = new RhinoButton("放样", rhinoToolbarWidget);
    btnLoft->setLeftClickAction("Loft");
    btnLoft->setRightClickAction("LoftOptions");
    
    // 添加按钮到工具栏
    rhinoToolbarLayout->addWidget(btnSelect);
    rhinoToolbarLayout->addWidget(btnMove);
    rhinoToolbarLayout->addWidget(btnRotate);
    rhinoToolbarLayout->addWidget(btnScale);
    rhinoToolbarLayout->addWidget(btnExtrude);
    rhinoToolbarLayout->addWidget(btnLoft);
    rhinoToolbarLayout->addStretch();

    // 连接工具栏右键菜单信号
    connect(rhinoToolbarWidget, &QWidget::customContextMenuRequested, this, [this, rhinoToolbarWidget, rhinoToolbarLayout](const QPoint& pos) {
        showToolbarContextMenu(rhinoToolbarWidget, rhinoToolbarLayout, pos);
    });

    commandLayout->addWidget(commandOutput, 1);
    commandLayout->addWidget(commandInputWidget);
    commandLayout->addWidget(rhinoToolbarWidget);
    commandLayout->setStretchFactor(commandOutput, 1);
    commandLayout->setStretchFactor(commandInputWidget, 0);
    commandLayout->setStretchFactor(rhinoToolbarWidget, 0);

    commandDock->setWidget(commandWidget);
    commandWidget->setMinimumHeight(85);
    commandWidget->setMinimumWidth(400);
    commandDock->setMinimumSize(400, 85);
    commandDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContentMinimumSize);

    threeDCanvas = new ThreeDCanvas(this);
    threeDCanvas->setMinimumSize(400, 300);
    
    // 创建环境标签组容器（三个标签共用一个外框）
    QWidget* envGroupWidget = new QWidget(threeDCanvas);
    envGroupWidget->setFixedHeight(INFO_HEIGHT);
    QHBoxLayout* envGroupLayout = new QHBoxLayout(envGroupWidget);
    envGroupLayout->setContentsMargins(0, 0, 0, 0);
    envGroupLayout->setSpacing(0);
    envGroupWidget->setStyleSheet("border: 1px solid #999; border-radius: 2px; padding: 0px;");
    
    // 创建标准标签
    QLabel* label2D = new QLabel("标准", envGroupWidget);
    label2D->setAlignment(Qt::AlignCenter);
    label2D->setCursor(Qt::PointingHandCursor);
    label2D->setObjectName("label2D");
    label2D->setStyleSheet("font-size: 12px; color: #fff; font-weight: bold; padding: 3px 8px; background: #4CAF50; border: none;");

    // 创建浮雕标签
    QLabel* labelRelief = new QLabel("浮雕", envGroupWidget);
    labelRelief->setAlignment(Qt::AlignCenter);
    labelRelief->setCursor(Qt::PointingHandCursor);
    labelRelief->setObjectName("labelRelief");
    labelRelief->setStyleSheet("font-size: 12px; color: #333; padding: 3px 8px; background: transparent; border: none;");

    envGroupLayout->addWidget(label2D);
    envGroupLayout->addWidget(labelRelief);
    
    // 创建双视窗标签（在环境标签组右边，间距 10）
    QLabel* labelDualView = new QLabel("双视窗", threeDCanvas);
    labelDualView->setFixedHeight(INFO_HEIGHT);
    labelDualView->setAlignment(Qt::AlignCenter);
    labelDualView->setCursor(Qt::PointingHandCursor);
    labelDualView->setObjectName("labelDualView");
    labelDualView->setStyleSheet("font-size: 12px; color: #333; padding: 3px 8px; background: transparent; border: 1px solid #999; border-radius: 2px;");
    
    // 创建信息显示框（在双视窗右边，间距 10）
    QWidget* infoWidget = new QWidget(threeDCanvas);
    infoWidget->setFixedHeight(INFO_HEIGHT);
    infoWidget->setObjectName("infoWidget");
    infoWidget->setStyleSheet(
        "QWidget#infoWidget { "
        "  background: rgba(255, 255, 255, 200); "
        "  border: 1px solid #ccc; "
        "  border-radius: 3px; "
        "} "
    );
    
    QHBoxLayout* infoLayout = new QHBoxLayout(infoWidget);
    infoLayout->setContentsMargins(8, 0, 8, 0);
    infoLayout->setSpacing(0);
    
    QLabel* infoLabel = new QLabel("已选 8 个对象||尺寸：X: 0, Y: 0, Z: 0", infoWidget);
    infoLabel->setStyleSheet("font-size: 12px; color: #333; background: transparent; border: none;");
    infoLayout->addWidget(infoLabel);
    
    // 根据文字内容调整宽度
    infoWidget->adjustSize();
    
    // 环境切换函数（只处理左边三个标签）
    auto switchEnvironment = [label2D, labelRelief](QLabel* selectedLabel) {
        // 重置所有环境标签样式（无边框，容器有边框）
        label2D->setStyleSheet("font-size: 12px; color: #333; padding: 3px 8px; background: transparent; border: none;");
        labelRelief->setStyleSheet("font-size: 12px; color: #333; padding: 3px 8px; background: transparent; border: none;");
        
        // 设置选中标签样式（绿色背景，无边框）
        selectedLabel->setStyleSheet("font-size: 12px; color: #fff; font-weight: bold; padding: 3px 8px; background: #4CAF50; border: none;");
    };
    
    // 双视窗切换函数（独立）
    auto switchDualView = [labelDualView](bool checked) {
        if (checked) {
            labelDualView->setStyleSheet("font-size: 12px; color: #fff; font-weight: bold; padding: 3px 8px; background: #4CAF50; border: 1px solid #4CAF50; border-radius: 2px;");
        } else {
            labelDualView->setStyleSheet("font-size: 12px; color: #333; padding: 3px 8px; background: transparent; border: 1px solid #999; border-radius: 2px;");
        }
    };
    
    // 点击事件过滤器
    class LabelClickFilter : public QObject {
    public:
        LabelClickFilter(QLabel* label, std::function<void(QLabel*)> onClick) 
            : targetLabel(label), clickHandler(onClick) {}
        
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                clickHandler(targetLabel);
                return true;
            }
            return QObject::eventFilter(obj, event);
        }
        
    private:
        QLabel* targetLabel;
        std::function<void(QLabel*)> clickHandler;
    };
    
    // 为每个标签添加点击事件
    LabelClickFilter* filter2D = new LabelClickFilter(label2D, switchEnvironment);
    filter2D->setParent(this);
    label2D->installEventFilter(filter2D);
    
    LabelClickFilter* filterRelief = new LabelClickFilter(labelRelief, switchEnvironment);
    filterRelief->setParent(this);
    labelRelief->installEventFilter(filterRelief);
    
    // 双视窗标签使用独立的点击事件（使用 QMouseEvent 过滤器）
    class DualViewClickFilter : public QObject {
    public:
        DualViewClickFilter(QLabel* label, std::function<void(bool)> onToggle) 
            : targetLabel(label), toggleHandler(onToggle), isChecked(false) {}
        
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                isChecked = !isChecked;
                toggleHandler(isChecked);
                return true;
            }
            return QObject::eventFilter(obj, event);
        }
        
    private:
        QLabel* targetLabel;
        std::function<void(bool)> toggleHandler;
        bool isChecked;
    };
    
    DualViewClickFilter* filterDualView = new DualViewClickFilter(labelDualView, switchDualView);
    filterDualView->setParent(this);
    labelDualView->installEventFilter(filterDualView);
    
    // 窗口初始化完成后再定位
    envGroupWidget->adjustSize();
    labelDualView->adjustSize();
    infoWidget->adjustSize();
    
    // 先临时定位到中间，窗口调整后会自动修正
    envGroupWidget->move((threeDCanvas->width() - envGroupWidget->width()) / 2, 2);
    labelDualView->move(envGroupWidget->x() + envGroupWidget->width() + 2, 2);
    infoWidget->move(labelDualView->x() + labelDualView->width() + 2, 2);
    
    // 监听尺寸变化，调整位置
    class EnvSwitchPositionFilter : public QObject {
    public:
        EnvSwitchPositionFilter(QWidget* envGroup, QWidget* dualView, QWidget* info) 
            : envGroupWidget(envGroup), dualViewWidget(dualView), infoWidget(info) {}
        
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override {
            if (event->type() == QEvent::Resize) {
                // 重新定位环境标签组到中间
                envGroupWidget->move((envGroupWidget->parentWidget()->width() - envGroupWidget->width()) / 2, 2);
                // 定位双视窗标签到环境标签组右边
        dualViewWidget->move(envGroupWidget->x() + envGroupWidget->width() + 2, 2);
        // 重新定位信息框
        infoWidget->move(dualViewWidget->x() + dualViewWidget->width() + 2, 2);
            }
            return QObject::eventFilter(obj, event);
        }
        
    private:
        QWidget* envGroupWidget;
        QWidget* dualViewWidget;
        QWidget* infoWidget;
    };
    
    EnvSwitchPositionFilter* filterEnvPos = new EnvSwitchPositionFilter(envGroupWidget, labelDualView, infoWidget);
    filterEnvPos->setParent(this);
    threeDCanvas->installEventFilter(filterEnvPos);
    


    CDockWidget* centralDock = new CDockWidget(DockManager, "CentralWidget");
    centralDock->setWidget(threeDCanvas);
    centralDock->setFeature(CDockWidget::NoTab, true);
    centralDock->setFeature(CDockWidget::DockWidgetClosable, false);
    centralDock->setFeature(CDockWidget::DockWidgetMovable, false);
    centralDock->setFeature(CDockWidget::DockWidgetFloatable, false);

    auto* CentralDockArea = DockManager->setCentralWidget(centralDock);
    CentralDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);

    DockManager->addDockWidget(DockWidgetArea::TopDockWidgetArea, commandDock, CentralDockArea);

    if (commandDock->dockAreaWidget()) {
        commandDock->dockAreaWidget()->titleBar()->setVisible(false);
    }

    // 创建参数面板
    CDockWidget* paramsDock = new CDockWidget(DockManager, "参数面板");
    paramsDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);
    
    QWidget* paramsWidget = new QWidget();
    QVBoxLayout* paramsLayout = new QVBoxLayout(paramsWidget);
    paramsLayout->setContentsMargins(5, 5, 5, 5);
    paramsLayout->setSpacing(5);
    
    QLabel* paramsLabel = new QLabel("参数设置", paramsWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLayout->addWidget(paramsLabel);
    
    // 添加一些示例参数控件
    for (int i = 1; i <= 5; ++i) {
        QWidget* paramItem = new QWidget(paramsWidget);
        QHBoxLayout* paramLayout = new QHBoxLayout(paramItem);
        paramLayout->setContentsMargins(0, 0, 0, 0);
        paramLayout->setSpacing(5);
        
        QLabel* paramName = new QLabel(QString("参数%1:").arg(i), paramItem);
        paramName->setFixedWidth(60);
        QLineEdit* paramValue = new QLineEdit(QString("值%1").arg(i), paramItem);
        
        paramLayout->addWidget(paramName);
        paramLayout->addWidget(paramValue);
        paramsLayout->addWidget(paramItem);
    }
    
    paramsDock->setWidget(paramsWidget);
    paramsWidget->setMinimumSize(50, 50);
    paramsDock->setMinimumSize(50, 50);
    paramsDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);

    // 创建文件保存面板
    fileDock = new CDockWidget(DockManager, "文件保存");
    fileDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);

    QWidget* fileWidget = new QWidget();
    QVBoxLayout* fileLayout = new QVBoxLayout(fileWidget);
    fileLayout->setContentsMargins(5, 5, 5, 5);
    fileLayout->setSpacing(5);
    fileLayout->setAlignment(Qt::AlignTop); // 内容从上往下排列，不要居中

    // 创建5个文件行
    for (int i = 1; i <= 5; i++) {
        QWidget* fileRow = new QWidget(fileWidget);
        fileRow->setFixedHeight(30);
        QHBoxLayout* rowLayout = new QHBoxLayout(fileRow);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(2);

        // 文件标签
        QLabel* fileLabel = new QLabel(QString("文件%1").arg(i), fileRow);
        fileLabel->setFixedWidth(40);
        rowLayout->addWidget(fileLabel);

        // 文本输入框
        QLineEdit* fileEdit = new QLineEdit(fileRow);
        fileEdit->setPlaceholderText(QString("输入文件%1的名称").arg(i));
        rowLayout->addWidget(fileEdit);

        // 粘贴按钮
        QPushButton* pasteBtn = new QPushButton("粘贴", fileRow);
        pasteBtn->setFixedSize(46, 24);
        pasteBtn->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");
        rowLayout->addWidget(pasteBtn);

        // 保存按钮
        QPushButton* saveBtn = new QPushButton("保存", fileRow);
        saveBtn->setFixedSize(46, 24);
        saveBtn->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");
        rowLayout->addWidget(saveBtn);

        fileLayout->addWidget(fileRow);
    }

    fileDock->setWidget(fileWidget);
    fileWidget->setMinimumSize(50, 50);
    fileDock->setMinimumSize(50, 50);
    fileDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);

    // 创建最近保存面板
    recentDock = new CDockWidget(DockManager, "最近保存");
    recentDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);

    QWidget* recentWidget = new QWidget();
    QVBoxLayout* recentLayout = new QVBoxLayout(recentWidget);
    recentLayout->setContentsMargins(5, 5, 5, 5);
    recentLayout->setSpacing(5);
    recentLayout->setAlignment(Qt::AlignTop); // 内容从上往下排列

    // 创建最近文件列表
    recentList = new QListWidget(recentWidget);
    recentList->setMaximumHeight(200); // 限制最大高度
    recentList->setAlternatingRowColors(true); // 交替行颜色
    recentLayout->addWidget(recentList);

    // 添加清空按钮
    QPushButton* clearButton = new QPushButton("清空列表", recentWidget);
    clearButton->setFixedHeight(25);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearRecentList);
    recentLayout->addWidget(clearButton);

    recentDock->setWidget(recentWidget);
    recentWidget->setMinimumSize(50, 50);
    recentDock->setMinimumSize(50, 50);
    recentDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);

    // 创建变换面板
    CDockWidget* propsDock = new CDockWidget(DockManager, "变换属性");
    propsDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);

    QWidget* propsWidget = new QWidget();
    QVBoxLayout* propsLayout = new QVBoxLayout(propsWidget);
    propsLayout->setContentsMargins(4, 4, 4, 4);
    propsLayout->setSpacing(0);

    // 创建坐标输入行
    auto createCoordRow = [](const QString& label, QWidget* parent) -> QWidget* {
        QWidget* row = new QWidget(parent);
        row->setFixedHeight(30);
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 2, 0, 2);
        rowLayout->setSpacing(2);
        rowLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel* lbl = new QLabel(label, row);
        lbl->setFixedWidth(14);
        lbl->setStyleSheet("font-size: 16px;");

        QDoubleSpinBox* spin = new QDoubleSpinBox(row);
        spin->setRange(-99999, 99999);
        spin->setValue(0);
        spin->setDecimals(2);
        spin->setFixedWidth(80);
        spin->setFixedHeight(26);
        spin->setStyleSheet("font-size: 14px;");
        spin->setButtonSymbols(QAbstractSpinBox::PlusMinus);

        QCheckBox* check = new QCheckBox("等比", row);
        check->setFixedHeight(26);
        QCheckBox* lock = new QCheckBox("锁定", row);
        lock->setFixedHeight(26);

        rowLayout->addWidget(lbl);
        rowLayout->addWidget(spin);
        rowLayout->addWidget(check);
        rowLayout->addWidget(lock);
        rowLayout->addStretch();

        return row;
    };

    propsLayout->addWidget(createCoordRow("X:", propsWidget));
    propsLayout->addWidget(createCoordRow("Y:", propsWidget));
    propsLayout->addWidget(createCoordRow("Z:", propsWidget));

    // 分隔线
    QFrame* separator = new QFrame(propsWidget);
    separator->setFixedHeight(1);
    separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("border: 1px solid #ccc; margin: 4px 0;");
    propsLayout->addWidget(separator);

    // 原点位置标题
    QLabel* originTitle = new QLabel("原点位置", propsWidget);
    originTitle->setFixedHeight(18);
    originTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    originTitle->setStyleSheet("font-weight: bold; font-size: 14px; padding: 0px 0;");
    propsLayout->addWidget(originTitle);

    // 创建原点位置行
    auto createOriginRow = [](const QString& label, QWidget* parent) -> QWidget* {
        QWidget* row = new QWidget(parent);
        row->setFixedHeight(26);
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QHBoxLayout* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(2);
        rowLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel* lbl = new QLabel(label, row);
        lbl->setFixedWidth(16);
        lbl->setStyleSheet("font-size: 14px;");

        QButtonGroup* group = new QButtonGroup(row);

        QRadioButton* rbMin = new QRadioButton("最小", row);
        rbMin->setChecked(true);
        rbMin->setFixedHeight(24);
        rbMin->setStyleSheet("font-size: 14px;");
        QRadioButton* rbCenter = new QRadioButton("中心点", row);
        rbCenter->setFixedHeight(24);
        rbCenter->setStyleSheet("font-size: 14px;");
        QRadioButton* rbMax = new QRadioButton("最大", row);
        rbMax->setFixedHeight(24);
        rbMax->setStyleSheet("font-size: 14px;");

        group->addButton(rbMin, 0);
        group->addButton(rbCenter, 1);
        group->addButton(rbMax, 2);

        rowLayout->addWidget(lbl);
        rowLayout->addWidget(rbMin);
        rowLayout->addWidget(rbCenter);
        rowLayout->addWidget(rbMax);
        rowLayout->addStretch();

        return row;
    };

    propsLayout->addWidget(createOriginRow("X:", propsWidget));
    propsLayout->addWidget(createOriginRow("Y:", propsWidget));
    propsLayout->addWidget(createOriginRow("Z:", propsWidget));

    // 计算内容固定高度
    int contentHeight = TITLE_HEIGHT + ROW_HEIGHT * 3 + SEPARATOR_HEIGHT + TITLE_HEIGHT + ORIGIN_ROW_HEIGHT * 3 + PANEL_MARGIN;
    propsWidget->setFixedHeight(contentHeight);
    propsWidget->setMaximumHeight(contentHeight);

    propsDock->setWidget(propsWidget);
    propsDock->setMinimumSize(50, contentHeight);
    propsDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);
    
    // 创建图层面板
    CDockWidget* layersDock = new CDockWidget(DockManager, "图层面板");
    layersDock->setFeatures(CDockWidget::DockWidgetClosable | CDockWidget::DockWidgetMovable | CDockWidget::DockWidgetFloatable);

    QWidget* layersWidget = new QWidget();
    QVBoxLayout* layersLayout = new QVBoxLayout(layersWidget);
    layersLayout->setContentsMargins(2, 2, 2, 2);
    layersLayout->setSpacing(0);

    // 工具栏按钮区域
    QHBoxLayout* toolLayout = new QHBoxLayout();
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(0);

    QPushButton* btnLayerNew = new QPushButton("新建", layersWidget);
    btnLayerNew->setFixedSize(48, 24);
    btnLayerNew->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");
    QPushButton* btnLayerDelete = new QPushButton("删除", layersWidget);
    btnLayerDelete->setFixedSize(48, 24);
    btnLayerDelete->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");
    QPushButton* btnLayerUp = new QPushButton("向上", layersWidget);
    btnLayerUp->setFixedSize(48, 24);
    btnLayerUp->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");
    QPushButton* btnLayerDown = new QPushButton("向下", layersWidget);
    btnLayerDown->setFixedSize(48, 24);
    btnLayerDown->setStyleSheet("text-align: center; padding: 0px; font-size: 12px;");

    toolLayout->addWidget(btnLayerNew);
    toolLayout->addWidget(btnLayerDelete);
    toolLayout->addWidget(btnLayerUp);
    toolLayout->addWidget(btnLayerDown);
    toolLayout->addStretch();

    layersLayout->addLayout(toolLayout);

    // 图层网格
    QTreeWidget* layerTree = new QTreeWidget(layersWidget);
    layerTree->setHeaderLabels(QStringList() << "名称" << "当前" << "可见" << "锁定" << "颜色" << "材质");
    layerTree->setColumnWidth(0, 40);
    layerTree->setColumnWidth(1, 35);
    layerTree->setColumnWidth(2, 35);
    layerTree->setColumnWidth(3, 35);
    layerTree->setColumnWidth(4, 35);
    layerTree->setColumnWidth(5, 45);
    layerTree->setAlternatingRowColors(true);
    layerTree->setRootIsDecorated(false);
    layerTree->setEditTriggers(QTreeWidget::DoubleClicked | QTreeWidget::EditKeyPressed);
    // 设置选择模式为整行选择
    layerTree->setSelectionMode(QAbstractItemView::SingleSelection);
    layerTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    layerTree->setStyleSheet(R"(
        QTreeWidget {
            border: 1px solid #ccc;
            background: white;
            show-decoration-selected: 0;
            outline: 0;
        }
        QTreeWidget::item {
            height: 28px;
            border: none;
            outline: none;
        }
        QTreeWidget::item:hover {
            background: #c3f8c3;
            border: none;
            outline: none;
        }
        QTreeWidget::item:selected {
            background: #4CAF50;
            color: white;
            border: none;
            outline: none;
        }
        QTreeWidget::item:selected:!active {
            background: #4CAF50;
            border: none;
            outline: none;
        }
        QTreeWidget::item:focus {
            border: none;
            outline: none;
        }
        QHeaderView::section {
            background: #f0f0f0;
            border: 1px solid #ccc;
            padding: 4px;
            font-size: 12px;
        }
    )");

    // ========== 数据驱动架构：初始化图层 ==========
    m_layers.append(LayerData("默认", true, true, false, QColor("#3498DB"), "默认"));
    m_layers.append(LayerData("图层1", false, true, false, QColor("#E74C3C"), "金属"));
    m_layers.append(LayerData("图层2", false, false, true, QColor("#2ECC71"), "塑料"));

    layersLayout->addWidget(layerTree);

    // 保存layerTree到成员变量（必须在refreshLayerUI之前）
    m_layerTree = layerTree;

    // 初始刷新UI
    refreshLayerUI();

    // 为按钮添加功能（只修改数据模型，然后调用refreshLayerUI）
    connect(btnLayerNew, &QPushButton::clicked, this, [this]() {
        int count = m_layers.size();
        QString name = QString("图层%1").arg(count);
        
        // 生成随机颜色
        QRandomGenerator generator(QDateTime::currentMSecsSinceEpoch());
        int r = generator.bounded(50, 200);  // 避免太亮或太暗的颜色
        int g = generator.bounded(50, 200);
        int b = generator.bounded(50, 200);
        QColor randomColor(r, g, b);
        
        m_layers.append(LayerData(name, false, true, false, randomColor, "默认"));
        refreshLayerUI();
    });

    connect(btnLayerDelete, &QPushButton::clicked, this, [this]() {
        QTreeWidgetItem* currentItem = m_layerTree->currentItem();
        if (!currentItem || m_layers.size() <= 1) return;
        
        int index = m_layerTree->indexOfTopLevelItem(currentItem);
        if (index >= 0 && index < m_layers.size()) {
            m_layers.removeAt(index);
            refreshLayerUI();
        }
    });

    connect(btnLayerUp, &QPushButton::clicked, this, [this]() {
        QTreeWidgetItem* currentItem = m_layerTree->currentItem();
        if (!currentItem) return;
        
        int index = m_layerTree->indexOfTopLevelItem(currentItem);
        if (index <= 0) return;
        
        // 只交换数据（安全！不涉及widget移动）
        std::swap(m_layers[index], m_layers[index - 1]);
        
        // 刷新UI（完全重建，保证一致性）
        refreshLayerUI();
        
        // 保持选中状态
        if (m_layerTree->topLevelItemCount() > index - 1) {
            m_layerTree->setCurrentItem(m_layerTree->topLevelItem(index - 1));
        }
    });

    connect(btnLayerDown, &QPushButton::clicked, this, [this]() {
        QTreeWidgetItem* currentItem = m_layerTree->currentItem();
        if (!currentItem) return;
        
        int index = m_layerTree->indexOfTopLevelItem(currentItem);
        if (index >= m_layers.size() - 1) return;
        
        // 只交换数据（安全！不涉及widget移动）
        std::swap(m_layers[index], m_layers[index + 1]);
        
        // 刷新UI（完全重建，保证一致性）
        refreshLayerUI();
        
        // 保持选中状态
        if (m_layerTree->topLevelItemCount() > index + 1) {
            m_layerTree->setCurrentItem(m_layerTree->topLevelItem(index + 1));
        }
    });

    layersDock->setWidget(layersWidget);
    layersWidget->setMinimumSize(50, 50);
    layersDock->setMinimumSize(50, 50);
    layersDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);
    
    // 为图层面板添加右键菜单支持
    m_layerTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_layerTree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::showLayerContextMenu);
    
    // 添加面板到主窗口
    // 参数面板在左边，按钮工具栏下方
    DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, paramsDock, CentralDockArea);
    // 属性面板和图层面板上下排，在右边
    DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, propsDock, CentralDockArea);
    
    if (propsDock->dockAreaWidget()) {
        DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, layersDock, propsDock->dockAreaWidget());
    }

    // 文件管理面板放在参数面板下方
    if (paramsDock->dockAreaWidget()) {
        // 先添加文件保存面板
        DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, fileDock, paramsDock->dockAreaWidget());
        
        // 然后将最近保存面板添加到文件保存面板的同一dock area中，形成标签页
        if (fileDock->dockAreaWidget()) {
            DockManager->addDockWidgetTabToArea(recentDock, fileDock->dockAreaWidget());
        }
    }
    
    // 创建视图切换面板（固定宽度 25，高度自适应，紧靠属性和图层面板左侧）
    const int TOOLBAR_WIDTH = 25;
    const int BUTTON_HEIGHT = 50;
    
    CDockWidget* viewDock = new CDockWidget(DockManager, "");
    QWidget* viewContainer = new QWidget();
    viewContainer->setFixedWidth(TOOLBAR_WIDTH);
    viewContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    
    QVBoxLayout* viewLayout = new QVBoxLayout(viewContainer);
    viewLayout->setContentsMargins(0, 0, 0, 0);
    viewLayout->setSpacing(0);
    viewLayout->setAlignment(Qt::AlignTop);
    
    // 辅助函数：创建垂直文字按钮
    auto createVerticalButton = [](const QString& text, int width, int height, bool isActive = false) -> QPushButton* {
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(width, height);
        btn->setCheckable(true);
        btn->setChecked(isActive);
        
        QString verticalText;
        for (int i = 0; i < text.length(); ++i) {
            verticalText += text[i];
            if (i < text.length() - 1) {
                verticalText += "\n";
            }
        }
        btn->setText(verticalText);
        
        if (isActive) {
            btn->setStyleSheet(
                "QPushButton { "
                "  border: 1px solid #4CAF50; "
                "  background: #4CAF50; "
                "  color: #fff; "
                "  font-weight: bold; "
                "  text-align: center; "
                "  font-size: 12px; "
                "  padding: 2px; "
                "} "
                "QPushButton:hover { background: #45a049; } "
                "QPushButton:pressed { background: #3d8b40; } "
            );
        } else {
            btn->setStyleSheet(
                "QPushButton { "
                "  border: 1px solid #ccc; "
                "  background: #f0f0f0; "
                "  text-align: center; "
                "  font-size: 12px; "
                "  padding: 2px; "
                "} "
                "QPushButton:hover { background: #e0e0e0; } "
                "QPushButton:pressed { background: #d0d0d0; } "
            );
        }
        return btn;
    };
    
    // 视图切换函数
    auto switchView = [TOOLBAR_WIDTH, BUTTON_HEIGHT](QPushButton* selectedButton, const QList<QPushButton*>& allButtons) {
        for (QPushButton* btn : allButtons) {
            if (btn == selectedButton) {
                btn->setChecked(true);
                btn->setStyleSheet(
                    "QPushButton { "
                    "  border: 1px solid #4CAF50; "
                    "  background: #4CAF50; "
                    "  color: #fff; "
                    "  font-weight: bold; "
                    "  text-align: center; "
                    "  font-size: 12px; "
                    "  padding: 2px; "
                    "} "
                    "QPushButton:hover { background: #45a049; } "
                    "QPushButton:pressed { background: #3d8b40; } "
                );
            } else {
                btn->setChecked(false);
                btn->setStyleSheet(
                    "QPushButton { "
                    "  border: 1px solid #ccc; "
                    "  background: #f0f0f0; "
                    "  text-align: center; "
                    "  font-size: 12px; "
                    "  padding: 2px; "
                    "} "
                    "QPushButton:hover { background: #e0e0e0; } "
                    "QPushButton:pressed { background: #d0d0d0; } "
                );
            }
        }
    };
    
    QPushButton* topViewButton = createVerticalButton("俯视", TOOLBAR_WIDTH, BUTTON_HEIGHT, true);
    viewLayout->addWidget(topViewButton);
    
    QPushButton* frontViewButton = createVerticalButton("顶视", TOOLBAR_WIDTH, BUTTON_HEIGHT, false);
    viewLayout->addWidget(frontViewButton);
    
    QPushButton* sideViewButton = createVerticalButton("前视", TOOLBAR_WIDTH, BUTTON_HEIGHT, false);
    viewLayout->addWidget(sideViewButton);
    
    QPushButton* rightViewButton = createVerticalButton("右视", TOOLBAR_WIDTH, BUTTON_HEIGHT, false);
    viewLayout->addWidget(rightViewButton);
    
    QList<QPushButton*> viewButtons = {topViewButton, frontViewButton, sideViewButton, rightViewButton};
    
    // 为每个按钮添加点击事件
    connect(topViewButton, &QPushButton::clicked, [switchView, topViewButton, viewButtons]() {
        switchView(topViewButton, viewButtons);
    });
    connect(frontViewButton, &QPushButton::clicked, [switchView, frontViewButton, viewButtons]() {
        switchView(frontViewButton, viewButtons);
    });
    connect(sideViewButton, &QPushButton::clicked, [switchView, sideViewButton, viewButtons]() {
        switchView(sideViewButton, viewButtons);
    });
    connect(rightViewButton, &QPushButton::clicked, [switchView, rightViewButton, viewButtons]() {
        switchView(rightViewButton, viewButtons);
    });
    
    viewLayout->addStretch();
    viewDock->setWidget(viewContainer);
    viewDock->setMinimumWidth(TOOLBAR_WIDTH);
    viewDock->setMaximumWidth(TOOLBAR_WIDTH);
    viewDock->setMinimumHeight(BUTTON_HEIGHT * 4);
    viewDock->setFeatures(CDockWidget::NoDockWidgetFeatures);
    viewDock->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidgetMinimumSize);
    viewDock->setObjectName("viewDock");
    viewDock->setStyleSheet(
        "#viewDock { "
        "  border: none; "
        "  min-width: " + QString::number(TOOLBAR_WIDTH) + "px; "
        "  max-width: " + QString::number(TOOLBAR_WIDTH) + "px; "
        "} "
        "#viewDock QWidget#dockAreaTitleBar { height: 0px; border: none; } "
        "#viewDock ads--CDockAreaWidget { border: none; min-width: " + QString::number(TOOLBAR_WIDTH) + "px; max-width: " + QString::number(TOOLBAR_WIDTH) + "px; } "
        "#viewDock QScrollBar { width: 0px; height: 0px; background: transparent; } "
    );
    
    // 禁用容器的滚动条
    viewContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    viewLayout->setSizeConstraint(QLayout::SetFixedSize);
    
    // 添加到属性面板的 dock area 中（这样视图标签栏就在属性和图层面板的左边）
    if (propsDock->dockAreaWidget()) {
        DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, viewDock, propsDock->dockAreaWidget());
    }
    
    // 彻底禁用分割器 - 找到并固定分割器尺寸
    if (viewDock->dockAreaWidget()) {
        auto* parentSplitter = viewDock->dockAreaWidget()->parentSplitter();
        if (parentSplitter) {
            parentSplitter->setChildrenCollapsible(false);
            parentSplitter->setHandleWidth(0);
            parentSplitter->setStyleSheet("QSplitter::handle { width: 0px; height: 0px; background: transparent; border: none; }");
            
            // 固定分割器尺寸分布
            QList<int> sizes;
            sizes << TOOLBAR_WIDTH << 1;
            parentSplitter->setSizes(sizes);
            parentSplitter->setStretchFactor(0, 0);
            parentSplitter->setStretchFactor(1, 1);
        }
    }
    
    // 创建底部工具栏（在状态栏上方）
    const int BOTTOM_TOOLBAR_HEIGHT = 24;
    QToolBar* bottomToolBar = new QToolBar("底部工具栏", this);
    bottomToolBar->setOrientation(Qt::Horizontal);
    bottomToolBar->setFixedHeight(BOTTOM_TOOLBAR_HEIGHT);
    bottomToolBar->setMovable(false);
    bottomToolBar->setFloatable(false);
    bottomToolBar->setIconSize(QSize(24, 24));
    
    // 设置工具栏停靠区域为底部左侧
    bottomToolBar->setAllowedAreas(Qt::BottomToolBarArea);
    
    // 添加标签按钮
    btnEndpoint = new QPushButton("端点", bottomToolBar);
    btnEndpoint->setCheckable(true);
    btnEndpoint->setChecked(true);
    bottomToolBar->addWidget(btnEndpoint);
    
    btnNearest = new QPushButton("最近点", bottomToolBar);
    btnNearest->setCheckable(true);
    bottomToolBar->addWidget(btnNearest);
    
    btnPoint = new QPushButton("点", bottomToolBar);
    btnPoint->setCheckable(true);
    bottomToolBar->addWidget(btnPoint);
    
    btnMidpoint = new QPushButton("中点", bottomToolBar);
    btnMidpoint->setCheckable(true);
    bottomToolBar->addWidget(btnMidpoint);
    
    btnCenter = new QPushButton("中心点", bottomToolBar);
    btnCenter->setCheckable(true);
    bottomToolBar->addWidget(btnCenter);
    
    btnIntersection = new QPushButton("交点", bottomToolBar);
    btnIntersection->setCheckable(true);
    bottomToolBar->addWidget(btnIntersection);
    
    btnPerpendicular = new QPushButton("垂点", bottomToolBar);
    btnPerpendicular->setCheckable(true);
    bottomToolBar->addWidget(btnPerpendicular);
    
    btnTangent = new QPushButton("切点", bottomToolBar);
    btnTangent->setCheckable(true);
    bottomToolBar->addWidget(btnTangent);
    
    btnQuarter = new QPushButton("四分点", bottomToolBar);
    btnQuarter->setCheckable(true);
    bottomToolBar->addWidget(btnQuarter);
    
    btnVertex = new QPushButton("顶点", bottomToolBar);
    btnVertex->setCheckable(true);
    bottomToolBar->addWidget(btnVertex);
    
    btnDisable = new QPushButton("停用", bottomToolBar);
    btnDisable->setCheckable(true);
    bottomToolBar->addWidget(btnDisable);
    
    // 设置工具栏样式
    bottomToolBar->setStyleSheet(
        "QToolBar { "
        "  background: #f0f0f0; "
        "  border: none; "
        "  border-top: 1px solid #ccc; "
        "  padding: 2px; "
        "  spacing: 4px; "
        "} "
        "QPushButton { "
        "  min-height: 20px; "
        "  font-size: 12px; "
        "  padding: 2px 8px; "
        "  border: 1px solid #ccc; "
        "  border-radius: 2px; "
        "  background: transparent; "
        "  color: #333; "
        "}"
        "QPushButton:hover { "
        "  background: #e0e0e0; "
        "}"
        "QPushButton:checked { "
        "  background: #4CAF50; "
        "  color: white; "
        "  border: 1px solid #4CAF50; "
        "}"
    );
    
    // 添加到底部
    addToolBar(Qt::BottomToolBarArea, bottomToolBar);
    
    // 创建第二个底部工具栏（过滤器）
    QToolBar* filterToolBar = new QToolBar("过滤器工具栏", this);
    filterToolBar->setOrientation(Qt::Horizontal);
    filterToolBar->setFixedHeight(BOTTOM_TOOLBAR_HEIGHT);
    filterToolBar->setMovable(false);
    filterToolBar->setFloatable(false);
    filterToolBar->setIconSize(QSize(24, 24));
    
    // 设置工具栏停靠区域为底部右侧
    filterToolBar->setAllowedAreas(Qt::BottomToolBarArea);
    
    // 添加间距
    QWidget* spacer = new QWidget(filterToolBar);
    spacer->setFixedWidth(20);
    filterToolBar->addWidget(spacer);
    
    // 添加过滤器标题
    QLabel* filterTitle = new QLabel("过滤器:", filterToolBar);
    filterTitle->setStyleSheet("font-size: 12px; color: #333; padding: 2px 4px;");
    filterToolBar->addWidget(filterTitle);
    
    // 添加过滤器标签按钮
    btnPointFilter = new QPushButton("点", filterToolBar);
    btnPointFilter->setCheckable(true);
    filterToolBar->addWidget(btnPointFilter);
    
    btnCurveFilter = new QPushButton("曲线", filterToolBar);
    btnCurveFilter->setCheckable(true);
    filterToolBar->addWidget(btnCurveFilter);
    
    btnSurfaceFilter = new QPushButton("曲面", filterToolBar);
    btnSurfaceFilter->setCheckable(true);
    filterToolBar->addWidget(btnSurfaceFilter);
    
    btnMeshFilter = new QPushButton("网格", filterToolBar);
    btnMeshFilter->setCheckable(true);
    filterToolBar->addWidget(btnMeshFilter);
    
    btnAnnotationFilter = new QPushButton("注释", filterToolBar);
    btnAnnotationFilter->setCheckable(true);
    filterToolBar->addWidget(btnAnnotationFilter);
    
    btnOtherFilter = new QPushButton("其他", filterToolBar);
    btnOtherFilter->setCheckable(true);
    filterToolBar->addWidget(btnOtherFilter);
    
    btnDisableFilter = new QPushButton("停用", filterToolBar);
    btnDisableFilter->setCheckable(true);
    btnDisableFilter->setChecked(true);
    filterToolBar->addWidget(btnDisableFilter);
    
    // 设置过滤器工具栏样式
    filterToolBar->setStyleSheet(
        "QToolBar { "
        "  background: #f0f0f0; "
        "  border: none; "
        "  border-top: 1px solid #ccc; "
        "  padding: 2px; "
        "  spacing: 4px; "
        "} "
        "QPushButton { "
        "  min-height: 20px; "
        "  font-size: 12px; "
        "  padding: 2px 8px; "
        "  border: 1px solid #ccc; "
        "  border-radius: 2px; "
        "  background: transparent; "
        "  color: #333; "
        "}"
        "QPushButton:hover { "
        "  background: #e0e0e0; "
        "}"
        "QPushButton:checked { "
        "  background: #4CAF50; "
        "  color: white; "
        "  border: 1px solid #4CAF50; "
        "}"
    );
    
    // 添加到底部，和第一个工具栏在同一行
    addToolBar(Qt::BottomToolBarArea, filterToolBar);
    
    // 工具栏标签逻辑
    // 保存原始选中状态到成员变量
    QList<QPushButton*> buttons = { btnEndpoint, btnNearest, btnPoint, btnMidpoint, btnCenter, btnIntersection, btnPerpendicular, btnTangent, btnQuarter, btnVertex };
    for (QPushButton* btn : buttons) {
        originalChecked[btn] = btn->isChecked();
    }
    
    // 右键点击事件处理
    for (QPushButton* btn : buttons) {
        btn->setContextMenuPolicy(Qt::NoContextMenu);
        btn->installEventFilter(this);
    }
    bottomToolBar->installEventFilter(this);
    
    // 停用按钮逻辑 - 使用成员变量存储状态
    connect(btnDisable, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            // 保存当前选中状态到成员变量
            QList<QPushButton*> buttons = { btnEndpoint, btnNearest, btnPoint, btnMidpoint, btnCenter, btnIntersection, btnPerpendicular, btnTangent, btnQuarter, btnVertex };
            for (QPushButton* btn : buttons) {
                originalChecked[btn] = btn->isChecked();
                btn->setChecked(false);
                btn->setEnabled(false);
                btn->setStyleSheet("QPushButton { min-height: 20px; font-size: 12px; padding: 2px 8px; border: 1px solid #ccc; border-radius: 2px; background: #f0f0f0; color: #999; }");
            }
        } else {
            // 启用所有标签并恢复原始选中状态
            QList<QPushButton*> buttons = { btnEndpoint, btnNearest, btnPoint, btnMidpoint, btnCenter, btnIntersection, btnPerpendicular, btnTangent, btnQuarter, btnVertex };
            for (QPushButton* btn : buttons) {
                btn->setEnabled(true);
                btn->setChecked(originalChecked[btn]);
                btn->setStyleSheet("QPushButton { min-height: 20px; font-size: 12px; padding: 2px 8px; border: 1px solid #ccc; border-radius: 2px; background: transparent; color: #333; } QPushButton:hover { background: #e0e0e0; } QPushButton:checked { background: #4CAF50; color: white; border: 1px solid #4CAF50; }");
            }
        }
    });
    
    // 过滤器停用按钮逻辑
    // 保存原始选中状态到成员变量
    QList<QPushButton*> filterButtons = { btnPointFilter, btnCurveFilter, btnSurfaceFilter, btnMeshFilter, btnAnnotationFilter, btnOtherFilter };
    for (QPushButton* btn : filterButtons) {
        filterOriginalChecked[btn] = btn->isChecked();
    }
    
    // 右键点击事件处理
    for (QPushButton* btn : filterButtons) {
        btn->setContextMenuPolicy(Qt::NoContextMenu);
        btn->installEventFilter(this);
    }
    filterToolBar->installEventFilter(this);
    
    // 停用按钮逻辑 - 使用成员变量存储状态
    connect(btnDisableFilter, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            // 保存当前选中状态到成员变量
            QList<QPushButton*> buttons = { btnPointFilter, btnCurveFilter, btnSurfaceFilter, btnMeshFilter, btnAnnotationFilter, btnOtherFilter };
            for (QPushButton* btn : buttons) {
                filterOriginalChecked[btn] = btn->isChecked();
                btn->setChecked(false);
                btn->setEnabled(false);
                btn->setStyleSheet("QPushButton { min-height: 20px; font-size: 12px; padding: 2px 8px; border: 1px solid #ccc; border-radius: 2px; background: #f0f0f0; color: #999; }");
            }
        } else {
            // 启用所有过滤器并恢复原始选中状态
            QList<QPushButton*> buttons = { btnPointFilter, btnCurveFilter, btnSurfaceFilter, btnMeshFilter, btnAnnotationFilter, btnOtherFilter };
            for (QPushButton* btn : buttons) {
                btn->setEnabled(true);
                btn->setChecked(filterOriginalChecked[btn]);
                btn->setStyleSheet("QPushButton { min-height: 20px; font-size: 12px; padding: 2px 8px; border: 1px solid #ccc; border-radius: 2px; background: transparent; color: #333; } QPushButton:hover { background: #e0e0e0; } QPushButton:checked { background: #4CAF50; color: white; border: 1px solid #4CAF50; }");
            }
        }
    });
    
    QMenu* fileMenu = menuBar->addMenu("文件");
    QAction* newAction = new QAction("新建", this);
    newAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newAction);

    QAction* openAction = new QAction("打开", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);

    fileMenu->addSeparator();

    QAction* saveAction = new QAction("保存", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);

    QAction* saveAsAction = new QAction("另存为", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    QMenu* editMenu = menuBar->addMenu("编辑");
    QAction* undoAction = new QAction("撤销", this);
    undoAction->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAction);

    QAction* redoAction = new QAction("重做", this);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    QAction* cutAction = new QAction("剪切", this);
    cutAction->setShortcut(QKeySequence::Cut);
    editMenu->addAction(cutAction);

    QAction* copyAction = new QAction("复制", this);
    copyAction->setShortcut(QKeySequence::Copy);
    editMenu->addAction(copyAction);

    QAction* pasteAction = new QAction("粘贴", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    editMenu->addAction(pasteAction);

    QMenu* viewMenu = menuBar->addMenu("视图");
    QAction* resetViewAction = new QAction("重置视图", this);
    resetViewAction->setShortcut(Qt::Key_R | Qt::CTRL);
    connect(resetViewAction, &QAction::triggered, this, [this]() {
        if (threeDCanvas) {
            threeDCanvas->resetView();
        }
    });
    viewMenu->addAction(resetViewAction);

    menuBar->addMenu("曲线");
    menuBar->addMenu("浮雕");
    menuBar->addMenu("3D建模");
    QMenu* toolsMenu = menuBar->addMenu("工具");
    
    // 添加设置菜单项
    QAction* settingsAction = new QAction("设置", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
    toolsMenu->addAction(settingsAction);
    
    menuBar->addMenu("分析");
    
    // 添加状态栏
    QStatusBar* statusBar = new QStatusBar(this);
    statusBar->setFixedHeight(20);
    statusBar->showMessage("就绪");
    setStatusBar(statusBar);
    
    // 创建视窗菜单并添加面板控制选项
    QMenu* viewWindowMenu = menuBar->addMenu("视窗");
    
    // 创建面板子菜单
    QMenu* panelsMenu = viewWindowMenu->addMenu("面板");
    
    // 为参数面板创建菜单项
    QAction* paramsAction = new QAction("参数面板", this);
    paramsAction->setCheckable(true);
    paramsAction->setChecked(!paramsDock->isClosed());
    connect(paramsAction, &QAction::triggered, this, [paramsDock](bool checked) {
        if (checked) {
            paramsDock->toggleView(true);
        } else {
            paramsDock->closeDockWidget();
        }
    });
    connect(paramsDock, &CDockWidget::viewToggled, this, [paramsAction](bool open) {
        paramsAction->setChecked(open);
    });
    panelsMenu->addAction(paramsAction);
    
    // 为属性面板创建菜单项
    QAction* propsAction = new QAction("属性面板", this);
    propsAction->setCheckable(true);
    propsAction->setChecked(!propsDock->isClosed());
    connect(propsAction, &QAction::triggered, this, [propsDock](bool checked) {
        if (checked) {
            propsDock->toggleView(true);
        } else {
            propsDock->closeDockWidget();
        }
    });
    connect(propsDock, &CDockWidget::viewToggled, this, [propsAction](bool open) {
        propsAction->setChecked(open);
    });
    panelsMenu->addAction(propsAction);
    
    // 为图层面板创建菜单项
    QAction* layersAction = new QAction("图层面板", this);
    layersAction->setCheckable(true);
    layersAction->setChecked(!layersDock->isClosed());
    connect(layersAction, &QAction::triggered, this, [layersDock](bool checked) {
        if (checked) {
            layersDock->toggleView(true);
        } else {
            layersDock->closeDockWidget();
        }
    });
    connect(layersDock, &CDockWidget::viewToggled, this, [layersAction](bool open) {
        layersAction->setChecked(open);
    });
    panelsMenu->addAction(layersAction);
    
    // 为文件保存面板创建菜单项
    QAction* fileSaveAction = new QAction("文件保存面板", this);
    fileSaveAction->setCheckable(true);
    fileSaveAction->setChecked(!fileDock->isClosed());
    connect(fileSaveAction, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            fileDock->toggleView(true);
        } else {
            fileDock->closeDockWidget();
        }
    });
    connect(fileDock, &CDockWidget::viewToggled, this, [fileSaveAction](bool open) {
        fileSaveAction->setChecked(open);
    });
    panelsMenu->addAction(fileSaveAction);
    
    // 为最近保存面板创建菜单项
    QAction* recentSaveAction = new QAction("最近保存面板", this);
    recentSaveAction->setCheckable(true);
    recentSaveAction->setChecked(!recentDock->isClosed());
    connect(recentSaveAction, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            recentDock->toggleView(true);
        } else {
            recentDock->closeDockWidget();
        }
    });
    connect(recentDock, &CDockWidget::viewToggled, this, [recentSaveAction](bool open) {
        recentSaveAction->setChecked(open);
    });
    panelsMenu->addAction(recentSaveAction);
    
    QMenu* helpMenu = menuBar->addMenu("说明");

    QAction* aboutAction = new QAction("关于", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::refreshLayerUI()
{
    if (!m_layerTree) return;
    
    // 清空复选框列表
    layerCheckboxes.clear();
    
    // 清空树控件
    m_layerTree->clear();
    
    // 根据m_layers数据重新创建所有UI
    for (int row = 0; row < m_layers.size(); ++row) {
        const LayerData& layerData = m_layers[row];
        
        QTreeWidgetItem* item = new QTreeWidgetItem(m_layerTree);
        item->setText(0, layerData.name);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        // 当前列 - 打勾复选框
        QCheckBox* currentCheck = new QCheckBox();
        currentCheck->setFocusPolicy(Qt::NoFocus);
        currentCheck->setChecked(layerData.isCurrent);
        currentCheck->setStyleSheet("margin-left: 5px;");
        m_layerTree->setItemWidget(item, 1, currentCheck);
        
        // 添加到复选框列表
        layerCheckboxes.append(currentCheck);
        
        // 连接点击事件，确保只能有一个复选框被勾选
        connect(currentCheck, &QCheckBox::clicked, this, [this, currentCheck, row]() {
            // 更新数据模型 - 只修改当前图层状态
            for (int i = 0; i < m_layers.size(); ++i) {
                m_layers[i].isCurrent = false;
            }
            m_layers[row].isCurrent = true;
            
            // 如果激活的图层是隐藏或锁定的，自动将其变为可见和解锁
            if (!m_layers[row].isVisible) {
                m_layers[row].isVisible = true;
            }
            if (m_layers[row].isLocked) {
                m_layers[row].isLocked = false;
            }
            
            // 刷新UI，重新构建所有图标
            refreshLayerUI();
            // 保持选中状态
            if (m_layerTree->topLevelItemCount() > row) {
                m_layerTree->setCurrentItem(m_layerTree->topLevelItem(row));
            }
        });

        // 可见列 - 灯泡图标（激活状态不显示）
        if (!layerData.isCurrent) {
            QLabel* visibleLabel = new QLabel();
            visibleLabel->setFixedSize(28, 24);
            visibleLabel->setText(layerData.isVisible ? "💡" : "●");
            visibleLabel->setStyleSheet(layerData.isVisible ? "font-size: 18px; background: transparent;" : "font-size: 30px; background: transparent;");
            visibleLabel->setAlignment(Qt::AlignCenter);
            visibleLabel->setProperty("visibleState", layerData.isVisible);
            visibleLabel->setProperty("layerRow", row);
            visibleLabel->setCursor(Qt::PointingHandCursor);
            visibleLabel->installEventFilter(this);
            m_layerTree->setItemWidget(item, 2, visibleLabel);
        }

        // 锁定列 - 锁图标（激活状态不显示）
        if (!layerData.isCurrent) {
            QLabel* lockLabel = new QLabel();
            lockLabel->setFixedSize(28, 24);
            lockLabel->setText(layerData.isLocked ? "🔒" : "🔓");
            lockLabel->setStyleSheet("font-size: 16px; background: transparent;");
            lockLabel->setAlignment(Qt::AlignCenter);
            lockLabel->setProperty("lockedState", layerData.isLocked);
            lockLabel->setProperty("layerRow", row);
            lockLabel->setCursor(Qt::PointingHandCursor);
            lockLabel->installEventFilter(this);
            m_layerTree->setItemWidget(item, 3, lockLabel);
        }

        // 颜色列 - 颜色编辑器
        QPushButton* colorBtn = new QPushButton();
        colorBtn->setFocusPolicy(Qt::NoFocus);
        colorBtn->setFixedSize(24, 24);
        colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999; border-radius: 2px; margin: auto;").arg(layerData.color.name()));
        colorBtn->setProperty("color", layerData.color);
        colorBtn->setProperty("layerRow", row);
        connect(colorBtn, &QPushButton::clicked, this, [this, colorBtn]() {
            QColor c = QColorDialog::getColor(colorBtn->property("color").value<QColor>(), nullptr, "选择颜色");
            if (c.isValid()) {
                colorBtn->setProperty("color", c);
                colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999; border-radius: 2px; margin: auto;").arg(c.name()));
                // 更新数据模型
                int row = colorBtn->property("layerRow").toInt();
                if (row >= 0 && row < m_layers.size()) {
                    m_layers[row].color = c;
                }
            }
        });
        m_layerTree->setItemWidget(item, 4, colorBtn);

        // 材质列 - 材质编辑器下拉框
        QComboBox* materialCombo = new QComboBox();
        materialCombo->setFocusPolicy(Qt::NoFocus);
        materialCombo->setFixedHeight(20);
        materialCombo->setFixedWidth(50); // 宽度足够显示2个字
        materialCombo->addItems({"默认", "金属", "塑料", "玻璃", "木材", "橡胶"});
        materialCombo->setCurrentText(layerData.material);
        materialCombo->setProperty("layerRow", row);
        materialCombo->setStyleSheet("font-size: 11px; padding: 1px;");
        connect(materialCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this, [this, materialCombo](const QString& text) {
            // 更新数据模型
            int row = materialCombo->property("layerRow").toInt();
            if (row >= 0 && row < m_layers.size()) {
                m_layers[row].material = text;
            }
        });
        m_layerTree->setItemWidget(item, 5, materialCombo);
    }
    
    m_layerTree->expandAll();
}

void MainWindow::showLayerContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = m_layerTree->itemAt(pos);
    if (!item) return;
    
    QMenu contextMenu(this);
    
    // 获取当前选中的图层索引
    int layerIndex = m_layerTree->indexOfTopLevelItem(item);
    if (layerIndex < 0 || layerIndex >= m_layers.size()) return;
    
    // 添加菜单项
    QAction* selectObjectsAction = contextMenu.addAction("选取该图层物体");
    QAction* putToLayerAction = contextMenu.addAction("放入该图层");
    QAction* deleteAction = contextMenu.addAction("删除");
    QAction* wireframeAction = contextMenu.addAction("线框");
    
    // 连接菜单项信号
    connect(selectObjectsAction, &QAction::triggered, this, [this, layerIndex]() {
        // 选取该图层物体的逻辑
        qDebug() << "选取图层" << layerIndex << "的物体";
    });
    
    connect(putToLayerAction, &QAction::triggered, this, [this, layerIndex]() {
        // 放入该图层的逻辑
        qDebug() << "将选中物体放入图层" << layerIndex;
    });
    
    connect(deleteAction, &QAction::triggered, this, [this, layerIndex]() {
        // 删除图层的逻辑
        if (m_layers.size() > 1) {
            m_layers.removeAt(layerIndex);
            refreshLayerUI();
        }
    });
    
    connect(wireframeAction, &QAction::triggered, this, [this, layerIndex]() {
        // 线框显示的逻辑
        qDebug() << "切换图层" << layerIndex << "的线框显示";
    });
    
    // 显示菜单
    contextMenu.exec(m_layerTree->mapToGlobal(pos));
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QWidget* w = childAt(event->pos());
    if (w && (w->inherits("QToolBar") || qobject_cast<QToolBar*>(w->parent()))) {
        event->ignore();
        return;
    }
    QMainWindow::contextMenuEvent(event);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ContextMenu) {
        QPushButton* btn = qobject_cast<QPushButton*>(obj);
        if (btn) {
            // 检查是否是主工具栏按钮
            QList<QPushButton*> mainButtons = { btnEndpoint, btnNearest, btnPoint, btnMidpoint, btnCenter, btnIntersection, btnPerpendicular, btnTangent, btnQuarter, btnVertex };
            if (mainButtons.contains(btn)) {
                // 处理主工具栏按钮右键点击
                bool onlyThisChecked = true;
                for (QPushButton* otherBtn : mainButtons) {
                    if (otherBtn != btn && otherBtn->isChecked()) {
                        onlyThisChecked = false;
                        break;
                    }
                }

                if (onlyThisChecked) {
                    // 再次点击，勾选所有标签
                    for (QPushButton* otherBtn : mainButtons) {
                        otherBtn->setChecked(true);
                    }
                } else {
                    // 选中当前标签，取消其他所有标签
                    for (QPushButton* otherBtn : mainButtons) {
                        otherBtn->setChecked(otherBtn == btn);
                    }
                }
                return true; // 阻止事件传播
            }

            // 检查是否是过滤器工具栏按钮
            QList<QPushButton*> filterButtons = { btnPointFilter, btnCurveFilter, btnSurfaceFilter, btnMeshFilter, btnAnnotationFilter, btnOtherFilter };
            if (filterButtons.contains(btn)) {
                // 处理过滤器工具栏按钮右键点击
                bool onlyThisChecked = true;
                for (QPushButton* otherBtn : filterButtons) {
                    if (otherBtn != btn && otherBtn->isChecked()) {
                        onlyThisChecked = false;
                        break;
                    }
                }

                if (onlyThisChecked) {
                    // 再次点击，勾选所有标签
                    for (QPushButton* otherBtn : filterButtons) {
                        otherBtn->setChecked(true);
                    }
                } else {
                    // 选中当前标签，取消其他所有标签
                    for (QPushButton* otherBtn : filterButtons) {
                        otherBtn->setChecked(otherBtn == btn);
                    }
                }
                return true; // 阻止事件传播
            }
        }

        // 如果是工具栏本身，阻止右键菜单
        if (qobject_cast<QToolBar*>(obj)) {
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* label = qobject_cast<QLabel*>(obj);
        if (label) {
            bool hasVisibleProp = label->property("visibleState").isValid();
            bool hasLockedProp = label->property("lockedState").isValid();
            if (hasVisibleProp) {
                bool state = label->property("visibleState").toBool();
                int row = label->property("layerRow").toInt();
                if (row >= 0 && row < m_layers.size()) {
                    // 更新数据模型
                    m_layers[row].isVisible = !state;
                    // 更新UI显示
                    label->setProperty("visibleState", !state);
                    label->setText(!state ? "💡" : "●");
                    label->setStyleSheet(!state ? "font-size: 18px; background: transparent;" : "font-size: 30px; background: transparent;");
                }
                return true;
            }
            if (hasLockedProp) {
                bool state = label->property("lockedState").toBool();
                int row = label->property("layerRow").toInt();
                if (row >= 0 && row < m_layers.size()) {
                    // 更新数据模型
                    m_layers[row].isLocked = !state;
                    // 更新UI显示
                    label->setProperty("lockedState", !state);
                    label->setText(!state ? "🔒" : "🔓");
                    label->setStyleSheet("font-size: 16px; background: transparent;");
                }
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onAbout()
{
}

void MainWindow::onSettings()
{
    SettingsDialog dialog(this);
    
    // 连接应用按钮信号
    connect(&dialog, &SettingsDialog::applyClicked, this, [&]() {
        updateFileNamesFromSettings(&dialog);
    });
    
    // 执行对话框
    int result = dialog.exec();
    
    // 如果用户点击了确定按钮，更新文件名
    if (result == QDialog::Accepted) {
        updateFileNamesFromSettings(&dialog);
    }
}

void MainWindow::showToolbarContextMenu(QWidget* toolbarWidget, QHBoxLayout* toolbarLayout, const QPoint& pos)
{
    QMenu contextMenu(toolbarWidget);
    
    // 添加分割线选项
    QAction* addSeparatorAction = contextMenu.addAction("添加分割线");
    
    // 添加新增按钮选项
    QAction* addButtonAction = contextMenu.addAction("新增按钮");
    
    // 显示菜单
    QAction* selectedAction = contextMenu.exec(toolbarWidget->mapToGlobal(pos));
    
    if (selectedAction == addSeparatorAction) {
        // 添加垂直分割线
        QFrame* separator = new QFrame(toolbarWidget);
        separator->setFrameShape(QFrame::VLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setFixedWidth(1);
        separator->setStyleSheet("background-color: #999;");
        
        // 插入到布局中
        toolbarLayout->insertWidget(toolbarLayout->count() - 1, separator);
        

    } else if (selectedAction == addButtonAction) {
        // 添加空白按钮
        RhinoButton* newButton = new RhinoButton("新按钮", toolbarWidget);
        newButton->setLeftClickAction("NewCommand");
        newButton->setRightClickAction("NewCommandOptions");
        
        // 插入到布局中（在最后一个拉伸项之前）
        toolbarLayout->insertWidget(toolbarLayout->count() - 1, newButton);
        

    }
}

void MainWindow::onFileNameChanged(int index, const QString& fileName)
{
    // 更新文件保存面板中的标签文本
    if (fileDock && fileDock->widget()) {
        QWidget* fileWidget = fileDock->widget();
        QVBoxLayout* fileLayout = qobject_cast<QVBoxLayout*>(fileWidget->layout());
        if (fileLayout && index >= 0 && index < fileLayout->count()) {
            QWidget* fileRow = fileLayout->itemAt(index)->widget();
            if (fileRow) {
                QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(fileRow->layout());
                if (rowLayout && rowLayout->count() > 0) {
                    QLabel* fileLabel = qobject_cast<QLabel*>(rowLayout->itemAt(0)->widget());
                    if (fileLabel) {
                        // 如果文件名为空，使用默认名称
                        if (fileName.isEmpty()) {
                            fileLabel->setText(QString("文件%1").arg(index + 1));
                        } else {
                            fileLabel->setText(fileName);
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::onClearRecentList()
{
    if (recentList) {
        recentList->clear();
    }
}

void MainWindow::addToRecentList(const QString& filePath)
{
    if (!recentList || filePath.isEmpty()) return;
    
    // 检查是否已存在相同文件
    for (int i = 0; i < recentList->count(); i++) {
        QListWidgetItem* item = recentList->item(i);
        if (item && item->text() == filePath) {
            // 如果已存在，移动到顶部
            recentList->takeItem(i);
            recentList->insertItem(0, item);
            return;
        }
    }
    
    // 添加新文件到顶部
    QListWidgetItem* item = new QListWidgetItem(filePath);
    recentList->insertItem(0, item);
    
    // 限制列表数量（最多10个）
    while (recentList->count() > 10) {
        delete recentList->takeItem(recentList->count() - 1);
    }
}

void MainWindow::updateFileNamesFromSettings(SettingsDialog* dialog)
{
    if (!dialog || !fileDock || !fileDock->widget()) return;
    
    QWidget* fileWidget = fileDock->widget();
    QVBoxLayout* fileLayout = qobject_cast<QVBoxLayout*>(fileWidget->layout());
    if (!fileLayout) return;
    
    // 更新所有5个文件的标签和显示状态
    for (int i = 0; i < 5; i++) {
        if (i < fileLayout->count()) {
            QWidget* fileRow = fileLayout->itemAt(i)->widget();
            if (fileRow) {
                // 控制文件行的显示/隐藏
                bool isVisible = dialog->isFileVisible(i);
                fileRow->setVisible(isVisible);
                
                if (isVisible) {
                    QHBoxLayout* rowLayout = qobject_cast<QHBoxLayout*>(fileRow->layout());
                    if (rowLayout && rowLayout->count() > 0) {
                        QLabel* fileLabel = qobject_cast<QLabel*>(rowLayout->itemAt(0)->widget());
                        if (fileLabel) {
                            QString fileName = dialog->getFileName(i);
                            // 如果文件名为空，使用默认名称
                            if (fileName.isEmpty()) {
                                fileLabel->setText(QString("文件%1").arg(i + 1));
                            } else {
                                fileLabel->setText(fileName);
                            }
                        }
                    }
                }
            }
        }
    }
}
