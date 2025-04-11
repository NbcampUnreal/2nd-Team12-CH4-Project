#include "Level/DFDeadZoneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFBattleGameMode.h"
#include "GameFramework/Pawn.h"

UDFDeadZoneComponent::UDFDeadZoneComponent()
{
    // 이 컴포넌트는 BlueprintSpawnableComponent이므로 에디터에서 추가할 수 있습니다.
// 기본적으로 UBoxComponent를 사용하므로, 크기나 충돌 프리셋은 에디터에서 조정 가능하지만,
// 여기서 기본 충돌 설정을 강제할 수 있습니다.

// Overlap 이벤트를 활성화하도록 설정
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetGenerateOverlapEvents(true);
}

void UDFDeadZoneComponent::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay에서 OnComponentBeginOverlap 이벤트를 바인딩합니다.
    OnComponentBeginOverlap.AddDynamic(this, &UDFDeadZoneComponent::OnOverlapBegin);

}

void UDFDeadZoneComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == GetOwner())
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("UDFDeadZoneComponent: Overlap detected with Actor: %s"), *OtherActor->GetName());

    // OtherActor가 Pawn인지 확인합니다.
    APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (OverlappingPawn && IsValid(OverlappingPawn))
    {
        UE_LOG(LogTemp, Log, TEXT("UDFDeadZoneComponent: Pawn %s entered the dead zone."), *OverlappingPawn->GetName());

        // 서버 권한(Authority)이 있는 경우에만 처리 (즉, 서버에서만 GameMode 로직을 호출)
        if (OverlappingPawn->HasAuthority())
        {
            ADFBattleGameMode* GM = Cast<ADFBattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM)
            {
                GM->HandlePlayerOutOfBounds(OverlappingPawn);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("UDFDeadZoneComponent: GameMode not found."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("UDFDeadZoneComponent: Overlap event on client side ignored."));
        }
    }
}