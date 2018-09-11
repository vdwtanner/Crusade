#pragma once

#include "pch.h"
#include <Box2D/Box2D.h>

/*This singleton will contain a handle to the b2World as well as a number of predefined material types, ie. friction and the like*/

class Box2DSingleton{
public:
	enum objCategory{
		NONE = 0x0000,
		ENEMY = 0x0001,
		WORLDTILE = 0x0002,
		CHARACTER = 0x0004,
		BOSS = 0x0008,
		TRIGGER = 0x0010,
		PHYSICAL_TRIGGER = 0x0020,
		ALL = 0xFFFF
	};

	static Box2DSingleton* Instance();
	b2World* getWorld();
	void stepWorld(float dt);
	void setWorldSize(Vector2 size);
	//void schedualForRemoval(b2Body* rigidbody);
	~Box2DSingleton();
	enum COLLISION_CATEGORY{ NO_PLAYER_COLLIDE=1 };
	float getTimeStep();
	int getVelocityIterations();
	int getPositionIterations();

	void clearWorld();
	void initWorld();

	void pause();
	void resume();

private:
	class ContactListener;

	ContactListener *m_contactListener;
	Box2DSingleton();
	static Box2DSingleton* m_instance;
	b2World* m_world;
	Vector2 m_worldSize;
	b2Body* m_worldBoundingBox;
	float32	m_timeStep = 1.0f / 30.0f;
	int32 m_velocityIterations = 6;
	int32 m_positionIterations = 2;
	float m_timer;

	bool m_paused;

};