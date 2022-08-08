// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "EdGraph/EdGraph.h"
#include "InputSequenceGraph.generated.h"

UCLASS()
class UInputSequenceGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
};