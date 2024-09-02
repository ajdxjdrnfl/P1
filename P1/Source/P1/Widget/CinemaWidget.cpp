// Fill out your copyright notice in the Description page of Project Settings.


#include "CinemaWidget.h"

void UCinemaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnCinemaAnimationEndDelegate.BindDynamic(this, &UCinemaWidget::OnCinemaAnimationEnd);
	BindToAnimationFinished(CinemaEndAnimation, OnCinemaAnimationEndDelegate);

}

void UCinemaWidget::PlayCinemaStartAnimation()
{
	PlayAnimation(CinemaStartAnimation);
}

void UCinemaWidget::PlayCinemaEndAnimation()
{
	PlayAnimation(CinemaEndAnimation);
}

void UCinemaWidget::OnCinemaAnimationEnd()
{
	RemoveFromParent();
}
