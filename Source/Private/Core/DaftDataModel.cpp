// Copyright (c) 2025 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/DaftDataModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DaftDataModel)

FDaftMoverInputCmd::FDaftMoverInputCmd()
	: bIsCrouchPressed(false)
{}

FMoverDataStructBase* FDaftMoverInputCmd::Clone() const
{
	FDaftMoverInputCmd* CopyPtr = new FDaftMoverInputCmd(*this);
	return CopyPtr;
}

bool FDaftMoverInputCmd::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar.SerializeBits(&bIsCrouchPressed, 1);
	return FCharacterDefaultInputs::NetSerialize(Ar, Map, bOutSuccess);
}

void FDaftMoverInputCmd::ToString(FAnsiStringBuilderBase& Out) const
{
	FCharacterDefaultInputs::ToString(Out);
	Out.Appendf("bIsCrouchPressed: %i\n", bIsCrouchPressed);
}
