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
		return { x * cos(degree) - y * sin(degree), x * sin(degree) + y * cos(degree) };
	}
};
