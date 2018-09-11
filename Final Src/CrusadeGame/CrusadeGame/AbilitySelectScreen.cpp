#include "pch.h"
#include "AbilitySelectScreen.h"

AbilitySelectScreen::AbilitySelectScreen(Game* game, float width, float height){
	//m_screenWidth = width;

	h_game = game;
	//m_screenHeight = height;
	m_loadBatch.reset(nullptr);
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	h_d3d = D3DManager::Instance();
	resize((int)width, (int)height);
	m_selected = 0;
	m_offset = 0;
	h_abiManager = AbilityManager::Instance();
	m_loadBatch.reset(new SpriteBatch(h_d3d->getContext()));

	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Backgrounds/abilityselectScreen.png",
		nullptr,
		&m_tempLoad));

	DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
		L"../img/Sprites/Attacks/icons/healthbar.png",
		nullptr,
		&m_selectLoad));
}

AbilitySelectScreen::~AbilitySelectScreen(){
	m_loadBatch.reset(nullptr);
	m_font.reset(nullptr);
	h_d3d = nullptr;
	m_tempLoad.Reset();
	m_selectLoad.Reset();
	m_tempCharacter.reset(nullptr);
	for (auto it = m_icons.begin(); it != m_icons.end(); it++){
		(*it)->Release();
	}
	m_icons.clear();

	m_ids.clear();

}

void AbilitySelectScreen::dispose(){

}

void AbilitySelectScreen::hide(){
	m_tempCharacter.reset(nullptr);
}
void AbilitySelectScreen::pause(){

}

void AbilitySelectScreen::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		if (msg.lParam >> 30 & 0x1){
			Sleep(100);
		}
		switch (msg.wParam){
		case VK_DOWN:
			incrementSelection();
			incrementSelection();
			break;
		case VK_UP:
			decrementSelection();
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
			bindAbilityToKey(msg.wParam);
			break;
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

void AbilitySelectScreen::execute(){
	
	switch (m_mode){
	case CAMPAIGN:
		h_game->getGameScreen()->loadCampaign((LPWSTR)m_world.c_str());
		break;
	case LEVEL:
		h_game->getGameScreen()->loadWorld((LPWSTR)m_world.c_str());
		break;
	case CONTINUE:
		h_game->getGameScreen()->loadCampaign((LPWSTR)m_world.c_str(), m_contLevel);
		break;
	}
}

void AbilitySelectScreen::bindAbilityToKey(WPARAM key){
	switch (key){
	case 'Z':
		h_game->getProfile()->setActiveAbility(0, m_ids[m_selected]);
		break;
	case 'X':
		h_game->getProfile()->setActiveAbility(1, m_ids[m_selected]);
		break;
	case 'C':
		h_game->getProfile()->setActiveAbility(2, m_ids[m_selected]);
		break;
	case 'V':
		h_game->getProfile()->setActiveAbility(3, m_ids[m_selected]);
		break;
	}
}

void AbilitySelectScreen::update(float dt){

}

void AbilitySelectScreen::render(float dt){
	Ability::AbilityDescription tempDes;
	tempDes = h_abiManager->getAbilityDescription(m_ids[m_selected]);

	//ability strings
	string nm = tempDes.name;
	wchar_t* wnm;
	CrusadeUtil::s_to_ws(nm, &wnm);

	string desc = tempDes.description;
	wchar_t* wdesc;
	CrusadeUtil::s_to_ws(desc, &wdesc);

	string mana = "ManaCost: " + to_string(h_abiManager->getManaCost(m_ids[m_selected], m_tempCharacter.get()));
	wchar_t* wmana;
	CrusadeUtil::s_to_ws(mana, &wmana);

	stringstream stream;
	stream << fixed << setprecision(2) << tempDes.cooldown;

	string cdw = "Cooldown: " + stream.str();
	wchar_t* wcdw;
	CrusadeUtil::s_to_ws(cdw, &wcdw);

	//for string positioning
	int txtw = (int)(m_screenWidth / 2 - 10);
	int txth = (int)(m_screenHeight * 0.3f);

	//Rectangles for images
	RECT selectRc;

	RECT descRc;
	descRc.top = (long)(m_screenHeight * 0.4f);
	descRc.bottom = (long)(m_screenHeight * 0.6f);
	descRc.left = (long)(m_screenWidth * 0.5 - 10);
	descRc.right = (long)(m_screenWidth * 0.4f);

	RECT screenRc;
	screenRc.top = 0;
	screenRc.left = 0;
	screenRc.bottom = (long)m_screenHeight;
	screenRc.right = (long)m_screenWidth;

	RECT iconRc;
	iconRc.top = (long)(m_screenHeight * 0.3f);
	iconRc.bottom = (long)(iconRc.top + 40);

	string tmps = "Press Enter to Continue";
	wchar_t* tmpw;
	CrusadeUtil::s_to_ws(tmps, &tmpw);

	//begin drawing
	m_loadBatch->Begin();
	m_loadBatch->Draw(m_tempLoad.Get(), screenRc); //background

	float tmpwHeight = CrusadeUtil::measureTextSize(tmpw, Vector2(.55f, .55f), m_font.get()).y;
	float tmpwWidth = CrusadeUtil::measureTextSize(tmpw, Vector2(.55f, .55f), m_font.get()).x;

	//ability info
	m_font->DrawString(m_loadBatch.get(), tmpw, Vector2(m_screenWidth - (tmpwWidth + 10), m_screenHeight - (tmpwHeight + 10)), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f, .55f));
	m_font->DrawString(m_loadBatch.get(), wnm, Vector2((float)txtw, (float)txth), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f, .55f));
	m_font->DrawString(m_loadBatch.get(), wmana, Vector2((float)txtw, m_screenHeight * 0.7f), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f, .55f));
	m_font->DrawString(m_loadBatch.get(), wcdw, Vector2((float)txtw, m_screenHeight * 0.75f), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f, .55f));
	CrusadeUtil::drawTextBox(m_font.get(), m_loadBatch.get(), wdesc, descRc, Vector2(.55f * (m_screenHeight / 600.0f), .55f * (m_screenHeight / 600.0f)));

	//ability icons
	if (!m_icons.empty()){
		for (int i = 0; i < (int)m_icons.size(); i += 2){
			iconRc.left = (long)(m_screenWidth * 0.07f);
			iconRc.right = (long)(iconRc.left + 40);

			for (int j = 0; j < 2; j++){
				if (m_selected == (i + j)){
					selectRc.top = iconRc.top - 5;
					selectRc.bottom = iconRc.bottom + 5;
					selectRc.left = iconRc.left - 5;
					selectRc.right = iconRc.right + 5;
					m_loadBatch->Draw(m_selectLoad.Get(), selectRc);
				}

				if (m_icons.size() != (i + j))
					m_loadBatch->Draw(m_icons[i + j], iconRc);

				iconRc.left += 60;
				iconRc.right += 60;
			}
			iconRc.top += 60;
			iconRc.bottom += 60;
		}
	}
	//end ability icons

	string tmpz[4];
	tmpz[0] = "Z";
	tmpz[1] = "X";
	tmpz[2] = "C";
	tmpz[3] = "V";
	wchar_t* tmpch;
	

	RECT activeRc;
	activeRc.top = (long)(m_screenHeight - 50);
	activeRc.bottom = (long)(activeRc.top + 40);
	activeRc.left = (long)(m_screenWidth * 0.07f);
	activeRc.right = (long)(activeRc.left + 40);
	
	UINT* tempAcv;
	int key;
	tempAcv = h_game->getProfile()->getActiveAbilities();
	for (int k = 0; k < 4; k++){
		CrusadeUtil::s_to_ws(tmpz[k], &tmpch);
		m_font->DrawString(m_loadBatch.get(), tmpch, Vector2((float)activeRc.left + 5, (float)activeRc.top - 25), Colors::White, 0.0f, Vector2(0.0f, 0.0f), Vector2(.55f, .55f));
		
		key = printActive(tempAcv[k]);
		if (key != -1){
			m_loadBatch->Draw(m_icons[key], activeRc);
			m_hudIcons[k] = m_icons[key];
		}
		else
			m_hudIcons[k] = nullptr;

		activeRc.right += 50;
		activeRc.left += 50;
	}
	m_loadBatch->End();
}

void AbilitySelectScreen::resize(int width, int height){
	m_screenWidth = (float)width;
	m_screenHeight = (float)height;
}

void AbilitySelectScreen::resume(){

}

void AbilitySelectScreen::show(){
	m_selected = 0;
	m_tempCharacter.reset(new Character(Vector2(0.0f, 0.0f), 0.f, h_game->getProfile()));
	render(0.1f);
}

void AbilitySelectScreen::incrementSelection(){
	m_selected++;
	if (m_selected == m_ids.size())
		m_selected = (m_ids.size() - 1);
}

void AbilitySelectScreen::decrementSelection(){
	m_selected--;
	if (m_selected < 0)
		m_selected = 0;
}

void AbilitySelectScreen::setWorld(wstring world, MODE mode){
	m_world = world;
	m_mode = mode;
}

void AbilitySelectScreen::seticons(UINT lv){
	for (auto it = m_icons.begin(); it != m_icons.end();it++){
		(*it)->Release();
	}
	m_icons.clear();

	m_ids.clear();

	using namespace std;
	using namespace std::tr2::sys;


	int i;
	vector<UINT> temp;
	Ability::AbilityDescription tempDes;
	wchar_t* tempStr;
	ID3D11ShaderResourceView* tempSRV;
	USES_CONVERSION;
	path relativePathAdjust("../img");

	temp = h_game->getLevelLookUp()->getAbilities(lv);
	m_ids = temp;
	for (i = 0; i < (int)temp.size(); i++){
		
		tempDes = h_abiManager->getAbilityDescription(temp[i]);
		
		path icon_path(tempDes.iconPath);
		icon_path = relativePathAdjust / icon_path;
		tempStr = A2W(icon_path.file_string().c_str());
		
		DX::ThrowIfFailed(CreateWICTextureFromFile(h_d3d->getDevice(),
			tempStr,
			nullptr,
			&tempSRV));
		m_icons.push_back(tempSRV);
	}

}

int AbilitySelectScreen::printActive(UINT id){
	for (int i = 0; i < (int)m_ids.size(); i++){
		if (id == m_ids[i])
			return i;
	}
	return -1;
}

ID3D11ShaderResourceView** AbilitySelectScreen::getHudIcons(){
	return m_hudIcons;
}

void AbilitySelectScreen::continueCampaign(wstring campaign, UINT level){
	m_mode = CONTINUE;
	m_world = campaign;
	m_contLevel = level;
}