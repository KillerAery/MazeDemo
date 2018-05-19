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
	// ���ú���ײ���� �͵��� 
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	// ʵ���ϵ�����ģ����
	m_solver = new btSequentialImpulseConstraintSolver();
	//����
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver,m_collisionConfiguration);
	//��������
	m_dynamicsWorld->setGravity(btVector3(0, -1, 0));
	//��������
	createGround();
}

void PhysicsWorld::stepWorld(float dt) {
		m_dynamicsWorld->stepSimulation(dt,10);
		//������������ÿһ������	
		auto & objectArray = m_dynamicsWorld->getCollisionObjectArray();
		for(int i =0;i<objectArray.size();++i)
		{
			//�������û�ָ�����˯���У��򲻴���
			if (!objectArray[i]->isActive() || objectArray[i]->isStaticObject())continue;
			Transform* object= reinterpret_cast<Transform*>(objectArray[i]->getUserPointer());
			if (!object)continue;

			//����Ŀ�������λ��
			const auto & pos = objectArray[i]->getWorldTransform().getOrigin();
			object->setPosition(pos.x(), pos.y(), pos.z());
			//����Ŀ���������ת�Ƕ�
			const auto & rotationM = objectArray[i]->getWorldTransform().getRotation();
			object->setTransform(XMMatrixRotationQuaternion(XMVectorSet(rotationM.getX(), rotationM.getY(), rotationM.getZ(), rotationM.getW())));
		}
}

void PhysicsWorld::createGround() {
	//������� ����
	m_groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	m_groundMotionState = new  btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,m_groundMotionState, m_groundShape, btVector3(0, 0, 0));
	m_groundbody = new btRigidBody(groundRigidBodyCI);
	//�����������ӵ� ��������
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