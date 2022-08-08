// Copyright 2022. Pentangle Studio. All rights reserved.

#include "AssetTypeActions/AssetTypeActions_InputSequenceAsset.h"
#include "AssetTypeCategories.h"
#include "InputSequenceAsset.h"
#include "InputSequenceAssetEditor.h"

UClass* FAssetTypeActions_InputSequenceAsset::GetSupportedClass() const { return UInputSequenceAsset::StaticClass(); }

void FAssetTypeActions_InputSequenceAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (TArray<UObject*>::TConstIterator ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UInputSequenceAsset* inputSequenceAsset = Cast<UInputSequenceAsset>(*ObjIt))
		{
			TSharedRef<FInputSequenceAssetEditor> NewEditor(new FInputSequenceAssetEditor());
			NewEditor->InitInputSequenceAssetEditor(Mode, EditWithinLevelEditor, inputSequenceAsset);
		}
	}
}

uint32 FAssetTypeActions_InputSequenceAsset::GetCategories() { return EAssetTypeCategories::Misc; }