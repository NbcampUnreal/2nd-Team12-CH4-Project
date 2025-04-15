#include "Level/DFDeadZone.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFBattleGameMode.h"
#include "Components/PrimitiveComponent.h" 

ADFDeadZone::ADFDeadZone()
{

}

void ADFDeadZone::BeginPlay()
{
	Super::BeginPlay();

    // DeadZone의 RootComponent가 유효한지 확인
    if (RootComponent)
    {
        if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(RootComponent))
        {
            PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            PrimComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
            PrimComp->SetGenerateOverlapEvents(true);
            UE_LOG(LogTemp, Log, TEXT("DeadZone: Collision settings applied."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadZone: RootComponent is null!"));
    }

	// 트리거 오버랩 시작 이벤트 바인딩
	OnActorBeginOverlap.AddDynamic(this, &ADFDeadZone::OnOverlapBegin);
}

void ADFDeadZone::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (!OverlappingPawn)
    {
        OverlappingPawn = OtherActor->GetTypedOuter<APawn>();
        if (!OverlappingPawn)
        {
            OverlappingPawn = Cast<APawn>(OtherActor->GetOwner());
        }
    }

    if (OverlappingPawn && IsValid(OverlappingPawn))
    {
        // 만약 Pawn이 "NoDeadZone" 태그를 갖고 있다면, 데드존 처리 건너뜁니다.
        if (OverlappingPawn->Tags.Contains(FName("NoDeadZone")))
        {
            UE_LOG(LogTemp, Log, TEXT("[DeadZoneVolume] Pawn %s is immune (just respawned)."), *OverlappingPawn->GetName());
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("[DeadZoneVolume] Pawn %s entered the dead zone."), *OverlappingPawn->GetName());

        if (OverlappingPawn->HasAuthority())
        {
            ADFBattleGameMode* GM = Cast<ADFBattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM)
            {
                GM->HandlePlayerOutOfBounds(OverlappingPawn);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[DeadZoneVolume] GameMode not found."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[DeadZoneVolume] Overlap on client side ignored."));
        }
    }
}

