#pragma once
#include <btBulletDynamicsCommon.h>
#include "Entity.h"

class PhysicsWorld
{
	//����ģ������
	btAxisSweep3* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	//�������籾��
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	//������״
	btCollisionShape* m_groundShape;
	//����transform
	btDefaultMotionState* m_groundMotionState;
	//��������
	btRigidBody* m_groundbody;
public:
	PhysicsWorld();
	~PhysicsWorld();
	void init();
	//����ģ�ⲽ��
	void stepWorld(float dt);
	btDiscreteDynamicsWorld* getWorld() { return m_dynamicsWorld; }
protected:
	//��������
	void createGround();
	void btRelease();
};

