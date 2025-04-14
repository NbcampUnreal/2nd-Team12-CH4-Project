
#pragma once

#include "CoreMinimal.h"
#include "CharacterStateBase.generated.h"
/**
 * 
 */
class ADFCharacter;

UENUM(BlueprintType)
enum class ECharacterStateType : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Stunned UMETA(DisplayName = "Stunned"),
	Grabbed UMETA(DisplayName = "Grabbed"),
	Recover UMETA(DisplayName = "Recover"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS(Abstract, Blueprintable)
class DFPROJECT_API UCharacterStateBase : public UObject
{
	GENERATED_BODY()
public:
	virtual void Enter(ADFCharacter* Character) {}
	virtual void Exit(ADFCharacter* Character) {}
	virtual void Tick(ADFCharacter* Character, float DeltaTime) {}
	virtual ECharacterStateType GetStateType() const {return ECharacterStateType::Idle;}
	
};
