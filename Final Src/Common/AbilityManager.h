#pragma once

#include "pch.h"
#include "rapidxml.hpp"
#include <string>
#include <map>
#include <set>
#include "CrusadeUtil.h"

#include "TriggerVolume.h"
#include "Ability.h"
// include all the types of skills
#include "Fireball.h"
#include "HolyMissile.h"
#include "HolyFire.h"
#include "Slash.h"
#include "Heal.h"

#include <filesystem>

class AbilityManager{

	struct ptr_less{
		template<class T>
		bool operator()(const T& left, const T& right) const{
			return ((*left) < (*right));
		}
	};
public:
	static const int MAX_ABILITIES = 256;

	static AbilityManager* Instance(LPWSTR path);
	static AbilityManager* Instance(std::string path);
	static AbilityManager* Instance();
	~AbilityManager();
	bool changeAbilityList(std::string path);

	bool loadAbilityData(LPWSTR path);
	bool isWorldEditMode();
	void setWorldEditMode(bool mode);
	bool AbilitySetIsLoaded();

	bool castAbility(UINT id, GameObject* caster, _Out_ Ability** ability_out);
	bool deleteAbility(Ability* ability);
	void deleteAllAbilities();

	UINT getRenderSetSize();
	std::multiset<TriggerVolume*, ptr_less>* getRenderSet();

	//UINT getAnimSize(UINT id, Ability::ANIMATION_TYPE type);
	//UINT getAnimIndex(UINT id, Ability::ANIMATION_TYPE type);
	void save(std::ofstream *file);
	void load(std::ifstream *file);

	void renderAbilities();
	void stepAbilities(float dt);
	void interpolate(float alpha);

	UINT getAnimSize(UINT id);
	UINT getAnimIndex(UINT id);

	float getCooldown(UINT index);

	void setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context);

	Vector2 getMaxHitboxSize(UINT id);

	int getManaCost(UINT id, GameObject* caster);
	Ability::AbilityDescription getAbilityDescription(UINT id);

	void pause();
	void resume();

private:
	static AbilityManager* m_instance;
	AbilityManager();
	AbilityManager(LPWSTR path);
	AbilityManager(std::string path);
	std::string m_xmlPath;
	std::map<UINT, Ability*> m_abilities;
	std::multiset<Ability*, ptr_less> m_renderSet;
	void loadAbilityAnimations(UINT id, std::tr2::sys::path path);
	// remember the texture paths... er. just in case
	std::map<UINT, std::vector<wchar_t*>>		m_texPaths;

	// one texture array for each Ability type, holding all animation frames
	std::map<UINT, ID3D11ShaderResourceView *>	m_texArrays;

	// map to keep track of the offset and size of each animation
	//       id			start index  size
	std::map<UINT, std::pair<UINT, UINT>>		m_indexSizeMap;

	std::tr2::sys::path relativePathAdjust;

	// holds all our Ability verts to be rendered
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;

	ID3D11Device			*h_device;
	ID3D11DeviceContext		*h_context;

	bool loadAbilityData();

	bool m_paused;
};