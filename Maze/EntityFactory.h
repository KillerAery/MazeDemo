#pragma once
#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>
#include "MathHelper.h"
#include "Wall.h"
#include "Skull.h"

//�Թ�����
namespace WallMaze{
	const int NodeWidth = 17;
	const int NodeHeight = 17;
	const int WallWidth = NodeWidth * 2 + 1;
	const int WallHeight = NodeHeight * 2 + 1;
}

//ʵ�����ɹ���
//Ŀǰ�������ɣ� ǽ������ͷ
class EntityFactory
{
protected:
	//�ڵ��Ƿ��߹�
	bool m_nodeWalked[WallMaze::NodeWidth][WallMaze::NodeHeight];
	/*---��Ⱦ����---*/
	//����һ��ǽ ��Ⱦ����� ���㣬���� �����Ϣ
	int mWallVertexOffset;
	UINT mWallIndexOffset;
	UINT mWallIndexCount;
	//����һ������ͷ��Ⱦ����
	int mSkullVertexOffset;
	UINT mSkullIndexOffset;
	UINT mSkullIndexCount;
	//ǽ������ͼ��Դ
	ID3D11ShaderResourceView* mWallTexSRV;

	/*---������---*/
	//Transform��Ϣ
	btMotionState* m_motionState;

	btCollisionShape* m_wallShape;	//ǽ��������״
	btScalar m_wallMass;	//ǽ���ܶ�
	btVector3 m_wallInertia;	//ǽ�Ĺ���

	btCollisionShape* m_skullShape;	//����ͷ��������״
	btScalar m_skullMass;	//����ͷ���ܶ�
	btVector3 m_skullInertia;	//����ͷ�Ĺ���
public:
	//�Ƿ����ǽ
	bool wallAlive[WallMaze::WallWidth][WallMaze::WallHeight];
	//����ǽ
	std::vector<std::shared_ptr<Wall>> walls;
	//��������ͷ
	std::vector<std::shared_ptr<Skull>> skulls;
public:
	EntityFactory();
	~EntityFactory();
	void init(int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount, int SkullVertexOffset, UINT SkullIndexOffset, UINT SkullIndexCount);
	//�����Թ�
	void createMazeEntities(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world);
	//�ж�λ���Ƿ񵽴��յ�
	bool posInEnd(CXMVECTOR position);
protected:
	void reset();
	void destroyRandomWall(int nodeX, int nodeY);
	void createWalls(btDiscreteDynamicsWorld * world);
	void createSkulls(btDiscreteDynamicsWorld * world);
};

