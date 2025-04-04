// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DFPlayerController.h"
#include "EnhancedInputSubsystems.h"

ADFPlayerController::ADFPlayerController()
{
}

void ADFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer());
	
	if (ensure(LocalPlayer))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		
		if (ensure(Subsystem))
		{
			if (ensure(IMC)) Subsystem->AddMappingContext(IMC, 0);
		}
	}
}
