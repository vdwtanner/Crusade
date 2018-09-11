//
// Game.h
//

#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "Camera.h"
#include "WorldEditor.h"
#include "WICTextureLoader.h"
#include "Character.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// A basic game implementation that creates a D3D11 device and
// provides a game loop

class WorldEditor;

class Game
{


public:
	static Camera *m_camera;

    Game();
	~Game();

    // Initialization and management
    void Initialize(HWND window);

    // Basic game loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();
    void Present();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged();

    // Properites
    void GetDefaultSize( size_t& width, size_t& height ) const;
	WorldEditor* GetWorldEditor();

private:

    void Update(DX::StepTimer const& timer);

	void CreateGeometryBuffers();
    
    void OnDeviceLost();

	//Manager that takes care of initializing DirectX
	D3DManager										*h_d3d;

	// The renderables
	std::unique_ptr<Character>						m_character;
	std::unique_ptr<WorldEditor>					m_worldEditor;
	// Enemy Manager goes here in spirit.

	bool											m_wndActive;

    // Application state
    HWND                                            m_window;
	
	float											m_sHeight;
	float											m_sWidth;

    // Game state
    DX::StepTimer                                   m_timer;


};