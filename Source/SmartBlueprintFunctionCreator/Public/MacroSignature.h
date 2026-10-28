// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "FunctionSignature.h"
#include "MacroSignature.generated.h"

/**
 * Represents a macro signature for blueprint macro creation
 * Reuses the extended FFunctionParameter structure with Direction support
 */
USTRUCT()
struct SMARTBLUEPRINTFUNCTIONCREATOR_API FMacroSignature
{
	GENERATED_BODY()

	// Macro name
	UPROPERTY()
	FString MacroName;

	// Macro parameters (input, output, or bidirectional)
	UPROPERTY()
	TArray<FFunctionParameter> Parameters;

	// Macro category for organization
	UPROPERTY()
	FString Category = TEXT("Default");

	// Whether the macro can be collapsed
	UPROPERTY()
	bool bIsCollapsible = true;

	// Whether the macro is pure (no execution pins)
	UPROPERTY()
	bool bIsPure = false;

	// Macro description/tooltip
	UPROPERTY()
	FString Description;

	// Default constructor
	FMacroSignature()
	{
		MacroName = TEXT("");
		Category = TEXT("Default");
		bIsCollapsible = true;
		bIsPure = false;
		Description = TEXT("");
	}

	// Constructor with name
	FMacroSignature(const FString& InMacroName)
		: MacroName(InMacroName)
		, Category(TEXT("Default"))
		, bIsCollapsible(true)
		, bIsPure(false)
		, Description(TEXT(""))
	{
	}

	// Helper function to get input parameters
	TArray<FFunctionParameter> GetInputParameters() const
	{
		TArray<FFunctionParameter> InputParams;
		for (const FFunctionParameter& Param : Parameters)
		{
			if (Param.Direction == EParameterDirection::Input || 
				Param.Direction == EParameterDirection::InputOutput)
			{
				InputParams.Add(Param);
			}
		}
		return InputParams;
	}

	// Helper function to get output parameters
	TArray<FFunctionParameter> GetOutputParameters() const
	{
		TArray<FFunctionParameter> OutputParams;
		for (const FFunctionParameter& Param : Parameters)
		{
			if (Param.Direction == EParameterDirection::Output || 
				Param.Direction == EParameterDirection::InputOutput)
			{
				OutputParams.Add(Param);
			}
		}
		return OutputParams;
	}

	// Validation function
	bool IsValid(FString* OutErrorMessage = nullptr) const
	{
		if (MacroName.IsEmpty())
		{
			if (OutErrorMessage)
			{
				*OutErrorMessage = TEXT("Macro name cannot be empty");
			}
			return false;
		}

		// Check for valid macro name (no spaces, special characters)
		if (MacroName.Contains(TEXT(" ")) || MacroName.Contains(TEXT("(")) || MacroName.Contains(TEXT(")")))
		{
			if (OutErrorMessage)
			{
				*OutErrorMessage = TEXT("Macro name cannot contain spaces or parentheses");
			}
			return false;
		}

		// Validate parameters
		for (const FFunctionParameter& Param : Parameters)
		{
			if (Param.Name.IsEmpty() || Param.Type.IsEmpty())
			{
				if (OutErrorMessage)
				{
					*OutErrorMessage = FString::Printf(TEXT("Parameter '%s' has empty name or type"), *Param.Name);
				}
				return false;
			}
		}

		return true;
	}
};
