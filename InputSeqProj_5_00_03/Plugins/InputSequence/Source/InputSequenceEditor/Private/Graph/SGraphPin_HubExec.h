// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "KismetPins/SGraphPinExec.h"

class UEdGraphPin;

class SGraphPin_HubExec : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGraphPin_HubExec) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:

	FText ToolTipText_Raw_RemovePin() const;

	FReply OnClicked_Raw_RemovePin() const;

protected:

	const FSlateBrush* CachedImg_Pin_ConnectedHovered;

	const FSlateBrush* CachedImg_Pin_DisconnectedHovered;
};