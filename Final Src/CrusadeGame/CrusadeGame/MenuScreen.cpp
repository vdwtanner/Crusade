#include "pch.h"
#include "MenuScreen.h"

MenuScreen::MenuScreen(Game* game, float width, float height){
	m_screenWidth = width;
	h_game = game;
	m_screenHeight = height;
	m_selected = PLAY;
	initResources();
}

MenuScreen::~MenuScreen(){
	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	m_backgroundSRV.Reset();
	h_d3d = nullptr;
}

void MenuScreen::initResources(){
	m_loadBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();
	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/MenuScreen.png",
		nullptr,
		m_backgroundSRV.ReleaseAndGetAddressOf()));
}

void MenuScreen::dispose(){

}

void MenuScreen::hide(){
	//AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
}
void MenuScreen::pause(){

}

//msg MUST be a WM_KEYDOWN message
void MenuScreen::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		if (msg.lParam >> 30 & 0x1){
			Sleep(100);
		}
		switch (msg.wParam){
		case VK_DOWN:
			m_selected = nextMenuItem(m_selected);
			break;
		case VK_UP:
			m_selected = previousMenuItem(m_selected);
			break;
		case VK_RIGHT:
			m_selected = nextMenuItem(m_selected);
			break;
		case VK_LEFT:
			m_selected = previousMenuItem(m_selected);
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
		case VK_BACK:
			if (m_selected > EXIT){
				m_selected = PLAY;
			}
			else{
				m_selected = EXIT;
				execute();
			}
			break;
		}
	}
}

void MenuScreen::execute(){
	switch (m_selected){
	case PLAY:
		m_selected = (h_game->getProfile()->getCurrentCampaignLevel() > 0) ? PLAY_CONTINUE : PLAY_CAMPAIGN;
		break;
	case PROFILE:
		h_game->getProfileScreen()->setMode(ProfileScreen::SELECT);
		h_game->showScreen(Game::PROFILE);
		break;
	case EDITOR:
		launchEditor();
		break;
	case EXIT:
		h_game->setExitCrusade(true);
		break;
	case PLAY_CONTINUE:
		h_game->getAbilitySelectScreen()->continueCampaign(h_game->getProfile()->getCurrentCampaign(), h_game->getProfile()->getCurrentCampaignLevel());
		h_game->getAbilitySelectScreen()->seticons(h_game->getProfile()->getLevel());
		h_game->showScreen(Game::ABILITY_SELECT);
		break;
	case PLAY_CAMPAIGN:
		h_game->getLevelSelectScreen()->setMode(LevelSelectScreen::CAMPAIGN);
		h_game->showScreen(Game::LEVEL_SELECT);
		/*h_game->getGameScreen()->loadCampaign(L"../img/Campaigns/campaign.ccf");
		h_game->showScreen(Game::GAME);*/
		break;
	case PLAY_LEVELS:
		h_game->getLevelSelectScreen()->setMode(LevelSelectScreen::LEVEL);
		h_game->showScreen(Game::LEVEL_SELECT);
		break;
	case PLAY_BACK:
		m_selected = PLAY;
		break;
	}
}

void MenuScreen::update(float dt){

}

void MenuScreen::render(float dt){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;

	wchar_t* text;
	CrusadeUtil::s_to_ws("PLAY", &text);
	Vector2 normalSize = measureTextSize(text, Vector2(1.0f, 1.0f));
	Vector2 textSize;
	m_loadBatch->Begin();
	m_loadBatch->Draw(m_backgroundSRV.Get(), screenRc);
	switch (m_selected){
	case PLAY:
	case PROFILE:
	case EDITOR:
	case EXIT:
		textSize = measureTextSize(text, (m_selected == PLAY) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f - normalSize.y), (m_selected == PLAY) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PLAY) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		CrusadeUtil::s_to_ws("PROFILE", &text);
		textSize = measureTextSize(text, (m_selected == PROFILE) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f), (m_selected == PROFILE) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PROFILE) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		CrusadeUtil::s_to_ws("LEVEL EDITOR", &text);
		textSize = measureTextSize(text, (m_selected == EDITOR) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f + normalSize.y), (m_selected == EDITOR) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == EDITOR) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		CrusadeUtil::s_to_ws("EXIT", &text);
		textSize = measureTextSize(text, (m_selected == EXIT) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f + normalSize.y * 2), (m_selected == EXIT) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == EXIT) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		break;
	case PLAY_CONTINUE:
	case PLAY_CAMPAIGN:
	case PLAY_LEVELS:
	case PLAY_BACK:
		
		if (h_game->getProfile()->getCurrentCampaignLevel() > 0){
			CrusadeUtil::s_to_ws("CONTINUE CAMPAIGN", &text);
			textSize = measureTextSize(text, (m_selected == PLAY_CONTINUE) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
			m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f - normalSize.y), (m_selected == PLAY_CONTINUE) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PLAY_CONTINUE) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		}
		
		CrusadeUtil::s_to_ws("PLAY CAMPAIGN", &text);
		textSize = measureTextSize(text, (m_selected == PLAY_CAMPAIGN) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f), (m_selected == PLAY_CAMPAIGN) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PLAY_CAMPAIGN) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		CrusadeUtil::s_to_ws("LEVEL SELECT", &text);
		textSize = measureTextSize(text, (m_selected == PLAY_LEVELS) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f + normalSize.y), (m_selected == PLAY_LEVELS) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PLAY_LEVELS) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		CrusadeUtil::s_to_ws("BACK", &text);
		textSize = measureTextSize(text, (m_selected == PLAY_BACK) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		m_font->DrawString(m_loadBatch.get(), text, Vector2(m_screenWidth*.5f - textSize.x / 2.0f, m_screenHeight*.5f - textSize.y / 2.0f + normalSize.y * 2), (m_selected == PLAY_BACK) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == PLAY_BACK) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		break;
	}
	
	m_loadBatch->End();
	/*RECT bounds = {m_screenWidth *.25, m_screenHeight*.75f, m_screenWidth*.5f, m_screenHeight*.20f};
	wstring test;
	wstring rest = CrusadeUtil::drawTextBox(L"Lorem Ipsum delor. Crusade is best game ever aw yeah!", bounds, Vector2(1.0f, 1.0f), Colors::Crimson, 0.0f, m_font.get(), m_loadBatch.get(), &test);*/
	delete[] text;
}

void MenuScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
}

void MenuScreen::resume(){

}

void MenuScreen::show(){
	render(0.1f);
	/*m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Stratosphere.wav", SoundEffectInstance_Default);
	m_bgmusic->Play();*/
}

MenuScreen::MENU_ITEM MenuScreen::nextMenuItem(MENU_ITEM m){
	switch (m){
	case PLAY:
		return PROFILE;
	case PROFILE:
		return EDITOR;
	case EDITOR:
		return EXIT;
	case EXIT:
		return PLAY;
	case PLAY_CONTINUE:
		return PLAY_CAMPAIGN;
	case PLAY_CAMPAIGN:
		return PLAY_LEVELS;
	case PLAY_LEVELS:
		return PLAY_BACK;
	case PLAY_BACK:
		return (h_game->getProfile()->getCurrentCampaignLevel() > 0) ? PLAY_CONTINUE : PLAY_CAMPAIGN;
	}
	//Should never reach this
	return (h_game->getProfile()->getCurrentCampaignLevel() > 0) ? PLAY_CONTINUE : PLAY_CAMPAIGN;
}

MenuScreen::MENU_ITEM MenuScreen::previousMenuItem(MENU_ITEM m){
	switch (m){
	case PLAY:
		return EXIT;
	case PROFILE:
		return PLAY;
	case EDITOR:
		return PROFILE;
	case EXIT:
		return EDITOR;
	case PLAY_CONTINUE:
		return PLAY_BACK;
	case PLAY_CAMPAIGN:
		return (h_game->getProfile()->getCurrentCampaignLevel() > 0) ? PLAY_CONTINUE : PLAY_BACK;
	case PLAY_LEVELS:
		return PLAY_CAMPAIGN;
	case PLAY_BACK:
		return PLAY_LEVELS;
	}
	//Should never reach this
	return (h_game->getProfile()->getCurrentCampaignLevel() > 0) ? PLAY_CONTINUE : PLAY_CAMPAIGN;
}

Vector2 MenuScreen::measureTextSize(LPWSTR text, Vector2 scale){
	Vector2 x = m_font->MeasureString(text);
	return Vector2(x.x*scale.x, x.y*scale.y);
}

void MenuScreen::launchEditor()
{
	ShellExecute(h_game->getWindow(), L"open", L"CrusadeLevelEditor.exe", L"", L"", SW_SHOW);
}