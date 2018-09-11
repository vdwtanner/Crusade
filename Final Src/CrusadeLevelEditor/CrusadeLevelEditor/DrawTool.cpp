#include "pch.h"
#include "DrawTool.h"
#include <math.h>

DrawTool::DrawTool(WorldEditor *editor){
	h_worldEditor = editor;
	m_world = Matrix::Identity;
	v1 = VertexPositionColor(Vector3(0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
	v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
	v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
	v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));

}

DrawTool::~DrawTool(){

}

void DrawTool::DrawCursor(PrimitiveBatch<VertexPositionColor> *batch){
	batch->DrawQuad(v1, v2, v3, v4);
}

void DrawTool::OnMouseMove(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	m_worldx = x;
	int y = floor(worldy);
	m_worldy = y;
	m_world = Matrix::CreateTranslation(x, y, 0);
	if (btnstate == MK_LBUTTON){
		h_worldEditor->placeTile(m_currTile, x, y);
	}
}

void DrawTool::OnMouseDown(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	int y = floor(worldy);
	if (btnstate == MK_LBUTTON){
		h_worldEditor->placeTile(m_currTile, x, y);
	}
}

void DrawTool::OnMouseUp(WPARAM btnstate, float worldx, float worldy){

}

Matrix DrawTool::getWorld(){
	return m_world;
}

void DrawTool::setWorld(Matrix m){
	m_world = m;
}
