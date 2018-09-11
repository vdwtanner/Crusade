#include "pch.h"
#include "LevelSelectScreen.h"

LevelSelectScreen::LevelSelectScreen(Game* game, float width, float height){
	//m_screenWidth = width;
	
	h_game = game;
	//m_screenHeight = height;
	m_selected = 0;
	m_offset = 0;
	//m_numLines = 1;
	m_mode = LEVEL;
	loadLevels();
	initResources();
	resize((int)width, (int)height);
}

LevelSelectScreen::~LevelSelectScreen(){
	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	m_backgroundSRV.Reset();
	h_d3d = nullptr;
}

void LevelSelectScreen::initResources(){
	m_loadBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();
	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/levelSelectScreen.png",
		nullptr,
		m_backgroundSRV.ReleaseAndGetAddressOf()));
}

void LevelSelectScreen::dispose(){

}

void LevelSelectScreen::hide(){

}
void LevelSelectScreen::pause(){

}

void LevelSelectScreen::loadLevels(){
	clearWorldsVector();
	using namespace std;
	using namespace std::tr2::sys;
	path worldPath;
	switch (m_mode){
	case CAMPAIGN:
		worldPath = "../img/Campaigns/";
		break;
	case LEVEL:
		worldPath = "../img/Worlds/";
		break;
	}
	int count = 0;
	for (directory_iterator i(worldPath), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".crwf") == 0)
		{
			//is a world
			string name = i->path().filename();
			name = name.substr(0, name.find_last_of("."));
			wchar_t* temp;
			CrusadeUtil::s_to_ws(name, &temp);
			m_worlds.push_back(temp);
		}
		else if (i->path().extension().compare(".ccf") == 0){
			//is a campaign
			string name = i->path().filename();
			name = name.substr(0, name.find_last_of("."));
			wchar_t* temp;
			CrusadeUtil::s_to_ws(name, &temp);
			m_worlds.push_back(temp);
		}
		else{
			//Not a world
		}
	}
}

//msg MUST be a WM_KEYDOWN message
void LevelSelectScreen::handleKeyPress(MSG msg){
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
			m_selected = 0;
			break;
		case VK_ESCAPE:
			h_game->showScreen(Game::MENU);
			m_selected = 0;
			break;
		}
	}
}

void LevelSelectScreen::execute(){
	wstring world(m_worlds[m_selected]);
	h_game->getAbilitySelectScreen()->seticons(h_game->getProfile()->getLevel());
	switch (m_mode){
	case CAMPAIGN:
		world = L"../img/Campaigns/" + world + L".ccf";
		h_game->getAbilitySelectScreen()->setWorld(world, AbilitySelectScreen::CAMPAIGN);
		h_game->showScreen(Game::ABILITY_SELECT);
		//h_game->getGameScreen()->loadCampaign((LPWSTR)world.c_str());
		break;
	case LEVEL:
		world = L"../img/Worlds/" + world + L".crwf";
		h_game->getAbilitySelectScreen()->setWorld(world, AbilitySelectScreen::LEVEL);
		h_game->showScreen(Game::ABILITY_SELECT);
		//h_game->getGameScreen()->loadWorld((LPWSTR)world.c_str());
		break;
	}
	
}

void LevelSelectScreen::update(float dt){

}

void LevelSelectScreen::render(float dt){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;
	m_loadBatch->Begin();
	m_loadBatch->Draw(m_backgroundSRV.Get(), screenRc);
	
	//Strange maximize (button max only, drag max works fine) here
	for (int x = m_offset; x < min((int)m_worlds.size(), m_offset + m_numLines+1); x++){
		Vector2 normalSize = measureTextSize(m_worlds[x], Vector2(1.0f, 1.0f));
		Vector2 textSize = measureTextSize(m_worlds[x], (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));

		m_font->DrawString(m_loadBatch.get(), m_worlds[x], Vector2(10, m_screenHeight*.25f - textSize.y / 2.0f + (normalSize.y * (x - m_offset))), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
	}
	/*int counter = 0;
	for (LPWSTR world : m_worlds){
		Vector2 normalSize = measureTextSize(world, Vector2(1.0f, 1.0f));
		Vector2 textSize = measureTextSize(world, (m_selected == counter) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		
		m_font->DrawString(m_loadBatch.get(), world, Vector2(10, m_screenHeight*.25f - textSize.y / 2.0f + (normalSize.y * counter)), (m_selected == counter) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == counter) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
		counter++;
	}*/
	m_loadBatch->End();
}

void LevelSelectScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
	m_numLines = CrusadeUtil::calcNumLines((int)(height*.7f), Vector2(1.0f,1.0f), m_font.get());
}

void LevelSelectScreen::resume(){

}

void LevelSelectScreen::show(){
	render(0.1f);
}

Vector2 LevelSelectScreen::measureTextSize(LPWSTR text, Vector2 scale){
	Vector2 x = m_font->MeasureString(text);
	return Vector2(x.x*scale.x, x.y*scale.y);
}

void LevelSelectScreen::incrementSelection(){
	m_selected++;
	if (m_selected >= (int)m_worlds.size()){
		m_selected = 0;
		m_offset = 0;
	}
	else{
		m_offset = (m_selected > m_offset + m_numLines) ? m_offset + (m_selected - (m_offset + m_numLines)) : m_offset;
	}
}

void LevelSelectScreen::decrementSelection(){
	m_selected--;
	if (m_selected < 0){
		m_selected = m_worlds.size() - 1;
		m_offset = max(m_selected - m_numLines, 0);
	}
	else{
		m_offset = (m_selected < m_offset) ? m_selected : m_offset;
	}
}

void LevelSelectScreen::setMode(MODE mode){
	m_mode = mode;
	loadLevels();
}

LevelSelectScreen::MODE LevelSelectScreen::getMode(){
	return m_mode;
}

void LevelSelectScreen::clearWorldsVector(){
	std::vector<LPWSTR>::iterator it;
	for (it = m_worlds.begin(); it != m_worlds.end(); it++){
		delete (*it);
	}
	m_worlds.clear();
}