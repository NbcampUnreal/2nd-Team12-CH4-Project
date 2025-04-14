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

    // 먼저 OtherActor를 APawn으로 캐스팅 시도
    APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (!OverlappingPawn)
    {
        // 만약 직접 캐스팅이 실패하면, OtherActor의 Outer에서 Pawn을 찾습니다.
        OverlappingPawn = OtherActor->GetTypedOuter<APawn>();

        // Outer에서도 찾지 못하면, 추가로 Owner에서 Pawn을 찾습니다.
        // (래그돌 상태의 경우, 충돌이 SkeletalMeshComponent를 통해 발생하여 Owner를 통해 본 소유 Pawn을 얻을 수 있음)
        if (!OverlappingPawn)
        {
            OverlappingPawn = Cast<APawn>(OtherActor->GetOwner());
        }
    }

    if (OverlappingPawn && IsValid(OverlappingPawn))
    {
        UE_LOG(LogTemp, Log, TEXT("[DeadZoneVolume] Pawn %s entered the dead zone."), *OverlappingPawn->GetName());

        // 서버 권한(Authority)이 있는 경우에만 처리 (서버에서만 로직 실행)
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

