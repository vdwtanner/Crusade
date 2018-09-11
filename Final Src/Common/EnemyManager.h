#pragma once

#include "pch.h"
#include "rapidxml.hpp"
#include <string>
#include <map>
#include <set>
#include "CrusadeUtil.h"
#include "Enemy.h"
#include "MeleeEnemy.h"
#include "MageEnemy.h"
#include "RangedEnemy.h"
#include <filesystem>

using namespace rapidxml;

class EnemyManager{
public:
	struct ptr_less{
		template<class T>
		bool operator()(const T& left, const T& right) const{
			return ((*left) < (*right));
		}

	};

	static const int MAX_ENEMIES = 256;

	static EnemyManager* Instance(LPWSTR path);
	static EnemyManager* Instance(std::string path);
	static EnemyManager* Instance();
	bool changeEnemyList(std::string path);
	void populateListbox(HWND listbox_hwnd);

	bool createEnemy(UINT id, _Out_ Enemy** enemy_out);
	bool deleteEnemyAt(float worldx, float worldy);
	void deleteAllEnemies();
	bool deleteEnemy(Enemy* enemy);

	bool loadEnemyData(LPWSTR path);
	bool isWorldEditMode();
	void setWorldEditMode(bool mode);
	bool EnemySetIsLoaded();

	UINT getRenderSetSize();
	std::multiset<Enemy*, ptr_less>* getRenderSet();

	UINT getAnimSize(UINT id, Enemy::ANIMATION_TYPE type);
	UINT getAnimIndex(UINT id, Enemy::ANIMATION_TYPE type);

	std::string getXMLPath();

	void renderEnemies();
	void stepEnemies(float dt);
	void interpolate(float alpha);

	void setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context);

	~EnemyManager();

	void pause();
	void resume();
private:
	static EnemyManager* m_instance;
	EnemyManager();
	EnemyManager(std::string path);
	EnemyManager(LPWSTR path);
	bool m_worldEditMode;
	std::string m_xmlPath;
	std::map<UINT, Enemy*> m_enemies;
	std::multiset<Enemy*, ptr_less> m_renderSet;
	void loadEnemyAnimations(UINT id, std::tr2::sys::path path);

	map<float, string> parseSounds(string csv);
	// remember the texture paths... er. just in case
	std::map<UINT, std::vector<wchar_t*>>		m_texPaths;

	// one texture array for each enemy type, holding all animation frames
	std::map<UINT, ID3D11ShaderResourceView *>	m_texArrays;

	// map to keep track of the offset and size of each animation
	//       id					start index  size
	std::map<UINT, std::vector<std::pair<UINT, UINT>>>		m_indexSizeMap;

	std::tr2::sys::path relativePathAdjust;

	// holds all our enemy verts to be rendered
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;

	ID3D11Device			*h_device;
	ID3D11DeviceContext		*h_context;

	bool loadEnemyData();

	bool m_paused;
	
};