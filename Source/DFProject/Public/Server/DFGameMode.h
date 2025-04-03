#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DFGameMode.generated.h"

UCLASS()
class DFPROJECT_API ADFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADFGameMode();

    // 메뉴 UI 위젯 클래스 (블루프린트에서 지정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MenuWidgetClass;

    // 생성된 메뉴 UI 위젯 인스턴스
    UPROPERTY()
    UUserWidget* CurrentMenuWidget;

    // 메뉴 UI를 초기화하고 화면에 표시하는 함수
    void InitializeMenu();
	
};
