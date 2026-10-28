
#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EVariableNodeSpawnMode : uint8
{
    Get UMETA(DisplayName = "Get"),
    Set UMETA(DisplayName = "Set"),
    None UMETA(DisplayName = "None")
};
