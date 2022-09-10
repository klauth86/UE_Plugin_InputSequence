// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Graph/InputSequenceGraphNode_Input.h"
#include "InputSequenceGraphNode_Release.generated.h"

UCLASS()
class UInputSequenceGraphNode_Release : public UInputSequenceGraphNode_Input
{
	GENERATED_UCLASS_BODY()

public:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const;

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual bool CanUserDeleteNode() const override { return false; }

	bool CanBePassedAfterTime() const { return canBePassedAfterTime; }

	float GetPassedAfterTime() const { return PassedAfterTime; }

protected:

	/* Node Time interval, after which node can be passed */
	UPROPERTY(EditAnywhere, Category = "Release node", meta = (DisplayPriority = 1, UIMin = 0.01, Min = 0.01, UIMax = 10, Max = 10, EditCondition = "EditConditionIndex > 1 && canBePassedAfterTime"))
		float PassedAfterTime;
};