#include "Item/DFItemAbilityComponent.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"

UDFItemAbilityComponent::UDFItemAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ParentItemData = nullptr;
	ParentActor = nullptr;
	ParentMesh = nullptr;
	ParentActionAnim = nullptr;
}

void UDFItemAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		ParentActor = Cast<ADFItemBaseActor>(OwnerActor);

		if (ParentActor)
		{
			if (ParentActor->ItemInstance && ParentActor->ItemInstance->ItemData)
			{
				ParentItemData = ParentActor->ItemInstance->ItemData;

				if (ParentItemData->ActionAnim)
				{
					ParentActionAnim = ParentItemData->ActionAnim;
				}
			}

			if (ParentActor->ItemMesh)
			{
				ParentMesh = ParentActor->ItemMesh;
			}

		}
	}		
}

void UDFItemAbilityComponent::MainAction()
{	
	if (ParentMesh && ParentActionAnim)
	{
		if (UAnimInstance* AnimInstance = ParentMesh->GetAnimInstance())
		{
			float Played = AnimInstance->Montage_Play(ParentActionAnim);
			UE_LOG(LogTemp, Warning, TEXT("🎬 Montage_Play 반환값: %f"), Played);

			if (AnimInstance->Montage_IsPlaying(ParentActionAnim))
			{
				UE_LOG(LogTemp, Warning, TEXT("✅ 몽타주 재생 확인됨"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ 몽타주 재생 안됨"));
			}
		}
	}
}