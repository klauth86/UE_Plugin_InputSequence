// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "EdGraph/EdGraph.h"
#include "InputSequenceGraph.generated.h"

UCLASS()
class UInputSequenceGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:

	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
};