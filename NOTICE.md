# 第三方开源库声明

Scalable-Executable-Framework (SEF) 项目使用了以下优秀的第三方开源库。我们在此对这些项目的贡献者表示衷心感谢。

## 核心依赖库

### libevent
- **许可证**: BSD-3-Clause License
- **来源**: https://github.com/libevent/libevent
- **用途**: 事件驱动网络编程
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/libevent/LICENSE`

### libuv
- **许可证**: MIT License
- **来源**: https://github.com/libuv/libuv
- **用途**: 跨平台异步 I/O
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/libuv/LICENSE`

### lwIP
- **许可证**: BSD-3-Clause License
- **来源**: https://github.com/lwip-tcpip/lwip
- **用途**: 轻量级 TCP/IP 协议栈
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/lwip/LICENSE`

### Protocol Buffers (protobuf)
- **许可证**: BSD-3-Clause License
- **来源**: https://github.com/protocolbuffers/protobuf
- **用途**: 高效数据序列化
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/google/protobuf/LICENSE`

### jsoncpp
- **许可证**: MIT License
- **来源**: https://github.com/open-source-parsers/jsoncpp
- **用途**: JSON 数据的解析和序列化
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/json/LICENSE`

### libpqxx
- **许可证**: BSD-3-Clause License
- **来源**: https://github.com/jtv/libpqxx
- **用途**: PostgreSQL C++ 客户端接口
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/pqxx/LICENSE`

### ZeroMQ
- **许可证**: Mozilla Public License Version 2.0
- **来源**: https://github.com/zeromq/libzmq
- **用途**: 高性能消息队列
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/libzmq/LICENSE`

### c-logger
- **许可证**: MIT License
- **来源**: https://github.com/yksz/c-logger
- **用途**: 轻量级的C语言日志记录
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/c-logger/LICENSE`

### log4z
- **许可证**: MIT License
- **来源**: https://github.com/zsummer/log4z
- **用途**: 轻量级高性能的跨平台日志库
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/log4z/LICENSE`

### vld
- **许可证**: LGPL-2.1 License
- **来源**: https://github.com/KindDragon/vld
- **用途**: 内置的内存泄漏检测功能
- **包含文件**: 头文件
- **许可证文件**: `sys/ThirdParty.win/License/vld/LICENSE`

## 许可证兼容性说明

本项目采用 **LGPL v3.0** 许可证发布。上述所有第三方库的许可证与 LGPL v3.0 兼容：

- **MIT** 和 **BSD-3-Clause** 许可证是高度宽松的，与 LGPL 完全兼容
- **LGPL** 许可证与自身自然兼容

## 使用方式

本项目中包含的第三方库以下列方式使用：

1. **头文件形式**: 大部分库仅包含必要的头文件用于编译时引用
2. **动态链接**: ZeroMQ、VLD 等库建议通过动态链接方式使用

## 版权声明要求

在使用或分发本软件时，您必须：

1. 保留所有原始版权声明和许可证文本
2. 在衍生作品的文档或版权声明中包含本声明文件的内容
3. 对于以源代码形式分发的第三方库，同时分发其完整的许可证文本

## 完整许可证文本

每个第三方库的完整许可证文本可在项目相应目录的 `LICENSE` 文件中找到。

---

*最后更新: 2025年11月*

*此文件根据各第三方库的要求提供归属信息。如有任何遗漏或错误，请联系项目维护者。*