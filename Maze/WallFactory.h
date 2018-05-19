#pragma once
#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>
#include "MathHelper.h"
#include "Wall.h"

const int NodeWidth = 12;
const int NodeHeight = 12;
const int WallWidth = NodeWidth * 2 + 1;
const int WallHeight = NodeHeight * 2 + 1;

class WallFactory
{
protected:
	//�ڵ��Ƿ��߹�
	bool m_nodeWalked[NodeWidth][NodeHeight];
	//�Ƿ����ǽ
	bool m_wallAlive[WallWidth][WallHeight];

	/*---��Ⱦ����---*/
	//����һ��ǽ ��Ⱦ����� ���㣬���� �����Ϣ
	int mWallVertexOffset;
	UINT mWallIndexOffset;
	UINT mWallIndexCount;
	//ǽ������ͼ��Դ
	ID3D11ShaderResourceView* mWallTexSRV;

	/*---������---*/
	//ǽ��������״
	btCollisionShape* m_wallShape;
	//ǽ��Transform
	btMotionState* m_motionState;
	//ǽ���ܶ�
	btScalar m_wallMass;
	//ǽ�Ĺ���
	btVector3 m_wallInertia;
public:
	//����ǽ
	std::vector<std::shared_ptr<Wall>> walls;
public:
	WallFactory();
	~WallFactory();
	void init();
	//�����Թ�
	void generateMazeWalls(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world, int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount);
protected:
	void reset();
	void destroyRandomWall(int nodeX, int nodeY);
	void generateWalls(btDiscreteDynamicsWorld * world);
	void btRelease();
};

