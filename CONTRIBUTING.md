# 贡献指南

感谢您对 Scalable-Executable-Framework (SEF) 的兴趣！我们欢迎并鼓励社区贡献。本指南将帮助您了解如何为项目做出贡献。

## 📋 开始之前

### 行为准则
- 保持尊重和专业的交流态度
- 欢迎不同经验和背景的贡献者
- 建设性的批评是有益的，人身攻击是不可接受的

### 需要帮助？
- 查看 [README.md](README.md) 了解项目概况
- 在 [GitHub Issues](https://github.com/NimbusAether/Scalable-Executable-Framework/issues) 中搜索相关问题
- 如仍有疑问，请创建新的 Issue 并清楚描述问题

## 🛠️ 开发环境设置

### 前提条件
- **C++17** 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2019+)
- **CMake** 3.12+
- **Git**

### 构建步骤
```bash
# 克隆仓库
git clone https://github.com/NimbusAether/Scalable-Executable-Framework.git
cd Scalable-Executable-Framework

# 查看ThirdParty_Install_Guide.md进行第三方库安装

# 配置和构建
mkdir build && cd build
cmake ..
```

### 开发工具推荐
- **IDE**: 
    - Visual Studio（首选）
    - Visual Studio Code
    - CLion

## 🔧 贡献流程

### 1. 报告问题
发现 Bug 或有功能建议？请通过 GitHub Issues 报告：
- 使用模板提供详细信息
- 包含复现步骤和环境信息
- 对于 Bug，描述期望行为和实际行为

### 2. 功能开发流程

#### 选择任务
- 查看 [Good First Issues](https://github.com/NimbusAether/Scalable-Executable-Framework/issues?q=is%3Aopen+is%3Aissue+label%3A"good+first+issue") 开始
- 或浏览 [项目看板](https://github.com/NimbusAether/Scalable-Executable-Framework/projects) 了解开发路线

#### 分支策略
```bash
# 从 development 分支创建功能分支
git checkout -b feature/your-feature-name development

# 或修复 Bug
git checkout -b fix/issue-description development
```

分支命名约定：
- `feature/` - 新功能
- `fix/` - Bug 修复
- `docs/` - 文档更新
- `test/` - 测试相关
- `refactor/` - 代码重构

### 3. 代码标准

#### C++ 编码规范
- 遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- 使用 clang-format 格式化代码
- 类名使用 PascalCase: `class DataProcessor`
- 函数和变量使用 camelCase: `processSensorData()`
- 常量使用 UPPER_CASE: `const int MAX_BUFFER_SIZE = 1024;`

#### 代码质量要求
```cpp
// 良好的示例
class DataProcessor {
public:
    // 使用有意义的名称和文档
    /// @brief 处理传感器数据并返回结果
    /// @param sensor_data 输入的传感器数据
    /// @return 处理后的数据，如果处理失败返回空对象
    std::optional<ProcessedData> processSensorData(const SensorData& sensor_data) {
        if (!sensor_data.isValid()) {
            SEF_LOG_ERROR("Invalid sensor data received");
            return std::nullopt;
        }
        
        // 业务逻辑...
        return ProcessedData(sensor_data);
    }
    
private:
    int sampling_rate_ = 1000;  // 成员变量以下划线结尾
};
```

### 4. 测试要求
- 为新功能添加单元测试
- 确保所有现有测试通过
- 测试覆盖关键路径和边界条件

```cpp
// 测试示例
TEST(DataProcessorTest, ShouldProcessValidSensorData) {
    DataProcessor processor;
    SensorData valid_data = CreateValidSensorData();
    
    auto result = processor.processSensorData(valid_data);
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getStatus(), DataStatus::PROCESSED);
}
```

### 5. 文档更新
- 更新 README.md 反映重要变更
- 为公共 API 添加 Doxygen 格式的注释
- 更新示例代码

### 6. 提交信息规范
使用约定式提交格式：
```
<类型>[可选的作用域]: <描述>

[可选的正文]

[可选的脚注]
```

类型包括：
- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式调整
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具变动

示例：
```
feat(scheduler): 添加实时任务优先级调度

- 实现基于优先级的抢占式调度算法
- 添加调度器性能监控接口
- 更新相关文档和测试用例

Closes #123
```

### 7. 创建 Pull Request

1. **准备 PR**:
   ```bash
   git push origin your-branch-name
   ```

2. **PR 描述**:
   - 清晰描述变更内容和动机
   - 关联相关 Issue (使用 "Closes #123")
   - 截图或示例（如适用）

3. **代码审查**:
   - 至少需要一名核心维护者批准
   - 响应审查意见并及时更新
   - 确保 CI 检查通过

## 🧪 质量保证

### 预提交检查
在提交 PR 前请确保：
- [ ] 代码通过 clang-format 格式化
- [ ] 所有测试通过
- [ ] 新增代码有适当的测试覆盖
- [ ] 文档已更新
- [ ] 没有引入新的编译器警告

### 运行测试
```bash
cd build
ctest -V  # 运行所有测试并显示详细输出
```

## 📝 许可证

通过贡献代码，您同意您的贡献将在项目的 LGPL v3.0 许可证下发布。

## 🏆 认可贡献者

我们通过以下方式认可贡献者：
- 在 README.md 的贡献者部分列出
- 在发布说明中致谢
- 对重大贡献者提供提交权限

## ❓ 常见问题

**Q: 我是新手，从哪里开始？**
A: 查看标记为 "good first issue" 的问题，或从文档改进开始。

**Q: 如何获取代码审查？**
A: 创建 Draft PR 或在项目 issues 中询问。

**Q: 我的 PR 多久会被审查？**
A: 我们争取在 3-5 个工作日内进行初步审查。

---

感谢您考虑为 SEF 做出贡献！您的每一份贡献都帮助我们构建更好的分布式计算框架。

*有问题？请通过 GitHub Issues 或项目讨论区联系我们。*