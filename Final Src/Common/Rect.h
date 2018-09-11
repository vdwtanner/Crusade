#pragma once

#include "pch.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Rect{
public:
	Rect(Vector2 position, Vector2 size);
	Rect(float x, float y, float width, float height);
	Vector2 getPosition();
	Vector2 getSize();
	Vector2 getMax();
	float getX();
	float getXMax();
	float getY();
	float getYMax();
	float getWidth();
	float getHeight();
private:
	Vector2 m_position, m_size;
};