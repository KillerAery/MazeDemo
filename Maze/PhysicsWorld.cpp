#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld():
	m_broadphase(nullptr),m_collisionConfiguration(nullptr),m_dispatcher(nullptr),
	m_dynamicsWorld(nullptr),m_solver(nullptr),m_groundbody(nullptr),m_groundShape(nullptr),
	m_groundMotionState(nullptr)
{
}


PhysicsWorld::~PhysicsWorld()
{
	btRelease();
}

void PhysicsWorld::init(){
	btVector3 worldAabbMin(-10000, -10000, -10000);
	btVector3 worldAabbMax(10000, 10000, 10000);
	int maxProxies = 1024;
	m_broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);
	// 设置好碰撞属性 和调度 
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	// 实际上的物理模拟器
	m_solver = new btSequentialImpulseConstraintSolver();
	//世界
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver,m_collisionConfiguration);
	//世界重力
	m_dynamicsWorld->setGravity(btVector3(0, -1, 0));
	//创建地面
	createGround();
}

void PhysicsWorld::stepWorld(float dt) {
		m_dynamicsWorld->stepSimulation(dt,10);
		//更新物理世界每一个物体	
		auto & objectArray = m_dynamicsWorld->getCollisionObjectArray();
		for(int i =0;i<objectArray.size();++i)
		{
			//不存在用户指针或者睡眠中，则不处理
			if (!objectArray[i]->isActive() || objectArray[i]->isStaticObject())continue;
			Transform* object= reinterpret_cast<Transform*>(objectArray[i]->getUserPointer());
			if (!object)continue;

			//更新目标物体的位置
			const auto & pos = objectArray[i]->getWorldTransform().getOrigin();
			object->setPosition(pos.x(), pos.y(), pos.z());
			//更新目标物体的旋转角度
			const auto & rotationM = objectArray[i]->getWorldTransform().getRotation();
			object->setTransform(XMMatrixRotationQuaternion(XMVectorSet(rotationM.getX(), rotationM.getY(), rotationM.getZ(), rotationM.getW())));
		}
}

void PhysicsWorld::createGround() {
	//地面刚体 设置
	m_groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	m_groundMotionState = new  btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,m_groundMotionState, m_groundShape, btVector3(0, 0, 0));
	m_groundbody = new btRigidBody(groundRigidBodyCI);
	//将地面刚体添加到 物理世界
	m_dynamicsWorld->addRigidBody(m_groundbody);
}

void PhysicsWorld::btRelease() {
	if (m_solver)delete m_solver;
	if (m_dynamicsWorld)delete m_dynamicsWorld;
	if (m_dispatcher)delete m_dispatcher;
	if (m_collisionConfiguration)delete m_collisionConfiguration;
	if (m_broadphase)delete m_broadphase;
	if (m_groundShape)delete m_groundShape;
}