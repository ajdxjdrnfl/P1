#include "pch.h"
#include "Utils.h"

float Utils::GetYawByVector(Vector vec)
{
	Vector xAxis = Vector{ 1.f, 0.f };

	float dot = xAxis.Dot(vec);
	float dist = xAxis.Length() * vec.Length();
	float cosV = dot / dist;

	if (dist == 0.f)
		return 0.f;

	else return acos(cosV) * 180.f / PI;
}

Vector Utils::GetVectorByYaw(float yaw)
{
	float radian = yaw * PI / 180.f;
	return { cos(radian), sin(radian) };
}
