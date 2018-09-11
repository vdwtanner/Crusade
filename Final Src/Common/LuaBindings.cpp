#include "pch.h"
#include "LuaBindings.h"
#include "Character.h"
#include "LuaTrigger.h"

LuaBindings::LuaBindings(Character* player){
	h_player = player;
}

LuaBindings::~LuaBindings(){

}

bool LuaBindings::BindAllFunctions(){
	m_lState = luaL_newstate();
	luaL_openlibs(m_lState);

	lua_register(m_lState, "Yield", lua_Yield);
	lua_register(m_lState, "YieldUntilInput", lua_YieldUntilInput);
	lua_register(m_lState, "YieldUntilDead", lua_YieldUntilDead);
	lua_register(m_lState, "PausePhysics", lua_PausePhysics);
	lua_register(m_lState, "ResumePhysics", lua_ResumePhysics);
	lua_register(m_lState, "PauseAI", lua_PauseAI);
	lua_register(m_lState, "ResumeAI", lua_ResumeAI);
	lua_register(m_lState, "Pause", lua_Pause);
	lua_register(m_lState, "Resume", lua_Resume);
#ifndef _CRUSADE_EDITOR_
	lua_register(m_lState, "EndLevel", lua_EndLevel);
	lua_register(m_lState, "EnableMovementInput", lua_EnableMovementInput);
	lua_register(m_lState, "DisableMovementInput", lua_DisableMovementInput);
	lua_register(m_lState, "EnableAbilityInput", lua_EnableAbilityInput);
	lua_register(m_lState, "DisableAbilityInput", lua_DisableAbilityInput);
#endif
	lua_register(m_lState, "EnablePlayerFlight", lua_EnablePlayerFlight);
	lua_register(m_lState, "DisablePlayerFlight", lua_DisablePlayerFlight);
	lua_register(m_lState, "ApplyImpulse", lua_ApplyImpulse);
	lua_register(m_lState, "Damage", lua_Damage);
	lua_register(m_lState, "Heal", lua_Heal);
	lua_register(m_lState, "AwardExp", lua_AwardExp);
	lua_register(m_lState, "GetPosition", lua_GetPosition);
	lua_register(m_lState, "GetPlayerCharacter", lua_GetPlayerCharacter);
	lua_register(m_lState, "MoveTo", lua_MoveTo);
	lua_register(m_lState, "Teleport", lua_Teleport);
	lua_register(m_lState, "EnableNoCollide", lua_EnableNoCollide);
	lua_register(m_lState, "DisableNoCollide", lua_DisableNoCollide);

	lua_register(m_lState, "CreateEnemy", lua_CreateEnemy);
	lua_register(m_lState, "DestroyEnemy", lua_DestroyEnemy);

#ifndef _CRUSADE_EDITOR_
	lua_register(m_lState, "CameraGetPosition", lua_CameraGetPosition);
	lua_register(m_lState, "CameraSetPosition", lua_CameraSetPosition);
	lua_register(m_lState, "CameraMoveTo", lua_CameraMoveTo);
	lua_register(m_lState, "CameraZoomTo", lua_CameraZoomTo);
	lua_register(m_lState, "CameraFollowPlayer", lua_CameraFollowPlayer);
#endif

	lua_register(m_lState, "DeactivateTrigger", lua_DeactivateTrigger);

#ifndef _CRUSADE_EDITOR_
	lua_register(m_lState, "DisplayTextbox", lua_DisplayTextbox);
#endif

#ifndef _CRUSADE_EDITOR_
	lua_register(m_lState, "PlaySound", lua_PlaySound);
	lua_register(m_lState, "PauseSound", lua_PauseSound);
	lua_register(m_lState, "ResumeSound", lua_ResumeSound);
	lua_register(m_lState, "StopSound", lua_StopSound);
	lua_register(m_lState, "PauseBGM", lua_PauseBGM);
	lua_register(m_lState, "ResumeBGM", lua_ResumeBGM);
	lua_register(m_lState, "SwitchBGM", lua_SwitchBGM);
	lua_register(m_lState, "SetBGMVolume", lua_SetBGMVolume);

	lua_register(m_lState, "DamageTile", lua_DamageTile);
	lua_register(m_lState, "EmitBlockParticles", lua_EmitBlockParticles);

#endif


	return true;
}

lua_State* LuaBindings::getLuaState(){
	return m_lState;
}

void LuaBindings::setCharacter(Character* character){
	h_player = character;
}

Character *LuaBindings::getCharacter(){
	return h_player;
}

#ifndef _CRUSADE_EDITOR_

GameScreen* LuaBindings::getGameScreen(){
	return h_gameScreen;
}

void LuaBindings::setGameScreen(GameScreen *screen){
	h_gameScreen = screen;
}

// in Lua: EndLevel(context)
int LuaBindings::lua_EndLevel(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENDLEVEL) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->loadNextWorld();
	return 0;
}
#endif

// in Lua: Yield(lightuserdata trigger, float time)
int LuaBindings::lua_Yield(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_YIELD) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaTrigger *trigger = (LuaTrigger*)lua_touserdata(luaState, 1);
	if (!trigger){
		lua_pushstring(luaState, "ERROR: (C++_YIELD) invalid trigger");
		lua_error(luaState);
	}
	float time = (float)lua_tonumber(luaState, 2);	// numbers are the floating point type in lua

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	trigger->yield(time);

	return 0;
}

// in Lua: YieldUntilInput(trigger)
int LuaBindings::lua_YieldUntilInput(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_YIELDUNTILINPUT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaTrigger *trigger = (LuaTrigger*)lua_touserdata(luaState, 1);
	if (!trigger){
		lua_pushstring(luaState, "ERROR: (C++_YIELDUNTILINPUT) invalid trigger");
		lua_error(luaState);
	}

	lua_pop(luaState, 1);

	trigger->anyKeyYield();

	return 0;
}

// in Lua: YieldUntilDead(trigger, enemy)
int LuaBindings::lua_YieldUntilDead(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_YIELDUNTILDEAD) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaTrigger *trigger = (LuaTrigger*)lua_touserdata(luaState, 1);
	GameObject *gameObj = (GameObject*)lua_touserdata(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("ENEMY") == 0) {
		((Enemy*)gameObj)->setContainingTrigger(trigger);
		trigger->deathYield((Enemy*)gameObj);
	}
	else
	{
		lua_pushstring(luaState, "ERROR: (C++_YIELDUNTILDEAD) parameter 2 is not an enemy");
		lua_error(luaState);
	}

	return 0;
}

// in Lua: PausePhysics(context)
int LuaBindings::lua_PausePhysics(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_PAUSEPHYSICS) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	Box2DSingleton::Instance()->pause();
	AbilityManager::Instance()->pause();

	return 0;
}

// in Lua: ResumePhysics(context)
int LuaBindings::lua_ResumePhysics(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_RESUMEPHYSICS) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	Box2DSingleton::Instance()->resume();
	AbilityManager::Instance()->resume();


	return 0;
}

// in Lua: PauseAI(context)
int LuaBindings::lua_PauseAI(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_PAUSEAI) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	EnemyManager::Instance()->pause();

	return 0;
}

// in Lua: ResumeAI(context)
int LuaBindings::lua_ResumeAI(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_RESUMEAI) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	EnemyManager::Instance()->resume();

	return 0;
}

// in Lua: Pause(context)
int LuaBindings::lua_Pause(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_PAUSE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	Box2DSingleton::Instance()->pause();
	AbilityManager::Instance()->pause();
	EnemyManager::Instance()->pause();
	return 0;
}

// in Lua: Resume(context)
int LuaBindings::lua_Resume(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_RESUME) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	Box2DSingleton::Instance()->resume();
	AbilityManager::Instance()->resume();
	EnemyManager::Instance()->resume();
	return 0;
}

#ifndef _CRUSADE_EDITOR_

// in Lua: EnableMovementInput(context)
int LuaBindings::lua_EnableMovementInput(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENABLEMOVEMENTINPUT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->enableMovementInput();

	return 0;
}

// in Lua: DisableMovementInput(context)
int LuaBindings::lua_DisableMovementInput(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_DISABLEMOVEMENTINPUT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->disableMovementInput();

	return 0;
}

// in Lua: EnableAbilityInput(context)
int LuaBindings::lua_EnableAbilityInput(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENABLEABILITYINPUT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->enableAbilityInput();

	return 0;
}

// in Lua: DisableAbilityInput(context);
int LuaBindings::lua_DisableAbilityInput(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_DISABLEABILITYINPUT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->disableAbilityInput();

	return 0;
}

#endif

// in Lua: EnablePlayerFlight(context)
int LuaBindings::lua_EnablePlayerFlight(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENABLEPLAYERFLIGHT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getCharacter()->enableFlight();

	return 0;
}

// in Lua: DisablePlayerFlight(context)
int LuaBindings::lua_DisablePlayerFlight(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_DISABLEPLAYERFLIGHT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getCharacter()->disableFlight();

	return 0;
}

// in Lua: ApplyImpulse(collider, fx, fy)
int LuaBindings::lua_ApplyImpulse(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 3){
		lua_pushstring(luaState, "ERROR: (C++_APPLYFORCE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	float fx = lua_tonumber(luaState, 2);
	float fy = lua_tonumber(luaState, 3);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("PLAYER") == 0){
		Character *player = (Character *)gameObj;
		player->getRigidbody()->ApplyLinearImpulse(b2Vec2(fx, fy), player->getRigidbody()->GetLocalCenter(), true);
	}
	else if (gameObj->getTag().compare("ENEMY") == 0) {
		Enemy *enemy = (Enemy *)gameObj;
		enemy->getRigidbody()->ApplyLinearImpulse(b2Vec2(fx, fy), enemy->getRigidbody()->GetLocalCenter(), true);
	}
	else if (gameObj->getTag().compare("ABILITY") == 0){
		Ability *ability = (Ability *)gameObj;
		ability->getSensorBody()->ApplyLinearImpulse(b2Vec2(fx, fy), ability->getSensorBody()->GetLocalCenter(), true);
	}

	return 0;
}

// in Lua: Damage(gameobject, damage)
int LuaBindings::lua_Damage(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_DAMAGE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	int damage = lua_tointeger(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	gameObj->damage(damage);

	return 0;
}

// in Lua: Heal(gameobject, health)
int LuaBindings::lua_Heal(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_HEAL) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	Vector2 healPos = Vector2(gameObj->getRenderPosition().x + (gameObj->isFacingRight() ? gameObj->getHitboxSize().x / 2 : (gameObj->getHitboxSize().x / 2) * -1), gameObj->getRenderPosition().y + gameObj->getHitboxSize().y / 2);
	ParticleManager::Instance()->EmitParticles(healPos, Vector2(0.05f, 0.05f), Vector2(0.0f, 0.0f), PT_HOLYSHOWER);
	int health = lua_tointeger(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	gameObj->heal(health);

	return 0;
}

//in Lua: AwardExp(gameobject, EXP)
int LuaBindings::lua_AwardExp(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_AWARDEXP) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject*)lua_touserdata(luaState, 1);
	if (gameObj->getTag().compare("PLAYER") != 0){
		//invalid GameObject
		lua_pop(luaState, 1);
		lua_pop(luaState, 1);
		return 0;
	}
	Vector2 healPos = Vector2(gameObj->getRenderPosition().x + (gameObj->isFacingRight() ? gameObj->getHitboxSize().x / 2 : (gameObj->getHitboxSize().x / 2) * -1), gameObj->getRenderPosition().y + gameObj->getHitboxSize().y / 2);
	ParticleManager::Instance()->EmitParticles(healPos, Vector2(0.05f, 0.05f), Vector2(0.0f, 0.0f), PT_EXPBURST);
	int exp = lua_tointeger(luaState, 2);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	((Character*)gameObj)->addExp(exp);
	return 0;
}

// in Lua: GetPosition(gameobject)
int LuaBindings::lua_GetPosition(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_GETPOSITION) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	Vector2 v = gameObj->getCenterPosition();

	lua_pushnumber(luaState, v.x);
	lua_pushnumber(luaState, v.y);

	return 2;
}

// in Lua: GetPlayerCharacter(context)
int LuaBindings::lua_GetPlayerCharacter(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_GETPLAYERCHARACTER) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	lua_pushlightuserdata(luaState, bindings->getCharacter());

	return 1;
}

// in Lua: MoveTo(physobject, x, y, time)
int LuaBindings::lua_MoveTo(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 4){
		lua_pushstring(luaState, "ERROR: (C++_MOVETO) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	float x = lua_tonumber(luaState, 2);
	float y = lua_tonumber(luaState, 3);
	float time = lua_tonumber(luaState, 4);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("PLAYER") == 0){
		((Character *)gameObj)->moveTo(x, y, time);
	}
	else if (gameObj->getTag().compare("ENEMY") == 0){
		((Enemy *)gameObj)->moveTo(x, y, time);
	}
	else if (gameObj->getTag().compare("ABILITY") == 0){
		// doesnt do anything at the moment
		((Ability *)gameObj)->moveTo(x, y, time);
	}

	return 0;
}

// in Lua: Teleport(physobject, x, y)
int LuaBindings::lua_Teleport(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 3){
		lua_pushstring(luaState, "ERROR: (C++_TELEPORT) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	float x = lua_tonumber(luaState, 2);
	float y = lua_tonumber(luaState, 3);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("PLAYER") == 0){
		((Character *)gameObj)->getRigidbody()->SetTransform(b2Vec2(x, y), ((Character *)gameObj)->getRigidbody()->GetAngle());
	}
	else if (gameObj->getTag().compare("ENEMY") == 0){
		((Enemy *)gameObj)->getRigidbody()->SetTransform(b2Vec2(x, y), ((Enemy *)gameObj)->getRigidbody()->GetAngle());
	}
	else if (gameObj->getTag().compare("ABILITY") == 0){
		// doesnt do anything at the moment
		((Ability *)gameObj)->getSensorBody()->SetTransform(b2Vec2(x, y), ((Ability *)gameObj)->getSensorBody()->GetAngle());
	}

	return 0;
}

// in Lua: EnableNoCollide(physobject)
int LuaBindings::lua_EnableNoCollide(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENABLENOCOLLIDE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("PLAYER") == 0){
		((Character *)gameObj)->setEnableCollisionWith(Box2DSingleton::WORLDTILE);
	}
	else if (gameObj->getTag().compare("ENEMY") == 0){
		((Enemy *)gameObj)->setEnableCollisionWith(Box2DSingleton::WORLDTILE);
	}
	else if (gameObj->getTag().compare("ABILITY") == 0){
		// doesnt do anything at the moment
		((Ability *)gameObj)->setEnableCollisionWith(((Ability *)gameObj)->getCollisionCategory(), Box2DSingleton::NONE);
	}

	return 0;
}

// in Lua: DisableNoCollide(physobject)
int LuaBindings::lua_DisableNoCollide(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_ENABLENOCOLLIDE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	GameObject *gameObj = (GameObject *)lua_touserdata(luaState, 1);
	lua_pop(luaState, 1);

	if (gameObj->getTag().compare("PLAYER") == 0){
		((Character *)gameObj)->setEnableCollisionWith(Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER | Box2DSingleton::TRIGGER | Box2DSingleton::PHYSICAL_TRIGGER);
	}
	else if (gameObj->getTag().compare("ENEMY") == 0){
		((Enemy *)gameObj)->setEnableCollisionWith(Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER | Box2DSingleton::TRIGGER | Box2DSingleton::PHYSICAL_TRIGGER);
	}
	else if (gameObj->getTag().compare("ABILITY") == 0){
		// doesnt do anything at the moment
		((Ability *)gameObj)->setEnableCollisionWith(((Ability *)gameObj)->getCollisionCategory(), Box2DSingleton::WORLDTILE | Box2DSingleton::ENEMY | Box2DSingleton::CHARACTER | Box2DSingleton::TRIGGER | Box2DSingleton::PHYSICAL_TRIGGER);
	}

	return 0;
}

// in Lua: CreateEnemy(context, id, x, y)
int LuaBindings::lua_CreateEnemy(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 4){
		lua_pushstring(luaState, "ERROR: (C++_CREATEENEMY) incorrect amount of arguments given");
		lua_error(luaState);
	}

	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	int id = lua_tointeger(luaState, 2);
	float x = lua_tonumber(luaState, 3);
	float y = lua_tonumber(luaState, 4);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	Enemy *out;
	EnemyManager::Instance()->createEnemy(id, &out);
	out->setPosition(Vector2(x, y));

	lua_pushlightuserdata(luaState, out);

	return 1;
}

// in Lua: DestroyEnemy(enemy)
int LuaBindings::lua_DestroyEnemy(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_CREATEENEMY) incorrect amount of arguments given");
		lua_error(luaState);
	}

	Enemy *enemy = (Enemy *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	EnemyManager::Instance()->deleteEnemy(enemy);

	return 0;
}

#ifndef _CRUSADE_EDITOR_

// in Lua: CameraGetPosition(context)
int LuaBindings::lua_CameraGetPosition(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_CAMERAGETPOSITION) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	lua_pushnumber(luaState, bindings->getGameScreen()->getCamera()->getPosition().x);
	lua_pushnumber(luaState, bindings->getGameScreen()->getCamera()->getPosition().y);

	return 2;
}

// in Lua: CameraSetPosition(context, x, y)
int LuaBindings::lua_CameraSetPosition(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 3){
		lua_pushstring(luaState, "ERROR: (C++_CAMERASETPOSITION) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	float x = lua_tonumber(luaState, 2);
	float y = lua_tonumber(luaState, 3);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->getCamera()->SetPos(Vector3(x, y, 5.0f));

	return 0;
}

// in Lua: CameraMoveTo(context, x, y, time)
int LuaBindings::lua_CameraMoveTo(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 4){
		lua_pushstring(luaState, "ERROR: (C++_CAMERAMOVETO) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	float x = lua_tonumber(luaState, 2);
	float y = lua_tonumber(luaState, 3);
	float time = lua_tonumber(luaState, 4);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->getCamera()->moveTo(x, y, 5.0f, time);

	return 0;
}

// in Lua: CameraZoomTo(context, zoom, time)
int LuaBindings::lua_CameraZoomTo(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 3){
		lua_pushstring(luaState, "ERROR: (C++_CAMERAZOOMTO) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	float zoom = lua_tonumber(luaState, 2);
	float time = lua_tonumber(luaState, 3);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->getCamera()->ZoomTo(1.0f / zoom, time);

	return 0;
}

// in Lua: CameraFollowPlayer(context)
int LuaBindings::lua_CameraFollowPlayer(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_CAMERAFOLLOWPLAYER) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	bindings->getGameScreen()->getCamera()->EnableFollowPlayer();

	return 0;
}

#endif

// in Lua: DeactivateTrigger(trigger)
int LuaBindings::lua_DeactivateTrigger(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_DELETETRIGGER) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaTrigger *trigger = (LuaTrigger*)lua_touserdata(luaState, 1);
	if (!trigger){
		lua_pushstring(luaState, "ERROR: (C++_DELETETRIGGER) invalid trigger");
		lua_error(luaState);
	}

	lua_pop(luaState, 1);

	trigger->Deactivate();

	return 0;
}

#ifndef _CRUSADE_EDITOR_
// in Lua: DisplayTextbox(context, title, msg)
int LuaBindings::lua_DisplayTextbox(lua_State *luaState){
	USES_CONVERSION;

	int numArgs = lua_gettop(luaState);
	if (numArgs != 3){
		lua_pushstring(luaState, "ERROR: (C++_DISPLAYTEXTBOX) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	std::wstring title = A2CW(lua_tostring(luaState, 2));
	std::wstring msg = A2CW(lua_tostring(luaState, 3));

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->getHUD()->displayTextBox(title, msg);

	return 0;
}
#endif

#ifndef _CRUSADE_EDITOR_
// in Lua: PlaySound(context, soundFile)
int LuaBindings::lua_PlaySound(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_PLAYSOUND) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	std::string file = lua_tostring(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	CSoundEffectInstance *sound = AudioManager::Instance()->PlaySound(file);

	lua_pushlightuserdata(luaState, sound);

	return 1;
}

// in Lua: PauseSound(sound)
int LuaBindings::lua_PauseSound(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_PAUSESOUND) incorrect amount of arguments given");
		lua_error(luaState);
	}
	CSoundEffectInstance *sound = (CSoundEffectInstance*)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);
	if (!sound){
		lua_pushstring(luaState, "ERROR: (C++_PAUSESOUND) invalid trigger");
		lua_error(luaState);
	}

	bool success = AudioManager::Instance()->PauseSound(sound);

	lua_pushboolean(luaState, success);

	return 1;

}

// in Lua: ResumeSound(sound)
int LuaBindings::lua_ResumeSound(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_RESUMESOUND) incorrect amount of arguments given");
		lua_error(luaState);
	}
	CSoundEffectInstance *sound = (CSoundEffectInstance*)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);
	if (!sound){
		lua_pushstring(luaState, "ERROR: (C++_RESUMESOUND) invalid trigger");
		lua_error(luaState);
	}

	bool success = AudioManager::Instance()->ResumeSound(sound);

	lua_pushboolean(luaState, success);

	return 1;
}

// in Lua: StopSound(sound)
int LuaBindings::lua_StopSound(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_STOPSOUND) incorrect amount of arguments given");
		lua_error(luaState);
	}
	CSoundEffectInstance *sound = (CSoundEffectInstance*)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);
	if (!sound){
		lua_pushstring(luaState, "ERROR: (C++_STOPSOUND) invalid trigger");
		lua_error(luaState);
	}

	bool success = AudioManager::Instance()->StopSound(sound);

	lua_pushboolean(luaState, success);

	return 1;
}

// in Lua: PauseBGM(context)
int LuaBindings::lua_PauseBGM(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_PAUSEBGM) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	bindings->getGameScreen()->PauseBGM();

	return 0;
}

// in Lua: ResumeBGM(context)
int LuaBindings::lua_ResumeBGM(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 1){
		lua_pushstring(luaState, "ERROR: (C++_RESUMEBGM) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);

	lua_pop(luaState, 1);

	bindings->getGameScreen()->ResumeBGM();

	return 0;
}

// in Lua: SwitchBGM(context, newBGM)
int LuaBindings::lua_SwitchBGM(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_SWITCHBGM) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	std::string file = lua_tostring(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->SwitchBGM(file);

	return 0;
}

// in Lua: SetBGMVolume(context, volume)
int LuaBindings::lua_SetBGMVolume(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 2){
		lua_pushstring(luaState, "ERROR: (C++_RESUMEBGM) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	float volume = lua_tonumber(luaState, 2);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->getGameScreen()->SetBGMVolume(volume);

	return 0;
}

#endif

#ifndef _CRUSADE_EDITOR_

// in Lua: DamageTile(context, damage, x, y)
int LuaBindings::lua_DamageTile(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 4){
		lua_pushstring(luaState, "ERROR: (C++_DAMAGETILE) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	int damage = lua_tointeger(luaState, 2);
	float x = lua_tonumber(luaState, 3);
	float y = lua_tonumber(luaState, 4);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	bindings->h_gameScreen->getWorldTileMatrix()->damageTile((int)x, (int)y, bindings->h_gameScreen->getWorldTileMatrix()->getActiveLayer(), damage);
	bindings->h_gameScreen->getWorldTileMatrix()->damageTilesInFront((int)x, (int)y, bindings->h_gameScreen->getWorldTileMatrix()->getActiveLayer(), damage);

	return 0;
}

// in Lua: EmitBlockParticles(context, x, y, dirx, diry, blockID)
int LuaBindings::lua_EmitBlockParticles(lua_State *luaState){
	int numArgs = lua_gettop(luaState);
	if (numArgs != 6){
		lua_pushstring(luaState, "ERROR: (C++_EMITBLOCKPARTICLES) incorrect amount of arguments given");
		lua_error(luaState);
	}
	LuaBindings *bindings = (LuaBindings *)lua_touserdata(luaState, 1);
	float x = lua_tonumber(luaState, 2);
	float y = lua_tonumber(luaState, 3);
	float dirx = lua_tonumber(luaState, 4);
	float diry = lua_tonumber(luaState, 5);
	int blockID = lua_tointeger(luaState, 6);

	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);
	lua_pop(luaState, 1);

	int texIndex = bindings->h_gameScreen->getGame()->getTileSet()->getTextureIndex(blockID);

	ParticleManager::Instance()->EmitParticles(Vector2(x, y), Vector2(.15f, .15f), Vector2(dirx, diry), PT_BLOCK, blockID);

	return 0;
}

#endif