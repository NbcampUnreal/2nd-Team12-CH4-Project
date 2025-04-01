

using UnrealBuildTool;
using System.Collections.Generic;

public class DNProjectTarget : TargetRules
{
	public DNProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "DNProject" } );
	}
}
