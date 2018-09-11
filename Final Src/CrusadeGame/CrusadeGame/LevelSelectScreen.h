#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <shellapi.h>
#include "Game.h"

class Game;



class LevelSelectScreen : public Screen {
public:
	LevelSelectScreen(Game* game, float width, float height);
	~LevelSelectScreen();

	void setText(string text);
	void update(float dt);
	void handleKeyPress(MSG msg);

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();

	enum MODE {CAMPAIGN, LEVEL};
	void setMode(MODE mode);
	MODE getMode();

	void clearWorldsVector();

	Vector2 measureTextSize(LPWSTR text, Vector2 scale);
	void loadLevels();
	void initResources();
private:
	int m_selected;
	float m_timer;
	vector<LPWSTR> m_worlds;
	MODE m_mode;
	int m_numLines;
	int m_offset;

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