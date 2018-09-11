//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0602
#include <SDKDDKVer.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#define SAFE_RELEASE(p) if(p){(p)->Release(); (p) = nullptr;}

#include <windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include "d3dx11effect.h"
#include "SimpleMath.h"
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <fstream>
#include <vector>

#include <algorithm>
#include <exception>
#include <memory>

#include "CrusadeEffects.h"
#include "CrusadeVertexTypes.h"
#include "PipelineStates.h"
#include "D3DManager.h"

#include "TextureLoader.h"

#include "EnemyManager.h"


namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}