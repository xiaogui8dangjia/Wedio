#include "DockablePanel.h"
#include <QApplication>

DockablePanel::DockablePanel(const QString& title, QWidget* parent)
    : QWidget(parent), isDragging(false), dragPreview(nullptr), 
      dockPosition(DockPosition::Left), dropIndicator(DropIndicator::None)
{
    setMinimumSize(200, 300);
    setMouseTracking(true);
    
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 创建标题栏
    titleBar = new QWidget(this);
    titleBar->setObjectName("panelTitleBar");
    titleBar->setStyleSheet(
        "QWidget#panelTitleBar {"
        "  background-color: #e0e0e0;"  // 改为浅灰色
        "  border-bottom: 1px solid #cccccc;"  // 改为浅色边框
        "  padding: 2px;"
        "}"
    );
    
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(5, 3, 5, 3);
    titleLayout->setSpacing(5);
    
    // 标题标签
    titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("color: #000000; font-weight: bold; font-size: 11px;");
    titleLayout->addWidget(titleLabel);
    
    titleLayout->addStretch();
    
    // 关闭按钮（可选，用于浮动模式）
    closeButton = new QPushButton("×", this);
    closeButton->setFixedSize(16, 16);
    closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #aaaaaa;"
        "  border: none;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #5a5a5a;"
        "  color: #ffffff;"
        "}"
    );
    titleLayout->addWidget(closeButton);
    
    mainLayout->addWidget(titleBar);
    
    // 创建内容区域
    contentWidget = new QWidget(this);
    contentWidget->setObjectName("panelContent");
    contentWidget->setStyleSheet(
        "QWidget#panelContent {"
        "  background-color: #f0f0f0;"  // 改为浅灰色
        "}"
    );
    // 注意：不在构造函数中设置布局，由外部代码设置
    mainLayout->addWidget(contentWidget);
    
    // 设置样式
    setStyleSheet(
        "DockablePanel {"
        "  background-color: #f0f0f0;"  // 改为浅灰色
        "  border: 1px solid #cccccc;"  // 改为浅色边框
        "}"
    );
    
    // 连接关闭按钮信号
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        hide();
    });
}

DockablePanel::~DockablePanel()
{
    if (dragPreview) {
        delete dragPreview;
    }
}

void DockablePanel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && titleBar->geometry().contains(event->pos())) {
        isDragging = true;
        dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        
        // 创建拖动预览
        dragPreview = new QWidget(nullptr);
        dragPreview->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
        dragPreview->setFixedSize(size());
        dragPreview->setStyleSheet(
            "QWidget {"
            "  background-color: rgba(240, 240, 240, 180);"  // 改为浅灰色半透明
            "  border: 2px solid #0078d4;"
            "}"
        );
        dragPreview->show();
        
        // 发射拖动开始信号
        emit dragStarted(this);
    } else {
        QWidget::mousePressEvent(event);
    }
}

void DockablePanel::mouseMoveEvent(QMouseEvent* event)
{
    if (isDragging && dragPreview) {
        dragPreview->move(event->globalPosition().toPoint() - dragStartPos);
        event->accept();
        
        // 发射请求显示吸附提示信号
        emit requestShowDropIndicator(event->globalPosition().toPoint(), this);
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void DockablePanel::mouseReleaseEvent(QMouseEvent* event)
{
    if (isDragging) {
        isDragging = false;
        
        if (dragPreview) {
            delete dragPreview;
            dragPreview = nullptr;
        }
        
        // 发射请求隐藏吸附提示信号
        emit requestHideDropIndicator();
        
        // 根据当前位置确定新的停靠位置（由父窗口处理）
        event->accept();
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void DockablePanel::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    
    // 绘制吸附指示器
    if (dropIndicator != DropIndicator::None) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        QColor indicatorColor(0, 120, 212, 100);
        QBrush brush(indicatorColor);
        QPen pen(indicatorColor.darker(150), 2);
        
        painter.setBrush(brush);
        painter.setPen(pen);
        
        QRect indicatorRect;
        
        switch (dropIndicator) {
            case DropIndicator::Top:
                indicatorRect = QRect(0, 0, width(), height() / 2);
                break;
            case DropIndicator::Bottom:
                indicatorRect = QRect(0, height() / 2, width(), height() / 2);
                break;
            case DropIndicator::Left:
                indicatorRect = QRect(0, 0, width() / 2, height());
                break;
            case DropIndicator::Right:
                indicatorRect = QRect(width() / 2, 0, width() / 2, height());
                break;
            case DropIndicator::Center:
                indicatorRect = rect().adjusted(10, 10, -10, -10);
                break;
            default:
                break;
        }
        
        if (dropIndicator != DropIndicator::None) {
            painter.drawRect(indicatorRect);
        }
    }
}
