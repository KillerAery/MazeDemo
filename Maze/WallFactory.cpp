#include "WallFactory.h"

using namespace std;

WallFactory::WallFactory() :mWallTexSRV(nullptr),m_wallShape(nullptr),m_motionState(nullptr)
{
}

WallFactory::~WallFactory()
{
	btRelease();
	ReleaseCOM(mWallTexSRV);
}

void WallFactory::init(){
	//初始化墙的物理形状
	m_wallShape = new btBoxShape(btVector3(1.0f,2.5f,1.0f));
	//0为静态刚体，其余为动态刚体
	m_wallMass = 0;
	m_wallInertia = btVector3(0, 0, 0);
	m_wallShape->calculateLocalInertia(m_wallMass, m_wallInertia);
	m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
}

void WallFactory::generateMazeWalls(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world, int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount) {
	//重置墙体信息
	reset();
	//顶点索引相关信息
	mWallIndexCount = WallIndexCount;
	mWallIndexOffset = WallIndexOffset;
	mWallVertexOffset = WallVertexOffset;
	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/bricks.dds", 0, 0, &mWallTexSRV, 0));
	//开始随机拆墙
	destroyRandomWall(0, 0);
	//根据所剩墙 生成 游戏对象
	generateWalls(world);
}

void WallFactory::reset(){
	//清理所有墙
	walls.clear();
	//设置墙的存在性
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i) {
			//节点所在位置不存在墙
			if (j % 2 == 1 && i % 2 == 1)
				m_wallAlive[i][j] = false;
			//其余地方一开始都有墙
			else
				m_wallAlive[i][j] = true;
		}
	//设置 所有节点 的 是否经过
	for (int j = 0; j < NodeHeight; ++j)
		for (int i = 0; i < NodeWidth; ++i)
			m_nodeWalked[i][j] = false;
}


void WallFactory::destroyRandomWall(int X, int Y) {
	int direction = rand() % 4;
	//搜索4个方向
	//4个方向顺序 随机
	for (int i = 0; i < 4; ++i) {

		int nextX = X;
		int nextY = Y;

		switch (direction)
		{
		case 0://上
			nextY = Y - 1 >= 0 ? Y - 1 : Y + 1;
			break;
		case 1://下
			nextY = Y + 1 < NodeHeight ? Y + 1 : Y - 1;
			break;
		case 2://左
			nextX = X - 1 >= 0 ? X - 1 : X + 1;
			break;
		case 3://右
			nextX = X + 1 < NodeWidth ? X + 1 : X - 1;
			break;
		}

		//拆墙操作
		if (m_nodeWalked[nextX][nextY] == false) {
			//拆墙
			m_wallAlive[X + nextX + 1][Y + nextY + 1] = false;
			//下一个节点 要 设置已走过
			m_nodeWalked[nextX][nextY] = true;
			//从 下一个节点开始 随机拆墙
			destroyRandomWall(nextX, nextY);
		}
		direction++;
		if (direction >= 4)direction = 0;
	}
}

void WallFactory::generateWalls(btDiscreteDynamicsWorld * world) {
	//设置墙的世界坐标
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i)
		{
			if (m_wallAlive[i][j]) {
				auto wall = Wall::create();
				float x = 2.0f*j - WallHeight * 1.0f;
				float y = 2.5f;
				float z = i * 2.0f - WallWidth * 1.0f;
				//设置坐标
				wall->setPosition(x,y,z);
				//设置纹理
				wall->setSRV(mWallTexSRV);
				//设置顶点索引相关信息
				wall->setVertexOffset(mWallVertexOffset);
				wall->setIndexCount(mWallIndexCount);
				wall->setIndexOffset(mWallIndexOffset);
				//设置墙的transform属性
				m_motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x,y,z)));
				//获得墙的物理形状+属性信息
				btRigidBody::btRigidBodyConstructionInfo wallConstructionInfor(m_wallMass, m_motionState, m_wallShape,m_wallInertia);
				//初始化物理身体
				wall->initbtBody(wallConstructionInfor , world);

				walls.push_back(std::move(wall));
			}
		}

}

void WallFactory::btRelease(){
	if (m_wallShape) {delete m_wallShape; m_wallShape = nullptr;}
	if (m_motionState) { delete m_motionState; m_motionState = nullptr; }
}