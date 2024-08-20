#include "pch.h"
#include "Utils.h"

float Utils::GetYawByVector(Vector vec)
{
	Vector xAxis = Vector{ 1.f, 0.f };

	float dot = xAxis.Dot(vec);
	float dist = xAxis.Length() * vec.Length();

	if (dist == 0.f)
		return 0.f;

	Vector yAxis = Vector{ 0.f, 1.f };

	float yDot = yAxis.Dot(vec);

	float cosV = dot / dist;
	
	float value = acos(cosV) * 180.f / PI;

	if (yDot >= 0)
	{
		return value;
	}
	else return 360.f - value;
}

Vector Utils::GetVectorByYaw(float yaw)
{
	float radian = yaw * PI / 180.f;
	return { cos(radian), sin(radian) };
}
