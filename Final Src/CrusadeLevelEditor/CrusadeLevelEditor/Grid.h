#pragma once

#include "pch.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "SimpleMath.h"
#include "Game.h"
#include <vector>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Grid
{
public:

	Grid();
	~Grid();
	Grid(float, float);
	void Draw(PrimitiveBatch<VertexPositionColor> *batch);
	void Update(float dt);
	Matrix getWorld();
	void setWorld(Matrix m);
	void Resize();
	int getWidth();
	int getHeight();

private:
	void buildGridVerts();
	float m_width;
	float m_height;
	Matrix m_world;
	std::vector<VertexPositionColor> vertArr;
};