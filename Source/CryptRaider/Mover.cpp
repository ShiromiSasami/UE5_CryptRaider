#include "Mover.h"

#include <Math/UnrealMathUtility.h>

UMover::UMover()
	:MoveOffset(0.f, 0.f, 0.f)
	,MoveTime(4.f)
	,ShouldMove(false)
	,OriginalLocation(0.f, 0.f, 0.f)
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UMover::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetOwner()->GetActorLocation();
	
}

void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector TargetLocation = OriginalLocation;

	if (ShouldMove)
	{
		TargetLocation = OriginalLocation + MoveOffset;
	}

	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	const float Speed = MoveOffset.Length() / MoveTime;

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
	GetOwner()->SetActorLocation(NewLocation);
}

