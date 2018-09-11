#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <shellapi.h>
#include "Game.h"

class Game;



class ProfileScreen : public Screen {
public:
	ProfileScreen(Game* game, float width, float height);
	~ProfileScreen();

	void setText(string text);
	void update(float dt);
	void handleKeyPress(MSG msg);
	void handleWM_CHAR(MSG msg);

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();

	enum MODE { SELECT, VIEW, NEW };
	void setMode(MODE mode);
	MODE getMode();

	void clearProfilesVector();
	void clearLevelsVector();

	void loadProfiles();
	void initResources();
private:
	int m_selected;
	float m_timer;
	vector<LPWSTR> m_profiles;
	vector<LPWSTR> m_levels;
	MODE m_mode;
	int m_numLines;
	int m_offset;
	wstring m_newName;

	int getCrusaderLevel(string crusaderFile);

	void incrementSelection();
	void decrementSelection();
	void execute();

	Game* h_game;
	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_loadBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundSRV;
	float m_screenWidth;
	float m_screenHeight;

};