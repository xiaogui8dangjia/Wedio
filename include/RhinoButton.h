#ifndef RHINOBUTTON_H
#define RHINOBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QDialogButtonBox>

class RhinoButtonEditor;

class RhinoButton : public QPushButton
{
    Q_OBJECT

public:
    explicit RhinoButton(const QString& text = "", QWidget* parent = nullptr);
    explicit RhinoButton(const QIcon& icon, const QString& text = "", QWidget* parent = nullptr);
    ~RhinoButton();

    // 按钮属性设置
    void setAppearanceMode(int mode); // 0=图标, 1=文本, 2=图标+文本
    int appearanceMode() const;
    
    void setLeftClickAction(const QString& action);
    QString leftClickAction() const;
    
    void setRightClickAction(const QString& action);
    QString rightClickAction() const;
    
    void setTooltip(const QString& tooltip);
    QString tooltip() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onEditButton();

private:
    void init();
    void startDrag();
    void updateAppearance();
    void moveButtonToPosition(const QPoint& globalPos);
    
    QPoint m_dragStartPosition;
    bool m_isDragging;
    bool m_shiftPressed;
    
    // 按钮属性
    int m_appearanceMode; // 0=图标, 1=文本, 2=图标+文本
    QString m_leftClickAction;
    QString m_rightClickAction;
    QString m_tooltip;
    
    RhinoButtonEditor* m_editor;
};

// 按钮编辑器对话框
class RhinoButtonEditor : public QDialog
{
    Q_OBJECT

public:
    explicit RhinoButtonEditor(RhinoButton* button, QWidget* parent = nullptr);
    
private slots:
    void onAppearanceModeChanged(int index);
    void onOkClicked();
    void onApplyClicked();
    void onCancelClicked();

private:
    void setupUI();
    void updateButtonFromUI();
    
    RhinoButton* m_button;
    
    // UI控件
    QComboBox* m_appearanceCombo;
    QLineEdit* m_textEdit;
    QTextEdit* m_leftActionEdit;
    QTextEdit* m_rightActionEdit;
    QLineEdit* m_leftTooltipEdit;
    QLineEdit* m_rightTooltipEdit;
};

#endif // RHINOBUTTON_H