#include "pch.h"
#include "HUD.h"

HUD::HUD(float screenx, float screeny, Character* player){
	this->m_screenx = screenx;
	this->m_screeny = screeny;
	this->m_textBox = false;
	this->h_player = player;

	this->m_name = L"";
	this->m_text = L"";
	this->m_txtovr = L"";

	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/helmet.png",
		nullptr,
		HelmLoad.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/healthbar.png",
		nullptr,
		HealthLoad.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/manabar.png",
		nullptr,
		ManaLoad.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/bar.png",
		nullptr,
		BarLoad.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/box.png",
		nullptr,
		BoxLoad.ReleaseAndGetAddressOf()));


	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	m_loadBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));
}

HUD::~HUD(){
	m_font.reset(nullptr);
	m_loadBatch.reset(nullptr);
	HelmLoad.Reset();
	HealthLoad.Reset();
	ManaLoad.Reset();
	BarLoad.Reset();
	BoxLoad.Reset();
}

void HUD::Draw(){
	//set up rectangles
	RECT HelmRc;
	HelmRc.top = 0;
	HelmRc.left = 0;
	HelmRc.bottom = (long)(m_screeny * 0.1f);
	HelmRc.right = HelmRc.bottom;

	RECT healthRc;
	healthRc.top = 5;
	healthRc.left = HelmRc.right + 5;
	healthRc.bottom = (long)(healthRc.top + m_screeny * 0.03f);
	healthRc.right = (long)(healthRc.left + (healthRc.bottom - healthRc.top) * 6 * ((float )h_player->getHp() / (float)h_player->getHpMax() ));

	RECT manaRc;
	manaRc.top = healthRc.bottom + 10;
	manaRc.left = HelmRc.right + 5;
	manaRc.bottom = (long)(manaRc.top + m_screeny * 0.03f);
	manaRc.right = (long)(manaRc.left + (manaRc.bottom - manaRc.top) * 6 * ((float)h_player->getMana() / (float)h_player->getManaMax()));
	
	//Show Player data
	//HEALTH
	string temp = "HP:" + to_string(h_player->getHp());
	wchar_t* whp;
	CrusadeUtil::s_to_ws(temp, &whp);
	//Mana
	string temp2 = "MP: " + to_string(h_player->getMana());
	wchar_t* wmp;
	CrusadeUtil::s_to_ws(temp2, &wmp);
	
	//ability rectangle
	RECT abilRc;
	abilRc.left = 5;
	abilRc.bottom = (long)(m_screeny - 5);
	abilRc.top = abilRc.bottom - 40;
	abilRc.right = abilRc.left + 40;

	//font size
	float fsize = m_screeny * 0.0008f;

	//draw all the things
	m_loadBatch->Begin(DirectX::SpriteSortMode_Deferred,PipelineStates::BasicAlpha );
	m_loadBatch->Draw(HelmLoad.Get(), HelmRc);
	m_loadBatch->Draw(HealthLoad.Get(), healthRc);
	m_loadBatch->Draw(ManaLoad.Get(), manaRc);
	m_font->DrawString(m_loadBatch.get(), whp, Vector2((float)healthRc.left, (float)healthRc.top), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(fsize, fsize));
	m_font->DrawString(m_loadBatch.get(), wmp, Vector2((float)manaRc.left, (float)manaRc.top), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(fsize, fsize));
	
	//ability numbers
	for (int i = 0; i < 4; i++){
		if (m_hudIcons[i] != nullptr)
		m_loadBatch->Draw(m_hudIcons[i], abilRc);
		//1st cooldown
		manaRc.top = (long)(abilRc.bottom - 40 * (h_player->getCooldownTimer(i) / h_player->getCooldown(i)));
		manaRc.left = abilRc.left;
		manaRc.bottom = abilRc.bottom;
		manaRc.right = abilRc.right;
		m_loadBatch->Draw(ManaLoad.Get(), manaRc);

		abilRc.left += 50;
		abilRc.right += 50;
	}


	if (m_textBox){
		manaRc.top = (long)(m_screeny / 1.5f);
		manaRc.left = 0;
		manaRc.bottom = (long)m_screeny;
		manaRc.right = (long)m_screenx;
		m_loadBatch->Draw(BoxLoad.Get(), manaRc);
		m_font->DrawString(m_loadBatch.get(), m_name.c_str(), Vector2((m_screenx/40), manaRc.top + (m_screeny/50)), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f * (m_screenx/800.0f), .55f * (m_screeny/600.0f)));
		m_font->DrawString(m_loadBatch.get(), L"Press Enter to Continue...", Vector2((m_screenx / 40), manaRc.bottom - (m_screeny / 18)), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.50f * (m_screenx / 800.0f), .50f * (m_screeny / 600.0f)));
		manaRc.top += (long)(50 * (m_screeny / 600.0f));
		manaRc.left += (long)((m_screenx/40));
		manaRc.right = (long)(m_screenx - (m_screenx/20));
		manaRc.bottom = (long)((m_screeny * 0.25f) - 10);
		m_txtovr = CrusadeUtil::drawTextBox(m_font.get(), m_loadBatch.get(), m_text, manaRc, Vector2(.55f * (m_screeny / 600.0f), .55f * (m_screeny / 600.0f)));

	}

	m_loadBatch->End();
}

void HUD::reSize(float screenx, float screeny){
	m_screenx = screenx;
	m_screeny = screeny;
}

void HUD::displayTextBox(wstring name, wstring text){
	m_textBox = true;
	m_name = name;
	m_text = text;
}

bool HUD::textBoxDisplayed(){
	return m_textBox;
}

bool HUD::scrollTextBox(){
	if (m_textBox){
		if (m_txtovr.compare(L"") != 0){
			m_text = m_txtovr;
			return true;
		}
		else
		{
			m_textBox = false;
			return false;
		}
	}
	return false;
}

void HUD::sethudIcons(ID3D11ShaderResourceView* Icons[]){
	for (int i = 0; i < 4; i++)
		m_hudIcons[i] = Icons[i];
}