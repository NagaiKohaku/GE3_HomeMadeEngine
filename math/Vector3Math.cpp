#include "Vector3Math.h"
#include "math.h"

//加算
Vector3 Vector3Math::Add(const Vector3& v1, const Vector3& v2) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;

}

//減算
Vector3 Vector3Math::Subtract(const Vector3& v1, const Vector3& v2) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;

}

//スカラー倍
Vector3 Vector3Math::Multiply(float scalar, const Vector3& v) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;

	return result;

}

//内積
float Vector3Math::Dot(const Vector3& v1, const Vector3& v2) {

	float result = 0.0f;

	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	return result;

}

//長さ(ノルム)
float Vector3Math::Length(const Vector3& v) {

	float result = 0.0f;

	result = sqrtf(Dot(v, v));

	return result;

}


//正規化
Vector3 Vector3Math::Normalize(const Vector3& v) {

	Vector3 result{ 0.0f,0.0f,0.0f };

	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);

	return result;

}

Vector3 Vector3Math::Perpendicular(const Vector3& v) {

	if (v.x != 0.0f || v.y != 0.0f) {
		return { -v.y,v.x,0.0f };
	}

	return{ 0.0f,-v.z,v.y };
}

//クロス積
Vector3 Vector3Math::Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 result{
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};

	return result;
}

//正射影ベクトルの生成
Vector3 Vector3Math::Project(const Vector3& v1, const Vector3& v2) {

	float dot = Vector3Math::Dot(v1, v2);

	float norm = powf(Vector3Math::Length(v2), 2.0f);

	Vector3 result = {
		dot / norm * (v2.x),
		dot / norm * (v2.y),
		dot / norm * (v2.z),
	};

	return result;
}

//最近接点の生成
Vector3 Vector3Math::ClosestPoint(const Vector3& point, const Segment& segment) {

	Vector3 project = Project(Vector3Math::Subtract(point, segment.origin), segment.diff);

	Vector3 result;

	result.x = segment.origin.x + project.x;
	result.y = segment.origin.y + project.y;
	result.z = segment.origin.z + project.z;

	return result;
}