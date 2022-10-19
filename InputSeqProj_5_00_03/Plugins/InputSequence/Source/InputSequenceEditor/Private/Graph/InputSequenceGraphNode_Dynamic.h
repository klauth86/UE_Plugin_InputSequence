// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

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