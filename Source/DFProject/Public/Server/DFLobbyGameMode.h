#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFLobbyGameMode.generated.h"

UCLASS()
class DFPROJECT_API ADFLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADFLobbyGameMode();

	// AGameModeBase 인터페이스
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	// 새로 접속한 플레이어에게 줄 슬롯 인덱스
	int32 NextSlotIndex;
	
};
