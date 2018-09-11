#include "pch.h"
#include "Fireball.h"
#include "AbilityManager.h"

Fireball::Fireball(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc)
	: Ability(id, tag, name, desc)
{
	// fireball is a multistage spell, it effectively is two seperate abilities defined in one class.
	// but instead of making two classes, we just switch on a stage enum type.

	// The name given to the spell determines its stage
	// in this case, names with "_explosion" are the second explosion stage
	// otherwise, it is the projectile stage.
	std::vector<string> frags = CrusadeUtil::split(name, '_');
	if (frags.size() > 1){
		if (frags[1].compare("explosion") == 0){
			m_stage = EXPLOSION;
			m_stagesRemaining = 0;
			return;
		}
		else{
			m_stagesRemaining = 1;
		}
	}
	m_stage = PROJECTILE;
}

// gets called immediately after initSensorBody
void Fireball::onCast(){
	// use this to set spell velocity and position
	float x;
	float y = 0;
	Vector2 pos;
	switch (m_stage){
	case PROJECTILE:
		setFacingRight(m_ownerFacingRight);
		x = m_ownerFacingRight ? 1.0f : -1.0f;
		pos.x = m_ownerFacingRight? m_ownerRight : m_ownerLeft;
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom)/3;

		// set position of center of object
		setCenterPosition(pos);
		fireInDirection(x*m_speed, y*m_speed);
		AudioManager::Instance()->PlaySound("fireshoot.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;

	case EXPLOSION:
		pos.x = m_ownerLeft + (m_ownerRight - m_ownerLeft) /2;
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom)/2;
		setCenterPosition(pos);
		AudioManager::Instance()->PlaySound("explosion.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;
	}

	if (m_newOwner){
		m_owner = m_newOwner;
		m_newOwner = nullptr;
	}
}

Fireball::~Fireball(){
	
}

// gets called when the ability manager creates a new instance of your ability
Fireball* Fireball::clone(){
	Fireball* clone = new Fireball(m_id, m_tag, m_name, &getAbilityDesc());
	return clone;
}

// gets called during the next update loop after the manager creates an ability
void Fireball::initSensorBody(){
	// use TriggerVolume::initSensorBody(b2FixtureDef) in this.
	b2FixtureDef fixDef;
	b2CircleShape circle;
	circle.m_radius = m_hitBoxSize.x/2;
	fixDef.shape = &circle;
	fixDef.isSensor = true;
	TriggerVolume::initSensorBody(fixDef);
	TriggerVolume::setEnableCollisionWith(Box2DSingleton::TRIGGER, Box2DSingleton::CHARACTER | Box2DSingleton::ENEMY | Box2DSingleton::WORLDTILE);
}

// gets called on every update, many times per second
void Fireball::step(float dt){
	// call both of these functions at the beginning of every update
	stepAnimation(dt);
	// dont call decay if your ability has no fixed duration.
	decay(dt);

	switch (m_stage){
	case PROJECTILE:
		break;

	case EXPLOSION:

		break;
	}
}

// gets called when a gameobject enters the trigger volume
void Fireball::onTrigger(GameObject* collider){
	//both the projectile and explosion damage things
	if (collider == m_owner && m_stage==PROJECTILE){
		return;
	}
	float ratio = 1;
	if (m_scaleFromCenter){
		float distx = abs(collider->getCenterPosition().x - getCenterPosition().x);
		float disty = abs(collider->getCenterPosition().y - getCenterPosition().y);
		float hyp = sqrtf(powf(distx, 2) + powf(disty, 2));
		ratio = 1 - (hyp / getHitboxSize().x);
		if (ratio < .25f)
			ratio = .25f;
	}
	collider->damage((int)(m_damage*ratio));
	if (m_owner){
		if (m_owner->getTag().compare("PLAYER") == 0 && collider->getTag().compare("ENEMY") == 0 && collider->getHp() <= 0){
			((Character*)m_owner)->addExp(((Enemy*)collider)->awardExp());
		}
	}

	switch (m_stage){
	case PROJECTILE:
		// if the projectile hit things, spawn a fireball_explosion
		// then kill the projectile
		if (!isDead()){
			Ability* ptr = castAbility(m_id + 1);
			ptr->setOwnerAfterSpawn(m_owner);
		}
			
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

	case EXPLOSION:
		// do nothing for an explosion. It will automatically decay and
		// die on its own.
		if (collider->getTag().compare("ENEMY") == 0){
			Enemy* enemy = ((Enemy*)collider);
			b2Vec2 test = m_body->GetPosition() - enemy->getRigidbody()->GetPosition();
			enemy->getRigidbody()->ApplyLinearImpulse(b2Vec2(m_knockback * (test.x < 0 ? 1 : -1), m_knockback * (test.y < 0 ? 1 : -1)), enemy->getRigidbody()->GetPosition(), true);
		}
		else if (collider->getTag().compare("PLAYER") == 0){
			Character* player = ((Character*)collider);
			b2Vec2 test = m_body->GetPosition() - player->getRigidbody()->GetPosition();
			player->getRigidbody()->ApplyLinearImpulse(b2Vec2(m_knockback * (test.x < 0 ? 1 : -1), m_knockback * (test.y < 0 ? 1 : -1)), player->getRigidbody()->GetPosition(), true);
		}
		break;
	}
}