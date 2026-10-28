// Copyright runking 2025 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * API definition for the SmartBlueprintFunctionCreator module
 */
#ifndef SMARTBLUEPRINTFUNCTIONCREATOR_API
	#ifdef SMARTBLUEPRINTFUNCTIONCREATOR_EXPORTS
		#define SMARTBLUEPRINTFUNCTIONCREATOR_API DLLEXPORT
	#else
		#define SMARTBLUEPRINTFUNCTIONCREATOR_API DLLIMPORT
	#endif
#endif

/**
 * The public interface to this module
 */
class ISmartBlueprintFunctionCreatorModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline ISmartBlueprintFunctionCreatorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ISmartBlueprintFunctionCreatorModule>("SmartBlueprintFunctionCreator");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SmartBlueprintFunctionCreator");
	}
};
