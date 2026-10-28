---
title: 蓝图宏自动创建功能实现细节
date: 2024-07-17
---

# 蓝图宏自动创建功能实现细节

## 一、功能概述

基于用户反馈，为 Quick Event Function Creator 插件添加"快速创建蓝图宏（Blueprint Macro）"功能，支持通过命令输入自动创建宏图表，并支持自定义输入输出参数。

## 二、蓝图宏概念与特点

### 1. 蓝图宏（Blueprint Macro）
- **定义**：可重用的节点组，包含多个连接的节点
- **特点**：有输入/输出执行引脚和参数引脚
- **用途**：封装常用逻辑，提高蓝图复用性
- **存储位置**：My Blueprint 面板的 Macros 部分

### 2. 宏 vs 函数 vs 事件
- **宏**：节点组，可包含多个节点，有输入输出引脚
- **函数**：单一节点，有返回值
- **事件**：单一节点，无返回值，通常用于响应

## 三、用户交互与命令语法

### 1. 基础语法
```bash
# 基础宏创建
macro MyMacro
mc MyMacro

# 带参数的宏
macro MyMacro(float InValue)
mc MyMacro(int32 Count, bool bEnabled)
```

### 2. 扩展语法（可选参数）
```bash
# 带输入参数的宏
macro MyMacro input(float InValue, int32 Count)

# 带输出参数的宏
macro MyMacro output(float OutResult)

# 带输入输出参数的宏
macro MyMacro input(float InValue) output(float OutResult)

# 带分类的宏
macro MyMacro category=Utility
```

## 四、已确认可用的技术基础

### 1. 现有 FBlueprintEditorUtils API
- `FBlueprintEditorUtils::CreateNewGraph` - 创建新图表
- `FBlueprintEditorUtils::AddFunctionGraph` - 添加函数图表
- `FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified` - 标记蓝图结构修改
- `FBlueprintEditorUtils::RefreshAllNodes` - 刷新所有节点

### 2. 已有的类型处理系统
- `GetPinTypeFromString()` 函数 - 将字符串转换为 FEdGraphPinType
- 完整的命令解析系统（FunctionParser）
- 错误处理和用户提示机制

### 3. 命令别名系统
- 现有别名：event(e)、func(fn)、server(s)、client(c)、multicast(m)、reliable(rel)、pure(p)、const(k)
- 可用的新别名：macro、mc（避免与现有别名冲突，注意"m"已被multicast占用）

## 五、需要进一步研究的实现细节

### 1. 宏图表创建
**需要确认的 API：**
- `FBlueprintEditorUtils::CreateNewGraph` 的宏类型参数
- 或者使用专门的宏创建 API

**实现思路：**
```cpp
// 伪代码示例
bool CreateMacroGraph(UBlueprint* Blueprint, const FString& MacroName)
{
    // 需要研究具体的 API 调用
    // 可能需要：
    // 1. 创建宏类型的图表
    // 2. 设置图表名称
    // 3. 添加到 Blueprint->MacroGraphs
    // 4. 标记蓝图为已修改
}
```

### 2. 宏输入输出引脚创建
**需要确认的 API：**
- `UK2Node_Tunnel` 相关 API - 创建输入输出隧道节点
- 或者使用专门的宏引脚创建 API

**实现思路：**
```cpp
// 伪代码示例
bool AddMacroInputPin(UEdGraph* MacroGraph, const FString& PinName, const FEdGraphPinType& PinType)
{
    // 需要研究具体的 API 调用
    // 可能需要：
    // 1. 创建 UK2Node_Tunnel 节点
    // 2. 设置为输入类型
    // 3. 设置引脚名称和类型
    // 4. 添加到宏图表
}

bool AddMacroOutputPin(UEdGraph* MacroGraph, const FString& PinName, const FEdGraphPinType& PinType)
{
    // 类似的实现，但设置为输出类型
}
```

### 3. 宏执行引脚
**需要确认的 API：**
- 宏的执行输入引脚（Entry）创建
- 宏的执行输出引脚（Exit）创建

**实现思路：**
```cpp
// 伪代码示例
bool CreateMacroExecutionPins(UEdGraph* MacroGraph)
{
    // 需要研究具体的 API 调用
    // 可能需要：
    // 1. 创建执行输入引脚（Entry）
    // 2. 创建执行输出引脚（Exit）
    // 3. 设置引脚位置和连接
}
```

## 六、宏的特殊考虑

### 1. 宏图表类型
- 需要确认宏图表的特殊类型标识
- 可能需要使用 `EGraphType::GT_Macro` 或类似枚举

### 2. 隧道节点（Tunnel Nodes）
- 宏的输入输出通过隧道节点实现
- 需要研究 `UK2Node_Tunnel` 的创建和配置

### 3. 宏的编译
- 宏需要特殊的编译处理
- 可能需要调用 `FBlueprintEditorUtils::CompileBlueprint` 或类似 API

### 4. 宏的复制
- 宏在蓝图中的复制机制
- 可能需要处理宏的实例化

## 七、实现优先级建议

### 1. 第一阶段（基础功能）
- 实现基础的宏图表创建
- 实现基础的执行引脚创建
- 集成到现有命令解析系统

### 2. 第二阶段（参数支持）
- 添加输入参数引脚创建
- 添加输出参数引脚创建
- 支持多种数据类型

### 3. 第三阶段（高级功能）
- 添加宏分类功能
- 添加宏描述和文档
- 添加宏的复制和粘贴支持

## 八、技术风险与注意事项

### 1. API 验证
- 需要实际验证宏创建相关的 FBlueprintEditorUtils API
- 需要确认隧道节点的创建和配置方法

### 2. 图表类型
- 宏图表与函数/事件图表的区别
- 需要确认正确的图表类型设置

### 3. 引脚管理
- 宏的输入输出引脚管理
- 执行引脚的创建和连接

### 4. 编译和验证
- 宏的编译机制
- 宏的有效性验证

### 5. 错误处理
- 宏名冲突检测
- 参数类型验证
- 图表创建失败处理

## 九、测试计划

### 1. 基础功能测试
- 宏图表创建测试
- 执行引脚创建测试
- 宏名称验证测试

### 2. 参数测试
- 输入参数引脚测试
- 输出参数引脚测试
- 多种数据类型测试

### 3. 边界情况测试
- 无效宏名测试
- 参数类型冲突测试
- 图表创建失败测试

### 4. 集成测试
- 与现有命令系统集成测试
- 宏在蓝图中的使用测试
- 宏的复制粘贴测试

## 十、与现有功能的集成

### 1. 命令解析扩展
- 在 `FunctionParser` 中添加宏命令解析
- 添加 `macro` 和 `mc` 命令支持

### 2. 别名系统扩展
- 在 `SmartBlueprintCreatorSettings` 中添加宏相关别名
- 确保不与现有别名冲突

### 3. UI 集成
- 在现有的输入框中支持宏命令
- 添加宏创建的错误提示

## 十一、参考资料

- Unreal Engine 蓝图宏系统文档
- FBlueprintEditorUtils 类参考
- UK2Node_Tunnel 类参考
- UEdGraph 类参考
- EGraphType 枚举参考 