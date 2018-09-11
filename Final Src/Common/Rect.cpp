#include "pch.h"
#include "Rect.h"

Rect::Rect(Vector2 position, Vector2 size){
	this->m_position = position;
	this->m_size = size;
}
Rect::Rect(float x, float y, float width, float height){
	this->m_position = Vector2(x, y);
	this->m_size = Vector2(width, height);
}

//(x, y)
Vector2 Rect::getPosition(){
	return m_position;
}

//(width, height)
Vector2 Rect::getSize(){
	return m_size;
}

//bottom right coordinate of rectangle
Vector2 Rect::getMax(){
	return Vector2(getXMax(), getYMax());
}

//left coordinate of rectangle
float Rect::getX(){
	return m_position.x;
}

//right coordinate of rectangle
float Rect::getXMax(){
	return m_position.x+m_size.x;
}

//top coordinate of rectangle
float Rect::getY(){
	return m_position.y;
}

//Bottom Coordinate of rectangle
float Rect::getYMax(){
	return m_position.y + m_size.y;
}

float Rect::getWidth(){
	return m_size.x;
}

float Rect::getHeight(){
	return m_size.y;
}