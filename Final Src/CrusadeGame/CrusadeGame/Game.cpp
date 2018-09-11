//
// Game.cpp -
//

#include "pch.h"
#include "Game.h"
#include "LuaBindings.h"


using namespace DirectX;

using Microsoft::WRL::ComPtr;

// Constructor.
Game::Game() :
m_window(0), m_wndActive(true)
{
	m_exit = false;
}

Game::~Game(){
	if (m_bgmusic)
		AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);


	//m_character.reset();
	m_levelLookUp.reset(nullptr);

	m_tileset.reset(nullptr);
	m_world.reset(nullptr);
	m_loadBatch.reset(nullptr);
	m_bindings.reset(nullptr);
	m_menuScreen.reset(nullptr);
	m_levelSelectScreen.reset(nullptr);
	m_gameScreen.reset(nullptr);
	m_loadingScreen.reset(nullptr);
	m_abilitySelectScreen.reset(nullptr);
	m_profileScreen.reset(nullptr);
	
	m_profile.reset(nullptr);
	m_endLevelScreen.reset(nullptr);

	delete AbilityManager::Instance();
	delete EnemyManager::Instance();
	delete ParticleManager::Instance();
	delete AudioManager::Instance();
	delete D3DManager::Instance();
}

void Game::handleKeyPress(MSG msg){
	if (WM_KEYDOWN == msg.message){
		switch (m_currentScreen){
		case MENU:
			m_menuScreen->handleKeyPress(msg);
			break;
		case LEVEL_SELECT:
			m_levelSelectScreen->handleKeyPress(msg);
			break;
		case PROFILE:
			m_profileScreen->handleKeyPress(msg);
			break;
		case ABILITY_SELECT:
			m_abilitySelectScreen->handleKeyPress(msg);
			break;
		case END_LEVEL:
			m_endLevelScreen->handleKeyPress(msg);
			break;
		case GAME:
			m_gameScreen->handleKeyPress(msg);
			break;
		}
	}
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window)
{
	m_allowInput = true;
	m_currentScreen = MENU;
    m_window = window;
	h_d3d = D3DManager::Instance();
	h_d3d->Initialize(window);

	float width = h_d3d->getBackBufferWidth();
	float height = h_d3d->getBackBufferHeight();

	m_levelLookUp.reset(new LevelLookUp());//can use a parameter here to load a specific levelling table if we want
	m_gameScreen.reset(new GameScreen(this));
	m_menuScreen.reset(new MenuScreen(this, width, height));
	m_levelSelectScreen.reset(new LevelSelectScreen(this, width, height));
	m_loadingScreen.reset(new LoadingScreen("", width, height));
	m_profileScreen.reset(new ProfileScreen(this, width, height));
	m_profile.reset(new Profile("Martin", m_levelLookUp.get()));
	
	m_endLevelScreen.reset(new EndLevelScreen(this, width, height));

	AudioManager::Initialize(L"../img/Sounds/");
	AudioManager::Instance()->setGameScreen(m_gameScreen.get());

	//AbilityManager must load first so that everything can get information from it
	AbilityManager::Instance(L"../img/Abilities.xml");
	m_abilitySelectScreen.reset(new AbilitySelectScreen(this, width, height));
	m_bindings.reset(new LuaBindings(nullptr));
	m_bindings->BindAllFunctions();
	TriggerManager::Instance(L"../img/Scripts/", m_bindings.get());
	TriggerManager::Instance()->setDebugMode(false);

	EnemyManager::Instance(L"../img/Enemy.xml");
	EnemyManager::Instance()->setWorldEditMode(false);
	m_tileset.reset(new TileSet(h_d3d->getDevice(), h_d3d->getContext()));
	m_tileset->LoadTileSet(L"../img/Tiles.xml");
	
	ParticleManager::Initialize();
	ParticleManager::Instance()->setTileSetTextureArray(m_tileset->getTextureArr());

	Clear();

	//Show Loading screen
	m_menuScreen->show();
	
	Present();
	m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Stratosphere.adpcm", SoundEffectInstance_Default);
	m_bgmusic->Play(true);
	m_bindings->setCharacter(m_gameScreen->getCharacter());
	m_bindings->setGameScreen(m_gameScreen.get());

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

}

// Executes basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());
	if (elapsedTime >= (1.0f / 30.0f))
		int mynum = 1;
	//UPDATE SCREENS
	switch (m_currentScreen){
	case MENU:
		m_menuScreen->update(elapsedTime);
		break;
	case GAME:
		m_gameScreen->update(elapsedTime);
		break;
	case LEVEL_SELECT:
		m_levelSelectScreen->update(elapsedTime);
		break;
	case PROFILE:
		m_profileScreen->update(elapsedTime);
		break;
	case ABILITY_SELECT:
		m_abilitySelectScreen->update(elapsedTime);
		break;
	case END_LEVEL:
		m_endLevelScreen->update(elapsedTime);
		break;
	}
	AudioManager::Instance()->Update(elapsedTime);
}

// Draws the scene
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

	// Clear the buffer with a lovely blue.
    Clear();
	float dt = (float)m_timer.GetElapsedSeconds();

	//RENDER SCREENS
	switch (m_currentScreen){
	case GAME:
		m_gameScreen->render(dt);
		break;
	case LOADING:
		m_loadingScreen->render(dt);
		break;
	case MENU:
		m_menuScreen->render(dt);
		break;
	case LEVEL_SELECT:
		m_levelSelectScreen->render(dt);
		break;
	case PROFILE:
		m_profileScreen->render(dt);
		break;
	case ABILITY_SELECT:
		m_abilitySelectScreen->render(dt);
		break;
	case END_LEVEL:
		m_endLevelScreen->render(dt);
		break;
	}
	

	// tell D3D to swap the buffers on the swap chain and paint our scene to the screen.
    Present();
}

// Helper method to clear the backbuffers
void Game::Clear()
{
    // Clear the views
    h_d3d->getContext()->ClearRenderTargetView(h_d3d->getRenderTarget(), Colors::CornflowerBlue);
    h_d3d->getContext()->ClearDepthStencilView(h_d3d->getDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    h_d3d->getContext()->OMSetRenderTargets(1, h_d3d->getRenderTargetAddress(), h_d3d->getDepthStencil());
}

// Presents the backbuffer contents to the screen
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = h_d3d->getSwapChain()->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        h_d3d->OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window
	m_wndActive = true;
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window
	m_wndActive = false;
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized)
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize)
}

void Game::OnWindowSizeChanged()
{
	h_d3d->CreateResources();
	float width = h_d3d->getBackBufferWidth();
	float height = h_d3d->getBackBufferHeight();
	switch (m_currentScreen){
	/*case MENU:
		m_menuScreen->resize(width, height);
		break;
	case LOADING:
		m_loadingScreen->resize(width, height);
		break;
	case LEVEL_SELECT:
		m_levelSelectScreen->resize(width, height);
		break;*/
	case GAME://This one breaks if it isn't currently active, probably due to camera issues
		m_gameScreen->resize((int)width, (int)height);
		break;
	}
	
	m_loadingScreen->resize((int)width, (int)height);
	m_levelSelectScreen->resize((int)width, (int)height);
	m_menuScreen->resize((int)width, (int)height);
	m_profileScreen->resize((int)width, (int)height);
	m_abilitySelectScreen->resize((int)width, (int)height);
	m_endLevelScreen->resize((int)width, (int)height);
    // TODO: Game window is being resized
}

void Game::OnNewAudioDevice(){
	AudioManager::Instance()->OnNewAudioDevice();
}

// Properties
void Game::GetDefaultSize(size_t& width, size_t& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200)
    width = 800;
    height = 600;
}

void Game::disableInput(){
	m_allowInput = false;
}

void Game::enableInput(){
	m_allowInput = true;
}
TileSet* Game::getTileSet(){
	return m_tileset.get();
}

bool Game::isWindActive(){
	return m_wndActive;
}

HWND Game::getWindow(){
	return m_window;
}

//switch to the specified screen
void Game::showScreen(SCREEN screen){
	switch (m_currentScreen){
	case GAME:
		m_gameScreen->hide();
		break;
	case LOADING:
		m_loadingScreen->hide();
		break;
	case MENU:
		m_menuScreen->hide();
		break;
	case LEVEL_SELECT:
		m_levelSelectScreen->hide();
		break;
	case PROFILE:
		m_profileScreen->hide();
		break;
	case ABILITY_SELECT:
		m_abilitySelectScreen->hide();
		break;
	case END_LEVEL:
		m_endLevelScreen->hide();
		m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Stratosphere.adpcm", SoundEffectInstance_Default);
		m_bgmusic->Play(true);
		break;
	}
	if (m_currentScreen == LOADING && m_loadingScreen->getText().compare("Closing and returning to menu")==0){//ESCAPE key was most likely pressed to end the game
		m_bgmusic = AudioManager::Instance()->CreateSoundInstance("Stratosphere.adpcm", SoundEffectInstance_Default);
		m_bgmusic->Play(true);
	}
	m_currentScreen = screen;
	switch (m_currentScreen){
	case GAME:
		AudioManager::Instance()->DeleteSoundInstance(m_bgmusic);
		m_bgmusic = nullptr;
		m_gameScreen->show();
		break;
	case LOADING:
		m_loadingScreen->show();
		break;
	case MENU:
		m_menuScreen->show();
		break;
	case LEVEL_SELECT:
		m_levelSelectScreen->show();
		break;
	case PROFILE:
		m_profileScreen->show();
		break;
	case ABILITY_SELECT:
		m_abilitySelectScreen->show();
		break;
	case END_LEVEL:
		m_endLevelScreen->show();
		break;
	}
}

GameScreen* Game::getGameScreen(){
	return m_gameScreen.get();
}

MenuScreen* Game::getMenuScreen(){
	return m_menuScreen.get();
}

LoadingScreen* Game::getLoadingScreen(){
	return m_loadingScreen.get();
}

LevelSelectScreen* Game::getLevelSelectScreen(){
	return m_levelSelectScreen.get();
}

ProfileScreen* Game::getProfileScreen(){
	return m_profileScreen.get();
}

AbilitySelectScreen* Game::getAbilitySelectScreen(){
	return m_abilitySelectScreen.get();
}

EndLevelScreen* Game::getEndLevelScreen(){
	return m_endLevelScreen.get();
}

Game::SCREEN Game::getCurrentScreen(){
	return m_currentScreen;
}

void Game::quitGame(bool end){
	if (end)
		showScreen(END_LEVEL);
	else
		showScreen(MENU);
	OnWindowSizeChanged();
	m_gameScreen.reset(new GameScreen(this));
	AudioManager::Instance()->setGameScreen(m_gameScreen.get());
	m_bindings->setGameScreen(m_gameScreen.get());
}

void Game::setExitCrusade(bool exit){
	m_exit = exit;
}

bool Game::exitCrusade(){
	return m_exit;
}

Profile* Game::getProfile(){
	return m_profile.get();
}

void Game::setProfile(Profile* profile){
	m_profile.reset(profile);
}

LevelLookUp* Game::getLevelLookUp(){
	return m_levelLookUp.get();
}