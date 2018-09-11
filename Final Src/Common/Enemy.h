#pragma once

#include "pch.h"
#include "GameObject.h"
#include <filesystem>
#include "Box2DSingleton.h"
#include "RayCast.h"
#include "WorldTileMatrix.h"
#include "LuaTrigger.h"
#include "Emitter.h"

class Enemy : public GameObject{
	friend class EnemyManager;
public:
	struct SoundDescription{
		map<float, string> engageSounds;
		map<float, string> hitSounds;
		map<float, string> tauntSounds;
		map<float, string> deathSounds;
		map<float, string> fleeSounds;
	};


	static UINT parseAIFlags(string flagsCSV);
	Enemy(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, float rotation, int id, int hp, float speed, float sight, float hearing, float massDensity, float attackMultiplier, float strength, float jumpStrength, int courage, int exp, std::string type, UINT aiFlags, std::string abilities, SoundDescription sounds);
	~Enemy();
	int getId() const;
	int getExp() const;
	int awardExp();
	UINT getTexIndex() const;

	void setId(int);
	void setExp(int);
	void setPosition(Vector2 pos);

	static const std::string MELEE;
	static const std::string RANGED;
	static const std::string MAGE;
	std::string getType();

	virtual Enemy* clone()=0;

	virtual void step(float dt) = 0;
	void interpolate(float alpha);

	enum AI_STATE{ AI_IDLE, AI_AWARE, AI_ALERT, AI_ENGAGE, AI_FLEE, AI_DEATH };
	enum ANIMATION_TYPE{ ANIM_IDLE, ANIM_WALK, ANIM_JUMP, ANIM_DEATH, ANIM_CAST, ANIM_SLASH};
	enum AI_FLAGS{ROAM=1, QUICK_RESPONSE=2, TRIGGER_HAPPY=4, PEACEFUL=8};
	enum SENSOR_TYPE{BODY, EYES, EARS};

	void setAiFlags(UINT flags);
	UINT getAiFlags();
	bool hasAiFlag(UINT flag);
	void setAiState(AI_STATE state);
	AI_STATE getAiState();

	bool operator<(Enemy const& e);

	void stepAnimation(float dt);

	b2Body* getRigidbody();
	void initRigidbody(b2BodyDef def);

	void setEnableCollisionWith(int collideWith);

	void BeginContact(b2Contact* contact, GameObject* other);
	void EndContact(b2Contact* contact, GameObject* other);

	void setAbilityCSV(std::string abilities);
	int getPassiveForAbility(UINT ability);
	std::string getAbilities();

	void respondToAggression();
	void GTFO(float dt);

	void determineWallBreakAbility();

	void moveTo(float x, float y, float time);

	void setContainingTrigger(LuaTrigger *trig);

	Ability* castAbility(UINT id);

	void damage(int damage);

	void setFacingRight(bool direction);

protected:
	SoundDescription m_sounds;
	int m_id;
	float m_speed;
	float m_sight;
	float m_hearing;
	float m_attackMultiplier;
	float m_massDensity;
	float m_strength;
	float m_jumpStrength;
	float m_actionTimers[6];//one for each ai state
	float m_aiStateTimer[2];//[0] is the current time, [1] is the max time
	float m_abilityChance[4];
	float m_cooldowns[4];
	float m_cooldownTimers[4];
	float m_fastestAbilityCooldown;
	UINT m_activeAbilities[4];
	UINT m_passiveAbilities[4];
	int m_wallBreakIndex;//Index of Ability to be used for smashing walls
	int m_courage;
	int m_exp;
	bool jumping;
	bool m_dead;
	bool m_playerInSight;
	bool m_hearPlayer;
	UINT m_texIndex;
	UINT m_aiFlags;
	std::string m_type;
	std::string m_abilityCsv;
	AI_STATE m_state;
	ANIMATION_TYPE m_currAnim;
	b2Body* m_rigidbody;

	Emitter *m_blood;

	bool m_moveTo;
	Vector2 m_moveToPos;
	float m_moveToTime;

	LuaTrigger *m_containingTrigger;

	void walk(float, float);
	void walk(float dt);
	void walkBackwards(float dt);
	void run(float dt);
	void jump(float impulse);
	void jump();
	void halt();
	void handleObstacles();
	void fidget();
	void roam(float dt);
	float lookForPlayer();
	void aerialCorrection();
	b2Vec2 compareVelociy(b2Body* other);
	bool shouldIJump();
	int stepsToX(float x);
	Vector2 findJumpDestination(b2Body* target);
	Vector2 m_jDestination;//Desired destination for the end of the jump.
	Character* m_player;
	b2Body* m_jTarget;//jump target
	float calculateVerticalVelocityForHeight(float desiredHeight);
	virtual void stepAI(float dt) = 0;
	bool isDead();
	int checkHP();
	void aiStateReduction(float dt);
	void aiChangeState(AI_STATE state);
	bool parseAbilityCSV();
	map<float, string> parseSounds(string csv);
	void attack();
	void updateCooldownTimers(float dt);
	void facePlayer();
private:

};