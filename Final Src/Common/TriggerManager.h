#pragma once

#include "pch.h"
#include "rapidxml.hpp"
#include <string>
#include <map>
#include <set>
#include "CrusadeUtil.h"

#include "TriggerVolume.h"
#include "LuaTrigger.h"
#include "LuaBindings.h"
#include "SpriteFont.h"
#include "Camera.h"

#include <filesystem>

/*
	Unlike the other managers, TriggerManager doesnt render
	its objects, since triggers are invisible.
	If we want backgrounds for triggers, put tiles in a layer behind the active layer
	to texture them
*/

class LuaBindings;

class TriggerManager{

public:
	struct ptr_less{
		template<class T>
		bool operator()(const T& left, const T& right) const{
			return ((*left) < (*right));
		}
	};

	static TriggerManager* Instance(LPWSTR path, LuaBindings* bindings);
	static TriggerManager* Instance();
	~TriggerManager();

	static const int MAX_TRIGGERS = 256;

	bool loadLuaData(LPWSTR path);
	void populateListbox(HWND listbox_hwnd);

	void setLuaBindings(LuaBindings*);
	LuaBindings *getLuaBindings();

	std::wstring getScriptFolder();

	bool isWorldEditMode();
	void setWorldEditMode(bool mode);
	bool isDebugMode();
	void setDebugMode(bool mode);
	bool LuaDataIsLoaded();

	void debugRender(Camera*);
	void stepTriggers(float dt);
	void interpolate(float alpha);

	bool createTrigger(std::wstring filename, Vector2 p1, Vector2 p2, _Out_ LuaTrigger** trigger_out);
	bool deleteTriggerAt(float worldx, float worldy);
	bool deleteTrigger(LuaTrigger* trigger);
	void deleteAllTriggers();

	void buildD3DResources();

	lua_State* runScript(std::wstring, LuaTrigger* context, GameObject* collider);

	std::multiset<LuaTrigger*, ptr_less>* getActiveTriggers();

	void anyKeyPressed();


private:
	LuaBindings *h_bindings;

	TriggerManager(LPWSTR path);
	TriggerManager();

	bool loadLuaData();

	static TriggerManager* m_instance;

	std::vector<std::string> m_luascripts;
	std::multiset<LuaTrigger*, ptr_less> m_activeTriggers;

	std::wstring m_scriptFolder;
	std::vector<std::wstring> m_filepaths;

	// holds all our debug verts to be rendered
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_spriteBatch;

	std::unique_ptr<BasicEffect>				m_effect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;

	bool m_trigDebugMode;
	bool m_worldEditMode;


};