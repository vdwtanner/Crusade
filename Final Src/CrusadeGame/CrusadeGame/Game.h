//
// Game.h
//

#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "WorldTileMatrix.h"
#include "AbilityManager.h"
#include "AudioManager.h"
#include "TriggerManager.h"
#include "ParticleManager.h"
#include "Character.h"
#include "Camera.h"
#include <SpriteBatch.h>
#include "LoadingScreen.h"
#include "GameScreen.h"
#include "MenuScreen.h"
#include "LevelSelectScreen.h"
#include "ProfileScreen.h"
#include "AbilitySelectScreen.h"
#include "LevelLookUp.h"
#include "EndLevelScreen.h"

#include "LuaBindings.h"
#include "HUD.h"
class GameScreen;
class LuaBindings;
class LoadingScreen;
class MenuScreen;
class LevelSelectScreen;
class ProfileScreen;
class AbilitySelectScreen;
class EndLevelScreen;
class Profile;

// A basic game implementation that creates a D3D11 device and
// provides a game loop
class Game
{
public:
	//static Camera *m_camera;

    Game();
	~Game();

    // Initialization and management
    void Initialize(HWND window);

    // Basic game loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();
    void Present();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged();
	void OnNewAudioDevice();

	void disableInput();
	void enableInput();

    // Properites
    void GetDefaultSize( size_t& width, size_t& height ) const;

	//Getters
	TileSet* getTileSet();
	bool isWindActive();
	HWND getWindow();

	//SCREENS - Menu type screens (use keys to select things) should go after MENU in order to work
	enum SCREEN {GAME, LOADING, MENU, LEVEL_SELECT, PROFILE, ABILITY_SELECT, END_LEVEL};
	void showScreen(SCREEN screen);
	GameScreen* getGameScreen();
	MenuScreen* getMenuScreen();
	LoadingScreen* getLoadingScreen();
	LevelSelectScreen* getLevelSelectScreen();
	ProfileScreen* getProfileScreen();
	AbilitySelectScreen* getAbilitySelectScreen();
	EndLevelScreen* getEndLevelScreen();

	//Profile
	Profile* getProfile();
	void setProfile(Profile* profile);


	void quitGame(bool end);
	LevelLookUp* getLevelLookUp();


	SCREEN getCurrentScreen();

	void handleKeyPress(MSG msg);

	bool exitCrusade();
	void setExitCrusade(bool exit);

private:
	//SCREENS
	unique_ptr<GameScreen> m_gameScreen;
	unique_ptr<LoadingScreen> m_loadingScreen;
	unique_ptr<MenuScreen> m_menuScreen;
	unique_ptr<LevelSelectScreen> m_levelSelectScreen;
	unique_ptr<ProfileScreen> m_profileScreen;
	unique_ptr<AbilitySelectScreen> m_abilitySelectScreen;
	unique_ptr<EndLevelScreen> m_endLevelScreen;
	SCREEN m_currentScreen;
	
	//Profile
	unique_ptr<Profile> m_profile;

	unique_ptr<LevelLookUp> m_levelLookUp;

    void Update(DX::StepTimer const& timer);

	CSoundEffectInstance *m_bgmusic;

    // Application state
    HWND                                            m_window;

	D3DManager										*h_d3d;

	// The renderables
	//std::unique_ptr<Character>						m_character;
	std::unique_ptr<TileSet>						m_tileset;
	std::unique_ptr<WorldTileMatrix>				m_world;
	std::unique_ptr<SpriteBatch>					m_loadBatch;
	// Enemy Manager goes here in spirit.

	std::unique_ptr<LuaBindings>				m_bindings;

    // Game state
    DX::StepTimer                                   m_timer;

	bool											m_wndActive;
	bool											m_allowInput;
	bool											m_exit;
	float											m_lastTime[4];
};