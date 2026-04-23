#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QApplication>
#include <QScreen>
#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("设置");
    setMinimumSize(600, 600);
    
    // 设置对话框居中显示
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    setupUI();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧分类列表
    categoryList = new QListWidget(this);
    categoryList->setFixedWidth(100);
    categoryList->setStyleSheet(
        "QListWidget {"
        "  background: #f5f5f5;"
        "  border: none;"
        "  border-right: 1px solid #ddd;"
        "}"
        "QListWidget::item {"
        "  padding: 8px 12px;"
        "  border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:selected {"
        "  background: #e6f3ff;"
        "  color: #0066cc;"
        "  border-right: 2px solid #0066cc;"
        "}"
    );
    
    // 添加分类项
    QStringList categories = {"常规", "显示", "文件", "工具", "高级"};
    categoryList->addItems(categories);
    categoryList->setCurrentRow(0);
    
    // 右侧内容区域
    contentStack = new QStackedWidget(this);
    
    // 创建各个设置页面
    createGeneralPage();
    createDisplayPage();
    createFilePage();
    createToolsPage();
    createAdvancedPage();
    
    // 按钮区域
    buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton("确定", QDialogButtonBox::AcceptRole);
    buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
    buttonBox->addButton("应用", QDialogButtonBox::ApplyRole);
    
    // 主布局
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(10);
    
    rightLayout->addWidget(contentStack, 1);
    rightLayout->addWidget(buttonBox);
    
    mainLayout->addWidget(categoryList);
    mainLayout->addLayout(rightLayout, 1);
    
    // 连接信号
    connect(categoryList, &QListWidget::currentRowChanged, this, &SettingsDialog::onCategoryChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // 连接应用按钮信号
    QPushButton* applyButton = buttonBox->button(QDialogButtonBox::Apply);
    if (applyButton) {
        connect(applyButton, &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    }
}

void SettingsDialog::createGeneralPage()
{
    generalPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(generalPage);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 基本设置组
    QGroupBox* basicGroup = new QGroupBox("基本设置", generalPage);
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    
    QComboBox* languageCombo = new QComboBox(generalPage);
    languageCombo->addItems({"简体中文", "English"});
    languageCombo->setFixedWidth(120);
    basicLayout->addRow("语言:", languageCombo);
    
    // 保存间隔（包含自动保存勾选项）
    QHBoxLayout* intervalLayout = new QHBoxLayout();
    intervalLayout->setContentsMargins(0, 0, 0, 0);
    intervalLayout->setSpacing(5);
    
    QCheckBox* autoSaveCheck = new QCheckBox("自动保存", generalPage);
    autoSaveCheck->setChecked(true);
    intervalLayout->addWidget(autoSaveCheck);
    
    QSpinBox* saveIntervalSpin = new QSpinBox(generalPage);
    saveIntervalSpin->setRange(1, 60);
    saveIntervalSpin->setValue(10);
    saveIntervalSpin->setFixedWidth(50);
    intervalLayout->addWidget(saveIntervalSpin);
    
    QLabel* minuteLabel = new QLabel("分钟", generalPage);
    intervalLayout->addWidget(minuteLabel);
    intervalLayout->addStretch();
    
    basicLayout->addRow("保存间隔:", intervalLayout);
    
    // 默认模板文件
    QLineEdit* templatePathEdit = new QLineEdit(generalPage);
    templatePathEdit->setPlaceholderText("默认模板文件路径");
    templatePathEdit->setText("template.sc");
    basicLayout->addRow("默认模板路径:", templatePathEdit);
    
    // 自动保存文件路径
    QLineEdit* autoSavePathEdit = new QLineEdit(generalPage);
    autoSavePathEdit->setPlaceholderText("自动保存文件路径");
    autoSavePathEdit->setText("autosave.sc");
    basicLayout->addRow("自动保存路径:", autoSavePathEdit);
    
    layout->addWidget(basicGroup);
    
    // 单位设置组
    QGroupBox* unitsGroup = new QGroupBox("单位设置", generalPage);
    QFormLayout* unitsLayout = new QFormLayout(unitsGroup);
    
    // 模型单位
    QComboBox* modelUnitsCombo = new QComboBox(generalPage);
    modelUnitsCombo->addItems({"毫米", "厘米", "米", "英寸", "英尺"});
    modelUnitsCombo->setCurrentIndex(0); // 默认毫米
    modelUnitsCombo->setFixedWidth(100);
    unitsLayout->addRow("模型单位:", modelUnitsCombo);
    
    // 绝对公差
    QHBoxLayout* toleranceLayout = new QHBoxLayout();
    toleranceLayout->setContentsMargins(0, 0, 0, 0);
    toleranceLayout->setSpacing(5);
    
    QDoubleSpinBox* absoluteToleranceSpin = new QDoubleSpinBox(generalPage);
    absoluteToleranceSpin->setRange(0.001, 1.0);
    absoluteToleranceSpin->setDecimals(4);
    absoluteToleranceSpin->setValue(0.01);
    absoluteToleranceSpin->setFixedWidth(80);
    toleranceLayout->addWidget(absoluteToleranceSpin);
    
    QLabel* toleranceLabel = new QLabel("mm", generalPage);
    toleranceLayout->addWidget(toleranceLabel);
    toleranceLayout->addStretch();
    
    unitsLayout->addRow("绝对公差:", toleranceLayout);
    
    // 角度公差
    QHBoxLayout* angleLayout = new QHBoxLayout();
    angleLayout->setContentsMargins(0, 0, 0, 0);
    angleLayout->setSpacing(5);
    
    QDoubleSpinBox* angleToleranceSpin = new QDoubleSpinBox(generalPage);
    angleToleranceSpin->setRange(0.01, 5.0);
    angleToleranceSpin->setDecimals(2);
    angleToleranceSpin->setValue(1.0);
    angleToleranceSpin->setFixedWidth(80);
    angleLayout->addWidget(angleToleranceSpin);
    
    QLabel* angleLabel = new QLabel("度", generalPage);
    angleLayout->addWidget(angleLabel);
    angleLayout->addStretch();
    
    unitsLayout->addRow("角度公差:", angleLayout);
    
    layout->addWidget(unitsGroup);
    
    // 显示精度
    QComboBox* displayPrecisionCombo = new QComboBox(generalPage);
    displayPrecisionCombo->addItems({"0", "0.1", "0.01", "0.001"});
    displayPrecisionCombo->setCurrentIndex(2); // 默认0.01
    displayPrecisionCombo->setFixedWidth(100);
    unitsLayout->addRow("显示精度:", displayPrecisionCombo);
    
    layout->addWidget(unitsGroup);
    
    // 便捷文件保存设置组（放在最下面）
    QGroupBox* fileSaveGroup = new QGroupBox("便捷文件保存", generalPage);
    QVBoxLayout* fileSaveLayout = new QVBoxLayout(fileSaveGroup);
    fileSaveLayout->setContentsMargins(10, 10, 10, 10);
    fileSaveLayout->setSpacing(5);
    
    // 创建5个文件设置（文件名 + 保存路径 + 显示控制）
    for (int i = 1; i <= 5; i++) {
        QHBoxLayout* fileLayout = new QHBoxLayout();
        fileLayout->setContentsMargins(0, 0, 0, 0);
        fileLayout->setSpacing(2);
        
        // 文件标签
        QLabel* fileLabel = new QLabel(QString("%1:").arg(i), fileSaveGroup);
        fileLabel->setFixedWidth(10);
        fileLayout->addWidget(fileLabel);
        
        // 文件名输入框（对应文件保存面板中的标签文本）
        QLineEdit* fileNameEdit = new QLineEdit(fileSaveGroup);
        fileNameEdit->setPlaceholderText(QString("文件%1名称").arg(i));
        fileNameEdit->setFixedWidth(120);
        fileNameEdit->setProperty("fileIndex", i-1); // 存储文件索引
        fileNameEdits.append(fileNameEdit);
        fileLayout->addWidget(fileNameEdit);
        
        // 文件保存路径输入框
        QLineEdit* filePathEdit = new QLineEdit(fileSaveGroup);
        filePathEdit->setPlaceholderText(QString("文件%1保存路径").arg(i));
        filePathEdit->setFixedWidth(360);
        fileLayout->addWidget(filePathEdit);
        
        // 显示勾选项
        QCheckBox* showCheckbox = new QCheckBox("显示", fileSaveGroup);
        showCheckbox->setChecked(true); // 默认勾选
        showCheckbox->setProperty("fileIndex", i-1); // 存储文件索引
        showCheckboxes.append(showCheckbox);
        fileLayout->addWidget(showCheckbox);
        
        fileSaveLayout->addLayout(fileLayout);
    }
    
    layout->addWidget(fileSaveGroup);
    layout->addStretch();
    
    contentStack->addWidget(generalPage);
}

void SettingsDialog::createDisplayPage()
{
    displayPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(displayPage);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 显示设置组
    QGroupBox* displayGroup = new QGroupBox("显示设置", displayPage);
    QFormLayout* displayLayout = new QFormLayout(displayGroup);
    
    QComboBox* themeCombo = new QComboBox(displayPage);
    themeCombo->addItems({"浅色主题", "深色主题", "系统主题"});
    displayLayout->addRow("主题:", themeCombo);
    
    QSpinBox* fontSizeSpin = new QSpinBox(displayPage);
    fontSizeSpin->setRange(8, 20);
    fontSizeSpin->setSuffix(" px");
    fontSizeSpin->setValue(12);
    displayLayout->addRow("字体大小:", fontSizeSpin);
    
    QCheckBox* gridCheck = new QCheckBox("显示网格", displayPage);
    gridCheck->setChecked(true);
    displayLayout->addRow("网格显示:", gridCheck);
    
    QCheckBox* axisCheck = new QCheckBox("显示坐标轴", displayPage);
    axisCheck->setChecked(true);
    displayLayout->addRow("坐标轴:", axisCheck);
    
    layout->addWidget(displayGroup);
    layout->addStretch();
    
    contentStack->addWidget(displayPage);
}

void SettingsDialog::createFilePage()
{
    filePage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(filePage);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 文件设置组
    QGroupBox* fileGroup = new QGroupBox("文件设置", filePage);
    QFormLayout* fileLayout = new QFormLayout(fileGroup);
    
    QLineEdit* defaultPathEdit = new QLineEdit(filePage);
    defaultPathEdit->setPlaceholderText("默认保存路径");
    fileLayout->addRow("默认路径:", defaultPathEdit);
    
    QSpinBox* maxBackupSpin = new QSpinBox(filePage);
    maxBackupSpin->setRange(1, 50);
    maxBackupSpin->setSuffix(" 个");
    maxBackupSpin->setValue(10);
    fileLayout->addRow("最大备份数:", maxBackupSpin);
    
    QCheckBox* compressCheck = new QCheckBox("压缩文件", filePage);
    compressCheck->setChecked(false);
    fileLayout->addRow("文件压缩:", compressCheck);
    
    QComboBox* formatCombo = new QComboBox(filePage);
    formatCombo->addItems({"SC格式", "STEP格式", "IGES格式", "STL格式"});
    fileLayout->addRow("默认格式:", formatCombo);
    
    layout->addWidget(fileGroup);
    layout->addStretch();
    
    contentStack->addWidget(filePage);
}

void SettingsDialog::createToolsPage()
{
    toolsPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(toolsPage);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 工具设置组
    QGroupBox* toolsGroup = new QGroupBox("工具设置", toolsPage);
    QFormLayout* toolsLayout = new QFormLayout(toolsGroup);
    
    QDoubleSpinBox* toleranceSpin = new QDoubleSpinBox(toolsPage);
    toleranceSpin->setRange(0.001, 1.0);
    toleranceSpin->setDecimals(3);
    toleranceSpin->setValue(0.01);
    toleranceSpin->setSuffix(" mm");
    toolsLayout->addRow("容差设置:", toleranceSpin);
    
    QCheckBox* snapCheck = new QCheckBox("启用捕捉", toolsPage);
    snapCheck->setChecked(true);
    toolsLayout->addRow("捕捉功能:", snapCheck);
    
    QSpinBox* undoLevelSpin = new QSpinBox(toolsPage);
    undoLevelSpin->setRange(10, 1000);
    undoLevelSpin->setSuffix(" 步");
    undoLevelSpin->setValue(100);
    toolsLayout->addRow("撤销步数:", undoLevelSpin);
    
    QCheckBox* toolTipsCheck = new QCheckBox("显示工具提示", toolsPage);
    toolTipsCheck->setChecked(true);
    toolsLayout->addRow("工具提示:", toolTipsCheck);
    
    layout->addWidget(toolsGroup);
    layout->addStretch();
    
    contentStack->addWidget(toolsPage);
}

void SettingsDialog::createAdvancedPage()
{
    advancedPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(advancedPage);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 高级设置组
    QGroupBox* advancedGroup = new QGroupBox("高级设置", advancedPage);
    QFormLayout* advancedLayout = new QFormLayout(advancedGroup);
    
    QCheckBox* debugCheck = new QCheckBox("启用调试模式", advancedPage);
    debugCheck->setChecked(false);
    advancedLayout->addRow("调试模式:", debugCheck);
    
    QSpinBox* cacheSizeSpin = new QSpinBox(advancedPage);
    cacheSizeSpin->setRange(10, 1000);
    cacheSizeSpin->setSuffix(" MB");
    cacheSizeSpin->setValue(100);
    advancedLayout->addRow("缓存大小:", cacheSizeSpin);
    
    QCheckBox* hardwareAccelCheck = new QCheckBox("启用硬件加速", advancedPage);
    hardwareAccelCheck->setChecked(true);
    advancedLayout->addRow("硬件加速:", hardwareAccelCheck);
    
    QComboBox* renderQualityCombo = new QComboBox(advancedPage);
    renderQualityCombo->addItems({"低质量", "中等质量", "高质量", "最高质量"});
    renderQualityCombo->setCurrentIndex(2);
    advancedLayout->addRow("渲染质量:", renderQualityCombo);
    
    layout->addWidget(advancedGroup);
    layout->addStretch();
    
    contentStack->addWidget(advancedPage);
}

void SettingsDialog::onCategoryChanged(int index)
{
    contentStack->setCurrentIndex(index);
}

void SettingsDialog::onFileNameChanged()
{
    QLineEdit* senderEdit = qobject_cast<QLineEdit*>(sender());
    if (senderEdit) {
        int index = senderEdit->property("fileIndex").toInt();
        QString fileName = senderEdit->text();
        emit fileNameChanged(index, fileName);
    }
}

QString SettingsDialog::getFileName(int index) const
{
    if (index >= 0 && index < fileNameEdits.size()) {
        return fileNameEdits[index]->text();
    }
    return QString();
}

void SettingsDialog::onApplyClicked()
{
    emit applyClicked();
}

bool SettingsDialog::isFileVisible(int index) const
{
    if (index >= 0 && index < showCheckboxes.size()) {
        return showCheckboxes[index]->isChecked();
    }
    return true; // 默认显示
}