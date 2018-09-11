#pragma once
#include "pch.h"

class PipelineStates{
public:
	static void InitAll(ID3D11Device *device);
	static void DestroyAll();
	
	static ID3D11BlendState *BasicAlpha;
	static ID3D11DepthStencilState *DepthDisabled;
};