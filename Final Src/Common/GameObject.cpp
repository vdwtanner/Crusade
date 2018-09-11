#include "pch.h"
#include "GameObject.h"
#include "AbilityManager.h"
#include "Ability.h"

GameObject::GameObject(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, int hp = -1, float rotation = 0.0, bool facingRight = true){
	this->m_tag = tag;
	this->m_name = name;
	this->m_position = position;
	this->m_textureSize = textureSize;
	this->m_hitBoxSize = hitBoxSize;
	this->m_hitBoxOffset = hitBoxOffset;
	this->m_rotation = rotation;
	this->m_hpmax = hp;
	this->m_hp = hp;
	this->m_timer = 0;
	this->facingRight = facingRight;
	this->m_prevFacingRight = facingRight;
	this->m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

GameObject::GameObject(){
	m_tag = "GAMEOBJECT";
	m_name = "";
	m_position = Vector2(0, 0);
	m_textureSize = Vector2(1, 1);
	m_hitBoxSize = Vector2(1, 1);
	m_rotation = 0;
	m_timer = 0;
	facingRight = true;
	m_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}

GameObject::~GameObject(){
	auto it = m_activeAbilities.begin();
	for (it; it != m_activeAbilities.end(); it++){
		(*it)->ownerHasDied();
	}
}

std::string GameObject::getName(){
	return m_name;
}

//Returns the tag of this Game Object. This will be Player, Enemy, or Entity
std::string GameObject::getTag(){
	return m_tag;
}

Vector2 GameObject::getPosition(){
	return m_position;
}

Vector2 GameObject::getCenterPosition(){
	return Vector2(getRenderPosition().x + m_textureSize.x / 2, getRenderPosition().y + m_textureSize.y / 2);
}

Vector2 GameObject::getRenderPosition(){
	return Vector2(m_position.x + (facingRight ? 0 : m_textureSize.x), m_position.y);
}

float GameObject::getRotation(){
	return m_rotation;
}

Vector2 GameObject::getSize(){
	return m_textureSize;
}

Vector2 GameObject::getHitBoxOffset(){
	return m_hitBoxOffset;
}

Vector2 GameObject::getRenderSize(){
	return Vector2(m_textureSize.x*(facingRight ? 1 : -1), m_textureSize.y);
}

Vector2 GameObject::getTrackingPosition(){
	Vector2 pos = getPosition();
	Vector2 size = getSize();

	if (isFacingRight())
		return Vector2(pos.x + (size.x / 2) + m_hitBoxOffset.x, pos.y + (size.y / 2) + m_hitBoxOffset.y);
	else
		return Vector2(pos.x + (size.x / 2) - m_hitBoxOffset.x, pos.y + (size.y / 2) + m_hitBoxOffset.y);
}

Vector4 GameObject::getColor(){
	return m_color;
}

int GameObject::getHp(){
	return m_hp;
}

int GameObject::getHpMax(){
	return m_hpmax;
}

void GameObject::setTag(std::string tag){
	m_tag = tag;
}

void GameObject::setName(std::string name){
	m_name = name;
}

void GameObject::setPosition(Vector2 position){
	m_position = position;
}

void GameObject::setSize(Vector2 size){
	m_textureSize = size;
}

void GameObject::setHitBoxOffset(Vector2 offset){
	m_hitBoxOffset = offset;
}

void GameObject::setColor(Vector4 color){
	m_color = color;
}

void GameObject::setHp(int hp){
	m_hp = hp;
}

void GameObject::setHpMax(int hp){
	m_hpmax = hp;
}

bool GameObject::isAnimationOver(const float dt, const UINT size, const float frameTime){
	if (m_timer + dt >= (frameTime * size))
		return true;
	else
		return false;
}

UINT GameObject::getAnimationFrameIndex(const float dt, const UINT start, const UINT size, const float frameTime){
	m_timer += dt;
	if (m_timer >= size * frameTime)
	{
		m_timer = 0;
		return start;
	}
	else
	{
		return start + (int)(m_timer / frameTime);
	}
}

void GameObject::resetAnimation(){
	m_timer = 0;
}

bool GameObject::isFacingRight(){
	return facingRight;
}

//true = right, false = left
void GameObject::setFacingRight(bool isRight){
	facingRight = isRight;
}

Vector2 GameObject::getHitboxSize(){
	return m_hitBoxSize;

}


void GameObject::applyStatusAilment(Status name, StatusDesc desc){
	m_statusMap[name] = desc;
}

void GameObject::cureStatusAilment(Status name){
	StatusDesc nullDesc = { 0 };
	m_statusMap[name] = nullDesc;
}

void GameObject::damage(int num){
	if (m_hp != -1){
		m_hp -= num;
		if (m_hp < 0){
			m_hp = -1;
		}
		if (m_tag == "ENEMY"){
			((Enemy*)this)->respondToAggression();
		}
	}
}

void GameObject::heal(int num){ 
	if (m_hp != -1){
		m_hp += num;
		if (m_hp > m_hpmax)
			m_hp = m_hpmax;
	}
}

Ability* GameObject::castAbility(UINT id){
	Ability* ptr;
	bool success = AbilityManager::Instance()->castAbility(id, this, &ptr);
	if (success)
		m_activeAbilities.push_back(ptr);

	return ptr;
}

bool GameObject::removeAbilityFromActive(Ability* ptr){
	auto it = m_activeAbilities.begin();
	for (it; it != m_activeAbilities.end();){
		if ((*it) == ptr)
		{
			it = m_activeAbilities.erase(it);
			return true;
		}
		else
		{
			it++;
		}
	}
	return false;
}

void GameObject::addAbilityToActive(Ability* ability){
	m_activeAbilities.push_back(ability);
}

void GameObject::PreSolve(b2Contact* contact, GameObject* other, const b2Manifold*){

}
void GameObject::PostSolve(b2Contact* contact, GameObject* other, const b2ContactImpulse*){

}