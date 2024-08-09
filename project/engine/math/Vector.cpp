#include "Vector.h"

Vector3 operator+(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

Vector3 operator*(const float s, const Vector3& v) {

	Vector3 result;

	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;

	return result;
}

Vector3 operator*(const Vector3& v, const float s) {

	return s * v;

}

Vector3 operator/(const Vector3& v, const float s) {

	Vector3 result;

	result.x = v.x * (1.0f / s);
	result.y = v.y * (1.0f / s);
	result.z = v.z * (1.0f / s);

	return result;
}

Vector3 operator+(const Vector3& v) {

	return v;

}

Vector3 operator-(const Vector3& v) {

	return { -v.x,-v.y,-v.z };

}