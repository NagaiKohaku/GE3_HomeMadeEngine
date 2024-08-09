#pragma once
#include "Vector.h"
#include "Matrix.h"

struct Line {
	Vector3 origin; //始点
	Vector3 diff; //終点の差分ベクトル
};

struct Ray {
	Vector3 origin; //始点
	Vector3 diff; //終点の差分ベクトル
};

struct Segment {
	Vector3 origin; //始点
	Vector3 diff; //終点の差分ベクトル
};

class Vector3Math {

public:

	//加算
	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	//減算
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);

	//スカラー倍
	static Vector3 Multiply(float scalar, const Vector3& v);

	//内積
	static float Dot(const Vector3& v1, const Vector3& v2);

	//長さ(ノルム)
	static float Length(const Vector3& v);

	//正規化
	static Vector3 Normalize(const Vector3& v);

	//垂直
	static Vector3 Perpendicular(const Vector3& v);

	//クロス積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	//正射影ベクトルの生成
	static Vector3 Project(const Vector3& v1, const Vector3& v2);

	//最近接点の生成
	static Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

	//ベクトル変換
	static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
};