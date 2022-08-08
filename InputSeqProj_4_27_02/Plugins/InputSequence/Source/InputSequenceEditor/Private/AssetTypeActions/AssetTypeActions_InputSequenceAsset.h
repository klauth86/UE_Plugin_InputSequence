// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_InputSequenceAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("FAssetTypeActions_InputSequenceAsset", "Name", "Input Sequence Asset"); }
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override { return FColor(129, 50, 255); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
};