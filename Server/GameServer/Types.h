#pragma once

struct VectorInt
{
	int32 x, y;

	bool operator < (const VectorInt& vec) const
	{
		if (x != vec.x)
			return x < vec.x;
		else
			return y < vec.y;
	}

	VectorInt operator+(const VectorInt& other) const
	{
		VectorInt ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}

	VectorInt operator-(const VectorInt& other)
	{
		VectorInt ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}

	bool operator==(const VectorInt& other)
	{
		return x == other.x && y == other.y;
	}

	int32 Distance(VectorInt pos)
	{
		return (x - pos.x) * (x - pos.y) + (y - pos.y) * (y - pos.y);
	}

	int32 Dot(VectorInt& other)
	{
		return x * other.x + y * other.y;
	}

};

struct Vector
{
	double x;
	double y;

	bool operator < (const Vector& vec) const
	{
		if (x != vec.x)
			return x < vec.x;
		else
			return y < vec.y;
	}

	Vector operator - (const Vector& vec) const
	{
		return { x - vec.x , y - vec.y };
	}

	Vector operator + (const Vector& vec) const
	{
		return { x + vec.x , y + vec.y };
	}

	Vector operator * (float scale) const
	{
		return { x * scale , y * scale };
	}

	double Distance(Vector pos)
	{
		return sqrt(DistanceSquared(pos));
	}

	double DistanceSquared(Vector pos)
	{
		return (x - pos.x) * (x - pos.x) + (y - pos.y) * (y - pos.y);
	}

	double Length()
	{
		return sqrt(LengthSquared());
	}

	double LengthSquared()
	{
		return x * x + y * y;
	}

	Vector Normalize()
	{
		float l = Length();
		return { x / l, y / l };
	}

	double Dot(Vector pos)
	{
		return x * pos.x + y * pos.y;
	}

	Vector Rotate(float degree)
	{
		float PI = 3.141592;
		float radian = PI * degree / 180.f;

		float cosTheta = cos(radian);
		float sinTheta = sin(radian);
		return { x * cosTheta - y * sinTheta, x * sinTheta + y * cosTheta };
	}
}typedef Vector;

struct Bound
{
	Bound()
	{

	}

	Bound(float x1, float y1, float x2, float y2)
	{
		topLeft.x = x1;
		topLeft.y = y1;
		BottomRight.x = x2;
		BottomRight.y = y2;
	}

	bool IsInBound(float x, float y)
	{
		if (topLeft.x > x || BottomRight.x < x)
			return false;

		else if (topLeft.y > y || BottomRight.y < y)
			return false;

		return true;
	}

	Vector GetSize()
	{
		return { BottomRight.x - topLeft.x, BottomRight.y - topLeft.y };
	}

	Vector topLeft = { 0,0 };
	Vector BottomRight = { 0, 0 };


};

struct OBB
{
	Vector extent;
	Vector pos;
	float orientation;

	Vector GetMin()
	{
		Vector xAxis = { 1,0 };
		Vector yAxis = { 0,1 };

		Vector oX = xAxis.Rotate(orientation);
		Vector oY = yAxis.Rotate(orientation);
		oX = { oX.x * extent.x , oX.y * extent.x };
		oY = { oY.x * extent.y , oY.y * extent.y };

		Vector result = pos;

		result.x = result.x - abs(oX.x) - abs(oY.x);
		result.y = result.y - abs(oX.y) - abs(oY.y);

		return result;
	}
	Vector GetMax()
	{
		Vector xAxis = { 1,0 };
		Vector yAxis = { 0,1 };

		Vector oX = xAxis.Rotate(orientation);
		Vector oY = yAxis.Rotate(orientation);
		oX = { oX.x * extent.x , oX.y * extent.x };
		oY = { oY.x * extent.y , oY.y * extent.y };

		Vector result = pos;

		result.x = result.x + abs(oX.x) + abs(oY.x);
		result.y = result.y + abs(oX.y) + abs(oY.y);

		return result;
	}

	Vector ClosestPoint(Vector point)
	{
		Vector xAxis = { 1,0 };
		Vector yAxis = { 0,1 };

		Vector oX = xAxis.Rotate(orientation);
		Vector oY = yAxis.Rotate(orientation);

		Vector dir = point - pos;

		float X = oX.Dot(dir);
		float Y = oY.Dot(dir);

		Vector result;

		if (X > extent.x)
			X = extent.x;

		if (X < -extent.x)
			X = -extent.x;

		if (Y > extent.y)
			Y = extent.y;

		if (Y < -extent.y)
			Y = -extent.y;

		oX = oX * X;
		oY = oY * Y;

		result = oX + oY + pos;

		return result;
	}
};