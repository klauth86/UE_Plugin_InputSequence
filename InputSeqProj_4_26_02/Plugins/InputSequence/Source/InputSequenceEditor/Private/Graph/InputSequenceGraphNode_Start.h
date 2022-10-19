// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Start.generated.h"

UCLASS()
class UInputSequenceGraphNode_Start : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FLinearColor GetNodeTitleColor() const override;

	virtual FText GetTooltipText() const override;

	virtual bool CanDuplicateNode() const override { return false; }

	virtual bool CanUserDeleteNode() const override { return false; }
};