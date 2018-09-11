#include "pch.h"
#include "TextureLoader.h"
#include <math.h>

// based off of Frank Luna's CreateTexture2DArraySRV in his book Introduction to 3D Game Programming with Directx 11
// his code has been modified to remove a d3dx library dependency

// This function loads a texturearray sequentially, aka, the order of the file paths will be the order
void TextureLoader::LoadSeqTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::vector< wchar_t *> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output){
	ID3D11Texture2D *tempTexture;
	std::vector<ID3D11Texture2D*> tempTextureVect;

	for (int i = 0; i < offset; i++){
		tempTextureVect.push_back(nullptr);
	}

	std::vector<wchar_t *>::iterator it;
	for (it = filePaths.begin(); it != filePaths.end(); it++){
		wchar_t *filepath = *(it);
		DX::ThrowIfFailed(CreateWICTextureFromFileEx(device,
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
		//textures[it->first] = tempTexture;
		tempTextureVect.push_back(tempTexture);
	}

	UINT maxW = 0;
	UINT maxH = 0;

	D3D11_TEXTURE2D_DESC texElementDesc;
	for (int i = offset; (UINT)i < tempTextureVect.size(); i++){
		tempTextureVect[i]->GetDesc(&texElementDesc);
		maxW = std::max(maxW, texElementDesc.Width);
		maxH = std::max(maxH, texElementDesc.Height);

	}

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = maxW;
	texArrayDesc.Height = maxH;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = tempTextureVect.size();
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	DX::ThrowIfFailed(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	for (UINT texElement = 0; texElement < tempTextureVect.size(); texElement++){
		if (tempTextureVect[texElement]){
			for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; mipLevel++){
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				DX::ThrowIfFailed(context->Map(tempTextureVect[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
				context->UpdateSubresource(
					texArray,
					D3D11CalcSubresource(
					mipLevel,
					texElement,
					texElementDesc.MipLevels),

					0,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					mappedTex2D.DepthPitch);
				context->Unmap(tempTextureVect[texElement], mipLevel);
			}
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texElementDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = tempTextureVect.size();

	DX::ThrowIfFailed(device->CreateShaderResourceView(texArray, &viewDesc, output));

	SAFE_RELEASE(texArray);


	for (UINT i = 0; i < tempTextureVect.size(); i++){
		SAFE_RELEASE(tempTextureVect[i]);
	}

}

void TextureLoader::LoadSeqTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::map< UINT, wchar_t *> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output){
	ID3D11Texture2D *tempTexture;
	std::vector<ID3D11Texture2D*> tempTextureVect;

	for (int i = 0; i < offset; i++){
		tempTextureVect.push_back(nullptr);
	}

	std::map<UINT, wchar_t *>::iterator it;
	for (it = filePaths.begin(); it != filePaths.end(); it++){
		wchar_t *filepath = it->second;
		DX::ThrowIfFailed(CreateWICTextureFromFileEx(device,
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
		//textures[it->first] = tempTexture;
		tempTextureVect.push_back(tempTexture);
	}

	UINT maxW = 0;
	UINT maxH = 0;

	D3D11_TEXTURE2D_DESC texElementDesc;
	for (int i = offset; (UINT)i < tempTextureVect.size(); i++){
		tempTextureVect[i]->GetDesc(&texElementDesc);
		maxW = std::max(maxW, texElementDesc.Width);
		maxH = std::max(maxH, texElementDesc.Height);

	}

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = tempTextureVect.size();
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	DX::ThrowIfFailed(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	for (UINT texElement = offset; texElement < tempTextureVect.size(); texElement++){
		if (tempTextureVect[texElement]){
			for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; mipLevel++){
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				DX::ThrowIfFailed(context->Map(tempTextureVect[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
				context->UpdateSubresource(
					texArray,
					D3D11CalcSubresource(
					mipLevel,
					texElement,
					texElementDesc.MipLevels),

					0,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					mappedTex2D.DepthPitch);
				context->Unmap(tempTextureVect[texElement], mipLevel);
			}
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texElementDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = tempTextureVect.size();

	DX::ThrowIfFailed(device->CreateShaderResourceView(texArray, &viewDesc, output));

	SAFE_RELEASE(texArray);


	for (UINT i = 0; i < tempTextureVect.size(); i++){
		SAFE_RELEASE(tempTextureVect[i]);
	}

}


void TextureLoader::LoadKeyedTextureArray(ID3D11Device *device, ID3D11DeviceContext* context, std::map<UINT, wchar_t *> filePaths, int offset, _Out_ ID3D11ShaderResourceView **output, _Out_ std::map<UINT, UINT> *keymap){
	ID3D11Texture2D *tempTexture;
	std::vector<ID3D11Texture2D*> tempTextureVect;
	keymap->clear();

	for (int i = 0; i < offset; i++){
		tempTextureVect.push_back(nullptr);
		keymap->insert(keymap->end(), std::pair<UINT, UINT>(0, 0));
	}

	std::map<UINT, wchar_t *>::iterator it;
	UINT indexCount = offset;

	for (it = filePaths.begin(); it != filePaths.end(); it++){
		wchar_t *filepath = it->second;
		DX::ThrowIfFailed(CreateWICTextureFromFileEx(device,
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
		//textures[it->first] = tempTexture;
		tempTextureVect.push_back(tempTexture);
		keymap->insert(keymap->end(), std::pair<UINT, UINT>(it->first, indexCount));
		//keymap[it->first] = indexCount;
		indexCount++;
	}

	UINT maxW = 0;
	UINT maxH = 0;

	D3D11_TEXTURE2D_DESC texElementDesc;
	D3D11_TEXTURE2D_DESC dirtDesc;
	tempTextureVect[1]->GetDesc(&dirtDesc);
	for (int i = offset; (UINT)i < tempTextureVect.size(); i++){
		tempTextureVect[i]->GetDesc(&texElementDesc);
		//maxW = std::max(maxW, texElementDesc.Width);
		//maxH = std::max(maxH, texElementDesc.Height);

	}

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = tempTextureVect.size();
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	DX::ThrowIfFailed(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	for (UINT texElement = 0; texElement < tempTextureVect.size(); texElement++){
		if (tempTextureVect[texElement]){
			for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; mipLevel++){
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				DX::ThrowIfFailed(context->Map(tempTextureVect[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));
				context->UpdateSubresource(
					texArray,
					D3D11CalcSubresource(
					mipLevel,
					texElement,
					texElementDesc.MipLevels),

					0,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					mappedTex2D.DepthPitch);
				context->Unmap(tempTextureVect[texElement], mipLevel);
			}
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texElementDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = tempTextureVect.size();

	DX::ThrowIfFailed(device->CreateShaderResourceView(texArray, &viewDesc, output));

	SAFE_RELEASE(texArray);


	for (UINT i = 0; i < tempTextureVect.size(); i++){
		SAFE_RELEASE(tempTextureVect[i]);
	}

}