#include "pch.h"
#include "CrusadeVertexTypes.h"
#include "CrusadeEffects.h"
#include "d3dx11effect.h"
#include "VertexTypes.h"

const D3D11_INPUT_ELEMENT_DESC InputLayouts::TexturedVertexDesc[5] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXINDEX", 0, DXGI_FORMAT_R32_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::ParticleDesc[8] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TIMER", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "EMITTER_INTERVAL", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "ETYPE", 0, DXGI_FORMAT_R32_UINT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

ID3D11InputLayout* InputLayouts::TexturedVertexLayout = 0;
ID3D11InputLayout* InputLayouts::ParticleVertexLayout = 0;

void InputLayouts::InitAll(ID3D11Device *device){
	D3DX11_PASS_DESC passDesc;

	CrusadeEffects::TextureFX->m_SpriteTech->GetPassByIndex(0)->GetDesc(&passDesc);
	DX::ThrowIfFailed(device->CreateInputLayout(InputLayouts::TexturedVertexDesc, 5, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &TexturedVertexLayout));

	CrusadeEffects::ParticleFX->m_DrawParticleTech->GetPassByIndex(0)->GetDesc(&passDesc);
	DX::ThrowIfFailed(device->CreateInputLayout(InputLayouts::ParticleDesc, 8, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &ParticleVertexLayout));

}

void InputLayouts::DestroyAll(){
	SAFE_RELEASE(TexturedVertexLayout);
	SAFE_RELEASE(ParticleVertexLayout);
}