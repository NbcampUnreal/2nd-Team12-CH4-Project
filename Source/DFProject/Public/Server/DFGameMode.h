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

// 맵 선택을 위한 Enum
UENUM(BlueprintType)
enum class EMapSelection : uint8
{
    MapOne   UMETA(DisplayName = "Map One"),
    MapTwo   UMETA(DisplayName = "Map Two"),
    MapThree UMETA(DisplayName = "Map Three")
};

UCLASS()
class DFPROJECT_API ADFGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADFGameMode();

    virtual void BeginPlay() override;

    // 캐릭터 선택 완료 후 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void OnCharacterSelectionComplete(int32 SelectedCharacterIndex, int32 SelectedCPUCount);

    // 맵 선택 완료 후 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "GameMode")
    void OnMapSelectionComplete(EMapSelection SelectedMap);

    // 팀 할당 함수 (특정 플레이어에게 팀 번호를 할당)
    UFUNCTION(BlueprintCallable, Category = "Team")
    void AssignTeam(APlayerController* PlayerController, int32 TeamID);
};