#include "pch.h"
#include "EnemyTool.h"

EnemyTool::EnemyTool(WorldEditor *editor){
	h_worldEditor = editor;
	m_world = Matrix::Identity;
	v1 = VertexPositionColor(Vector3(0.0f, 0.0f, 1.0f), Vector4(.8f, 0.0f, 0.0f, .5f));
	v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 1.0f), Vector4(.8f, 0.0f, 0.0f, .5f));
	v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 1.0f), Vector4(.8f, 0.0f, 0.0f, .5f));
	v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 1.0f), Vector4(.8f, 0.0f, 0.0f, .5f));

}

EnemyTool::~EnemyTool(){

}

void EnemyTool::DrawCursor(PrimitiveBatch<VertexPositionColor> *batch){
	batch->DrawQuad(v1, v2, v3, v4);
}

void EnemyTool::OnMouseMove(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	m_worldx = x;
	int y = floor(worldy);
	m_worldy = y;
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		m_world = Matrix::CreateTranslation(worldx-.5f, y, 0);
	else
		m_world = Matrix::CreateTranslation(worldx-.5f, worldy-.5f, 0);

}

void EnemyTool::OnMouseDown(WPARAM btnstate, float worldx, float worldy){
	int x = floor(worldx);
	int y = floor(worldy);
	//if (btnstate == MK_LBUTTON){
		Enemy *temp;
		if (getTileType() == 0){
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				EnemyManager::Instance()->deleteEnemyAt(worldx, y);
			}
			else
			{
				EnemyManager::Instance()->deleteEnemyAt(worldx, worldy);
			}
		}
		else if (EnemyManager::Instance()->createEnemy((UINT)m_currTile, &temp)){
			//temp->setPosition(Vector2(x, y));
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				temp->setPosition(Vector2(worldx-.5, y));
			}
			else
			{
				temp->setPosition(Vector2(worldx-.5, worldy-.5));
			}
		}
	//}
}

void EnemyTool::OnMouseUp(WPARAM btnstate, float worldx, float worldy){

}

Matrix EnemyTool::getWorld(){
	return m_world;
}

void EnemyTool::setWorld(Matrix m){
	m_world = m;
}
