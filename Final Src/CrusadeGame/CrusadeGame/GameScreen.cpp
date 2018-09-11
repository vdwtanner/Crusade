#include "pch.h"
#include "GameScreen.h"

GameScreen::GameScreen(Game* game){
	h_game = game;
	m_screenRc = RECT{0,0,0,0};
	m_timer = 0;
	m_world.reset(nullptr);
	m_character.reset(nullptr);
	m_hud.reset(nullptr);
	//m_campaign = new Campaign(L"../img/Campaigns/campaign.ccf");
	m_campaign.reset(nullptr);
	h_d3d = D3DManager::Instance();
	m_font.reset(new SpriteFont(D3DManager::Instance()->getDevice(), L"../img/courier.spritefont"));
	m_spriteBatch.reset(new SpriteBatch(h_d3d->getContext()));
	m_gameOverTimer = 0.0f;
	m_loadNextWorld = false;
	m_currentWorldFile = L"";

	m_movementEnabled = true;
	m_abilitiesEnabled = true;
	// Create an Orthographic Camera
	//                  (ortho?) (viewport dimensions)
	m_camera = new Camera(true, Vector2(30, 20));

	DX::ThrowIfFailed(CreateWICTextureFromFile(D3DManager::Instance()->getDevice(),
		L"../img/Hud/box.png",
		nullptr,
		m_pauseBox.ReleaseAndGetAddressOf()));
}

GameScreen::~GameScreen(){
	if (m_bgmusic)
		AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
	m_bgmusic = nullptr;

	m_character.reset(nullptr);
	m_font.reset(nullptr);
	m_spriteBatch.reset(nullptr);
	m_world.reset(nullptr);
	m_campaign.reset(nullptr);
}

//Load the next world in the campaign. Returns true if successful, false if there are no more levels to load
bool GameScreen::loadNextWorld(){
	m_loadNextWorld = true;
	return false;
}

//Delete old world contents and load a new world
void GameScreen::loadWorld(LPWSTR filepath){
	m_currentWorldFile = filepath;
	m_gameOverTimer = 0.0f;
	string loading = string(CW2A(filepath));
	loading = loading.substr((int)loading.find_last_of("/", loading.length() - 5) + 1);
	loading = loading.substr(0, loading.length() - 5);
	h_game->getLoadingScreen()->setText(loading);
	h_game->showScreen(Game::LOADING);
	h_game->Render();
	TriggerManager::Instance()->deleteAllTriggers();
	AbilityManager::Instance()->deleteAllAbilities();
	EnemyManager::Instance()->deleteAllEnemies();
	ParticleManager::Instance()->Clear();
	Box2DSingleton::Instance()->clearWorld();
	m_levelTimer = 0.0f;
	
	float width = h_d3d->getBackBufferWidth();
	float height = h_d3d->getBackBufferHeight();
	m_screenRc.top = 0;
	m_screenRc.left = 0;
	m_screenRc.bottom = (long)height;
	m_screenRc.right = (long)width;
	m_camera->Resize(width, height);
	m_camera->SetViewportOrtho(30, 30 * (height / width));
	// Z is our depth
	m_camera->Init(Vector3(0, 0, 5.0f), Vector3(0, 0, 0));
	
	m_hud.reset(nullptr);
	m_character.reset(nullptr);
	m_world.reset(nullptr);


	Box2DSingleton::Instance()->initWorld();
	m_world.reset(new WorldTileMatrix(h_d3d->getDevice(), h_d3d->getContext(), filepath, h_game->getTileSet()));
	m_world->setLayerViewState(WorldTileMatrix::LayerViewState::VIEW_ALL);
	m_world->setBackgroundRect(m_screenRc);

	Box2DSingleton::Instance()->setWorldSize(Vector2(width, height));
	//Character must load after AbilityManager
	m_character.reset(new Character(Vector2(m_world->getPlayerSpawn().x, m_world->getPlayerSpawn().y), 0.0f, h_game->getProfile()));
	TriggerManager::Instance()->getLuaBindings()->setCharacter(m_character.get());

	m_hud.reset(new HUD(width, height, m_character.get()));
	m_hud->sethudIcons(h_game->getAbilitySelectScreen()->getHudIcons());
	m_camera->EnableFollowPlayer();
	m_camera->SetPos(Vector3(m_character->getPosition().x, m_character->getPosition().y, 5.0f));
	m_camera->LookAt(Vector3(m_character->getPosition().x, m_character->getPosition().y, 0.0f));
	m_camera->setWorld((float)m_world->getHeight(), (float)m_world->getWidth());
	m_camera->rePosition(m_character->getTrackingPosition(), 0);
	m_camera->Update();
	h_game->showScreen(Game::GAME);
	m_abilitiesEnabled = true;
	m_movementEnabled = true;
	Box2DSingleton::Instance()->resume();
	AbilityManager::Instance()->resume();
	EnemyManager::Instance()->resume();

	/*ParticleManager::Instance()->CreateEmitter(Vector2(5.0f, 80.0f), Vector2(0.0f, 4.0f), PT_BLOOD, 0.01f);
	ParticleManager::Instance()->CreateEmitter(Vector2(5.0f, 80.0f), Vector2(1.0f, 3.0f), PT_BLOOD, 0.02f);
	ParticleManager::Instance()->CreateEmitter(Vector2(5.0f, 80.0f), Vector2(-1.0f, 2.0f), PT_BLOOD, 0.03f);
	ParticleManager::Instance()->CreateEmitter(Vector2(5.0f, 80.0f), Vector2(0.0f, 1.0f), PT_BLOOD, 0.04f);
	ParticleManager::Instance()->CreateEmitter(Vector2(5.0f, 80.0f), Vector2(0.0f, 5.0f), PT_BLOOD, 0.05f);*/
	m_paused = false;
}

//Load a campaign from the starting point.
bool GameScreen::loadCampaign(LPWSTR filepath){
	m_campaign.reset(new Campaign(filepath));
	if (m_campaign->getFilePaths()->size() < 1){
		return false;
	}
	loadWorld(m_campaign->getFilePath(m_campaign->getCurrentLevel()));
	return true;
}

//Load campaign from a specified level. This is the continue function
bool GameScreen::loadCampaign(LPWSTR filepath, UINT level){
	m_campaign.reset(new Campaign(filepath));
	if (m_campaign->getFilePaths()->size() < 1){
		return false;
	}
	m_campaign->setCurrentLevel(level);
	loadWorld(m_campaign->getFilePath(level));
	return true;
}

Game* GameScreen::getGame(){
	return h_game;
}

Camera* GameScreen::getCamera(){
	return m_camera;
}

WorldTileMatrix* GameScreen::getWorldTileMatrix(){
	return m_world.get();
}

Character* GameScreen::getCharacter(){
	return m_character.get();
}

HUD* GameScreen::getHUD(){
	return m_hud.get();
}

void GameScreen::dispose(){

}

void GameScreen::hide(){
	m_bgmusic->Stop();
	AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
	m_bgmusic = nullptr;
}

void GameScreen::pause(){
	m_paused = true;
}

void GameScreen::update(float dt){
	if (!m_paused){
		if (m_character->isDead()){
			m_gameOverTimer += dt;
		}
		if (m_loadNextWorld){
			UINT lvl;
			
			h_game->getEndLevelScreen()->setnewEXP(m_character->getExp());
			h_game->getEndLevelScreen()->setcurrEXP(m_character->getProfile()->getExp());
			m_character->applyExp();
			lvl = h_game->getProfile()->getLevel();
			h_game->getEndLevelScreen()->setlevel(lvl);
			h_game->getEndLevelScreen()->setnxtLevelEXP(h_game->getLevelLookUp()->getExpForNextLevel(lvl));
			if (m_campaign.get()){
				UINT level = m_campaign->getCurrentLevel();
				if (level + 1 < m_campaign->getFilePaths()->size()){
					m_character->getProfile()->setCurrentCampaign(m_campaign->getFilename());
					m_character->getProfile()->setCurrentCampaignLevel(level + 1);
				}
				else{
					m_character->getProfile()->setCurrentCampaign(L"");
					m_character->getProfile()->setCurrentCampaignLevel(0);
				}

			}
			m_character->getProfile()->save();
			h_game->getEndLevelScreen()->setgameTime(m_levelTimer);
			if (m_campaign.get()){
				LPWSTR filepath = m_campaign->getNextLevel();
				if (wcscmp(filepath, L"CAMPAIGN FINISHED")){//true if not the same (compare != 0)
					h_game->getAbilitySelectScreen()->setWorld(filepath, AbilitySelectScreen::LEVEL);
					h_game->getAbilitySelectScreen()->seticons(lvl);
					h_game->getEndLevelScreen()->setnxtlvl(true);
					h_game->showScreen(Game::END_LEVEL);
				}
				else{
					h_game->getEndLevelScreen()->setnxtlvl(false);
					h_game->getLoadingScreen()->setText("You won! Congratulations!\n");
					h_game->showScreen(Game::LOADING);
					h_game->Render();
					h_game->quitGame(true);
					return;
				}
			}
			else{
				h_game->getEndLevelScreen()->setnxtlvl(false);
				h_game->getLoadingScreen()->setText("You won! Congratulations!\n");
				h_game->showScreen(Game::LOADING);
				h_game->Render();
				h_game->quitGame(true);
				return;
			}

			m_loadNextWorld = false;
			return;
		}

		if (m_movementEnabled){
			m_levelTimer += dt;
		}


		if (h_game->isWindActive()){
			SetFocus(h_game->getWindow());
			// TODO: Add your game logic here
			m_character->halt();
			
			if (m_movementEnabled){
				if (GetAsyncKeyState(VK_DOWN) & 0x8000){
					if (m_character->canFly()){
						m_character->moveDown(dt);
					}
				}
				if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
					m_character->setFacingRight(true);
					m_character->walk(dt);
				}
				if (GetAsyncKeyState(VK_LEFT) & 0x8000){
					m_character->setFacingRight(false);
					m_character->walk(dt);
				}
				if (GetAsyncKeyState(VK_UP) & 0x8000){
					if (m_character->canFly()){
						m_character->moveUp(dt);
					}
					else{
						m_character->jump();
					}
				}
			}
			if (m_abilitiesEnabled){
				if (GetAsyncKeyState('Z') & 0x8000){
					if (m_character->getCooldownTimer(0) <= 0){
						Ability* ptr = m_character->castAbility(m_character->getActiveAbility(0));
						if (ptr){
							m_character->startCooldownTimer(0);
						}
					}
				}
				if (GetAsyncKeyState('X') & 0x8000){
					if (m_character->getCooldownTimer(1) <= 0){
						Ability* ptr = m_character->castAbility(m_character->getActiveAbility(1));
						if (ptr){
							m_character->startCooldownTimer(1);
						}
					}
				}
				if (GetAsyncKeyState('C') & 0x8000){
					if (m_character->getCooldownTimer(2) <= 0){
						Ability* ptr = m_character->castAbility(m_character->getActiveAbility(2));
						if (ptr){
							m_character->startCooldownTimer(2);
						}
					}
				}
				if (GetAsyncKeyState('V') & 0x8000){
					if (m_character->getCooldownTimer(3) <= 0){
						Ability* ptr = m_character->castAbility(m_character->getActiveAbility(3));
						if (ptr){
							m_character->startCooldownTimer(3);
						}
					}
				}
			}
			if (GetAsyncKeyState('1') & 0x8000){
				//Change Z passive
				m_character->setPassive(0, (m_character->getPassive(0) + 1) % 3);
			}
			if (GetAsyncKeyState('2') & 0x8000){
				//Change X passive
				m_character->setPassive(1, (m_character->getPassive(1) + 1) % 3);
			}
			if (GetAsyncKeyState('3') & 0x8000){
				//Change C passive
				m_character->setPassive(2, (m_character->getPassive(2) + 1) % 3);
			}
			if (GetAsyncKeyState('4') & 0x8000){
				//Change V passive
				m_character->setPassive(3, (m_character->getPassive(3) + 1) % 3);
			}
			if (GetAsyncKeyState('K') & 0x8000){
				m_camera->EnableFollowPlayer();
			}
			if (GetAsyncKeyState('J') & 0x8000){
				m_camera->DisableFollowPlayer();
			}
			if (GetAsyncKeyState('L') & 0x8000){
				loadNextWorld();
			}
		}
		m_world->update(dt);
		m_timer += dt;
		float timestep = Box2DSingleton::Instance()->getTimeStep();
		if (m_timer > .1f)
			m_timer = .1f;
		while (m_timer >= timestep){
			Box2DSingleton::Instance()->stepWorld(timestep);
			m_timer -= timestep;

			EnemyManager::Instance()->stepEnemies(timestep);
			AbilityManager::Instance()->stepAbilities(timestep);
			TriggerManager::Instance()->stepTriggers(timestep);
			ParticleManager::Instance()->Update(timestep);
			m_character->step(timestep);
		}
		float alpha = m_timer / timestep;
		EnemyManager::Instance()->interpolate(alpha);
		AbilityManager::Instance()->interpolate(alpha);
		TriggerManager::Instance()->interpolate(alpha);
		m_character->interpolate(alpha);

		int *myint = new int[5];
		delete[] myint;

		m_camera->rePosition(m_character->getTrackingPosition(), dt);
		m_camera->Update();
	}
}

//Render the game screen and all its contents
void GameScreen::render(float dt){
	UINT active = m_world->getActiveLayer();
	UINT numLayers = m_world->getLayers();

	m_world->RenderBackground();

	// render all layers from the backmost layer up to and including the active layer
	m_world->RenderLayers(m_camera->getViewProj(), numLayers - 1, (numLayers - active));

	// draw the enemies on the active layer
	EnemyManager::Instance()->renderEnemies();

	// draw the character on the active layer
	if (!m_character->isDead()){
		m_character->renderCharacter();
	}
	else{
		m_camera->DisableFollowPlayer();
	}
	

	// draw the abilities on the active layer
	AbilityManager::Instance()->renderAbilities();

	// draw debug triggers
	TriggerManager::Instance()->debugRender(m_camera);

	// draw the remaining layers
	m_world->RenderLayers(m_camera->getViewProj(), active - 1, active);

	// draw the particles
	ParticleManager::Instance()->Draw(m_camera->getViewProj());

	//Cooldowns
	/*string temp = "Z: " + to_string(m_character->getCooldownTimer(0)) + "  X: " + to_string(m_character->getCooldownTimer(1)) + "  C: " + to_string(m_character->getCooldownTimer(2)) + "  V: " + to_string(m_character->getCooldownTimer(3));
	wchar_t* wcool;
	CrusadeUtil::s_to_ws(temp, &wcool);*/
	m_hud->Draw();
	stringstream stream;
	stream << fixed << setprecision(2) << m_levelTimer;
	string time = stream.str();
	wchar_t* wtime;
	CrusadeUtil::s_to_ws(time, &wtime);
	m_spriteBatch->Begin();
	m_font->DrawString(m_spriteBatch.get(), wtime, Vector2(m_screenRc.right/2.0f - CrusadeUtil::measureTextSize(wtime, Vector2(.8f,.8f), m_font.get()).x/2.0f, .8f), Colors::Wheat, 0.0f, Vector2(0.0f, 0.0f), Vector2(.8f, .8f));


	//m_hud->displayTextBox(L"PAUSED", L"Press ESC to close\nPress P to resume.");
	if (m_paused && !m_character->isDead()){
		RECT pauseScreen;
		pauseScreen.left = (long)(m_screenRc.right * .25f);
		pauseScreen.right = (long)(m_screenRc.right * .75f);
		pauseScreen.top = (long)(m_screenRc.bottom *.25f);
		pauseScreen.bottom = (long)(m_screenRc.bottom * .75f);
		m_spriteBatch->Draw(m_pauseBox.Get(), pauseScreen, Colors::White);
		wstring pauseText = L"PAUSED\nPress Q to quit.\nPress ESCAPE or P to resume.\n\nEXP this level: " + to_wstring(m_character->getExp()) + L"\nEXP to level up: " + to_wstring(m_character->getProfile()->getExpToNextLevel());
		Vector2 textScale = Vector2(.7f * m_screenRc.right / 800, .7f* m_screenRc.bottom / 800);
		Vector2 size = CrusadeUtil::measureTextSize((LPWSTR)pauseText.c_str(), textScale, m_font.get());
		Vector2 location = Vector2(m_screenRc.right / 2 - size.x / 2, m_screenRc.bottom / 2 - size.y / 2);
		pauseScreen.top += 10 *m_screenRc.bottom / 800;
		pauseScreen.bottom -= pauseScreen.top;
		pauseScreen.right -= pauseScreen.left;
		CrusadeUtil::drawTextBox(m_font.get(), m_spriteBatch.get(), wstring(pauseText), pauseScreen, textScale, Colors::White);
		//m_font->DrawString(m_spriteBatch.get(), (const wchar_t*)pauseText, location, Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);
	}

	if (m_gameOverTimer >= 1.5f){
		RECT pauseScreen;
		pauseScreen.left = (long)(m_screenRc.right * .35f);
		pauseScreen.right = (long)(m_screenRc.right * .65f);
		pauseScreen.top = (long)(m_screenRc.bottom *.35f);
		pauseScreen.bottom = (long)(m_screenRc.bottom * .65f);
		m_spriteBatch->Draw(m_pauseBox.Get(), pauseScreen, Colors::White);
		Vector2 textScale = Vector2(.9f * m_screenRc.right / 800, .9f* m_screenRc.bottom / 800);
		Vector2 size = CrusadeUtil::measureTextSize(L"YOU DIED", textScale, m_font.get());
		Vector2 location = Vector2((float)m_screenRc.right / 2.0f - size.x / 2.0f, (float)pauseScreen.top);
		m_font->DrawString(m_spriteBatch.get(), L"YOU DIED", location, Colors::Crimson, 0.0f, Vector2(0.0f, 0.0f), textScale);
		pauseScreen.top += (long)size.y;
		wstring pauseText = L"Press Q to quit\nPress ENTER to retry";
		textScale = Vector2(.7f * m_screenRc.right / 800, .7f* m_screenRc.bottom / 800);
		size = CrusadeUtil::measureTextSize((LPWSTR)pauseText.c_str(), textScale, m_font.get());
		location = Vector2(m_screenRc.right / 2 - size.x / 2, m_screenRc.bottom / 2 - size.y / 2);
		pauseScreen.top += 10 *m_screenRc.bottom / 800;
		pauseScreen.bottom -= pauseScreen.top;
		pauseScreen.right -= pauseScreen.left;
		CrusadeUtil::drawTextBox(m_font.get(), m_spriteBatch.get(), wstring(pauseText), pauseScreen, textScale, Colors::Crimson);
		EnemyManager::Instance()->pause();
		//m_font->DrawString(m_spriteBatch.get(), (const wchar_t*)pauseText, location, Colors::White, 0.0f, Vector2(0.0f, 0.0f), textScale);
	}

	m_spriteBatch->End();
}

void GameScreen::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		switch (msg.wParam){
		case VK_RETURN:
			if (m_hud->textBoxDisplayed())
				if(!m_hud->scrollTextBox())
					TriggerManager::Instance()->anyKeyPressed();
			if (m_gameOverTimer >= 1.5f){
				loadWorld(m_currentWorldFile);
			}
			break;
		case 'T':
			TriggerManager::Instance()->setDebugMode(!TriggerManager::Instance()->isDebugMode());
			break;
		case 'G':
			m_character->toggleGodMode();
			break;
		case VK_ESCAPE:
			if (m_paused){
				resume();
			}
			else{
				pause();
			}
			break;
		case 'Q':
			if (m_paused || m_gameOverTimer >= 1.5f){
				h_game->getLoadingScreen()->setText("Closing and returning to menu");
				h_game->showScreen(Game::LOADING);
				h_game->Render();
				h_game->quitGame(false);
			}
			break;
		case 'P':
			if (m_paused){
				resume();
			}
			else{
				pause();
			}
			break;
		}
	}
}


void GameScreen::resize(int width, int height){
	m_camera->Resize((float)width, (float)height);
	m_camera->SetViewportOrtho(30, 30 * ((float)height / (float)width));
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = height;
	rc.right = width;
	m_world->setBackgroundRect(rc);
	m_screenRc = rc;
	m_hud->reSize((float)width, (float)height);
}

void GameScreen::resume(){
	m_paused = false;
}

void GameScreen::show(){
	if (m_world->getMusicPath().length() < 5){//Music path too short to be valid
		m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Windy_Old_Weather.adpcm");
	}
	else{//music path should work
		string file = string(CW2A((LPWSTR)m_world->getMusicPath().c_str()));
		m_bgmusic = AudioManager::Instance()->CreateSoundInstance(file);
	}
	m_bgmusic->SetVolume(.5f);
	m_bgmusic->Play(true);
}

void GameScreen::enableMovementInput(){
	m_movementEnabled = true;
}

void GameScreen::disableMovementInput(){
	m_movementEnabled = false;
}

void GameScreen::enableAbilityInput(){
	m_abilitiesEnabled = true;
}

void GameScreen::disableAbilityInput(){
	m_abilitiesEnabled = false;
}

float GameScreen::getLevelTimer(){
	return m_levelTimer;
}

void GameScreen::PauseBGM(){
	m_bgmusic->Pause();
}

void GameScreen::ResumeBGM(){
	m_bgmusic->Resume();
}

void GameScreen::SwitchBGM(std::string song){
	m_bgmusic->Stop();
	AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
	m_bgmusic = AudioManager::Instance()->CreateSoundInstance(song);
	m_bgmusic->Play(true);
}

void GameScreen::SetBGMVolume(float volume){
	m_bgmusic->SetVolume(volume);
}