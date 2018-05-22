#pragma once

#include <btBulletDynamicsCommon.h>
#include "Transform.h"

class Entity :
	public Transform
{
protected:
	/*---渲染部分---*/
	//材料属性
	Material mMat;
	//纹理贴图资源
	ID3D11ShaderResourceView* r_texSRV;
	//顶点，索引 相关信息
	int mVertexOffset;
	UINT mIndexOffset;
	UINT mIndexCount;
	/*---物理部分---*/
	//物理身体
	btRigidBody * m_body;
	//密度
	btScalar m_mass;
	//惯性
	btVector3 m_inertia;
public:
	Entity();
	virtual ~Entity();
	void initbtBody(const btRigidBody::btRigidBodyConstructionInfo& constructionInfor, btDiscreteDynamicsWorld * world);
public:
	virtual void draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex)override;
	virtual void update(float dt)override;
	void setSRV(ID3D11ShaderResourceView* SRV) { r_texSRV = SRV; }
	void setVertexOffset(int vertexOffset) { mVertexOffset = vertexOffset; }
	void setIndexOffset(UINT indexOffset) { mIndexOffset = indexOffset; }
	void setIndexCount(UINT indexCount) { mIndexCount = indexCount; }
protected:
};

