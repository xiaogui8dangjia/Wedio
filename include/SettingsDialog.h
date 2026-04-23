#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QDialogButtonBox>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

    // 获取文件名设置
    QString getFileName(int index) const;

signals:
    void fileNameChanged(int index, const QString& fileName);
    void applyClicked();

private:
    void setupUI();
    void createGeneralPage();
    void createDisplayPage();
    void createFilePage();
    void createToolsPage();
    void createAdvancedPage();

    // UI components
    QHBoxLayout* mainLayout;
    QListWidget* categoryList;
    QStackedWidget* contentStack;
    QDialogButtonBox* buttonBox;

    // Settings pages
    QWidget* generalPage;
    QWidget* displayPage;
    QWidget* filePage;
    QWidget* toolsPage;
    QWidget* advancedPage;

    // 文件名输入框
    QList<QLineEdit*> fileNameEdits;
    
    // 显示勾选项
    QList<QCheckBox*> showCheckboxes;

private slots:
    void onCategoryChanged(int index);
    void onFileNameChanged();
    void onApplyClicked();

public:
    bool isFileVisible(int index) const;
};

#endif // SETTINGSDIALOG_H