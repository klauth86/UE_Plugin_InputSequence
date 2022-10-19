// Copyright 2022 Pentangle Studio Licensed under the Apache License, Version 2.0 (the «License»);

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;
class SStickZone;

class SGraphPin_2DAxis : public SGraphPin
{
public:

	enum ETextBoxIndex
	{
		TextBox_X,
		TextBox_Y,
		TextBox_Z
	};

	SLATE_BEGIN_ARGS(SGraphPin_2DAxis) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual ~SGraphPin_2DAxis();

	virtual FSlateColor GetPinTextColor() const override;

protected:

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	FString GetCurrentValue_X() const;

	FString GetCurrentValue_Y() const;

	TOptional<float> GetTypeInValue_Z() const;

	FString GetValue(ETextBoxIndex Index) const;

	void OnChangedValueTextBox_X(float NewValue, ETextCommit::Type CommitInfo);

	void OnChangedValueTextBox_Y(float NewValue, ETextCommit::Type CommitInfo);

	void OnChangedValueTextBox_Z(float NewValue, ETextCommit::Type CommitInfo);

	FText ToolTipText_Raw_Label() const;

	FText ToolTipText_Raw_RemovePin() const;

	FReply OnClicked_Raw_RemovePin() const;

	void EvalAndSetValueFromMouseEvent(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void TrySetDefaultValue(const FString& VectorString);

	void OnStickZoneValueChanged(float NewValue, ETextBoxIndex Index);

	TSharedPtr<SStickZone> StickZone;
};