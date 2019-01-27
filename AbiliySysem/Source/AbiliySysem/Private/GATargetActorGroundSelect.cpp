// Copyright @MazenMorgan

#include "GATargetActorGroundSelect.h"


#include "Abilities/GameplayAbility.h"

void AGATargetActorGroundSelect::StartTargeting(UGameplayAbility* Ability)
{
	OwningAbility = Ability;
	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
}

void AGATargetActorGroundSelect::ConfirmTargetingAndContinue()
{
	FVector ViewPoint;
	GetPlayerLookingPoint(ViewPoint);

	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>> OverlappedActors;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	APawn* MasterPawn = MasterPC->GetPawn();
	if (MasterPawn)
	{
		QueryParams.AddIgnoredActor(MasterPawn);
	}

	bool bTryOverlap = GetWorld()->OverlapMultiByObjectType(Overlaps, ViewPoint,
		FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(SphereRadius), QueryParams);

	if (bTryOverlap)
	{
		for (int i = 0; i < Overlaps.Num(); ++i)
		{
			APawn* PawnOverlapped = Cast<APawn>(Overlaps[i].GetActor());
			if (PawnOverlapped)
			{
				OverlappedActors.AddUnique(PawnOverlapped);
			}
		}
	}
}

bool AGATargetActorGroundSelect::GetPlayerLookingPoint(OUT FVector& OutViewPoint)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	if (MasterPC)
	{
		MasterPC->GetActorEyesViewPoint(ViewPoint, ViewRotation);
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	APawn* MasterPawn = MasterPC->GetPawn();

	if (MasterPawn)
	{
		QueryParams.AddIgnoredActor(MasterPawn); // Or WE can use MasterPawn->GetUniqueID
	}

	bool TryLineTrace = GetWorld()->LineTraceSingleByChannel(HitResult, ViewPoint, ViewPoint + ViewRotation.Vector() * 10000.0f, ECC_Visibility, QueryParams);

	if (TryLineTrace)
	{
		OutViewPoint = HitResult.ImpactPoint;
	}
	else
	{
		OutViewPoint = FVector();
	}

	return TryLineTrace;
}
