#pragma once

#include "pch.h"
#include "Rect.h"
#include <string>
#include <SpriteBatch.h>
#include <SpriteFont.h>

class HUD{
public:
	HUD(float screenx, float screeny, Character* player);
	~HUD();
	void Draw();
	void reSize(float screenx, float screeny);
	void displayTextBox(wstring name, wstring text);
	bool textBoxDisplayed();
	bool scrollTextBox();
	void sethudIcons(ID3D11ShaderResourceView* Icons[4]);

private:
	std::unique_ptr<SpriteBatch>							m_loadBatch;
	std::unique_ptr<SpriteFont>								m_font;
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		HelmLoad;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		HealthLoad;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		ManaLoad;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		BarLoad;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		BoxLoad;

	ID3D11ShaderResourceView* m_hudIcons[4];
	Character* h_player;

	float m_screenx;
	float m_screeny;

	bool m_textBox;
	wstring m_name;
	wstring m_text;
	wstring m_txtovr;

};