// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "SGraphNode.h"

class SInputSequenceGraphNode_Dynamic : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SInputSequenceGraphNode_Dynamic) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

	virtual ~SInputSequenceGraphNode_Dynamic();
};