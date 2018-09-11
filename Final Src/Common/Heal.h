#pragma once

#include "pch.h"
#include "Ability.h"

class Heal : public Ability{
	enum Stage{
		BURST
	};

public:
	Heal(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc);
	~Heal();

	// ALL ABILITY CHILDREN MUST IMPLEMENT THESE 5 FUNCTIONS


	// gets called when the ability manager creates a new instance of your ability
	Heal* clone();
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

	int getManaCost(GameObject* caster);

private:
	Stage m_stage;

	float m_timer;
	int m_healPerBurst;
	int m_leftovers;

	float m_manaPercentage;
	float m_healPercentage;
};