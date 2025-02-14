// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Modes/DaftAirMode.h"

#include "DaftMoverCVars.h"
#include "Core/DaftDataModel.h"
#include "Core/DaftMovementUtils.h"
#include "Core/DaftMoverComponent.h"
#include "DaftMoverDefines.h"

#include "Components/CapsuleComponent.h"
#include "MoveLibrary/MovementUtils.h"
#include "MoveLibrary/FloorQueryUtils.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DaftAirMode)

/**
 * Generate a single substep of movement for the mode - remember this is sub-stepping against
 * network prediction plugins tick and NOT the game thread tick. This function would typically
 * process a single input command, generating a proposed move to be processed in the next tick.
 * Note that a "Move" does not encapsulate movement for a simulation tick, but rather a "sub-tick".
 * 
 * @param StartState - Inputs, sync state, and other data from the start of the tick.
 * @param TimeStep - The time step for this substep.
 * @param OutProposedMove - The proposed move to provide to the next tick that runs.
 */
void UDaftAirMode::OnGenerateMove(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove) const
{
	const FDaftMoverInputCmd* CharacterInputs = StartState.InputCmd.InputCollection.FindDataByType<FDaftMoverInputCmd>();
	const FMoverDefaultSyncState* StartingSyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(StartingSyncState);

	OutProposedMove.LinearVelocity = StartingSyncState->GetVelocity_WorldSpace();
	const float DeltaTime = TimeStep.StepMs * 0.001f;

	constexpr float TurningRateLimit = 5000.0f;
	
	// @TODO: This is going to break if the player isn't Z up. Works for now.
	OutProposedMove.AngularVelocity = UMovementUtils::ComputeAngularVelocity(
		StartingSyncState->GetOrientation_WorldSpace(),
		CharacterInputs->GetOrientationIntentDir_WorldSpace().ToOrientationRotator(),
		DeltaTime,
		TurningRateLimit);

	UE_LOGFMT(LogMover, Display, "Angular Velocity: {AngVel}", *OutProposedMove.AngularVelocity.ToString());

	OutProposedMove.DirectionIntent = CharacterInputs->GetOrientationIntentDir_WorldSpace();

	FVector MoveInputWS = OutProposedMove.DirectionIntent.ToOrientationRotator().RotateVector(CharacterInputs->GetMoveInput());
	
	UDaftMovementUtils::ApplyAcceleration(CastChecked<UDaftMoverComponent>(GetOuter()), OutProposedMove, DeltaTime, MoveInputWS, Daft::CVars::AirSpeed);

	OutProposedMove.LinearVelocity -= FVector::UpVector * Daft::CVars::GravitySpeed * DeltaTime;

    UE_LOGFMT(LogMover, Display, "Linear Velocity: {LinVel}", *OutProposedMove.LinearVelocity.ToString());
}

/**
 * Actual network prediction plugin tick - Evolution of the sync state.
 * A DeltaTime is calculated from the last NPP tick, and then we figure out how many "sub-ticks"
 * happened in that time, aggregate them together using the MovementRecord, and then apply the
 * moves to the Pawn. This is where we actually see the moves from OnGenerateMove happen.
 * 
 * @param Params - The parameters for the tick.
 * @param OutputState - The final state of the mover after the tick.
 */
void UDaftAirMode::OnSimulationTick(const FSimulationTickParams& Params, FMoverTickEndData& OutputState)
{
	const FMoverTickStartData& StartState = Params.StartState;
	USceneComponent* UpdatedComponent = Params.MovingComps.UpdatedComponent.Get();
	UPrimitiveComponent* UpdatedPrimitive = Params.MovingComps.UpdatedPrimitive.Get();
	FProposedMove ProposedMove = Params.ProposedMove;

	const FMoverDefaultSyncState* StartingSyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(StartingSyncState);

	FMoverDefaultSyncState& OutputSyncState = OutputState.SyncState.SyncStateCollection.FindOrAddMutableDataByType<FMoverDefaultSyncState>();
	
	const float DeltaSeconds = Params.TimeStep.StepMs * 0.001f;

	FMovementRecord MoveRecord;
	MoveRecord.SetDeltaSeconds(DeltaSeconds);

	UMoverBlackboard* SimBlackboard = GetMoverComponent()->GetSimBlackboard_Mutable();
	SimBlackboard->Invalidate(CommonBlackboard::LastFloorResult); // Flush last floor result.

	constexpr float FloorSweepDist = 1.f;
	constexpr float MaxWalkSlopeCosine = 0.71f;

	FFloorCheckResult NewFloor;
	
	// If we don't have cached floor information, we need to search for it again
	UFloorQueryUtils::FindFloor(UpdatedComponent, UpdatedPrimitive, FloorSweepDist,
		MaxWalkSlopeCosine, UpdatedPrimitive->GetComponentLocation(), NewFloor);

	SimBlackboard->Set(CommonBlackboard::LastFloorResult, NewFloor);
	
	OutputSyncState.MoveDirectionIntent = (ProposedMove.bHasDirIntent ? ProposedMove.DirectionIntent : FVector::ZeroVector);

	// Use the orientation intent directly. If no intent is provided, use last frame's orientation. Note that we are assuming rotation changes can't fail. 
	const FRotator StartingOrient = StartingSyncState->GetOrientation_WorldSpace();
	FRotator TargetOrient = StartingOrient;

	bool bIsOrientationChanging = false;

	// Apply orientation changes (if any)
	if (!ProposedMove.AngularVelocity.IsZero())
	{
		TargetOrient += (ProposedMove.AngularVelocity * DeltaSeconds);
		bIsOrientationChanging = (TargetOrient != StartingOrient);
	}
	
	FVector MoveDelta = ProposedMove.LinearVelocity * DeltaSeconds;
	const FQuat OrientQuat = TargetOrient.Quaternion();

	FHitResult Hit(1.f);

	if (!MoveDelta.IsNearlyZero() || bIsOrientationChanging)
	{
		UMovementUtils::TrySafeMoveUpdatedComponent(Params.MovingComps, MoveDelta, OrientQuat, true, Hit, ETeleportType::None, MoveRecord);
	}

	if(Hit.bBlockingHit)
	{
		UMovementUtils::TryMoveToSlideAlongSurface(
			FMovingComponentSet(GetMoverComponent()),
			MoveDelta,
			1.f - Hit.Time,
			OrientQuat,
			Hit.Normal,
			Hit,
			false,
			MoveRecord);
	}
	
	if(NewFloor.bWalkableFloor)
	{
		FMoverOnImpactParams ImpactParams(Daft::Modes::Air, Hit, MoveDelta);
		GetMoverComponent()->HandleImpact(ImpactParams);
		OutputState.MovementEndState.NextModeName = Daft::Modes::Walk;
	}

	Daft::CaptureFinalState(UpdatedComponent, MoveRecord, *StartingSyncState, OutputSyncState, DeltaSeconds);
}
