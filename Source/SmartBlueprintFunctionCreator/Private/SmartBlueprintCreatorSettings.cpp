// Copyright runking 2025 All Rights Reserved.

#include "SmartBlueprintCreatorSettings.h"
#include "UObject/Object.h"
#include "InputCoreTypes.h"
#include "SmartBlueprintFunctionCreator/Public/SmartBlueprintCreatorSettings.h"

// This file is intentionally structured this way to solve a UBT/UHT compile error
// where the .cpp does not see the UPROPERTY members from the .h
// #include "UObject/Object.h"
// #include "SmartBlueprintCreatorSettings.h"

// Define the static delegate
USmartBlueprintCreatorSettings::FOnToolbarButtonVisibilityChanged USmartBlueprintCreatorSettings::OnToolbarButtonVisibilityChanged;


USmartBlueprintCreatorSettings::USmartBlueprintCreatorSettings()
	: ActivationChord(EKeys::LeftMouseButton, EModifierKey::Shift)
	, bShowUsageTips(true)
{
	// Defaults are initialized in PostInitProperties to handle reset correctly.
}

void USmartBlueprintCreatorSettings::PostInitProperties()
{
	Super::PostInitProperties();

	// If alias maps are empty, populate them with defaults.
#if ENGINE_MAJOR_VERSION >= 5
	if ((EventKeywordAliases.IsEmpty() && FunctionKeywordAliases.IsEmpty() && VariableKeywordAliases.IsEmpty() && NodeOperationAliases.IsEmpty() && MacroKeywordAliases.IsEmpty() && ParameterDirectionAliases.IsEmpty()) || DataTypeAliases.IsEmpty())
#else
	if ((EventKeywordAliases.Num() == 0 && FunctionKeywordAliases.Num() == 0 && VariableKeywordAliases.Num() == 0 && NodeOperationAliases.Num() == 0 && MacroKeywordAliases.Num() == 0 && ParameterDirectionAliases.Num() == 0) || DataTypeAliases.Num() == 0)
#endif
	{
		ResetAliasesToDefaults();
	}
}

void USmartBlueprintCreatorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if toolbar button visibility changed
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(USmartBlueprintCreatorSettings, bShowToolbarButton))
	{
		OnToolbarButtonVisibilityChanged.Broadcast(bShowToolbarButton);
	}

	SaveSettings();
}

void USmartBlueprintCreatorSettings::SaveSettings()
{
#if ENGINE_MAJOR_VERSION >= 5
	TryUpdateDefaultConfigFile(GetDefaultConfigFilename());
#else
	UpdateDefaultConfigFile(GetDefaultConfigFilename());
#endif
	InvalidateCache();
}

void USmartBlueprintCreatorSettings::ResetAliasesToDefaults()
{
	PopulateDefaultAliases();
	SaveSettings();
}

void USmartBlueprintCreatorSettings::ResetAllSettingsToDefaults()
{
	// Reset all settings to their actual default values

	// General settings (from constructor)
	bShowUsageTips = true;  // Constructor default: true
	bShowToolbarButton = true;  // Header default: true

	// Hotkeys (from constructor)
	ActivationChord = FInputChord(EKeys::LeftMouseButton, EModifierKey::Shift);  // Constructor default

	// Behavior settings (from header defaults)
	DefaultCreationType = EDefaultCreationType::Event;  // Header default
	VariableNodeSpawnMode = EVariableNodeSpawnMode::Get;  // Header default

	// Reset aliases to defaults
	PopulateDefaultAliases();

	// Save all changes
	SaveSettings();

	// Notify toolbar button visibility change
	OnToolbarButtonVisibilityChanged.Broadcast(bShowToolbarButton);
}

void USmartBlueprintCreatorSettings::PopulateDefaultAliases()
{
	// Clear existing aliases first
	EventKeywordAliases.Empty();
	FunctionKeywordAliases.Empty();
	VariableKeywordAliases.Empty();
	NodeOperationAliases.Empty();
	MacroKeywordAliases.Empty();
	ParameterDirectionAliases.Empty();
	DataTypeAliases.Empty();

	// -- Event Commands --
	EventKeywordAliases.Add({ "event", {TEXT("e")} });
	EventKeywordAliases.Add({ "server", {TEXT("s")} });
	EventKeywordAliases.Add({ "client", {TEXT("c")} });
	EventKeywordAliases.Add({ "multicast", {TEXT("m")} });
	EventKeywordAliases.Add({ "reliable", {TEXT("rel")} });

	// -- Function Commands --
	FunctionKeywordAliases.Add({ "func", {TEXT("fn")} });
	FunctionKeywordAliases.Add({ "pure", {TEXT("p")} });
	FunctionKeywordAliases.Add({ "const", {TEXT("k")} });

	// -- Variable Commands --
	VariableKeywordAliases.Add({ "var", {TEXT("var")} });
	VariableKeywordAliases.Add({ "local", {TEXT("lv"), TEXT("local")} });

	// -- Node Operations --
	NodeOperationAliases.Add({ "get", {TEXT("g"), TEXT("get")} });
	NodeOperationAliases.Add({ "set", {TEXT("st"), TEXT("set")} });

	// -- Macro Commands --
	MacroKeywordAliases.Add({ "macro", {TEXT("mc"), TEXT("macro")} });

	// -- Parameter Directions --
	ParameterDirectionAliases.Add({ "in", {TEXT("in"), TEXT("input")} });
	ParameterDirectionAliases.Add({ "out", {TEXT("out"), TEXT("output")} });
	ParameterDirectionAliases.Add({ "inout", {TEXT("inout"), TEXT("ref"), TEXT("io")} });

	// -- Data Types --
	DataTypeAliases.Add({ "exec", {TEXT("exec"), TEXT("execute"), TEXT("execution")} });
	DataTypeAliases.Add({ "boolean", {TEXT("b"), TEXT("bool"), TEXT("boolean")} });
	DataTypeAliases.Add({ "byte", {TEXT("u8"), TEXT("uint8"), TEXT("byte")} });
	DataTypeAliases.Add({ "integer", {TEXT("i"), TEXT("int"), TEXT("int32"), TEXT("integer")} });
	DataTypeAliases.Add({ "integer64", {TEXT("i64"), TEXT("int64"), TEXT("long"), TEXT("integer64")} });
	DataTypeAliases.Add({ "float", {TEXT("f"), TEXT("float"), TEXT("double")} });
	DataTypeAliases.Add({ "string", {TEXT("str"), TEXT("string")} });
	DataTypeAliases.Add({ "name", {TEXT("n"), TEXT("name")} });
	DataTypeAliases.Add({ "text", {TEXT("txt"), TEXT("text")} });
	DataTypeAliases.Add({ "vector", {TEXT("v"), TEXT("vec"), TEXT("vec3"), TEXT("vector")} });
	DataTypeAliases.Add({ "rotator", {TEXT("r"), TEXT("rot"), TEXT("rotator")} });
	DataTypeAliases.Add({ "transform", {TEXT("t"), TEXT("xform"), TEXT("transform")} });
}

void USmartBlueprintCreatorSettings::InvalidateCache()
{
	bIsCacheDirty = true;
}

void USmartBlueprintCreatorSettings::RebuildCache() const
{
	// If the source alias list is empty, it's very likely because an empty user config
	// has overwritten the defaults. We re-populate them here to ensure the plugin is robust.
#if ENGINE_MAJOR_VERSION >= 5
	if ((EventKeywordAliases.IsEmpty() && FunctionKeywordAliases.IsEmpty() && VariableKeywordAliases.IsEmpty() && NodeOperationAliases.IsEmpty() && MacroKeywordAliases.IsEmpty() && ParameterDirectionAliases.IsEmpty()) || DataTypeAliases.IsEmpty())
#else
	if ((EventKeywordAliases.Num() == 0 && FunctionKeywordAliases.Num() == 0 && VariableKeywordAliases.Num() == 0 && NodeOperationAliases.Num() == 0 && MacroKeywordAliases.Num() == 0 && ParameterDirectionAliases.Num() == 0) || DataTypeAliases.Num() == 0)
#endif
	{
		// Need to cast away const to fix the in-memory representation.
		// This is safe because we are only populating the defaults, not saving them.
		const_cast<USmartBlueprintCreatorSettings*>(this)->PopulateDefaultAliases();
	}

	ReversedCommandAliasesCache.Empty();

	// Helper lambda to add aliases to cache
	auto AddAliasesToCache = [this](const TArray<FKeywordAliases>& AliasArray) {
		for (const auto& Entry : AliasArray)
		{
			for (const FString& Alias : Entry.Aliases)
			{
				if (!Alias.IsEmpty())
				{
					ReversedCommandAliasesCache.Add(Alias, Entry.Keyword);
				}
			}
		}
	};

	// Add aliases from all categories to cache
	AddAliasesToCache(EventKeywordAliases);
	AddAliasesToCache(FunctionKeywordAliases);
	AddAliasesToCache(VariableKeywordAliases);
	AddAliasesToCache(MacroKeywordAliases);

	ReversedDataTypeAliasesCache.Empty();
	for (const auto& Entry : DataTypeAliases)
	{
		for (const FString& Alias : Entry.Aliases)
		{
			if (!Alias.IsEmpty())
			{
				ReversedDataTypeAliasesCache.Add(Alias, Entry.Keyword);
			}
		}
	}

	bIsCacheDirty = false;
}

const TMap<FString, FString>& USmartBlueprintCreatorSettings::GetReversedCommandAliases() const
{
	if (bIsCacheDirty)
	{
		RebuildCache();
	}
	return ReversedCommandAliasesCache;
}

const TMap<FString, FString>& USmartBlueprintCreatorSettings::GetReversedDataTypeAliases() const
{
	if (bIsCacheDirty)
	{
		RebuildCache();
	}
	return ReversedDataTypeAliasesCache;
}

TMap<FString, FString> USmartBlueprintCreatorSettings::GetCommandAliasMap() const
{
	TMap<FString, FString> AliasMap;

	// Helper lambda to add aliases from an array
	auto AddAliasesFromArray = [&AliasMap](const TArray<FKeywordAliases>& AliasArray) {
		for (const auto& Entry : AliasArray)
		{
			for (const FString& Alias : Entry.Aliases)
			{
				if (!Alias.IsEmpty())
				{
					AliasMap.Add(Alias, Entry.Keyword);
				}
			}
		}
	};

	// Add aliases from all categories
	AddAliasesFromArray(EventKeywordAliases);
	AddAliasesFromArray(FunctionKeywordAliases);
	AddAliasesFromArray(VariableKeywordAliases);
	AddAliasesFromArray(MacroKeywordAliases);

	return AliasMap;
}

TMap<FString, FString> USmartBlueprintCreatorSettings::GetDataTypeAliasMap() const
{
	TMap<FString, FString> AliasMap;
	for (const auto& Entry : DataTypeAliases)
	{
		for (const FString& Alias : Entry.Aliases)
		{
			if (!Alias.IsEmpty())
			{
				AliasMap.Add(Alias, Entry.Keyword);
			}
		}
	}
	return AliasMap;
}

TMap<FString, FString> USmartBlueprintCreatorSettings::GetNodeOperationAliasMap() const
{
	TMap<FString, FString> AliasMap;
	for (const auto& Entry : NodeOperationAliases)
	{
		for (const FString& Alias : Entry.Aliases)
		{
			if (!Alias.IsEmpty())
			{
				AliasMap.Add(Alias, Entry.Keyword);
			}
		}
	}
	return AliasMap;
}

TMap<FString, FString> USmartBlueprintCreatorSettings::GetMacroCommandAliasMap() const
{
	TMap<FString, FString> AliasMap;
	for (const auto& Entry : MacroKeywordAliases)
	{
		for (const FString& Alias : Entry.Aliases)
		{
			if (!Alias.IsEmpty())
			{
				AliasMap.Add(Alias, Entry.Keyword);
			}
		}
	}
	return AliasMap;
}

TMap<FString, FString> USmartBlueprintCreatorSettings::GetParameterDirectionAliasMap() const
{
	TMap<FString, FString> AliasMap;
	for (const auto& Entry : ParameterDirectionAliases)
	{
		for (const FString& Alias : Entry.Aliases)
		{
			if (!Alias.IsEmpty())
			{
				AliasMap.Add(Alias, Entry.Keyword);
			}
		}
	}
	return AliasMap;
}