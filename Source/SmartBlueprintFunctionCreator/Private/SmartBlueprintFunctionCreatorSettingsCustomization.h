// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class IDetailCategoryBuilder;
class IPropertyHandle;

class USmartBlueprintCreatorSettings; // Forward declaration

class FSmartBlueprintFunctionCreatorSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

private:
	FReply OnResetToDefaultsClicked();
	void BuildAliasUI(IDetailCategoryBuilder& CategoryBuilder, TSharedPtr<IPropertyHandle> AliasesPropertyHandle, const FString& GroupName);
	void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils);
	void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils);
	
	IDetailLayoutBuilder* CachedDetailBuilder = nullptr;
	TWeakObjectPtr<USmartBlueprintCreatorSettings> Settings;
}; 