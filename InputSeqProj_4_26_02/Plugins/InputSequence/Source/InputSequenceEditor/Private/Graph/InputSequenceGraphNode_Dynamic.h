// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "Graph/InputSequenceGraphNode_Base.h"
#include "InputSequenceGraphNode_Dynamic.generated.h"

UCLASS()
class UInputSequenceGraphNode_Dynamic : public UInputSequenceGraphNode_Base
{
	GENERATED_BODY()

public:

	DECLARE_DELEGATE(FInvalidateWidgetEvent);

	FInvalidateWidgetEvent OnUpdateGraphNode;
};