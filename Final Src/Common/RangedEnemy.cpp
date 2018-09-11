#include "pch.h"
#include "RangedEnemy.h"

RangedEnemy::RangedEnemy(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, float rotation, int id, int hp, float speed, float sight, float hearing, float massDensity, float attackMultiplier, float strength, float jumpStrength, int courage, int exp, std::string type, UINT aiFlags, std::string abilities, SoundDescription sounds)
	: Enemy(tag, name, position, textureSize, hitBoxSize, hitBoxOffset, rotation, id, hp, speed, sight, hearing, massDensity, attackMultiplier, strength, jumpStrength, courage, exp, type, aiFlags, abilities, sounds){
	this->m_state = AI_IDLE;
	m_currAnim = ANIM_IDLE;
	setAiFlags(aiFlags);
}

RangedEnemy* RangedEnemy::clone(){
	return new RangedEnemy(m_tag, m_name, getPosition(), m_textureSize, m_hitBoxSize, m_hitBoxOffset, m_rotation, m_id, m_hp, m_speed, m_sight, m_hearing, m_massDensity, m_attackMultiplier, m_strength, m_jumpStrength, m_courage, m_exp, m_type, m_aiFlags, m_abilityCsv, m_sounds);
}

void RangedEnemy::step(float dt){
	//Do anything that needs to happen every game loop
	updateCooldownTimers(dt);

	if (m_moveTo)
	{
		float xdisPerSec = (m_moveToPos.x - getPosition().x) / m_moveToTime;
		float ydisPerSec = (m_moveToPos.y - getPosition().y) / m_moveToTime;

		if (xdisPerSec > 0.01f){
			setFacingRight(true);
		}
		else
		{
			setFacingRight(false);
		}
		walk(xdisPerSec, ydisPerSec);
		if (ydisPerSec > 0.1f){
			jump();
		}

		m_moveToTime -= dt;
		if (m_moveToTime <= 0){
			m_moveTo = false;
			m_rigidbody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}

	}
}

void RangedEnemy::stepAI(float dt){
	switch (m_state){
	case AI_IDLE:
		if (walkTimer - dtWalkTimer <= 0){
			if (hasAiFlag(ROAM)){
				switch (m_currAnim){
				case ANIM_IDLE:
					m_currAnim = ANIM_WALK;
					walkTimer = 2;
					break;
				case ANIM_WALK:
					m_currAnim = ANIM_IDLE;
					walkTimer = 5;
					break;
				}
			}
			dtWalkTimer = 0;
		}
		if (m_currAnim == ANIM_WALK){
			setPosition(Vector2(getPosition().x + m_speed*dt*(isFacingRight() ? 1 : -1), getPosition().y));
		}
		break;
	case AI_AWARE:
	case AI_ALERT:
	case AI_ENGAGE:
	case AI_FLEE:
	case AI_DEATH:
		break;
	}

}
