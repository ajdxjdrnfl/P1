// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

bool UItemSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    TArray<UClass*> ChildClasses;
    GetDerivedClasses(GetClass(), ChildClasses, false);
    return (ChildClasses.Num() == 0);
}
