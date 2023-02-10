﻿// Copyright (c) 2023 Jared Taylor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SprintMovement.generated.h"

class ASprintCharacter;
UCLASS()
class PREDICTEDMOVEMENT_API USprintMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
private:
	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ASprintCharacter> MyCharacterOwner;

public:
	/** The maximum ground speed when sprinting. */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxWalkSpeedSprinting;
	
public:
	/** If true, try to Sprint (or keep Sprinting) on next update. If false, try to stop Sprinting on next update. */
	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint:1;

public:
	USprintMovement();
	
	virtual void PostLoad() override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

	virtual float GetMaxSpeed() const override;
	
public:
	virtual bool IsSprinting() const;

	/**
	 * Call CharacterOwner->OnStartSprint() if successful.
	 * In general you should set bWantsToSprint instead to have the Sprint persist during movement, or just use the Sprint functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsSprinted is replicated to non owned clients.
	 */
	virtual void Sprint(bool bClientSimulation = false);
	
	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndSprint() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsSprinted is replicated to non owned clients.
	 */
	virtual void UnSprint(bool bClientSimulation = false);

	/** Returns true if the character is allowed to Sprint in the current state. By default it is allowed when walking or falling. */
	virtual bool CanSprintInCurrentState() const;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

protected:
	virtual bool ClientUpdatePositionAfterServerUpdate() override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
public:
	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};

class PREDICTEDMOVEMENT_API FSavedMove_Character_Sprint : public FSavedMove_Character
{
public:
	FSavedMove_Character_Sprint()
		: bWantsToSprint(0)
	{
	}

	virtual ~FSavedMove_Character_Sprint() override
	{}

	uint32 bWantsToSprint:1;
		
	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;
};

class PREDICTEDMOVEMENT_API FNetworkPredictionData_Client_Character_Sprint : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	FNetworkPredictionData_Client_Character_Sprint(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
	{}

	virtual FSavedMovePtr AllocateNewMove() override;
};
