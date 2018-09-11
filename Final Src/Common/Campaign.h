#pragma once

#include "pch.h"
#include "CrusadeUtil.h"

//Contains all information necessary to run a campaign: World order, world file locations, and world difficulty multipliers to increase difficulty and EXP of a level
class Campaign {
public:
	Campaign(LPWSTR filepath);
	~Campaign();

	void loadCampaign(LPWSTR filepath);

	//GETTERS
	vector<LPWSTR>* getFilePaths();
	LPWSTR getFilePath(int index);
	vector<float>* getDifficultyMultipliers();
	float getDifficultyMultiplier(int index);
	int getCurrentLevel();
	void setCurrentLevel(int level);
	LPWSTR getNextLevel();

	wstring getName();
	wstring getFilename();
private:
	vector<LPWSTR> m_filepaths;
	vector<float> m_difficultyMultipliers;
	int m_currentLevel;
	wstring m_name;
};