#include "pch.h"
#include "LuaTriggerTool.h"
#include "TriggerManager.h"

LuaTriggerTool::LuaTriggerTool(WorldEditor *editor){
	h_worldEditor = editor;
	m_world = Matrix::Identity;
	v1 = VertexPositionColor(Vector3(0.0f, 0.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));//Position (Vector3), Color (Vector4 - RGBA format) 
	v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));//v2, v3, v4 change to create box
	v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));
	v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));
	start = 0;
	stop = 0;
}

LuaTriggerTool::~LuaTriggerTool(){
	delete start;
	delete stop;
	start = 0;
	stop = 0;
}



void LuaTriggerTool::DrawCursor(PrimitiveBatch<VertexPositionColor> *batch){
	batch->DrawQuad(v1, v2, v3, v4);
}

void LuaTriggerTool::OnMouseMove(WPARAM btnstate, float worldx, float worldy){

	if (start == 0){
		int x = floor(worldx);
		m_worldx = x;
		int y = floor(worldy);
		m_worldy = y;
		m_world = Matrix::CreateTranslation(x, y, 0);
	}
	else{
		int x = ceil(worldx);
		int y = ceil(worldy);
		if (x > start->x){
			if (y > start->y){
				v1 = VertexPositionColor(Vector3(0, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(0, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(x - start->x, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else if (y <= start->y){
				v1 = VertexPositionColor(Vector3(0, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(x - start->x, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x, y - start->y - 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(0, y - start->y - 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else{
				v1 = VertexPositionColor(Vector3(0, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
		}
		else{
			if (y > start->y){
				v1 = VertexPositionColor(Vector3(1, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(x - start->x - 1, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x - 1, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(1, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else if (y <= start->y){
				v1 = VertexPositionColor(Vector3(1, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(1, y - start->y - 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x - 1, y - start->y - 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(x - start->x - 1, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else{
				v1 = VertexPositionColor(Vector3(0, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
		}
	}

}

void LuaTriggerTool::OnMouseDown(WPARAM btnstate, float worldx, float worldy){
	if (start == 0){
		start = new Vector2(floor(worldx), floor(worldy));
	}
	else{
		stop = new Vector2(floor(worldx), floor(worldy));
		//Save tiles()
		Vector2 bottomLeft(fmin(start->x, stop->x), fmin(start->y, stop->y));
		Vector2 topRight(fmax(start->x, stop->x)+1, fmax(start->y, stop->y)+1);

		if (m_selectedStr.compare(L"ERASER") == 0)
		{
			TriggerManager::Instance()->deleteTriggerAt(worldx, worldy);
		}
		else
		{
			//Create trigger here.
			LuaTrigger *trig;
			TriggerManager::Instance()->createTrigger(m_selectedStr, bottomLeft, topRight, &trig);
		}

		delete start;
		delete stop;
		start = 0;
		stop = 0;
		v1 = VertexPositionColor(Vector3(0, 0, 0), Vector4(1.0f, 1.0f, 0.0f, .5f));
		v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));
		v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));
		v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(1.0f, 1.0f, 0.0f, .5f));
		m_world = Matrix::CreateTranslation(floor(worldx), floor(worldy), 0);


	}
}

void LuaTriggerTool::OnMouseUp(WPARAM btnstate, float worldx, float worldy){

}

Matrix LuaTriggerTool::getWorld(){
	return m_world;
}

void LuaTriggerTool::setWorld(Matrix m){
	m_world = m;
}