#include "pch.h"
#include "TriggerVolume.h"
#include "GameObject.h"

TriggerVolume::TriggerVolume(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitboxSize, Vector2 hitBoxOffset, float rotation, int id, int hp)
	: GameObject(tag, name, position, textureSize, hitboxSize, hitBoxOffset, -1, rotation, true)
{
	m_id = id;
	m_body = nullptr;
}

TriggerVolume::~TriggerVolume(){
	if (m_body)
		Box2DSingleton::Instance()->getWorld()->DestroyBody(m_body);
}

//Allow sorting of TriggerVolumes based off of ID
bool TriggerVolume::operator<(TriggerVolume const& e){
	return m_id < (UINT)e.getId();
}

int TriggerVolume::getId() const{
	return m_id;
}

void TriggerVolume::setId(int id){
	m_id = id;
}

float TriggerVolume::getDuration() const{
	return m_duration;
}

void TriggerVolume::setDuration(float dur){
	m_duration = dur;
}

void TriggerVolume::decay(float dt){
	m_duration -= dt;
	if (m_duration < 0){
		m_duration = 0;
	}
}

b2Body* TriggerVolume::getSensorBody(){
	return m_body;
}

void TriggerVolume::initSensorBody(b2FixtureDef sensorDef){
	b2BodyDef bodyDef;
	bodyDef.position.Set(getPosition().x + m_hitBoxSize.x / 2, getPosition().y + m_hitBoxSize.y / 2);
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = false;

	m_body = Box2DSingleton::Instance()->getWorld()->CreateBody(&bodyDef);
	m_body->CreateFixture(&sensorDef);
	m_body->SetUserData(this);
	m_body->SetGravityScale(0);
	if (sensorDef.isSensor)
		setEnableCollisionWith(Box2DSingleton::TRIGGER, Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER);
	else
		setEnableCollisionWith(Box2DSingleton::PHYSICAL_TRIGGER, Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER);
}

int TriggerVolume::getCollisionCategory(){
	return m_body->GetFixtureList()->GetFilterData().categoryBits;
}

void TriggerVolume::setEnableCollisionWith(int category, int collideWith){
	b2Filter filter;
	filter.categoryBits = category;
	filter.maskBits = collideWith;
	m_body->GetFixtureList()->SetFilterData(filter);
}

void TriggerVolume::BeginContact(b2Contact* contact, GameObject* other){
	for (int i = 0; (UINT)i < m_insideVolume.size(); i++){
		if (m_insideVolume[i] == other)
			return;
	}
	if (other)
	{
		if (other->getTag().compare("WORLDTILE") == 0)
		{
			if (((WorldTile*)other)->getId() == 0)
			{
				onAirTrigger((WorldTile*)other);
			}
			else
			{
				onTrigger(other);
			}
		}
		else
		{
			onTrigger(other);
		}
	}
	m_insideVolume.push_back(other);
}

void TriggerVolume::EndContact(b2Contact* contact, GameObject* other){
	std::vector<GameObject*>::iterator it;
	for (it = m_insideVolume.begin(); it != m_insideVolume.end(); ){
		if ((*it) == other)
		{
			it = m_insideVolume.erase(it);
			if (other->getTag().compare("WORLDTILE") == 0)
			{
				if (((WorldTile*)other)->getId() != 0)
					onExitTrigger(other);
			}
			else
			{
				onExitTrigger(other);
			}
		}
		else
		{
			it++;
		}
	}
}

void TriggerVolume::onExitTrigger(GameObject* collider){

}

bool TriggerVolume::isDead() const{
	return m_duration <= 0;
}

void TriggerVolume::die(){
	m_duration = 0;
}

void TriggerVolume::setCenterPosition(Vector2 pos){
	if (isFacingRight())
	{
		GameObject::setPosition(Vector2(pos.x - m_textureSize.x / 2.0f, pos.y - m_textureSize.y / 2.0f));
		//GameObject::setPosition(Vector2(pos.x - m_textureSize.x / 2.0f, pos.y));
	}
	else
	{
		GameObject::setPosition(Vector2(pos.x - m_textureSize.x / 2.0f, pos.y - m_textureSize.y / 2.0f));
	}
	if (m_body)
		m_body->SetTransform(b2Vec2(pos.x, pos.y), m_body->GetAngle());
}

void TriggerVolume::setPosition(Vector2 pos){
	GameObject::setPosition(Vector2(pos.x, pos.y));
	if (isFacingRight())
		m_body->SetTransform(b2Vec2(pos.x + m_textureSize.x / 2, pos.y + m_textureSize.y / 2), m_body->GetAngle());
	else
		m_body->SetTransform(b2Vec2(pos.x + m_textureSize.x / 2, pos.y + m_textureSize.y / 2), m_body->GetAngle());
}

// based on David Saltares "Fix your timestep in Libgdx and Box2D" interpolation function
// http://saltares.com/blog/games/fixing-your-timestep-in-libgdx-and-box2d/
void TriggerVolume::interpolate(float alpha){
	Vector2 position = getPosition();
	b2Vec2 bodyPosition = m_body->GetPosition();


	if (!isFacingRight())
	{
		position.x = (bodyPosition.x + m_hitBoxOffset.x) * alpha + (position.x + (m_textureSize.x / 2)) * (1.0f - alpha);
		position.x -= m_textureSize.x / 2;

	}
	else

	{
		position.x = (bodyPosition.x - m_hitBoxOffset.x) * alpha + (position.x + (m_textureSize.x / 2)) * (1.0f - alpha);
		position.x -= m_textureSize.x / 2;

	}

	position.y = (bodyPosition.y - m_hitBoxOffset.y) * alpha + (position.y + (m_textureSize.y / 2)) * (1.0f - alpha);
	position.y -= m_textureSize.y / 2;

	GameObject::setPosition(position);
}