// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "InputSequenceGraphSchema.generated.h"

USTRUCT()
struct FInputSequenceGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_BODY();

	FInputSequenceGraphSchemaAction_NewComment() : FEdGraphSchemaAction() {}

	FInputSequenceGraphSchemaAction_NewComment(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	FSlateRect SelectedNodesBounds;
};

USTRUCT()
struct FInputSequenceGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	UEdGraphNode* NodeTemplate;

	FInputSequenceGraphSchemaAction_NewNode() : FEdGraphSchemaAction(), NodeTemplate(nullptr) {}

	FInputSequenceGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping), NodeTemplate(nullptr)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

USTRUCT()
struct FInputSequenceGraphSchemaAction_AddPin : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FName InputName;

	int32 InputIndex;

	int32 CorrectedInputIndex;

	uint8 IsAxis : 1;
	uint8 Is2DAxis : 1;

	FInputSequenceGraphSchemaAction_AddPin() : FEdGraphSchemaAction(), InputName(NAME_None), InputIndex(INDEX_NONE), CorrectedInputIndex(INDEX_NONE), IsAxis(0), Is2DAxis(0) {}

	FInputSequenceGraphSchemaAction_AddPin(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, int32 InSectionID)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping, FText::GetEmpty(), InSectionID), InputName(NAME_None), InputIndex(INDEX_NONE), CorrectedInputIndex(INDEX_NONE), IsAxis(0), Is2DAxis(0)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

UCLASS()
class UInputSequenceGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:

	static const FName PC_Exec;

	static const FName PC_Action;

	static const FName PC_Add;

	static const FName PC_2DAxis;

	static const FName PC_Axis;

	static const FName PC_HubAdd;

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* pinA, const UEdGraphPin* pinB) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override { return FLinearColor::White; }

	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
};