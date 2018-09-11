#include "pch.h"
#include "EndLevelScreen.h"

EndLevelScreen::EndLevelScreen(Game* game, float width, float height){
	//m_screenWidth = width;

	h_game = game;
	//m_screenHeight = height;
	m_loadBatch.reset(nullptr);
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();
	resize((int)width, (int)height);
	m_selected = 0;
	m_offset = 0;

	m_loadBatch.reset(new SpriteBatch(h_d3d->getContext()));

	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/endLevelScreen.png",
		nullptr,
		&tempLoad));

}

EndLevelScreen::~EndLevelScreen(){
	if (m_bgmusic)
		AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);

	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	h_d3d = nullptr;
	tempLoad.Reset();
}

void EndLevelScreen::dispose(){

}

void EndLevelScreen::hide(){
	AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
	m_bgmusic = nullptr;
}
void EndLevelScreen::pause(){

}

void EndLevelScreen::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		if (msg.lParam >> 30 & 0x1){
			Sleep(100);
		}
		switch (msg.wParam){
		case VK_DOWN:
			incrementSelection();
			break;
		case VK_UP:
			decrementSelection();
			break;
		case VK_RIGHT:
			incrementSelection();
			break;
		case VK_LEFT:
			decrementSelection();
			break;
		case 'Z':
		case 'X':
		case 'C':
		case 'V':
		case VK_SPACE:
		case VK_RETURN:
			execute();
			break;
		case VK_ESCAPE:
			h_game->showScreen(Game::MENU);
			break;
		}
	}
}

void EndLevelScreen::execute(){
	if (m_nxtlvl)
		h_game->showScreen(Game::ABILITY_SELECT);
	else
		h_game->showScreen(Game::MENU);
}

void EndLevelScreen::update(float dt){

}

void EndLevelScreen::render(float dt){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;

	Vector2 textScale = Vector2(.7f * m_screenWidth / 800, .7f* m_screenWidth / 800);

	string lvs = "LV: " + to_string(m_level);
	wchar_t* wlv;
	CrusadeUtil::s_to_ws(lvs, &wlv);

	string exps = "New EXP: " + to_string(m_newEXP);
	wchar_t* wexp;
	CrusadeUtil::s_to_ws(exps, &wexp);

	int txtw = (int)(m_screenWidth / 2 - (m_screenWidth / 10));
	int txth = (int)(m_screenHeight / 2 - (m_screenWidth / 10));

	m_loadBatch->Begin();
	m_loadBatch->Draw(tempLoad.Get(), screenRc);
	
	m_font->DrawString(m_loadBatch.get(), wlv, Vector2((float)txtw, (float)txth), Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);
	m_font->DrawString(m_loadBatch.get(), wexp, Vector2((float)txtw, (float)txth + (m_screenHeight / 20)), Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);
	
	exps = "Old EXP: " + to_string(m_currEXP);
	CrusadeUtil::s_to_ws(exps, &wexp);
	m_font->DrawString(m_loadBatch.get(), wexp, Vector2((float)txtw, txth + (m_screenHeight / 20) * 2), Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);
	
	exps = "Total: " + to_string(m_newEXP + m_currEXP);
	CrusadeUtil::s_to_ws(exps, &wexp);
	m_font->DrawString(m_loadBatch.get(), wexp, Vector2((float)txtw, txth + (m_screenHeight / 20) * 3), Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);

	exps = "Next Level: " + to_string(m_nxtLevelEXP);
	CrusadeUtil::s_to_ws(exps, &wexp);
	m_font->DrawString(m_loadBatch.get(), wexp, Vector2((float)txtw, txth + (m_screenHeight / 20) * 5), Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);

	m_loadBatch->End();
}

void EndLevelScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
}

void EndLevelScreen::resume(){

}

void EndLevelScreen::show(){
	render(0.1f);
	m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Fanfare.adpcm", SoundEffectInstance_Default);
	m_bgmusic->Play();
}

void EndLevelScreen::incrementSelection(){

}

void EndLevelScreen::decrementSelection(){

}

void EndLevelScreen::setnxtlvl(bool next){
	m_nxtlvl = next;
}

void EndLevelScreen::setcurrEXP(UINT exp){
	m_currEXP = exp;
}

void EndLevelScreen::setnewEXP(UINT exp){
	m_newEXP = exp;
}

void EndLevelScreen::setnxtLevelEXP(UINT exp){
	m_nxtLevelEXP = exp;
}

void EndLevelScreen::setgameTime(float time){
	m_gameTime = time;
}

void EndLevelScreen::setlevel(UINT lv){
	m_level = lv;
}