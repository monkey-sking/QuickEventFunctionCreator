// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "FunctionSignature.generated.h"

// Forward Declaration
class FFunctionParser;

UENUM()
enum class ESignatureType : uint8
{
	Function,
	Event,
	Variable,
	Macro
};

UENUM()
enum class EParameterDirection : uint8
{
	Input,
	Output,
	InputOutput
};

USTRUCT()
struct FFunctionParameter
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Type;

	// Parameter direction for macro support (Input/Output/InputOutput)
	// Default to Input for backward compatibility
	UPROPERTY()
	EParameterDirection Direction = EParameterDirection::Input;
};

UENUM()
enum class EVariableScope : uint8
{
	Global,
	Local
};

UENUM()
enum class EFunctionReplicationCondition : uint8
{
	NotReplicated,
	Server,
	Client,
	NetMulticast
};

/**
 * Represents a parsed function, event, or variable signature.
 * Stores all the necessary information to create a blueprint node.
 */
USTRUCT(BlueprintType)
struct FFunctionSignature
{
	GENERATED_BODY()

	// Common properties
	UPROPERTY()
	ESignatureType SignatureType = ESignatureType::Event;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Type; // For variables: the variable type; For functions: return type

	// Function/Event specific properties
	UPROPERTY()
	TArray<FFunctionParameter> Parameters;

	UPROPERTY()
	bool bIsFunction = true;

	UPROPERTY()
	bool bWasTypeKeywordFound = false;

	UPROPERTY()
	bool bIsConst = false;

	UPROPERTY()
	bool bIsPure = false;

	UPROPERTY()
	EFunctionReplicationCondition ReplicationCondition = EFunctionReplicationCondition::NotReplicated;

	UPROPERTY()
	bool bIsReliable = false;

	// Variable specific properties
	UPROPERTY()
	EVariableScope VariableScope = EVariableScope::Global;

	UPROPERTY()
	bool bIsReadOnly = false;

	UPROPERTY()
	bool bIsStatic = false;

	UPROPERTY()
	FString DefaultValue;

	UPROPERTY()
	FString Category;

	// Helper properties
	UPROPERTY()
	FString FunctionName; // Backward compatibility

	/**
	 * Calculates the appropriate function flags for K2 nodes based on the signature properties.
	 * @return The integer value representing the combined function flags.
	 */
	int32 GetFunctionFlags() const;
}; 