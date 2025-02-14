// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/DaftPawn.h"
#include "Core/DaftMoverComponent.h"
#include "Core/DaftDataModel.h"
#include "DaftMoverDefines.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DaftPawn)

ADaftPawn::ADaftPawn()
{
	SetReplicatingMovement(false);
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetHiddenInGame(false);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CapsuleComponent);
	CameraComponent->bUsePawnControlRotation = true;
	
	MoverComponent = CreateDefaultSubobject<UDaftMoverComponent>(TEXT("MoverComponent"));
}

void ADaftPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static constexpr double LookRateYaw = 150.0;	// Degs/Sec
	static constexpr double LookRatePitch = 150.0;	// Degs/Sec
	
	AddControllerYawInput(CachedLookInput.Yaw * LookRateYaw * DeltaSeconds);
	AddControllerPitchInput(CachedLookInput.Pitch * LookRatePitch * DeltaSeconds);

	CachedLookInput = FRotator3d::ZeroRotator;
}

void ADaftPawn::Move(const FInputActionValue& Value)
{
	const FVector MoveVector = Value.Get<FVector>();
	AddMovementInput(MoveVector);
}

void ADaftPawn::MoveCompleted(const FInputActionValue& Value)
{
	ConsumeMovementInputVector();
}

void ADaftPawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	CachedLookInput.Yaw = CachedTurnInput.Yaw = LookVector.X;
	CachedLookInput.Pitch = CachedTurnInput.Pitch = LookVector.Y;
}

void ADaftPawn::Jump()
{
	JumpButtonDown = true;
}

void ADaftPawn::JumpCompleted()
{
	JumpButtonDown = false;
}

void ADaftPawn::Crouch()
{
	CrouchButtonDown = true;
}

void ADaftPawn::CrouchCompleted()
{
	CrouchButtonDown = false;
}

// Produce input is used to build an input cmd for the frame.
void ADaftPawn::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd)
{
	FDaftMoverInputCmd& CharacterInputs = OutInputCmd.InputCollection.FindOrAddMutableDataByType<FDaftMoverInputCmd>();

	if (!GetController())
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
		{
			static const FDaftMoverInputCmd DoNothingInput;
			CharacterInputs = DoNothingInput;
		}
		return;
	}

	FRotator IntentRotation = GetControlRotation();
	IntentRotation.Pitch = 0.0f;
	IntentRotation.Roll = 0.0f;

	UE_LOGFMT(LogMover, Display, "Local - JumpButtonDown {Jumping}", JumpButtonDown);

	UE_LOGFMT(LogMover, Display, "Input - {Vector}", GetPendingMovementInputVector().ToString());

	CharacterInputs.ControlRotation = GetControlRotation();
	CharacterInputs.bUsingMovementBase = false;
	CharacterInputs.OrientationIntent = IntentRotation.Vector();
	CharacterInputs.bIsJumpPressed = JumpButtonDown;
	CharacterInputs.bIsCrouchPressed = CrouchButtonDown;
	CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, ConsumeMovementInputVector());
}
