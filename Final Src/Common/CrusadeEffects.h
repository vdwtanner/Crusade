#pragma once

#include "pch.h"
#include <string>

using namespace DirectX::SimpleMath;

class CrusadeEffect{
public:
	CrusadeEffect(ID3D11Device* device, const std::wstring& filename);
	virtual ~CrusadeEffect();

protected:
	ID3DX11Effect* m_FX;
};

class TexturedEffect : public CrusadeEffect{
public:
	TexturedEffect(ID3D11Device* device, const std::wstring& filename);
	~TexturedEffect();

	void SetWorldViewProj(Matrix M)				{ m_worldViewProj->SetMatrix(reinterpret_cast<const float *>(&M)); }
	void SetWorld(Matrix M)						{ m_world->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMaps(ID3D11ShaderResourceView* tex)   { m_diffuseMapArr->SetResource(tex); }
	void SetColorCorrect(Vector4 vect)			{ m_colorCorrect->SetFloatVector(reinterpret_cast<const float*>(&vect)); }

	ID3DX11EffectTechnique* m_SpriteTech;
	ID3DX11EffectTechnique* m_ColorBoxTech;

	ID3DX11EffectMatrixVariable* m_worldViewProj;
	ID3DX11EffectMatrixVariable* m_world;

	ID3DX11EffectShaderResourceVariable* m_diffuseMapArr;
	ID3DX11EffectVectorVariable* m_colorCorrect;
};

class ParticleEffect : public CrusadeEffect{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	void SetWorldViewProj(Matrix M)							{ m_worldViewProj->SetMatrix(reinterpret_cast<const float *>(&M)); }
	void SetDiffuseMaps(ID3D11ShaderResourceView* tex)		{ m_diffuseMapArr->SetResource(tex); }
	void SetRandTexture(ID3D11ShaderResourceView* rand)		{ m_randTexture->SetResource(rand); }
	void SetGlobalTime(float time)							{ m_globalTime->SetFloat(time); }
	void SetElaspedTime(float dt)							{ m_elapsedTime->SetFloat(dt); }
	void SetGravity(Vector4 grav)							{ m_gravity->SetFloatVector(reinterpret_cast<const float *>(&grav)); }
	void SetBloodSize(float size)							{ m_bloodSize->SetFloat(size); }

	ID3DX11EffectTechnique* m_StreamTech;
	ID3DX11EffectTechnique* m_DrawParticleTech;

	ID3DX11EffectMatrixVariable* m_worldViewProj;
	ID3DX11EffectShaderResourceVariable* m_diffuseMapArr;
	ID3DX11EffectShaderResourceVariable* m_randTexture;

	ID3DX11EffectScalarVariable* m_globalTime;
	ID3DX11EffectScalarVariable* m_elapsedTime;
	ID3DX11EffectVectorVariable* m_gravity;
	ID3DX11EffectScalarVariable* m_bloodSize;

};

class CrusadeEffects{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static TexturedEffect* TextureFX;
	static ParticleEffect* ParticleFX;
};