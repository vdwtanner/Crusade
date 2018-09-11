#pragma once

#include "pch.h"
#include "Box2DSingleton.h"

Box2DSingleton* Box2DSingleton::m_instance = NULL;

class Box2DSingleton::ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact){
		GameObject* objA = nullptr;
		GameObject* objB = nullptr;
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA)
			objA = static_cast<GameObject*>(bodyUserDataA);
		if (bodyUserDataB)
			objB = static_cast<GameObject*>(bodyUserDataB);

		if (objA && bodyUserDataA)		
			objA->BeginContact(contact, objB);
		if (objB && bodyUserDataB)
			objB->BeginContact(contact, objA);
	}

	void EndContact(b2Contact* contact){
		GameObject* objA = nullptr;
		GameObject* objB = nullptr;
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA)
			objA = static_cast<GameObject*>(bodyUserDataA);
		if (bodyUserDataB)
			objB = static_cast<GameObject*>(bodyUserDataB);

		if (objA && bodyUserDataA)
			objA->EndContact(contact, objB);
		if (objB && bodyUserDataB)
			objB->EndContact(contact, objA);
	}

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
		GameObject* objA = nullptr;
		GameObject* objB = nullptr;
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA)
			objA = static_cast<GameObject*>(bodyUserDataA);
		if (bodyUserDataB)
			objB = static_cast<GameObject*>(bodyUserDataB);

		if (objA && bodyUserDataA)
			objA->PreSolve(contact, objB, oldManifold);
		if (objB && bodyUserDataB)
			objB->PreSolve(contact, objA, oldManifold);
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){
		GameObject* objA = nullptr;
		GameObject* objB = nullptr;
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA)
			objA = static_cast<GameObject*>(bodyUserDataA);
		if (bodyUserDataB)
			objB = static_cast<GameObject*>(bodyUserDataB);

		if (objA && bodyUserDataA)
			objA->PostSolve(contact, objB, impulse);
		if (objB && bodyUserDataB)
			objB->PostSolve(contact, objA, impulse);
	}
	
};

//Get a pointer to the instance of the Box2D world
Box2DSingleton* Box2DSingleton::Instance(){
	if (!m_instance){
		m_instance =new Box2DSingleton();
	}
	return m_instance;
}

//Initialize the world
Box2DSingleton::Box2DSingleton(){
	b2Vec2 gravity(0.0f, -10.0f);
	//b2World m_world(gravity);
	m_world = new b2World(gravity);
	m_timer = 0;
	m_contactListener = new ContactListener();
	m_world->SetContactListener(m_contactListener);
	m_paused = false;
}

void Box2DSingleton::stepWorld(float dt){
	if (!m_paused){
		m_world->Step(m_timeStep, m_velocityIterations, m_positionIterations);
		//m_world->ClearForces();
	}
}

//If this method is called all pointers to b2 objects need to be set to null or else they will be pointing to invalid locations
Box2DSingleton::~Box2DSingleton(){
	delete m_contactListener;
	delete m_world;
}

void Box2DSingleton::setWorldSize(Vector2 size){
	m_worldSize = size;
}

b2World* Box2DSingleton::getWorld(){
	return m_world;
}

int Box2DSingleton::getPositionIterations(){
	return m_positionIterations;
}

float Box2DSingleton::getTimeStep(){
	return m_timeStep;
}

int Box2DSingleton::getVelocityIterations(){
	return m_velocityIterations;
}

void Box2DSingleton::clearWorld(){
	delete m_contactListener;
	m_contactListener = nullptr;
	delete m_world;
	m_world = nullptr;
}

void Box2DSingleton::initWorld(){
	b2Vec2 gravity(0.0f, -10.0f);
	//b2World m_world(gravity);
	m_world = new b2World(gravity);
	m_timer = 0;
	m_contactListener = new ContactListener();
	m_world->SetContactListener(m_contactListener);
}

void Box2DSingleton::pause(){
	m_paused = true;
}

void Box2DSingleton::resume(){
	m_paused = false;
}