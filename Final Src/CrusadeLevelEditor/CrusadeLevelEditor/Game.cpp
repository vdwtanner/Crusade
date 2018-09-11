//
// Game.cpp -
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

// Create an Orthographic Camera
								// (ortho?) (viewport dimensions)
Camera *Game::m_camera = new Camera(true, Vector2(30, 20));



// Constructor.
Game::Game() :
	m_window(0)
{
}

Game::~Game(){
	delete m_camera;
	m_character.reset(nullptr);
	m_worldEditor.reset(nullptr);

	delete D3DManager::Instance();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window)
{
    m_window = window;

	h_d3d = D3DManager::Instance();
	h_d3d->Initialize(window);
	float width = h_d3d->getBackBufferWidth();
	float height = h_d3d->getBackBufferHeight();
	m_camera->Resize(width, height);
	m_camera->SetViewportOrtho(30, 30 * (height / width));

	// Z is our depth
	m_camera->Init(Vector3(0, 0, 5.0f), Vector3(0, 0, 0));

	m_worldEditor.reset(new WorldEditor(h_d3d->getDevice(), h_d3d->getContext()));
	
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
	if (m_wndActive){
		SetFocus(m_window);
		// TODO: Add your game logic here
		if (GetAsyncKeyState('W') & 0x8000){	// dont know why you need the bitwise and, just do it
			m_camera->Move(0, 10 * elapsedTime*m_camera->getZoomLevel(), 0);
		}
		if (GetAsyncKeyState('A') & 0x8000){
			m_camera->Move(-10 * elapsedTime*m_camera->getZoomLevel(), 0, 0);
		}
		if (GetAsyncKeyState('S') & 0x8000){
			m_camera->Move(0, -10 * elapsedTime*m_camera->getZoomLevel(), 0);
		}
		if (GetAsyncKeyState('D') & 0x8000){
			m_camera->Move(10 * elapsedTime*m_camera->getZoomLevel(), 0, 0);
		}
		if (GetAsyncKeyState('Q') & 0x8000){
			m_camera->ZoomOrtho(-10 * elapsedTime);
		}
		if (GetAsyncKeyState('E') & 0x8000){
			m_camera->ZoomOrtho(10 * elapsedTime);
		}
	}

	m_camera->Update();
	m_worldEditor->Update(elapsedTime);

}

// Draws the scene
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    Clear();

	m_worldEditor->Render();

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
        OnDeviceLost();
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
	m_camera->Resize(width, height);
	m_camera->SetViewportOrtho(30, 30 * (height/width));
    // TODO: Game window is being resized
	m_worldEditor->getGrid()->Resize();
}

// Properties
void Game::GetDefaultSize(size_t& width, size_t& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200)
    width = 800;
    height = 600;
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here

	InputLayouts::DestroyAll();
	CrusadeEffects::DestroyAll();
	PipelineStates::DestroyAll();
	InputLayouts::InitAll(h_d3d->getDevice());
	CrusadeEffects::InitAll(h_d3d->getDevice());
	PipelineStates::InitAll(h_d3d->getDevice());

	m_worldEditor->setDeviceAndContextHandle(h_d3d->getDevice(), h_d3d->getContext());
}

WorldEditor* Game::GetWorldEditor(){
	return m_worldEditor.get();
}