#include "pch.h"
#include "Profile.h"

Profile::Profile(string name, LevelLookUp* llu){
	m_exp = 0;
	m_level = 1;
	m_name = name;
	m_currCampaign = L"";
	m_currCampaignLevel = 0;
	m_levelLookUp = llu;
	load();
}

Profile::~Profile(){
	//delete m_levelLookUp;
}

bool Profile::load(){
	string temp = "../img/Profiles/" + m_name + ".crusader";
	wchar_t* path;
	CrusadeUtil::s_to_ws(temp, &path);
	LPWSTR filepath = (LPWSTR)path;
	ifstream file(filepath, ios::in | ios::binary);
	m_currCampaign = L"";
	m_currCampaignLevel = 0;
	//Initialize player level 1 abilities (only Holy Missile)
	for (int x = 0; x < 4; x++){
		m_activeAbilities[x] = 0;
		m_passiveAbilities[x] = 0;
	}
	m_activeAbilities[0] = 7;
	if (!file){
		file.close();
		return false;
	}
	file.seekg(0, std::ios::beg);
	//Read exp and level, name is in filename
	file.read((char*)&m_exp, 4);
	calcLevel();
	//file.read((char*)&m_level, 4);

	//Read campaign info
	UINT campaignSize;
	file.read((char*)&campaignSize, 4);
	wchar_t campaignBuffer[261];
	file.read((char*)&campaignBuffer, campaignSize * sizeof(wchar_t));
	campaignBuffer[campaignSize] = '\0';
	m_currCampaign = wstring(campaignBuffer);
	file.read((char*)&m_currCampaignLevel, 4);

	//Read ability info (active/passive paired)
	for (int x = 0; x < 4; x++){
		file.read((char*)&m_activeAbilities[x], 4);
		file.read((char*)&m_passiveAbilities[x], 4);
	}


	file.close();
	return true;
}

bool Profile::save(){
	string temp = "../img/Profiles/" + m_name + ".crusader";
	wchar_t* path;
	CrusadeUtil::s_to_ws(temp, &path);
	LPWSTR filepath = (LPWSTR)path;
	ofstream file(filepath, ios::out | ios::binary);
	if (!file){
		return false;
	}
	//Write exp; name is in filename and level is calculated from EXP
	file.write((char*)&m_exp, 4);
	//file.write((char*)&m_level, 4);

	//Write campaign data next (next peice that we would want to read if we were reading without the help of Profile
	UINT campaignSize = m_currCampaign.size();
	file.write((char*)&campaignSize, 4);
	file.write((char*)m_currCampaign.c_str(), campaignSize * sizeof(wchar_t));
	file.write((char*)&m_currCampaignLevel, 4);
	
	//Write ability info (active/passive paired)
	for (int x = 0; x < 4; x++){
		file.write((char*)&m_activeAbilities[x], 4);
		file.write((char*)&m_passiveAbilities[x], 4);
	}
	file.close();
	return true;
}

//Add exp to this player profile. Returns the number of levels that the player went up due to the addition
int Profile::addExp(int exp){
	int old = m_level;
	m_exp += exp;
	return calcLevel() - old;
}

UINT Profile::getExp(){
	return m_exp;
}

UINT Profile::getExpToNextLevel(){
	return m_levelLookUp->getExpForNextLevel(m_level) - m_exp;
}

int Profile::getLevel(){
	return m_level;
}

int Profile::calcLevel(){
	if (!m_levelLookUp){
		m_level = Profile::calcLevel(m_exp);
		return m_level;
	}
	m_level = m_levelLookUp->getLevel(m_exp);
	return m_level;
}

/*DEPRECATED
This is a psudo level calculation for temporary use.*/
int Profile::calcLevel(int exp){
	int level = 1;
	while (exp >= 300){
		exp /= 3;
		level++;
	}
	return level;
}

string Profile::getName(){
	return m_name;
}

//set the active abilities
void Profile::setActiveAbilities(UINT actives[4]){
	for (int x = 0; x < 4; x++){
		m_activeAbilities[x] = actives[x];
	}
}

void Profile::setActiveAbility(int index, UINT ability){
	for (int x = 0; x < 4; x++){
		if (m_activeAbilities[x] == ability)
			m_activeAbilities[x] = 0;
	}
	m_activeAbilities[index] = ability;
}

//returns a pointer to the array UINT m_activeAbilities[4]
UINT* Profile::getActiveAbilities(){
	return m_activeAbilities;
}

//set the Passive abilities
void Profile::setPassiveAbilities(UINT passives[4]){
	for (int x = 0; x < 4; x++){
		m_passiveAbilities[x] = passives[x];
	}
}

void Profile::setPassiveAbility(int index, UINT passive){
	for (int x = 0; x < 4; x++){
		if (m_passiveAbilities[x] == passive)
			m_passiveAbilities[x] = 0;
	}
	m_passiveAbilities[index] = passive;
}

//Returns a pointer to the array UINT m_passiveAbilities[4]
UINT* Profile::getPassiveAbilities(){
	return m_passiveAbilities;
}

LevelLookUp* Profile::getLevelLookUp(){
	return m_levelLookUp;
}

void Profile::setCurrentCampaign(wstring campaign){
	m_currCampaign = campaign;
}

wstring Profile::getCurrentCampaign(){
	return m_currCampaign;
}

void Profile::setCurrentCampaignLevel(UINT level){
	m_currCampaignLevel = level;
}

UINT Profile::getCurrentCampaignLevel(){
	return m_currCampaignLevel;
}