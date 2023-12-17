// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

#include "Mover.h"

UTriggerComponent::UTriggerComponent()
	:AcceptableActorTag(TEXT("None"))
	, Mover(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (auto Actor = GetAcceptableActor())
	{
		if (auto Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
		{
			Component->SetSimulatePhysics(false);
		}
		Actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		Mover->SetShouldMove(true);
	}
	else
	{
		Mover->SetShouldMove(false);
	}
}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);

	for (auto Actor : Actors)
	{
		bool HasAcceptableTag = Actor->ActorHasTag(AcceptableActorTag); 
		bool IsGrabbed = Actor->ActorHasTag("Grabbed");
		if (HasAcceptableTag && !IsGrabbed)
		{
			return Actor;
		}
	}

	return nullptr;
}
