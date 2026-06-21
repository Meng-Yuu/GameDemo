// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyBase::NotifyAttackEnd()
{
	ED_AttackEnd.Broadcast();
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	ResetHealth();
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float ActualDamage = FMath::Max(0.0f, Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser));
	if (ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);
	BP_OnDamaged(ActualDamage, CurrentHealth, GetHealthPercent(), EventInstigator, DamageCauser);

	if (CurrentHealth <= 0.0f)
	{
		HandleDeath(EventInstigator, DamageCauser);
	}

	return ActualDamage;
}

bool AEnemyBase::IsDead() const
{
	return bIsDead;
}

float AEnemyBase::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

float AEnemyBase::GetCurrentHealth() const
{
	return CurrentHealth;
}

float AEnemyBase::GetMaxHealth() const
{
	return MaxHealth;
}

void AEnemyBase::ResetHealth()
{
	MaxHealth = FMath::Max(1.0f, MaxHealth);
	CurrentHealth = MaxHealth;
	bIsDead = false;
	Tags.Remove(DeathTag);
}

float AEnemyBase::GetAttackDamage() const
{
	return AttackDamage;
}

void AEnemyBase::SetAttackDamage(float NewAttackDamage)
{
	AttackDamage = FMath::Max(0.0f, NewAttackDamage);
}

void AEnemyBase::HandleDeath(AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHealth = 0.0f;
	Tags.AddUnique(DeathTag);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}

	BP_OnDeath(EventInstigator, DamageCauser);
}

