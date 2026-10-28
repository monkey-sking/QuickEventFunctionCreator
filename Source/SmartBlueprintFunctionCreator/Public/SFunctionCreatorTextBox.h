// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Types/SlateEnums.h"

// Forward declare SWindow to avoid including the full header
class SWindow;
class SGraphEditor;

DECLARE_DELEGATE_FourParams(FOnCreatorTextCommitted, const FText&, ETextCommit::Type, TSharedPtr<SGraphEditor>, TWeakPtr<SWindow>);

/**
 * A Slate widget for creating functions
 */
class SFunctionCreatorTextBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFunctionCreatorTextBox) {}
		SLATE_EVENT(FOnCreatorTextCommitted, OnTextCommitted)
		SLATE_ARGUMENT(TWeakPtr<SGraphEditor>, GraphEditor)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	// Make this widget keyboard focusable
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** Deprecated. The widget now handles its own focus. */
	void SetFocusToEditableText();

protected:
	// Handles key down events
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void OnTextCommitted_Internal(const FText& InText, ETextCommit::Type InCommitType);

	/** Handler for when text is changed. */
	void OnTextChanged(const FText& InText);

	/** Gets the hint text to display below the input box. */
	FText GetHintText() const;

	/** Determines the visibility of the hint text. */
	EVisibility GetHintTextVisibility() const;

	/** The editable text box widget */
	TSharedPtr<SEditableTextBox> EditableTextBox;

	/** Delegate to call when text is committed */
	FOnCreatorTextCommitted OnTextCommitted;

	/** Weak pointer to the parent graph editor */
	TWeakPtr<SGraphEditor> GraphEditor;
};