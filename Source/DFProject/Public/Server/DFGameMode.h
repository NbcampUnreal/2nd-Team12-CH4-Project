#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFGameMode.generated.h"

// 메뉴 모드를 구분하기 위한 Enum (필요에 따라 확장 가능)
UENUM(BlueprintType)
enum class EMenuMode : uint8
{
    None          UMETA(DisplayName = "None"),
    SinglePlayer  UMETA(DisplayName = "SinglePlayer"),
    MultiPlayer   UMETA(DisplayName = "MultiPlayer"),
    MapSelect     UMETA(DisplayName = "MapSelect")
};

UCLASS()
class DFPROJECT_API ADFGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADFGameMode();

    // 게임 시작 시 호출됩니다.
    virtual void BeginPlay() override;

    // 플레이어 접속 후 자동 호출됨
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // 플레이어 종료 시 자동 호출됨
    virtual void Logout(AController* Exiting) override;

    // 캐릭터 선택 완료 후 호출 (UI에서 버튼 클릭 등으로 호출)
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount);

    // 플레이어에게 팀을 할당하는 함수
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void AssignTeam(APlayerController* PlayerController, int32 TeamID);

    // 세션 관련 추가 변수 (옵션)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
    int32 MaxPlayers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
    FString RoomName;


};