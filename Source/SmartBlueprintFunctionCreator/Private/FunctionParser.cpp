// Copyright runking 2025 All Rights Reserved.

#include "FunctionParser.h"
#include "FunctionSignature.h"
#include "SmartBlueprintCreatorSettings.h"
#include "Containers/UnrealString.h"
#include "Misc/Char.h"
#include "Internationalization/Regex.h"

namespace
{
	// Helper to find and remove a word from a TArray<FString>, returns true if found
	bool FindAndRemoveWord(TArray<FString>& Words, const FString& WordToRemove, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive)
	{
		int32 FoundIndex = Words.IndexOfByPredicate([&](const FString& Word)
		{
			return Word.Equals(WordToRemove, SearchCase);
		});

		if (FoundIndex != INDEX_NONE)
		{
			Words.RemoveAt(FoundIndex);
			return true;
		}
		return false;
	}
}

int32 FFunctionSignature::GetFunctionFlags() const
{
	int32 Flags = FUNC_Public; // Start with public access.

	if (bIsFunction)
	{
		// This is a standard blueprint-callable function.
		Flags |= FUNC_BlueprintCallable;

		if (bIsPure) Flags |= FUNC_BlueprintPure;
		if (bIsConst) Flags |= FUNC_Const;
	}
	else // This is an Event.
	{
		// Events are callable and are implemented in the Event Graph.
		Flags |= FUNC_BlueprintCallable;
		Flags |= FUNC_BlueprintEvent;

		// Handle network replication for events.
		if (ReplicationCondition != EFunctionReplicationCondition::NotReplicated)
		{
			Flags |= FUNC_Net;
			switch (ReplicationCondition)
			{
			case EFunctionReplicationCondition::Server:
				Flags |= FUNC_NetServer;
				break;
			case EFunctionReplicationCondition::Client:
				Flags |= FUNC_NetClient;
				break;
			case EFunctionReplicationCondition::NetMulticast:
				Flags |= FUNC_NetMulticast;
				break;
			default:
				break;
			}

			if (bIsReliable)
			{
				Flags |= FUNC_NetReliable;
			}
		}
	}

	return Flags;
}


bool FFunctionParser::Parse(const FString& Input, FFunctionSignature& OutSignature, const TMap<FString, FString>& CommandAliases, const TMap<FString, FString>& DataTypeAliases, const TMap<FString, FString>& ParameterDirectionAliases)
{
	OutSignature = FFunctionSignature();
	FString WorkingString = Input.TrimStartAndEnd();
	if (WorkingString.IsEmpty()) return false;

	// Step 0: Alias Normalization (for command keywords)
	TArray<FString> Words;
	WorkingString.ParseIntoArray(Words, TEXT(" "), true);

	// Early detection: Check if this looks like a variable declaration
	// Only treat as variable if it has non-scope variable keywords (readonly, static)
	// Scope keywords (local, var) will be handled in the main keyword detection
	bool bLooksLikeVariable = false;
	if (Words.Num() >= 2)
	{
		bool bHasNonScopeVariableKeywords = false;

		for (const FString& Word : Words)
		{
			FString LowerWord = Word.ToLower();
			// Only check for non-scope variable keywords here
			// local and var will be handled in main keyword detection
			if (LowerWord == TEXT("readonly") || LowerWord == TEXT("static"))
			{
				bHasNonScopeVariableKeywords = true;
				break;
			}
		}

		// Only treat as variable if it has non-scope variable keywords
		if (bHasNonScopeVariableKeywords)
		{
			bLooksLikeVariable = true;
		}
	}

	// If this looks like a variable, parse it as such
	if (bLooksLikeVariable)
	{
		return ParseVariable(WorkingString, OutSignature, DataTypeAliases);
	}

	for (FString& Word : Words)
	{
		const FString* FoundAlias = CommandAliases.Find(Word.ToLower());
		if (FoundAlias)
		{
			UE_LOG(LogTemp, Warning, TEXT("🧪 Alias Debug: '%s' -> '%s'"), *Word, **FoundAlias);
			Word = *FoundAlias;
		}
	}

	// Step 1: Determine function type by finding keywords anywhere in the input.
	// This is more robust than assuming the keyword is the first word.
	
	// Determine default creation type. In an isolated parser, we can't access settings.
	// We'll assume 'event' is the default unless 'func' is specified.
	// The calling context can override this if needed.
	OutSignature.bIsFunction = false; // Default to event

	// Check for variable keywords first
	if (FindAndRemoveWord(Words, TEXT("var"), ESearchCase::IgnoreCase))
	{
		OutSignature.SignatureType = ESignatureType::Variable;
		OutSignature.VariableScope = EVariableScope::Global; // var = global variable
		OutSignature.bWasTypeKeywordFound = true;
		return ParseVariable(FString::Join(Words, TEXT(" ")), OutSignature, DataTypeAliases);
	}
	else if (FindAndRemoveWord(Words, TEXT("local"), ESearchCase::IgnoreCase))
	{
		OutSignature.SignatureType = ESignatureType::Variable;
		OutSignature.VariableScope = EVariableScope::Local; // local = local variable
		OutSignature.bWasTypeKeywordFound = true;
		return ParseVariable(FString::Join(Words, TEXT(" ")), OutSignature, DataTypeAliases);
	}

	// Check for macro keywords
	UE_LOG(LogTemp, Warning, TEXT("🧪 Checking for macro keyword in words: %s"), *FString::Join(Words, TEXT(" ")));
	if (FindAndRemoveWord(Words, TEXT("macro"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("🧪 MACRO KEYWORD FOUND! Setting SignatureType to Macro"));
		OutSignature.SignatureType = ESignatureType::Macro;
		OutSignature.bWasTypeKeywordFound = true;
		// Continue parsing as macro (similar to function but with different signature type)
	}

	// Then check for function/event keywords
	if (FindAndRemoveWord(Words, TEXT("func"), ESearchCase::IgnoreCase))
	{
		OutSignature.bIsFunction = true;
		OutSignature.bWasTypeKeywordFound = true;
	}
	else if (FindAndRemoveWord(Words, TEXT("event"), ESearchCase::IgnoreCase))
	{
		OutSignature.bIsFunction = false;
		OutSignature.bWasTypeKeywordFound = true;
	}

	// Step 2: Attribute Extraction (now that we definitively know the type)
	if (OutSignature.SignatureType == ESignatureType::Macro)
	{
		// Macros don't have function/event specific attributes like pure, const, etc.
		// But they still need parameter parsing, so continue to Step 3
	}
	else if (OutSignature.bIsFunction)
	{
		if (FindAndRemoveWord(Words, TEXT("pure"), ESearchCase::IgnoreCase)) OutSignature.bIsPure = true;
		if (FindAndRemoveWord(Words, TEXT("const"), ESearchCase::IgnoreCase)) OutSignature.bIsConst = true;
	}
	else // It's an event
	{
		// These are mutually exclusive, so use an if-else if chain.
		if (FindAndRemoveWord(Words, TEXT("server"), ESearchCase::IgnoreCase))
		{
			OutSignature.ReplicationCondition = EFunctionReplicationCondition::Server;
		}
		else if (FindAndRemoveWord(Words, TEXT("client"), ESearchCase::IgnoreCase))
		{
			OutSignature.ReplicationCondition = EFunctionReplicationCondition::Client;
		}
		else if (FindAndRemoveWord(Words, TEXT("multicast"), ESearchCase::IgnoreCase))
		{
			OutSignature.ReplicationCondition = EFunctionReplicationCondition::NetMulticast;
		}

		if (FindAndRemoveWord(Words, TEXT("reliable"), ESearchCase::IgnoreCase)) OutSignature.bIsReliable = true;
	}
	
	WorkingString = FString::Join(Words, TEXT(" "));

	// Step 3: Signature Parsing (Simplified: No Return Types)
	FString ParamsString;
	if (!WorkingString.Split(TEXT("("), &OutSignature.FunctionName, &ParamsString))
	{
		// No parentheses found, so the whole string is the function name
		OutSignature.FunctionName = WorkingString.TrimStartAndEnd();
	}
	else
	{
		// Parentheses found, extract function name and parameters
		OutSignature.FunctionName = OutSignature.FunctionName.TrimStartAndEnd();
		
		// We only care about the content within the first pair of parentheses
		if (ParamsString.Contains(TEXT(")")))
		{
			ParamsString = ParamsString.Left(ParamsString.Find(TEXT(")"), ESearchCase::CaseSensitive, ESearchDir::FromEnd));
		}
		
		TArray<FString> ParamArray;
		ParamsString.ParseIntoArray(ParamArray, TEXT(","), true);

		for (const FString& ParamPairStr : ParamArray)
		{
			FFunctionParameter ParsedParam;
			if (ParseParameterWithDirection(ParamPairStr.TrimStartAndEnd(), ParsedParam, DataTypeAliases, ParameterDirectionAliases))
			{
				OutSignature.Parameters.Add(ParsedParam);
			}
		}
	}
	
	// A function cannot be an RPC. If it's determined to be a function, wipe network settings.
	if (OutSignature.bIsFunction)
	{
		OutSignature.ReplicationCondition = EFunctionReplicationCondition::NotReplicated;
		OutSignature.bIsReliable = false;
	}

	bool bParseSuccess = !OutSignature.FunctionName.IsEmpty();

	// Set the Name field for functions, events, and macros
	if (bParseSuccess && (OutSignature.SignatureType == ESignatureType::Function ||
	                      OutSignature.SignatureType == ESignatureType::Event ||
	                      OutSignature.SignatureType == ESignatureType::Macro))
	{
		OutSignature.Name = OutSignature.FunctionName;
	}

	// Fallback: If function/event parsing failed and input looks like "type name", try parsing as variable
	if (!bParseSuccess)
	{
		TArray<FString> FallbackWords;
		Input.TrimStartAndEnd().ParseIntoArray(FallbackWords, TEXT(" "), true);

		// If exactly 2 words and no parentheses, might be a simple variable declaration
		if (FallbackWords.Num() == 2 && !Input.Contains(TEXT("(")))
		{
			return ParseVariable(Input, OutSignature, DataTypeAliases);
		}
	}

	return bParseSuccess;
}

bool FFunctionParser::ParseVariable(const FString& Input, FFunctionSignature& OutSignature, const TMap<FString, FString>& DataTypeAliases)
{
	OutSignature = FFunctionSignature();
	OutSignature.SignatureType = ESignatureType::Variable;

	FString WorkingString = Input.TrimStartAndEnd();
	if (WorkingString.IsEmpty()) return false;

	TArray<FString> Words;
	WorkingString.ParseIntoArray(Words, TEXT(" "), true);

	if (Words.Num() < 2) return false; // Need at least type and name

	// Parse modifiers and determine variable properties
	int32 TypeIndex = 0;

	// Scope is already set by the main parser if 'local' or 'var' keyword was used
	// No need to check for scope modifiers here

	// Check for other modifiers
	while (TypeIndex < Words.Num() - 2) // Leave at least type and name
	{
		FString LowerWord = Words[TypeIndex].ToLower();
		if (LowerWord == TEXT("readonly"))
		{
			OutSignature.bIsReadOnly = true;
			TypeIndex++;
		}
		else if (LowerWord == TEXT("static"))
		{
			OutSignature.bIsStatic = true;
			TypeIndex++;
		}
		else
		{
			break; // Not a modifier, must be the type
		}
	}

	// Must have at least type and name remaining
	if (TypeIndex >= Words.Num() - 1) return false;

	// Get type and name
	FString VarType = Words[TypeIndex];
	FString VarName = Words[TypeIndex + 1];

	// Apply type aliases
	if (DataTypeAliases.Contains(VarType.ToLower()))
	{
		VarType = DataTypeAliases[VarType.ToLower()];
	}

	// Set signature properties
	OutSignature.Type = VarType;
	OutSignature.Name = VarName;
	OutSignature.FunctionName = VarName; // For backward compatibility

	// Parse default value if provided (format: "type name = value")
	if (TypeIndex + 2 < Words.Num() && Words[TypeIndex + 2] == TEXT("="))
	{
		TArray<FString> ValueParts;
		for (int32 i = TypeIndex + 3; i < Words.Num(); i++)
		{
			ValueParts.Add(Words[i]);
		}
		OutSignature.DefaultValue = FString::Join(ValueParts, TEXT(" "));
	}

	return !OutSignature.Name.IsEmpty();
}

bool FFunctionParser::ParseParameterWithDirection(const FString& ParamString, FFunctionParameter& OutParameter, const TMap<FString, FString>& DataTypeAliases, const TMap<FString, FString>& ParameterDirectionAliases)
{
	OutParameter = FFunctionParameter();

	FString WorkingString = ParamString.TrimStartAndEnd();
	if (WorkingString.IsEmpty()) return false;

	TArray<FString> Words;
	WorkingString.ParseIntoArray(Words, TEXT(" "), true);

	if (Words.Num() < 2) return false; // Need at least type and name

	int32 TypeIndex = 0;

	// Check for parameter direction prefix
	if (Words.Num() >= 3) // Need direction + type + name
	{
		FString FirstWord = Words[0].ToLower();

		// Check if first word is a direction alias
		const FString* FoundDirectionAlias = ParameterDirectionAliases.Find(FirstWord);
		if (FoundDirectionAlias)
		{
			FString Direction = *FoundDirectionAlias;
			if (Direction == TEXT("out"))
			{
				OutParameter.Direction = EParameterDirection::Output;
			}
			else if (Direction == TEXT("inout"))
			{
				OutParameter.Direction = EParameterDirection::InputOutput;
			}
			TypeIndex = 1; // Skip the direction word
		}
		// Also check for direct direction keywords
		else if (FirstWord == TEXT("out"))
		{
			OutParameter.Direction = EParameterDirection::Output;
			TypeIndex = 1;
		}
		else if (FirstWord == TEXT("inout") || FirstWord == TEXT("ref") || FirstWord == TEXT("io"))
		{
			OutParameter.Direction = EParameterDirection::InputOutput;
			TypeIndex = 1;
		}
	}

	// Default to Input if no direction specified
	if (OutParameter.Direction == EParameterDirection::Input && TypeIndex == 0)
	{
		OutParameter.Direction = EParameterDirection::Input;
	}

	// Must have at least type and name remaining
	if (TypeIndex >= Words.Num() - 1) return false;

	// Get type and name
	FString Type = Words[TypeIndex];
	FString Name = Words[TypeIndex + 1];

	// Apply type aliases
	const FString* FoundTypeAlias = DataTypeAliases.Find(Type.ToLower());
	if (FoundTypeAlias)
	{
		Type = *FoundTypeAlias;
	}

	// Set parameter properties
	OutParameter.Type = Type.TrimStartAndEnd();
	OutParameter.Name = Name.TrimStartAndEnd();

	return !OutParameter.Type.IsEmpty() && !OutParameter.Name.IsEmpty();
}