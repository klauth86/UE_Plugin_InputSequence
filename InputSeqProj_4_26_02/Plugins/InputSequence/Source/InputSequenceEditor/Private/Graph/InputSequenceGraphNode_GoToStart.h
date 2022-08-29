// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Graph/InputSequenceGraphNode_Base.h"
#include "InputSequenceGraphNode_GoToStart.generated.h"

UCLASS()
class UInputSequenceGraphNode_GoToStart : public UInputSequenceGraphNode_Base
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const override;
};