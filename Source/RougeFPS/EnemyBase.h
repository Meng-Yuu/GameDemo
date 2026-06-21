// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyAttackEndSignature);

UCLASS(Blueprintable)
class ROUGEFPS_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category="Enemy|State")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category="Enemy|Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category="Enemy|Health")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category="Enemy|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="Enemy|Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category="Enemy|Combat")
	float GetAttackDamage() const;

	UFUNCTION(BlueprintCallable, Category="Enemy|Combat")
	void SetAttackDamage(float NewAttackDamage);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Enemy|Attack")
	FEnemyAttackEndSignature ED_AttackEnd;
	
	UFUNCTION(BlueprintCallable, Category="Enemy|Attack")
	void NotifyAttackEnd();

protected:
	virtual void BeginPlay() override;

	void HandleDeath(AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category="Enemy|Health", meta=(DisplayName="On Damaged"))
	void BP_OnDamaged(float DamageAmount, float NewHealth, float HealthPercent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category="Enemy|Health", meta=(DisplayName="On Death"))
	void BP_OnDeath(AController* EventInstigator, AActor* DamageCauser);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Health", meta=(ClampMin="1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Enemy|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Enemy|State")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Combat", meta=(ClampMin="0.0"))
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|State")
	FName DeathTag = FName("Dead");
};
