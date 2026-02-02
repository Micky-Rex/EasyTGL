# EasyTGL - 轻量级终端图形库

EasyTGL 是一个头文件式的 C++ 终端图形接口库，允许在终端中实现基于文本图形的图形渲染。

## 特性

- **真彩色支持**：24位 RGB 颜色
- **高性能**：支持差量重绘，减少终端重绘开销
- **多平台**：支持 Windows 和 Linux 终端
- **零依赖**：仅需 C++17 标准库

## 快速开始

```cpp
#include "EasyTGL.h"

int main() {
    // 初始化库
    EasyTGL::init();
    
    // 创建屏幕（宽度80像素, 高度24行, 位置在(1,1)）
    EasyTGL::screen screen(80, 24, 1, 1);
    
    // 绘制一个红色像素点
    screen.draw_pixel(EasyTGL::vec2i(10, 5), EasyTGL::rgb(255, 0, 0));
    
    // 绘制一条蓝色线条
    screen.draw_line(
        EasyTGL::vec2i(0, 0), 
        EasyTGL::vec2i(79, 23), 
        EasyTGL::rgb(0, 0, 255)
    );
    
    // 显示到终端(第一帧视口为空, 因此差量更新也可以正常显示)
    screen.display();   // 第一帧, 与使用screen.display(true)等价
    
    // 呈现效果, 等待用户关闭
    getchar();
    
    // 清理资源
    EasyTGL::shut();
    return 0;
}
```

## 核心功能
### 基本图形

- 绘制像素点 (draw_pixel)
- 绘制直线 (draw_line)
- 绘制三角形（填充/线框）(draw_triangle)
- 水平/垂直线 (draw_hline, draw_vline)

### 图像处理
- 图像绘制 (draw_image)
- 视口捕获 (capture)

### 渲染优化
- 差分渲染（仅重绘变化部分）
- 缓冲区预分配

## 系统要求
### 编译器
- C++17 或更高版本

### 终端要求
- 支持 24位真彩色（大多数现代终端都支持）
- 支持 ANSI 转义码（Windows 10+ 的终端，Linux/macOS 的默认终端）

## 配置选项
在包含头文件前定义以下宏以自定义行为：

```cpp
// 禁用输出缓冲区（不推荐）
#define EASYTGL_USE_OUTPUT_BUFFER false

// 调整缓冲区大小
#define EASYTGL_DEFAULT_MAIN_BUFFER_SIZE 32768
#define EASYTGL_DEFAULT_FRAME_BUFFER_SIZE 16384

#include "EasyTGL.h"
```
已知限制
像素宽高比: 终端字符通常高度大于宽度，通过调整 pixel_width 参数近似正方形像素

性能: 大量像素更新可能影响性能，建议使用差分渲染

颜色精度: 终端颜色显示可能因终端模拟器而异

平台差异: Windows 和 Linux 的终端行为略有差异