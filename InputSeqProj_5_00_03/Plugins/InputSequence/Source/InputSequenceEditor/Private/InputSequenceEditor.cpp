// Copyright 2022. Pentangle Studio. All rights reserved.

#include "InputSequenceEditor.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions/AssetTypeActions_InputSequenceAsset.h"
#include "Graph/InputSequenceGraphFactories.h"

#define LOCTEXT_NAMESPACE "FInputSequenceEditorModule"

const FName AssetToolsModuleName("AssetTools");
const FName PropertyEditorModuleName("PropertyEditor");

void FInputSequenceEditorModule::StartupModule()
{
	RegisteredAssetTypeActions.Add(MakeShared<FAssetTypeActions_InputSequenceAsset>());

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
	for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
	{
		if (registeredAssetTypeAction.IsValid()) AssetTools.RegisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
	}

	InputSequenceGraphNodeFactory = MakeShareable(new FInputSequenceGraphNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(InputSequenceGraphNodeFactory);

	InputSequenceGraphPinFactory = MakeShareable(new FInputSequenceGraphPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(InputSequenceGraphPinFactory);

	InputSequenceGraphPinConnectionFactory = MakeShareable(new FInputSequenceGraphPinConnectionFactory);
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(InputSequenceGraphPinConnectionFactory);
}

void FInputSequenceEditorModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(InputSequenceGraphPinConnectionFactory);
	InputSequenceGraphPinConnectionFactory.Reset();

	FEdGraphUtilities::UnregisterVisualPinFactory(InputSequenceGraphPinFactory);
	InputSequenceGraphPinFactory.Reset();

	FEdGraphUtilities::UnregisterVisualNodeFactory(InputSequenceGraphNodeFactory);
	InputSequenceGraphNodeFactory.Reset();

	if (FModuleManager::Get().IsModuleLoaded(AssetToolsModuleName))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(AssetToolsModuleName).Get();
		for (TSharedPtr<FAssetTypeActions_Base>& registeredAssetTypeAction : RegisteredAssetTypeActions)
		{
			if (registeredAssetTypeAction.IsValid()) AssetTools.UnregisterAssetTypeActions(registeredAssetTypeAction.ToSharedRef());
		}
	}

	RegisteredAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInputSequenceEditorModule, InputSequenceEditor)