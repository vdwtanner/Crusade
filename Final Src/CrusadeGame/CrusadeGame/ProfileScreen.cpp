#include "pch.h"
#include "ProfileScreen.h"

ProfileScreen::ProfileScreen(Game* game, float width, float height){
	//m_screenWidth = width;

	h_game = game;
	//m_screenHeight = height;

	m_selected = 0;
	m_offset = 0;
	//m_numLines = 1;
	m_mode = SELECT;
	loadProfiles();
	initResources();
	resize((int)width, (int)height);
}

ProfileScreen::~ProfileScreen(){
	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	m_backgroundSRV.Reset();
	h_d3d = nullptr;
}

void ProfileScreen::initResources(){
	m_loadBatch.reset(new SpriteBatch(D3DManager::Instance()->getContext()));
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();

	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/profileScreen.png",
		nullptr,
		m_backgroundSRV.ReleaseAndGetAddressOf()));
}

void ProfileScreen::dispose(){

}

void ProfileScreen::hide(){

}
void ProfileScreen::pause(){

}

void ProfileScreen::loadProfiles(){
	//clearProfilesVector();
	//clearLevelsVector();
	m_profiles.clear();
	m_levels.clear();
	using namespace std;
	using namespace std::tr2::sys;
	path worldPath = "../img/Profiles/";
	int count = 0;
	for (directory_iterator i(worldPath), end; i != end; i++){
		path extension = i->path().extension();
		if (i->path().extension().compare(".crusader") == 0)
		{
			//is a profile
			string name = i->path().filename();
			name = name.substr(0, name.find_last_of("."));
			int level = getCrusaderLevel(name);
			string levelText = " - " + to_string(level);
			wchar_t* temp;
			CrusadeUtil::s_to_ws(name, &temp);
			m_profiles.push_back(temp);
			CrusadeUtil::s_to_ws(levelText, &temp);
			m_levels.push_back(temp);
		}
		else{
			//Not a profile
		}
	}
}

//msg MUST be a WM_KEYDOWN message
void ProfileScreen::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		if (msg.lParam >> 30 & 0x1){
			if (m_mode != NEW){
				Sleep(100);
			}
		}
		switch (m_mode){
		//SELECT STATE
		case SELECT:
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
			break;
		//NEW STATE
		case NEW:
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
			case VK_ESCAPE:
				m_selected = 0;
				m_mode = SELECT;
				break;
			case VK_BACK:
				if (m_selected > 0){
					m_selected = 0;
					m_mode = SELECT;
				}
				break;
			case VK_RETURN:
				if (m_selected == 0){
					m_selected = 1;
				}
				execute();
				break;
			default:
				if (m_selected > 0){
					switch (msg.wParam){
					case 'Z':
					case 'X':
					case 'C':
					case 'V':
					case VK_SPACE:
					case VK_RETURN:
						execute();
						break;
					}
				}
			}
			break;
		}
	}
}

void ProfileScreen::handleWM_CHAR(MSG msg){
	if (WM_CHAR == msg.message && m_mode == NEW){
		switch (msg.wParam){
		case VK_DOWN:
			break;
		case VK_UP:
			break;
		case VK_RIGHT:
			break;
		case VK_LEFT:
			break;
		case VK_ESCAPE:
			m_selected = 0;
			m_mode = SELECT;
			break;
		case VK_BACK:
			m_newName = m_newName.substr(0, m_newName.length() - 1);
			break;
		case 0x09:
		case 0x0A:
		case VK_RETURN:
			break;

		default:
			if (m_selected == 0){
				if (m_newName.length() < 12){
					wchar_t ch = msg.wParam;
					m_newName += ch;
				}
				else{
					MessageBeep((UINT)-1);
				}
				
			}
			break;
		}
	}
}

void ProfileScreen::execute(){
	wstring profile;
	switch (m_mode){
	case SELECT:
		if (m_selected < (int)m_profiles.size()){
			h_game->setProfile(new Profile((string)CW2A(m_profiles[m_selected]), h_game->getLevelLookUp()));
			h_game->showScreen(Game::MENU);
		}
		else if (m_selected = m_profiles.size()){
			m_selected = 0;
			m_mode = NEW;
		}
		
		break;
	case VIEW:
		//h_game->getGameScreen()->loadprofile((LPWSTR)profile.c_str());
		break;
	case NEW:
		if (m_selected == 1){
			Profile* p = new Profile((string)CW2A((LPWSTR)m_newName.c_str()), nullptr);
			p->save();
			delete(p);
			m_selected = 0;
			loadProfiles();
			m_mode = SELECT;
		}
		else if (m_selected == 2){
			m_selected = 0;
			m_mode = SELECT;
		}
		break;
	}

}

void ProfileScreen::update(float dt){

}

void ProfileScreen::render(float dt){
	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;

	m_loadBatch->Begin();
	m_loadBatch->Draw(m_backgroundSRV.Get(), screenRc);

	//Strange maximize (button max only, drag max works fine) here
	switch (m_mode){
	case SELECT:
		for (int x = m_offset; x < min((int)m_profiles.size() + 1, m_offset + m_numLines + 2); x++){
			//Profiles
			if (x < (int)m_profiles.size()){
				Vector2 normalSize = CrusadeUtil::measureTextSize(m_profiles[x], Vector2(1.0f, 1.0f), m_font.get());
				Vector2 textSize = CrusadeUtil::measureTextSize(m_profiles[x], (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f), m_font.get());
				//Draw Profile Name
				m_font->DrawString(m_loadBatch.get(), m_profiles[x], Vector2(10, m_screenHeight*.25f - textSize.y / 2.0f + (normalSize.y * (x - m_offset))), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
				//Draw Level Text
				m_font->DrawString(m_loadBatch.get(), m_levels[x], Vector2(textSize.x, m_screenHeight*.25f - textSize.y / 2.0f + (normalSize.y * (x - m_offset))), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
			}
			if (x >= (int)m_profiles.size()){
				//Draw NEW button
				Vector2 normalSize = CrusadeUtil::measureTextSize(L"NEW", Vector2(1.0f, 1.0f), m_font.get());
				Vector2 textSize = CrusadeUtil::measureTextSize(L"NEW", (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f), m_font.get());
				m_font->DrawString(m_loadBatch.get(), L"NEW", Vector2(10, m_screenHeight*.25f - textSize.y / 2.0f + (normalSize.y * (x - m_offset))), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
			}
		}
		break;
	case NEW:
		wstring newNameText = L"NAME: " + m_newName + L"_";
		Vector2 normalSize;
		Vector2 textSize;
		for (int x = m_offset; x < 3; x++){
			switch (x){
			case 0:
				normalSize = CrusadeUtil::measureTextSize((LPWSTR)newNameText.c_str(), Vector2(1.0f, 1.0f), m_font.get());
				textSize = CrusadeUtil::measureTextSize((LPWSTR)newNameText.c_str(), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f), m_font.get());
				m_font->DrawString(m_loadBatch.get(), (LPWSTR)newNameText.c_str(), Vector2(10, m_screenHeight*.5f - textSize.y / 2.0f - normalSize.y), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
				break;
			case 1:
				normalSize = CrusadeUtil::measureTextSize(L"FINISH", Vector2(1.0f, 1.0f), m_font.get());
				textSize = CrusadeUtil::measureTextSize(L"FINISH", (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f), m_font.get());
				m_font->DrawString(m_loadBatch.get(), L"FINISH", Vector2(10, m_screenHeight*.5f - textSize.y / 2.0f), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
				break;
			case 2:
				normalSize = CrusadeUtil::measureTextSize(L"QUIT", Vector2(1.0f, 1.0f), m_font.get());
				textSize = CrusadeUtil::measureTextSize(L"QUIT", (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f), m_font.get());
				m_font->DrawString(m_loadBatch.get(), L"QUIT", Vector2(10, m_screenHeight*.5f - textSize.y / 2.0f + normalSize.y), (m_selected == x) ? Colors::Gold : Colors::White, 0.0f, Vector2(0.0f, 0.0f), (m_selected == x) ? Vector2(1.2f, 1.2f) : Vector2(1.0f, 1.0f));
				break;
			}
		}
		break;
	}
	
	m_loadBatch->End();
}

void ProfileScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
	m_numLines = CrusadeUtil::calcNumLines((int)(height*.7f), Vector2(1.0f, 1.0f), m_font.get());
}

void ProfileScreen::resume(){

}

void ProfileScreen::show(){
	render(0.1f);
}

void ProfileScreen::incrementSelection(){
	m_selected++;
	switch (m_mode){
	case SELECT:
		if (m_selected >= (int)m_profiles.size() + 1){
			m_selected = 0;
			m_offset = 0;
		}
		else{
			m_offset = (m_selected > m_offset + m_numLines) ? m_offset + (m_selected - (m_offset + m_numLines)) : m_offset;
		}
		break;
	case NEW:
		if (m_selected >= 3){
			m_selected = 0;
			m_offset = 0;
		}
		else{
			m_offset = (m_selected > m_offset + m_numLines) ? m_offset + (m_selected - (m_offset + m_numLines)) : m_offset;
		}
		break;
	case VIEW:
		break;
	}
}

void ProfileScreen::decrementSelection(){
	m_selected--;
	switch (m_mode){
	case SELECT:
		if (m_selected < 0){
			m_selected = m_profiles.size();
			m_offset = max(m_selected - m_numLines, 0);
		}
		else{
			m_offset = (m_selected < m_offset) ? m_selected : m_offset;
		}
		break;
	case NEW:
		if (m_selected < 0){
			m_selected = 3;
			m_offset = max(m_selected - m_numLines, 0);
		}
		else{
			m_offset = (m_selected < m_offset) ? m_selected : m_offset;
		}
		break;
	case VIEW:
		break;
	}
	
}

void ProfileScreen::setMode(MODE mode){
	m_mode = mode;
	loadProfiles();
}

ProfileScreen::MODE ProfileScreen::getMode(){
	return m_mode;
}

void ProfileScreen::clearProfilesVector(){
	std::vector<LPWSTR>::iterator it;
	for (it = m_profiles.begin(); it != m_profiles.end(); it++){
		delete (*it);
	}
	m_profiles.clear();
}

void ProfileScreen::clearLevelsVector(){
	std::vector<LPWSTR>::iterator it;
	for (it = m_levels.begin(); it != m_levels.end(); it++){
		delete (*it);
	}
	m_levels.clear();
}

/*Get level information from file. crusaderProfile should be <name> (no file extension)
 Returns -1 if profile not found*/
int ProfileScreen::getCrusaderLevel(string crusaderProfile){
	string temp = "../img/Profiles/" + crusaderProfile + ".crusader";
	wchar_t* path;
	CrusadeUtil::s_to_ws(temp, &path);
	LPWSTR filepath = (LPWSTR)path;
	ifstream file(filepath, ios::in | ios::binary);
	if (!file){
		return -1;
	}
	int exp;
	int level;
	file.seekg(0, std::ios::beg);
	//Read exp
	file.read((char*)&exp, 4);
	level = h_game->getLevelLookUp()->getLevel(exp);
	return level;
}