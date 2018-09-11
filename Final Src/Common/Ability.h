#pragma once

#include "pch.h"
#include <filesystem>
#include "Box2DSingleton.h"
#include "TriggerVolume.h"
#include "ParticleManager.h"

class Ability : public TriggerVolume{
public:
	struct AbilityDescription{
		Vector2 texSize;
		Vector2 hitboxSize;
		Vector2 hitboxOffset;
		float animSpeed;
		bool animLoop;
		int damage;
		float duration;
		float range;
		float speed;
		float knockback;
		int mana;
		float cooldown;
		bool scaleFromCenter;
		string castAnim;
		string description;
		string iconPath;
		string name;
	};

	Ability(UINT id, std::string tag, std::string name, AbilityDescription* desc);
	~Ability();

	// ALL ABILITY CHILDREN MUST IMPLEMENT THESE 5 FUNCTIONS

	// gets called when the ability manager creates a new instance of your ability
	virtual Ability* clone() = 0;
	// gets called during the next update loop after the manager creates an ability
	// use TriggerVolume::initSensorBody(b2FixtureDef) in this.
	virtual void initSensorBody() = 0;
	// gets called on every update, many times per second
	virtual void step(float dt) = 0;
	// gets called when a gameobject enters the trigger volume
	// CAN BE CALLED MULTIPLE TIMES ON THE SAME GAMEOBJECT
	virtual void onTrigger(GameObject* collider) = 0;
	// This may be overrided when a child inherits,
	// however often it should not be. Default just damages
	// tiles in front of air tiles without killing the volume
	virtual void onAirTrigger(WorldTile* air);
	// gets called immediately after initSensorBody
	// use this to set spell velocity and position
	virtual void onCast() = 0;


	// so each ability has an owner, although it doesnt have to.
	// the owner can be nullptr.
	GameObject* getOwner() const;
	virtual int getManaCost(GameObject* caster);
	string getDescription();
	string getIconPath();
	AbilityDescription getAbilityDesc();

	void setOwner(GameObject*);
	void setOwnerAfterSpawn(GameObject* newOwner);
	void ownerHasDied();

	float getCooldown();

	void stepAnimation(float dt);
	bool initialized();
	bool rangeCheck();
	void fireInDirection(float vx, float vy);
	UINT getTexIndex() const;
	int getStagesRemaining();

	void moveTo(float x, float y, float time);

	void reCalcOwnerPosition();

protected:
	Vector2 m_texSize;
	int m_damage;
	float m_range;
	float m_speed;
	float m_knockback;
	int m_mana;
	float m_cooldown;
	bool m_scaleFromCenter;
	float m_animSpeed;
	bool m_animLoop;
	Vector2 m_startPosition;
	UINT m_texIndex;
	int m_stagesRemaining;
	string m_castAnim;
	string m_iconPath;
	//Not to be confused with AbilityDescription, which is a struct. This is a description of what the ability does and its flavor text.
	string m_description;

	

	GameObject* m_owner;
	GameObject* m_newOwner;

	// copies of owner data for initialization, since owner could die before Ability::onCast()
	float m_ownerBottom;
	float m_ownerTop;
	float m_ownerLeft;
	float m_ownerRight;
	bool m_ownerFacingRight;
	int m_passive;//if 0, no passive chosen; if 1, first passive chosen; if 2, second passive chosen

	bool m_moveTo;
	Vector2 m_moveToPos;
	float m_moveToTime;
};