#pragma once

#include "pch.h"
#include "WorldEditor.h"
#include "Tool.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class WorldEditor;

class BoxTool : public Tool{

public:

	BoxTool(WorldEditor *editor);
	~BoxTool();

	void DrawCursor(PrimitiveBatch<VertexPositionColor> *batch);
	void OnMouseMove(WPARAM btnstate, float worldx, float worldy);
	void OnMouseDown(WPARAM btnstate, float worldx, float worldy);
	void OnMouseUp(WPARAM btnstate, float worldx, float worldy);

	Matrix getWorld();
	void setWorld(Matrix m);

private:
	WorldEditor *h_worldEditor;
	Matrix m_world;
	VertexPositionColor v1;
	VertexPositionColor v2;
	VertexPositionColor v3;
	VertexPositionColor v4;

	Vector2 *start;
	Vector2 *stop;

};