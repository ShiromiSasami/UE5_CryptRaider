// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"

#include <Engine/World.h>
#include <DrawDebugHelpers.h>
#include <PhysicsEngine/PhysicsHandleComponent.h>

// Sets default values for this component's properties
UGrabber::UGrabber()
	:MaxGrabDistance(400.f)
	, GrabRadius(100.f)
	, HoldDistance(200.f)
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	if (auto PhysicsHandle = GetPhysicsHandle())
	{
		UE_LOG(LogTemp, Display, TEXT("Got Physics Handle: %s"), *PhysicsHandle->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Physics Handle Found!"));
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto PhysicsHandle = GetPhysicsHandle();
	if(PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab()
{
	auto PhysicsHandle = GetPhysicsHandle();
	
	FHitResult HitResult;
	if (PhysicsHandle && GetGrabbableInReach(HitResult))
	{
		auto HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		auto HitActor = HitResult.GetActor();
		HitActor->Tags.Add("Grabbed");
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);
	}
}

void UGrabber::Release()
{
	auto PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{ 
		auto GrabbedActor = PhysicsHandle->GetGrabbedComponent()->GetOwner();
		GrabbedActor->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	auto Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!Result)
	{
		UE_LOG(LogTemp, Display, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}

	return Result;
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Blue, true, 5);

	FCollisionShape Shape = FCollisionShape::MakeSphere(GrabRadius);
	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Shape
	);
}
