#include "pch.h"
#include "HolyFire.h"
#include "AbilityManager.h"

HolyFire::HolyFire(UINT id, std::string tag, std::string name, Ability::AbilityDescription* desc)
	: Ability(id, tag, name, desc)
{
	// fireball is a multistage spell, it effectively is two seperate abilities defined in one class.
	// but instead of making two classes, we just switch on a stage enum type.

	// The name given to the spell determines its stage
	// in this case, names with "_fire" are the second explosion stage
	// otherwise, it is the projectile stage.
	std::vector<string> frags = CrusadeUtil::split(name, '_');
	if (frags.size() > 1){
		if (frags[1].compare("fire") == 0){
			m_stage = FIRE;
			return;
		}
	}
	m_passive = 0;
	m_stage = PROJECTILE;
	m_maxDuration = m_duration;
	m_dying = false;
	m_dead = false;
}

// gets called immediately after initSensorBody
void HolyFire::onCast(){
	// use this to set spell velocity and position
	float x;
	float y = .7f;
	Vector2 pos;
	Vector4 hsl;
	switch (m_stage){
	case PROJECTILE:
		m_body->SetGravityScale(1);
		m_body->GetFixtureList()->SetSensor(false);
		m_body->GetFixtureList()->SetRestitution(.9f);
		TriggerVolume::setEnableCollisionWith(Box2DSingleton::PHYSICAL_TRIGGER, Box2DSingleton::ENEMY | Box2DSingleton::WORLDTILE);
		setFacingRight(m_ownerFacingRight);
		x = m_ownerFacingRight ? 1.0f : -1.0f;
		pos.x = m_ownerFacingRight ? m_ownerRight : m_ownerLeft;
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 3;
		// set position of center of object
		setCenterPosition(pos);
		fireInDirection(x*m_speed, y*m_speed);
		AudioManager::Instance()->PlaySound("fireshoot.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;

	case FIRE:
		m_body->GetFixtureList()->SetSensor(true);
		m_body->GetFixtureList()->SetRestitution(0);
		pos.x = m_ownerLeft + (m_ownerRight - m_ownerLeft) / 2;
		pos.y = m_ownerBottom + (m_ownerTop - m_ownerBottom) / 2;
		setEnableCollisionWith(Box2DSingleton::TRIGGER, Box2DSingleton::CHARACTER | Box2DSingleton::ENEMY | Box2DSingleton::WORLDTILE);
		setCenterPosition(pos);
		AudioManager::Instance()->PlaySound("explosion.adpcm", SoundEffectInstance_Default, true, 1.0f, pos);
		break;
	}

	if (m_newOwner){
		m_owner = m_newOwner;
		m_newOwner = nullptr;
	}
}

HolyFire::~HolyFire(){

}

// gets called when the ability manager creates a new instance of your ability
HolyFire* HolyFire::clone(){
	HolyFire* clone = new HolyFire(m_id, m_tag, m_name, &getAbilityDesc());
	return clone;
}

// gets called during the next update loop after the manager creates an ability
void HolyFire::initSensorBody(){
	// use TriggerVolume::initSensorBody(b2FixtureDef) in this.
	b2FixtureDef fixDef;
	b2CircleShape circle;
	circle.m_radius = m_hitBoxSize.x / 2;
	fixDef.shape = &circle;
	fixDef.isSensor = true;
	TriggerVolume::initSensorBody(fixDef);
}

// gets called on every update, many times per second
void HolyFire::step(float dt){
	// call both of these functions at the beginning of every update
	stepAnimation(dt);
	// dont call decay if your ability has no fixed duration.
	decay(dt);

	switch (m_stage){
	case PROJECTILE:
		if (m_duration <= 0 && !m_dead){
			m_dying = true;
			m_duration = 0.001f;
		}
		if (m_body->GetLinearVelocity().x == 0 && m_body->GetLinearVelocity().y == 0){
			castAbility(m_id + 1);
			m_dead = true;
		}
		break;

	case FIRE:
		if (m_burnTimer <= 0){
			m_burnTimer = m_speed;
			//Find all enemies touching this HolyFire and BURN THEM
			for (b2ContactEdge* edge = m_body->GetContactList(); edge; edge = edge->next){
				if (edge->contact->IsTouching()){
					GameObject* objA = nullptr;
					GameObject* objB = nullptr;
					void* bodyUserDataA = edge->contact->GetFixtureA()->GetBody()->GetUserData();
					void* bodyUserDataB = edge->contact->GetFixtureB()->GetBody()->GetUserData();
					if (bodyUserDataA)
						objA = static_cast<GameObject*>(bodyUserDataA);
					if (bodyUserDataB)
						objB = static_cast<GameObject*>(bodyUserDataB);
					if (objA && bodyUserDataA){
						if (!(objA->getTag().compare("WORLDTILE") == 0) && !(objA->getTag().compare("HOLYFIRE") == 0)){//&& objA != m_owner
							objA->damage(m_damage);
							if (m_owner){
								if (m_owner->getTag().compare("PLAYER") == 0 && objA->getTag().compare("ENEMY") == 0 && objA->getHp() <= 0){
									((Character*)m_owner)->addExp(((Enemy*)objA)->awardExp());
								}
							}
						}
					}		
					if (objB && bodyUserDataB){
						if (!(objB->getTag().compare("WORLDTILE") == 0) && !(objB->getTag().compare("HOLYFIRE") == 0)){// && objB != m_owner
							objB->damage(m_damage);
							if (m_owner){
								if (m_owner->getTag().compare("PLAYER") == 0 && objB->getTag().compare("ENEMY") == 0 && objB->getHp() <= 0){
									((Character*)m_owner)->addExp(((Enemy*)objB)->awardExp());
								}
							}
						}
					}
				}
			}
		}
		m_burnTimer -= dt;
		break;
	}
}

// gets called when a gameobject enters the trigger volume
void HolyFire::onTrigger(GameObject* collider){
	//both the projectile and explosion damage things
	
	if (collider == m_owner){
		return;
	}
	switch (m_stage){
	case PROJECTILE:
		collider->damage(m_damage);

		if (m_owner){
			if (m_owner->getTag().compare("PLAYER") == 0 && collider->getTag().compare("ENEMY") == 0 && collider->getHp() <= 0){
				((Character*)m_owner)->addExp(((Enemy*)collider)->awardExp());
			}
		}

		// if the projectile hit things, figure out the appropriate response
		if (collider->getTag().compare("ENEMY") == 0){
			Ability* ptr = castAbility(m_id + 1);
			ptr->setOwnerAfterSpawn(m_owner);
			//If we can multicast, go ahead and cast it now and then keep moving
			if (m_passive==1){
				m_passive = 0;
			}
			else{
				m_dead = true;
				die();
			}
		}
		else if (m_passive == 1 && m_onTopOfTile){
			m_passive = 0;
			Ability* ptr = castAbility(m_id + 1);//spawn an early fire if we have the passive
			ptr->setOwnerAfterSpawn(m_owner);
		}
		else if(m_onTopOfTile){
			Ability* ptr = castAbility(m_id + 1);
			ptr->setOwnerAfterSpawn(m_owner);
			m_dead = true;
			die();
		}
		break;

	case FIRE:
		if (collider->getTag().compare("ENEMY") == 0){
			collider->damage(m_damage);
		}
		if (m_owner){
			if (m_owner->getTag().compare("PLAYER") == 0 && collider->getTag().compare("ENEMY") == 0 && collider->getHp() <= 0){
				((Character*)m_owner)->addExp(((Enemy*)collider)->awardExp());
			}
		}
		// do nothing for a fire. It will automatically decay and
		// die on its own.
		break;
	}
}

void HolyFire::BeginContact(b2Contact* contact, GameObject* other){
	m_onTopOfTile = false;
	GameObject* objA = nullptr;
	GameObject* objB = nullptr;
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserDataA)
		objA = static_cast<GameObject*>(bodyUserDataA);
	if (bodyUserDataB)
		objB = static_cast<GameObject*>(bodyUserDataB);
	if (objA && bodyUserDataA){
		if ((objA->getTag().compare("WORLDTILE") == 0) && objA != m_owner){
			int fixtureUserData = (int)contact->GetFixtureA()->GetUserData();
			if (fixtureUserData == WorldTile::Sides::UP){
				m_onTopOfTile = true;
			}
		}
	}
	if (objB && bodyUserDataB){
		if ((objB->getTag().compare("WORLDTILE") == 0) && objB != m_owner){
			int fixtureUserData = (int)contact->GetFixtureB()->GetUserData();
			if (fixtureUserData == WorldTile::Sides::UP){
				m_onTopOfTile = true;
			}
		}
	}
	TriggerVolume::BeginContact(contact, other);
}