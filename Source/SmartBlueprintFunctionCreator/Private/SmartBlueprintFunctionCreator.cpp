// Copyright runking 2025 All Rights Reserved.

#include "SmartBlueprintFunctionCreator.h"
#include "VariableNodeSpawnMode.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableGet.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableSet.h"
#include "Widgets/SWidget.h"
#include "Misc/App.h"

// Settings & UI
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "SFunctionCreatorTextBox.h"
#include "SmartBlueprintCreatorSettings.h"
#include "SmartBlueprintFunctionCreatorSettingsCustomization.h"
#include "ToolMenus.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SButton.h"
#include "InputCoreTypes.h"
#include "Misc/MessageDialog.h"
#include "Framework/Commands/UIAction.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

// Blueprint & Graph
#include "BlueprintEditor.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CustomEvent.h"
#include "Editor/BlueprintGraph/Classes/K2Node_FunctionEntry.h"
#include "Editor/BlueprintGraph/Classes/K2Node_FunctionResult.h"
#include "Editor/BlueprintGraph/Classes/K2Node_Tunnel.h"
#include "Editor/BlueprintGraph/Classes/K2Node_MacroInstance.h"
#include "Editor.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "GraphEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ScopedTransaction.h"
#include "SGraphPanel.h"
#include "Subsystems/AssetEditorSubsystem.h"

// Custom Parser
#include "FunctionSignature.h"
#include "FunctionParser.h"

// Kismet Compiler
#include "KismetCompiler.h"

// Notifications
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FSmartBlueprintFunctionCreatorModule"

namespace
{
FName GetQEFCStyleSetName()
{
#if ENGINE_MAJOR_VERSION >= 5
	return FAppStyle::GetAppStyleSetName();
#else
	return FEditorStyle::GetStyleSetName();
#endif
}
}

// Helper function to create FEdGraphPinType from a string
FEdGraphPinType GetPinTypeFromString(const FString& TypeName)
{
	// First, check our an internal map of "nice names" to engine types.
	// This is faster than a reflection search and handles common cases.
	const FString CleanTypeName = TypeName.ToLower().TrimStartAndEnd();

	// Execution pin type
	if (CleanTypeName == TEXT("exec") || CleanTypeName == TEXT("execute")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Exec, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }

	if (CleanTypeName == TEXT("boolean")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Boolean, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("byte")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Byte, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("integer")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Int, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("integer64")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Int64, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
#if ENGINE_MAJOR_VERSION >= 5
	if (CleanTypeName == TEXT("float")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Real, FName(TEXT("float")), nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("double")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Real, FName(TEXT("double")), nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
#else
	if (CleanTypeName == TEXT("float")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Float, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
#endif
	if (CleanTypeName == TEXT("string")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_String, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("text")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Text, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("name")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Name, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("vector")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FVector>::Get(), EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("rotator")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FRotator>::Get(), EPinContainerType::None, false, FEdGraphTerminalType()); }
	if (CleanTypeName == TEXT("transform")) { return FEdGraphPinType(UEdGraphSchema_K2::PC_Struct, NAME_None, TBaseStructure<FTransform>::Get(), EPinContainerType::None, false, FEdGraphTerminalType()); }

	// If not a built-in "nice name", perform an exhaustive search using reflection.
	// This logic handles stripping common C++ prefixes (A, U, F, E) to find the base type name used by the reflection system.
	TArray<FString> CandidateNames;
	CandidateNames.Add(TypeName); // 1. The name as-is, in case the user typed it correctly.

	// 2. The name without a potential C++ prefix.
	if (TypeName.Len() > 1 && (TypeName.StartsWith(TEXT("A")) || TypeName.StartsWith(TEXT("U")) || TypeName.StartsWith(TEXT("F")) || TypeName.StartsWith(TEXT("E"))))
	{
		// Convention is that the character after the prefix is uppercase.
		if (FChar::IsUpper(TypeName[1]))
		{
			CandidateNames.Add(TypeName.Mid(1));
		}
	}
	
	// Remove duplicates
	TArray<FString> UniqueCandidateNames;
	for (const FString& Name : CandidateNames)
	{
		UniqueCandidateNames.AddUnique(Name);
	}
	
	TArray<FString> BasePaths = { TEXT(""), TEXT("/Script/CoreUObject"), TEXT("/Script/Engine"), TEXT("/Script/PhysicsCore") };
	if (FApp::IsProjectNameEmpty() == false)
	{
		BasePaths.Add(FString::Printf(TEXT("/Script/%s"), FApp::GetProjectName()));
	}

	UObject* FoundType = nullptr;

	for (const FString& Name : UniqueCandidateNames)
	{
		for (const FString& Path : BasePaths)
		{
			const FString FullPath = Path.IsEmpty() ? Name : Path + TEXT(".") + Name;
			
			// Pass 1: Find object already in memory
			FoundType = StaticFindObject(UObject::StaticClass(), nullptr, *FullPath, false);
			if (FoundType) break;

			// Pass 2: Try to load from disk if not found in memory
			FoundType = LoadObject<UObject>(nullptr, *FullPath);
			if (FoundType) break;
		}
		if (FoundType) break;
	}
	
	if (UClass* FoundClass = Cast<UClass>(FoundType))
	{
		return FEdGraphPinType(UEdGraphSchema_K2::PC_Object, NAME_None, FoundClass, EPinContainerType::None, false, FEdGraphTerminalType());
	}
	if (UScriptStruct* FoundStruct = Cast<UScriptStruct>(FoundType))
	{
		return FEdGraphPinType(UEdGraphSchema_K2::PC_Struct, NAME_None, FoundStruct, EPinContainerType::None, false, FEdGraphTerminalType());
	}
	if (UEnum* FoundEnum = Cast<UEnum>(FoundType))
	{
		return FEdGraphPinType(UEdGraphSchema_K2::PC_Byte, NAME_None, FoundEnum, EPinContainerType::None, false, FEdGraphTerminalType());
	}
	
	// Default to a generic Object pin if no type is found. The user can then change it manually.
	return FEdGraphPinType(UEdGraphSchema_K2::PC_Object, NAME_None, UObject::StaticClass(), EPinContainerType::None, false, FEdGraphTerminalType());
}

// --- Module Implementation ---
void FSmartBlueprintFunctionCreatorModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: StartupModule called"));

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Registering settings"));
		SettingsModule->RegisterSettings("Project", "Plugins", "QuickEventFunctionCreator",
			LOCTEXT("QEFCSettingsName", "Quick Event Function Creator"),
			LOCTEXT("QEFCSettingsDescription", "Configure the Quick Event Function Creator plugin with unified syntax for variables, functions, and events."),
			GetMutableDefault<USmartBlueprintCreatorSettings>()
		);
	}

	// The following are UI-related and should not be run in commandlets or game-only modes.
	if (!IsRunningCommandlet() && !IsRunningGame())
	{
		// Register custom detail customization
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: About to register settings customization"));
		if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: PropertyEditor module found, registering customization"));
			PropertyModule->RegisterCustomClassLayout(
				USmartBlueprintCreatorSettings::StaticClass()->GetFName(),
				FOnGetDetailCustomizationInstance::CreateStatic(&FSmartBlueprintFunctionCreatorSettingsCustomization::MakeInstance)
			);
			UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Settings customization registered successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: PropertyEditor module NOT found!"));
		}
	
		// Register input processor
		InputProcessor = MakeShared<FSmartBlueprintFunctionCreatorInputProcessor>([this](TSharedPtr<SGraphEditor> GraphEditor) {
			this->OnHotkeyPressed(GraphEditor);
		});
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);

		// Register toolbar extension
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: About to register toolbar extension"));
		RegisterToolbarExtension();
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Toolbar extension registration completed"));

		// Listen for toolbar button visibility changes
		USmartBlueprintCreatorSettings::OnToolbarButtonVisibilityChanged.AddRaw(this, &FSmartBlueprintFunctionCreatorModule::OnToolbarButtonVisibilityChanged);
	}

	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: StartupModule completed"));
}

void FSmartBlueprintFunctionCreatorModule::ShutdownModule()
{
	// It's generally not necessary to unregister customizations unless you are doing
	// hot-reloading of this module, and even then it can be problematic.
	// We'll leave it registered for the lifetime of the editor.
	/*
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(USmartBlueprintCreatorSettings::StaticClass()->GetFName());
	}
	*/

	// Cleanup toolbar button visibility delegate
	USmartBlueprintCreatorSettings::OnToolbarButtonVisibilityChanged.RemoveAll(this);

	// Unregister settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "QuickEventFunctionCreator");
	}

	// Unregister input processor
	if (InputProcessor.IsValid() && FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

    if (CreatorPopupWindow.IsValid())
    {
        CreatorPopupWindow.Pin()->RequestDestroyWindow();
    }
}

void FSmartBlueprintFunctionCreatorModule::OnHotkeyPressed(TSharedPtr<SGraphEditor> GraphEditor)
{
	ShowInputBox(GraphEditor);
}

void FSmartBlueprintFunctionCreatorModule::ShowInputBox(TSharedPtr<SGraphEditor> GraphEditor)
{
	if(GraphEditor.IsValid())
	{
		const FVector2D CursorPos = FSlateApplication::Get().GetCursorPos();
		
		TSharedRef<SFunctionCreatorTextBox> FunctionCreatorTextBox = SNew(SFunctionCreatorTextBox)
			.OnTextCommitted_Raw(this, &FSmartBlueprintFunctionCreatorModule::OnTextCommitted)
			.GraphEditor(GraphEditor);
		
		FSlateApplication::Get().PushMenu(
			GraphEditor.ToSharedRef(),
			FWidgetPath(),
			FunctionCreatorTextBox,
			CursorPos,
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
		);
	}
}


void FSmartBlueprintFunctionCreatorModule::OnTextCommitted(const FText& InText, ETextCommit::Type InCommitType, TSharedPtr<SGraphEditor> GraphEditor, TWeakPtr<SWindow> WindowPtr)
{
	// With PushMenu, the menu should close automatically. We just need to dismiss it from Slate's perspective.
	FSlateApplication::Get().DismissAllMenus();

	if (InCommitType != ETextCommit::OnEnter || !GraphEditor.IsValid() || InText.IsEmptyOrWhitespace())
	{
		return;
	}

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);

	if (!Graph || !Blueprint) return;

	// Get the location in the graph under the mouse cursor right before we create the node.
	// Use GetPasteLocation2f() if available (UE 5.6+), otherwise fallback to GetPasteLocation()
	#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
		const FVector2f PasteLocation = GraphEditor->GetPasteLocation2f();
		const FVector2D Location(static_cast<double>(PasteLocation.X), static_cast<double>(PasteLocation.Y));
	#else
		const FVector2D Location = GraphEditor->GetPasteLocation();
	#endif

	// Get settings and alias maps
	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	const TMap<FString, FString> CommandAliases = Settings->GetReversedCommandAliases();
	const TMap<FString, FString> DataTypeAliases = Settings->GetReversedDataTypeAliases();
	const TMap<FString, FString> NodeOperationAliases = Settings->GetNodeOperationAliasMap();
	const TMap<FString, FString> ParameterDirectionAliases = Settings->GetParameterDirectionAliasMap();

	// First, try variable commands (Phase 1): var / lv / local / get / set
	{
		bool bIsLocal = false; FString VarType; FString VarName;
		const FString RawInput = InText.ToString();

		// Check for get/set commands first (with alias support)
		TArray<FString> InputTokens;
		RawInput.ParseIntoArrayWS(InputTokens);

		if (InputTokens.Num() >= 2)
		{
			FString FirstToken = InputTokens[0].ToLower();

			// Apply node operation aliases
			if (NodeOperationAliases.Contains(FirstToken))
			{
				FirstToken = NodeOperationAliases[FirstToken];
			}

			if (FirstToken == TEXT("get") || FirstToken == TEXT("set"))
			{
				bool bIsSet = (FirstToken == TEXT("set"));
				FString VarNameOnly = InputTokens[1].TrimStartAndEnd();

				if (!VarNameOnly.IsEmpty())
				{
					const FScopedTransaction Tx(LOCTEXT("Transaction_CreateVariableNode", "Create Variable Node"));
					Graph->Modify();
					CreateVariableNode(Blueprint, Graph, VarNameOnly, bIsSet, Location);
					return;
				}
			}
		}

		// Then try variable creation commands
		if (ParseVariableCommand(RawInput, CommandAliases, DataTypeAliases, bIsLocal, VarType, VarName))
		{
			const FScopedTransaction Tx(LOCTEXT("Transaction_CreateVariable", "Create Variable"));
			Blueprint->Modify(); Graph->Modify();
			   bool bCreated = bIsLocal ? CreateLocalVariable(Blueprint, Graph, VarType, VarName, Location)
									   : CreateGlobalVariable(Blueprint, Graph, VarType, VarName, Location);
			if (!bCreated)
			{
				FNotificationInfo Info(LOCTEXT("VarCreateFailed", "Failed to create variable. Check type/name or context."));
				Info.ExpireDuration = 4.0f; FSlateNotificationManager::Get().AddNotification(Info);
			}
			return; // Do not fallthrough to function/event parsing on variable commands
		}
	}

	// Parse the signature from the committed text (unified: variables/functions/events)
	FFunctionSignature Signature;
	if (FFunctionParser::Parse(InText.ToString(), Signature, CommandAliases, DataTypeAliases, ParameterDirectionAliases))
	{
		// 🧪 第二阶段测试：输出解析结果用于验证
		UE_LOG(LogTemp, Warning, TEXT("🧪 Parse Test - Input: %s"), *InText.ToString());
		UE_LOG(LogTemp, Warning, TEXT("🧪 Parse Test - SignatureType: %d (0=Function, 1=Event, 2=Variable, 3=Macro)"), (int32)Signature.SignatureType);
		UE_LOG(LogTemp, Warning, TEXT("🧪 Parse Test - Name: %s"), *Signature.Name);
		UE_LOG(LogTemp, Warning, TEXT("🧪 Parse Test - Parameters Count: %d"), Signature.Parameters.Num());
		for (int32 i = 0; i < Signature.Parameters.Num(); i++)
		{
			const auto& Param = Signature.Parameters[i];
			UE_LOG(LogTemp, Warning, TEXT("🧪 Parse Test - Param[%d]: %s %s (Direction: %d)"), i, *Param.Type, *Param.Name, (int32)Param.Direction);
		}
	}

	// Handle variable creation through unified parser
		if (Signature.SignatureType == ESignatureType::Variable)
		{
			const FScopedTransaction Tx(LOCTEXT("Transaction_CreateVariable", "Create Variable"));
			Blueprint->Modify(); Graph->Modify();

			bool bCreated = (Signature.VariableScope == EVariableScope::Local)
				? CreateLocalVariable(Blueprint, Graph, Signature.Type, Signature.Name, Location)
				: CreateGlobalVariable(Blueprint, Graph, Signature.Type, Signature.Name, Location, Signature.bIsReadOnly, Signature.bIsStatic);

			if (bCreated)
			{
				FNotificationInfo Info(FText::Format(LOCTEXT("VarCreated", "Created {0} variable '{1}'"),
					(Signature.VariableScope == EVariableScope::Local) ? LOCTEXT("Local", "local") : LOCTEXT("Global", "global"),
					FText::FromString(Signature.Name)));
				Info.ExpireDuration = 2.5f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
			else
			{
				FNotificationInfo Info(LOCTEXT("VarCreateFailed", "Failed to create variable. Check type/name or context."));
				Info.ExpireDuration = 4.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
			return;
		}
		// Macro parsing successful, continue to creation logic
		// If no specific keyword was found, use the user's default setting
		if (!Signature.bWasTypeKeywordFound)
		{
			if (Settings->DefaultCreationType == EDefaultCreationType::Variable)
			{
				// Convert to variable signature with smart scope detection
				Signature.SignatureType = ESignatureType::Variable;

				// Smart scope: local in functions, global in event graphs
				if (Graph->GetSchema()->GetGraphType(Graph) == GT_Function)
				{
					Signature.VariableScope = EVariableScope::Local;
				}
				else
				{
					Signature.VariableScope = EVariableScope::Global;
				}

				// For variable creation, we need type and name
				// If the input doesn't look like "type name", treat as simple name with default type
				TArray<FString> InputWords;
				InText.ToString().TrimStartAndEnd().ParseIntoArray(InputWords, TEXT(" "), true);

				if (InputWords.Num() == 1)
				{
					// Single word input, assume it's a variable name with default type
					Signature.Type = TEXT("float");  // Default type
					Signature.Name = InputWords[0];
				}
				else if (InputWords.Num() >= 2)
				{
					// Multiple words, assume "type name" format
					Signature.Type = InputWords[0];
					Signature.Name = InputWords[1];
				}
				else
				{
					// Invalid input
					return;
				}

				// Handle variable creation
				const FScopedTransaction Tx(LOCTEXT("Transaction_CreateVariable", "Create Variable"));
				Blueprint->Modify(); Graph->Modify();

				bool bCreated = (Signature.VariableScope == EVariableScope::Local)
					? CreateLocalVariable(Blueprint, Graph, Signature.Type, Signature.Name, Location)
					: CreateGlobalVariable(Blueprint, Graph, Signature.Type, Signature.Name, Location, Signature.bIsReadOnly, Signature.bIsStatic);

				if (bCreated)
				{
					FNotificationInfo Info(FText::Format(LOCTEXT("VarCreated", "Created {0} variable '{1}'"),
						(Signature.VariableScope == EVariableScope::Local) ? LOCTEXT("Local", "local") : LOCTEXT("Global", "global"),
						FText::FromString(Signature.Name)));
					Info.ExpireDuration = 2.5f;
					FSlateNotificationManager::Get().AddNotification(Info);
				}
				else
				{
					FNotificationInfo Info(LOCTEXT("VarCreateFailed", "Failed to create variable. Check type/name or context."));
					Info.ExpireDuration = 4.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				}
				return;
			}
			else
			{
				// Traditional function/event logic
				Signature.bIsFunction = (Settings->DefaultCreationType == EDefaultCreationType::Function);
			}
		}

		// Check if trying to create an event inside a function graph
		if (!Signature.bIsFunction && Graph->GetSchema()->GetGraphType(Graph) == GT_Function)
		{
			FNotificationInfo Info(LOCTEXT("CannotCreateEventInFunction", "Events can only be created in the Event Graph."));
			Info.ExpireDuration = 5.0f;
			FSlateNotificationManager::Get().AddNotification(Info);
			return;
		}

		const FScopedTransaction Transaction(LOCTEXT("Transaction", "Create Blueprint Element"));
		Blueprint->Modify();
		Graph->Modify();

		UK2Node* NewNode = nullptr;

		if (Signature.SignatureType == ESignatureType::Function)
		{
			NewNode = CreateFunction(Blueprint, Graph, Location, Signature);
		}
		else if (Signature.SignatureType == ESignatureType::Macro)
		{
			NewNode = CreateMacro(Blueprint, Graph, Location, Signature);
		}
		else // It's an Event
		{
			NewNode = CreateEvent(Blueprint, Graph, Location, Signature);
		}

		if (NewNode)
		{
			GraphEditor->ClearSelectionSet();
			GraphEditor->SetNodeSelection(NewNode, true);
		}
	}

UK2Node_CustomEvent* FSmartBlueprintFunctionCreatorModule::CreateEvent(UBlueprint* Blueprint, UEdGraph* Graph, const FVector2D& Location, const FFunctionSignature& Signature)
{
	UK2Node_CustomEvent* EventNode = NewObject<UK2Node_CustomEvent>(Graph);
	EventNode->CreateNewGuid();
	EventNode->CustomFunctionName = FName(*Signature.FunctionName);
	EventNode->NodePosX = Location.X;
	EventNode->NodePosY = Location.Y;
	EventNode->FunctionFlags = Signature.GetFunctionFlags();
	
	Graph->AddNode(EventNode, true, false);

	EventNode->AllocateDefaultPins();
	
	// For events, all parameters are treated as output pins on the node.
	for (const auto& Param : Signature.Parameters)
	{
		FEdGraphPinType PinType = GetPinTypeFromString(Param.Type);
		EventNode->CreateUserDefinedPin(FName(*Param.Name), PinType, EGPD_Output, false);
	}

	EventNode->ReconstructNode();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
	return EventNode;
}

UK2Node* FSmartBlueprintFunctionCreatorModule::CreateFunction(UBlueprint* Blueprint, UEdGraph* OriginalGraph, const FVector2D& Location, const FFunctionSignature& Signature)
{
	const FName FunctionName = FName(*Signature.FunctionName);

	UEdGraph* NewFunctionGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	
	// By passing a statically-typed nullptr, we explicitly tell the engine
	// that this is a new base function, not an override of a function from a parent class or interface.
	// This resolves the template deduction failure and correctly creates a new user-defined function.
	FBlueprintEditorUtils::AddFunctionGraph(Blueprint, NewFunctionGraph, /*bIsUserCreated=*/ true, static_cast<UClass*>(nullptr));

	TArray<UK2Node_FunctionEntry*> EntryNodes;
	NewFunctionGraph->GetNodesOfClass(EntryNodes);
	UK2Node_FunctionEntry* EntryNode = (EntryNodes.Num() > 0) ? EntryNodes[0] : nullptr;

	if (EntryNode)
	{
		const FScopedTransaction Transaction(LOCTEXT("CreateFunction", "Create Function"));
		Blueprint->Modify();
		EntryNode->Modify();

		UK2Node* NodeToReturn = EntryNode;

		EntryNode->NodePosX = Location.X;
		EntryNode->NodePosY = Location.Y;

		// --- Set Function Flags (Pure, Const, etc.) ---
		if (Signature.bIsPure) EntryNode->AddExtraFlags(FUNC_BlueprintPure);
		if (Signature.bIsConst) EntryNode->AddExtraFlags(FUNC_Const);

		// --- Create Input Pins (Output pins on the Entry node) ---
		for (const auto& Param : Signature.Parameters)
		{
			FEdGraphPinType PinType = GetPinTypeFromString(Param.Type);
			EntryNode->CreateUserDefinedPin(FName(*Param.Name), PinType, EGPD_Output, false);
		}

		EntryNode->ReconstructNode();

		// For Pure functions, the Result node should not have an execution pin.
		if (Signature.bIsPure)
		{
			if (UK2Node_FunctionResult* ResultNode = FBlueprintEditorUtils::FindOrCreateFunctionResultNode(EntryNode))
			{
				UEdGraphPin* ExecPin = ResultNode->FindPin(UEdGraphSchema_K2::PN_Execute);
				if (ExecPin)
				{
					ResultNode->Modify();
					ResultNode->RemovePin(ExecPin);
					ResultNode->ReconstructNode();
				}
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		// --- Auto-place Call Function Node in Original Graph ---
		if (OriginalGraph && OriginalGraph != NewFunctionGraph)
		{
			UK2Node_CallFunction* CallNode = NewObject<UK2Node_CallFunction>(OriginalGraph);
			CallNode->CreateNewGuid();
			CallNode->NodePosX = Location.X;
			CallNode->NodePosY = Location.Y;
			CallNode->SetFromFunction(Blueprint->SkeletonGeneratedClass->FindFunctionByName(FunctionName));

			OriginalGraph->AddNode(CallNode, true, true);
			CallNode->ReconstructNode();
			NodeToReturn = CallNode;
		}
		
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		return NodeToReturn;
	}
	return nullptr;
}


// --- FSmartBlueprintFunctionCreatorInputProcessor Implementation ---

FSmartBlueprintFunctionCreatorInputProcessor::FSmartBlueprintFunctionCreatorInputProcessor(TFunction<void(TSharedPtr<SGraphEditor>)> InOnHotkeyPressed)
	: OnHotkeyPressed(InOnHotkeyPressed)
{}

void FSmartBlueprintFunctionCreatorInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	bEventHandledInTick = false;
}

bool FSmartBlueprintFunctionCreatorInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (bEventHandledInTick) return false;

	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	const FModifierKeysState Modifiers = MouseEvent.GetModifierKeys();
	const FInputChord CurrentChord(MouseEvent.GetEffectingButton(), Modifiers.IsShiftDown(), Modifiers.IsControlDown(), Modifiers.IsAltDown(), Modifiers.IsCommandDown());

	if (CurrentChord == Settings->ActivationChord)
	{
		TSharedPtr<SGraphEditor> GraphEditor = GetFocusedGraphEditor();
		if (GraphEditor.IsValid() && OnHotkeyPressed)
		{
			bEventHandledInTick = true;
			OnHotkeyPressed(GraphEditor);
			return true;
		}
	}

	return false;
}

bool FSmartBlueprintFunctionCreatorInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& KeyEvent)
{
	if (bEventHandledInTick) return false;
	
	// We don't want to trigger on just a modifier key press
	if(KeyEvent.GetKey().IsModifierKey()) return false;

	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	const FModifierKeysState Modifiers = KeyEvent.GetModifierKeys();
	const FInputChord CurrentChord(KeyEvent.GetKey(), Modifiers.IsShiftDown(), Modifiers.IsControlDown(), Modifiers.IsAltDown(), Modifiers.IsCommandDown());

	if (CurrentChord == Settings->ActivationChord)
	{
		TSharedPtr<SGraphEditor> GraphEditor = GetFocusedGraphEditor();
		if (GraphEditor.IsValid() && OnHotkeyPressed)
		{
			bEventHandledInTick = true;
			OnHotkeyPressed(GraphEditor);
			return true;
		}
	}
	
	return false;
}

TSharedPtr<SGraphEditor> FSmartBlueprintFunctionCreatorInputProcessor::GetFocusedGraphEditor() const
{
	// First, try to get the keyboard-focused widget. This is the most reliable way.
	TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
    if (!FocusedWidget.IsValid())
    {
        FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);
    }

	TSharedPtr<SWidget> CurrentWidget = FocusedWidget;
	while (CurrentWidget.IsValid())
	{
		if (CurrentWidget->GetTypeAsString() == "SGraphEditor")
		{
			return StaticCastSharedPtr<SGraphEditor>(CurrentWidget);
		}
		CurrentWidget = CurrentWidget->GetParentWidget();
	}
	return nullptr;
}


bool FSmartBlueprintFunctionCreatorModule::ParseVariableCommand(const FString& Input,
	const TMap<FString, FString>& CommandAliases,
	const TMap<FString, FString>& DataTypeAliases,
	bool& bOutIsLocal,
	FString& OutType,
	FString& OutName) const
{
	bOutIsLocal = false; OutType.Reset(); OutName.Reset();
	FString Working = Input.TrimStartAndEnd();
	if (Working.IsEmpty()) return false;

	TArray<FString> Tokens;
	Working.ParseIntoArrayWS(Tokens);
	if (Tokens.Num() < 3) return false;

	auto NormalizeCmd = [&](const FString& S)->FString {
		const FString* Canon = CommandAliases.Find(S.ToLower());
		return Canon ? *Canon : S;
	};

	const FString Cmd = NormalizeCmd(Tokens[0]);
	if (!Cmd.Equals(TEXT("var"), ESearchCase::IgnoreCase) &&
		!Cmd.Equals(TEXT("local"), ESearchCase::IgnoreCase))
	{
		// Not a variable command
		return false;
	}

	bOutIsLocal = Cmd.Equals(TEXT("local"), ESearchCase::IgnoreCase);

	FString Type = Tokens[1];
	if (const FString* Mapped = DataTypeAliases.Find(Type.ToLower()))
	{
		Type = *Mapped; // use canonical internal type name
	}
	else
	{
		// Accept canonical C++ names directly
		const FString Low = Type.ToLower();
		if (Low == TEXT("int32") || Low == TEXT("int")) { Type = TEXT("integer"); }
		else if (Low == TEXT("int64") || Low == TEXT("long")) { Type = TEXT("integer64"); }
		else if (Low == TEXT("uint8") || Low == TEXT("u8")) { Type = TEXT("byte"); }
	}
	OutType = Type.TrimStartAndEnd();
	OutName = Tokens[2].TrimStartAndEnd();
	return !OutType.IsEmpty() && !OutName.IsEmpty();
}

bool FSmartBlueprintFunctionCreatorModule::CreateGlobalVariable(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarType, const FString& VarName, const FVector2D& Location, bool bIsReadOnly, bool bIsStatic)
{
	if (!Blueprint) return false;

	// Check name conflict
	for (const FBPVariableDescription& Desc : Blueprint->NewVariables)
	{
		if (Desc.VarName == FName(*VarName))
		{
			FNotificationInfo Info(FText::Format(LOCTEXT("GlobalVarExists", "Variable '{0}' already exists."), FText::FromString(VarName)));
			Info.ExpireDuration = 4.0f; FSlateNotificationManager::Get().AddNotification(Info);
			return false;
		}
	}

	FEdGraphPinType PinType = GetPinTypeFromString(VarType);
	const bool bAdded = FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*VarName), PinType);
	if (bAdded)
	{
		// Apply variable attributes
		for (FBPVariableDescription& VarDesc : Blueprint->NewVariables)
		{
			if (VarDesc.VarName == FName(*VarName))
			{
				if (bIsReadOnly)
				{
					VarDesc.PropertyFlags |= CPF_BlueprintReadOnly;
				}
				if (bIsStatic)
				{
					// Note: Static variables in Blueprint are not directly supported
					// This is a placeholder for future implementation
					VarDesc.PropertyFlags |= CPF_DisableEditOnInstance;
				}
				break;
			}
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);

		FString AttributeText = "";
		if (bIsReadOnly) AttributeText += "readonly ";
		if (bIsStatic) AttributeText += "static ";

		FNotificationInfo Info(FText::Format(LOCTEXT("GlobalVarCreated", "Created {0}global variable '{1}'"),
			FText::FromString(AttributeText), FText::FromString(VarName)));
		Info.ExpireDuration = 2.5f; FSlateNotificationManager::Get().AddNotification(Info);

		// 自动生成Get/Set节点
		const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
		if (Settings && Graph)
		{
			// 使用鼠标位置
			int32 NodePosX = Location.X;
			int32 NodePosY = Location.Y;

			switch (Settings->VariableNodeSpawnMode)
			{
				case EVariableNodeSpawnMode::Get:
				{
					UK2Node_VariableGet* GetNode = NewObject<UK2Node_VariableGet>(Graph);
					GetNode->VariableReference.SetSelfMember(FName(*VarName));
					GetNode->NodePosX = NodePosX;
					GetNode->NodePosY = NodePosY;
					GetNode->NodeGuid = FGuid::NewGuid(); // 生成唯一 GUID
					Graph->AddNode(GetNode, true, true);
					GetNode->AllocateDefaultPins();
					GetNode->ReconstructNode();
					FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
					break;
				}
				case EVariableNodeSpawnMode::Set:
				{
					UK2Node_VariableSet* SetNode = NewObject<UK2Node_VariableSet>(Graph);
					SetNode->VariableReference.SetSelfMember(FName(*VarName));
					SetNode->NodePosX = NodePosX;
					SetNode->NodePosY = NodePosY;
					SetNode->NodeGuid = FGuid::NewGuid(); // 生成唯一 GUID
					Graph->AddNode(SetNode, true, true);
					SetNode->AllocateDefaultPins();
					SetNode->ReconstructNode();
					FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
					break;
				}
				case EVariableNodeSpawnMode::None:
				default:
					break;
			}
		}
	}
	return bAdded;
}

bool FSmartBlueprintFunctionCreatorModule::CreateLocalVariable(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarType, const FString& VarName, const FVector2D& Location)
{
	if (!Blueprint || !Graph) return false;
	if (Graph->GetSchema()->GetGraphType(Graph) != GT_Function)
	{
		FNotificationInfo Info(LOCTEXT("LocalVarOnlyInFunction", "Local variables are only supported inside function graphs."));
		Info.ExpireDuration = 4.0f; FSlateNotificationManager::Get().AddNotification(Info);
		return false;
	}

	TArray<UK2Node_FunctionEntry*> EntryNodes;
	Graph->GetNodesOfClass(EntryNodes);
	UK2Node_FunctionEntry* Entry = EntryNodes.Num() > 0 ? EntryNodes[0] : nullptr;
	if (!Entry)
	{
		return false;
	}

	// Check conflict
	for (const FBPVariableDescription& LocalDesc : Entry->LocalVariables)
	{
		if (LocalDesc.VarName == FName(*VarName))
		{
			FNotificationInfo Info(FText::Format(LOCTEXT("LocalVarExists", "Local variable '{0}' already exists."), FText::FromString(VarName)));
			Info.ExpireDuration = 4.0f; FSlateNotificationManager::Get().AddNotification(Info);
			return false;
		}
	}

	FBPVariableDescription NewLocal;
	NewLocal.VarName = FName(*VarName);
	NewLocal.VarType = GetPinTypeFromString(VarType);
	Entry->LocalVariables.Add(NewLocal);

	Entry->ReconstructNode();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	FBlueprintEditorUtils::RefreshAllNodes(Blueprint);

	// 自动生成Get/Set节点（与全局变量相同的逻辑）
	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	if (Settings && Graph)
	{
		// 使用鼠标位置
		int32 NodePosX = Location.X;
		int32 NodePosY = Location.Y;

		switch (Settings->VariableNodeSpawnMode)
		{
			case EVariableNodeSpawnMode::Get:
			{
				UK2Node_VariableGet* GetNode = NewObject<UK2Node_VariableGet>(Graph);
				// 找到局部变量的描述来获取 GUID
				FGuid LocalVarGuid;
				for (const FBPVariableDescription& LocalDesc : Entry->LocalVariables)
				{
					if (LocalDesc.VarName == FName(*VarName))
					{
						LocalVarGuid = LocalDesc.VarGuid;
						break;
					}
				}
				GetNode->VariableReference.SetLocalMember(FName(*VarName), Graph->GetFName().ToString(), LocalVarGuid);
				GetNode->NodePosX = NodePosX;
				GetNode->NodePosY = NodePosY;
				GetNode->NodeGuid = FGuid::NewGuid();
				Graph->AddNode(GetNode, true, true);
				GetNode->AllocateDefaultPins();
				GetNode->ReconstructNode();
				FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
				break;
			}
			case EVariableNodeSpawnMode::Set:
			{
				UK2Node_VariableSet* SetNode = NewObject<UK2Node_VariableSet>(Graph);
				// 找到局部变量的描述来获取 GUID
				FGuid LocalVarGuid;
				for (const FBPVariableDescription& LocalDesc : Entry->LocalVariables)
				{
					if (LocalDesc.VarName == FName(*VarName))
					{
						LocalVarGuid = LocalDesc.VarGuid;
						break;
					}
				}
				SetNode->VariableReference.SetLocalMember(FName(*VarName), Graph->GetFName().ToString(), LocalVarGuid);
				SetNode->NodePosX = NodePosX;
				SetNode->NodePosY = NodePosY;
				SetNode->NodeGuid = FGuid::NewGuid();
				Graph->AddNode(SetNode, true, true);
				SetNode->AllocateDefaultPins();
				SetNode->ReconstructNode();
				FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
				break;
			}
			case EVariableNodeSpawnMode::None:
			default:
				break;
		}
	}

	FNotificationInfo Info(FText::Format(LOCTEXT("LocalVarCreated", "Created local variable '{0}'"), FText::FromString(VarName)));
	Info.ExpireDuration = 2.5f; FSlateNotificationManager::Get().AddNotification(Info);
	return true;
}

void FSmartBlueprintFunctionCreatorModule::CreateVariableNode(UBlueprint* Blueprint, UEdGraph* Graph, const FString& VarName, bool bIsSet, const FVector2D& Location)
{
	if (!Blueprint || !Graph)
	{
		FNotificationInfo Info(LOCTEXT("InvalidContext", "Invalid blueprint or graph context."));
		Info.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
		return;
	}

	// 使用鼠标位置
	int32 NodePosX = Location.X;
	int32 NodePosY = Location.Y;

	// Check if variable exists (global or local)
	bool bVariableExists = false;
	bool bIsLocalVariable = false;
	UK2Node_FunctionEntry* FunctionEntry = nullptr;

	// First check global variables
	for (const FBPVariableDescription& Desc : Blueprint->NewVariables)
	{
		if (Desc.VarName == FName(*VarName))
		{
			bVariableExists = true;
			break;
		}
	}

	// If not found in global, check local variables
	if (!bVariableExists && Graph->GetSchema()->GetGraphType(Graph) == GT_Function)
	{
		TArray<UK2Node_FunctionEntry*> EntryNodes;
		Graph->GetNodesOfClass(EntryNodes);
		if (EntryNodes.Num() > 0)
		{
			FunctionEntry = EntryNodes[0];
			for (const FBPVariableDescription& LocalDesc : FunctionEntry->LocalVariables)
			{
				if (LocalDesc.VarName == FName(*VarName))
				{
					bVariableExists = true;
					bIsLocalVariable = true;
					break;
				}
			}
		}
	}

	if (!bVariableExists)
	{
		FNotificationInfo Info(FText::Format(LOCTEXT("VarNotFound", "Variable '{0}' not found. Create it first with 'type {0}' or 'local type {0}'."), FText::FromString(VarName)));
		Info.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
		return;
	}

	// Create the appropriate node
	if (bIsSet)
	{
		UK2Node_VariableSet* SetNode = NewObject<UK2Node_VariableSet>(Graph);
		if (bIsLocalVariable && FunctionEntry)
		{
			// 找到局部变量的描述来获取 GUID
			FGuid LocalVarGuid;
			for (const FBPVariableDescription& LocalDesc : FunctionEntry->LocalVariables)
			{
				if (LocalDesc.VarName == FName(*VarName))
				{
					LocalVarGuid = LocalDesc.VarGuid;
					break;
				}
			}
			SetNode->VariableReference.SetLocalMember(FName(*VarName), Graph->GetFName().ToString(), LocalVarGuid);
		}
		else
		{
			SetNode->VariableReference.SetSelfMember(FName(*VarName));
		}
		SetNode->NodePosX = NodePosX;
		SetNode->NodePosY = NodePosY;
		SetNode->NodeGuid = FGuid::NewGuid(); // 生成唯一 GUID
		Graph->AddNode(SetNode, true, true);
		SetNode->AllocateDefaultPins();
		SetNode->ReconstructNode();
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		FNotificationInfo Info(FText::Format(LOCTEXT("SetNodeCreated", "Created Set node for '{0}'"), FText::FromString(VarName)));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
	else
	{
		UK2Node_VariableGet* GetNode = NewObject<UK2Node_VariableGet>(Graph);
		if (bIsLocalVariable && FunctionEntry)
		{
			// 找到局部变量的描述来获取 GUID
			FGuid LocalVarGuid;
			for (const FBPVariableDescription& LocalDesc : FunctionEntry->LocalVariables)
			{
				if (LocalDesc.VarName == FName(*VarName))
				{
					LocalVarGuid = LocalDesc.VarGuid;
					break;
				}
			}
			GetNode->VariableReference.SetLocalMember(FName(*VarName), Graph->GetFName().ToString(), LocalVarGuid);
		}
		else
		{
			GetNode->VariableReference.SetSelfMember(FName(*VarName));
		}
		GetNode->NodePosX = NodePosX;
		GetNode->NodePosY = NodePosY;
		GetNode->NodeGuid = FGuid::NewGuid(); // 生成唯一 GUID
		Graph->AddNode(GetNode, true, true);
		GetNode->AllocateDefaultPins();
		GetNode->ReconstructNode();
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		FNotificationInfo Info(FText::Format(LOCTEXT("GetNodeCreated", "Created Get node for '{0}'"), FText::FromString(VarName)));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void FSmartBlueprintFunctionCreatorModule::RegisterToolbarExtension()
{
	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: RegisterToolbarExtension called"));

	// Register a callback for when UToolMenus is ready
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSmartBlueprintFunctionCreatorModule::RegisterToolbarExtensionInternal));

	// Also try to register immediately if possible
	RegisterToolbarExtensionInternal();
}

void FSmartBlueprintFunctionCreatorModule::OnModulesChanged(FName ModuleName, EModuleChangeReason Reason)
{
	// This function is no longer used but kept for compatibility
}

void FSmartBlueprintFunctionCreatorModule::RegisterToolbarExtensionInternal()
{
	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: RegisterToolbarExtensionInternal called"));

	// Check if toolbar button should be shown
	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	if (!Settings || !Settings->bShowToolbarButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Toolbar button is disabled in settings"));
		return;
	}

	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: UToolMenus not available yet"));
		return;
	}

	// Try multiple possible toolbar menu names for Blueprint Editor
	TArray<FName> PossibleMenuNames = {
		"AssetEditor.BlueprintEditor.ToolBar",
		"BlueprintEditor.BlueprintEditorToolbar.ToolBar",
		"BlueprintEditor.ToolBar",
		"BlueprintEditor.MainToolBar"
	};

	UToolMenu* ToolbarMenu = nullptr;
	FName SuccessfulMenuName;

	for (const FName& MenuName : PossibleMenuNames)
	{
		ToolbarMenu = ToolMenus->ExtendMenu(MenuName);
		if (ToolbarMenu)
		{
			SuccessfulMenuName = MenuName;
			UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Successfully found and extended toolbar menu: %s"), *MenuName.ToString());
			break;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Failed to find toolbar menu: %s"), *MenuName.ToString());
		}
	}

	if (!ToolbarMenu)
	{
		UE_LOG(LogTemp, Error, TEXT("SmartBlueprintCreator: Could not find any suitable toolbar menu"));
		return;
	}

	// Add our button to the toolbar
	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("QuickEventFunctionCreator");
	Section.Label = LOCTEXT("QEFCSection", "Quick Event Function Creator");

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"OpenQEFCSettings",
		FUIAction(FExecuteAction::CreateRaw(this, &FSmartBlueprintFunctionCreatorModule::OpenPluginSettings)),
		LOCTEXT("OpenQEFCLabel", "QEFC Settings"),
		LOCTEXT("OpenQEFCTooltip", "Open Quick Event Function Creator Settings"),
		FSlateIcon(GetQEFCStyleSetName(), "Icons.Settings")
	));

	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Toolbar button added successfully to menu: %s"), *SuccessfulMenuName.ToString());
}

void FSmartBlueprintFunctionCreatorModule::OnToolbarButtonVisibilityChanged(bool bVisible)
{
	UE_LOG(LogTemp, Warning, TEXT("SmartBlueprintCreator: Toolbar button visibility changed to: %s"), bVisible ? TEXT("Visible") : TEXT("Hidden"));

	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		return;
	}

	// Try to find and modify existing toolbar menus
	TArray<FName> PossibleMenuNames = {
		"AssetEditor.BlueprintEditor.ToolBar",
		"BlueprintEditor.BlueprintEditorToolbar.ToolBar",
		"BlueprintEditor.ToolBar",
		"BlueprintEditor.MainToolBar"
	};

	for (const FName& MenuName : PossibleMenuNames)
	{
		UToolMenu* ToolbarMenu = ToolMenus->FindMenu(MenuName);
		if (ToolbarMenu)
		{
			if (bVisible)
			{
				// Add the button if it doesn't exist
				FToolMenuSection* Section = ToolbarMenu->FindSection("QuickEventFunctionCreator");
				if (!Section)
				{
					FToolMenuSection& NewSection = ToolbarMenu->FindOrAddSection("QuickEventFunctionCreator");
					NewSection.Label = LOCTEXT("QEFCSection", "Quick Event Function Creator");

					NewSection.AddEntry(FToolMenuEntry::InitToolBarButton(
						"OpenQEFCSettings",
						FUIAction(FExecuteAction::CreateRaw(this, &FSmartBlueprintFunctionCreatorModule::OpenPluginSettings)),
						LOCTEXT("OpenQEFCLabel", "QEFC Settings"),
						LOCTEXT("OpenQEFCTooltip", "Open Quick Event Function Creator Settings"),
						FSlateIcon(GetQEFCStyleSetName(), "Icons.Settings")
					));
				}
			}
			else
			{
				// Remove the section if it exists
				ToolbarMenu->RemoveSection("QuickEventFunctionCreator");
			}

			// Refresh the menu
			ToolMenus->RefreshAllWidgets();
		}
	}
}

UK2Node* FSmartBlueprintFunctionCreatorModule::CreateMacro(UBlueprint* Blueprint, UEdGraph* OriginalGraph, const FVector2D& Location, const FFunctionSignature& Signature)
{
	UE_LOG(LogTemp, Warning, TEXT("🧪 CreateMacro called for: %s"), *Signature.FunctionName);

	const FName MacroName = FName(*Signature.FunctionName);

	// Create macro graph using UE's official API
	// First create the graph, then add it to the blueprint
	UEdGraph* MacroGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, MacroName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());

	if (MacroGraph)
	{
		// Add the graph as a macro to the blueprint
		FBlueprintEditorUtils::AddMacroGraph(Blueprint, MacroGraph, /*bIsUserCreated=*/ true, /*SignatureFromClass=*/ nullptr);
	}

	if (!MacroGraph)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to create macro graph for: %s"), *Signature.FunctionName);
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("✅ Macro graph created successfully: %s"), *Signature.FunctionName);

	// Find Entry and Result nodes
	UK2Node_Tunnel* EntryNode = nullptr;
	UK2Node_Tunnel* ResultNode = nullptr;

	for (UEdGraphNode* Node : MacroGraph->Nodes)
	{
		if (UK2Node_Tunnel* TunnelNode = Cast<UK2Node_Tunnel>(Node))
		{
			if (!TunnelNode->bCanHaveInputs && TunnelNode->bCanHaveOutputs)
			{
				EntryNode = TunnelNode;
				UE_LOG(LogTemp, Warning, TEXT("🔍 Found Entry node (bCanHaveOutputs=True)"));
			}
			else if (TunnelNode->bCanHaveInputs && !TunnelNode->bCanHaveOutputs)
			{
				ResultNode = TunnelNode;
				UE_LOG(LogTemp, Warning, TEXT("🔍 Found Result node (bCanHaveInputs=True)"));
			}
		}
	}

	if (!EntryNode || !ResultNode)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to find Entry or Result nodes"));
		return nullptr;
	}

	// Add parameters to Entry and Result nodes
	const FScopedTransaction Transaction(LOCTEXT("CreateMacro", "Create Macro"));
	Blueprint->Modify();
	EntryNode->Modify();
	ResultNode->Modify();

	// Process parameters
	for (const FFunctionParameter& Param : Signature.Parameters)
	{
		FEdGraphPinType PinType = GetPinTypeFromString(Param.Type);

		const FName ParamName = FName(*Param.Name);

		if (Param.Direction == EParameterDirection::Input)
		{
			// Input parameters: Entry node outputs data to macro interior
			UE_LOG(LogTemp, Warning, TEXT("🔧 Adding input parameter: %s to Entry node as output"), *Param.Name);
			EntryNode->CreateUserDefinedPin(ParamName, PinType, EGPD_Output, false);
		}
		else if (Param.Direction == EParameterDirection::Output)
		{
			// Output parameters: Result node receives data from macro interior
			UE_LOG(LogTemp, Warning, TEXT("🔧 Adding output parameter: %s to Result node as input"), *Param.Name);
			ResultNode->CreateUserDefinedPin(ParamName, PinType, EGPD_Input, false);
		}
		else if (Param.Direction == EParameterDirection::InputOutput)
		{
			// InputOutput parameters: Entry outputs, Result receives
			UE_LOG(LogTemp, Warning, TEXT("🔧 Adding input/output parameter: %s"), *Param.Name);
			EntryNode->CreateUserDefinedPin(ParamName, PinType, EGPD_Output, false);
			ResultNode->CreateUserDefinedPin(ParamName, PinType, EGPD_Input, false);
		}
	}

	// Reconstruct nodes to apply changes
	EntryNode->ReconstructNode();
	ResultNode->ReconstructNode();

	UE_LOG(LogTemp, Warning, TEXT("✅ Macro creation completed: %s"), *Signature.FunctionName);

	// Create macro instance node in the original graph (like function/event/variable creation)
	UK2Node_MacroInstance* MacroInstanceNode = NewObject<UK2Node_MacroInstance>(OriginalGraph);
	MacroInstanceNode->CreateNewGuid();
	MacroInstanceNode->SetMacroGraph(MacroGraph);
	MacroInstanceNode->NodePosX = Location.X;
	MacroInstanceNode->NodePosY = Location.Y;

	// Add the macro instance node to the original graph
	OriginalGraph->AddNode(MacroInstanceNode, true, false);

	// Allocate default pins for the macro instance
	MacroInstanceNode->AllocateDefaultPins();
	MacroInstanceNode->ReconstructNode();

	UE_LOG(LogTemp, Warning, TEXT("✅ Macro instance node created at location (%.1f, %.1f)"), Location.X, Location.Y);

	// Return the macro instance node (like other creation methods)
	return MacroInstanceNode;
}

void FSmartBlueprintFunctionCreatorModule::OpenPluginSettings()
{
	// Open the project settings and navigate to our plugin settings
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Project", "Plugins", "QuickEventFunctionCreator");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSmartBlueprintFunctionCreatorModule, SmartBlueprintFunctionCreator)
