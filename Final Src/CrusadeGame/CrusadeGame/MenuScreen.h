#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <shellapi.h>
#include "Game.h"

class Game;



class MenuScreen : public Screen {
public:
	MenuScreen(Game* game, float width, float height);
	~MenuScreen();

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
	
	enum MENU_ITEM{PLAY, PROFILE, EDITOR, EXIT, PLAY_CONTINUE, PLAY_CAMPAIGN, PLAY_LEVELS, PLAY_BACK};
	MENU_ITEM nextMenuItem(MENU_ITEM m);
	MENU_ITEM previousMenuItem(MENU_ITEM m);
	void execute();

	Vector2 measureTextSize(LPWSTR text, Vector2 scale);

	void initResources();

private:
	MENU_ITEM m_selected;
	float m_timer;

	void launchEditor();

	Game* h_game;
	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_loadBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundSRV;
	float m_screenWidth;
	float m_screenHeight;

	CSoundEffectInstance *m_bgmusic;

};

/*
enum class MenuItem{ PLAY, PROFILE, EXIT, END_OF_LIST };

// Special behavior for ++MenuItem
MenuItem& operator++(MenuItem &c) {
	using IntType = typename std::underlying_type<MenuItem>::type;
	c = static_cast<MenuItem>(static_cast<IntType>(c)+1);
	if (c == MenuItem::END_OF_LIST)
		c = static_cast<MenuItem>(0);
	return c;
}

// Special behavior for MenuItem++
MenuItem operator++(MenuItem &c, int) {
	MenuItem result = c;
	++c;
	return result;
	}*/