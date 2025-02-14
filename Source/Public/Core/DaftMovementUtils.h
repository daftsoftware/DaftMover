// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "MoverSimulationTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MoveLibrary/MovementRecord.h"
#include "DaftMovementUtils.generated.h"

class UDaftMoverComponent;
struct FProposedMove;

// @TODO: Remove or put into MovementUtils class.
namespace Daft
{
	FORCEINLINE bool AttemptTeleport(USceneComponent* UpdatedComponent, const FVector& TeleportPos, const FRotator& TeleportRot, const FMoverDefaultSyncState& StartingSyncState, FMoverTickEndData& Output)
	{
		if (UpdatedComponent->GetOwner()->TeleportTo(TeleportPos, TeleportRot))
		{
			FMoverDefaultSyncState& OutputSyncState = Output.SyncState.SyncStateCollection.FindOrAddMutableDataByType<FMoverDefaultSyncState>();
	
			OutputSyncState.SetTransforms_WorldSpace(UpdatedComponent->GetComponentLocation(),
				UpdatedComponent->GetComponentRotation(),
				StartingSyncState.GetVelocity_WorldSpace(),
				nullptr); // no movement base
	
			UpdatedComponent->ComponentVelocity = StartingSyncState.GetVelocity_WorldSpace();
			return true;
		}
	
		return false;
	}

	// TODO: replace this function with simply looking at/collapsing the MovementRecord
	FORCEINLINE void CaptureFinalState(USceneComponent* UpdatedComponent, FMovementRecord& Record, const FMoverDefaultSyncState& StartSyncState, FMoverDefaultSyncState& OutputSyncState, const float DeltaSeconds)
	{
		const FVector FinalLocation = UpdatedComponent->GetComponentLocation();
		const FVector FinalVelocity = Record.GetRelevantVelocity();
		
		// TODO: Update Main/large movement record with substeps from our local record
	
		OutputSyncState.SetTransforms_WorldSpace(FinalLocation,
			UpdatedComponent->GetComponentRotation(),
			FinalVelocity,
			nullptr); // no movement base
	
		UpdatedComponent->ComponentVelocity = FinalVelocity;
	}
}

/**
 * Mover utility functions.
 */
UCLASS()
class DAFTMOVER_API UDaftMovementUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Apply damper force to a proposed move, applying any ground friction or drag.
	 * 
	 * @param MoverComponent - The mover component.
	 * @param Move - The proposed move to apply damping to.
	 * @param DeltaTime - Time passed since last tick.
	 */
	UFUNCTION(BlueprintCallable, Category = Mover)
	static void ApplyDamping(UDaftMoverComponent* MoverComponent, FProposedMove& Move, float DeltaTime);

	/**
	 * Project current velocity onto a direction intent, accelerating towards the
	 * new direction and outputting the new velocity to a proposed move.
	 *
	 * @param MoverComponent - The mover component.
	 * @param Move - The proposed move to apply acceleration to.
	 * @param DeltaTime - Time passed since last tick.
	 * @param DirectionIntent - The direction to accelerate in.
	 * @param DesiredSpeed - The speed to accelerate to.
	 */
	UFUNCTION(BlueprintCallable, Category = Mover)
	static void ApplyAcceleration(UDaftMoverComponent* MoverComponent, FProposedMove& Move, float DeltaTime, FVector DirectionIntent, float DesiredSpeed);
};