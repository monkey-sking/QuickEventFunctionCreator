// Copyright runking 2025 All Rights Reserved.

using UnrealBuildTool;

public class SmartBlueprintFunctionCreator : ModuleRules
{
	public SmartBlueprintFunctionCreator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// Add public include paths here
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// Add private include paths here
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects" // Moved from Private for Ticker header visibility
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd", 
				"ToolMenus", 
				"Kismet",
				"BlueprintGraph",
				"GraphEditor",
				"PropertyEditor",
				"DeveloperSettings",
				"DetailCustomizations"
				// ... add other private dependencies that you statically link with here ...	
			}
			);
		
		if (Target.Version.MajorVersion >= 5)
		{
			PrivateDependencyModuleNames.Add("EditorFramework");
		}
		else
		{
			PrivateDependencyModuleNames.Add("EditorStyle");
		}
	}
} 