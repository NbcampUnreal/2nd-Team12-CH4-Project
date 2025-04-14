#include "Item/DFWearableItemActor.h"
#include "Item/DFWearableItem.h"

ADFWearableItemActor::ADFWearableItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(Scene);

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(PreviewMesh);

	
}

void ADFWearableItemActor::SetActor(UDFWearableItem* ItemDataToShow)
{
	if (!ItemDataToShow || !ItemDataToShow->ItemMesh)
	{
		return;
	}

	ItemData = ItemDataToShow;
	FName SocketName = ItemData->EquipSocketName;

	ItemMesh ->SetSkeletalMesh(ItemData->ItemMesh);
	ItemMesh->AttachToComponent(PreviewMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

}


