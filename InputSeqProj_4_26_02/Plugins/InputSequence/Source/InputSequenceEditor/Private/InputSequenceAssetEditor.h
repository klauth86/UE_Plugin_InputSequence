// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "EditorUndoClient.h"
#include "Toolkits/AssetEditorToolkit.h"

class UInputSequenceAsset;
class IDetailsView;

class FInputSequenceAssetEditor : public FEditorUndoClient, public FAssetEditorToolkit
{
public:

	static const FName AppIdentifier;
	static const FName DetailsTabId;
	static const FName GraphTabId;

	void InitInputSequenceAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UInputSequenceAsset* inputSequenceAsset);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	virtual FName GetToolkitFName() const override { return FName("InputSequenceAssetEditor"); }
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("FInputSequenceAssetEditor", "BaseToolkitName", "Input Sequence Asset Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "InputSequenceAssetEditor"; }

protected:

	TSharedRef<SDockTab> SpawnTab_DetailsTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_GraphTab(const FSpawnTabArgs& Args);

	void CreateCommandList();

	void OnSelectionChanged(const TSet<UObject*>& selectedNodes);

	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient

	FGraphPanelSelectionSet GetSelectedNodes() const;

	void SelectAllNodes();

	bool CanSelectAllNodes() const { return true; }

	void DeleteSelectedNodes();

	bool CanDeleteNodes() const;
	
	void CopySelectedNodes();

	bool CanCopyNodes() const;

	void DeleteSelectedDuplicatableNodes();

	void CutSelectedNodes() { CopySelectedNodes(); DeleteSelectedDuplicatableNodes(); }

	bool CanCutNodes() const { return CanCopyNodes() && CanDeleteNodes(); }

	void PasteNodes();

	bool CanPasteNodes() const;

	void DuplicateNodes() { CopySelectedNodes(); PasteNodes(); }

	bool CanDuplicateNodes() const { return CanCopyNodes(); }

	void OnCreateComment();

	bool CanCreateComment() const;

protected:

	UInputSequenceAsset* InputSequenceAsset;

	TSharedPtr<FUICommandList> GraphEditorCommands;

	TWeakPtr<SGraphEditor> GraphEditorPtr;

	TSharedPtr<IDetailsView> DetailsView;
};