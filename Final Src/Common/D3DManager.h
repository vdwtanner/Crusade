#pragma once
#include "pch.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class D3DManager{
public:
	static D3DManager* Instance();
	D3DManager();
	~D3DManager();
	void Initialize(HWND window);

	ID3D11DeviceContext* getContext();
	ID3D11Device* getDevice();

	ID3D11RenderTargetView* getRenderTarget();
	ID3D11RenderTargetView** getRenderTargetAddress();
	ID3D11DepthStencilView* getDepthStencil();
	IDXGISwapChain*			getSwapChain();

	float getBackBufferWidth();
	float getBackBufferHeight();

	void OnDeviceLost();

	void CreateDevice();
	void CreateResources();
private:


	static D3DManager *m_instance;

	HWND                                            h_window;

	// Direct3D Objects
	D3D_FEATURE_LEVEL                               m_featureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;

	// Rendering resources
	Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencil;

	float											m_bbWidth;
	float											m_bbHeight;


};