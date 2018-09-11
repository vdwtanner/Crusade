#include "pch.h"
#include "LevelLookUp.h"

LevelLookUp::LevelLookUp(){
	loadLevelData(L"../img/Levelling.xml");
}

LevelLookUp::LevelLookUp(wstring file){
	loadLevelData(file);
}

LevelLookUp::~LevelLookUp(){
	m_levelData.clear();
}

bool LevelLookUp::loadLevelData(wstring file){
	string xml = CrusadeUtil::loadFileAsString((LPWSTR)file.c_str());
	xml_document<> doc;
	doc.parse<0>((char*)xml.c_str());
	xml_node<>* level = doc.first_node()->first_node("Levelling");
	if (!level){
		return false;
	}
	do{
		LevelDescription ld;
		ld.level = stoi(level->first_node("Level")->value());
		ld.exp = stoi(level->first_node("EXP")->value());
		if (level->first_node("Abilities")){
			ld.abilities = parseAbilities(level->first_node("Abilities")->value());
		}
		else{
			ld.abilities = parseAbilities("-1");
		}
		if (level->first_node("Passives")){
			ld.passives = parsePassives(level->first_node("Passives")->value());
		}
		else{
			ld.passives = parsePassives("-1");
		}
		ld.hp = stoi(level->first_node("HP")->value());
		ld.mana = stoi(level->first_node("Mana")->value());
		ld.manaRegen = stoi(level->first_node("ManaRegen")->value());
		m_levelData.emplace(ld.level, ld);
		level = level->next_sibling();
	} while (level);
	CrusadeUtil::debug("Level Data Loaded: " + std::to_string(m_levelData.size()));
	CrusadeUtil::debug("\n");
	return true;
}

/*Parse ability data from a string of text. An ability of 0 or less is interpretted as no ability.
An empty vector will be returned if there are no abilities.*/
vector<UINT> LevelLookUp::parseAbilities(string text){
	text.erase(remove_if(text.begin(), text.end(), isspace), text.end());
	vector<string> parts = CrusadeUtil::split(text, ',');
	vector<UINT> abilities;
	for (string s : parts){
		int x = stoi(s);
		if (x > 0){
			abilities.push_back((UINT)x);
		}
	}
	return abilities;
}

/*Parse passive ability data from string of text.
 Passives are represented as smeicolon separated ordered pairs. First number of ordered pair is the main ability, second number is the passive. For example:
  (4,1);(2,2)
  will be interpretted as "the first passive of ability 4 and the second passive of ability 2"
  An empty map will be returned if there are no passives.*/
map<UINT, vector<UINT>> LevelLookUp::parsePassives(string text){
	map<UINT, vector<UINT>> passives;
	text.erase(remove_if(text.begin(), text.end(), isspace), text.end());
	if (text.compare("-1") == 0){
		return passives;//No passives
	}
	vector<string> parts = CrusadeUtil::split(text, ';');
	vector<string> nums;
	for (string pair : parts){
		nums = CrusadeUtil::split(pair.substr(1, pair.length() - 2), ',');
		UINT key = (UINT)stoi(nums[0]);
		vector<UINT> values;
		UINT value = (UINT)stoi(nums[1]);
		auto iterator = passives.find(key);
		if (iterator == passives.end()){
			values.push_back(value);
			passives.emplace(key, values);
		}
		else{
			values = iterator->second;
			if (find(values.begin(), values.end(), value) == values.end()){
				values.push_back(value);
				passives.insert(std::pair<UINT, vector<UINT>>(key, values));
			}
		}
	}
	return passives;
}

/*Get the description of the specified level. Not that each description only has the abilites that you get on that level, not all abilities up to that level. Use the getters for abilities and passives to find cumulative data.*/
LevelLookUp::LevelDescription LevelLookUp::getLevelDescription(UINT level){
	if (level >= m_levelData.size()){
		LevelDescription ld = m_levelData[m_levelData.size() - 1];
		ld.hp += (level - m_levelData.size()) * 15;
		ld.mana += (level - m_levelData.size()) * 15;
		return ld;
	}
	return m_levelData.at(level);
}

/*Determine level based off of exp. It is possible to have a higher level than the max defined in m_levelData. The player will be given bonus health and mana.*/
UINT LevelLookUp::getLevel(UINT exp){
	UINT level = 1;
	while (level < m_levelData.size() && m_levelData.at(level+1).exp <= exp){
		level++;
	}
	if (level >= m_levelData.size()){
		UINT reqExp = (UINT)(m_levelData.at(level).exp*1.5);
		while (exp >= reqExp){
			level++;
			reqExp = (long)(reqExp * 1.5);
		}
	}
	return level;
}

/*Get all abilities available to a character at the specified level*/
vector<UINT> LevelLookUp::getAbilities(UINT level){
	vector<UINT> abilities;
	for (UINT x = 1; x <= level; x++){
		auto it = m_levelData.find(x);
		//Make sure that we are looking for a level that actually exists in our level data
		if (it != m_levelData.end()){
			if (it->second.abilities.size() > 0){
				for (UINT ability : it->second.abilities){
					//check if ability already added
					auto iter = find(abilities.begin(), abilities.end(), ability);
					if (iter == abilities.end()){
						//add if not already in abilities
						abilities.push_back(ability);
					}
				}
			}
		}
	}
	return abilities;
}

/*Get all passives available to a character at the specified level*/
map<UINT, vector<UINT>> LevelLookUp::getPassives(UINT level){
	map<UINT, vector<UINT>> passives;
	for (UINT x = 1; x <= level; x++){
		auto it = m_levelData.find(x);
		//Make sure that we are looking for a level that actually exists in our level data
		if (it != m_levelData.end()){
			//merge maps
			passives.insert(it->second.passives.begin(), it->second.passives.end());
		}
	}
	return passives;
}

UINT LevelLookUp::getHP(UINT level){
	if (level >= m_levelData.size()){
		UINT hp = m_levelData[m_levelData.size() - 1].hp;
		hp += (level - m_levelData.size()) * 15;
		return hp;
	}
	return m_levelData[level].hp;
}

UINT LevelLookUp::getMana(UINT level){
	if (level >= m_levelData.size()){
		UINT mana = m_levelData[m_levelData.size() - 1].mana;
		mana += (level - m_levelData.size()) * 15;
		return mana;
	}
	return m_levelData[level].mana;
}

UINT LevelLookUp::getManaRegen(UINT level){
	if (level >= m_levelData.size()){
		UINT regen = m_levelData[m_levelData.size() - 1].manaRegen;
		regen += (level - m_levelData.size()) / 3;
		return regen;
	}
	return m_levelData[level].manaRegen;
}

/*Get the EXP threshold for the next level. This is the total value, not the difference between the player's current EXP and the amount required for next level*/
UINT LevelLookUp::getExpForNextLevel(UINT level){
	if (m_levelData.upper_bound(level+1) == m_levelData.end()){
		auto it = m_levelData.end();
		UINT reqExp = (UINT)((--it)->second.exp*1.5f);
		UINT lvl = it->first;
		while (level > lvl){
			lvl++;
			reqExp = (long)(reqExp * 1.5f);
		}
		return reqExp;
	}
	return m_levelData[level + 1].exp;

/*	UINT level = 1;
	while (level < m_levelData.size() && m_levelData.at(level + 1).exp <= exp){
		level++;
	}
	if (level >= m_levelData.size()){
		UINT reqExp = m_levelData.at(level).exp*1.5;
		while (exp >= reqExp){
			level++;
			reqExp *= 1.5;
		}
	}
	return level;*/
}