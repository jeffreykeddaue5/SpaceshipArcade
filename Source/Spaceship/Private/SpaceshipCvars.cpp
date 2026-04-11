#include "SpaceshipCvars.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<float> CVarMaxCruiseSpeed(
	TEXT("spaceship.maxspeed"),
	1750.0f,
	TEXT("Max spaceship cruise speed"),
	ECVF_Cheat
	);

namespace SpaceshipCVars
{
	float GetMaxCruiseSpeed()
	{
		return CVarMaxCruiseSpeed.GetValueOnAnyThread();
	}
}