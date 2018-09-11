#pragma once
#include "pch.h"
#include <lua.hpp>

class Enemy;

class LuaTrigger : public TriggerVolume{
public:
	enum Precondition{
		ENEMIES_CONTAINED_DEAD = 0x0001,

	};
	// name is our filename
	LuaTrigger(lua_State *threadState,std::string tag, std::string name, std::wstring filename, Vector2 position, Vector2 hitboxSize, float rotation, int id);
	~LuaTrigger();

	virtual LuaTrigger* clone();
	virtual void step(float dt);

	void yield(float time);

	virtual void onTrigger(GameObject* collider);
	virtual void onExitTrigger(GameObject* collider);
	virtual void onAirTrigger(WorldTile* air);

	void Deactivate();

	std::wstring getFilename();

	void notifyOfDeath(Enemy* enemy);

	void anyKeyPressed();
	void anyKeyYield();
	void deathYield(Enemy* enemy);

private:
	bool m_playerOnlyTrig;
	// the local coroutine state
	lua_State *m_coState;
	// a pointer to the overall threadState
	lua_State *m_threadState;
	std::wstring m_filename;
	// when this script yields, it sets our yieldTimer to the amount passed in by lua:Yield(float time)
	// each iteration then subtracts dt from yieldTimer, and when yieldTimer runs out, the script resumes
	// until the next yield or until end of execution.
	float m_yieldTimer;
	float m_timer;
	// we only resume on updates, so this bool keeps track of if we have entered or exited a trigger recently.
	bool m_triggered;
	bool m_yielded;
	bool m_anyKeyYielded;
	bool m_deathYielded;
	bool m_anyKeyPressed;
	bool m_deactivated;

	std::vector<Enemy*> m_containedOnStart;
	Enemy* h_yieldEnemy;

	int m_conditions;

};