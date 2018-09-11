#pragma once

#include "pch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "SimpleMath.h"
#include "WICTextureLoader.h"
#include "WorldTile.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class TextureLoader{
public:
	enum TextureSet{
		TILESET,
		ENEMYSET,
		CHARACTERSET,
		ENTITYSET
	};

	static void LoadSeqTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::vector<wchar_t*> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output);
	static void LoadSeqTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::map<UINT, wchar_t *> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output);

	// outputs a keymap with the shaderResourceView, which has <id, index in the resource view> pairs
	static void LoadKeyedTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::map<UINT, wchar_t *> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output, _Out_ std::map<UINT, UINT> *keymap);
};