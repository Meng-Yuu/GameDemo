// Copyright Epic Games, Inc. All Rights Reserved.

#include "RougeFPSCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RougeFPS.h"

ARougeFPSCharacter::ARougeFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// ------------------------------------------------------------
	// Third Person / World Mesh
	// ------------------------------------------------------------
	// CharacterMesh0 用于世界中的角色表现，例如其他玩家看到的身体
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// ------------------------------------------------------------
	// First Person Camera
	// ------------------------------------------------------------
	// 创建第一人称相机
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));

	// 关键修改：相机不要挂在 FirstPersonMesh 上，而是挂在 Capsule 上
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());

	// 相机位置：根据角色身高调整
	// 如果进入游戏后视角太高/太低，可以主要调整 Z 值
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	// 关键：相机跟随控制器视角旋转
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// UE5 第一人称渲染相关设置
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// ------------------------------------------------------------
	// First Person Mesh
	// ------------------------------------------------------------
	// 创建第一人称手臂 Mesh，只给自己看
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	// 关键修改：第一人称手臂挂到相机下面
	// 这样视角上下左右移动时，手臂和武器都会跟随相机移动
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);

	// 手臂相对相机的位置和旋转
	// 这里通常需要你在蓝图中微调
	// 如果手臂看不到，优先调整 Location 的 X/Y/Z
	FirstPersonMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -150.0f));
	FirstPersonMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// ------------------------------------------------------------
	// Capsule
	// ------------------------------------------------------------
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// ------------------------------------------------------------
	// Character Rotation
	// ------------------------------------------------------------
	// FPS 常用设置：
	// Yaw 控制角色整体左右转
	// Pitch 只影响相机，不让整个角色身体上下翻转
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 不让角色移动方向自动改变角色朝向
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// ------------------------------------------------------------
	// Character Movement
	// ------------------------------------------------------------
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ARougeFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARougeFPSCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARougeFPSCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARougeFPSCharacter::MoveInput);

		// Looking / Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARougeFPSCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ARougeFPSCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogRougeFPS, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARougeFPSCharacter::AddWeapon(AActor* NewWeapon)
{
	if (NewWeapon == nullptr)
	{
		return;
	}

	EquippedWeapon = NewWeapon;

	// Attach the weapon to the First Person Mesh
	// 注意：WeaponPoint 必须是 FirstPersonMesh 上存在的 Socket 名称
	NewWeapon->AttachToComponent(
		FirstPersonMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("WeaponPoint")
	);

	// Set the owner of the weapon to this character
	NewWeapon->SetOwner(this);

	// 关闭武器碰撞，避免第一人称武器和角色发生碰撞
	NewWeapon->SetActorEnableCollision(false);
}

void ARougeFPSCharacter::MoveInput(const FInputActionValue& Value)
{
	// Get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ARougeFPSCharacter::LookInput(const FInputActionValue& Value)
{
	// Get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void ARougeFPSCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// Yaw: 左右转
		AddControllerYawInput(Yaw);

		// Pitch: 上下看
		AddControllerPitchInput(Pitch);
	}
}

void ARougeFPSCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// Right / Forward movement relative to character direction
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ARougeFPSCharacter::DoJumpStart()
{
	Jump();
}

void ARougeFPSCharacter::DoJumpEnd()
{
	StopJumping();
}