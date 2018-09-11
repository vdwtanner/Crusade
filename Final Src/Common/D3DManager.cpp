#include "pch.h"
#include "D3DManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

D3DManager* D3DManager::m_instance = nullptr;

D3DManager* D3DManager::Instance(){
	if (!m_instance){
		m_instance = new D3DManager();
	}
	return m_instance;
}

D3DManager::D3DManager() : m_featureLevel(D3D_FEATURE_LEVEL_9_1){

}

D3DManager::~D3DManager(){
	CrusadeEffects::DestroyAll();
	PipelineStates::DestroyAll();
	m_depthStencil.Reset();
	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain1.Reset();
	m_swapChain.Reset();
	m_d3dContext1.Reset();
	m_d3dContext.Reset();
	m_d3dDevice1.Reset();
	m_d3dDevice.Reset();
}

void D3DManager::Initialize(HWND window){
	h_window = window;

	CreateDevice();

	CreateResources();

	CrusadeEffects::InitAll(m_d3dDevice.Get());
	PipelineStates::InitAll(m_d3dDevice.Get());
	InputLayouts::InitAll(m_d3dDevice.Get());
}

// These are the resources that depend on the device.
void D3DManager::CreateDevice()
{
	// This flag adds support for surfaces with a different color channel ordering than the API default.
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels (see code below related to 11.1 fallback handling)
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	HRESULT hr = D3D11CreateDevice(
		nullptr,                                // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,                                // leave as nullptr unless software device
		creationFlags,                          // optionally set debug and Direct2D compatibility flags
		featureLevels,                          // list of feature levels this app can support
		_countof(featureLevels),                // number of entries in above list
		D3D11_SDK_VERSION,                      // always set this to D3D11_SDK_VERSION
		m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
		&m_featureLevel,                        // returns feature level of device created
		m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
		);

	if (hr == E_INVALIDARG)
	{
		// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			creationFlags, &featureLevels[1], _countof(featureLevels) - 1,
			D3D11_SDK_VERSION, m_d3dDevice.ReleaseAndGetAddressOf(),
			&m_featureLevel, m_d3dContext.ReleaseAndGetAddressOf());
	}

	DX::ThrowIfFailed(hr);

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	hr = m_d3dDevice.As(&d3dDebug);
	if (SUCCEEDED(hr))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		hr = d3dDebug.As(&d3dInfoQueue);
		if (SUCCEEDED(hr))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed 
			};
			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	// TODO: Initialize device dependent objects here (independent of window size)
}

// Allocate all memory resources that change on a window SizeChanged event.
void D3DManager::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	RECT rc;
	GetWindowRect(h_window, &rc);

	UINT backBufferWidth = std::max<UINT>(rc.right - rc.left, 1);
	UINT backBufferHeight = std::max<UINT>(rc.bottom - rc.top, 1);
	m_bbWidth = (float)backBufferWidth;
	m_bbHeight = (float)backBufferHeight;
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	DXGI_FORMAT depthBufferFormat = (m_featureLevel >= D3D_FEATURE_LEVEL_10_0) ? DXGI_FORMAT_D32_FLOAT : DXGI_FORMAT_D16_UNORM;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(2, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory1> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory));

		ComPtr<IDXGIFactory2> dxgiFactory2;
		HRESULT hr = dxgiFactory.As(&dxgiFactory2);
		if (SUCCEEDED(hr))
		{
			// DirectX 11.1 or later
			m_d3dDevice.As(&m_d3dDevice1);
			m_d3dContext.As(&m_d3dContext1);

			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = backBufferFormat;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
			fsSwapChainDesc.Windowed = TRUE;

			// Create a SwapChain from a CoreWindow.
			DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
				m_d3dDevice.Get(), h_window, &swapChainDesc,
				&fsSwapChainDesc,
				nullptr, m_swapChain1.ReleaseAndGetAddressOf()));

			m_swapChain1.As(&m_swapChain);
		}
		else
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
			swapChainDesc.BufferCount = 2;
			swapChainDesc.BufferDesc.Width = backBufferWidth;
			swapChainDesc.BufferDesc.Height = backBufferHeight;
			swapChainDesc.BufferDesc.Format = backBufferFormat;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = h_window;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = TRUE;

			DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
		}

		// This template does not support 'full-screen' mode and prevents the ALT+ENTER shortcut from working
		dxgiFactory->MakeWindowAssociation(h_window, DXGI_MWA_NO_ALT_ENTER);
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

	// Create a viewport descriptor of the full window size.
	CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(backBufferWidth), static_cast<float>(backBufferHeight));

	// Set the current viewport using the descriptor.
	m_d3dContext->RSSetViewports(1, &viewPort);

	// TODO: Initialize windows-size dependent objects here
}

ID3D11DeviceContext* D3DManager::getContext(){
	return m_d3dContext.Get();
}
ID3D11Device* D3DManager::getDevice(){
	return m_d3dDevice.Get();
}

ID3D11RenderTargetView* D3DManager::getRenderTarget(){
	return m_renderTargetView.Get();
}

ID3D11RenderTargetView** D3DManager::getRenderTargetAddress(){
	return m_renderTargetView.GetAddressOf();
}

ID3D11DepthStencilView* D3DManager::getDepthStencil(){
	return m_depthStencilView.Get();
}

IDXGISwapChain* D3DManager::getSwapChain(){
	return m_swapChain.Get();
}

float D3DManager::getBackBufferWidth(){
	return m_bbWidth;
}

float D3DManager::getBackBufferHeight(){
	return m_bbHeight;
}

void D3DManager::OnDeviceLost(){
	m_depthStencil.Reset();
	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain1.Reset();
	m_swapChain.Reset();
	m_d3dContext1.Reset();
	m_d3dContext.Reset();
	m_d3dDevice1.Reset();
	m_d3dDevice.Reset();

	CreateDevice();

	CreateResources();
}
