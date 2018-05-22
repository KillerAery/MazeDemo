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
	/*-----设置玩家物理属性----*/
	float x = 3.0f * (1)  - WallMaze::WallHeight * 1.5f;
	float y = 2.5f;
	float z = 3.0f * (1) - WallMaze::WallWidth * 1.5f;

	//设置坐标
	player->setPosition(x, y, z);
	//初始化玩家的物理形状
	player->m_playerShape = new btCylinderShape(btVector3(1.3f,2.5f, 1.3f));
	float playerMass = 1;
	btVector3 playerInertia = btVector3(0,0,0);
	player->m_playerShape->calculateLocalInertia(playerMass,playerInertia);
	player->m_motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x,y,z)));
	//获得墙的物理形状+属性信息
	btRigidBody::btRigidBodyConstructionInfo playerConstructionInfor(playerMass, player->m_motionState, player->m_playerShape,playerInertia);
	//初始化物理身体
	player->initbtBody(playerConstructionInfor, world);
	//摩擦
	player->m_body->setFriction(1.0f);
	//锁定旋转
	player->m_body->setAngularFactor(0);
	/*-----绑定相机给玩家----*/
	player->r_Camera = camera;

	return player;
}

void Player::update(float dt) {
	//设置玩家位移
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

	//用改变速度来驱动玩家行走
	m_body->setLinearVelocity(btVector3(direction.x, m_body->getLinearVelocity().y(), direction.z));

	//设置摄像机位置
	if (r_Camera)
		r_Camera->SetPosition(m_position);
}

void Player::draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex) {
	//暂时没有模型纹理
	//所以不渲染任何东西
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