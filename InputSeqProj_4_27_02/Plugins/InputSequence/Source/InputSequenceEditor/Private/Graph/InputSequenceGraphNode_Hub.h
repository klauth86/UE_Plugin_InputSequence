// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Graph/InputSequenceGraphNode_Dynamic.h"
#include "InputSequenceGraphNode_Hub.generated.h"

UCLASS()
class UInputSequenceGraphNode_Hub : public UInputSequenceGraphNode_Dynamic
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const override;
};