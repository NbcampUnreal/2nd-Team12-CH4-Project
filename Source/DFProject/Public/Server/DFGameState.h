#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DFGameState.generated.h"

UCLASS()
class DFPROJECT_API ADFGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADFGameState();

    // 로비 이름
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    FString LobbyName;

    // 로비 최대 플레이어 수
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 LobbyMaxPlayers;

    // 현재 로비에 참가한 플레이어 수
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 CurrentPlayerCount;

    // 게임 시작 전 카운트다운 시간 (초)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    float StartCountdown;

    // 로비가 준비되었는지 여부
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    bool bLobbyReady;

    // 로비에 참가한 플레이어들의 이름 (옵션)
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    TArray<FString> PlayerNames;

    // 복제할 변수들을 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
