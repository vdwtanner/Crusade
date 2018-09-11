#pragma once

#include "pch.h"
#include "Screen.h"
#include "Game.h"
#include "WorldTileMatrix.h"
#include "Campaign.h"
#include <SpriteBatch.h>
#include "SpriteFont.h"
#include "Camera.h"
#include "HUD.h"
#include <iomanip>


class Game;

class GameScreen : public Screen {
public:
	GameScreen(Game* game);
	~GameScreen();
	Game* getGame();
	Camera* getCamera();
	WorldTileMatrix* getWorldTileMatrix();
	HUD* getHUD();
	void loadWorld(LPWSTR filepath);

	void handleKeyPress(MSG msg);

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();

	void update(float dt);

	void enableMovementInput();
	void disableMovementInput();
	void enableAbilityInput();
	void disableAbilityInput();

	//CAMPAIGN
	int numLevelsRemaining();
	bool loadNextWorld();
	bool loadCampaign(LPWSTR filepath);
	bool loadCampaign(LPWSTR filepath, UINT level);
	float getLevelTimer();

	//BGMusic
	void PauseBGM();
	void ResumeBGM();
	void SwitchBGM(std::string song);
	void SetBGMVolume(float volume);

	Character* getCharacter();
private:
	Game* h_game;
	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_spriteBatch;
	Camera *m_camera;
	std::unique_ptr<WorldTileMatrix> m_world;
	std::unique_ptr<Campaign> m_campaign;

	LPWSTR m_currentWorldFile;

	RECT m_screenRc;
	
	std::unique_ptr<Character> m_character;

	bool m_loadNextWorld;
	//bool m_reloadWorld;

	std::unique_ptr<HUD> m_hud;

	bool m_movementEnabled;
	bool m_paused;
	float m_gameOverTimer;
	bool m_abilitiesEnabled;

	CSoundEffectInstance *m_bgmusic;

	//PAUSE OVERLAY
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_pauseBox;

	//EXTRA DATA
	float m_levelTimer;
	float m_timer;
};