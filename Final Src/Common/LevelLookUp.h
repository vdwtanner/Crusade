#pragma once

#include "pch.h"
#include "rapidxml.hpp"
#include <string>
#include <map>
#include <set>
#include "CrusadeUtil.h"
#include <filesystem>

using namespace rapidxml;

class LevelLookUp {
public:
	struct LevelDescription{
		UINT level;
		UINT exp;
		vector<UINT> abilities;
		map<UINT, vector<UINT>> passives;
		UINT hp;
		UINT mana;
		UINT manaRegen;
	};

	LevelLookUp();
	LevelLookUp(wstring file);
	~LevelLookUp();

	bool loadLevelData(wstring file);
	vector<UINT> parseAbilities(string text);
	map<UINT, vector<UINT>> parsePassives(string text);

	LevelDescription getLevelDescription(UINT level);
	UINT getLevel(UINT exp);
	vector<UINT> getAbilities(UINT level);
	map<UINT, vector<UINT>> getPassives(UINT level);
	UINT getHP(UINT level);
	UINT getMana(UINT level);
	UINT getManaRegen(UINT level);
	UINT getExpForNextLevel(UINT level);
private:
	wstring m_xmlPath;
	map<UINT, LevelDescription> m_levelData;
};