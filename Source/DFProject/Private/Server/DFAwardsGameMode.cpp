#include "Server/DFAwardsGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Server/DFBattleGameState.h"
#include "Character/DFPlayerState.h"

void ADFAwardsGameMode::BeginPlay()
{
    Super::BeginPlay();

    ADFBattleGameState* GS = GetGameState<ADFBattleGameState>();
    if (!GS) { UE_LOG(LogTemp, Warning, TEXT("GS NULL"));  return; }

    static const FName RankTags[3] = { TEXT("Rank1"), TEXT("Rank2"), TEXT("Rank3") };

    AActor* Podium[3] = {
        FindRankPoint(RankTags[0]),
        FindRankPoint(RankTags[1]),
        FindRankPoint(RankTags[2])
    };

    for (int32 Rank = 0; Rank < 3; ++Rank)
    {
        UE_LOG(LogTemp, Warning, TEXT("▶ Rank%d : Tag=%s  Podium=%s"),
            Rank + 1, *RankTags[Rank].ToString(),
            Podium[Rank] ? *Podium[Rank]->GetName() : TEXT("NULL"));                 // ★

        if (!Podium[Rank]) continue;
        const FString& Name = GS->PlaceNames[Rank];
        if (Name.IsEmpty() || Name == TEXT("None")) { UE_LOG(LogTemp, Warning, TEXT("  → 이름 None"));  continue; }

        // PlayerState 찾기
        ADFPlayerState* TargetPS = nullptr;
        for (APlayerState* PSBase : GameState->PlayerArray)
        {
            if (PSBase && PSBase->GetPlayerName() == Name)
            {
                TargetPS = Cast<ADFPlayerState>(PSBase); break;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("  → PlayerState %s"), TargetPS ? TEXT("OK") : TEXT("NULL"));     // ★
        if (!TargetPS) continue;

        // Controller 찾기
        AController* Ctrl = nullptr;
        for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
        {
            if (It->Get() && It->Get()->PlayerState == TargetPS)
            {
                Ctrl = It->Get(); break;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("  → Controller %s"), Ctrl ? *Ctrl->GetName() : TEXT("NULL"));    // ★
        if (!Ctrl) continue;

        const FTransform T = Podium[Rank]->GetActorTransform();

        // Spawn
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, T, Params);
        UE_LOG(LogTemp, Warning, TEXT("  → Spawn %s"), NewPawn ? TEXT("OK") : TEXT("FAIL"));           // ★

        if (NewPawn)
        {
            Ctrl->Possess(NewPawn);
            NewPawn->DisableInput(nullptr);
            NewPawn->SetActorEnableCollision(false);
        }
    }
}

AActor* ADFAwardsGameMode::FindRankPoint(const FName& Tag) const
{
    TArray<AActor*> Out;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, Out);
    return Out.IsValidIndex(0) ? Out[0] : nullptr;
}


