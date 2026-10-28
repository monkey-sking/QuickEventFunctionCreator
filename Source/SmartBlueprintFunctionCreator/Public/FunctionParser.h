// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Forward declaration
struct FFunctionSignature;

/**
 * Unified parser for functions, events, and variables.
 * Supports consistent syntax across all blueprint elements.
 */
class SMARTBLUEPRINTFUNCTIONCREATOR_API FFunctionParser
{
public:
	/**
	 * Parses the input string to populate a unified signature.
	 * Supports functions, events, and variables with consistent syntax.
	 *
	 * Examples:
	 * - Functions: "func float GetHealth()" or "pure func float GetMaxHealth() const"
	 * - Events: "void OnHealthChanged(float NewHealth)" or "event void OnDamaged()"
	 * - Variables: "float Health" or "local int32 Counter" or "readonly float MaxHealth"
	 * - Macros: "macro Calculate(float Base, out float Result)" or "mc Process(inout string Data)"
	 *
	 * @param Input The string to parse.
	 * @param OutSignature The signature to populate.
	 * @param CommandAliases A map of command aliases (e.g., "e" -> "event").
	 * @param DataTypeAliases A map of data type aliases (e.g., "str" -> "string").
	 * @param ParameterDirectionAliases A map of parameter direction aliases (e.g., "out" -> "out").
	 * @return True if parsing was successful and a name was found.
	 */
	static bool Parse(const FString& Input, FFunctionSignature& OutSignature, const TMap<FString, FString>& CommandAliases, const TMap<FString, FString>& DataTypeAliases, const TMap<FString, FString>& ParameterDirectionAliases = TMap<FString, FString>());

private:
	/**
	 * Parses variable declarations with unified syntax.
	 * Supports: "type name", "local type name", "readonly type name", etc.
	 */
	static bool ParseVariable(const FString& Input, FFunctionSignature& OutSignature, const TMap<FString, FString>& DataTypeAliases);

	/**
	 * Parses a single parameter with optional direction prefix.
	 * Supports: "type name", "out type name", "inout type name"
	 * @param ParamString The parameter string to parse
	 * @param OutParameter The parsed parameter
	 * @param DataTypeAliases Map of data type aliases
	 * @param ParameterDirectionAliases Map of parameter direction aliases
	 * @return True if parsing was successful
	 */
	static bool ParseParameterWithDirection(const FString& ParamString, struct FFunctionParameter& OutParameter, const TMap<FString, FString>& DataTypeAliases, const TMap<FString, FString>& ParameterDirectionAliases);
};