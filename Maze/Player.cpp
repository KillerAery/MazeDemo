#include "Player.h"

Player::Player():m_playerShape(nullptr),m_motionState(nullptr), r_Camera(nullptr),m_jumpable(true),m_WASD{0,0,0,0}
{
}


Player::~Player()
{
	SafeDelete(m_motionState);
	SafeDelete(m_playerShape);
}

shared_ptr<Player> Player::create(btDiscreteDynamicsWorld * world, Camera* camera) {
	shared_ptr<Player> player = std::make_shared<Player>();
	/*-----���������������----*/
	float x = 3.0f * (1)  - WallMaze::WallHeight * 1.5f;
	float y = 2.5f;
	float z = 3.0f * (1) - WallMaze::WallWidth * 1.5f;

	//��������
	player->setPosition(x, y, z);
	//��ʼ����ҵ�������״
	player->m_playerShape = new btCylinderShape(btVector3(1.3f,2.5f, 1.3f));
	float playerMass = 1;
	btVector3 playerInertia = btVector3(0,0,0);
	player->m_playerShape->calculateLocalInertia(playerMass,playerInertia);
	player->m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x,y,z)));
	//���ǽ��������״+������Ϣ
	btRigidBody::btRigidBodyConstructionInfo playerConstructionInfor(playerMass, player->m_motionState, player->m_playerShape,playerInertia);
	//��ʼ����������
	player->initbtBody(playerConstructionInfor, world);
	//Ħ��
	player->m_body->setFriction(1.0f);
	//������ת
	player->m_body->setAngularFactor(0);
	/*-----����������----*/
	player->r_Camera = camera;

	return player;
}

void Player::update(float dt) {
	//�������λ��
	m_body->activate(true);

	XMFLOAT3 direction;

	XMStoreFloat3(&direction,
		XMVectorScale(XMVector3Normalize(
				XMVectorScale(XMLoadFloat3(&r_Camera->GetLook()), (m_WASD[0] - m_WASD[2])) +
				XMVectorScale(XMLoadFloat3(&r_Camera->GetRight()), (m_WASD[3] - m_WASD[1])))
			,6.0f)
	);


	for (int i = 0; i < 4; ++i)
		m_WASD[i] = 0;

	//�øı��ٶ��������������
	m_body->setLinearVelocity(btVector3(direction.x, m_body->getLinearVelocity().y(), direction.z));

	//���������λ��
	if (r_Camera)
		r_Camera->SetPosition(m_position);
}

void Player::draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex) {
	//��ʱû��ģ������
	//���Բ���Ⱦ�κζ���
}

void Player::walk(ButtonDown down){
	switch (down)
	{
	case Player::ButtonDown::W_Down:
		m_WASD[0] = 1;
		break;
	case Player::ButtonDown::A_Down:
		m_WASD[1] = 1;
		break;
	case Player::ButtonDown::S_Down:
		m_WASD[2] = 1;
		break;
	case Player::ButtonDown::D_Down:
		m_WASD[3] = 1;
		break;
	default:
		break;
	}
}



void Player::stopWalk(){
	m_body->setLinearFactor(btVector3(0,m_body->getLinearVelocity().y(),0));
}

void Player::jump() {
	if (m_jumpable) {
		m_body->activate(true);
		m_body->applyForce(btVector3(0,-1000,0), btVector3(m_position.x, m_position.y, m_position.z));
	//	m_jumpable = false;
	}
}