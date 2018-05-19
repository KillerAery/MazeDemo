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
	//��ʼ��ǽ��������״
	m_wallShape = new btBoxShape(btVector3(1.0f,2.5f,1.0f));
	//0Ϊ��̬���壬����Ϊ��̬����
	m_wallMass = 0;
	m_wallInertia = btVector3(0, 0, 0);
	m_wallShape->calculateLocalInertia(m_wallMass, m_wallInertia);
	m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
}

void WallFactory::generateMazeWalls(ID3D11Device* d3dDevice, btDiscreteDynamicsWorld * world, int WallVertexOffset, UINT WallIndexOffset, UINT WallIndexCount) {
	//����ǽ����Ϣ
	reset();
	//�������������Ϣ
	mWallIndexCount = WallIndexCount;
	mWallIndexOffset = WallIndexOffset;
	mWallVertexOffset = WallVertexOffset;
	//����������Դ
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/bricks.dds", 0, 0, &mWallTexSRV, 0));
	//��ʼ�����ǽ
	destroyRandomWall(0, 0);
	//������ʣǽ ���� ��Ϸ����
	generateWalls(world);
}

void WallFactory::reset(){
	//��������ǽ
	walls.clear();
	//����ǽ�Ĵ�����
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i) {
			//�ڵ�����λ�ò�����ǽ
			if (j % 2 == 1 && i % 2 == 1)
				m_wallAlive[i][j] = false;
			//����ط�һ��ʼ����ǽ
			else
				m_wallAlive[i][j] = true;
		}
	//���� ���нڵ� �� �Ƿ񾭹�
	for (int j = 0; j < NodeHeight; ++j)
		for (int i = 0; i < NodeWidth; ++i)
			m_nodeWalked[i][j] = false;
}


void WallFactory::destroyRandomWall(int X, int Y) {
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
			m_wallAlive[X + nextX + 1][Y + nextY + 1] = false;
			//��һ���ڵ� Ҫ �������߹�
			m_nodeWalked[nextX][nextY] = true;
			//�� ��һ���ڵ㿪ʼ �����ǽ
			destroyRandomWall(nextX, nextY);
		}
		direction++;
		if (direction >= 4)direction = 0;
	}
}

void WallFactory::generateWalls(btDiscreteDynamicsWorld * world) {
	//����ǽ����������
	for (int j = 0; j < WallHeight; ++j)
		for (int i = 0; i < WallWidth; ++i)
		{
			if (m_wallAlive[i][j]) {
				auto wall = Wall::create();
				float x = 2.0f*j - WallHeight * 1.0f;
				float y = 2.5f;
				float z = i * 2.0f - WallWidth * 1.0f;
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
				wall->initbtBody(wallConstructionInfor , world);

				walls.push_back(std::move(wall));
			}
		}

}

void WallFactory::btRelease(){
	if (m_wallShape) {delete m_wallShape; m_wallShape = nullptr;}
	if (m_motionState) { delete m_motionState; m_motionState = nullptr; }
}