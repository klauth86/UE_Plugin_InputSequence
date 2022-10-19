// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "InputSequenceGraphNode_Base.generated.h"

UCLASS()
class UInputSequenceGraphNode_Base : public UEdGraphNode
{
	GENERATED_BODY()

public:

	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
};