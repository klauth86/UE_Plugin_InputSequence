// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "Graph/InputSequenceGraphNode_Input.h"
#include "InputSequenceGraphNode_Axis.generated.h"

UCLASS()
class UInputSequenceGraphNode_Axis : public UInputSequenceGraphNode_Input
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const;
};