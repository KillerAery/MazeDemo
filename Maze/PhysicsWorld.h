#pragma once
#include <btBulletDynamicsCommon.h>
#include "Entity.h"

class PhysicsWorld
{
	//物理模拟所需
	btAxisSweep3* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	//物理世界本体
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	//地面形状
	btCollisionShape* m_groundShape;
	//地面transform
	btDefaultMotionState* m_groundMotionState;
	//地面身体
	btRigidBody* m_groundbody;
public:
	PhysicsWorld();
	~PhysicsWorld();
	void init();
	//物理模拟步长
	void stepWorld(float dt);
	btDiscreteDynamicsWorld* getWorld() { return m_dynamicsWorld; }
protected:
	//创建地面
	void createGround();
	void btRelease();
};

