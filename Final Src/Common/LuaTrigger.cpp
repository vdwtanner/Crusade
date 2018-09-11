#include "pch.h"
#include "LuaTrigger.h"
#include "TriggerManager.h"

LuaTrigger::LuaTrigger(lua_State *threadStack, std::string tag, std::string name, std::wstring filename, Vector2 position, Vector2 hitboxSize, float rotation, int id)
	: TriggerVolume(tag, name, position, hitboxSize, hitboxSize, Vector2(0.0f, 0.0f), rotation, id, -1)
{
	m_threadState = threadStack;
	m_filename = filename;
	m_coState = nullptr;

	m_triggered = false;
	m_yielded = false;
	m_anyKeyYielded = false;
	m_anyKeyPressed = false;
	m_deathYielded = false;
	m_playerOnlyTrig = true;
	m_deactivated = false;

	m_conditions = 0;
	m_timer = 0;

	h_yieldEnemy = nullptr;

	b2PolygonShape poly;
	poly.SetAsBox(hitboxSize.x / 2, hitboxSize.y / 2);
	b2FixtureDef def;
	def.isSensor = true;
	def.shape = &poly;
	def.filter.categoryBits = Box2DSingleton::TRIGGER;
	def.filter.maskBits = Box2DSingleton::CHARACTER;
	initSensorBody(def);

	m_yieldTimer = 0;
	setColor(Vector4(1.0f, 1.0f, 0.0f, .5f));
}

LuaTrigger::~LuaTrigger(){

}

LuaTrigger* LuaTrigger::clone(){
	return new LuaTrigger(m_threadState, getTag(), getName(), m_filename, getPosition(), getHitboxSize(), getRotation(), getId());
}

void LuaTrigger::step(float dt){
	if (m_timer <= .1f){
		m_timer += dt;
	}
	if (m_yieldTimer <= 0 && m_coState  && m_timer >= .1f)
	{
		m_yieldTimer = 0;
		if (m_yielded)
		{
			// if we are returning from a yield, our function needs no additional arguments.
			m_yielded = false;
			lua_resume(m_coState, nullptr, 0);
		}
		if (m_anyKeyYielded && m_anyKeyPressed)	
		{
			m_anyKeyYielded = false;
			m_anyKeyPressed = false;
			lua_resume(m_coState, nullptr, 0);
		}
		if (m_deathYielded){
			if (h_yieldEnemy == nullptr){
				m_deathYielded = false;
				lua_resume(m_coState, nullptr, 0);
			}
		}
		// resume our script. this will return once the script has yielded.
		if (m_triggered){
			if (m_conditions == 0){
				// if this is the first time we call resume, it needs to take in 3 arguments
				int mynum = lua_gettop(m_coState);
				if (lua_gettop(m_coState) >= 4){
					lua_resume(m_coState, nullptr, 3);
					m_triggered = false;
				}
			}
		}
	}
	else if (m_yielded)
	{
		m_yieldTimer -= dt;
	}
}

void LuaTrigger::onTrigger(GameObject* collider){
#ifndef _CRUSADE_EDITOR_
	if (m_timer <= .1f){
		if (collider->getTag().compare("ENEMY") == 0){
			m_containedOnStart.push_back((Enemy*)collider);
			((Enemy*)collider)->setContainingTrigger(this);
		}
	}
	if (!m_yielded && !m_deactivated && !m_triggered){	// we only want one instance of this trigger running at any one time.
		if (m_playerOnlyTrig)
		{
			if (collider->getTag().compare("PLAYER") == 0){
				m_coState = TriggerManager::Instance()->runScript(m_filename, this, collider);
				// lets check the preconditions
				// if they provided a precondition (its not nil)
				if (lua_isstring(m_coState, 9)){
					std::string precon = lua_tostring(m_coState, 9);
					if (precon.compare("ENEMIES_CONTAINED_DEAD") == 0)
						m_conditions = m_conditions | ENEMIES_CONTAINED_DEAD;
				}
				lua_pop(m_coState, 1);

				m_triggered = true;
				setColor(Vector4(0.0f, 1.0f, 0.0f, 0.5f));
			}
		}
		else
		{
			m_coState = TriggerManager::Instance()->runScript(m_filename, this, collider);
			// lets check the preconditions
			// if they provided a precondition (its not nil)
			if (lua_isstring(m_coState, 9)){
				std::string precon = lua_tostring(m_coState, 9);
				if (precon.compare("ENEMIES_CONTAINED_DEAD") == 0)
					m_conditions = m_conditions | ENEMIES_CONTAINED_DEAD;
			}
			lua_pop(m_coState, 1);

			m_triggered = true;

		}
	}

#endif
}

void LuaTrigger::onExitTrigger(GameObject* collider){
#ifndef _CRUSADE_EDITOR_
	if (m_coState && !m_deactivated){
		if (m_playerOnlyTrig){
			if (collider->getTag().compare("PLAYER") == 0){
				// ensure that we are about to call onExitTrigger.
				// (there should only be one function on the stack if so)
				m_triggered = true;
				setColor(Vector4(1.0f, 1.0f, 0.0f, .5f));
			}

		}
	}
#endif
}

void LuaTrigger::onAirTrigger(WorldTile* air){

}

void LuaTrigger::notifyOfDeath(Enemy* enemy){
	if (h_yieldEnemy == enemy){
		h_yieldEnemy = nullptr;
	}
	for (auto it = m_containedOnStart.begin(); it != m_containedOnStart.end();){
		if ((*it) == enemy)
		{
			it = m_containedOnStart.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (m_containedOnStart.size() == 0){
		// remove the precondition flag
		m_conditions &= ~(ENEMIES_CONTAINED_DEAD);
	}
}

void LuaTrigger::anyKeyPressed(){
	m_anyKeyPressed = true;
}

void LuaTrigger::yield(float time){
	m_yieldTimer = time;
	m_yielded = true;
	lua_yield(m_coState, 0);
}

void LuaTrigger::anyKeyYield(){
	m_anyKeyYielded = true;
	m_anyKeyPressed = false;
	lua_yield(m_coState, 0);
}

void LuaTrigger::deathYield(Enemy* enemy){
	m_deathYielded = true;
	h_yieldEnemy = enemy;
	lua_yield(m_coState, 0);
}

std::wstring LuaTrigger::getFilename(){
	return m_filename;
}

void LuaTrigger::Deactivate(){
	m_deactivated = true;
	setColor(Vector4(.7f, .7f, .7f, .5f));
}
