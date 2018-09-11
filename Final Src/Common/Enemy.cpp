#include "pch.h"
#include "Enemy.h"
#include "ParticleManager.h"

const std::string Enemy::MELEE = "melee";
const std::string Enemy::RANGED = "ranged";
const std::string Enemy::MAGE = "mage";

//Used by EnemyFactory to generate the Master Objects for 
Enemy::Enemy(std::string tag, std::string name, Vector2 position, Vector2 textureSize, Vector2 hitBoxSize, Vector2 hitBoxOffset, float rotation, int id, int hp, float speed, float sight, float hearing, float massDensity, float attackMultiplier, float strength, float jumpStrength, int courage, int exp, std::string type, UINT aiFlags, std::string abilities, SoundDescription sounds)
	: GameObject(tag, name, position, textureSize, hitBoxSize, hitBoxOffset, hp, rotation, true){
	this->m_id = id;
	this->m_exp = exp;
	this->m_type = type;
	this->m_state = AI_IDLE;
	this->m_speed = speed;
	this->m_sight = sight;
	this->m_hearing = hearing;
	this->m_massDensity = massDensity;
	this->m_strength = strength;
	this->m_jumpStrength = jumpStrength;
	this->m_courage = courage;
	this->m_aiFlags = aiFlags;
	this->m_currAnim = ANIM_IDLE;
	this->jumping = false;
	this->m_dead = false;
	this->m_attackMultiplier = attackMultiplier;
	this->m_sounds = sounds;
	this->m_texIndex = 1;

	this->m_moveTo = false;

	this->m_blood = nullptr;

	this->m_containingTrigger = 0;

	for (int x = 0; x < 6; x++){
		m_actionTimers[x] = 0;
	}
	m_aiStateTimer[0] = 5;
	m_aiStateTimer[1] = 5;
	m_rigidbody = NULL;
	this->m_playerInSight = false;
	this->m_hearPlayer = false;
	this->m_player = nullptr;
	for (int x = 0; x < 4; x++){
		m_cooldownTimers[x] = 0;
		m_activeAbilities[x] = 0;
	}
	setAbilityCSV(abilities);
	
	determineWallBreakAbility();
}

/*Parse a comma separated string of aiFlags*/
UINT Enemy::parseAIFlags(string flagsCSV){
	vector<string> flags = CrusadeUtil::split(CrusadeUtil::toLowerCase(flagsCSV), ',');
	UINT aiFlags = 0;
	for (string flag: flags){
		flag.erase(remove_if(flag.begin(), flag.end(), isspace), flag.end());
		if (!flag.compare("roam")){
			aiFlags |= ROAM;
		}
		else if (!flag.compare("quick_response")){
			aiFlags |= QUICK_RESPONSE;
		}
		else if (!flag.compare("trigger_happy")){
			aiFlags |= TRIGGER_HAPPY;
		}
		else if (!flag.compare("peaceful")){
			aiFlags |= PEACEFUL;
		}
			
	}
	return aiFlags;
}

void Enemy::setAbilityCSV(std::string abilities){
	m_abilityCsv = abilities;
	m_abilityCsv.erase(remove_if(m_abilityCsv.begin(), m_abilityCsv.end(), isspace), m_abilityCsv.end());//we don't want spaces corrupting data
	if (!parseAbilityCSV()){
		CrusadeUtil::debug("Hey Boss, we got an error while parsing an enemy's CSV!");
	}
}

/*Parses a string of the form (<AbilityID>, <PassiveID>, <Cooldown>, <CastChance>); (<AbilityID>, <PassiveID>, <Cooldown>, <CastChance>); ...*/
std::string Enemy::getAbilities(){
	return m_abilityCsv;
}

/*Parses the abilities defined in m_abilityCsv. Automatically called when you call setAbilityCSV(string). Returns true if everything parsed correctly, false if data wasn't correct.*/
bool Enemy::parseAbilityCSV(){
	if (m_abilityCsv.compare("null") == 0){
		//No abilities for this enemy apparently. What's up with that?
		return true;
	}
	vector<string> abilities = CrusadeUtil::split(CrusadeUtil::toLowerCase(m_abilityCsv), ';');
	int x = 0;
	m_fastestAbilityCooldown = 10;
	for (string abilityQuad : abilities){
		abilityQuad = abilityQuad.substr(1, abilityQuad.length() - 2);
		vector<string> ability = CrusadeUtil::split(abilityQuad, ',');
		if (ability.size() != 4){
			return false;
		}
		m_activeAbilities[x] = stoi(ability[0]);
		m_passiveAbilities[x] = stoi(ability[1]);
		m_cooldowns[x] = stof(ability[2]);
		if (m_cooldowns[x] < m_fastestAbilityCooldown){
			m_fastestAbilityCooldown = fmax(m_cooldowns[x], .025f);
		}
		m_abilityChance[x] = stof(ability[3]);
		x++;
	}
	for (; x < 4; x++){
		m_activeAbilities[x] = 0;
		m_passiveAbilities[x] = 0;
		m_cooldowns[x] = 0;
		m_abilityChance[x] = 0;
	}
	return true;
}

/*A sound should be defined as a set of ordered pairs delimited by semicolons.
 Example: (ogre_ow_1, .333); (ogre_ow_2, .333); (ogre_ow_3, .334)*/
map<float, string> Enemy::parseSounds(string csv){
	map<float, string> sounds;
	if (csv.length() < 8){
		sounds.emplace(1.0f, "NO AUDIO");
		return sounds;
	}
	csv.erase(remove_if(csv.begin(), csv.end(), isspace), csv.end());
	vector<string> pairs = CrusadeUtil::split(csv, ';');
	for (string s : pairs){
		s = s.substr(1, s.length() - 2);
		vector<string> parts = CrusadeUtil::split(s, ',');
		sounds.emplace(stof(parts[1]), parts[0]);
	}
	return sounds;
}

Enemy::~Enemy(){
	if (m_blood){
		ParticleManager::Instance()->DeleteEmitter(m_blood);
		m_blood = nullptr;
	}
	if (m_rigidbody){
		Box2DSingleton::Instance()->getWorld()->DestroyBody(m_rigidbody);
		m_rigidbody = nullptr;
	}
}

int Enemy::getId() const{
	return m_id;
}

//Get this Enemy's EXP value. DO NOT USE TO AWARD EXP! Use Enemy::awardExp() instead (prevents double reward)
int Enemy::getExp() const{
	return m_exp;
}

//Awards exp and then sets the amount of exp this enemy can award to 0. Prevents double EXP reward.
int Enemy::awardExp(){
	int exp = m_exp;
	m_exp = 0;
	return exp;
}

std::string Enemy::getType(){
	return m_type;
}

UINT Enemy::getTexIndex() const{
	return m_texIndex;
}

void Enemy::setId(int id){
	m_id = id;
}

void Enemy::setExp(int exp){
	m_exp = exp;
}

void Enemy::setContainingTrigger(LuaTrigger *trig){
	m_containingTrigger = trig;
}

//This is the only way that position shouild be set since it will also update the rigidbody
void Enemy::setPosition(Vector2 pos){
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
}

void Enemy::moveTo(float x, float y, float time){
	m_moveTo = true;
	m_moveToPos = Vector2(x, y);
	m_moveToTime = time;
}

void Enemy::setAiFlags(UINT flags){
	m_aiFlags = flags;
}

UINT Enemy::getAiFlags(){
	return m_aiFlags;
}

bool Enemy::hasAiFlag(UINT flag){
	return (m_aiFlags & flag) > 0;
}

/*Sets the AI state of this enemy and resets the state reduction timer. Also resets the action timers*/
void Enemy::setAiState(AI_STATE state){
	m_state = state;
	m_aiStateTimer[0] = 5;//Take 5 seconds to revert back to an earlier state.
	m_aiStateTimer[1] = 5;
	for (int x = 0; x < 6; x++){
		m_actionTimers[x] = 0;
	}
}

Enemy::AI_STATE Enemy::getAiState(){
	return m_state;
}

void Enemy::aiStateReduction(float dt){
	m_aiStateTimer[0] -= dt;
	if (m_playerInSight){
		m_aiStateTimer[0] = m_aiStateTimer[1];
	}
	if (m_aiStateTimer[0] <= 0){
		switch (m_state){
		case AI_ENGAGE:
		case AI_FLEE:
		case AI_ALERT:
			setAiState(AI_AWARE);
			break;
		case AI_AWARE:
			setAiState(AI_IDLE);
			break;
		}
		m_aiStateTimer[0] = m_aiStateTimer[1];
	}
}

b2Body* Enemy::getRigidbody(){
	return m_rigidbody;
}

void Enemy::initRigidbody(b2BodyDef def){
	m_rigidbody = Box2DSingleton::Instance()->getWorld()->CreateBody(&def);
	b2PolygonShape tileBox;
	tileBox.SetAsBox(m_hitBoxSize.x / 2.0f, m_hitBoxSize.y / 2.0f);
	//Apparently fixtures are added to a stack, so GetFixtureList is LIFO. As such, the body is going to be added after the sensory things so that everything works correctly,
	
	//Add Feet (for jumping)
	

	//Add senses
	b2Filter filter;
	filter.categoryBits = Box2DSingleton::ENEMY;
	filter.maskBits = Box2DSingleton::CHARACTER;
	b2PolygonShape sight;
	b2CircleShape hearing;
	b2Vec2 triangle[3];
	triangle[0] = m_rigidbody->GetPosition();
	triangle[1].Set(m_rigidbody->GetPosition().x + m_sight, m_rigidbody->GetPosition().y * sinf(0.261799f));//We want a 30 degree field of vision, so 15 degrees each direction (must be converted to radians)
	triangle[2].Set(m_rigidbody->GetPosition().x + m_sight, m_rigidbody->GetPosition().y * sinf(0.261799f) * -1);//other direction
	sight.Set(triangle, 3);
	hearing.m_radius = m_hearing;
	b2FixtureDef eyes;
	eyes.shape = &sight;
	eyes.isSensor = true;
	eyes.userData = (void*)EYES;
	eyes.filter = filter;
	eyes.friction = 0;
	eyes.density = 0;
	b2FixtureDef ears;
	ears.shape = &hearing;
	ears.isSensor = true;
	ears.userData = (void*)EARS;
	ears.filter = filter;
	ears.friction = 0;
	ears.density = 0;
	m_rigidbody->CreateFixture(&eyes);
	m_rigidbody->CreateFixture(&ears);
	
	//Now we can add the body and everything should work.
	m_rigidbody->CreateFixture(&tileBox, 0.3f);
	m_rigidbody->GetFixtureList()->SetUserData((void*)BODY);
	m_rigidbody->GetFixtureList()->SetDensity(m_massDensity);
	m_rigidbody->SetUserData(this);
	m_rigidbody->ResetMassData();
	setEnableCollisionWith(Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER | Box2DSingleton::TRIGGER | Box2DSingleton::PHYSICAL_TRIGGER);
}

void Enemy::setEnableCollisionWith(int collideWith){
	b2Filter filter;
	filter.categoryBits = Box2DSingleton::ENEMY;
	filter.maskBits = collideWith;
	m_rigidbody->GetFixtureList()->SetFilterData(filter);
}

// based on David Saltares "Fix your timestep in Libgdx and Box2D" interpolation function
// http://saltares.com/blog/games/fixing-your-timestep-in-libgdx-and-box2d/
void Enemy::interpolate(float alpha){
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
	if (m_blood){
		Vector2 bodyCenter = Vector2(position.x + (m_textureSize.x / 2) + (isFacingRight() ? 1 : -1)*m_hitBoxOffset.x, position.y + (m_textureSize.y / 2) + m_hitBoxOffset.y);
		m_blood->setPosition(bodyCenter);
		b2PolygonShape* eyes = (b2PolygonShape*)m_rigidbody->GetFixtureList()->GetNext()->GetNext()->GetShape();
		/*See where the enemy's eyes are.
		b2Vec2 vertex = eyes->GetVertex(1);
		vertex = m_rigidbody->GetWorldPoint(vertex);
		m_blood->setPosition(Vector2(vertex.x, vertex.y));*/
	}
	//reflect cone of vision as necessary
	b2PolygonShape* sight = (b2PolygonShape*)m_rigidbody->GetFixtureList()->GetNext()->GetNext()->GetShape();//the third fixture is the sight fixture
	if (isFacingRight() && sight->m_vertices[0].x < 0){
		sight->m_vertices[0].Set(sight->m_vertices[0].x * -1, sight->m_vertices[0].y);
		sight->m_vertices[1].Set(sight->m_vertices[1].x * -1, sight->m_vertices[1].y);
	}
	if(!isFacingRight() && sight->m_vertices[1].x > 0){
		sight->m_vertices[0].Set(sight->m_vertices[0].x * -1, sight->m_vertices[0].y);
		sight->m_vertices[1].Set(sight->m_vertices[1].x * -1, sight->m_vertices[1].y);
	}

}

void Enemy::setFacingRight(bool direction){
	facingRight = direction;
	if (m_prevFacingRight && !facingRight){
		GameObject::setPosition(Vector2(getPosition().x + (m_hitBoxOffset.x*2), getPosition().y));
	}
	if (!m_prevFacingRight && facingRight){
		GameObject::setPosition(Vector2(getPosition().x - (m_hitBoxOffset.x*2), getPosition().y));
	}
	m_prevFacingRight = direction;
}

//Allow sorting of Enemies based off of ID
bool Enemy::operator<(Enemy const& e){
	return m_id < e.getId();
}

int Enemy::checkHP(){
	if (m_hp <= 0){
		m_currAnim = ANIM_DEATH;
		m_state = AI_DEATH;
	}
	return m_hp;
}

bool Enemy::isDead(){
	return m_dead;
}

void Enemy::stepAnimation(float dt){
	if (m_currAnim != ANIM_DEATH && m_currAnim != ANIM_CAST && m_currAnim != ANIM_SLASH){
		if (m_rigidbody->GetLinearVelocity().x != 0)
			m_currAnim = ANIM_WALK;
		if (m_rigidbody->GetLinearVelocity().x == 0
			&& m_rigidbody->GetLinearVelocity().y == 0)
			m_currAnim = ANIM_IDLE;
	}

	EnemyManager *man = EnemyManager::Instance();
	switch (m_currAnim){
	case ANIM_IDLE:
		m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_IDLE), man->getAnimSize(m_id, ANIM_IDLE), .5f);
		break;
	case ANIM_WALK:
		m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_WALK), man->getAnimSize(m_id, ANIM_WALK), .2f);
		break;
	case ANIM_JUMP:
		if (!isAnimationOver(dt, man->getAnimSize(m_id, ANIM_JUMP), .1f))
			m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_JUMP), man->getAnimSize(m_id, ANIM_JUMP), .5f);
		else
			m_texIndex = man->getAnimIndex(m_id, ANIM_JUMP);
		break;
	case ANIM_DEATH:
		if (!isAnimationOver(dt, man->getAnimSize(m_id, ANIM_DEATH), .1f))
		{
			m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_DEATH), man->getAnimSize(m_id, ANIM_DEATH), .2f);
		}
		else
		{
			m_dead = true;
			b2Vec2 b2 = m_rigidbody->GetPosition();
			ParticleManager::Instance()->EmitParticles(Vector2(b2.x, b2.y), Vector2(0.05f, 0.05f), Vector2(0.0f, 0.0f), PT_DEATHGORE);
		}
		break;
	case ANIM_CAST:
		if (!isAnimationOver(dt, man->getAnimSize(m_id, ANIM_CAST), .25f))
			m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_CAST), man->getAnimSize(m_id, ANIM_CAST), .25f);
		else
			m_currAnim = ANIM_JUMP;
		break;
	case ANIM_SLASH:
		if (!isAnimationOver(dt, man->getAnimSize(m_id, ANIM_SLASH), .2f))
			m_texIndex = getAnimationFrameIndex(dt, man->getAnimIndex(m_id, ANIM_SLASH), man->getAnimSize(m_id, ANIM_SLASH), .2f);
		else
			m_currAnim = ANIM_JUMP;
		break;
	}
}

void Enemy::damage(int damage){
	GameObject::damage(damage);
	if (!m_blood)
	{
		float ratio = ((float)m_hp / (float)m_hpmax);
		m_blood = ParticleManager::Instance()->CreateEmitter(Vector2(getPosition().x, getPosition().y), Vector2(.04f, .04f), Vector2(0.0f, 0.0f), PT_BLOOD, ratio * 0.5f);
	}
	else
	{
		float ratio = ((float)m_hp / (float)m_hpmax);
		m_blood->setInterval(ratio * 0.5f);
	}

	CSoundEffectInstance *sound = nullptr;
	float percentile = ((float)(rand() % 1000)) / 1000.0f;//allow precision down to .1%
	auto it = m_sounds.hitSounds.begin();
	if (it->second.compare("NO AUDIO")==0){
		sound = AudioManager::Instance()->PlaySound("player_hit.adpcm");
		sound->SetPitch(.5f);
		return;
	}
	while (percentile > it->first){
		it++;
		if (it == m_sounds.hitSounds.end()){
			sound = AudioManager::Instance()->PlaySound("player_hit.adpcm");
			sound->SetPitch(.5f);
			break;
		}
	}
	if (it != m_sounds.hitSounds.end()){
		sound = AudioManager::Instance()->PlaySound(it->second);
	}
	

}


/*MOVEMENT FUNCTIONS*/

/*Switch wich way the enemy is facing*/
void Enemy::fidget(){
	setFacingRight(!isFacingRight());
}

/*This sets the linear velocity to velocity in the direction that the Enemy is facing
 Use Enemy::walk() to do realistic walking
 velocity should be m_strength*(isFacingRight() ? 1 : -1)
 NOTE: The Enemy will be affected by friction, thus slowing it down*/
void Enemy::walk(float xvel, float yvel){
	m_rigidbody->SetLinearVelocity(b2Vec2(xvel, yvel));
	m_rigidbody->SetAwake(true);
}

/*A walk that actually uses physics*/
void Enemy::walk(float dt){
	if (abs(m_rigidbody->GetLinearVelocity().x) != m_speed){
		if (m_rigidbody->GetLinearVelocity().y == 0){
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength*(isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}
		else{
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength/2*(isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}
		
	}
	if (m_rigidbody->GetLinearVelocity().x > m_speed){
		walk(m_speed, m_rigidbody->GetLinearVelocity().y);
	}
	else if (m_rigidbody->GetLinearVelocity().x < m_speed*-1){
		walk(m_speed*-1, m_rigidbody->GetLinearVelocity().y);
	}
}
/*Walk in reverse direction. Max speed is speed*.8*/
void Enemy::walkBackwards(float dt){
	if (abs(m_rigidbody->GetLinearVelocity().x) != m_speed*.8){
		if (m_rigidbody->GetLinearVelocity().y == 0){
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength*(isFacingRight() ? -1 : 1) * dt * 60.0f, 0), true);
		}
		else{
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength / 2 * (isFacingRight() ? -1 : 1)  * dt * 60.0f, 0), true);
		}

	}
	if (m_rigidbody->GetLinearVelocity().x > m_speed*.8){
		walk(m_speed*.8f, m_rigidbody->GetLinearVelocity().y);
	}
	else if (m_rigidbody->GetLinearVelocity().x < m_speed*.8*-1){
		walk(m_speed*.8f*-1, m_rigidbody->GetLinearVelocity().y);
	}
}

void Enemy::run(float dt){
	if (abs(m_rigidbody->GetLinearVelocity().x) != m_speed*1.75){
		if (m_rigidbody->GetLinearVelocity().y == 0){
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength*(isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}
		else{
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength / 1.25f * (isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}

	}
	if (m_rigidbody->GetLinearVelocity().x > m_speed*1.75){
		walk(m_speed*1.75f, m_rigidbody->GetLinearVelocity().y);
	}
	else if (m_rigidbody->GetLinearVelocity().x < m_speed*-1){
		walk(m_speed*-1.75f, m_rigidbody->GetLinearVelocity().y);
	}
}

//slowly walk around. Jump if necessary.
void Enemy::roam(float dt){
	m_currAnim = ANIM_WALK;
	if (rand() % 100 == 1){
		fidget();
	}
	handleObstacles();
	if (abs(m_rigidbody->GetLinearVelocity().x) != m_speed*.6f){
		if (m_rigidbody->GetLinearVelocity().y == 0){
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength*(isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}
		else{
			m_rigidbody->ApplyForceToCenter(b2Vec2(m_strength / 2 * (isFacingRight() ? 1 : -1) * dt * 60.0f, 0), true);
		}

	}
	if (m_rigidbody->GetLinearVelocity().x > m_speed * .6f){
		walk(m_speed * .6f, m_rigidbody->GetLinearVelocity().y);
	}
	else if (m_rigidbody->GetLinearVelocity().x < m_speed*-.6f){
		walk(m_speed * -.6f, m_rigidbody->GetLinearVelocity().y);
	}
}

void Enemy::halt(){
	if (m_rigidbody->GetLinearVelocity().y == 0){
		m_rigidbody->SetLinearVelocity(b2Vec2(0, 0));
	}
}

//impulse is the force to use for this jump
void Enemy::jump(float impulse){
	impulse = fmin(impulse, m_jumpStrength);
	if (m_rigidbody->GetLinearVelocity().y == 0){
		m_rigidbody->ApplyLinearImpulse(b2Vec2(0, impulse), m_rigidbody->GetLocalCenter(), true);
	}
}

void Enemy::jump(){
	jump(m_jumpStrength);
}

/*Compare my velocity with the velocity of other*/
b2Vec2 Enemy::compareVelociy(b2Body* other){
	return b2Vec2(m_rigidbody->GetLinearVelocity().x - other->GetLinearVelocity().x, m_rigidbody->GetLinearVelocity().y - other->GetLinearVelocity().y);;
}

/*Determine if we should jump, try to smash through with abilities, or turn around.*/
void Enemy::handleObstacles(){
	if (shouldIJump()){//Check to see if there is anything in the way and set jump destination/target obect
		if (m_jDestination.y - getPosition().y <= 2.5){ // can we make a jump? 2.5 should be replaced by a calculation of this enemy's max height
			jump();
		}
		else if(m_wallBreakIndex > -1){//Smash through!
			if (m_cooldownTimers[m_wallBreakIndex] <= 0){
				castAbility(m_activeAbilities[m_wallBreakIndex]);
				m_cooldownTimers[m_wallBreakIndex] = m_cooldowns[m_wallBreakIndex];
			}
		}
		else{//Turn around
			setFacingRight(!isFacingRight());
		}
	}
}

/*Determine if we should jump here*/
bool Enemy::shouldIJump(){
	if (m_rigidbody->GetLinearVelocity().y == 0){
		m_jTarget = nullptr;//reset so that we don't have old data
		b2Transform transform;
		transform.SetIdentity();
		b2RayCastInput in;
		b2RayCastOutput out;
		b2Vec2 pos = b2Vec2(m_rigidbody->GetPosition().x + ((m_hitBoxSize.x/2.0f)*(isFacingRight() ? 1 : -1)), m_rigidbody->GetPosition().y);
		in.maxFraction = 1;
		RayCastClosestCallbackIgnoreMe callback = RayCastClosestCallbackIgnoreMe(this);
		in.p1 = b2Vec2(pos.x, pos.y-(m_hitBoxSize.y/2.0f));
		in.p2 = b2Vec2(in.p1.x + 2.5f * (isFacingRight() ? 1 : -1), in.p1.y);
		Box2DSingleton::Instance()->getWorld()->RayCast(&callback, in.p1, in.p2);
		bool bottom = callback.m_hit;
		//b2Body* other = (callback.m_body->GetType() == b2BodyType::b2_dynamicBody) ? callback.m_body : nullptr; //don't waste time thinking about non-dynamic bodies
		b2Body* other = callback.m_body;
		in.p1 = pos;
		Box2DSingleton::Instance()->getWorld()->RayCast(&callback, in.p1, in.p2);
		bool mid = callback.m_hit;
		//other = (callback.m_body->GetType() == b2BodyType::b2_dynamicBody) ? callback.m_body : other; //Only think about my position relative to one other dynamic body per cycle
		other = callback.m_body;
		in.p1 = b2Vec2(pos.x, pos.y + (m_hitBoxSize.y / 2.0f));
		Box2DSingleton::Instance()->getWorld()->RayCast(&callback, in.p1, in.p2);
		bool top = callback.m_hit;
		//other = (callback.m_body->GetType() == b2BodyType::b2_dynamicBody) ? callback.m_body : other;
		other = callback.m_body;
		if ((bottom | mid | top) && other != nullptr){
			if (((GameObject*)other->GetUserData())->getTag().compare("ENEMY") == 0){//try to avoid other enemies if necessary
				if (compareVelociy(other).x > 0.5f){//only jump if we are moving faster than they are
					m_jTarget = other;
					m_jDestination = findJumpDestination(other);
					return true;
				}
				else{
					return false;
				}
			}
			m_jTarget = other;
			m_jDestination = findJumpDestination(other);
		}
		return bottom|mid|top;
	}
	return false;
}

/*How many steps it will take for the Enemy to reach the x coordinate specified at the current speed*/
int Enemy::stepsToX(float x){
	if (m_rigidbody->GetLinearVelocity().x == 0){
		return 0;
	}
	float timeStep = Box2DSingleton::Instance()->getTimeStep();
	int n = max((int)ceil((x - m_rigidbody->GetPosition().x/(m_rigidbody->GetLinearVelocity().x))), 1);
	return n;
}

/*Calculate the velocity needed to reach the desired height*/
float Enemy::calculateVerticalVelocityForHeight(float desiredHeight)
{
	if (desiredHeight <= 0)
		return 0; //wanna go down? just let it drop

	//gravity is given per second but we want time step values here
	float t = Box2DSingleton::Instance()->getTimeStep();
	b2Vec2 stepGravity = t * t * Box2DSingleton::Instance()->getWorld()->GetGravity(); // m/s/s

	//quadratic equation setup (ax² + bx + c = 0)
	float a = 0.5f / stepGravity.y;
	float b = 0.5f;
	float c = desiredHeight;

	//check both possible solutions
	float quadraticSolution1 = (-b - b2Sqrt(b*b - 4 * a*c)) / (2 * a);
	float quadraticSolution2 = (-b + b2Sqrt(b*b - 4 * a*c)) / (2 * a);

	//use the one which is positive
	float v = quadraticSolution1;
	if (v < 0)
		v = quadraticSolution2;

	//convert answer back to seconds
	return v * 60.0f;
}

/*Target is the body that we are trying to jump on top of*/
Vector2 Enemy::findJumpDestination(b2Body* target){
	GameObject* object = (GameObject*)target->GetUserData();
	if (object != nullptr){
		if (object->getTag().compare("WORLDTILE") == 0){
			WorldTile* tile = (WorldTile*)object;
			if (tile->getId() == 0){
				return Vector2(target->GetPosition().x, target->GetPosition().y);
			}
			else{
				WorldTileMatrix* wtm = WorldTileMatrix::Instance();
				int indexOfTarget = (int)(tile->getPosition().x + tile->getPosition().y*wtm->getWidth() + wtm->getHeight()*wtm->getWidth()*wtm->getActiveLayer());
				int yOffset = 0;
				do{
					yOffset += wtm->getWidth();

				} while (wtm->m_worldTileArr[indexOfTarget + yOffset].getId() != 0);//Count until we find an air tile
				yOffset /= wtm->getWidth();//get the number of tiles up to offset
				return Vector2(target->GetPosition().x, target->GetPosition().y + tile->getHitboxSize().y / 2.0f + yOffset + m_hitBoxSize.y / 2.0f);//get the location of the top of the block to jump to.
			}

		}
		else{
			return Vector2(target->GetPosition().x, target->GetPosition().y + object->getHitboxSize().y/2.0f + m_hitBoxSize.y / 2.0f);
		}
	}
	return 	Vector2(target->GetPosition().x, target->GetPosition().y + m_hitBoxSize.y / 2.0f);
}

void Enemy::BeginContact(b2Contact* contact, GameObject* other){
	if (!(contact->GetFixtureA()->GetFilterData().maskBits & contact->GetFixtureB()->GetFilterData().categoryBits) != 0 && (contact->GetFixtureA()->GetFilterData().categoryBits & contact->GetFixtureB()->GetFilterData().maskBits) != 0){
		return;
	}
	GameObject* objA = nullptr;
	GameObject* objB = nullptr;
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
	int fixtureDataA = (int)contact->GetFixtureA()->GetUserData();
	int fixtureDataB = (int)contact->GetFixtureB()->GetUserData();
	int x = 0;
	//Do some AI things
	if (bodyUserDataA){
		objA = static_cast<GameObject*>(bodyUserDataA);
		if (objA->getTag().compare("ENEMY") == 0){
			switch (fixtureDataA){
			case EYES:
				//userDataB should be player at this point
				m_player = (Character*)contact->GetFixtureB()->GetBody()->GetUserData();
				if (lookForPlayer()>=0){
					m_playerInSight = true;
					if (m_state == AI_ALERT){
						m_state = AI_ENGAGE;
					}
					else{
						m_state = AI_ALERT;
					}
				}
				break;
			case EARS:
				m_player = (Character*)contact->GetFixtureB()->GetBody()->GetUserData();
				m_hearPlayer = true;
				if (contact->GetFixtureB()->GetBody()->GetPosition().x > getPosition().x){
					setFacingRight(true);
				}
				else if (contact->GetFixtureB()->GetBody()->GetPosition().x < getPosition().x){
					setFacingRight(false);
				}
				break;
			}
		}
	}		
	if (bodyUserDataB){
		objB = static_cast<GameObject*>(bodyUserDataB);
		if (objB->getTag().compare("ENEMY") == 0){
			switch (fixtureDataB){
			case EYES:
				if (lookForPlayer()>0){
					m_playerInSight = true;
					/*if (m_state == AI_ALERT){
						m_state = AI_ENGAGE;
					}
					else{
						m_state = AI_ALERT;
					}*/
				}
				break;
			case EARS:
				m_player = (Character*)contact->GetFixtureA()->GetBody()->GetUserData();
				m_hearPlayer = true;
				if (contact->GetFixtureA()->GetBody()->GetPosition().x > m_rigidbody->GetPosition().x){
					setFacingRight(true);
				}
				else if (contact->GetFixtureA()->GetBody()->GetPosition().x < m_rigidbody->GetPosition().x){
					setFacingRight(false);
				}
				break;
			}
		}
	}
		
	
}

void Enemy::EndContact(b2Contact* contact, GameObject* other){
	if (!(contact->GetFixtureA()->GetFilterData().maskBits & contact->GetFixtureB()->GetFilterData().categoryBits) != 0 && (contact->GetFixtureA()->GetFilterData().categoryBits & contact->GetFixtureB()->GetFilterData().maskBits) != 0){
		return;
	}
	GameObject* objA = nullptr;
	GameObject* objB = nullptr;
	
	void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
	int fixtureDataA = (int)contact->GetFixtureA()->GetUserData();
	int fixtureDataB = (int)contact->GetFixtureB()->GetUserData();
	int x = 0;
	//Do some AI things
	if (bodyUserDataA){
		objA = static_cast<GameObject*>(bodyUserDataA);
		if (objA->getTag().compare("ENEMY") == 0){
			switch (fixtureDataA){
			case EYES:
				m_playerInSight = false;
				break;
			case EARS:
				m_hearPlayer = false;
				break;
			}
		}
	}
	if (bodyUserDataB){
		objB = static_cast<GameObject*>(bodyUserDataB);
		if (objB->getTag().compare("ENEMY") == 0){
			switch (fixtureDataB){
			case EYES:
				m_playerInSight = false;
				break;
			case EARS:
				m_hearPlayer = false;
				break;
			}
		}
	}
}

/*returns distance to player if it succeeds, -1 if it fails (player is out of sight or there is a tile blocking him/her from sight*/
float Enemy::lookForPlayer(){
	b2Transform transform;
	transform.SetIdentity();
	b2RayCastInput in;
	b2RayCastOutput out;
	b2Vec2 pos = m_rigidbody->GetPosition();
	in.maxFraction = 1;
	RayCastLookForPlayer callback;
	in.p1 = b2Vec2(pos.x, pos.y - (m_hitBoxSize.y / 2.0f));
	if (m_player == nullptr){
		in.p2 = b2Vec2(in.p1.x + m_sight * (isFacingRight() ? 1 : -1), in.p1.y);//End point to use if we don't know any better
	}
	else{
		in.p2 = b2Vec2(m_player->getRigidbody()->GetPosition().x, m_player->getRigidbody()->GetPosition().y);
	}
	Box2DSingleton::Instance()->getWorld()->RayCast(&callback, in.p1, in.p2);
	if (!callback.m_hit){
		return -1;
	}
	//m_playerLocation = callback.getPlayer()->getPosition();
	return callback.m_fraction*m_sight;
}

/*cast an ability at the player*/
void Enemy::attack(){
	float percentile = ((float)(rand() % 1000))/1000.0f;//allow precision down to .1%
	if (percentile < m_abilityChance[0]){
		//cast first ability
		if (m_cooldownTimers[0] <= 0){
			castAbility(m_activeAbilities[0]);
			m_cooldownTimers[0] = m_cooldowns[0];
		}
	}
	else if (percentile < m_abilityChance[1]+m_abilityChance[0]){
		if (m_cooldownTimers[1] <= 0){
			castAbility(m_activeAbilities[1]);
			m_cooldownTimers[1] = m_cooldowns[1];
		}
	}
	else if (percentile < m_abilityChance[2] + m_abilityChance[1] + m_abilityChance[0]){
		if (m_cooldownTimers[2] <= 0){
			castAbility(m_activeAbilities[2]);
			m_cooldownTimers[2] = m_cooldowns[2];
		}
	}
	else if (percentile < m_abilityChance[3] + m_abilityChance[2] + m_abilityChance[1] + m_abilityChance[0]){
		if (m_cooldownTimers[3] <= 0){
			castAbility(m_activeAbilities[3]);
			m_cooldownTimers[3] = m_cooldowns[3];
		}
	}
}

/*Update timers so that we can fire again*/
void Enemy::updateCooldownTimers(float dt){
	for (int x = 0; x < 4; x++){
		m_cooldownTimers[x] = fmaxf(m_cooldownTimers[x] - dt, 0);
	}
}

/*Makes the enemy face in the x direction toward the player*/
void Enemy::facePlayer(){
	if (m_player == nullptr){
		return;
	}
	if (m_player->getRigidbody()->GetPosition().x > m_rigidbody->GetPosition().x){
		setFacingRight(true);
	}
	else if (m_player->getRigidbody()->GetPosition().x < m_rigidbody->GetPosition().x){
		setFacingRight(false);
	}
}

/*Fight or flight (or die) mechanism. should be called after taking damage, preferrably damage caused by an ability, not physics damage*/
void Enemy::respondToAggression(){
	if (m_hp > (m_hpmax * ((float)(30 - m_courage)) / 100.0)){
		if (getAiState() < AI_ALERT){
			setAiState(AI_ALERT);
		}
	}
	else if(m_hp>0){
		setAiState(AI_FLEE);
		m_aiStateTimer[0] = 12;
	}
	if (m_hp <= 0){
		setAiState(AI_DEATH);
	}
}

void Enemy::GTFO(float dt){
	if (m_player == nullptr){
		setFacingRight(true);
	}
	else if (m_player->getRigidbody()->GetPosition().x > getPosition().x){
		setFacingRight(false);
	}
	else if (m_player->getRigidbody()->GetPosition().x < getPosition().x){
		setFacingRight(true);
	}
	if (shouldIJump()){
		jump();
	}
	run(dt);
}

/*Figure out which ability is most likely to give us a good wall breaker (EX: Fireball is better than firebolt)*/
void Enemy::determineWallBreakAbility(){
	int x = 0;
	Vector2 temp;
	Vector2 max = Vector2(0, 0);
	m_wallBreakIndex = -1;
	while (m_activeAbilities[x] > 0){
		temp = AbilityManager::Instance()->getMaxHitboxSize(m_activeAbilities[x]);
		if (temp.x * temp.y > max.x * max.y){
			max = temp;
			m_wallBreakIndex = x;
		}
		x++;
		if (x > 4){
			break;
		}
	}
}

Ability* Enemy::castAbility(UINT id){
	if (id == 0){
		return nullptr;
	}
	Ability* ptr = nullptr;
	UINT cost = AbilityManager::Instance()->getManaCost(id, this);
	m_timer = 0;
	ptr = GameObject::castAbility(id);
	if (ptr->getAbilityDesc().castAnim.compare("NO_ANIM") == 0){

	}
	else if (ptr->getAbilityDesc().castAnim.compare("SLASH") == 0){
		m_currAnim = ANIM_SLASH;
	}
	else{
		m_currAnim = ANIM_CAST;
	}
	return ptr;
}

/*Get the passive for the requested ability*/
int Enemy::getPassiveForAbility(UINT ability){
	if (ability <= 3){
		return m_passiveAbilities[ability];
	}
	return 0;
}