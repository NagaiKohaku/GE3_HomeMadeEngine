#pragma once

struct Vector4 {
	float x, y, z, w;
};

struct Vector3 {
	float x, y, z;
};

struct Vector2 {
	float x, y;
};

Vector3 operator+(const Vector3& v1, const Vector3& v2);

Vector3 operator-(const Vector3& v1, const Vector3& v2);

Vector3 operator*(const float s, const Vector3& v);

Vector3 operator*(const Vector3& v, const float s);

Vector3 operator*(const Vector3& v1, const Vector3& v2);

Vector3 operator/(const Vector3& v, const float s);

Vector3 operator+(const Vector3& v);

Vector3 operator-(const Vector3& v);