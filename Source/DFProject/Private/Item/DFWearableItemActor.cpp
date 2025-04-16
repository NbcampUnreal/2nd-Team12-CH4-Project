#include "Item/DFWearableItemActor.h"
#include "Item/DFWearableItem.h"

ADFWearableItemActor::ADFWearableItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(Scene);
}

void ADFWearableItemActor::SetActor(UDFWearableItem* ItemDataToShow)
{
	if (!ItemDataToShow || !ItemDataToShow->ItemMesh)
	{
		return;
	}

	ItemData = ItemDataToShow;
	FName SocketName = ItemData->EquipSocketName;

	ItemMesh->SetSkeletalMesh(ItemData->ItemMesh);
	ItemMesh->AttachToComponent(PreviewMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	ItemMesh->SetCollisionProfileName(TEXT("Item"));

	if (ItemData->ItemType == EItemType::Fabric)
	{
		ItemMesh->SetAllBodiesBelowSimulatePhysics(TEXT("FabricBone"), true, true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}


