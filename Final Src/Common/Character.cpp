#include "pch.h"
#include "Character.h"
#include "Windows.h"
#include "ParticleManager.h"


//Used by Character to generate the Master Objects for 
//DO NOT USE FOR CRUSADE GAME! Only used for Level Editor
Character::Character(Vector2 position, float rotation)
	: GameObject("PLAYER", "Player", position, Vector2(2.0f , 2.0f), Vector2(.6f , 1.5), Vector2(-0.5f, -0.25f), 20, rotation, true){
	m_profile = new Profile("Martin", nullptr);
	m_deleteProfileOnDestruct = true;

	this->m_id = 0;
	this->m_hp = 40 + 10*m_profile->getLevel();
	this->m_hpmax = m_hp;
	this->m_exp = 0;
	this->m_speed = 5;
	this->m_strength = 200*1.2;
	this->m_massDensity = 15;
	this->m_bodyAnim = ANIM_IDLE;
	this->m_spawnPoint = position;
	this->m_manaTimer = 0.0f;
	this->m_godMode = false;
	this->m_moveTo = false;
	this->m_moveToTime = 0.0f;
	this->m_dead = false;
	
	m_canFly = false;
	m_rigidbody = NULL;
	wchar_t workingDir[300];
	GetCurrentDirectory(300, workingDir);
	loadTextures("../img/Sprites/Characters/player/");
	b2BodyDef def;
	def.position.Set(m_hitBoxSize.x / 2.0f, m_hitBoxSize.y / 2.0f);
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	initRigidbody(def);
	setPosition(position);
	createBuffers();
	//test values
	UINT actives[] = { 1, 3, 4, 6 };
	UINT passives[] = { 0, 0, 1, 0 };
	//end test values
	initAbilities(m_profile->getActiveAbilities(), m_profile->getPassiveAbilities());
	initCooldownArrays();
}

Character::Character(Vector2 position, float rotation, Profile* profile)
	: GameObject("PLAYER", "Player", position, Vector2(2.0f, 2.0f), Vector2(.6f, 1.5f), Vector2(-0.5f, -0.25f), 20, rotation, true){
	m_profile = profile;
	m_deleteProfileOnDestruct = false;

	UINT level = m_profile->getLevel();
	this->m_id = 0;
	this->m_hp = m_profile->getLevelLookUp()->getHP(level);
	this->m_hpmax = m_hp;
	this->m_exp = 0;
	this->m_speed = 5;
	this->m_strength = 200 * 1.2;
	this->m_massDensity = 15;
	this->m_bodyAnim = ANIM_IDLE;
	this->m_spawnPoint = position;
	this->m_mana = m_profile->getLevelLookUp()->getMana(level);
	this->m_manaMax = m_mana;
	this->m_manaTimer = 0.0f;
	this->m_godMode = false;
	this->m_dead = false;

	m_canFly = false;
	m_rigidbody = NULL;
	wchar_t workingDir[300];
	GetCurrentDirectory(300, workingDir);
	loadTextures("../img/Sprites/Characters/player/");
	b2BodyDef def;
	def.position.Set(m_hitBoxSize.x / 2.0f, m_hitBoxSize.y / 2.0f);
	def.type = b2_dynamicBody;
	def.fixedRotation = true;
	initRigidbody(def);
	setPosition(position);
	createBuffers();
	//test values
	UINT actives[] = { 1, 3, 4, 6 };
	UINT passives[] = { 0, 0, 1, 0 };
	//end test values
	initAbilities(m_profile->getActiveAbilities(), m_profile->getPassiveAbilities());
	initCooldownArrays();

	vector<UINT> abils = m_profile->getLevelLookUp()->getAbilities(m_profile->getLevel());
}

Character::~Character(){
	if (m_deleteProfileOnDestruct){
		delete(m_profile);
	}
	m_VB.Reset();
	m_texArray.Reset();
	for (auto it = m_texPaths.begin(); it != m_texPaths.end(); it++){
		delete[] *it;
	}
	if (Box2DSingleton::Instance()->getWorld())
		Box2DSingleton::Instance()->getWorld()->DestroyBody(m_rigidbody);
}

int Character::getId() const{
	return m_id;
}

int Character::getExp() const{
	return m_exp;
}

UINT Character::getBodyTexIndex() const{
	return m_bodyTexIndex;
}

int Character::getMana(){
	return m_mana;
}

int Character::getManaMax(){
	return m_manaMax;
}

void Character::setId(int id){
	m_id = id;
}

void Character::setExp(int exp){
	m_exp = exp;
}

//Add to the current amount of player EXP
void Character::addExp(int exp){
	m_exp += exp;
}

//adds EXP to profile, resets current EXP, and returns leveles gained.
int Character::applyExp(){
	int levelsGained = m_profile->addExp(m_exp);
	m_exp = 0;
	return levelsGained;
}

//This is the only way that position shouild be set since it will also update the rigidbody
void Character::setPosition(Vector2 pos){
	if (isFacingRight())
	{
		GameObject::setPosition(Vector2(pos.x, pos.y));
		m_rigidbody->SetTransform(b2Vec2(pos.x + m_textureSize.x / 2, pos.y + m_textureSize.y / 2), m_rigidbody->GetAngle());
	}
	else
	{
		GameObject::setPosition(Vector2(pos.x, pos.y));
		m_rigidbody->SetTransform(b2Vec2(pos.x - m_textureSize.x / 2, pos.y + m_textureSize.y / 2), m_rigidbody->GetAngle());
	}
	m_rigidbody->SetAwake(true);
}

b2Body* Character::getRigidbody(){
	return m_rigidbody;
}

/*Initialize the cooldown arrays*/
void Character::initCooldownArrays(){
	AbilityManager* am = AbilityManager::Instance();
	for (int x = 0; x < 4; x++){
		m_cooldowns[x] = am->getCooldown(m_activeAbilities[x]);
		m_cooldownTimers[x] = 0;
	}
}

/*initialize the arrays for active and passive ability IDs*/
void Character::initAbilities(UINT actives[4], UINT passives[4]){
	for (int x = 0; x < 4; x++){
		m_activeAbilities[x] = actives[x];
		m_passiveAbilities[x] = passives[x];
	}
}

void Character::initRigidbody(b2BodyDef def){
	m_rigidbody = Box2DSingleton::Instance()->getWorld()->CreateBody(&def);
	b2PolygonShape tileBox;
	tileBox.SetAsBox(m_hitBoxSize.x / 2.0f, m_hitBoxSize.y / 2.0f);
	m_rigidbody->CreateFixture(&tileBox, 0.5f);
	m_rigidbody->GetFixtureList()->SetFriction(.5f);
	m_rigidbody->GetFixtureList()->SetDensity(m_massDensity);
	m_rigidbody->ResetMassData();
	m_rigidbody->SetUserData(this);
	m_rigidbody->SetGravityScale(1.3f);
	setEnableCollisionWith(Box2DSingleton::ALL);
}


// based on David Saltares "Fix your timestep in Libgdx and Box2D" interpolation function
// http://saltares.com/blog/games/fixing-your-timestep-in-libgdx-and-box2d/
void Character::interpolate(float alpha){
	if (!m_dead){
		Vector2 position = getPosition();
		b2Vec2 bodyPosition = m_rigidbody->GetPosition();


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
}

void Character::setEnableCollisionWith(int collideWith){
	b2Filter filter;
	filter.categoryBits = Box2DSingleton::CHARACTER;
	filter.maskBits = collideWith;
	m_rigidbody->GetFixtureList()->SetFilterData(filter);
}

//Allow sorting of Enemies based off of ID
bool Character::operator<(Character const& e){
	return m_id < e.getId();
}

void Character::setFacingRight(bool direction){
	facingRight = direction;
	if (m_prevFacingRight && !facingRight){
		GameObject::setPosition(Vector2(getPosition().x + (m_hitBoxOffset.x * 2), getPosition().y));
	}
	if (!m_prevFacingRight && facingRight){
		GameObject::setPosition(Vector2(getPosition().x - (m_hitBoxOffset.x * 2), getPosition().y));
	}
	m_prevFacingRight = direction;
}

void Character::stepAnimation(float dt){
	/*Original style - Single sprite for everything
	switch (m_currAnim){
	case ANIM_IDLE:
		m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_IDLE), getAnimSize(m_id, ANIM_IDLE), .5f);
		break;
	case ANIM_WALK:
		m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_WALK), getAnimSize(m_id, ANIM_WALK), .15f);
		break;
	case ANIM_JUMP:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_JUMP), .1f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_JUMP), getAnimSize(m_id, ANIM_JUMP), .5f);
		else
			m_bodyTexIndex = getAnimIndex(m_id, ANIM_JUMP);
		break;
	case ANIM_DEATH:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_DEATH), .1f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_DEATH), getAnimSize(m_id, ANIM_DEATH), .2f);
		break;
	case ANIM_CAST:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_CAST), .2f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_CAST), getAnimSize(m_id, ANIM_CAST), .5f);
		else
			m_currAnim = ANIM_JUMP;
		break;
	case ANIM_SLASH:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_SLASH), .2f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_SLASH), getAnimSize(m_id, ANIM_SLASH), .5f);
		else
			m_currAnim = ANIM_JUMP;
		break;
	}
	*/

	//New style - Body and arm separate to allow for simultaneous attacking and moving animations
	//First we set body tex index
	switch (m_bodyAnim){
	case ANIM_IDLE:
		m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_IDLE), getAnimSize(m_id, ANIM_IDLE), .5f);
		break;
	case ANIM_WALK:
		m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_WALK), getAnimSize(m_id, ANIM_WALK), .15f);
		break;
	case ANIM_JUMP:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_JUMP), .1f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_JUMP), getAnimSize(m_id, ANIM_JUMP), .5f);
		else
			m_bodyTexIndex = getAnimIndex(m_id, ANIM_JUMP);
		break;
	case ANIM_DEATH:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_DEATH), .1f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_DEATH), getAnimSize(m_id, ANIM_DEATH), .2f);
		break;
	default:
		//default to idle animation and switch to ANIM_IDLE so that nothing is messed up on next step
		m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_IDLE), getAnimSize(m_id, ANIM_IDLE), .5f);
		m_bodyAnim = ANIM_IDLE;
		break;
	}

	switch (m_armAnim){
	case ANIM_IDLE:
		m_armTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_IDLE), getAnimSize(m_id, ANIM_IDLE), .5f);
		break;
	case ANIM_WALK:
		m_armTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_WALK), getAnimSize(m_id, ANIM_WALK), .15f);
		break;
	case ANIM_JUMP:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_JUMP), .1f))
			m_armTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_JUMP), getAnimSize(m_id, ANIM_JUMP), .5f);
		else
			m_armTexIndex = getAnimIndex(m_id, ANIM_JUMP);
		break;
	case ANIM_DEATH:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_DEATH), .1f))
			m_armTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_DEATH), getAnimSize(m_id, ANIM_DEATH), .2f);
		break;
	case ANIM_SLASH:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_SLASH), .2f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_SLASH), getAnimSize(m_id, ANIM_SLASH), .5f);
		else
			m_armAnim = ANIM_IDLE;
		break;
	case ANIM_CAST:
		if (!isAnimationOver(dt, getAnimSize(m_id, ANIM_CAST), .2f))
			m_bodyTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_CAST), getAnimSize(m_id, ANIM_CAST), .5f);
		else
			m_armAnim = ANIM_IDLE;
		break;
	default:
		//default to idle animation and switch to ANIM_IDLE so that nothing is messed up on next step
		m_armTexIndex = getAnimationFrameIndex(dt, getAnimIndex(m_id, ANIM_IDLE), getAnimSize(m_id, ANIM_IDLE), .5f);
		m_armAnim = ANIM_IDLE;
		break;
	}

}


/*velocity should be speed*(isFacingRight() ? 1 : -1)
NOTE: The Character will be affected by friction, thus slowing it down*/
void Character::walk(b2Vec2 vel){
	m_rigidbody->SetLinearDamping(0);
	m_rigidbody->SetLinearVelocity(vel);
	m_rigidbody->SetAwake(true);
}

void Character::halt(){
	if (m_rigidbody->GetLinearVelocity().y == 0 || canFly())
		m_rigidbody->SetLinearDamping(8);
	else
		m_rigidbody->SetLinearDamping(0);
}


void Character::walk(float dt){
	m_rigidbody->SetLinearDamping(0);
	if (abs(m_rigidbody->GetLinearVelocity().x) != m_speed){
		if (m_rigidbody->GetLinearVelocity().y == 0){

			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength*(isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}
		else{

			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength / 2 * (isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}

	}
	if (m_rigidbody->GetLinearVelocity().x > m_speed){
		walk(b2Vec2(m_speed, m_rigidbody->GetLinearVelocity().y));
	}
	else if (m_rigidbody->GetLinearVelocity().x < m_speed*-1){
		walk(b2Vec2(-m_speed, m_rigidbody->GetLinearVelocity().y));
	}
}


void Character::moveUp(float dt){
	m_rigidbody->SetLinearDamping(0);
	if (m_rigidbody->GetLinearVelocity().y < m_speed){

		m_rigidbody->ApplyForceToCenter(b2Vec2(0, m_strength * dt * 60.0f), true);
	}
	if (m_rigidbody->GetLinearVelocity().y > m_speed){
		walk(b2Vec2(m_rigidbody->GetLinearVelocity().x, m_speed));
	}
}


void Character::moveDown(float dt){
	m_rigidbody->SetLinearDamping(0);
	if (m_rigidbody->GetLinearVelocity().y > -m_speed){

		m_rigidbody->ApplyForceToCenter(b2Vec2(0, -m_strength * dt * 60.0f), true);
	}
	if (m_rigidbody->GetLinearVelocity().y < -m_speed){
		walk(b2Vec2(m_rigidbody->GetLinearVelocity().x, -m_speed));
	}
}

void Character::moveTo(float x, float y, float time){
	m_moveTo = true;
	m_moveToPos = Vector2(x, y);
	m_moveToTime = time;
}


//impulse is the force to use for this jump
void Character::jump(float impulse){
	m_rigidbody->SetLinearDamping(0);
	impulse = fmin(impulse, m_strength/1.7f);
	if (m_rigidbody->GetLinearVelocity().y == 0){
		m_rigidbody->ApplyLinearImpulse(b2Vec2(0, impulse), m_rigidbody->GetLocalCenter(), true);
	}
}

void Character::jump(){
	jump(m_strength/1.7f);
}

void Character::renderCharacter(){
	
	UINT stride = sizeof(CrusadeVertexTypes::TexturedVertex);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedVB;
	//This maps m_VB.Get() to mappedVB according to the middle parameters (mappedVB is an out parameter)
	D3DManager::Instance()->getContext()->Map(m_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);

	//Get a pointer to the subresource data so we can make the necessary changes
	CrusadeVertexTypes::TexturedVertex *v = reinterpret_cast<CrusadeVertexTypes::TexturedVertex*>(mappedVB.pData);

	//Set the subresource data how we want
	v[0].Pos = Vector3(getRenderPosition().x, getRenderPosition().y, .1f);
	v[0].Rotation = getRotation();
	v[0].Size = getRenderSize();
	v[0].TexIndex = getBodyTexIndex();
	v[0].Color = getColor();

	D3DManager::Instance()->getContext()->Unmap(m_VB.Get(), 0);

	D3DManager::Instance()->getContext()->IASetInputLayout(InputLayouts::TexturedVertexLayout);
	D3DManager::Instance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3DManager::Instance()->getContext()->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &stride, &offset);
	CrusadeEffects::TextureFX->SetDiffuseMaps(m_texArray.Get());
	CrusadeEffects::TextureFX->SetColorCorrect(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->Apply(0, D3DManager::Instance()->getContext());

	D3DManager::Instance()->getContext()->Draw(1, 0);
}

void Character::loadTextures(std::tr2::sys::path anim_path){
	using namespace std;
	using namespace std::tr2::sys;
	//anim_path = m_relativePathAdjust / anim_path;
	path idle = anim_path;
	path walk = anim_path;
	path jump = anim_path;
	path death = anim_path;
	path cast = anim_path;
	path slash = anim_path;

	// the /= operator is overloaded to add the string on to the path with a prepended '/'
	idle /= "ANIM_IDLE";
	walk /= "ANIM_WALK";
	jump /= "ANIM_JUMP";
	death /= "ANIM_DEATH";
	cast /= "ANIM_CAST";
	slash /= "ANIM_SLASH";
	std::vector<wchar_t*> wPaths;
	int count = 1;
	int start = 1;

	// LOAD IDLE ANIMATION
	for (directory_iterator i(idle), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}

	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD WALK ANIMATION
	for (directory_iterator i(walk), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD JUMP ANIMATION
	for (directory_iterator i(jump), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD DEATH ANIMATION
	for (directory_iterator i(death), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD CAST ANIMATION
	for (directory_iterator i(cast), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	// LOAD SLASH ANIMATION
	for (directory_iterator i(slash), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".png") == 0)
		{
			count++;
			CrusadeUtil::debug(to_string(count));
			wchar_t *buffer;
			CrusadeUtil::s_to_ws(i->path(), &buffer);
			wPaths.insert(wPaths.end(), buffer);
		}
		else{
			CrusadeUtil::debug("\nNot a png: ");
			CrusadeUtil::debug(i->path());
		}
	}
	m_indexSizeVector.insert(m_indexSizeVector.end(), std::pair<UINT, UINT>(start, count - start));
	start = count;

	m_texPaths = wPaths;

	ID3D11ShaderResourceView *tempView;
	// offset of one for extra buffer verts that wont be rendered
	TextureLoader::LoadSeqTextureArray(D3DManager::Instance()->getDevice(), D3DManager::Instance()->getContext(), wPaths, 1, &tempView);

	m_texArray = tempView;

	//CrusadeUtil::debug(to_string(wPaths.size()));


}

Character* Character::clone(){
	Character* clone = new Character(getPosition(), m_rotation);
	b2BodyDef bodyDef;
	bodyDef.position.Set(m_hitBoxSize.x / 2, m_hitBoxSize.y / 2);
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	clone->initRigidbody(bodyDef);
	return clone;
}

void Character::step(float dt){
	//Do anything that needs to happen every game loop
	if (m_rigidbody->GetLinearVelocity().x != 0){
		//m_currAnim = ANIM_WALK;
		m_bodyAnim = ANIM_WALK;
	}

	if (m_rigidbody->GetLinearVelocity().y < -0.1f || m_rigidbody->GetLinearVelocity().y > 0.1f)
		m_bodyAnim = ANIM_JUMP;
	if (m_rigidbody->GetLinearVelocity().x == 0	&& m_rigidbody->GetLinearVelocity().y == 0)
		m_bodyAnim = ANIM_IDLE;

	b2Vec2 vel = m_rigidbody->GetLinearVelocity();
	if (vel.y < 0 && getPosition().y < -1){
		damage((int)(fabs(vel.y)*.8));//Damage equals .8 times the speed of descent (cost to jump back onto stage)
		m_rigidbody->SetLinearVelocity(b2Vec2(vel.x, fabs(vel.y) + 2));
	}
	//DEATH
	if (m_hp <= 0 && !m_moveTo && !m_dead){	// invulnerable during moveto
		b2Vec2 pos = m_rigidbody->GetPosition();
		//Blow up in blood particles
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.05f, .05f), Vector2(0.0f, 100.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.05f, .075f), Vector2(50.0f, 50.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.075f, .05f), Vector2(-50.0f, 50.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.08f, .05f), Vector2(25.0f, 150.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.075f, .05f), Vector2(100.0f, 50.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.05f, .075f), Vector2(-100.0f, 50.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.05f, .05f), Vector2(0.0f, 400.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.05f, .075f), Vector2(200.0f, 300.0f), PT_DEATHGORE);
		ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.075f, .05f), Vector2(-200.0f, 300.0f), PT_DEATHGORE);
		m_dead = true;
		setEnableCollisionWith(Box2DSingleton::NONE);
		//ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.1, .05), Vector2(0.0f, 10.0f), PT_HOLYSHOWER);
		//ParticleManager::Instance()->EmitParticles(Vector2(pos.x, pos.y), Vector2(.1, .05), Vector2(0.0f, 100.0f), PT_EXPBURST);
		//setPosition(m_spawnPoint);
		//m_rigidbody->SetLinearVelocity(b2Vec2_zero);
		//m_hp = m_hpmax;
	}
	
	//Mana Regen
	if (m_mana < m_manaMax){
		m_manaTimer += dt;
		if (m_manaTimer >= .6f){
			m_mana += m_profile->getLevelLookUp()->getManaRegen(m_profile->getLevel());
			m_manaTimer -= .6f;
			if (m_mana >= m_manaMax){
				m_manaTimer = 0.0f;
			}
		}
	}

	if (m_moveTo){
		float xdisPerSec = (m_moveToPos.x - getPosition().x) / m_moveToTime;
		float ydisPerSec = (m_moveToPos.y - getPosition().y) / m_moveToTime;

		if (xdisPerSec > 0.05f){
			setFacingRight(true);
		}
		else
		{
			setFacingRight(false);
		}
		if (m_canFly)
			walk(b2Vec2(xdisPerSec, ydisPerSec));
		else
			walk(b2Vec2(xdisPerSec, m_rigidbody->GetLinearVelocity().y));
		if (ydisPerSec > 0.1f){
			jump();
		}

		m_moveToTime -= dt;
		if (m_moveToTime <= 0){
			m_moveTo = false;
			m_rigidbody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}

	}

	stepAnimation(dt);
	updateCooldownTimers(dt);
}

void Character::createBuffers(){
	CrusadeVertexTypes::TexturedVertex temp;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(CrusadeVertexTypes::TexturedVertex);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &temp;

	DX::ThrowIfFailed(D3DManager::Instance()->getDevice()->CreateBuffer(&vbd, &vinitData, m_VB.ReleaseAndGetAddressOf()));
}

void Character::BeginContact(b2Contact* contact, GameObject* other){

}

void Character::EndContact(b2Contact* contact, GameObject* other){

}

/*Set the cooldown duration of ability represented by index:
 z - 0
 x - 1
 c - 2
 v - 3*/
void Character::setCooldown(int index, float duration){
	m_cooldowns[index] = duration;
}

/*Set the cooldown timer of ability represented by index. This method should be used in special cirumstances such as disabling abilities for a duration. Use startCooldownTimer(int index) for typical purposes.
z - 0
x - 1
c - 2
v - 3*/
void Character::setCooldownTimer(int index, float duration){
	m_cooldownTimers[index] = duration;
}

/*Start the cooldown timer of ability represented by index.
z - 0
x - 1
c - 2
v - 3*/
void Character::startCooldownTimer(int index){
	m_cooldownTimers[index] = m_cooldowns[index];
}

/*Get the cooldown duration of ability represented by index.
z - 0
x - 1
c - 2
v - 3*/
float Character::getCooldown(int index){
	return m_cooldowns[index];
}

/*Get the time left for the cooldown timer of ability represented by index.
z - 0
x - 1
c - 2
v - 3*/
float Character::getCooldownTimer(int index){
	return m_cooldownTimers[index];
}

/*Update the cooldown timers so that they countdown to zero.*/
void Character::updateCooldownTimers(float dt){
	for (int x = 0; x < 4; x++){
		m_cooldownTimers[x] = fmaxf(m_cooldownTimers[x] - dt, 0);
	}
}

/*Get the active ability at index:
 z - 0
 x - 1
 c - 2
 v - 3*/
int Character::getActiveAbility(int index){
	return m_activeAbilities[index];
}

/*Get the Player's chosen passive for ability with ID = id.*/
int Character::getPassiveForAbility(UINT id){
	for (int x = 0; x < 4; x++){
		if (m_activeAbilities[x] == id){
			return m_passiveAbilities[x];
		}
	}
	return -1;//We don't actually have this ability
}

/*Allows changing of the selected passive.
 Indecies
 z - 0
 x - 1
 c - 2
 v - 3

 Passives
 <=0 - no passive
 1 - passive1
 2 - passive2
 >=3 - passive3+ or no passive*/ 
void Character::setPassive(int index, int passive){
	m_passiveAbilities[index] = passive;
}

/*Get the passive at index
 z - 0
 x - 1
 c - 2
 v - 3*/
int Character::getPassive(int index){
	return m_passiveAbilities[index];
}

/*Use thios to bind abilities to different keys.
z - 0
x - 1
c - 2
v - 3*/
void Character::bindAbility(int index, UINT abilityID){
	m_activeAbilities[index] = abilityID;
}

UINT Character::getAnimSize(UINT id, Character::ANIMATION_TYPE type){
	return m_indexSizeVector.at(type).second;
}

UINT Character::getAnimIndex(UINT id, Character::ANIMATION_TYPE type){
	return m_indexSizeVector.at(type).first;
}

Ability* Character::castAbility(UINT id){
	if (id == 0){
		return nullptr;
	}
	Ability* ptr = nullptr;
	UINT cost = AbilityManager::Instance()->getManaCost(id, this);
	if ((int)cost <= m_mana || m_godMode){
		if (!m_godMode){
			m_mana -= cost;
		}
		m_timer = 0;
		ptr = GameObject::castAbility(id);
		if (ptr->getAbilityDesc().castAnim.compare("NO_ANIM") == 0){
			
		}
		else if (ptr->getAbilityDesc().castAnim.compare("SLASH") == 0){
			m_armAnim = ANIM_SLASH;
		}
		else{
			m_armAnim = ANIM_CAST;
		}
	}
	return ptr;
}

void Character::enableFlight(){
	m_canFly = true;
	m_rigidbody->SetGravityScale(0);
	m_rigidbody->SetLinearDamping(1);
}

void Character::disableFlight(){
	m_canFly = false;
	m_rigidbody->SetGravityScale(1.3f);
	m_rigidbody->SetLinearDamping(0);
}

bool Character::canFly(){
	return m_canFly;
}

void Character::damage(int damage){
	if (!m_godMode && !m_dead){
		GameObject::damage(damage);
		Vector2 damagePos = Vector2(getRenderPosition().x + (isFacingRight() ? getHitboxSize().x / 2 : (getHitboxSize().x / 2) * -1), getRenderPosition().y + getHitboxSize().y / 2);
		ParticleManager::Instance()->EmitParticles(damagePos, Vector2(0.05f, 0.05f), Vector2(0.0f, 0.0f), PT_DEATHGORE);
		b2Vec2 pos = m_rigidbody->GetPosition();
		
		AudioManager::Instance()->PlaySound("player_hit.adpcm");
	}
}

void Character::heal(int health){
	GameObject::heal(health);
}

bool Character::save(){
	return m_profile->save();
}

Profile* Character::getProfile(){
	return m_profile;
}

void Character::toggleGodMode(){
	m_godMode = !m_godMode;
	if (m_godMode){
		setColor(Vector4(1.0f,.8f,0.0f,1.0f));
	}
	else{
		setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

bool Character::getGodMode(){
	return m_godMode;
}

bool Character::isDead(){
	return m_dead;
}