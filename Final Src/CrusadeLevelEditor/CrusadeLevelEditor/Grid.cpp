#include "pch.h"
#include "Grid.h"

Grid::Grid(){
	//vertArr = std::vector<VertexPositionColorTexture>();
	m_width = 128;
	m_height = 32;
	buildGridVerts();
}

Grid::Grid(float w, float h){
	m_width = w;
	m_height = h;
	buildGridVerts();
}

Grid::~Grid(){

}

void Grid::Update(float dt){
	
}

void Grid::Draw(PrimitiveBatch<VertexPositionColor> *batch){
	for (size_t i = 0; i < vertArr.size(); i += 2){
		batch->DrawLine(vertArr.at(i), vertArr.at(i+1));
	}
}

void Grid::buildGridVerts(){
	if (vertArr.size() > 0)
		vertArr.clear();

 	/*int viewportWidth = Game::m_camera->getViewportWidth();
	int viewportHeight = Game::m_camera->getViewportHeight();
	float halfWidth = viewportWidth / 2.f;
	float halfHeight = viewportHeight / 2.f;
	
	// vertical lines
	for (float i = -halfWidth; i < halfWidth; i++){
		vertArr.push_back(VertexPositionColor(Vector3(i, halfHeight, 0), Colors::Black));
		vertArr.push_back(VertexPositionColor(Vector3(i, -halfHeight, 0), Colors::Black));
	}
	// horizontal lines
	for (float i = -halfHeight; i < halfHeight; i++){
		vertArr.push_back(VertexPositionColor(Vector3(halfWidth, i, 0), Colors::Black));
		vertArr.push_back(VertexPositionColor(Vector3(-halfWidth, i, 0), Colors::Black));
	}*/

	// vertical lines
	for (float i = 0; i < m_width+1; i++){
		vertArr.push_back(VertexPositionColor(Vector3(i, 0, 1), Colors::Black));
		vertArr.push_back(VertexPositionColor(Vector3(i, m_height, 1), Colors::Black));
	}
	// horizontal lines
	for (float i = 0; i < m_height+1; i++){
		vertArr.push_back(VertexPositionColor(Vector3(0, i, 1), Colors::Black));
		vertArr.push_back(VertexPositionColor(Vector3(m_width, i, 1), Colors::Black));
	}

}

Matrix Grid::getWorld(){
	return m_world;
}

void Grid::setWorld(Matrix m){
	m_world = m;
}

void Grid::Resize(){
	buildGridVerts();
}

int Grid::getWidth(){
	return m_width;
}

int Grid::getHeight(){
	return m_height;
}