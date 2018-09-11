#pragma once

#include "pch.h"
#include "Game.h"
#include "DrawTool.h"
#include <Windows.h>
#include <Commdlg.h>

class InputProcessor{
public:
	InputProcessor(Game *game);
	~InputProcessor();

	LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK NewDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(WPARAM btnstate, int x, int y);
	void OnMouseDown(WPARAM btnstate, int x, int y);
	void OnMouseUp(WPARAM btnstate, int x, int y);
	void OnClick(WPARAM btnstate, int x, int y);
	void OnDoubleClick(WPARAM btnstate, int x, int y);
	void updateTool();
	void giveHandles(HWND hwnd, HWND dx_hwnd, HWND lb_hwnd, HWND tb_hwnd);

	void Update();	//check async key states for key combos

	Vector2 getMousePos();

private:
	Game *h_game;
	WorldEditor *h_worldEditor;
	Tool *h_tool;
	WCHAR m_fileName[200];

	std::wstring m_savePath;

	
	bool mousedown;

	HWND h_hwnd;
	HWND h_dx_hwnd;
	HWND h_lb_hwnd;
	HWND h_tb_hwnd;
	Vector2 m_mousePos;
};