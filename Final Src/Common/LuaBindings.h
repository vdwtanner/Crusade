#pragma once

#include "pch.h"
#include <lua.hpp>
#include "ParticleManager.h"
#ifndef _CRUSADE_EDITOR_
#include "GameScreen.h"
#endif

class GameScreen;
class Character;

class LuaBindings{
public:
	LuaBindings(Character* player);
	~LuaBindings();
	bool BindAllFunctions();

	lua_State* getLuaState();

	void setCharacter(Character*);
	Character *getCharacter();

#ifndef _CRUSADE_EDITOR_
	GameScreen *getGameScreen();
	void setGameScreen(GameScreen*);
#endif

	// when calling a c function from lua, lua creates a private stack
	// for each invocation. It then pushes the args on the stack where
	// the first arg is at position 1 on the stack.
	
	/********************
		General Functs
	*********************/
	// in Lua: Yield(lightuserdata trigger, float time)
	static int lua_Yield(lua_State *luaState);
	// in Lua: YieldUntilInput(trigger)
	static int lua_YieldUntilInput(lua_State *luaState);
	// in Lua: YieldUntilDead(trigger, enemy)
	static int lua_YieldUntilDead(lua_State *luaState);
	// in Lua: PausePhysics(context)
	static int lua_PausePhysics(lua_State *luaState);
	// in Lua: ResumePhysics(context)
	static int lua_ResumePhysics(lua_State *luaState);
	// in Lua: PauseAI(context)
	static int lua_PauseAI(lua_State *luaState);
	// in Lua: ResumeAI(context)
	static int lua_ResumeAI(lua_State *luaState);
	// in Lua: Pause(context)
	static int lua_Pause(lua_State *luaState);
	// in Lua: Resume(context)
	static int lua_Resume(lua_State *luaState);
#ifndef _CRUSADE_EDITOR_
	// in Lua: EndLevel(context)
	static int lua_EndLevel(lua_State *luaState);
#endif

	/*****************************
		Player/GameObject Functs
	******************************/
#ifndef _CRUSADE_EDITOR_
	// in Lua: EnableMovementInput(context)
	static int lua_EnableMovementInput(lua_State *luaState);
	// in Lua: DisableMovementInput(context)
	static int lua_DisableMovementInput(lua_State *luaState);
	// in Lua: EnableAbilityInput(context)
	static int lua_EnableAbilityInput(lua_State *luaState);
	// in Lua: DisableAbilityInput(context);
	static int lua_DisableAbilityInput(lua_State *luaState);
#endif
	// in Lua: EnablePlayerFlight(context)
	static int lua_EnablePlayerFlight(lua_State *luaState);
	// in Lua: DisablePlayerFlight(context)
	static int lua_DisablePlayerFlight(lua_State *luaState);
	// in Lua: ApplyImpulse(collider, fx, fy)
	static int lua_ApplyImpulse(lua_State *luaState);
	// in Lua: Damage(gameobject, damage)
	static int lua_Damage(lua_State *luaState);
	// in Lua: Heal(gameobject, health)
	static int lua_Heal(lua_State *luaState);
	// in Lua: Heal(gameobject, EXP)
	static int lua_AwardExp(lua_State *luaState);
	// in Lua: GetPosition(gameobject)
	static int lua_GetPosition(lua_State *luaState);
	// in Lua: GetPlayerCharacter(context)
	static int lua_GetPlayerCharacter(lua_State *luaState);
	// in Lua: MoveTo(physobject, x, y, time)
	static int lua_MoveTo(lua_State *luaState);
	// in Lua: Teleport(physobject, x, y)
	static int lua_Teleport(lua_State *luaState);
	// in Lua: EnableNoCollide(physobject)
	static int lua_EnableNoCollide(lua_State *luaState);
	// in Lua: DisableNoCollide(physobject)
	static int lua_DisableNoCollide(lua_State *luaState);

	/*******************
		Enemy Functs
	********************/

	// in Lua: CreateEnemy(context, id, x, y)
	static int lua_CreateEnemy(lua_State *luaState);
	// in Lua: DestroyEnemy(enemy)
	static int lua_DestroyEnemy(lua_State *luaState);

	/*******************
		Camera Functs
	********************/
#ifndef _CRUSADE_EDITOR_
	// in Lua: CameraGetPosition(context)
	static int lua_CameraGetPosition(lua_State *luaState);
	// in Lua: CameraSetPosition(context, x, y)
	static int lua_CameraSetPosition(lua_State *luaState);
	// in Lua: CameraMoveTo(context, x, y, time)
	static int lua_CameraMoveTo(lua_State *luaState);
	// in Lua: CameraZoomTo(context, zoom, time);
	static int lua_CameraZoomTo(lua_State *luaState);
	// in Lua: CameraFollowPlayer(context)
	static int lua_CameraFollowPlayer(lua_State *luaState);
#endif

	/***********************
		Trigger Functs
	************************/
	// in Lua: DeactivateTrigger(trigger)
	static int lua_DeactivateTrigger(lua_State *luaState);

	/************************
		Textbox Functs
	*************************/
#ifndef _CRUSADE_EDITOR_
	// in Lua: DisplayTextbox(context, title, msg)
	static int lua_DisplayTextbox(lua_State *luaState);
#endif

	/************************
		Sound Functs
	*************************/
#ifndef _CRUSADE_EDITOR_
	// in Lua: PlaySound(context, soundFile)
	static int lua_PlaySound(lua_State *luaState);
	// in Lua: PauseSound(sound)
	static int lua_PauseSound(lua_State *luaState);
	// in Lua: ResumeSound(sound)
	static int lua_ResumeSound(lua_State *luaState);
	// in Lua: StopSound(sound)
	static int lua_StopSound(lua_State *luaState);
	// in Lua: PauseBGM(context)
	static int lua_PauseBGM(lua_State *luaState);
	// in Lua: ResumeBGM(context)
	static int lua_ResumeBGM(lua_State *luaState);
	// in Lua: SwitchBGM(context, newBGM)
	static int lua_SwitchBGM(lua_State *luaState);
	// in Lua: SetBGMVolume(context)
	static int lua_SetBGMVolume(lua_State *luaState);
#endif

	/*******************************
		Block Functions
	********************************/
#ifndef _CRUSADE_EDITOR_
	// in Lua: DamageTile(context, damage, x, y, layer)
	static int lua_DamageTile(lua_State *luaState);
	// in Lua: EmitBlockParticles(context, x, y, dirx, diry, blockID)
	static int lua_EmitBlockParticles(lua_State *luaState);

#endif

private:
	lua_State *m_lState;
	Character *h_player;

#ifndef _CRUSADE_EDITOR_
	GameScreen *h_gameScreen;
#endif
};