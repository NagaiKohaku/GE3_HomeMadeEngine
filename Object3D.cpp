#include "Object3D.h"

Object3D* Object3D::GetInstance() {
	static Object3D instance;
	return &instance;
}

void Object3D::Initialize() {

}