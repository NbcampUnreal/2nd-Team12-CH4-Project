#include "Item/DFItemAbilityComponent.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "Kismet/GameplayStatics.h"

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

				if (ParentItemData->ActionSound)
				{
					ParentActionsound = ParentItemData->ActionSound;
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

		}
	}

	PlayActionSound();
}

void UDFItemAbilityComponent::PlayActionSound()
{
	if (!ParentActionsound)
	{
		return;
	}

	if (ParentActionsound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ParentActionsound, ParentActor->GetActorLocation());
	}
}