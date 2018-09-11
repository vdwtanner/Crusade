#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <shellapi.h>
#include "Game.h"
#include <filesystem>

class Game;



class AbilitySelectScreen : public Screen {
public:
	AbilitySelectScreen(Game* game, float width, float height);
	~AbilitySelectScreen();

	void setText(string text);
	void update(float dt);
	void handleKeyPress(MSG msg);

	enum MODE { CAMPAIGN, LEVEL, CONTINUE };
	void setWorld(wstring world, MODE mode);

	void continueCampaign(wstring campaign, UINT level);

	void seticons(UINT lv);
	ID3D11ShaderResourceView** getHudIcons();

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();


private:
	int m_selected;
	float m_timer;
	int m_numLines;
	int m_offset;
	wstring m_world;
	UINT m_contLevel;
	
	MODE m_mode;

	AbilityManager* h_abiManager;
	vector<ID3D11ShaderResourceView*> m_icons;
	//The vector of ability Ids
	vector<UINT> m_ids;
	ID3D11ShaderResourceView* m_hudIcons[4];

	void incrementSelection();
	void decrementSelection();
	void execute();
	void bindAbilityToKey(WPARAM key);
	int printActive(UINT id);

	Game* h_game;
	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_loadBatch;
	float m_screenWidth;
	float m_screenHeight;

	unique_ptr<Character> m_tempCharacter;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_tempLoad;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_selectLoad;
};