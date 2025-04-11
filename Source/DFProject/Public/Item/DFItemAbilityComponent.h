#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFItemAbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DFPROJECT_API UDFItemAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDFItemAbilityComponent();

protected:
	virtual void BeginPlay() override;


	virtual void MainAction();

};
