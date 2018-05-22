#pragma once
#include "Entity.h"

class Skull :
	public Entity
{
public:
	Skull();
	virtual ~Skull();
	static shared_ptr<Skull> create();
	virtual void draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex)override;
	virtual void update(float dt)override;
};

