// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Graph/InputSequenceGraphNode_Input.h"
#include "InputSequenceGraphNode_Press.generated.h"

UCLASS()
class UInputSequenceGraphNode_Press : public UInputSequenceGraphNode_Input
{
	GENERATED_UCLASS_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual void DestroyNode() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const;
};