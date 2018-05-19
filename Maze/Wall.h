#pragma once
#include "Entity.h"

class Wall :
	public Entity
{
protected:

public:
	Wall();
	virtual ~Wall();
	static shared_ptr<Wall> create();
public:
	virtual void update(float dt)override;
};

