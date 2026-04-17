#include "SpaceshipCvars.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<float> CVarCruiseMaxSpeed(
	TEXT("spaceship.CruiseMaxSpeed"),
	1750.0f,
	TEXT("Spaceship Cruise Max Speed"),
	ECVF_Cheat
	);

static TAutoConsoleVariable<float> CVarCruiseAccelerationRate(
	TEXT("spaceship.CruiseAccelerationeRate"),
	1200.0f,
	TEXT("Spaceship Cruise Acceleration rate"),
	ECVF_Cheat
	);

static TAutoConsoleVariable<float> CVarCruiseDecelerationRate(
	TEXT("spaceship.CruiseDecelerationRate"),
	1800.0f,
	TEXT("Spaceship Cruise Deceleration rate"),
	ECVF_Cheat
	);


namespace SpaceshipCVars
{
	float GetCruiseMaxSpeed()
	{
		return CVarCruiseMaxSpeed.GetValueOnAnyThread();
	}
	float GetCruiseAccelerationRate()
	{
		return CVarCruiseAccelerationRate.GetValueOnAnyThread();
	}
	float GetCruiseDecelerationRate()
	{
		return CVarCruiseDecelerationRate.GetValueOnAnyThread();
	}
}