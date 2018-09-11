#include "pch.h"
#include "TileSet.h"
#include "WICTextureLoader.h"
#include "CrusadeUtil.h"

TileSet::TileSet(ID3D11Device *device, ID3D11DeviceContext *context) : m_texturesSRV(0){
	h_device = device;
	h_context = context;
}

TileSet::~TileSet(){
	std::map<UINT, wchar_t *>::iterator it;
	for (it = m_filePaths.begin(); it != m_filePaths.end(); it++){
		delete[] it->second;
	}
	for (it = m_names.begin(); it != m_names.end(); it++){
		delete[] it->second;
	}
	m_texturesSRV.Reset();
	/*std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>::iterator textIt;
	for (textIt = m_textureMap.begin(); textIt != m_textureMap.end(); textIt++){
		textIt->second.Reset();
	}*/
}

bool TileSet::LoadTileSet(LPWSTR filepath){
	m_ids.clear();
	m_names.clear();
	m_healths.clear();
	m_filePaths.clear();

	std::string pathCorrection = "../img/";//This is so that we can have paths for tiles be correct
	xml_document<> doc;
	std::string  xml = CrusadeUtil::loadFileAsString(filepath);
	const char *c_xml = xml.c_str();
	//strcpy(c_xml, xml.c_str());
	doc.parse<0>((char*)c_xml);
	xml_node<> *tile = doc.first_node()->first_node("Tiles");
	if (!tile)
		return 0;
	do{
		int id = std::stoi(tile->first_node("ID")->value());
		char *name = tile->first_node("Name")->value();
		bool isAtlas = (strcmp(tile->first_node("IsAtlas")->value(), "TRUE") == 0) ? true : false;
		int health = std::stoi(tile->first_node("HP")->value());
		char *path = tile->first_node("Path")->value();
		//SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(s_to_ws(name)));

		wchar_t *temp;
		CrusadeUtil::s_to_ws(name, &temp);
		wchar_t *wpath;
		CrusadeUtil::s_to_ws(pathCorrection + path, &wpath);

		//std::wstring mystr(filepath);
		//std::wstring myother(wpath);
		//std::wstring concat = mystr + L"\\" + myother;

		m_ids.push_back(id);
		m_names.insert(m_names.end(), std::pair<UINT, wchar_t *>(id, temp));
		m_healths.insert(m_healths.end(), std::pair<UINT, int>(id, health));
		m_filePaths.insert(m_filePaths.end(), std::pair<UINT, wchar_t *>(id, wpath));

		tile = tile->next_sibling();
	} while (tile);
	LoadTextures();
	return 1;
}

// based off of Frank Luna's CreateTexture2DArraySRV in his book Introduction to 3D Game Programming with Directx 11
// his code has been modified to remove a d3dx library dependency
void TileSet::LoadTextures(){
/*	std::vector<ID3D11Texture2D*> textures((--m_filePaths.end())->first+1);
	ID3D11Texture2D *tempTexture;
	std::map<UINT, wchar_t *>::iterator it;
	for (it = m_filePaths.begin(); it != m_filePaths.end(); it++){
		ID3D11Resource *resource;
		wchar_t *filepath = it->second;
		DX::ThrowIfFailed(CreateWICTextureFromFileEx(h_device,
										filepath,
										10000,
										D3D11_USAGE_STAGING,
										0,
										D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
										0,
										false,
										(ID3D11Resource **)&tempTexture,
										nullptr));
		//m_textureMap.insert(m_textureMap.end(), std::pair<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(it->first, view));
		textures[it->first] = tempTexture;
	}

	D3D11_TEXTURE2D_DESC texElementDesc;
	textures[1]->GetDesc(&texElementDesc);
	
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = textures.size();
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0; 
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	DX::ThrowIfFailed(h_device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	for (UINT texElement = 0; texElement < textures.size(); texElement++){
		if (textures[texElement]){
			for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; mipLevel++){
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				DX::ThrowIfFailed(h_context->Map(textures[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
				h_context->UpdateSubresource(
						texArray,
						D3D11CalcSubresource(
						mipLevel,
						texElement,
						texElementDesc.MipLevels),

						0,
						mappedTex2D.pData,
						mappedTex2D.RowPitch,
						mappedTex2D.DepthPitch);
				h_context->Unmap(textures[texElement], mipLevel);
			}
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = textures.size();

	DX::ThrowIfFailed(h_device->CreateShaderResourceView(texArray, &viewDesc, &m_textureArr));

	SAFE_RELEASE(texArray);

	for (UINT i = 0; i < m_filePaths.size(); i++){
		SAFE_RELEASE(textures[i]);
	}*/

	//TextureLoader::LoadSeqTextureArray(h_device, h_context, m_filePaths, 1, &m_texturesSRV);
	TextureLoader::LoadKeyedTextureArray(D3DManager::Instance()->getDevice(), D3DManager::Instance()->getContext(), m_filePaths, 1, m_texturesSRV.ReleaseAndGetAddressOf(), &m_keymap);
}

void TileSet::PopulateListBox(HWND listbox_hwnd){
	SendMessage(listbox_hwnd, LB_RESETCONTENT, 0, 0);
	int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(L"ERASER"));
	SendMessage(listbox_hwnd, LB_SETITEMDATA, pos, (LPARAM)(0));

	for (int i = 0; (UINT)i < m_names.size(); i++){
		int pos = (int)SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)(m_names.at(m_ids.at(i))));
		// lets add the id of each tile to its listbox string
		SendMessage(listbox_hwnd, LB_SETITEMDATA, pos, (LPARAM)m_ids.at(i));
	}
}


/*std::string TileSet::loadFileAsString(LPWSTR path){
	using namespace std;
	ifstream readFile;
	string output = "";
	string temp;
	readFile.open(path);
	if (readFile.is_open()){
		while (!readFile.eof()){
			getline(readFile, temp);
			output += temp + "\n";
		}
	}
	return output;
}*/

std::vector<UINT> TileSet::getIdVector(){
	return m_ids;
}

ID3D11ShaderResourceView* TileSet::getTextureFromId(UINT id){
	/*if (m_textureMap.find(id) != m_textureMap.end())
		return m_textureMap.find(id)->second.Get();
	else*/
		return 0;
}

ID3D11ShaderResourceView* TileSet::getTextureArr(){
	return m_texturesSRV.Get();
}

UINT TileSet::getTextureIndex(UINT id){
	return m_keymap[id];
}

int TileSet::getTextureArrSize(){
	return m_filePaths.size();
}

void TileSet::setDeviceAndContextHandle(ID3D11Device *device, ID3D11DeviceContext *context){
	h_device = device;
	h_context = context;

	if (m_filePaths.size() > 0)
		LoadTextures();
}

int TileSet::getHpFromId(UINT id){
	return m_healths[id];
}


