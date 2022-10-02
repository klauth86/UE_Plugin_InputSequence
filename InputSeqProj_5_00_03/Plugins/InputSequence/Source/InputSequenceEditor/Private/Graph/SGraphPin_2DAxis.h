// Copyright 2022. Pentangle Studio. All rights reserved.

#pragma once

#include "SGraphPin.h"

class UEdGraphPin;

class SGraphPin_2DAxis : public SGraphPin
{
public:
	
	SLATE_BEGIN_ARGS(SGraphPin_2DAxis) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual FSlateColor GetPinTextColor() const override;

protected:

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	enum ETextBoxIndex
	{
		TextBox_X,
		TextBox_Y,
		TextBox_Z
	};

	FString GetCurrentValue_X() const;

	FString GetCurrentValue_Y() const;

	TOptional<float> GetTypeInValue_Y() const;

	FString GetValue(ETextBoxIndex Index) const;

	void OnChangedValueTextBox_X(float NewValue, ETextCommit::Type CommitInfo);

	void OnChangedValueTextBox_Y(float NewValue, ETextCommit::Type CommitInfo);

	void OnChangedValueTextBox_Z(float NewValue, ETextCommit::Type CommitInfo);

	FText ToolTipText_Raw_Label() const;

	FText ToolTipText_Raw_RemovePin() const;

	FReply OnClicked_Raw_RemovePin() const;

	FReply HandleOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<SBorder> capturingWidget);

	FReply HandleOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply HandleOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void TrySetDefaultValue(const FString& VectorString);

	uint8 mouseIsCaptured : 1;
};