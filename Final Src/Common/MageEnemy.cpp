#include "pch.h"
#include "MageEnemy.h"



MageEnemy::MageEnemy(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, float rotation, int id, int hp, float speed, float sight, float hearing, float massDensity, float attackMultiplier, float strength, float jumpStrength, int courage, int exp, std::string type, UINT aiFlags, std::string abilities, SoundDescription sounds)
	: Enemy(tag, name, position, textureSize, hitBoxSize, hitBoxOffset, rotation, id, hp, speed, sight, hearing, massDensity, attackMultiplier, strength, jumpStrength, courage, exp, type, aiFlags, abilities, sounds){
	this->m_state = AI_IDLE;
	m_currAnim = ANIM_IDLE;
	setAiFlags(aiFlags);
}

MageEnemy* MageEnemy::clone(){
	MageEnemy* clone = new MageEnemy(m_tag, m_name, getPosition(), m_textureSize, m_hitBoxSize, m_hitBoxOffset, m_rotation, m_id, m_hp, m_speed, m_sight, m_hearing, m_massDensity, m_attackMultiplier, m_strength, m_jumpStrength, m_courage, m_exp, m_type, m_aiFlags, m_abilityCsv, m_sounds);
	b2BodyDef bodyDef;
	bodyDef.position.Set(m_hitBoxSize.x / 2, m_hitBoxSize.y / 2);
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	clone->initRigidbody(bodyDef);
	return clone;
}

void MageEnemy::step(float dt){
	//Do anything that needs to happen every game loop
	if (m_moveTo)
	{
		float xdisPerSec = (m_moveToPos.x - getPosition().x) / m_moveToTime;
		float ydisPerSec = (m_moveToPos.y - getPosition().y) / m_moveToTime;

		walk(xdisPerSec, ydisPerSec);
		if (xdisPerSec > 0.01f){
			setFacingRight(true);
		}
		else
		{
			setFacingRight(false);
		}
		if (ydisPerSec > 0.1f){
			jump();
		}

		m_moveToTime -= dt;
		if (m_moveToTime <= 0){
			m_moveTo = false;
			m_rigidbody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}

	}
	else
	{
		stepAI(dt);
		aiStateReduction(dt);
		updateCooldownTimers(dt);
	}

	stepAnimation(dt);
}

void MageEnemy::stepAI(float dt){
	switch (m_state){
	case AI_IDLE:
		if (rand() % 200 == 1){
			fidget();
		}
		if (m_hearPlayer){
			m_actionTimers[0] += dt;
		}
		if (m_playerInSight){
			m_actionTimers[0] += dt*3;
		}
		if (m_actionTimers[0] >= .5){
			setAiState(AI_AWARE);
		}
		
		break;
	case AI_AWARE:
		if (rand() % 100 == 1 && !m_hearPlayer){
			fidget();
		}
		if (m_hearPlayer){
			m_actionTimers[1] += dt;
		}
		if (m_playerInSight){
			m_actionTimers[1] += dt*3;
		}
		if (m_playerInSight && m_actionTimers[1] >= .25){
			setAiState(AI_ENGAGE);
		}
		if (m_hearPlayer && fmod(m_actionTimers[1] / dt, 30) <= 4){ //every half second that we hear the player we can try to face th player. <= 4 so that we don't miss a check due to player being in sight
			facePlayer();
		}if (m_hearPlayer && m_actionTimers[1] >= .75){
			setAiState(AI_ALERT);
		}
		break;
	case AI_ALERT:
		if (m_playerInSight){
			m_actionTimers[2] += dt;
			if (m_actionTimers[2] >= .15){
				setAiState(AI_ENGAGE);
			}
		}
		if (m_hearPlayer && fmod(m_actionTimers[2] / dt, 15) <= .5){
			facePlayer();
		}
	case AI_ENGAGE:
		m_actionTimers[3] += dt;
		if (m_actionTimers[3] >= m_fastestAbilityCooldown){
			m_actionTimers[3] -= m_fastestAbilityCooldown;
			attack();
		}
		if (m_hearPlayer){
			facePlayer();
		}
		walk(dt);
		handleObstacles();
		break;
	case AI_FLEE://GTFO
		GTFO(dt);
		break;
	case AI_DEATH:
		break;
	}

}