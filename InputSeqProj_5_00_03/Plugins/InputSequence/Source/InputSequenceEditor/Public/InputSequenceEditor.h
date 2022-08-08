// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FAssetTypeActions_Base;

class FInputSequenceEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetTypeActions;
	TSharedPtr<FInputSequenceGraphNodeFactory> InputSequenceGraphNodeFactory;
	TSharedPtr<FInputSequenceGraphPinFactory> InputSequenceGraphPinFactory;
	TSharedPtr<FInputSequenceGraphPinConnectionFactory> InputSequenceGraphPinConnectionFactory;
};