#pragma once
#include "pch.h"

class Screen {
public:
	virtual void dispose() = 0;
	virtual void hide() = 0;
	virtual void pause() = 0;
	virtual void render(float dt) = 0;
	virtual void resize(int width, int height) = 0;
	virtual void resume() = 0;
	virtual void show() = 0;
};