#pragma once
#include "Entity.h"
#include "Camera.h"
#include "EntityFactory.h"

class Player :
	public Entity
{
public:
	enum class ButtonDown{
		W_Down,A_Down,S_Down,D_Down
	};
protected:
	//控制摄像机
	Camera * r_Camera;
	//玩家的物理形状
	btCollisionShape* m_playerShape;
	//玩家的Transform
	btMotionState* m_motionState;
	//按下前后左右的状态
	int m_WASD[4];
	bool m_jumpable;
public:
	Player();
	virtual ~Player();
	static shared_ptr<Player> create(btDiscreteDynamicsWorld * world,Camera* camera);
public:
	virtual void update(float dt)override;
	virtual void draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex)override;
	void walk(ButtonDown button);
	void stopWalk();
	void jump();
};

