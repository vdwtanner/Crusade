#include "pch.h"
#include "Heal.h"
#include "AbilityManager.h"
#include "AudioManager.h"

Heal::Heal(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc)
	: Ability(id, tag, name, desc)
{
	m_manaPercentage = fmin((float)m_mana / 100.0f, 1.0f);
	m_healPercentage = fmin((float)m_mana / 100.0f, 1.0f);
	//Heal is a single stage attack as of now, but if we add special things we can add them here
	std::vector<string> frags = CrusadeUtil::split(name, '_');
	if (frags.size() > 1){
		if (frags[1].compare("explosion") == 0){
			return;
		}
	}
	m_stage = BURST;
}

// gets called immediately after initSensorBody
void Heal::onCast(){
	// use this to set spell velocity and position
	float x;
	float y = 0;
	m_timer = 0;
	Vector2 pos;
	switch (m_stage){
	case BURST:
		int totalHeal = (int)(m_healPercentage * m_owner->getHpMax());
		m_healPerBurst = (int)(totalHeal / (m_duration / m_animSpeed));
		m_leftovers = (int)(totalHeal - (m_healPerBurst * (m_duration / m_animSpeed)));
		x = m_ownerFacingRight ? 1.0f : -1.0f;
		pos.x = m_ownerFacingRight ? m_ownerRight - ((m_ownerRight - m_ownerLeft) / 2.0f) : m_ownerLeft + ((m_ownerRight - m_ownerLeft) / 2.0f);
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 2;
		m_startPosition = pos;
		//m_healer = ParticleManager::Instance()->CreateEmitter(pos, Vector2(.025f, .025f), Vector2(0.0f, 1.0f), PT_HOLYSHOWER, m_animSpeed);
		// set position of center of object
		setCenterPosition(pos);
		setFacingRight(m_ownerFacingRight);
		//fireInDirection(x*m_speed, y*m_speed); //Heal doesn't move, it is instantaneous
		AudioManager::Instance()->PlaySound("shoot.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;
	};
}

Heal::~Heal(){

}

// gets called when the ability manager creates a new instance of your ability
Heal* Heal::clone(){
	Heal* clone = new Heal(m_id, m_tag, m_name, &getAbilityDesc());
	return clone;
}

// gets called during the next update loop after the manager creates an ability
void Heal::initSensorBody(){
	// use TriggerVolume::initSensorBody(b2FixtureDef) in this.
	b2FixtureDef fixDef;
	b2PolygonShape box;
	box.SetAsBox(m_hitBoxSize.x / 2.0f, m_hitBoxSize.y / 2.0f);
	fixDef.shape = &box;
	fixDef.isSensor = true;
	TriggerVolume::initSensorBody(fixDef);
	m_body->SetGravityScale(0.0f);
}

// gets called on every update, many times per second
void Heal::step(float dt){
	// call both of these functions at the beginning of every update
	stepAnimation(dt);
	// dont call decay if your ability has no fixed duration.
	if (!m_owner){
		m_duration = 0;
	}
	decay(dt);
	m_timer += dt;
	/*if (!rangeCheck()){
	die();
	}*/

	switch (m_stage){
	case BURST:
		if (m_timer >= m_animSpeed){
			m_owner->heal(m_healPerBurst);
			if (m_leftovers > 0){
				m_owner->heal(1);
				m_leftovers--;
			}
			m_timer -= m_animSpeed;
			int x = m_ownerFacingRight ? 1 : -1;
			reCalcOwnerPosition();
			Vector2 pos;
			pos.x = m_ownerFacingRight ? m_ownerRight - ((m_ownerRight - m_ownerLeft) / 2.0f) : m_ownerLeft + ((m_ownerRight - m_ownerLeft) / 2.0f);
			pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 2;
			m_startPosition = pos;
			ParticleManager::Instance()->EmitParticles(pos, m_texSize, Vector2(0.0f, 0.0f), PT_HOLYSHOWER);
		}
		
		int x = m_ownerFacingRight ? 1 : -1;
		Vector2 pos;
		pos.x = m_ownerFacingRight ? m_ownerRight - ((m_ownerRight - m_ownerLeft) / 2.0f) : m_ownerLeft + ((m_ownerRight - m_ownerLeft) / 2.0f);
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 2;
		//m_healer->setPosition(pos);
		break;
	};
}

// gets called when a gameobject enters the trigger volume
void Heal::onTrigger(GameObject* collider){
	if (collider == m_owner){
		return;
	}
	collider->damage(m_damage);
	if (m_owner){
		if (m_owner->getTag().compare("PLAYER") == 0 && collider->getTag().compare("ENEMY") == 0 && collider->getHp() <= 0){
			((Character*)m_owner)->addExp(((Enemy*)collider)->awardExp());
		}
	}

	switch (m_stage){
	case BURST:
		if (collider->getTag().compare("ENEMY") == 0){
			Enemy* enemy = ((Enemy*)collider);
			enemy->getRigidbody()->ApplyLinearImpulse(b2Vec2(m_knockback * (isFacingRight() ? 1 : -1), 0), enemy->getRigidbody()->GetPosition(), true);
		}
		else if (collider->getTag().compare("PLAYER") == 0){
			Character* player = ((Character*)collider);
			player->getRigidbody()->ApplyLinearImpulse(b2Vec2(m_knockback * (isFacingRight() ? 1 : -1), 0), player->getRigidbody()->GetPosition(), true);
		}
		die();
		break;
	};
}

int Heal::getManaCost(GameObject* caster){
	if (caster->getTag().compare("PLAYER") == 0){
		return (int)(m_manaPercentage * ((Character*)caster)->getManaMax());
	}
	return m_mana * 50;
}