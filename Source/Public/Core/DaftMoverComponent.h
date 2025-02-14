// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "MoverComponent.h"
#include "DaftMoverComponent.generated.h"

UCLASS()
class DAFTMOVER_API UDaftMoverComponent : public UMoverComponent
{
	GENERATED_BODY()
public:

	UDaftMoverComponent();

	virtual FVector GetFeetLocation();
	
	//~ Begin UMoverComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool IsAirborne() const;
	virtual bool IsOnGround() const;
	//~ End UMoverComponent
};
