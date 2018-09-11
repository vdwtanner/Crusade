#pragma once

#include "pch.h"
#include "Ability.h"

class HolyFire : public Ability{
	enum Stage{
		PROJECTILE,
		FIRE
	};

public:
	HolyFire(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc);
	~HolyFire();

	// ALL ABILITY CHILDREN MUST IMPLEMENT THESE 5 FUNCTIONS

	// gets called when the ability manager creates a new instance of your ability
	HolyFire* clone();
	// gets called during the next update loop after the manager creates an ability
	// use TriggerVolume::initSensorBody(b2FixtureDef) in this.
	void initSensorBody();
	// gets called on every update, many times per second
	void step(float dt);
	// gets called when a gameobject enters the trigger volume
	// CAN BE CALLED MULTIPLE TIMES ON THE SAME GAMEOBJECT
	void onTrigger(GameObject* collider);
	// gets called immediately after initSensorBody
	// use this to set spell velocity and position
	void onCast();
	void BeginContact(b2Contact* contact, GameObject* other);

private:
	Stage m_stage;
	float m_burnTimer;
	float m_maxDuration;
	bool m_dying;//when duration decays to <=0 but we aren't touching a body while a projectile, go into this state and increase duration to just above 0 to avoid TriggerVolume from cleaning me up
	bool m_dead;//When true I will no longer increase my duration, I will actually die
	bool m_onTopOfTile;
};