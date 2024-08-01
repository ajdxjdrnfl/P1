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

	else return acos(cosV);
}