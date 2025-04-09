// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DFItemInstance.generated.h"

class UDFBattleItem;

UCLASS()
class DFPROJECT_API UDFItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UDFBattleItem* ItemData;

	void Initialize(UDFBattleItem* SpawnItemData);

};
