#pragma once

#include "CoreMinimal.h"
#include "Server/DFBattleGameMode.h"
#include "DFTimeOutGameMode.generated.h"

UCLASS()
class DFPROJECT_API ADFTimeOutGameMode : public ADFBattleGameMode
{
	GENERATED_BODY()

public:
	ADFTimeOutGameMode();

	// 타임아웃 지속 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	float TimeoutDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	float RespawnDelay;

	// 타임아웃 여부를 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	bool HasTimedOut() const;

	// 플레이어가 장외 상태일 때 호출되는 함수 재정의 (리스폰 방식으로 변경)
	virtual void HandlePlayerOutOfBounds(APawn* Pawn) override;

protected:
	// 게임 시작 시 초기화
	virtual void BeginPlay() override;

	// 매 프레임 호출되는 Tick 함수에서 타임아웃 여부 체크
	virtual void Tick(float DeltaSeconds) override;

	virtual void EndGame() override;

private:
	// 타임아웃 발생 시 호출되는 함수
	void OnTimeout();

	FTimerHandle TimeoutHandle;

	// 헤더(ADFTimeOutGameMode.h) private 영역에 추가:
	void RespawnPlayer(AController* Controller, APawn* PawnToRespawn);


};
