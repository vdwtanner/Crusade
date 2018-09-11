#include "pch.h"
#include "Campaign.h"

Campaign::Campaign(LPWSTR filepath){
	m_name = wstring(filepath).substr(0, wstring(filepath).length() - 4);
	loadCampaign(filepath);
	m_currentLevel = 0;
}

Campaign::~Campaign(){

}

//parse csv at LPWSTR filepath to load campaign data.
void Campaign::loadCampaign(LPWSTR filepath){
	ifstream ccf(filepath, std::ios::in);
	//ccf.open(filepath);
	if (!ccf){
		return;
	}
	string csv;
	getline(ccf, csv);
	csv.erase(remove_if(csv.begin(), csv.end(), isspace), csv.end()); //Remove spaces that were there for readability
	vector<string> levels = CrusadeUtil::split(csv, ';');
	for (string level : levels){
		level = level.substr(1, level.length() - 2);
		vector<string> data = CrusadeUtil::split(level, ',');
		string temp = "../img/Worlds/" + data[0] + ".crwf";
		wchar_t* wtemp;
		CrusadeUtil::s_to_ws(temp, &wtemp);
		m_filepaths.push_back(wtemp);
		if (data[1].length() > 0){
			m_difficultyMultipliers.push_back((float)stoi(data[1]));
		}
		else{
			m_difficultyMultipliers.push_back(1);
		}
	}
}

//get all filepaths
vector<LPWSTR>* Campaign::getFilePaths(){
	return &m_filepaths;
}

//get a specific filepath
LPWSTR Campaign::getFilePath(int index){
	return m_filepaths[index];
}

vector<float>* Campaign::getDifficultyMultipliers(){
	return &m_difficultyMultipliers;
}

float Campaign::getDifficultyMultiplier(int index){
	return m_difficultyMultipliers[index];
}

void Campaign::setCurrentLevel(int level){
	m_currentLevel = level;
}

int Campaign::getCurrentLevel(){
	return m_currentLevel;
}

//Returns the path to the next level of the campaign if there is another level. If there isn't another level it returns L"CAMPAIGN FINISHED"
LPWSTR Campaign::getNextLevel(){
	if (m_currentLevel+1 < (int)m_filepaths.size()){
		return getFilePath(++m_currentLevel);
	}
	else
		return L"CAMPAIGN FINISHED";
}

wstring Campaign::getName(){
	return m_name;
}

wstring Campaign::getFilename(){
	return m_name + L".ccf";
}