#include "pch.h"
#include "WorldTileMatrix.h"
#include "WorldTile.h"
#include "ParticleManager.h"

/*Position of tile is bottom left corner
 */
/*WorldTile::WorldTile(Vector3 pos, UINT id, int health, bool isActiveLayer){
	m_id = id;
	m_pos = pos;
	m_health = health;
	if (isActiveLayer){
		b2BodyDef tileBodyDef;
		tileBodyDef.position.Set(pos.x, pos.y);
		m_rigidbody = Box2DSingleton::Instance()->getWorld()->CreateBody(&tileBodyDef);
		b2PolygonShape tileBox;
		tileBox.SetAsBox(.5f, .5f);
		m_rigidbody->CreateFixture(&tileBox, 0.0f);

		if (id != 0)
		{
			setEnableCollisionWith(Box2DSingleton::NONE);
		}
		else
		{
			setEnableCollisionWith(Box2DSingleton::ALL);
		}
	}
	else{
		m_rigidbody = NULL;
	}
}*/

//Not the best option, will not be useable
WorldTile::WorldTile() : GameObject("WORLDTILE", "tile", Vector2(0, 0), Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), 100, 0, true){
	m_id = 0;
	m_rigidbody = NULL;
	this->setTag("WORLDTILE");
}

WorldTile::~WorldTile(){
	if (m_rigidbody){
		if (Box2DSingleton::Instance()->getWorld())
			Box2DSingleton::Instance()->getWorld()->DestroyBody(m_rigidbody);
	}
}

void WorldTile::Update(float dt){

}

UINT WorldTile::getId(){
	return m_id;
}

void WorldTile::setId(UINT id){
	m_id = id;
}

void WorldTile::initRigidbody(b2BodyDef def){
	m_rigidbody = Box2DSingleton::Instance()->getWorld()->CreateBody(&def);

	b2EdgeShape up;
	up.Set(b2Vec2(-.5f, .5f), b2Vec2(.5f, .5f));

	b2EdgeShape right;
	right.Set(b2Vec2(.5f, .5f), b2Vec2(.5f, -.5f));

	b2EdgeShape down;
	down.Set(b2Vec2(-.5f, -.5f), b2Vec2(.5f, -.5f));

	b2EdgeShape left;
	left.Set(b2Vec2(-.5f, .5f), b2Vec2(-.5f, -.5f));

	b2Fixture *upfix = m_rigidbody->CreateFixture(&up, 0.5f);
	upfix->SetUserData((void *) UP);

	b2Fixture *rightfix = m_rigidbody->CreateFixture(&right, 0.5f);
	rightfix->SetUserData((void *) RIGHT);

	b2Fixture *downfix = m_rigidbody->CreateFixture(&down, 0.5f);
	downfix->SetUserData((void *) DOWN);

	b2Fixture *leftfix = m_rigidbody->CreateFixture(&left, 0.5f);
	leftfix->SetUserData((void *) LEFT);

	m_rigidbody->GetFixtureList()->SetFriction(.5);
	//We want to set the fricition for all fixtures in the world tile
	for (b2Fixture* fixture = m_rigidbody->GetFixtureList(); fixture; fixture = fixture->GetNext()){
		fixture->SetFriction(.5);
	}
	m_rigidbody->SetUserData(this);

	setSideEnableCollisionWith(ALL, Box2DSingleton::ALL);
}

b2Body* WorldTile::getRigidbody(){
	return m_rigidbody;
}


void WorldTile::damage(int num){
	if (m_hp != -1){
		m_hp -= num;
		float ratio = ((float)m_hp / (float)m_hpmax) * .4f;
		setColor(Vector4(ratio+.6f, ratio+.6f, ratio+.6f, 1.0f));
		WorldTileMatrix::Instance()->updateStagingTile((int)getPosition().x, (int)getPosition().y, getLayer());
		if (m_hp < 0){
			m_hp = -1;
			ParticleManager::Instance()->EmitParticles(Vector2(getPosition().x + 0.5f, getPosition().y + 0.5f), Vector2(.1f, .1f), Vector2(0.0f, 0.5f), PT_BLOCK, WorldTileMatrix::Instance()->getTextureIndex(m_id));
			WorldTileMatrix::Instance()->removeTile((int)getPosition().x, (int)getPosition().y, getLayer());
		}
		AudioManager::Instance()->PlaySound("block_hit.adpcm", SoundEffectInstance_Default, true, 1.0f, getPosition());
	}
	WorldTileMatrix::Instance()->damageTilesInFront((int)getPosition().x, (int)getPosition().y, getLayer(), num);
}

void WorldTile::damageThisOnly(int num){
	if (m_hp != -1){
		m_hp -= num;
		float ratio = ((float)m_hp / (float)m_hpmax) * .4f;
		setColor(Vector4(ratio + .6f, ratio + .6f, ratio + .6f, 1.0f));
		WorldTileMatrix::Instance()->updateStagingTile((int)getPosition().x, (int)getPosition().y, getLayer());
		if (m_hp < 0){
			m_hp = -1;
			ParticleManager::Instance()->EmitParticles(Vector2(getPosition().x + 0.5f, getPosition().y + 0.5f), Vector2(.1f, .1f), Vector2(0.0f, 0.5f), PT_BLOCK, WorldTileMatrix::Instance()->getTextureIndex(m_id));
			WorldTileMatrix::Instance()->removeTile((int)getPosition().x, (int)getPosition().y, getLayer());
		}
	}
}

void WorldTile::heal(int num){
	if (m_hp != -1){
		m_hp += num;
		float ratio = (float)m_hp / (float)m_hpmax;
		setColor(Vector4(ratio, ratio, ratio, 1.0f));
		WorldTileMatrix::Instance()->updateStagingTile((int)getPosition().x, (int)getPosition().y, getLayer());
		if (m_hp > m_hpmax)
			m_hp = m_hpmax;
	}
}

void WorldTile::destroyTile(){
	//TODO add destroy animation
	m_id = 0;
	m_hp = -1;
	m_hpmax = -1;
	setSideEnableCollisionWith(ALL, Box2DSingleton::NONE);
}

void WorldTile::setSideEnableCollisionWith(int side, int collideWith){
	b2Filter filter;
	filter.categoryBits = Box2DSingleton::WORLDTILE;
	filter.maskBits = collideWith;

	b2Fixture *it;
	switch (side){
	case UP:
		for (it = m_rigidbody->GetFixtureList(); it; it = it->GetNext()){
			if ((int)it->GetUserData() == UP){
				it->SetFilterData(filter);
				break;
			}
		}
		break;
	case RIGHT:
		for (it = m_rigidbody->GetFixtureList(); it; it = it->GetNext()){
			if ((int)it->GetUserData() == RIGHT){
				it->SetFilterData(filter);
				break;
			}
		}
		break;
	case DOWN:
		for (it = m_rigidbody->GetFixtureList(); it; it = it->GetNext()){
			if ((int)it->GetUserData() == DOWN){
				it->SetFilterData(filter);
				break;
			}
		}
		break;
	case LEFT:
		for (it = m_rigidbody->GetFixtureList(); it; it = it->GetNext()){
			if ((int)it->GetUserData() == LEFT){
				it->SetFilterData(filter);
				break;
			}
		}
		break;
	case ALL:
		for (it = m_rigidbody->GetFixtureList(); it; it = it->GetNext()){
			it->SetFilterData(filter);
		}
		break;
	}
}

GameObject* WorldTile::clone(){
	return nullptr;
}

void WorldTile::step(float dt){

}

void WorldTile::BeginContact(b2Contact* contact, GameObject* other){

}

void WorldTile::EndContact(b2Contact* contact, GameObject* other){

}

int WorldTile::getLayer(){
	return m_layer;
}

void WorldTile::setLayer(int layer){
	m_layer = layer;
}