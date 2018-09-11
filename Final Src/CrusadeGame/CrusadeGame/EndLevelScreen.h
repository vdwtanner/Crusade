#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <shellapi.h>
#include "Game.h"

class Game;



class EndLevelScreen : public Screen {
public:
	EndLevelScreen(Game* game, float width, float height);
	~EndLevelScreen();

	void setText(string text);
	void update(float dt);
	void handleKeyPress(MSG msg);

	void setnxtlvl(bool nxt);

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();

	void setcurrEXP(UINT exp);
	void setnewEXP(UINT exp);
	void setnxtLevelEXP(UINT exp);
	void setlevel(UINT lv);
	void setgameTime(float time);

private:
	int m_selected;
	float m_timer;
	int m_numLines;
	int m_offset;

	UINT m_currEXP;
	UINT m_newEXP;
	UINT m_nxtLevelEXP;
	UINT m_level;
	float m_gameTime;

	bool m_nxtlvl;

	void incrementSelection();
	void decrementSelection();
	void execute();

	Game* h_game;
	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_loadBatch;
	float m_screenWidth;
	float m_screenHeight;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		tempLoad;

	CSoundEffectInstance *m_bgmusic;
};