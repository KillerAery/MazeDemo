#pragma once
#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>
#include "MathHelper.h"
#include "Wall.h"
#include "Skull.h"

//迷宫属性
namespace WallMaze{
	const int NodeWidth = 17;
	const int NodeHeight = 17;
	const int WallWidth = NodeWidth * 2 + 1;
	const int WallHeight = NodeHeight * 2 + 1;
}

//实体生成工厂
//目前负责生成： 墙、骷髅头
class EntityFactory
{
protected:
	//节点是否被走过
	bool m_nodeWalked[WallMaze::NodeWidth][WallMaze::NodeHeight];
	/*---渲染部分---*/
	//其中一面墙 渲染所需的 顶点，索引 相关信息
	int mWallVertexOffset;
	UINT mWallIndexOffset;
	UINT mWallIndexCount;
	//其中一个骷髅头渲染所需
	int mSkullVertexOffset;
	UINT mSkullIndexOffset;
	UINT mSkullIndexCount;
	//墙纹理贴图资源
	ID3D11ShaderResourceView* mWallTexSRV;

	/*---物理部分---*/
	//Transform信息
	btMotionState* m_motionState;

	btCollisionShape* m_wallShape;	//墙的物理形状
	btScalar m_wallMass;	//墙的密度
	btVector3 m_wallInertia;	//墙的惯性

	btCollisionShape* m_skullShape;	//骷髅头的物理形状
	btScalar m_skullMass;	//骷髅头的密度
	btVector3 m_skullInertia;	//骷髅头的惯性
public:
	//是否存在墙
	bool wallAlive[WallMaze::WallWidth][WallMaze::WallHeight];
	//所有墙
	std::vector<std::shared_ptr<Wall>> walls;
	//所有骷髅头
	std::vector<std::shared_ptr<Skull>> skulls;
public:
	EntityFactory();
	~EntityFactory();
	void init(int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount, int SkullVertexOffset, UINT SkullIndexOffset, UINT SkullIndexCount);
	//生成迷宫
	void createMazeEntities(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world);
	//判断位置是否到达终点
	bool posInEnd(CXMVECTOR position);
protected:
	void reset();
	void destroyRandomWall(int nodeX, int nodeY);
	void createWalls(btDiscreteDynamicsWorld * world);
	void createSkulls(btDiscreteDynamicsWorld * world);
};

