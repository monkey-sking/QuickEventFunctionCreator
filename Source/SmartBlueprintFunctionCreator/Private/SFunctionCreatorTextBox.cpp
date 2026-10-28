// Copyright runking 2025 All Rights Reserved.

#include "SFunctionCreatorTextBox.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Application/SlateApplication.h"
#if ENGINE_MAJOR_VERSION == 4
#include "EditorStyleSet.h"
#endif
#include "GraphEditor.h"
#include "SmartBlueprintCreatorSettings.h"

#define LOCTEXT_NAMESPACE "SFunctionCreatorTextBox"

void SFunctionCreatorTextBox::Construct(const FArguments& InArgs)
{
	OnTextCommitted = InArgs._OnTextCommitted;
	GraphEditor = InArgs._GraphEditor;
	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(400)
		[
			SNew(SVerticalBox)

			// Input Text Box
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(EditableTextBox, SEditableTextBox)
				.HintText(LOCTEXT("InputHint", "Type unified syntax: var i Health | fn Calculate | e OnDeath | macro Process(exec In, float Input, inout bool Flag, out exec Success, out float Result) | g Health | st Health"))
				.OnTextCommitted(this, &SFunctionCreatorTextBox::OnTextCommitted_Internal)
				.OnTextChanged(this, &SFunctionCreatorTextBox::OnTextChanged)
			]

			// Usage Tips
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(STextBlock)
				.Text(this, &SFunctionCreatorTextBox::GetHintText)
				.Visibility(this, &SFunctionCreatorTextBox::GetHintTextVisibility)
#if ENGINE_MAJOR_VERSION >= 5
				.TextStyle(FAppStyle::Get(), "HintText")
#else
				.TextStyle(FEditorStyle::Get(), "HintText")
#endif
				.AutoWrapText(true)
			]
		]
	];
	
	// Defer setting focus to the next tick to ensure the widget is ready.
	TWeakPtr<SEditableTextBox> WeakEditableTextBox = EditableTextBox;
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateLambda(
		[WeakEditableTextBox](double InCurrentTime, float InDeltaTime)
		{
			if (WeakEditableTextBox.IsValid())
			{
				FSlateApplication::Get().SetKeyboardFocus(WeakEditableTextBox.Pin(), EFocusCause::SetDirectly);
			}
			return EActiveTimerReturnType::Stop;
		}));
}

void SFunctionCreatorTextBox::OnTextChanged(const FText& InText)
{
	// Can be used for syntax highlighting in the future
}

FText SFunctionCreatorTextBox::GetHintText() const
{
	const FString Hint = TEXT(
		"🎯 v2.0 Unified Syntax Examples:\n"
		"Variables:   var i Health | lv str Name | var f MaxSpeed\n"
		"Functions:   fn Calculate | p fn GetScore | k fn GetMax\n"
		"Events:      e OnDeath | s OnUpdate | rel m OnSync\n"
		"Macros:      macro Calculate(float Input, out float Result) | mc ProcessData(exec In, string Input, inout bool Flag, out exec Success)\n"
		"Var Nodes:   g Health (Get) | st Health (Set)\n"
		"---\n"
		"⚡ Smart Aliases: e→event, fn→func, lv→local, g→get, st→set, mc→macro\n"
		"🏷️ Type Aliases: i→integer, str→string, b→boolean, v→vector, r→rotator, t→transform, exec→execute\n"
		"📋 Macro Params: in Input params (default), out Output params, inout InputOutput params, exec Execution pins\n"
		"🔀 Execution Flow: exec In (input execution), out exec Success (output execution), multiple exec outputs supported\n"
		"💡 Context Smart: 'var' in functions = local, in events = global | Macros auto-place call nodes"
	);
	return FText::FromString(Hint);
}

EVisibility SFunctionCreatorTextBox::GetHintTextVisibility() const
{
	const USmartBlueprintCreatorSettings* Settings = GetDefault<USmartBlueprintCreatorSettings>();
	return (Settings && Settings->bShowUsageTips) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SFunctionCreatorTextBox::OnTextCommitted_Internal(const FText& InText, ETextCommit::Type InCommitType)
{
	OnTextCommitted.ExecuteIfBound(InText, InCommitType, GraphEditor.Pin(), TWeakPtr<SWindow>());
}

FReply SFunctionCreatorTextBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		FSlateApplication::Get().DismissAllMenus();
		return FReply::Handled();
	}
	
	// Pass all other key events to the parent class to handle standard behavior like copy/paste
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

#undef LOCTEXT_NAMESPACE 