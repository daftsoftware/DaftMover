// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "DaftMoverCVars.h"

namespace Daft::CVars
{
	bool DrawMovementDebug = true;
	FAutoConsoleVariableRef CVarDrawMovementDebug(
		TEXT("Daft.DrawMovementDebug"),
		DrawMovementDebug,
		TEXT("Draws debug helpers for Daft Movement (0/1)."),
		ECVF_Default);
	
	float JumpForce = 300.0f;
	FAutoConsoleVariableRef CVarJumpForce(
		TEXT("Daft.Move.JumpForce"),
		JumpForce,
		TEXT("Z Force to add on a player jump."),
		ECVF_Default
	);

	float CrouchSpeedMult = 0.75f;
	FAutoConsoleVariableRef CVarCrouchSpeedMult(
		TEXT("Daft.Move.CrouchSpeedMult"),
		CrouchSpeedMult,
		TEXT("Multiplier for walk speed when crouching."),
		ECVF_Default
	);

	float SprintSpeedMult = 1.5f;
	FAutoConsoleVariableRef CVarSprintSpeedMult(
		TEXT("Daft.Move.SprintSpeedMult"),
		SprintSpeedMult,
		TEXT("Multiplier for walk speed when sprinting."),
		ECVF_Default
	);

	float GroundDamping = 6.0f;
	FAutoConsoleVariableRef CVarGroundDamping(
		TEXT("Daft.Move.GroundDamping"),
		GroundDamping,
		TEXT("Ground damping amount, before additional damping is factored in."),
		ECVF_Default
	);

	float AirDamping = 5.0f;
	FAutoConsoleVariableRef CVarAirDamping(
		TEXT("Daft.Move.AirDamping"),
		AirDamping,
		TEXT("Air damping amount, before additional damping is factored in."),
		ECVF_Default
	);

	float GroundAcceleration = 8.0f;
    FAutoConsoleVariableRef CVarGroundAcceleration(
    	TEXT("Daft.Move.GroundAcceleration"),
    	GroundAcceleration,
    	TEXT("Ground acceleration amount, providing ground control."),
    	ECVF_Default
    );
    
    float AirAcceleration = 2.0f;
    FAutoConsoleVariableRef CVarAirAcceleration(
    	TEXT("Daft.Move.AirAcceleration"),
    	AirAcceleration,
    	TEXT("Air acceleration amount, providing air control."),
    	ECVF_Default
    );
	
	float GroundSpeed = 1200.0f;
	FAutoConsoleVariableRef CVarGroundSpeed(
		TEXT("Daft.Move.GroundSpeed"),
		GroundSpeed,
		TEXT("Constant walk speed to apply."),
		ECVF_Default
	);

	float SlipFactor = 750.0f;
	FAutoConsoleVariableRef CVarSlipFactor(
		TEXT("Daft.Move.SlipFactor"),
		SlipFactor,
		TEXT("Minimum speed threshold for slip (Drag force is scaled at this speed at minimum)."),
		ECVF_Default
	);

	float AirSpeed = 1200.0f;
	FAutoConsoleVariableRef CVarAirSpeed(
		TEXT("Daft.Move.AirSpeed"),
		AirSpeed,
		TEXT("Constant air speed to apply."),
		ECVF_Default
	);

	float GravitySpeed = 800.0f;
	FAutoConsoleVariableRef CVarGravitySpeed(
		TEXT("Daft.Move.GravitySpeed"),
		GravitySpeed,
		TEXT("Constant gravity speed to apply."),
		ECVF_Default
	);
}
