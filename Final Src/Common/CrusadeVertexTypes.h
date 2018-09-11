#pragma once

#include "pch.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

#define USE_TEXTURE 0
#define USE_COLOR_ONLY 1

#define PT_EMITTER	10
#define PT_BLOOD	1
#define PT_RAIN		2
#define PT_SMOKE	3
#define PT_EMBER	4
#define PT_BLOCK	5

#define PT_DEATHGORE 6
#define PT_HOLYSHOWER 7
#define PT_EXPBURST 8

namespace CrusadeVertexTypes{

	struct TexturedVertex{
		Vector3 Pos;
		Vector2 Size;
		float Rotation;
		UINT TexIndex;
		Vector4 Color;
	};
	struct Particle{
		Vector3 pos;
		Vector2 vel;
		Vector2 size;
		float timer;
		float emitInterval;
		UINT type;
		UINT etype;
		UINT texIndex;
	};
}

class InputLayouts{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	// descriptions of all our layouts
	static const D3D11_INPUT_ELEMENT_DESC TexturedVertexDesc[5];
	static const D3D11_INPUT_ELEMENT_DESC ParticleDesc[8];

	// our layouts
	static ID3D11InputLayout *TexturedVertexLayout;
	static ID3D11InputLayout *ParticleVertexLayout;
};