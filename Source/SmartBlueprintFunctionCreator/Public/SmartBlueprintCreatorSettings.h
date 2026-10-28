
#pragma once
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "VariableNodeSpawnMode.h"
#include "SmartBlueprintCreatorSettings.generated.h"

USTRUCT()
struct FKeywordAliases
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Alias Settings", meta = (DisplayName = "Keyword"))
	FString Keyword;

	UPROPERTY(config, EditAnywhere, Category="Alias Settings", meta = (DisplayName = "Aliases"))
	TArray<FString> Aliases;
};

UENUM()
enum class EDefaultCreationType : uint8
{
	// Default to creating an Event
	Event UMETA(DisplayName = "Event"),
	// Default to creating a Function
	Function UMETA(DisplayName = "Function"),
	// Default to creating a Variable with smart scope detection
	Variable UMETA(DisplayName = "Variable (Smart Scope)")
};

/**
 * Settings for the Quick Event & Function Creator plugin.
 */
UCLASS(config = EditorPerProjectUserSettings, DefaultConfig, meta = (DisplayName = "Quick Event Function Creator"))
class SMARTBLUEPRINTFUNCTIONCREATOR_API USmartBlueprintCreatorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USmartBlueprintCreatorSettings();



	/** When no specific keyword is provided, what should be created by default? */
	UPROPERTY(config, EditAnywhere, Category = "Behavior", meta = (DisplayName = "Default Creation Type", Tooltip = "Sets the default creation type when no specific keyword (like 'event', 'func', 'local') is used. Variable uses smart scope: local in functions, global in event graphs."))
	EDefaultCreationType DefaultCreationType = EDefaultCreationType::Event;

	/** Variable node auto-generation mode (Get/Set/None), defaults to Get */
	UPROPERTY(config, EditAnywhere, Category = "Behavior", meta = (DisplayName = "Variable Node Auto-Spawn Mode", ToolTip = "Automatically generate Get/Set nodes when creating variables. Choose Get, Set, or None."))
	EVariableNodeSpawnMode VariableNodeSpawnMode = EVariableNodeSpawnMode::Get;

	/** Resets all aliases to their default values and saves the settings. */
	UFUNCTION(BlueprintCallable, Category = "Aliases")
	void ResetAliasesToDefaults();

	/** Resets all plugin settings to their default values and saves the settings. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetAllSettingsToDefaults();

	/** Gets a map of command aliases for the parser. Key: Alias, Value: Internal Command. */
	TMap<FString, FString> GetCommandAliasMap() const;

	/** Gets a map of data type aliases for the parser. Key: Alias, Value: Internal Type. */
	TMap<FString, FString> GetDataTypeAliasMap() const;

	/** Gets a map of node operation aliases for the parser. Key: Alias, Value: Internal Operation. */
	TMap<FString, FString> GetNodeOperationAliasMap() const;

	/** Gets a map of macro command aliases for the parser. Key: Alias, Value: Internal Command. */
	TMap<FString, FString> GetMacroCommandAliasMap() const;

	/** Gets a map of parameter direction aliases for the parser. Key: Alias, Value: Internal Direction. */
	TMap<FString, FString> GetParameterDirectionAliasMap() const;

	// UDeveloperSettings interface
	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("QuickEventFunctionCreator"); }
	// End of UDeveloperSettings interface

	// UObject interface
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface

	const TMap<FString, FString>& GetReversedCommandAliases() const;
	const TMap<FString, FString>& GetReversedDataTypeAliases() const;

	void InvalidateCache();
	void SaveSettings();

	/** The key combination that activates the unified creation dialog. */
	UPROPERTY(config, EditAnywhere, Category = "Hotkeys", meta = (Tooltip = "The key combination that activates the unified creation dialog for variables, functions, and events."))
	FInputChord ActivationChord;



	/** Event-related command aliases */
	UPROPERTY(config, EditAnywhere, Category = "Event Aliases", meta = (DisplayName = "Event Commands", Tooltip = "Define aliases for event-related keywords (e.g., 'e' for 'event', 's' for 'server')."))
	TArray<FKeywordAliases> EventKeywordAliases;

	/** Function-related command aliases */
	UPROPERTY(config, EditAnywhere, Category = "Function Aliases", meta = (DisplayName = "Function Commands", Tooltip = "Define aliases for function-related keywords (e.g., 'fn' for 'func', 'p' for 'pure')."))
	TArray<FKeywordAliases> FunctionKeywordAliases;

	/** Variable-related command aliases */
	UPROPERTY(config, EditAnywhere, Category = "Variable Aliases", meta = (DisplayName = "Variable Commands", Tooltip = "Define aliases for variable-related keywords (e.g., 'lv' for 'local')."))
	TArray<FKeywordAliases> VariableKeywordAliases;

	/** Node operation aliases for Get/Set variable nodes */
	UPROPERTY(config, EditAnywhere, Category = "Variable Aliases", meta = (DisplayName = "Node Operations", Tooltip = "Define aliases for node operations. 'get' creates a Get Variable node to read a variable's value. 'set' creates a Set Variable node to write a variable's value."))
	TArray<FKeywordAliases> NodeOperationAliases;

	/** Macro-related command aliases */
	UPROPERTY(config, EditAnywhere, Category = "Macro Aliases", meta = (DisplayName = "Macro Commands", Tooltip = "Define aliases for macro-related keywords (e.g., 'mc' for 'macro')."))
	TArray<FKeywordAliases> MacroKeywordAliases;

	/** Parameter direction aliases for macro parameters */
	UPROPERTY(config, EditAnywhere, Category = "Macro Aliases", meta = (DisplayName = "Parameter Directions", Tooltip = "Define aliases for parameter directions. 'out' creates output parameters, 'inout' creates input/output parameters."))
	TArray<FKeywordAliases> ParameterDirectionAliases;



	/** Defines aliases for data types used in variables and functions. */
	UPROPERTY(config, EditAnywhere, Category = "Aliases", meta = (DisplayName = "Data Type Aliases", Tooltip = "Define aliases for data types (e.g., 'str' for 'string', 'vec3' for 'vector', 'int' for 'int32')."))
	TArray<FKeywordAliases> DataTypeAliases;

	/** Whether to show the unified syntax usage tips below the input box. */
	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (DisplayName = "Show Usage Hints", Tooltip = "Display syntax examples and tips below the input box to help with unified syntax."))
	bool bShowUsageTips = true;

	/** Whether to show the toolbar button in Blueprint Editor. */
	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (DisplayName = "Show Toolbar Button", Tooltip = "Show the Quick Event Function Creator button in Blueprint Editor toolbar."))
	bool bShowToolbarButton = true;

public:
	/** Delegate called when toolbar button visibility changes */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnToolbarButtonVisibilityChanged, bool);
	static FOnToolbarButtonVisibilityChanged OnToolbarButtonVisibilityChanged;

private:
	void PopulateDefaultAliases();
	void RebuildCache() const;

	mutable TMap<FString, FString> ReversedCommandAliasesCache;
	mutable TMap<FString, FString> ReversedDataTypeAliasesCache;
	mutable bool bIsCacheDirty = true;
}; 