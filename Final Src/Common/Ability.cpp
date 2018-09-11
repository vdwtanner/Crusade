#include "pch.h"
#include "Ability.h"
#include "AbilityManager.h"

Ability::Ability(UINT id, std::string tag, std::string name, AbilityDescription* desc)
	: TriggerVolume(tag, name, Vector2(0, 0), desc->texSize, desc->hitboxSize, desc->hitboxOffset, 0, id, -1)
{
	m_texSize = desc->texSize;
	m_hitBoxSize = desc->hitboxSize;
	m_hitBoxOffset = desc->hitboxOffset;
	m_animSpeed = desc->animSpeed;
	m_animLoop = desc->animLoop;
	m_damage = desc->damage;
	m_duration = desc->duration;
	m_range = desc->range;
	m_speed = desc->speed;
	m_knockback = desc->knockback;
	m_mana = desc->mana;
	m_cooldown = desc->cooldown;
	m_scaleFromCenter = desc->scaleFromCenter;
	m_castAnim = desc->castAnim;
	m_description = desc->description;
	m_iconPath = desc->iconPath;
	m_stagesRemaining = 0;
	m_texIndex = 0;

	m_owner = nullptr;
	m_newOwner = nullptr;
	m_ownerBottom = 0;
	m_ownerTop = 0;
	m_ownerLeft = 0;
	m_ownerRight = 0;
}

Ability::~Ability(){
	if (m_owner)
		m_owner->removeAbilityFromActive(this);
}

Ability::AbilityDescription Ability::getAbilityDesc(){
	AbilityDescription desc;

	desc.texSize = m_texSize;
	desc.hitboxSize = m_hitBoxSize;
	desc.hitboxOffset = m_hitBoxOffset;
	desc.animSpeed = m_animSpeed;
	desc.animLoop = m_animLoop;
	desc.damage = m_damage;
	desc.duration = m_duration;
	desc.range = m_range;
	desc.speed = m_speed;
	desc.knockback = m_knockback;
	desc.mana = m_mana;
	desc.cooldown = m_cooldown;
	desc.scaleFromCenter = m_scaleFromCenter;
	desc.castAnim = m_castAnim;
	desc.description = m_description;
	desc.iconPath = m_iconPath;
	desc.name = m_name;

	return desc;
}

float Ability::getCooldown(){
	return m_cooldown;
}

void Ability::stepAnimation(float dt){
	AbilityManager *man = AbilityManager::Instance();
	if (m_animLoop)
	{
		m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id), man->getAnimSize(m_id), m_animSpeed);
	}
	else
	{
		if (!isAnimationOver(dt, man->getAnimSize(m_id), m_animSpeed))
			m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id), man->getAnimSize(m_id), m_animSpeed);
		else
			m_texIndex = man->getAnimIndex(m_id) + (man->getAnimIndex(m_id) - 1);
	}
	
}

UINT Ability::getTexIndex() const{
	return m_texIndex;
}

void Ability::fireInDirection(float vx, float vy){
	m_body->SetLinearVelocity(b2Vec2(vx, vy));
}

bool Ability::initialized(){
	return m_body != nullptr;
}

GameObject* Ability::getOwner() const{
	return m_owner;
}

void Ability::setOwner(GameObject* owner){
	m_owner = owner;
	if (m_owner){
		if (owner->getTag().compare("PLAYER") == 0){
			Character* pc = ((Character*)owner);
			m_passive = pc->getPassiveForAbility(m_id);
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + pc->getHitboxSize().y;
			m_ownerFacingRight = m_owner->isFacingRight();
			if (!m_ownerFacingRight){
				m_ownerLeft = m_owner->getPosition().x + m_owner->getSize().x - pc->getHitboxSize().x;
				m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			}
			else{
				m_ownerLeft = m_owner->getPosition().x;
				m_ownerRight = m_owner->getPosition().x + pc->getHitboxSize().x;
			}
			
			
		}
		else if (owner->getTag().compare("ENEMY") == 0){
			Enemy* enemy = ((Enemy*)owner);
			m_passive = enemy->getPassiveForAbility(m_id);
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + enemy->getHitboxSize().y;
			m_ownerFacingRight = m_owner->isFacingRight();
			if (!m_ownerFacingRight){
				m_ownerLeft = m_owner->getPosition().x + m_owner->getSize().x - enemy->getHitboxSize().x;
				m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			}
			else{
				m_ownerLeft = m_owner->getPosition().x;
				m_ownerRight = m_owner->getPosition().x + enemy->getHitboxSize().x;
			}
		}
		else{
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + m_owner->getSize().y;
			m_ownerLeft = m_owner->getPosition().x;
			m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			m_ownerFacingRight = m_owner->isFacingRight();
		}
		/*if (m_owner->getTag().compare("PLAYER") == 0){
			m_passive=((Character*)owner)->getPassiveForAbility(m_id);
		}*/
	}
}

void Ability::reCalcOwnerPosition(){
	if (m_owner){
		if (m_owner->getTag().compare("PLAYER") == 0){
			Character* pc = ((Character*)m_owner);
			m_passive = pc->getPassiveForAbility(m_id);
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + pc->getHitboxSize().y;
			m_ownerFacingRight = m_owner->isFacingRight();
			if (!m_ownerFacingRight){
				m_ownerLeft = m_owner->getPosition().x + m_owner->getSize().x - pc->getHitboxSize().x;
				m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			}
			else{
				m_ownerLeft = m_owner->getPosition().x;
				m_ownerRight = m_owner->getPosition().x + pc->getHitboxSize().x;
			}


		}
		else if (m_owner->getTag().compare("ENEMY") == 0){
			Enemy* enemy = ((Enemy*)m_owner);
			m_passive = enemy->getPassiveForAbility(m_id);
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + enemy->getHitboxSize().y;
			m_ownerFacingRight = m_owner->isFacingRight();
			if (!m_ownerFacingRight){
				m_ownerLeft = m_owner->getPosition().x + m_owner->getSize().x - enemy->getHitboxSize().x;
				m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			}
			else{
				m_ownerLeft = m_owner->getPosition().x;
				m_ownerRight = m_owner->getPosition().x + enemy->getHitboxSize().x;
			}
		}
		else{
			m_ownerBottom = m_owner->getPosition().y;
			m_ownerTop = m_owner->getPosition().y + m_owner->getSize().y;
			m_ownerLeft = m_owner->getPosition().x;
			m_ownerRight = m_owner->getPosition().x + m_owner->getSize().x;
			m_ownerFacingRight = m_owner->isFacingRight();
		}
	}
}

/*Will change the owner of this abiltiy from the caster to the specified game object after the object spawns.
  This function to be called immediately after calling cast ability. Use as folows:
  Ability* ptr = castAbility(<id>);
  ptr->setOwnerAfterSpawn(<newOwner>);
 */
void Ability::setOwnerAfterSpawn(GameObject* newOwner){
	if (newOwner){
		m_newOwner = newOwner;
		m_newOwner->addAbilityToActive(this);
		bool success = m_owner->removeAbilityFromActive(this);
	}
}

/*Returns true if within range limit, false if over the range limit. Should probably call die() on false.*/
bool Ability::rangeCheck(){
	float hypotenuse = b2Distance(b2Vec2(m_startPosition.x,m_startPosition.y), b2Vec2(getPosition().x,getPosition().y));
	if (m_range - hypotenuse <= 0){
		return false;
	}
	return true;
}

void Ability::ownerHasDied(){
	m_owner = nullptr;
}

void Ability::onAirTrigger(WorldTile* air){
	float ratio = 1;
	if (m_scaleFromCenter){
		float distx = abs(air->getCenterPosition().x - getCenterPosition().x);
		float disty = abs(air->getCenterPosition().y - getCenterPosition().y);
		float hyp = sqrtf(powf(distx, 2) + powf(disty, 2));
		ratio = 1 - (hyp / getHitboxSize().x);
		if (ratio < .25f)
			ratio = .25f;
	}
	air->damage((int)(m_damage*ratio));
}

int Ability::getStagesRemaining(){
	return m_stagesRemaining;
}

int Ability::getManaCost(GameObject* caster){
	return m_mana;
}

string Ability::getDescription(){
	return m_description;
}

string Ability::getIconPath(){
	return m_iconPath;
}

void Ability::moveTo(float x, float y, float time){
	m_moveTo = true;
	m_moveToPos = Vector2(x, y);
	m_moveToTime = time;
}
