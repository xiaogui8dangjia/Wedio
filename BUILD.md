# 3D建模软件 - 构建说明

## 项目概述
这是一个基于Qt 6和OpenGL的3D建模软件界面雏形，包含完整的菜单栏和3D绘图区。

## 系统要求
- Windows 10或更高版本
- Qt 6.11.0或更高版本
- CMake 3.16或更高版本
- C++17兼容编译器
- OpenGL 3.3或更高版本

## 文件结构
```
Wedio/
├── CMakeLists.txt          # CMake构建配置
├── 3DModelingSoftware.pro  # qmake项目文件
├── BUILD.md               # 构建说明
├── include/               # 头文件目录
│   ├── MainWindow.h      # 主窗口类头文件
│   ├── ThreeDCanvas.h    # 3D绘图区类头文件
│   └── Camera.h          # 相机类头文件
├── src/                  # 源文件目录
│   ├── main.cpp          # 应用程序入口点
│   ├── MainWindow.cpp    # 主窗口实现
│   ├── ThreeDCanvas.cpp  # 3D绘图区实现
│   ├── Camera.cpp        # 相机实现
│   ├── shaders/          # 着色器文件
│   │   ├── basic.vert    # 顶点着色器
│   │   └── basic.frag    # 片段着色器
│   └── resources/        # 资源文件
│       ├── resources.qrc # Qt资源文件
│       ├── style.css     # 样式表
│       └── icons/        # 图标文件
└── .codeartsdoer/        # 设计文档
    └── specs/
        └── 3d-modeling-software/
            ├── spec.md   # 需求规格文档
            ├── design.md # 技术设计文档
            └── tasks.md  # 任务规划文档
```

## 构建方法

### 方法1：使用CMake（推荐）

#### Windows (MinGW)
1. 确保已安装Qt 6.11.0 mingw版本到 `D:\Qt\6.11.0\mingw_64`
2. 确保已安装CMake 3.16或更高版本
3. 打开命令提示符，导航到项目目录
4. 运行以下命令：
   ```bash
   cd Wedio
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="D:/Qt/6.11.0/mingw_64"
   mingw32-make
   ```

#### Windows (Visual Studio)
1. 确保已安装Qt 6.11.0 msvc版本到 `D:\Qt\6.11.0\msvc2022_64`
2. 确保已安装Visual Studio 2022
3. 打开x64 Native Tools Command Prompt for VS 2022
4. 运行以下命令：
   ```bash
   cd Wedio
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="D:/Qt/6.11.0/msvc2022_64"
   cmake --build . --config Release
   ```

### 方法2：使用qmake
1. 确保已安装Qt 6.11.0
2. 打开Qt Creator
3. 打开 `3DModelingSoftware.pro` 文件
4. 配置构建套件（选择正确的Qt版本和编译器）
5. 点击构建按钮

### 方法3：手动编译
1. 设置Qt环境变量：
   ```bash
   set PATH=D:\Qt\6.11.0\mingw_64\bin;%PATH%
   set PATH=D:\Qt\Tools\mingw1120_64\bin;%PATH%
   ```

2. 生成Makefile：
   ```bash
   cd Wedio
   qmake 3DModelingSoftware.pro
   ```

3. 编译：
   ```bash
   mingw32-make
   ```

## 运行应用程序
构建完成后，可执行文件将位于：
- CMake构建：`build/bin/3DModelingSoftware.exe`
- qmake构建：`release/3DModelingSoftware.exe` 或 `debug/3DModelingSoftware.exe`

双击运行可执行文件。

## 功能特性

### 已实现功能
1. **主窗口**：1280x720像素窗口，标题为"3D建模软件"
2. **菜单栏**：包含10个菜单项：
   - 文件(F)：新建、打开、保存、另存为、退出
   - 编辑(E)：撤销、重做、剪切、复制、粘贴（占位）
   - 视图(V)：重置视图、显示网格、显示坐标系
   - 曲线(C)：占位菜单项
   - 浮雕(S)：占位菜单项
   - 3D建模(U)：占位菜单项
   - 工具(L)：占位菜单项
   - 分析(A)：占位菜单项
   - 视窗(W)：占位菜单项
   - 说明(H)：关于、关于Qt

3. **3D绘图区**：
   - 深灰色背景(#2B2B2B)
   - 显示XYZ坐标系（红、绿、蓝轴）
   - 显示参考网格
   - 支持鼠标交互：
     - 左键拖拽：旋转视图
     - 中键拖拽：平移视图
     - 滚轮：缩放视图

4. **界面样式**：
   - 深色主题
   - 自定义样式表
   - 状态栏显示操作反馈

### 技术实现
- 使用Qt 6作为GUI框架
- 使用QOpenGLWidget进行3D渲染
- 使用Qt信号槽机制处理事件
- 模块化设计，便于扩展

## 测试验证
1. **编译测试**：确保项目能成功编译，无错误警告
2. **运行测试**：应用程序能正常启动和关闭
3. **功能测试**：
   - 所有菜单项可点击（占位功能显示状态栏提示）
   - 3D绘图区正确显示坐标系和网格
   - 鼠标交互功能正常工作
   - 窗口大小调整时界面正常
4. **性能测试**：空场景帧率应达到60FPS

## 已知问题
1. 编译环境需要正确配置Qt和编译器路径
2. 某些菜单项功能为占位实现，需要后续开发
3. 需要OpenGL 3.3或更高版本支持

## 后续开发
1. 实现具体的3D建模功能
2. 添加文件保存/加载功能
3. 实现撤销/重做系统
4. 添加更多视图控制选项
5. 优化渲染性能
6. 添加插件系统支持