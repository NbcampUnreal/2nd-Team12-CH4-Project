#include "Item/DFCrownActor.h"
#include "Character/DFCharacter.h"
#include "Character/DFPlayerController.h"
#include "Server/DFTimeOutGameMode.h"
#include "Components/SphereComponent.h"

ADFCrownActor::ADFCrownActor()
{
	bReplicates = true;
	GripArea->SetupAttachment(RootComponent);
}

void ADFCrownActor::OnGripAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ADFCharacter::StaticClass()))
	{
		ADFPlayerController* PlayerContoller = Cast<ADFPlayerController>(OtherActor->GetOwner());
		if (ADFTimeOutGameMode* GM = Cast<ADFTimeOutGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->UpdateCrownScores(OtherActor);
		}

		DestroySelf();
	}
}

void ADFCrownActor::DestroySelf()
{
	Destroy();
}