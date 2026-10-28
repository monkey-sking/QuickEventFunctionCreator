---
title: 蓝图变量自动创建功能实现细节
date: 2024-07-17
---

# 蓝图变量自动创建功能实现细节

## 一、功能概述

基于用户反馈，为 Quick Event Function Creator 插件添加"快速创建变量"功能，支持全局变量（成员变量）和本地变量（局部变量）的自动创建。

## 二、变量类型与作用域

### 1. 全局变量（成员变量）
- **作用域**：整个蓝图类可访问
- **生命周期**：随蓝图实例
- **典型用途**：角色属性、状态、配置等
- **存储位置**：My Blueprint 面板的 Variables 部分

### 2. 本地变量（局部变量）
- **作用域**：仅在当前函数/事件图表内
- **生命周期**：函数/事件执行期间
- **典型用途**：临时计算、循环计数器等
- **存储位置**：函数/事件图表内部

## 三、用户交互与命令语法

### 1. 基础语法
```bash
# 全局变量
var float Health
var int32 Score

# 本地变量
lv float temp
local int32 i
```

### 2. 扩展语法（可选参数）
```bash
# 带可见性
var float Health public
var int32 Score private

# 带分类
var float Health category=Stats
var int32 Score category=Player

# 带默认值
var float Health default=100
var int32 Score default=0

# 组合使用
var float Health public category=Stats default=100
```

## 四、已确认可用的技术基础

### 1. 现有 FBlueprintEditorUtils API
- `FBlueprintEditorUtils::CreateNewGraph` - 创建新图表
- `FBlueprintEditorUtils::AddFunctionGraph` - 添加函数图表
- `FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified` - 标记蓝图结构修改
- `FBlueprintEditorUtils::RefreshAllNodes` - 刷新所有节点
- `FBlueprintEditorUtils::FindOrCreateFunctionResultNode` - 查找或创建函数结果节点

### 2. 已有的类型处理系统
- `GetPinTypeFromString()` 函数 - 将字符串转换为 FEdGraphPinType
- 完整的命令解析系统（FunctionParser）
- 错误处理和用户提示机制

### 3. 命令别名系统
- 现有别名：event(e)、func(fn)、server(s)、client(c)、multicast(m)、reliable(rel)、pure(p)、const(k)
- 可用的新别名：var、lv、local（避免与现有别名冲突）

## 五、需要进一步研究的实现细节

### 1. 全局变量创建
**需要确认的 API：**
- `FBlueprintEditorUtils::AddMemberVariable` - 需要验证是否存在及参数
- 或者通过 `UBlueprint::NewVariables` 数组直接操作
- 或者使用反射系统动态添加属性

**实现思路：**
```cpp
// 伪代码示例
bool CreateGlobalVariable(UBlueprint* Blueprint, const FString& VariableName, const FEdGraphPinType& PinType)
{
    // 需要研究具体的 API 调用
    // 可能需要：
    // 1. 创建 UProperty
    // 2. 添加到 Blueprint->NewVariables
    // 3. 设置属性元数据
    // 4. 标记蓝图为已修改
}
```

### 2. 本地变量创建
**需要确认的 API：**
- `FBlueprintEditorUtils::AddLocalVariable` - 需要确认是否存在
- 或者创建 `UK2Node_Variable` 节点并添加到当前图表

**实现思路：**
```cpp
// 伪代码示例
bool CreateLocalVariable(UEdGraph* CurrentGraph, const FString& VariableName, const FEdGraphPinType& PinType)
{
    // 需要研究具体的 API 调用
    // 可能需要：
    // 1. 创建 UK2Node_Variable 节点
    // 2. 设置变量类型和名称
    // 3. 添加到当前图表
    // 4. 刷新节点显示
}
```

### 3. 变量属性设置
**需要确认的 API：**
- `FBlueprintEditorUtils::SetBlueprintVariableDefaultValue` - 设置默认值
- `FBlueprintEditorUtils::SetBlueprintVariableFlags` - 设置可见性标志
- `FBlueprintEditorUtils::SetBlueprintVariableCategory` - 设置分类

**属性类型：**
- **可见性（Visibility）**：Public、Private、BlueprintReadWrite、BlueprintReadOnly
- **分类（Category）**：变量在蓝图编辑器中的分组
- **默认值（Default Value）**：变量的初始值
- **网络同步（Replication）**：Replicated、RepNotify 等

## 六、实现优先级建议

### 1. 第一阶段（基础功能）
- 实现基础的全局变量创建（类型+名称）
- 实现基础的本地变量创建（类型+名称）
- 集成到现有命令解析系统

### 2. 第二阶段（属性支持）
- 添加可见性设置（Public/Private）
- 添加分类设置
- 添加默认值设置

### 3. 第三阶段（高级功能）
- 添加网络同步属性
- 添加变量复制设置
- 添加变量元数据支持

## 七、技术风险与注意事项

### 1. API 验证
- 需要实际验证所有 FBlueprintEditorUtils API 的存在性和参数
- 可能需要查阅 UE 源码或官方文档确认

### 2. 上下文检查
- 本地变量创建需要确保当前在函数/事件图表中
- 需要处理无效上下文的情况

### 3. 错误处理
- 变量名冲突检测
- 类型验证
- 属性值验证

### 4. 性能考虑
- 大量变量创建时的性能优化
- 蓝图编译时的处理

## 八、测试计划

### 1. 基础功能测试
- 全局变量创建测试
- 本地变量创建测试
- 类型支持测试

### 2. 属性测试
- 可见性设置测试
- 分类设置测试
- 默认值设置测试

### 3. 边界情况测试
- 无效变量名测试
- 类型冲突测试
- 上下文错误测试

## 九、参考资料

- Unreal Engine 蓝图变量系统文档
- FBlueprintEditorUtils 类参考
- UBlueprint 类参考
- UK2Node_Variable 类参考 