#include "pch.h"
#include "Slash.h"
#include "AbilityManager.h"
#include "AudioManager.h"

Slash::Slash(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc)
	: Ability(id, tag, name, desc)
{
	//Slash is a single stage attack as of now, but if we add special things we can add them here
	std::vector<string> frags = CrusadeUtil::split(name, '_');
	if (frags.size() > 1){
		if (frags[1].compare("explosion") == 0){
			return;
		}
	}
	m_stage = SLASH;
}

// gets called immediately after initSensorBody
void Slash::onCast(){
	// use this to set spell velocity and position
	float x;
	float y = 0;
	Vector2 pos;
	switch (m_stage){
	case SLASH:
		x = m_ownerFacingRight ? 1.0f : -1.0f;
		pos.x = m_ownerFacingRight ? m_ownerRight + .5f : m_ownerLeft - .5f;
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 3;
		m_startPosition = pos;
		// set position of center of object
		setCenterPosition(pos);
		setFacingRight(m_ownerFacingRight);
		//fireInDirection(x*m_speed, y*m_speed); //Slash doesn't move, it is instantaneous
		AudioManager::Instance()->PlaySound("shoot.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;
	};
}

Slash::~Slash(){

}

// gets called when the ability manager creates a new instance of your ability
Slash* Slash::clone(){
	Slash* clone = new Slash(m_id, m_tag, m_name, &getAbilityDesc());
	return clone;
}

// gets called during the next update loop after the manager creates an ability
void Slash::initSensorBody(){
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
void Slash::step(float dt){
	// call both of these functions at the beginning of every update
	stepAnimation(dt);
	// dont call decay if your ability has no fixed duration.
	decay(dt);
	/*if (!rangeCheck()){
		die();
	}*/

	switch (m_stage){
	case SLASH:

		break;
	};
}

// gets called when a gameobject enters the trigger volume
void Slash::onTrigger(GameObject* collider){
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
	case SLASH:
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