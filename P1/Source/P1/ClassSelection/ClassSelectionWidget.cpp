// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassSelectionWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LeftButton->OnClicked.AddDynamic(this, &UClassSelectionWidget::OnLeftButtonClicked);
	RightButton->OnClicked.AddDynamic(this, &UClassSelectionWidget::OnRightButtonClicked);
	SelectButton->OnClicked.AddDynamic(this, &UClassSelectionWidget::OnSelectButtonClicked);

	ClassMap.Add(0, Protocol::CasterType::CASTER_TYPE_WARRIOR);
	ClassMap.Add(1, Protocol::CasterType::CASTER_TYPE_ARCHER);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
}

void UClassSelectionWidget::OnSelectButtonClicked()
{
	{
		Protocol::C_ENTER_GAME Pkt;
		Pkt.set_castertype(*ClassMap.Find(ClassIndex));
		Pkt.set_roomtype(Protocol::RoomType::ROOM_TYPE_FIELD);

		SEND_PACKET(Pkt);
	}
}

void UClassSelectionWidget::OnRightButtonClicked()
{
	ClassIndex++;
	if (ClassIndex == ClassMaterials.Num()) ClassIndex = 0;
	MeshCaptureImage->SetBrushFromMaterial(ClassMaterials[ClassIndex]);
}

void UClassSelectionWidget::OnLeftButtonClicked()
{
	ClassIndex--;
	if (ClassIndex == -1) ClassIndex = ClassMaterials.Num() - 1;
	MeshCaptureImage->SetBrushFromMaterial(ClassMaterials[ClassIndex]);
}
