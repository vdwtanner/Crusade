#include "pch.h"
#include "BoxTool.h"
#include <math.h>

BoxTool::BoxTool(WorldEditor *editor){
	h_worldEditor = editor;
	m_world = Matrix::Identity;
	v1 = VertexPositionColor(Vector3(0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.5f, 0.5f, .5f));//Position (Vector3), Color (Vector4 - RGBA format) 
	v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(0.0f, 0.5f, 0.5f, .5f));//v2, v3, v4 change to create box
	v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(0.0f, 0.5f, 0.5f, .5f));
	v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(0.0f, 0.5f, 0.5f, .5f));
	start = 0;
	stop = 0;
}

BoxTool::~BoxTool(){
	delete start;
	delete stop;
	start = 0;
	stop = 0;
}

void BoxTool::DrawCursor(PrimitiveBatch<VertexPositionColor> *batch){
	batch->DrawQuad(v1, v2, v3, v4);	
}

void BoxTool::OnMouseMove(WPARAM btnstate, float worldx, float worldy){
	
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
				v2 = VertexPositionColor(Vector3(0, y-start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x-start->x, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(x - start->x, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else if(y<=start->y){
				v1 = VertexPositionColor(Vector3(0, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(x - start->x, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x, y - start->y-1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(0, y - start->y-1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
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
				v2 = VertexPositionColor(Vector3(x - start->x-1, 0, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x-1, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(1, y - start->y, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
			}
			else if (y<=start->y){
				v1 = VertexPositionColor(Vector3(1, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v2 = VertexPositionColor(Vector3(1, y - start->y-1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v3 = VertexPositionColor(Vector3(x - start->x-1, y - start->y-1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
				v4 = VertexPositionColor(Vector3(x - start->x-1, 1, 0), Vector4(0.0f, 1.0f, 0.0f, .5f));
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

void BoxTool::OnMouseDown(WPARAM btnstate, float worldx, float worldy){
	if (start == 0){
		start = new Vector2(floor(worldx), floor(worldy));
	}
	else{
		stop = new Vector2(floor(worldx), floor(worldy));
		//Save tiles()
		Vector2 bottomLeft(fmin(start->x, stop->x), fmin(start->y, stop->y));
		Vector2 topRight(fmax(start->x, stop->x), fmax(start->y, stop->y));

		for (int i = bottomLeft.x; i < topRight.x + 1; i++){
			for (int j = bottomLeft.y; j < topRight.y + 1; j++){
				h_worldEditor->placeTile(m_currTile, i, j);
			}
		}

		delete start;
		delete stop;
		start = 0;
		stop = 0;
		v1 = VertexPositionColor(Vector3(0, 0, 0), Vector4(0.0f, .5f, 0.5f, .5f));
		v2 = VertexPositionColor(Vector3(0.0f, 1.0f, 0.0f), Vector4(0.0f, .5f, 0.5f, .5f));
		v3 = VertexPositionColor(Vector3(1.0f, 1.0f, 0.0f), Vector4(0.0f, .5f, 0.5f, .5f));
		v4 = VertexPositionColor(Vector3(1.0f, 0.0f, 0.0f), Vector4(0.0f, .5f, 0.5f, .5f));
		m_world = Matrix::CreateTranslation(floor(worldx), floor(worldy), 0);


	}
}

void BoxTool::OnMouseUp(WPARAM btnstate, float worldx, float worldy){

}

Matrix BoxTool::getWorld(){
	return m_world;
}

void BoxTool::setWorld(Matrix m){
	m_world = m;
}