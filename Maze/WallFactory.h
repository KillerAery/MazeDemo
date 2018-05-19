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
	//节点是否被走过
	bool m_nodeWalked[NodeWidth][NodeHeight];
	//是否存在墙
	bool m_wallAlive[WallWidth][WallHeight];

	/*---渲染部分---*/
	//其中一面墙 渲染所需的 顶点，索引 相关信息
	int mWallVertexOffset;
	UINT mWallIndexOffset;
	UINT mWallIndexCount;
	//墙纹理贴图资源
	ID3D11ShaderResourceView* mWallTexSRV;

	/*---物理部分---*/
	//墙的物理形状
	btCollisionShape* m_wallShape;
	//墙的Transform
	btMotionState* m_motionState;
	//墙的密度
	btScalar m_wallMass;
	//墙的惯性
	btVector3 m_wallInertia;
public:
	//所有墙
	std::vector<std::shared_ptr<Wall>> walls;
public:
	WallFactory();
	~WallFactory();
	void init();
	//生成迷宫
	void generateMazeWalls(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world, int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount);
protected:
	void reset();
	void destroyRandomWall(int nodeX, int nodeY);
	void generateWalls(btDiscreteDynamicsWorld * world);
	void btRelease();
};

