#pragma once

#include "pch.h"
#include "WorldEditor.h"
#include "Tool.h"

class EnemyTool : public Tool{
public:

	EnemyTool(WorldEditor *editor);
	~EnemyTool();

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


};