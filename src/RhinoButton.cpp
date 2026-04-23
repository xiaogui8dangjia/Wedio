#include "RhinoButton.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDebug>

RhinoButton::RhinoButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
    , m_isDragging(false)
    , m_shiftPressed(false)
    , m_appearanceMode(2)
    , m_editor(nullptr)
{
    init();
}

RhinoButton::RhinoButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QPushButton(icon, text, parent)
    , m_isDragging(false)
    , m_shiftPressed(false)
    , m_appearanceMode(2)
    , m_editor(nullptr)
{
    init();
}

RhinoButton::~RhinoButton()
{
    if (m_editor) {
        delete m_editor;
    }
}

void RhinoButton::init()
{
    // 设置按钮样式
    setStyleSheet(
        "RhinoButton {"
        "    border: 1px solid #999;"
        "    border-radius: 3px;"
        "    padding: 4px 8px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f6f6f6, stop:1 #e0e0e0);"
        "    min-width: 60px;"
        "    min-height: 24px;"
        "}"
        "RhinoButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e8e8e8, stop:1 #d0d0d0);"
        "}"
        "RhinoButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d0d0d0, stop:1 #b8b8b8);"
        "}"
    );
    
    // 设置工具提示
    if (!m_tooltip.isEmpty()) {
        setToolTip(m_tooltip);
    }
    
    // 启用拖放功能
    setAcceptDrops(true);
    
    updateAppearance();
}

void RhinoButton::setAppearanceMode(int mode)
{
    if (mode >= 0 && mode <= 2) {
        m_appearanceMode = mode;
        updateAppearance();
    }
}

int RhinoButton::appearanceMode() const
{
    return m_appearanceMode;
}

void RhinoButton::setLeftClickAction(const QString& action)
{
    m_leftClickAction = action;
}

QString RhinoButton::leftClickAction() const
{
    return m_leftClickAction;
}

void RhinoButton::setRightClickAction(const QString& action)
{
    m_rightClickAction = action;
}

QString RhinoButton::rightClickAction() const
{
    return m_rightClickAction;
}

void RhinoButton::setTooltip(const QString& tooltip)
{
    m_tooltip = tooltip;
    setToolTip(tooltip);
}

QString RhinoButton::tooltip() const
{
    return m_tooltip;
}

void RhinoButton::mousePressEvent(QMouseEvent* event)
{
    m_shiftPressed = (event->modifiers() & Qt::ShiftModifier);
    
    if (event->button() == Qt::LeftButton && m_shiftPressed) {
        // Shift+左键：开始拖动
        m_dragStartPosition = event->pos();
        m_isDragging = false;
        // 需要调用父类处理来确保拖放功能正常工作
        QPushButton::mousePressEvent(event);
        return;
    }
    
    if (event->button() == Qt::RightButton && m_shiftPressed) {
        // Shift+右键：打开按钮编辑器
        onEditButton();
        return;
    }
    
    if (event->button() == Qt::RightButton) {
        // 右键：执行右键功能
        if (!m_rightClickAction.isEmpty()) {
            // 执行右键动作
            emit clicked();
            // 这里可以添加具体的右键功能执行逻辑
        }
        return;
    }
    
    // 普通左键点击：总是调用父类处理
    if (event->button() == Qt::LeftButton && !m_shiftPressed) {
        if (!m_leftClickAction.isEmpty()) {
            // 执行左键动作
            emit clicked();
        }
        QPushButton::mousePressEvent(event);
    }
}

void RhinoButton::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton) || !m_shiftPressed) {
        QPushButton::mouseMoveEvent(event);
        return;
    }
    
    // 检查是否开始拖动
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    
    // 开始拖动
    startDrag();
    m_isDragging = true;
}

void RhinoButton::mouseReleaseEvent(QMouseEvent* event)
{
    m_isDragging = false;
    m_shiftPressed = false;
    
    QPushButton::mouseReleaseEvent(event);
}

void RhinoButton::contextMenuEvent(QContextMenuEvent* event)
{
    // 阻止默认的上下文菜单，使用我们自定义的右键处理
    event->ignore();
}

void RhinoButton::onEditButton()
{
    if (!m_editor) {
        m_editor = new RhinoButtonEditor(this, parentWidget());
    }
    m_editor->exec();
}

void RhinoButton::startDrag()
{
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    
    // 设置拖拽数据 - 包含被拖动按钮的指针信息
    mimeData->setText(text());
    
    // 将按钮指针转换为字符串存储
    QString buttonPtr = QString::number(reinterpret_cast<quintptr>(this));
    mimeData->setData("application/x-rhinobutton", buttonPtr.toUtf8());
    
    drag->setMimeData(mimeData);
    
    // 创建拖拽时的预览图像
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(width()/2, height()/2));
    
    // 执行拖拽
    drag->exec(Qt::MoveAction);
}

void RhinoButton::updateAppearance()
{
    switch (m_appearanceMode) {
    case 0: // 仅图标
        setText("");
        break;
    case 1: // 仅文本
        setIcon(QIcon());
        break;
    case 2: // 图标+文本
        // 保持当前设置
        break;
    }
}

void RhinoButton::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-rhinobutton")) {
        event->acceptProposedAction();
        // 完全移除拖放视觉反馈，保持按钮原样
    }
}

void RhinoButton::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-rhinobutton")) {
        event->acceptProposedAction();
        // 完全移除拖放视觉反馈，保持按钮原样
    }
}

void RhinoButton::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-rhinobutton")) {
        event->acceptProposedAction();
        
        QByteArray buttonPtrData = event->mimeData()->data("application/x-rhinobutton");
        QString buttonPtrStr = QString::fromUtf8(buttonPtrData);
        quintptr buttonPtr = buttonPtrStr.toULongLong();
        RhinoButton* draggedButton = reinterpret_cast<RhinoButton*>(buttonPtr);
        
        if (draggedButton && draggedButton != this) {
            QPoint cursorPos = QCursor::pos();
            draggedButton->moveButtonToPosition(cursorPos);
        }
    }
}

void RhinoButton::moveButtonToPosition(const QPoint& globalPos)
{
    QWidget* parentWidget = this->parentWidget();
    if (!parentWidget) return;
    
    QLayout* layout = parentWidget->layout();
    if (!layout) return;
    
    QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(layout);
    if (!hLayout) return;
    
    QPoint localPos = parentWidget->mapFromGlobal(globalPos);
    int mouseX = localPos.x();
    
    // 找到当前按钮在布局中的真实索引
    int currentIndex = -1;
    for (int i = 0; i < hLayout->count(); i++) {
        QLayoutItem* item = hLayout->itemAt(i);
        if (item && item->widget() == this) {
            currentIndex = i;
            break;
        }
    }
    if (currentIndex == -1) return;
    
    // 遍历布局找目标插入点
    int insertIndex = -1;
    
    for (int i = 0; i < hLayout->count(); i++) {
        QLayoutItem* item = hLayout->itemAt(i);
        if (!item || !item->widget()) continue;
        
        RhinoButton* btn = qobject_cast<RhinoButton*>(item->widget());
        if (!btn) continue;
        
        QWidget* w = item->widget();
        QRect rect = w->geometry();
        
        if (mouseX <= rect.center().x()) {
            insertIndex = i;
            break;
        }
    }
    
    // 如果没找到，插入到最后一个按钮之后
    if (insertIndex == -1) {
        for (int i = hLayout->count() - 1; i >= 0; i--) {
            QLayoutItem* item = hLayout->itemAt(i);
            if (item && item->widget() && qobject_cast<RhinoButton*>(item->widget())) {
                insertIndex = i + 1;
                break;
            }
        }
    }
    
    if (insertIndex != currentIndex && insertIndex != -1) {
        hLayout->removeWidget(this);
        
        // remove后索引偏移修正
        if (insertIndex > currentIndex) {
            insertIndex--;
        }
        
        insertIndex = qBound(0, insertIndex, hLayout->count());
        
        hLayout->insertWidget(insertIndex, this);
        hLayout->activate();
        parentWidget->update();
        
        // 确保按钮恢复默认状态和外观
        this->setDown(false);
        this->setChecked(false);
        this->clearFocus();
    }
}

// 按钮编辑器实现
RhinoButtonEditor::RhinoButtonEditor(RhinoButton* button, QWidget* parent)
    : QDialog(parent)
    , m_button(button)
{
    setupUI();
    setWindowTitle("按钮编辑器");
    setModal(true);
    resize(400, 300);
}

void RhinoButtonEditor::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 外观设置组
    QGroupBox* appearanceGroup = new QGroupBox("外观设置", this);
    QVBoxLayout* appearanceLayout = new QVBoxLayout(appearanceGroup);
    
    QHBoxLayout* modeLayout = new QHBoxLayout();
    QLabel* modeLabel = new QLabel("显示模式:", this);
    m_appearanceCombo = new QComboBox(this);
    m_appearanceCombo->addItem("仅图标", 0);
    m_appearanceCombo->addItem("仅文本", 1);
    m_appearanceCombo->addItem("图标和文本", 2);
    m_appearanceCombo->setCurrentIndex(m_button->appearanceMode());
    
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(m_appearanceCombo);
    modeLayout->addStretch();
    
    QHBoxLayout* textLayout = new QHBoxLayout();
    QLabel* textLabel = new QLabel("按钮文本:", this);
    m_textEdit = new QLineEdit(m_button->text(), this);
    textLayout->addWidget(textLabel);
    textLayout->addWidget(m_textEdit);
    
    appearanceLayout->addLayout(modeLayout);
    appearanceLayout->addLayout(textLayout);
    
    // 功能设置 - 左右分组布局
    QHBoxLayout* functionLayout = new QHBoxLayout();
    
    // 左键功能组（左边）
    QVBoxLayout* leftActionLayout = new QVBoxLayout();
    QLabel* leftHeaderLabel = new QLabel("鼠标左键", this);
    leftHeaderLabel->setAlignment(Qt::AlignCenter);
    leftHeaderLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    QLabel* leftTooltipLabel = new QLabel("工具提示:", this);
    m_leftTooltipEdit = new QLineEdit(m_button->tooltip(), this);
    
    QLabel* leftCommandLabel = new QLabel("命令:", this);
    m_leftActionEdit = new QTextEdit(m_button->leftClickAction(), this);
    m_leftActionEdit->setFixedSize(300, 400);
    
    leftActionLayout->addWidget(leftHeaderLabel);
    leftActionLayout->addWidget(leftTooltipLabel);
    leftActionLayout->addWidget(m_leftTooltipEdit);
    leftActionLayout->addWidget(leftCommandLabel);
    leftActionLayout->addWidget(m_leftActionEdit);
    
    // 右键功能组（右边）
    QVBoxLayout* rightActionLayout = new QVBoxLayout();
    QLabel* rightHeaderLabel = new QLabel("鼠标右键", this);
    rightHeaderLabel->setAlignment(Qt::AlignCenter);
    rightHeaderLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    QLabel* rightTooltipLabel = new QLabel("工具提示:", this);
    m_rightTooltipEdit = new QLineEdit(m_button->tooltip(), this);
    
    QLabel* rightCommandLabel = new QLabel("命令:", this);
    m_rightActionEdit = new QTextEdit(m_button->rightClickAction(), this);
    m_rightActionEdit->setFixedSize(300, 400);
    
    rightActionLayout->addWidget(rightHeaderLabel);
    rightActionLayout->addWidget(rightTooltipLabel);
    rightActionLayout->addWidget(m_rightTooltipEdit);
    rightActionLayout->addWidget(rightCommandLabel);
    rightActionLayout->addWidget(m_rightActionEdit);
    
    functionLayout->addLayout(leftActionLayout);
    functionLayout->addLayout(rightActionLayout);
    
    // 按钮组
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    QPushButton* okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton* applyButton = buttonBox->addButton("应用", QDialogButtonBox::ApplyRole);
    QPushButton* cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    
    mainLayout->addWidget(appearanceGroup);
    mainLayout->addLayout(functionLayout);
    mainLayout->addWidget(buttonBox);
    
    // 连接信号
    connect(m_appearanceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RhinoButtonEditor::onAppearanceModeChanged);
    connect(okButton, &QPushButton::clicked, this, &RhinoButtonEditor::onOkClicked);
    connect(applyButton, &QPushButton::clicked, this, &RhinoButtonEditor::onApplyClicked);
    connect(cancelButton, &QPushButton::clicked, this, &RhinoButtonEditor::onCancelClicked);
}

void RhinoButtonEditor::onAppearanceModeChanged(int index)
{
    // 实时预览外观变化
    m_button->setAppearanceMode(index);
    m_button->setText(m_textEdit->text());
}

void RhinoButtonEditor::onOkClicked()
{
    updateButtonFromUI();
    accept();
}

void RhinoButtonEditor::onApplyClicked()
{
    updateButtonFromUI();
}

void RhinoButtonEditor::onCancelClicked()
{
    reject();
}

void RhinoButtonEditor::updateButtonFromUI()
{
    m_button->setAppearanceMode(m_appearanceCombo->currentIndex());
    m_button->setText(m_textEdit->text());
    
    // 组合工具提示格式：第一行左键功能，第二行右键功能
    QString leftTooltip = m_leftTooltipEdit->text().trimmed();
    QString rightTooltip = m_rightTooltipEdit->text().trimmed();
    
    QString combinedTooltip;
    if (!leftTooltip.isEmpty() && !rightTooltip.isEmpty()) {
        combinedTooltip = QString("<html><b>左:</b> %1<br><b>右:</b> %2</html>").arg(leftTooltip).arg(rightTooltip);
    } else if (!leftTooltip.isEmpty()) {
        combinedTooltip = QString("<html><b>左:</b> %1</html>").arg(leftTooltip);
    } else if (!rightTooltip.isEmpty()) {
        combinedTooltip = QString("<html><b>右:</b> %1</html>").arg(rightTooltip);
    }
    
    m_button->setToolTip(combinedTooltip);
    m_button->setLeftClickAction(m_leftActionEdit->toPlainText());
    m_button->setRightClickAction(m_rightActionEdit->toPlainText());
}