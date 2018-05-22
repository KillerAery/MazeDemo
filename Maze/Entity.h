#pragma once

#include <btBulletDynamicsCommon.h>
#include "Transform.h"

class Entity :
	public Transform
{
protected:
	/*---��Ⱦ����---*/
	//��������
	Material mMat;
	//������ͼ��Դ
	ID3D11ShaderResourceView* r_texSRV;
	//���㣬���� �����Ϣ
	int mVertexOffset;
	UINT mIndexOffset;
	UINT mIndexCount;
	/*---������---*/
	//��������
	btRigidBody * m_body;
	//�ܶ�
	btScalar m_mass;
	//����
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

