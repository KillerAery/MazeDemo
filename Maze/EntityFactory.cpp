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
	//�������������Ϣ
	mWallIndexCount = WallIndexCount;
	mWallIndexOffset = WallIndexOffset;
	mWallVertexOffset = WallVertexOffset;

	mSkullIndexCount = SkullIndexCount;
	mSkullVertexOffset = SkullVertexOffset;
	mSkullIndexOffset = SkullIndexOffset;

	//��ʼ��transform��Ϣ
	m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	//��ʼ��ǽ��������״
	m_wallShape = new btBoxShape(btVector3(1.5f,3.0f,1.5f));	
	m_wallMass = 0;//0Ϊ��̬���壬����Ϊ��̬����
	m_wallInertia = btVector3(0, 0, 0);
	m_wallShape->calculateLocalInertia(m_wallMass, m_wallInertia);

	//��ʼ������ͷ��������״
	m_skullShape = new btSphereShape(0.3f);
	m_skullMass = 1;
	m_skullInertia = btVector3(0, 0, 0);
	m_skullShape->calculateLocalInertia(m_skullMass, m_skullInertia);
}

void EntityFactory::createMazeEntities(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world){
	//����ǽ����Ϣ
	reset();

	//����������Դ
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/bricks.dds", 0, 0, &mWallTexSRV, 0));

	//��ʼ�����ǽ
	destroyRandomWall(0, 0);

	//����յ�ǽ
	wallAlive[WallWidth - 1][WallHeight - 1] = false;
	wallAlive[WallWidth - 2][WallHeight - 1] = false;
	wallAlive[WallWidth - 1][WallHeight - 2] = false;

	//������ʣǽ ���� ��Ϸ����
	createWalls(world);
	createSkulls(world);
}

bool EntityFactory::posInEnd(CXMVECTOR _position) {
	//�յ�λ��
	float x = 3.0f*(WallWidth - 1) - WallHeight * 1.5f;
	float z = 3.0f*(WallHeight -1) - WallHeight * 1.5f;
	//Ŀ��λ��
	XMFLOAT3 position;
	XMStoreFloat3(&position, _position);
	return (position.x < (x + 1.5f) && position.x > (x - 1.5f) && position.z < (z + 1.5f) && position.z > (z - 1.5f));
}

void EntityFactory::reset(){
	//��������ǽ
	walls.clear();
	//����ǽ�Ĵ�����
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i) {
			//�ڵ�����λ�ò�����ǽ
			if (j % 2 == 1 && i % 2 == 1)
				wallAlive[i][j] = false;
			//����ط�һ��ʼ����ǽ
			else
				wallAlive[i][j] = true;
		}
	//���� ���нڵ� �� �Ƿ񾭹�
	for (int j = 0; j < NodeHeight; ++j)
		for (int i = 0; i < NodeWidth; ++i)
			m_nodeWalked[i][j] = false;
}


void EntityFactory::destroyRandomWall(int X, int Y) {
	int direction = rand() % 4;
	//����4������
	//4������˳�� ���
	for (int i = 0; i < 4; ++i) {

		int nextX = X;
		int nextY = Y;

		switch (direction)
		{
		case 0://��
			nextY = Y - 1 >= 0 ? Y - 1 : Y + 1;
			break;
		case 1://��
			nextY = Y + 1 < NodeHeight ? Y + 1 : Y - 1;
			break;
		case 2://��
			nextX = X - 1 >= 0 ? X - 1 : X + 1;
			break;
		case 3://��
			nextX = X + 1 < NodeWidth ? X + 1 : X - 1;
			break;
		}

		//��ǽ����
		if (m_nodeWalked[nextX][nextY] == false) {
			//��ǽ
			wallAlive[X + nextX + 1][Y + nextY + 1] = false;
			//��һ���ڵ� Ҫ �������߹�
			m_nodeWalked[nextX][nextY] = true;
			//�� ��һ���ڵ㿪ʼ �����ǽ
			destroyRandomWall(nextX, nextY);
		}
		direction++;
		if (direction >= 4)direction = 0;
	}
}

void EntityFactory::createWalls(btDiscreteDynamicsWorld * world) {
	//����ǽ����������
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i)
		{
			if (wallAlive[i][j]) {
				auto wall = Wall::create();
				float x = 3.0f*j - WallHeight * 1.5f;
				float y = 3.0f;
				float z = 3.0f*i  - WallWidth * 1.5f;
				//��������
				wall->setPosition(x,y,z);
				//��������
				wall->setSRV(mWallTexSRV);
				//���ö������������Ϣ
				wall->setVertexOffset(mWallVertexOffset);
				wall->setIndexCount(mWallIndexCount);
				wall->setIndexOffset(mWallIndexOffset);
				//����ǽ��transform����
				m_motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x,y,z)));
				//���ǽ��������״+������Ϣ
				btRigidBody::btRigidBodyConstructionInfo wallConstructionInfor(m_wallMass, m_motionState, m_wallShape,m_wallInertia);
				//��ʼ����������
				wall->initbtBody(wallConstructionInfor,world);

				walls.push_back(std::move(wall));
			}
		}
}

void EntityFactory::createSkulls(btDiscreteDynamicsWorld * world) {
	//����ǽ����������
	for (int iy = 0; iy < WallHeight; ++iy)
		for (int ix = 0; ix < WallWidth; ++ix)
		{
			//�п�λ����������ͷ 1/4�Ŀ�λ
			if (!wallAlive[ix][iy] && rand()%4 == 0) {
				//һ����λҪ���ɵ�����ͷ��
				//���0~2��
				int skullNumInOneChunk = rand()%3;
				for (int i = 0; i < skullNumInOneChunk; ++i) {
					auto skull = Skull::create();
					float x = 3.0f*iy - WallHeight * 1.5f + rand()%10 * 0.3f - 1.5f;
					float y = 0.5f;
					float z = 3.0f*ix - WallWidth * 1.5f + rand()%10 * 0.3f - 1.5f;
					//��������
					skull->setPosition(x, y, z);
					//��������
					skull->setSRV(0);
					//���ö������������Ϣ
					skull->setVertexOffset(mSkullVertexOffset);
					skull->setIndexCount(mSkullIndexCount);
					skull->setIndexOffset(mSkullIndexOffset);
					//����ǽ��transform����
					m_motionState->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, z)));
					//���ǽ��������״+������Ϣ
					btRigidBody::btRigidBodyConstructionInfo skullConstruction(m_skullMass, m_motionState, m_skullShape, m_skullInertia);
					//��ʼ����������
					skull->initbtBody(skullConstruction, world);
					skulls.push_back(std::move(skull));

				}
			}
		}
}
