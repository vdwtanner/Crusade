#pragma once

#include "pch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "SimpleMath.h"
#include "WICTextureLoader.h"
#include "WorldTile.h"
#include "rapidxml.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace rapidxml;

class TileSet{
public:
	TileSet(ID3D11Device *device, ID3D11DeviceContext *context);
	~TileSet();

	bool LoadTileSet(LPWSTR filepath);
	void PopulateListBox(HWND listbox_hwnd);
	void LoadTextures();
	ID3D11ShaderResourceView* getTextureFromId(UINT id);
	ID3D11ShaderResourceView* getTextureArr();
	UINT getTextureIndex(UINT id);
	int getTextureArrSize();

	std::vector<UINT> getIdVector();
	int getHpFromId(UINT);

	void setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context);

private:
	//std::string loadFileAsString(LPWSTR);
	
	ID3D11Device							*h_device;
	ID3D11DeviceContext						*h_context;

	std::vector<UINT>							m_ids;
	std::map<UINT, wchar_t *>					m_filePaths;
	std::map<UINT, wchar_t *>					m_names;
	std::map<UINT, int>						m_healths;

	std::map<UINT, UINT>						m_keymap;

	//std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	m_textureMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_texturesSRV;
	
};