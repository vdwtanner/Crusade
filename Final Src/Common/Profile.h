#pragma once

#include "pch.h"
#include "CrusadeUtil.h"
#include "LevelLookUp.h"

class Profile{
public:
	Profile(string name, LevelLookUp* llu);
	~Profile();
	bool save();
	bool load();
	UINT getExp();
	UINT getExpToNextLevel();
	int addExp(int exp);
	int getLevel();
	int calcLevel();
	static int calcLevel(int exp);
	string getName();
	void setActiveAbilities(UINT actives[4]);
	void setActiveAbility(int index, UINT ability);
	UINT* getActiveAbilities();
	void setPassiveAbilities(UINT passives[4]);
	void setPassiveAbility(int index, UINT passive);
	UINT* getPassiveAbilities();
	LevelLookUp* getLevelLookUp();

	void setCurrentCampaign(wstring campaign);
	wstring getCurrentCampaign();
	void setCurrentCampaignLevel(UINT level);
	UINT getCurrentCampaignLevel();
private:
	UINT m_exp;
	int m_level;
	string m_name;
	LevelLookUp* m_levelLookUp;
	UINT m_activeAbilities[4];
	UINT m_passiveAbilities[4];
	wstring m_currCampaign;
	UINT m_currCampaignLevel;
};