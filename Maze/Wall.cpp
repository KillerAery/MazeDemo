#include "Wall.h"



Wall::Wall(){
	//…Ë÷√≤ƒ¡œ Ù–‘
	mMat.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mMat.Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}


Wall::~Wall(){
}

shared_ptr<Wall> Wall::create(){
	shared_ptr<Wall> wall = std::make_shared<Wall>();
	return wall;
}

void Wall::update(float dt) {

}