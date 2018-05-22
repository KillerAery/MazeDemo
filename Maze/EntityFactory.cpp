#include "EntityFactory.h"

using namespace std;
using namespace WallMaze;

EntityFactory::EntityFactory() :mWallTexSRV(nullptr),m_wallShape(nullptr),m_motionState(nullptr), m_skullShape(nullptr)
{
}

EntityFactory::~EntityFactory()
{
	SafeDelete(m_wallShape);
	SafeDelete(m_skullShape);
	SafeDelete(m_motionState);
	ReleaseCOM(mWallTexSRV);
}

void EntityFactory::init(int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount,int SkullVertexOffset, UINT SkullIndexOffset, UINT SkullIndexCount) {
	//顶点索引相关信息
	mWallIndexCount = WallIndexCount;
	mWallIndexOffset = WallIndexOffset;
	mWallVertexOffset = WallVertexOffset;

	mSkullIndexCount = SkullIndexCount;
	mSkullVertexOffset = SkullVertexOffset;
	mSkullIndexOffset = SkullIndexOffset;

	//初始化transform信息
	m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	//初始化墙的物理形状
	m_wallShape = new btBoxShape(btVector3(1.5f,3.0f,1.5f));	
	m_wallMass = 0;//0为静态刚体，其余为动态刚体
	m_wallInertia = btVector3(0, 0, 0);
	m_wallShape->calculateLocalInertia(m_wallMass, m_wallInertia);

	//初始化骷髅头的物理形状
	m_skullShape = new btSphereShape(0.3f);
	m_skullMass = 1;
	m_skullInertia = btVector3(0, 0, 0);
	m_skullShape->calculateLocalInertia(m_skullMass, m_skullInertia);
}

void EntityFactory::createMazeEntities(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world){
	//重置墙体信息
	reset();

	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/bricks.dds", 0, 0, &mWallTexSRV, 0));

	//开始随机拆墙
	destroyRandomWall(0, 0);

	//拆掉终点墙
	wallAlive[WallWidth - 1][WallHeight - 1] = false;
	wallAlive[WallWidth - 2][WallHeight - 1] = false;
	wallAlive[WallWidth - 1][WallHeight - 2] = false;

	//根据所剩墙 生成 游戏对象
	createWalls(world);
	createSkulls(world);
}

bool EntityFactory::posInEnd(CXMVECTOR _position) {
	//终点位置
	float x = 3.0f*(WallWidth - 1) - WallHeight * 1.5f;
	float z = 3.0f*(WallHeight -1) - WallHeight * 1.5f;
	//目标位置
	XMFLOAT3 position;
	XMStoreFloat3(&position, _position);
	return (position.x < (x + 1.5f) && position.x > (x - 1.5f) && position.z < (z + 1.5f) && position.z > (z - 1.5f));
}

void EntityFactory::reset(){
	//清理所有墙
	walls.clear();
	//设置墙的存在性
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i) {
			//节点所在位置不存在墙
			if (j % 2 == 1 && i % 2 == 1)
				wallAlive[i][j] = false;
			//其余地方一开始都有墙
			else
				wallAlive[i][j] = true;
		}
	//设置 所有节点 的 是否经过
	for (int j = 0; j < NodeHeight; ++j)
		for (int i = 0; i < NodeWidth; ++i)
			m_nodeWalked[i][j] = false;
}


void EntityFactory::destroyRandomWall(int X, int Y) {
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
			wallAlive[X + nextX + 1][Y + nextY + 1] = false;
			//下一个节点 要 设置已走过
			m_nodeWalked[nextX][nextY] = true;
			//从 下一个节点开始 随机拆墙
			destroyRandomWall(nextX, nextY);
		}
		direction++;
		if (direction >= 4)direction = 0;
	}
}

void EntityFactory::createWalls(btDiscreteDynamicsWorld * world) {
	//设置墙的世界坐标
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i)
		{
			if (wallAlive[i][j]) {
				auto wall = Wall::create();
				float x = 3.0f*j - WallHeight * 1.5f;
				float y = 3.0f;
				float z = 3.0f*i  - WallWidth * 1.5f;
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
				wall->initbtBody(wallConstructionInfor,world);

				walls.push_back(std::move(wall));
			}
		}
}

void EntityFactory::createSkulls(btDiscreteDynamicsWorld * world) {
	//设置墙的世界坐标
	for (int iy = 0; iy < WallHeight; ++iy)
		for (int ix = 0; ix < WallWidth; ++ix)
		{
			//有空位才生成骷髅头 1/4的空位
			if (!wallAlive[ix][iy] && rand()%4 == 0) {
				//一个空位要生成的骷髅头数
				//随机0~2个
				int skullNumInOneChunk = rand()%3;
				for (int i = 0; i < skullNumInOneChunk; ++i) {
					auto skull = Skull::create();
					float x = 3.0f*iy - WallHeight * 1.5f + rand()%10 * 0.3f - 1.5f;
					float y = 0.5f;
					float z = 3.0f*ix - WallWidth * 1.5f + rand()%10 * 0.3f - 1.5f;
					//设置坐标
					skull->setPosition(x, y, z);
					//设置纹理
					skull->setSRV(0);
					//设置顶点索引相关信息
					skull->setVertexOffset(mSkullVertexOffset);
					skull->setIndexCount(mSkullIndexCount);
					skull->setIndexOffset(mSkullIndexOffset);
					//设置墙的transform属性
					m_motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, z)));
					//获得墙的物理形状+属性信息
					btRigidBody::btRigidBodyConstructionInfo skullConstruction(m_skullMass, m_motionState, m_skullShape, m_skullInertia);
					//初始化物理身体
					skull->initbtBody(skullConstruction, world);
					skulls.push_back(std::move(skull));

				}
			}
		}
}
