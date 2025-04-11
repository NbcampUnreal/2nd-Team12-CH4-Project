#include "Level/DFDeadZone.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFBattleGameMode.h"

ADFDeadZone::ADFDeadZone()
{

}

void ADFDeadZone::BeginPlay()
{
	Super::BeginPlay();
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
    if (OverlappingPawn && IsValid(OverlappingPawn))
    {
        UE_LOG(LogTemp, Log, TEXT("[DeadZoneVolume] Pawn %s entered the dead zone."), *OverlappingPawn->GetName());

        // 서버 권한(Authority)이 있는 경우에만 GameMode를 사용해서 처리
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
    }
}

