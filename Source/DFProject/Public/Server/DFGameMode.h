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

    // 세션 관련 추가 변수 (옵션)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
    int32 MaxPlayers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
    FString RoomName;


};