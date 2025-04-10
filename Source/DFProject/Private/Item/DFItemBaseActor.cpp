#include "Item/DFItemBaseActor.h"
#include "Item/DFItemInstance.h"
#include "Item/DFBattleItem.h"
#include "Item/DFItemAbilityComponent.h"
#include "Components/SphereComponent.h"

ADFItemBaseActor::ADFItemBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);	

	GripArea = CreateDefaultSubobject<USphereComponent>(TEXT("GripArea"));
	GripArea->InitSphereRadius(50.0f);
	GripArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GripArea->SetCollisionObjectType(ECC_WorldDynamic);
	GripArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	GripArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GripArea->OnComponentBeginOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaBeginOverlap);
	GripArea->OnComponentEndOverlap.AddDynamic(this, &ADFItemBaseActor::OnGripAreaEndOverlap);

	ItemInstance = nullptr;

	bCanBeGrabbed = false;
}

void ADFItemBaseActor::BeginPlay()
{
	Super::BeginPlay();	
}

void ADFItemBaseActor::SetupItem(UDFItemInstance* NewInstance)
{
	if (NewInstance && NewInstance->ItemData->ItemMesh)
	{		
	ItemInstance = NewInstance;
	ItemMesh->SetSkeletalMesh(NewInstance->ItemData->ItemMesh);

	GripArea->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HandGripSocket"));

	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetAllBodiesBelowSimulatePhysics("Root", false, false);
	ItemMesh->SetAllBodiesBelowPhysicsBlendWeight("Root", 0.0f, true);

	AttachAbilities();
	}
	
}

void ADFItemBaseActor::OnGripAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OterActor,
	UPrimitiveComponent* OtherCompm,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	bCanBeGrabbed = true;
}

void ADFItemBaseActor::OnGripAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bCanBeGrabbed = false;
}

void ADFItemBaseActor::AttachAbilities()
{
	if (ItemInstance)
	{
		for (TSubclassOf<UDFItemAbilityComponent> Ability : ItemInstance->ItemData->Abilities)
		{
			if (!Ability)
			{
				continue;
			}

			UDFItemAbilityComponent* NewAbility = NewObject<UDFItemAbilityComponent>(this, Ability);

			if (NewAbility)
			{
				NewAbility->RegisterComponent();
				AddInstanceComponent(NewAbility);
				NewAbility->Activate(true);
			}
		}
	}
}

FName ADFItemBaseActor::GetCurrentnItemId() const
{
	return ItemInstance->ItemData->GetItemId();
}