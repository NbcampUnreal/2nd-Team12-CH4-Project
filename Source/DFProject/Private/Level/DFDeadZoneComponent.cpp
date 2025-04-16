#include "Level/DFDeadZoneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFBattleGameMode.h"
#include "GameFramework/Pawn.h"
#include "NavModifierComponent.h"
#include "NavigationSystem.h"
#include "NavAreas/NavArea_Null.h"

UDFDeadZoneComponent::UDFDeadZoneComponent()
{
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetGenerateOverlapEvents(true);

    NavModifierComp = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavBlocker"));
    NavModifierComp->SetAreaClass(UNavArea_Null::StaticClass());
    
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