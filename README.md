# JPEG
同济大学CS《高级程序语言设计2》大作业: 图像压缩TongJi University CS C++ assignment: image compression
## 概述

本项目是为2020年同济大学计算机系高级程序语言设计1期末作业。实现了JPEG压缩算法。

## 功能

本次实验选用频域编码算法，实现了以傅里叶小波算法为核心的JPEG压缩算法，可以将bmp，tiff等多种图片格式压缩为jpeg格式的图片，最终的jpeg图片符合ISO/IECIS 10918-1标准，可以用其他看图软件（如Windows默认的图片查看器）打开。最终压缩图片大小为48KB(49,152 字节)，压缩率为6.21%。除此以外，还实现了对jpeg图片的解码功能，可以读取程序压缩的jpeg图片并显示。

## 备注

1. 这里面的算法太神奇了比如离散余弦变换、Zigzag扫描、差分编码

2. 我居然在高级语言中尝到了一丝硬件编程的感觉😂
