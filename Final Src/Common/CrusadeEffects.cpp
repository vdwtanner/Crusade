#include "pch.h"
#include "CrusadeEffects.h"

//
//	Effect class
//
CrusadeEffect::CrusadeEffect(ID3D11Device* device, const std::wstring& filename) : m_FX(0){
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	DX::ThrowIfFailed(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &m_FX));
}

CrusadeEffect::~CrusadeEffect(){
	SAFE_RELEASE(m_FX);
}

// END EFFECT CLASS

//
//	TextureEffect class
//
TexturedEffect::TexturedEffect(ID3D11Device* device, const std::wstring& filename)
	: CrusadeEffect(device, filename)
{
	m_SpriteTech = m_FX->GetTechniqueByName("SpriteTech");
	m_ColorBoxTech = m_FX->GetTechniqueByName("ColorBoxTech");

	m_worldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_world = m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_diffuseMapArr = m_FX->GetVariableByName("gDiffuseMaps")->AsShaderResource();
	m_colorCorrect = m_FX->GetVariableByName("gColorCorrect")->AsVector();
}

TexturedEffect::~TexturedEffect(){

}

// END TEXTUREEFFECT CLASS

//
//	ParticleEffect class
//
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: CrusadeEffect(device, filename)
{
	m_StreamTech = m_FX->GetTechniqueByName("UpdateParticles");
	m_DrawParticleTech = m_FX->GetTechniqueByName("DrawParticles");

	m_worldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_diffuseMapArr = m_FX->GetVariableByName("gDiffuseMaps")->AsShaderResource();
	m_randTexture = m_FX->GetVariableByName("gtexRandom")->AsShaderResource();

	m_globalTime = m_FX->GetVariableByName("gGlobalTime")->AsScalar();
	m_elapsedTime = m_FX->GetVariableByName("gElapsedTime")->AsScalar();
	m_gravity = m_FX->GetVariableByName("gGravity")->AsVector();
	m_bloodSize = m_FX->GetVariableByName("gBloodParticleSize")->AsScalar();
}

ParticleEffect::~ParticleEffect(){

}

// END PARTICLEEFFECT CLASS

TexturedEffect* CrusadeEffects::TextureFX = 0;
ParticleEffect* CrusadeEffects::ParticleFX = 0;

void CrusadeEffects::InitAll(ID3D11Device* device)
{
	TextureFX = new TexturedEffect(device, L"../Common/FX/Texture.fxo");
	ParticleFX = new ParticleEffect(device, L"../Common/FX/Particles.fxo");
}

void CrusadeEffects::DestroyAll()
{
	delete TextureFX;
	TextureFX = 0;
	delete ParticleFX;
	ParticleFX = 0;
}
