// Copyright runking 2025 All Rights Reserved.

#include "SmartBlueprintFunctionCreatorSettingsCustomization.h"
#include "SmartBlueprintCreatorSettings.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SmartBlueprintFunctionCreatorSettings"

TSharedRef<IDetailCustomization> FSmartBlueprintFunctionCreatorSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FSmartBlueprintFunctionCreatorSettingsCustomization);
}

void FSmartBlueprintFunctionCreatorSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: CustomizeDetails called!"));

	// Set the order of categories
	IDetailCategoryBuilder& GeneralCategory = DetailLayout.EditCategory("General", FText::GetEmpty(), ECategoryPriority::Important);
	IDetailCategoryBuilder& HotkeysCategory = DetailLayout.EditCategory("Hotkeys", FText::GetEmpty(), ECategoryPriority::Important);
	IDetailCategoryBuilder& BehaviorCategory = DetailLayout.EditCategory("Behavior", FText::GetEmpty(), ECategoryPriority::Important);
	IDetailCategoryBuilder& AliasesCategory = DetailLayout.EditCategory("Aliases", FText::GetEmpty(), ECategoryPriority::Default);

	// Attempt to get a mutable default object of our settings class.
       if (USmartBlueprintCreatorSettings* SettingsInstance = GetMutableDefault<USmartBlueprintCreatorSettings>())
       {
	       // General 分类下添加重置按钮
	       GeneralCategory.AddCustomRow(LOCTEXT("ResetAllSearchText", "Reset All"))
	       .ValueContent()
	       .HAlign(HAlign_Center)
	       .VAlign(VAlign_Center)
	       [
		       SNew(SButton)
		       .Text(LOCTEXT("ResetAllButtonText", "Reset All Settings to Default"))
		       .ToolTipText(LOCTEXT("ResetAllButtonTooltip", "Reset all plugin settings (General, Hotkeys, Behavior, Aliases) to their default values"))
		       .OnClicked_Lambda([SettingsInstance]() -> FReply {
			       if (SettingsInstance)
			       {
				       SettingsInstance->ResetAllSettingsToDefaults();
			       }
			       return FReply::Handled();
		       })
	       ];

	       // Behavior 分类下手动控制属性顺序
	       BehaviorCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, DefaultCreationType));
	       BehaviorCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, VariableNodeSpawnMode));

	       // 创建事件别名子分类
	       IDetailCategoryBuilder& EventAliasesCategory = DetailLayout.EditCategory("Event Aliases", FText::GetEmpty(), ECategoryPriority::Default);
	       EventAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, EventKeywordAliases));

	       // 创建函数别名子分类
	       IDetailCategoryBuilder& FunctionAliasesCategory = DetailLayout.EditCategory("Function Aliases", FText::GetEmpty(), ECategoryPriority::Default);
	       FunctionAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, FunctionKeywordAliases));

	       // 创建变量别名子分类
	       IDetailCategoryBuilder& VariableAliasesCategory = DetailLayout.EditCategory("Variable Aliases", FText::GetEmpty(), ECategoryPriority::Default);
	       VariableAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, VariableKeywordAliases));
	       VariableAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, NodeOperationAliases));

	       // 创建宏别名子分类
	       IDetailCategoryBuilder& MacroAliasesCategory = DetailLayout.EditCategory("Macro Aliases", FText::GetEmpty(), ECategoryPriority::Default);
	       MacroAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, MacroKeywordAliases));
	       MacroAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, ParameterDirectionAliases));

	       // 创建数据类型别名子分类
	       IDetailCategoryBuilder& DataTypeAliasesCategory = DetailLayout.EditCategory("Data Type Aliases", FText::GetEmpty(), ECategoryPriority::Default);
	       DataTypeAliasesCategory.AddProperty(GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, DataTypeAliases));


       }
}

#undef LOCTEXT_NAMESPACE 