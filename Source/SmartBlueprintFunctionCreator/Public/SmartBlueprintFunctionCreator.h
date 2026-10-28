// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SmartBlueprintFunctionCreatorModule.h"
#include "InputCoreTypes.h"
#include "Framework/Application/IInputProcessor.h"
#include "FunctionSignature.h"
#include "MacroSignature.h"

class FToolBarBuilder;
class FMenuBuilder;
class SGraphEditor;
class UBlueprint;
class FSmartBlueprintFunctionCreatorInputProcessor;
class SWindow;
class UK2Node_CustomEvent;
class UK2Node;

class FSmartBlueprintFunctionCreatorModule : public ISmartBlueprintFunctionCreatorModule
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnHotkeyPressed(TSharedPtr<SGraphEditor> GraphEditor);
	void OnTextCommitted(const FText& InText, ETextCommit::Type InCommitType, TSharedPtr<SGraphEditor> GraphEditor, TWeakPtr<SWindow> WindowPtr);

private:
	void ShowInputBox(TSharedPtr<SGraphEditor> GraphEditor);
	UK2Node_CustomEvent* CreateEvent(UBlueprint* Blueprint, UEdGraph* Graph, const FVector2D& Location, const FFunctionSignature& Signature);
	UK2Node* CreateFunction(UBlueprint* Blueprint, UEdGraph* OriginalGraph, const FVector2D& Location, const FFunctionSignature& Signature);
	UK2Node* CreateMacro(UBlueprint* Blueprint, UEdGraph* OriginalGraph, const FVector2D& Location, const FFunctionSignature& Signature);

	// Variable creation support (Phase 1): var / local
	bool ParseVariableCommand(const FString& Input,
		const TMap<FString, FString>& CommandAliases,
		const TMap<FString, FString>& DataTypeAliases,
		bool& bOutIsLocal,
		FString& OutType,
		FString& OutName) const;
	bool CreateGlobalVariable(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarType, const FString& VarName, const FVector2D& Location, bool bIsReadOnly = false, bool bIsStatic = false);
	bool CreateLocalVariable(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarType, const FString& VarName, const FVector2D& Location);
	void CreateVariableNode(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarName, bool bIsSet, const FVector2D& Location);

	// Toolbar extension
	void RegisterToolbarExtension();
	void RegisterToolbarExtensionInternal();
	void OnModulesChanged(FName ModuleName, EModuleChangeReason Reason);
	void OnToolbarButtonVisibilityChanged(bool bVisible);
	void OpenPluginSettings();

	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<FSmartBlueprintFunctionCreatorInputProcessor> InputProcessor;
	TWeakPtr<SWindow> CreatorPopupWindow;
};

class FSmartBlueprintFunctionCreatorInputProcessor : public IInputProcessor
{
public:
	FSmartBlueprintFunctionCreatorInputProcessor(TFunction<void(TSharedPtr<SGraphEditor>)> InOnHotkeyPressed);
	
	//~ IInputProcessor interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& KeyEvent) override;
	//~ End IInputProcessor interface

private:
	TSharedPtr<SGraphEditor> GetFocusedGraphEditor() const;
	TFunction<void(TSharedPtr<SGraphEditor>)> OnHotkeyPressed;
	
	/** A flag to ensure we only handle one hotkey event per frame, preventing double-triggers. */
	bool bEventHandledInTick = false;
};