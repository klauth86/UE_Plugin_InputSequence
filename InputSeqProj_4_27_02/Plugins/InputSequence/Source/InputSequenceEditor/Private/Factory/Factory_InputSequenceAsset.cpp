// Copyright 2022. Pentangle Studio. All rights reserved.

#include "Factory/Factory_InputSequenceAsset.h"
#include "AssetTypeCategories.h"
#include "InputSequenceAsset.h"

UFactory_InputSequenceAsset::UFactory_InputSequenceAsset(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UInputSequenceAsset::StaticClass();
}

UObject* UFactory_InputSequenceAsset::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return NewObject<UInputSequenceAsset>(InParent, InClass, InName, Flags);
}

uint32 UFactory_InputSequenceAsset::GetMenuCategories() const { return EAssetTypeCategories::Misc; }