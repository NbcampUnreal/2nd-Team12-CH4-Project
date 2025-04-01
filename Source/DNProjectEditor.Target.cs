

using UnrealBuildTool;
using System.Collections.Generic;

public class DNProjectEditorTarget : TargetRules
{
	public DNProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "DNProject" } );
	}
}
