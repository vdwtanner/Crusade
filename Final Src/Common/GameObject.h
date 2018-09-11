#pragma once
#include "pch.h"
#include <string>
#include <map>
#include "SimpleMath.h"
#include "CrusadeUtil.h"

class Ability;

class GameObject{
public:
	enum Status{
		BURN = 0x0001,
		POISON = 0x0002,
		SLOW = 0x0004,
		STUN = 0x0008
	};
	struct StatusDesc{
		float effectInfo;	// this var holds information about how powerful this status effect is. For example,
							// effectInfo determines BURN's damage, SLOW's speed reduction, or POISON's damage
							// effectInfo may be unused for statuses such as stun
		float remainingTime;
	};

	GameObject(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, int hp, float rotation, bool facingRight);
	~GameObject();

	std::string getTag();
	std::string getName();
	Vector2 getPosition();
	Vector2 getCenterPosition();
	Vector2 getRenderPosition();
	Vector2 getTrackingPosition();
	Vector2 getSize();
	Vector2 getHitBoxOffset();
	Vector2 getRenderSize();
	Vector4 getColor();
	float getRotation();
	int getHp();
	int getHpMax();

	void setTag(std::string);
	void setName(std::string);
	void setPosition(Vector2);
	void setSize(Vector2);	
	void setHitBoxOffset(Vector2);
	void setColor(Vector4);
	void setHp(int);
	void setHpMax(int);

	virtual void damage(int);
	virtual void heal(int);

	virtual GameObject* clone() = 0;

	virtual void step(float dt) = 0;

	bool isFacingRight();
	virtual void setFacingRight(bool direction);
	Vector2 getHitboxSize();
	void applyStatusAilment(Status name, StatusDesc desc);
	void cureStatusAilment(Status name);

	virtual Ability* castAbility(UINT id);
	bool removeAbilityFromActive(Ability*);
	void addAbilityToActive(Ability* ability);

	virtual void BeginContact(b2Contact* contact, GameObject* other) = 0;
	virtual void EndContact(b2Contact* contact, GameObject* other) = 0;
	virtual void PreSolve(b2Contact* contact, GameObject* other, const b2Manifold*);
	virtual void PostSolve(b2Contact* contact, GameObject* other, const b2ContactImpulse*);

protected:
	GameObject();

	// So i dont have to make an animation class
	// call this before getting getAnimationFrameIndex
	bool isAnimationOver(const float dt, const UINT size, const float frameTime);
	UINT getAnimationFrameIndex(const float dt, const UINT start, const UINT size, const float frameTime);
	void resetAnimation();

	std::map < Status, StatusDesc > m_statusMap;
	std::vector<Ability*> m_activeAbilities;
	std::string m_tag;
	std::string m_name;
	
	Vector2 m_textureSize;
	Vector2 m_hitBoxSize;
	Vector2 m_hitBoxOffset;
	Vector4 m_color;
	float m_rotation;
	float m_timer;
	int m_hp;
	int m_hpmax;

	bool facingRight;
	bool m_prevFacingRight;
	//void setTag(std::string tag);
	//void setName(std::string name);
private:
	Vector2 m_position;
};