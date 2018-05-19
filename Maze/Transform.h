#pragma once
#include <memory>
//Í¼ÐÎ·½·¨
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"

using std::shared_ptr;

class Transform
{
protected:
	XMFLOAT3 m_position;
	XMFLOAT4X4 m_rotation;
public:
	Transform();
	virtual ~Transform();
	virtual void draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex) = 0;
	virtual void update(float dt) = 0;
	void setPosition(const XMFLOAT3& pos) { m_position = pos; }
	void setPosition(float x, float y, float z) { m_position.x = x; m_position.y = y; m_position.z = z; }
	void setPositionX(float x) { m_position.x = x; }
	void setPositionY(float y) { m_position.y = y; }
	void setPositionZ(float z) { m_position.z = z; }
	void setTransform(CXMMATRIX transform) { XMStoreFloat4x4(&m_rotation,transform); }
};

