#pragma once

#include "pch.h"
#include "GameObject.h"
#include <filesystem>
#include "Box2DSingleton.h"
#include "RayCast.h"
#include "AbilityManager.h"
#include "AudioManager.h"
#include "Profile.h"
#include "Emitter.h"

class Character : public GameObject{
public:
	Character( Vector2 position, float rotation);
	Character(Vector2 position, float rotation, Profile* profile);
	~Character();
	int getId() const;
	int getExp() const;
	UINT getBodyTexIndex() const;
	UINT getArmTexIndex() const;
	int getMana();
	int getManaMax();
	void setId(int);
	void setExp(int);
	void addExp(int exp);
	int applyExp();
	void setPosition(Vector2 pos);
	Character* clone();
	void step(float dt) override;
	enum ANIMATION_TYPE{ ANIM_IDLE, ANIM_WALK, ANIM_JUMP, ANIM_DEATH, ANIM_CAST, ANIM_SLASH};
	bool operator<(Character const& e);
	void stepAnimation(float dt);
	b2Body* getRigidbody();
	void initRigidbody(b2BodyDef def);
	void Character::loadTextures(std::tr2::sys::path anim_path);
	void renderCharacter();
	void moveUp(float dt);
	void moveDown(float dt);
	void moveTo(float x, float y, float time);
	void walk(b2Vec2);
	void walk(float dt);
	void jump(float impulse);
	void jump();
	void halt();


	void interpolate(float alpha);
	void setFacingRight(bool direction);
	void enableFlight();
	void disableFlight();
	bool canFly();

	void setCooldown(int index, float duration);
	void setCooldownTimer(int index, float duration);
	void startCooldownTimer(int index);
	float getCooldown(int index);
	float getCooldownTimer(int index);
	void setEnableCollisionWith(int);

	int getActiveAbility(int index);
	void BeginContact(b2Contact* contact, GameObject* other);
	void EndContact(b2Contact* contact, GameObject* other);
	void initCooldownArrays();
	void initAbilities(UINT abilities[4], UINT passives[4]);
	int getPassiveForAbility(UINT id);
	int getPassive(int index);
	void setPassive(int index, int passive);
	void bindAbility(int index, UINT abilityID);

	UINT getAnimSize(UINT id, Character::ANIMATION_TYPE type);
	UINT getAnimIndex(UINT id, Character::ANIMATION_TYPE type);

	Ability* castAbility(UINT id);

	bool save();
	Profile* getProfile();

	void toggleGodMode();
	bool getGodMode();

	void damage(int damage);
	void heal(int health);

	bool isDead();

protected:
	int m_id;
	int m_mana;
	int m_manaMax;
	float m_speed;
	float m_strength;
	float m_massDensity;
	int m_exp;
	UINT m_bodyTexIndex;
	UINT m_armTexIndex;
	ANIMATION_TYPE m_bodyAnim;
	ANIMATION_TYPE m_armAnim;
	b2Body* m_rigidbody;

	bool m_deleteProfileOnDestruct;
	float m_manaTimer;
	bool m_godMode;
	bool m_dead;

private:
	Profile* m_profile;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texArray;
	std::tr2::sys::path m_relativePathAdjust;
	std::vector<std::pair<UINT, UINT>>		m_indexSizeVector;
	std::vector<wchar_t*> m_texPaths;
	void createBuffers();
	float m_cooldowns[4];
	float m_cooldownTimers[4];
	UINT m_activeAbilities[4];
	UINT m_passiveAbilities[4];
	void updateCooldownTimers(float dt);
	
	Vector2 m_spawnPoint;

	bool m_canFly;
	bool m_moveTo;
	Vector2 m_moveToPos;
	float m_moveToTime;
};