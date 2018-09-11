#include "pch.h"
#include "PlayerSpawnTool.h"

PlayerSpawnTool::PlayerSpawnTool(WorldEditor *editor){
	h_worldEditor = editor;
	m_world = Matrix::Identity;
	v1 = VertexPositionColor(Vector3(0.0f, 0.0f, 1.0f), Vector4(.8f, 0.0f, 0.9f, .5f));
	v2 = VertexPositionColor(Vector3(0.0f, 1.5f, 1.0f), Vector4(.8f, 0.0f, 0.9f, .5f));
	v3 = VertexPositionColor(Vector3(1.0f, 1.5f, 1.0f), Vector4(.8f, 0.0f, 0.9f, .5f));
	v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 1.0f), Vector4(.8f, 0.0f, 0.9f, .5f));

}

PlayerSpawnTool::~PlayerSpawnTool(){

}

void PlayerSpawnTool::DrawCursor(PrimitiveBatch<VertexPositionColor> *batch){
	batch->DrawQuad(v1, v2, v3, v4);
}

void PlayerSpawnTool::OnMouseMove(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	m_worldx = x;
	int y = floor(worldy);
	m_worldy = y;
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		m_world = Matrix::CreateTranslation(worldx - .5f, y, 0);
	else
		m_world = Matrix::CreateTranslation(worldx - .5f, worldy - .5f, 0);

}

void PlayerSpawnTool::OnMouseDown(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	int y = floor(worldy);
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		h_worldEditor->setCharacterPosition(Vector2(x-.5, worldy-.5));
	}
	else
	{
		h_worldEditor->setCharacterPosition(Vector2(worldx-.5, worldy-.5));
	}
}

void PlayerSpawnTool::OnMouseUp(WPARAM btnstate, float worldx, float worldy){

}

Matrix PlayerSpawnTool::getWorld(){
	return m_world;
}

void PlayerSpawnTool::setWorld(Matrix m){
	m_world = m;
}
