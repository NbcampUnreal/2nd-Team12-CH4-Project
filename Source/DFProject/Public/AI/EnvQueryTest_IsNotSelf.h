#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_IsNotSelf.generated.h"

UCLASS()
class DFPROJECT_API UEnvQueryTest_IsNotSelf : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_IsNotSelf();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
