#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DFPlayerState.generated.h"

UCLASS()
class DFPROJECT_API ADFPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ADFPlayerState();

    // 플레이어가 속한 팀 번호 (예: 0, 1, 등)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Team")
    int32 TeamID;

    // 플레이어의 점수. 개인전 모드에서 개별 점수를 관리하기 위한 변수.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Score")
    int32 IndividualScore;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void ResetPlayerScore();

    // 플레이어의 마지막 공격자 (예: 데미지를 준 플레이어)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Combat")
    APlayerState* LastDamageDealer;

    // 점수 변경 시 호출되는 함수 (서버에서만 호출됨)
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Score")
    void AddIndividualScore(int32 ScoreDelta);  

    // 사망 시점 (0보다 크면 사망 처리됨)
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float DeathTime = -1.f;

    // 생존 여부 확인 함수  
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsAlive() const { return DeathTime < 0.f; }

    // UI에서 선택한 슬롯 번호
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 SlotIndex = INDEX_NONE;




    // 네트워크 복제를 위한 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
