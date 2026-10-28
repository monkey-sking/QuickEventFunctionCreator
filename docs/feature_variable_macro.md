---
title: Quick Event Function Creator 新功能开发需求
date: 2024-07-17
---

# Quick Event Function Creator 新功能开发需求文档

## 一、背景与目标

用户反馈希望插件除了现有的“快速创建函数/事件”外，还能一键创建变量（全局/本地）和蓝图宏（Blueprint Macro），并且交互方式与现有功能一致。

## 二、功能需求

### 1. 快速创建变量
- 支持全局变量（成员变量）和本地变量（函数/事件内临时变量）
- 语法示例：
  - 全局变量：`var float Health`、`var int32 Score`
  - 本地变量：`lv float temp`、`local int32 i`
- 可设置类型、名字、默认值、可见性、分类等属性
- 交互方式与现有命令输入框一致

### 2. 快速创建蓝图宏（Macro）
- 支持通过命令输入自动新建蓝图宏（Blueprint Macro）
- 语法示例：`macro MyMacro`、`mc MyMacro(float InValue)`
- 支持自定义输入输出参数
- 交互方式与现有命令输入框一致

## 三、技术可行性分析

### 1. 变量自动创建
- 全局变量：可用 FBlueprintEditorUtils::AddMemberVariable 实现
- 本地变量：需在当前函数/事件图表内插入变量声明节点，技术可行但实现略复杂

### 2. 宏自动创建
- 可用 FBlueprintEditorUtils::CreateNewGraph 并指定宏类型实现
- 参数通过 UK2Node_Tunnel 节点设置

## 四、风险与注意事项
- 本地变量需确保上下文为函数/事件图表
- 宏自动创建涉及节点布局和参数映射，需充分测试
- 需提供错误提示和用法说明

## 五、命令与别名冲突检查结论

- 现有命令别名：
  - event（e）
  - func（fn）
  - server（s）
  - client（c）
  - multicast（m）
  - reliable（rel）
  - pure（p）
  - const（k）
- 现有数据类型别名：
  - v 仅作为 vector 的别名，不涉及 variable
- “m” 已被 multicast 占用，不能再作为 macro 的别名
- “v” 已被 vector 占用，不能直接用作 variable 的缩写，建议用 var 或 lv
- macro、mc、var、lv、local 等未被占用，可直接用作新命令或别名
- 设计新命令/别名时，务必避开已用的 m（multicast）和 v（vector）

## 六、结论与建议
- 推荐优先实现全局变量和宏的自动创建，本地变量可作为进阶功能逐步完善
- 交互和语法建议与现有风格保持一致，便于用户上手
- 新命令/别名设计需避开已用关键词，确保无冲突

