#pragma once

#include "pch.h"
#include "Screen.h"
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include "Game.h"

class LoadingScreen : public Screen {
public:
	LoadingScreen(string text, float width, float height);
	~LoadingScreen();

	void setText(string text);
	string getText();

	//Screen overrides
	void dispose();
	void hide();
	void pause();
	void render(float dt);
	void resize(int width, int height);
	void resume();
	void show();

	void initResources();

private:
	string m_text;

	D3DManager* h_d3d;
	std::unique_ptr<DirectX::SpriteFont>		m_font;
	std::unique_ptr<DirectX::SpriteBatch>		m_loadBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backgroundSRV;
	float m_screenWidth;
	float m_screenHeight;
	
};